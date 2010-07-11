/*  This file is part of the KDE project
    Copyright (C) 2009 Colin Guthrie <cguthrie@mandriva.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#ifdef HAVE_PULSEAUDIO
#include "pulsestream_p.h"
#include <glib.h>
#include <pulse/pulseaudio.h>
#include <pulse/xmalloc.h>
#include <pulse/glib-mainloop.h>
#ifdef HAVE_PULSEAUDIO_DEVICE_MANAGER
#  include <pulse/ext-device-manager.h>
#endif
#endif // HAVE_PULSEAUDIO

#include "pulsesupport.h"

QT_BEGIN_NAMESPACE

namespace Phonon
{

static PulseSupport* s_instance = NULL;

#ifdef HAVE_PULSEAUDIO
/***
* Prints a conditional debug message based on the current debug level
* If obj is provided, classname and objectname will be printed as well
*
* see debugLevel()
*/

static int debugLevel() {
    static int level = -1;
    if (level < 1) {
        level = 0;
        QString pulseenv = qgetenv("PHONON_PULSEAUDIO_DEBUG");
        int l = pulseenv.toInt();
        if (l > 0)
            level = (l > 2 ? 2 : l);
    }
    return level;
}

static void logMessage(const QString &message, int priority = 2, QObject *obj=0);
static void logMessage(const QString &message, int priority, QObject *obj)
{
    if (debugLevel() > 0) {
        QString output;
        if (obj) {
            // Strip away namespace from className
            QString className(obj->metaObject()->className());
            int nameLength = className.length() - className.lastIndexOf(':') - 1;
            className = className.right(nameLength);
            output.sprintf("%s %s (%s %p)", message.toLatin1().constData(), 
                           obj->objectName().toLatin1().constData(), 
                           className.toLatin1().constData(), obj);
        }
        else {
            output = message;
        }
        if (priority <= debugLevel()) {
            qDebug() << QString("PulseSupport(%1): %2").arg(priority).arg(output);
        }
    }
}


class AudioDevice
{
    public:
        inline
        AudioDevice(QString name, QString desc, QString icon, uint32_t index)
        : pulseName(name), pulseIndex(index)
        {
            properties["name"] = desc;
            properties["description"] = ""; // We don't have descriptions (well we do, but we use them as the name!)
            properties["icon"] = icon;
            properties["available"] = (index != PA_INVALID_INDEX);
            properties["isAdvanced"] = false; // Nothing is advanced!
        }

        // Needed for QMap
        inline AudioDevice() {}

        QString pulseName;
        uint32_t pulseIndex;
        QHash<QByteArray, QVariant> properties;
};
bool operator!=(const AudioDevice &a, const AudioDevice &b)
{
    return !(a.pulseName == b.pulseName && a.properties == b.properties);
}

class PulseUserData
{
    public:
        inline 
        PulseUserData()
        {
        }

        QMap<QString, AudioDevice> newOutputDevices;
        QMap<Phonon::Category, QMap<int, int> > newOutputDevicePriorities; // prio, device

        QMap<QString, AudioDevice> newCaptureDevices;
        QMap<Phonon::Category, QMap<int, int> > newCaptureDevicePriorities; // prio, device
};

static QMap<QString, Phonon::Category> s_roleCategoryMap;

static bool s_pulseActive = false;

static pa_glib_mainloop *s_mainloop = NULL;
static pa_context *s_context = NULL;



static int s_deviceIndexCounter = 0;

static QMap<QString, int> s_outputDeviceIndexes;
static QMap<int, AudioDevice> s_outputDevices;
static QMap<Phonon::Category, QMap<int, int> > s_outputDevicePriorities; // prio, device
static QMap<QString, PulseStream*> s_outputStreams;

static QMap<QString, int> s_captureDeviceIndexes;
static QMap<int, AudioDevice> s_captureDevices;
static QMap<Phonon::Category, QMap<int, int> > s_captureDevicePriorities; // prio, device
static QMap<QString, PulseStream*> s_captureStreams;

static PulseStream* findStreamByPulseIndex(QMap<QString, PulseStream*> map, uint32_t index)
{
  QMap<QString, PulseStream*>::iterator it;
  for (it = map.begin(); it != map.end(); ++it)
    if ((*it)->index() == index)
      return *it;
  return NULL;
}

static void createGenericDevices()
{
    // OK so we don't have the device manager extension, but we can show a single device and fake it.
    int index;
    s_outputDeviceIndexes.clear();
    s_outputDevices.clear();
    s_outputDevicePriorities.clear();
    index = s_deviceIndexCounter++;
    s_outputDeviceIndexes.insert("sink:default", index);
    s_outputDevices.insert(index, AudioDevice("sink:default", QObject::tr("PulseAudio Sound Server"), "audio-backend-pulseaudio", 0));
    for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
        Phonon::Category cat = static_cast<Phonon::Category>(i);
        s_outputDevicePriorities[cat].insert(0, index);
    }

    s_captureDeviceIndexes.clear();
    s_captureDevices.clear();
    s_captureDevicePriorities.clear();
    index = s_deviceIndexCounter++;
    s_captureDeviceIndexes.insert("source:default", index);
    s_captureDevices.insert(index, AudioDevice("source:default", QObject::tr("PulseAudio Sound Server"), "audio-backend-pulseaudio", 0));
    for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
        Phonon::Category cat = static_cast<Phonon::Category>(i);
        s_captureDevicePriorities[cat].insert(0, index);
    }
}

#ifdef HAVE_PULSEAUDIO_DEVICE_MANAGER
static void ext_device_manager_read_cb(pa_context *c, const pa_ext_device_manager_info *info, int eol, void *userdata) {
    Q_ASSERT(c);
    Q_ASSERT(userdata);

    PulseUserData *u = reinterpret_cast<PulseUserData*>(userdata);

    if (eol < 0) {
        logMessage(QString("Failed to initialize device manager extension: %1").arg(pa_strerror(pa_context_errno(c))));
        if (s_context != c) {
            logMessage("Falling back to single device mode");
            // Only create our gerneric devices during the probe phase.
            createGenericDevices();
            // As this is our probe phase, exit immediately
            pa_context_disconnect(c);
        }
        delete u;

        return;
    }

    if (eol) {
        // We're done reading the data, so order it by priority and copy it into the
        // static variables where it can then be accessed by those classes that need it.

        QMap<QString, AudioDevice>::iterator newdev_it;

        // Check for new output devices or things changing about known output devices.
        bool output_changed = false;
        for (newdev_it = u->newOutputDevices.begin(); newdev_it != u->newOutputDevices.end(); ++newdev_it) {
            QString name = newdev_it.key();

            // The name + index map is always written when a new device is added.
            Q_ASSERT(s_outputDeviceIndexes.contains(name));

            int index = s_outputDeviceIndexes[name];
            if (!s_outputDevices.contains(index)) {
                // This is a totally new device
                output_changed = true;
                logMessage(QString("Brand New Output Device Found."));
                s_outputDevices.insert(index, *newdev_it);
            } else  if (s_outputDevices[index] != *newdev_it) {
                // We have this device already, but is it different?
                output_changed = true;
                logMessage(QString("Change to Existing Output Device (may be Added/Removed or something else)"));
                s_outputDevices.remove(index);
                s_outputDevices.insert(index, *newdev_it);
            }
        }
        // Go through the output devices we know about and see if any are no longer mentioned in the list.
        QMutableMapIterator<QString, int> output_existing_it(s_outputDeviceIndexes);
        while (output_existing_it.hasNext()) {
            output_existing_it.next();
            if (!u->newOutputDevices.contains(output_existing_it.key())) {
                output_changed = true;
                logMessage(QString("Output Device Completely Removed"));
                s_outputDevices.remove(output_existing_it.value());
                output_existing_it.remove();
            }
        }

        // Check for new capture devices or things changing about known capture devices.
        bool capture_changed = false;
        for (newdev_it = u->newCaptureDevices.begin(); newdev_it != u->newCaptureDevices.end(); ++newdev_it) {
            QString name = newdev_it.key();

            // The name + index map is always written when a new device is added.
            Q_ASSERT(s_captureDeviceIndexes.contains(name));

            int index = s_captureDeviceIndexes[name];
            if (!s_captureDevices.contains(index)) {
                // This is a totally new device
                capture_changed = true;
                logMessage(QString("Brand New Capture Device Found."));
                s_captureDevices.insert(index, *newdev_it);
            } else  if (s_captureDevices[index] != *newdev_it) {
                // We have this device already, but is it different?
                capture_changed = true;
                logMessage(QString("Change to Existing Capture Device (may be Added/Removed or something else)"));
                s_captureDevices.remove(index);
                s_captureDevices.insert(index, *newdev_it);
            }
        }
        // Go through the capture devices we know about and see if any are no longer mentioned in the list.
        QMutableMapIterator<QString, int> capture_existing_it(s_captureDeviceIndexes);
        while (capture_existing_it.hasNext()) {
            capture_existing_it.next();
            if (!u->newCaptureDevices.contains(capture_existing_it.key())) {
                capture_changed = true;
                logMessage(QString("Capture Device Completely Removed"));
                s_captureDevices.remove(capture_existing_it.value());
                capture_existing_it.remove();
            }
        }

        // Just copy accross the new priority lists as we know they are valid
        if (s_outputDevicePriorities != u->newOutputDevicePriorities) {
            output_changed = true;
            s_outputDevicePriorities = u->newOutputDevicePriorities;
        }
        if (s_captureDevicePriorities != u->newCaptureDevicePriorities) {
            capture_changed = true;
            s_captureDevicePriorities = u->newCaptureDevicePriorities;
        }

        if (s_instance) {
            // This wont be emitted durring the connection probe phase
            // which is intensional
            if (output_changed)
                s_instance->emitObjectDescriptionChanged(AudioOutputDeviceType);
            if (capture_changed)
                s_instance->emitObjectDescriptionChanged(AudioCaptureDeviceType);
        }

        // We can free the user data as we will not be called again.
        delete u;

        // Some debug
        logMessage(QString("Output Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_outputDevicePriorities.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                int count = 0;
                foreach (int j, s_outputDevicePriorities[cat]) {
                    QHash<QByteArray, QVariant> &props = s_outputDevices[j].properties;
                    logMessage(QString("    %1. %2 (Available: %3)").arg(++count).arg(props["name"].toString()).arg(props["available"].toBool()));
                }
            }
        }
        logMessage(QString("Capture Device Priority List:"));
        for (int i = Phonon::NoCategory; i <= Phonon::LastCategory; ++i) {
            Phonon::Category cat = static_cast<Phonon::Category>(i);
            if (s_captureDevicePriorities.contains(cat)) {
                logMessage(QString("  Phonon Category %1").arg(cat));
                int count = 0;
                foreach (int j, s_captureDevicePriorities[cat]) {
                    QHash<QByteArray, QVariant> &props = s_captureDevices[j].properties;
                    logMessage(QString("    %1. %2 (Available: %3)").arg(++count).arg(props["name"].toString()).arg(props["available"].toBool()));
                }
            }
        }

        // If this is our probe phase, exit now as we're finished reading
        // our device info and can exit and reconnect
        if (s_context != c)
            pa_context_disconnect(c);
    }

    if (!info)
        return;

    Q_ASSERT(info->name);
    Q_ASSERT(info->description);
    Q_ASSERT(info->icon);

    // QString wrapper
    QString name(info->name);
    int index;
    QMap<Phonon::Category, QMap<int, int> > *new_prio_map_cats; // prio, device
    QMap<QString, AudioDevice> *new_devices;

    if (name.startsWith("sink:")) {
        new_devices = &u->newOutputDevices;
        new_prio_map_cats = &u->newOutputDevicePriorities;

        if (s_outputDeviceIndexes.contains(name))
            index = s_outputDeviceIndexes[name];
        else
            index = s_outputDeviceIndexes[name] = s_deviceIndexCounter++;
    } else if (name.startsWith("source:")) {
        new_devices = &u->newCaptureDevices;
        new_prio_map_cats = &u->newCaptureDevicePriorities;

        if (s_captureDeviceIndexes.contains(name))
            index = s_captureDeviceIndexes[name];
        else
            index = s_captureDeviceIndexes[name] = s_deviceIndexCounter++;
    } else {
        // This indicates a bug in pulseaudio.
        return;
    }

    // Add the new device itself.
    new_devices->insert(name, AudioDevice(name, QString::fromUtf8(info->description), QString::fromUtf8(info->icon), info->index));

    // For each role in the priority, map it to a phonon category and store the order.
    for (uint32_t i = 0; i < info->n_role_priorities; ++i) {
        pa_ext_device_manager_role_priority_info* role_prio = &info->role_priorities[i];
        Q_ASSERT(role_prio->role);

        if (s_roleCategoryMap.contains(role_prio->role)) {
            Phonon::Category cat = s_roleCategoryMap[role_prio->role];

            (*new_prio_map_cats)[cat].insert(role_prio->priority, index);
        }
    }
}

static void ext_device_manager_subscribe_cb(pa_context *c, void *) {
    Q_ASSERT(c);

    pa_operation *o;
    PulseUserData *u = new PulseUserData;
    if (!(o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, u))) {
        logMessage(QString("pa_ext_device_manager_read() failed."));
        delete u;
        return;
    }
    pa_operation_unref(o);
}
#endif

static void sink_input_cb(pa_context *c, const pa_sink_input_info *i, int eol, void *userdata) {
    Q_UNUSED(userdata);
    Q_ASSERT(c);

    if (eol < 0) {
        if (pa_context_errno(c) == PA_ERR_NOENTITY)
            return;

        logMessage(QString("Sink input callback failure"));
        return;
    }

    if (eol > 0)
        return;

    Q_ASSERT(i);

    // loop through (*i) and extract phonon->streamindex...
    const char *t;
    if ((t = pa_proplist_gets(i->proplist, "phonon.streamid"))) {
        logMessage(QString("Found PulseAudio stream index %1 for Phonon Output Stream %2").arg(i->index).arg(t));

        // We only care about our own streams (other phonon processes are irrelevent)
        if (s_outputStreams.contains(QString(t))) {
            PulseStream *stream = s_outputStreams[QString(t)];
            stream->setIndex(i->index);
            stream->setVolume(&i->volume);
            stream->setMute(!!i->mute);

            // Find the sink's phonon index and notify whoever cares...
            if (PA_INVALID_INDEX != i->sink) {
                QMap<int, AudioDevice>::iterator it;
                for (it = s_outputDevices.begin(); it != s_outputDevices.end(); ++it) {
                    if ((*it).pulseIndex == i->sink) {
                        stream->setDevice(it.key());
                        break;
                    }
                }
            }
        }
    }
}

static void source_output_cb(pa_context *c, const pa_source_output_info *i, int eol, void *userdata) {
    Q_UNUSED(userdata);
    Q_ASSERT(c);

    if (eol < 0) {
        if (pa_context_errno(c) == PA_ERR_NOENTITY)
            return;

        logMessage(QString("Source output callback failure"));
        return;
    }

    if (eol > 0)
        return;

    Q_ASSERT(i);

    // loop through (*i) and extract phonon->streamindex...
    const char *t;
    if ((t = pa_proplist_gets(i->proplist, "phonon.streamid"))) {
        logMessage(QString("Found PulseAudio stream index %1 for Phonon Capture Stream %2").arg(i->index).arg(t));

        // We only care about our own streams (other phonon processes are irrelevent)
        if (s_captureStreams.contains(QString(t))) {
            PulseStream *stream = s_captureStreams[QString(t)];
            stream->setIndex(i->index);
            //stream->setVolume(&i->volume);
            //stream->setMute(!!i->mute);

            // Find the source's phonon index and notify whoever cares...
            if (PA_INVALID_INDEX != i->source) {
                QMap<int, AudioDevice>::iterator it;
                for (it = s_captureDevices.begin(); it != s_captureDevices.end(); ++it) {
                    if ((*it).pulseIndex == i->source) {
                        stream->setDevice(it.key());
                        break;
                    }
                }
            }
        }
    }
}

static void subscribe_cb(pa_context *c, pa_subscription_event_type_t t, uint32_t index, void *userdata) {
    Q_UNUSED(userdata);

    switch (t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) {
        case PA_SUBSCRIPTION_EVENT_SINK_INPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
                PulseStream *stream = findStreamByPulseIndex(s_outputStreams, index);
                if (stream) {
                    logMessage(QString("Phonon Output Stream %1 is gone at the PA end. Marking it as invalid in our cache as we may reuse it.").arg(stream->uuid()));
                    stream->setIndex(PA_INVALID_INDEX);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_sink_input_info(c, index, sink_input_cb, NULL))) {
                    logMessage(QString("pa_context_get_sink_input_info() failed"));
                    return;
                }
                pa_operation_unref(o);
            }
            break;

        case PA_SUBSCRIPTION_EVENT_SOURCE_OUTPUT:
            if ((t & PA_SUBSCRIPTION_EVENT_TYPE_MASK) == PA_SUBSCRIPTION_EVENT_REMOVE) {
              PulseStream *stream = findStreamByPulseIndex(s_captureStreams, index);
              if (stream) {
                    logMessage(QString("Phonon Capture Stream %1 is gone at the PA end. Marking it as invalid in our cache as we may reuse it.").arg(stream->uuid()));
                    stream->setIndex(PA_INVALID_INDEX);
                }
            } else {
                pa_operation *o;
                if (!(o = pa_context_get_source_output_info(c, index, source_output_cb, NULL))) {
                    logMessage(QString("pa_context_get_sink_input_info() failed"));
                    return;
                }
                pa_operation_unref(o);
            }
            break;
    }
}


static const char* statename(pa_context_state_t state)
{
    switch (state)
    {
        case PA_CONTEXT_UNCONNECTED:  return "Unconnected";
        case PA_CONTEXT_CONNECTING:   return "Connecting";
        case PA_CONTEXT_AUTHORIZING:  return "Authorizing";
        case PA_CONTEXT_SETTING_NAME: return "Setting Name";
        case PA_CONTEXT_READY:        return "Ready";
        case PA_CONTEXT_FAILED:       return "Failed";
        case PA_CONTEXT_TERMINATED:   return "Terminated";
    }

    static QString unknown;
    unknown = QString("Unknown state: %0").arg(state);
    return unknown.toAscii().constData();
}

static void context_state_callback(pa_context *c, void *)
{
    Q_ASSERT(c);

    logMessage(QString("context_state_callback %1").arg(statename(pa_context_get_state(c))));
    pa_context_state_t state = pa_context_get_state(c);
    if (state == PA_CONTEXT_READY) {
        // We've connected to PA, so it is active
        s_pulseActive = true;

        // Attempt to load things up
        pa_operation *o;

        // 1. Register for the stream changes (except during probe)
        if (s_context == c) {
            pa_context_set_subscribe_callback(c, subscribe_cb, NULL);

            if (!(o = pa_context_subscribe(c, (pa_subscription_mask_t)
                                              (PA_SUBSCRIPTION_MASK_SINK_INPUT|
                                               PA_SUBSCRIPTION_MASK_SOURCE_OUTPUT), NULL, NULL))) {
                logMessage(QString("pa_context_subscribe() failed"));
                return;
            }
            pa_operation_unref(o);

            // In the case of reconnection or simply lagging behind the stream object creation
            // on startup (due to the probe+reconnect system), we invalidate all loaded streams
            // and then load up info about all streams.
            for (QMap<QString, PulseStream*>::iterator it = s_outputStreams.begin(); it != s_outputStreams.end(); ++it) {
              PulseStream *stream = *it;
              logMessage(QString("Phonon Output Stream %1 is gone at the PA end. Marking it as invalid in our cache as we may reuse it.").arg(stream->uuid()));
              stream->setIndex(PA_INVALID_INDEX);
            }
            if (!(o = pa_context_get_sink_input_info_list(c, sink_input_cb, NULL))) {
              logMessage(QString("pa_context_get_sink_input_info_list() failed"));
              return;
            }
            pa_operation_unref(o);

            for (QMap<QString, PulseStream*>::iterator it = s_captureStreams.begin(); it != s_captureStreams.end(); ++it) {
              PulseStream *stream = *it;
              logMessage(QString("Phonon Capture Stream %1 is gone at the PA end. Marking it as invalid in our cache as we may reuse it.").arg(stream->uuid()));
              stream->setIndex(PA_INVALID_INDEX);
            }
            if (!(o = pa_context_get_source_output_info_list(c, source_output_cb, NULL))) {
              logMessage(QString("pa_context_get_source_output_info_list() failed"));
              return;
            }
            pa_operation_unref(o);
        }

#ifdef HAVE_PULSEAUDIO_DEVICE_MANAGER
        // 2a. Attempt to initialise Device Manager info (except during probe)
        if (s_context == c) {
            pa_ext_device_manager_set_subscribe_cb(c, ext_device_manager_subscribe_cb, NULL);
            if (!(o = pa_ext_device_manager_subscribe(c, 1, NULL, NULL))) {
                logMessage(QString("pa_ext_device_manager_subscribe() failed"));
                return;
            }
            pa_operation_unref(o);
        }

        // 3. Attempt to read info from Device Manager
        PulseUserData *u = new PulseUserData;
        if (!(o = pa_ext_device_manager_read(c, ext_device_manager_read_cb, u))) {
            if (s_context != c) {
                logMessage(QString("pa_ext_device_manager_read() failed. Attempting to continue without device manager support"));
                // Only create our gerneric devices during the probe phase.
                createGenericDevices();
                // As this is our probe phase, exit immediately
                pa_context_disconnect(c);
            }
            delete u;

            return;
        }
        pa_operation_unref(o);

#else
        // If we know do not have Device Manager support, we just create our dummy devices now
        if (s_context != c) {
            // Only create our gerneric devices during the probe phase.
            createGenericDevices();
            // As this is our probe phase, exit immediately
            pa_context_disconnect(c);
        }

#endif
    } else if (!PA_CONTEXT_IS_GOOD(state)) {
        /// @todo Deal with reconnection...
        //logMessage(QString("Connection to PulseAudio lost: %1").arg(pa_strerror(pa_context_errno(c))));

        // If this is our probe phase, exit our context immediately
        if (s_context != c)
            pa_context_disconnect(c);
        else {
            pa_context_unref(s_context);
            s_context = NULL;
            QTimer::singleShot(50, PulseSupport::getInstance(), SLOT(connectToDaemon()));
        }
    }
}
#endif // HAVE_PULSEAUDIO


PulseSupport* PulseSupport::getInstance()
{
    if (NULL == s_instance) {
        s_instance = new PulseSupport();
    }
    return s_instance;
}

void PulseSupport::shutdown()
{
    if (NULL != s_instance) {
        delete s_instance;
        s_instance = NULL;
    }
}

void PulseSupport::debug()
{
#ifdef HAVE_PULSEAUDIO
    logMessage(QString("Have we been initialised yet? %1").arg(s_instance ? "Yes" : "No"));
    if (s_instance) {
        logMessage(QString("Connected to PulseAudio? %1").arg(s_pulseActive ? "Yes" : "No"));
        logMessage(QString("PulseAudio support 'Active'? %1").arg(s_instance->isActive() ? "Yes" : "No"));
    }
#endif
}

PulseSupport::PulseSupport()
 : QObject(), mEnabled(false)
{
#ifdef HAVE_PULSEAUDIO
    // Initialise our map (is there a better way to do this?)
    s_roleCategoryMap["none"] = Phonon::NoCategory;
    s_roleCategoryMap["video"] = Phonon::VideoCategory;
    s_roleCategoryMap["music"] = Phonon::MusicCategory;
    s_roleCategoryMap["game"] = Phonon::GameCategory;
    s_roleCategoryMap["event"] = Phonon::NotificationCategory;
    s_roleCategoryMap["phone"] = Phonon::CommunicationCategory;
    //s_roleCategoryMap["animation"]; // No Mapping
    //s_roleCategoryMap["production"]; // No Mapping
    s_roleCategoryMap["a11y"] = Phonon::AccessibilityCategory;

    // To allow for easy debugging, give an easy way to disable this pulseaudio check
    QString pulseenv = qgetenv("PHONON_PULSEAUDIO_DISABLE");
    if (pulseenv.toInt()) {
        logMessage("PulseAudio support disabled: PHONON_PULSEAUDIO_DISABLE is set");
        return;
    }

    // We require a glib event loop
    if (QLatin1String(QAbstractEventDispatcher::instance()->metaObject()->className())
            != "QGuiEventDispatcherGlib") {
        logMessage("Disabling PulseAudio integration for lack of GLib event loop.");
        return;
    }

    // First of all conenct to PA via simple/blocking means and if that succeeds,
    // use a fully async integrated mainloop method to connect and get proper support.
    pa_mainloop *p_test_mainloop;
    if (!(p_test_mainloop = pa_mainloop_new())) {
        logMessage("PulseAudio support disabled: Unable to create mainloop");
        return;
    }

    pa_context *p_test_context;
    if (!(p_test_context = pa_context_new(pa_mainloop_get_api(p_test_mainloop), "libphonon-probe"))) {
        logMessage("PulseAudio support disabled: Unable to create context");
        pa_mainloop_free(p_test_mainloop);
        return;
    }

    logMessage("Probing for PulseAudio...");
    // (cg) Convert to PA_CONTEXT_NOFLAGS when PulseAudio 0.9.19 is required
    if (pa_context_connect(p_test_context, NULL, static_cast<pa_context_flags_t>(0), NULL) < 0) {
        logMessage(QString("PulseAudio support disabled: %1").arg(pa_strerror(pa_context_errno(p_test_context))));
        pa_context_disconnect(p_test_context);
        pa_context_unref(p_test_context);
        pa_mainloop_free(p_test_mainloop);
        return;
    }

    pa_context_set_state_callback(p_test_context, &context_state_callback, NULL);
    for (;;) {
        pa_mainloop_iterate(p_test_mainloop, 1, NULL);

        if (!PA_CONTEXT_IS_GOOD(pa_context_get_state(p_test_context))) {
            logMessage("PulseAudio probe complete.");
            break;
        }
    }
    pa_context_disconnect(p_test_context);
    pa_context_unref(p_test_context);
    pa_mainloop_free(p_test_mainloop);

    if (!s_pulseActive) {
        logMessage("PulseAudio support is not available.");
        return;
    }

    // If we're still here, PA is available.
    logMessage("PulseAudio support enabled");

    // Now we connect for real using a proper main loop that we can forget
    // all about processing.
    s_mainloop = pa_glib_mainloop_new(NULL);
    Q_ASSERT(s_mainloop);

    connectToDaemon();
#endif
}

PulseSupport::~PulseSupport()
{
#ifdef HAVE_PULSEAUDIO
    if (s_context) {
        pa_context_disconnect(s_context);
        s_context = NULL;
    }

    if (s_mainloop) {
        pa_glib_mainloop_free(s_mainloop);
        s_mainloop = NULL;
    }
#endif
}


void PulseSupport::connectToDaemon()
{
#ifdef HAVE_PULSEAUDIO
    pa_mainloop_api *api = pa_glib_mainloop_get_api(s_mainloop);

    s_context = pa_context_new(api, "libphonon");
    if (pa_context_connect(s_context, NULL, PA_CONTEXT_NOFAIL, 0) >= 0)
        pa_context_set_state_callback(s_context, &context_state_callback, NULL);
#endif
}

bool PulseSupport::isActive()
{
#ifdef HAVE_PULSEAUDIO
    return mEnabled && s_pulseActive;
#else
    return false;
#endif
}

void PulseSupport::enable(bool enabled)
{
    mEnabled = enabled;
#ifdef HAVE_PULSEAUDIO
    logMessage(QString("Enabled Breakdown: mEnabled: %1, s_pulseActive %2").arg(mEnabled ? "Yes" : "No" ).arg(s_pulseActive ? "Yes" : "No"));
#endif
}

QList<int> PulseSupport::objectDescriptionIndexes(ObjectDescriptionType type) const
{
    QList<int> list;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return list;

#ifdef HAVE_PULSEAUDIO
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType: {
                QMap<QString, int>::iterator it;
                for (it = s_outputDeviceIndexes.begin(); it != s_outputDeviceIndexes.end(); ++it) {
                    list.append(*it);
                }
                break;
            }
            case AudioCaptureDeviceType: {
                QMap<QString, int>::iterator it;
                for (it = s_captureDeviceIndexes.begin(); it != s_captureDeviceIndexes.end(); ++it) {
                    list.append(*it);
                }
                break;
            }
            default:
                break;
        }
    }
#endif

    return list;
}

QHash<QByteArray, QVariant> PulseSupport::objectDescriptionProperties(ObjectDescriptionType type, int index) const
{
    QHash<QByteArray, QVariant> ret;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return ret;

#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(index);
#else
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType:
                Q_ASSERT(s_outputDevices.contains(index));
                ret = s_outputDevices[index].properties;
                break;

            case AudioCaptureDeviceType:
                Q_ASSERT(s_captureDevices.contains(index));
                ret = s_captureDevices[index].properties;
                break;

            default:
                break;
        }
    }
#endif

    return ret;
}

QList<int> PulseSupport::objectIndexesByCategory(ObjectDescriptionType type, Category category) const
{
    QList<int> ret;

    if (type != AudioOutputDeviceType && type != AudioCaptureDeviceType)
        return ret;

#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
#else
    if (s_pulseActive) {
        switch (type) {

            case AudioOutputDeviceType:
                if (s_outputDevicePriorities.contains(category))
                    ret = s_outputDevicePriorities[category].values();
                break;

            case AudioCaptureDeviceType:
                if (s_captureDevicePriorities.contains(category))
                    ret = s_captureDevicePriorities[category].values();
                break;

            default:
                break;
        }
    }
#endif

    return ret;
}

#ifdef HAVE_PULSEAUDIO
static void setDevicePriority(Category category, QStringList list)
{
    QString role = s_roleCategoryMap.key(category);
    if (role.isEmpty())
        return;

    logMessage(QString("Reindexing %1: %2").arg(role).arg(list.join(", ")));

    char **devices;
    devices = pa_xnew(char *, list.size()+1);
    int i = 0;
    foreach (QString str, list) {
        devices[i++] = pa_xstrdup(str.toUtf8().constData());
    }
    devices[list.size()] = NULL;

#ifdef HAVE_PULSEAUDIO_DEVICE_MANAGER 
    pa_operation *o;
    if (!(o = pa_ext_device_manager_reorder_devices_for_role(s_context, role.toUtf8().constData(), (const char**)devices, NULL, NULL)))
        logMessage(QString("pa_ext_device_manager_reorder_devices_for_role() failed"));
    else
        pa_operation_unref(o);
#endif

    for (i = 0; i < list.size(); ++i)
        pa_xfree(devices[i]);
    pa_xfree(devices);
}
#endif

void PulseSupport::setOutputDevicePriorityForCategory(Category category, QList<int> order)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
    Q_UNUSED(order);
#else
    QStringList list;
    QList<int>::iterator it;

    for (it = order.begin(); it != order.end(); ++it) {
        if (s_outputDevices.contains(*it)) {
            list << s_outputDeviceIndexes.key(*it);
        }
    }
    setDevicePriority(category, list);
#endif
}

void PulseSupport::setCaptureDevicePriorityForCategory(Category category, QList<int> order)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(category);
    Q_UNUSED(order);
#else
    QStringList list;
    QList<int>::iterator it;

    for (it = order.begin(); it != order.end(); ++it) {
        if (s_captureDevices.contains(*it)) {
            list << s_captureDeviceIndexes.key(*it);
        }
    }
    setDevicePriority(category, list);
#endif
}

#ifdef HAVE_PULSEAUDIO
static PulseStream* register_stream(QMap<QString,PulseStream*> &map, QString streamUuid, Category category)
{
    logMessage(QString("Initialising streamindex %1").arg(streamUuid));
    QString role = s_roleCategoryMap.key(category);
    if (!role.isEmpty()) {
        logMessage(QString("Setting role to %1 for streamindex %2").arg(role).arg(streamUuid));
        setenv("PULSE_PROP_media.role", role.toLatin1().constData(), 1);
    }
    setenv("PULSE_PROP_phonon.streamid", streamUuid.toLatin1().constData(), 1);
    PulseStream *stream = new PulseStream(streamUuid);
    map[streamUuid] = stream;
    return stream;
}
#endif

PulseStream *PulseSupport::registerOutputStream(QString streamUuid, Category category)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(category);
    return NULL;
#else
    return register_stream(s_outputStreams, streamUuid, category);
#endif
}

PulseStream *PulseSupport::registerCaptureStream(QString streamUuid, Category category)
{
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(category);
    return NULL;
#else
    return register_stream(s_captureStreams, streamUuid, category);
#endif
}

void PulseSupport::emitObjectDescriptionChanged(ObjectDescriptionType type)
{
    if (mEnabled)
        emit objectDescriptionChanged(type);
}

bool PulseSupport::setOutputName(QString streamUuid, QString name) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(name);
    return false;
#else
    logMessage(QString("Unimplemented: Need to find a way to set either application.name or media.name in SI proplist"));
    Q_UNUSED(streamUuid);
    Q_UNUSED(name);
    return true;
#endif
}

bool PulseSupport::setOutputDevice(QString streamUuid, int device) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(device);
    return false;
#else
    if (s_outputDevices.size() < 2)
        return true;

    if (!s_outputDevices.contains(device)) {
        logMessage(QString("Attempting to set Output Device for invalid device id %1.").arg(device));
        return false;
    }
    const QVariant var = s_outputDevices[device].properties["name"];
    logMessage(QString("Attempting to set Output Device to '%1' for Output Stream %2").arg(var.toString()).arg(streamUuid));

    // Attempt to look up the pulse stream index.
    if (s_outputStreams.contains(streamUuid) && s_outputStreams[streamUuid]->index() != PA_INVALID_INDEX) {
        logMessage(QString("... Found in map. Moving now"));

        uint32_t pulse_device_index = s_outputDevices[device].pulseIndex;
        uint32_t pulse_stream_index = s_outputStreams[streamUuid]->index();

        logMessage(QString("Moving Pulse Sink Input %1 to '%2' (Pulse Sink %3)").arg(pulse_stream_index).arg(var.toString()).arg(pulse_device_index));

        /// @todo Find a way to move the stream without saving it... We don't want to pollute the stream restore db.
        pa_operation* o;
        if (!(o = pa_context_move_sink_input_by_index(s_context, pulse_stream_index, pulse_device_index, NULL, NULL))) {
            logMessage(QString("pa_context_move_sink_input_by_index() failed"));
            return false;
        }
        pa_operation_unref(o);
    } else {
        logMessage(QString("... Not found in map. We will be notified of the device when the stream appears and we can process any moves needed then"));
    }
    return true;
#endif
}

bool PulseSupport::setOutputVolume(QString streamUuid, qreal volume) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(volume);
    return false;
#else
    logMessage(QString("Attempting to set volume to %1 for Output Stream %2").arg(volume).arg(streamUuid));

    // Attempt to look up the pulse stream index.
    if (s_outputStreams.contains(streamUuid) && s_outputStreams[streamUuid]->index() != PA_INVALID_INDEX) {
        PulseStream *stream = s_outputStreams[streamUuid];

        uint8_t channels = stream->channels();
        if (channels < 1) {
            logMessage("Channel count is less than 1. Cannot set volume.");
            return false;
        }

        pa_cvolume vol;
        pa_cvolume_set(&vol, channels, (volume * PA_VOLUME_NORM));

        logMessage(QString("Found PA index %1. Calling pa_context_set_sink_input_volume()").arg(stream->index()));
        pa_operation* o;
        if (!(o = pa_context_set_sink_input_volume(s_context, stream->index(), &vol, NULL, NULL))) {
            logMessage(QString("pa_context_set_sink_input_volume() failed"));
            return false;
        }
        pa_operation_unref(o);
    }
    return true;
#endif
}

bool PulseSupport::setOutputMute(QString streamUuid, bool mute) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(mute);
    return false;
#else
    logMessage(QString("Attempting to %1 mute for Output Stream %2").arg(mute ? "set" : "unset").arg(streamUuid));

    // Attempt to look up the pulse stream index.
    if (s_outputStreams.contains(streamUuid) && s_outputStreams[streamUuid]->index() != PA_INVALID_INDEX) {
        PulseStream *stream = s_outputStreams[streamUuid];

        logMessage(QString("Found PA index %1. Calling pa_context_set_sink_input_mute()").arg(stream->index()));
        pa_operation* o;
        if (!(o = pa_context_set_sink_input_mute(s_context, stream->index(), (mute ? 1 : 0), NULL, NULL))) {
            logMessage(QString("pa_context_set_sink_input_mute() failed"));
            return false;
        }
        pa_operation_unref(o);
    }
    return true;
#endif
}

bool PulseSupport::setCaptureDevice(QString streamUuid, int device) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    Q_UNUSED(device);
    return false;
#else
    if (s_captureDevices.size() < 2)
        return true;

    if (!s_captureDevices.contains(device)) {
        logMessage(QString("Attempting to set Capture Device for invalid device id %1.").arg(device));
        return false;
    }
    const QVariant var = s_captureDevices[device].properties["name"];
    logMessage(QString("Attempting to set Capture Device to '%1' for Capture Stream %2").arg(var.toString()).arg(streamUuid));

    // Attempt to look up the pulse stream index.
    if (s_captureStreams.contains(streamUuid) && s_captureStreams[streamUuid]->index() == PA_INVALID_INDEX) {
        logMessage(QString("... Found in map. Moving now"));

        uint32_t pulse_device_index = s_captureDevices[device].pulseIndex;
        uint32_t pulse_stream_index = s_captureStreams[streamUuid]->index();

        logMessage(QString("Moving Pulse Source Output %1 to '%2' (Pulse Sink %3)").arg(pulse_stream_index).arg(var.toString()).arg(pulse_device_index));

        /// @todo Find a way to move the stream without saving it... We don't want to pollute the stream restore db.
        pa_operation* o;
        if (!(o = pa_context_move_source_output_by_index(s_context, pulse_stream_index, pulse_device_index, NULL, NULL))) {
            logMessage(QString("pa_context_move_source_output_by_index() failed"));
            return false;
        }
        pa_operation_unref(o);
    } else {
        logMessage(QString("... Not found in map. We will be notified of the device when the stream appears and we can process any moves needed then"));
    }
    return true;
#endif
}

void PulseSupport::clearStreamCache(QString streamUuid) {
#ifndef HAVE_PULSEAUDIO
    Q_UNUSED(streamUuid);
    return;
#else
    logMessage(QString("Clearing stream cache for stream %1").arg(streamUuid));
    if (s_outputStreams.contains(streamUuid)) {
        PulseStream *stream = s_outputStreams[streamUuid];
        s_outputStreams.remove(streamUuid);
        delete stream;
    } else if (s_captureStreams.contains(streamUuid)) {
        PulseStream *stream = s_captureStreams[streamUuid];
        s_captureStreams.remove(streamUuid);
        delete stream;
    }
#endif
}

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_pulsesupport.cpp"

// vim: sw=4 ts=4
