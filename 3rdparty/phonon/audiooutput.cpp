/*  This file is part of the KDE project
    Copyright (C) 2005-2006 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "audiooutput.h"
#include "audiooutput_p.h"
#include "factory_p.h"
#include "objectdescription.h"
#include "audiooutputadaptor_p.h"
#include "globalconfig_p.h"
#include "audiooutputinterface.h"
#include "phononnamespace_p.h"
#include "platform_p.h"

#include <qmath.h>

#define PHONON_CLASSNAME AudioOutput
#define IFACES2 AudioOutputInterface42
#define IFACES1 IFACES2
#define IFACES0 AudioOutputInterface40, IFACES1
#define PHONON_INTERFACENAME IFACES0

QT_BEGIN_NAMESPACE

namespace Phonon
{

static inline bool callSetOutputDevice(MediaNodePrivate *const d, int index)
{
    Iface<IFACES2> iface(d);
    if (iface) {
        return iface->setOutputDevice(AudioOutputDevice::fromIndex(index));
    }
    return Iface<IFACES0>::cast(d)->setOutputDevice(index);
}

static inline bool callSetOutputDevice(MediaNodePrivate *const d, const AudioOutputDevice &dev)
{
    Iface<IFACES2> iface(d);
    if (iface) {
        return iface->setOutputDevice(dev);
    }
    return Iface<IFACES0>::cast(d)->setOutputDevice(dev.index());
}

AudioOutput::AudioOutput(Phonon::Category category, QObject *parent)
    : AbstractAudioOutput(*new AudioOutputPrivate, parent)
{
    K_D(AudioOutput);
    d->init(category);
}

AudioOutput::AudioOutput(QObject *parent) 
    : AbstractAudioOutput(*new AudioOutputPrivate, parent)
{
    K_D(AudioOutput);
    d->init(NoCategory);
}

void AudioOutputPrivate::init(Phonon::Category c)
{
    Q_Q(AudioOutput);
    category = c;

    // select hardware device according to the category
    outputDeviceIndex = GlobalConfig().audioOutputDeviceFor(category);

    createBackendObject();
#ifndef QT_NO_DBUS
    new AudioOutputAdaptor(q);
    static unsigned int number = 0;
    QDBusConnection::sessionBus().registerObject("/AudioOutputs/" + QString::number(number++), q);
#endif

    q->connect(Factory::sender(), SIGNAL(availableAudioOutputDevicesChanged()), SLOT(_k_deviceListChanged()));
}



void AudioOutputPrivate::createBackendObject()
{
    if (m_backendObject)
        return;
    Q_Q(AudioOutput);
    m_backendObject = Factory::createAudioOutput(q);
    if (m_backendObject) {
        setupBackendObject();
    }
}

QString AudioOutput::name() const
{
    K_D(const AudioOutput);
    return d->name;
}

void AudioOutput::setName(const QString &newName)
{
    K_D(AudioOutput);
    d->name = newName;
    setVolume(Platform::loadVolume(newName));
}

static const qreal LOUDNESS_TO_VOLTAGE_EXPONENT = 0.67;
static const qreal VOLTAGE_TO_LOUDNESS_EXPONENT = 1.0/LOUDNESS_TO_VOLTAGE_EXPONENT;

void AudioOutput::setVolume(qreal volume)
{
    K_D(AudioOutput);
    d->volume = volume;
    if (k_ptr->backendObject() && !d->muted) {
        // using Stevens' power law loudness is proportional to (sound pressure)^0.67
        // sound pressure is proportional to voltage:
        // p² \prop P \prop V²
        // => if a factor for loudness of x is requested
        INTERFACE_CALL(setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));
    } else {
        emit volumeChanged(volume);
    }
    Platform::saveVolume(d->name, volume);
}

qreal AudioOutput::volume() const
{
    K_D(const AudioOutput);
    if (d->muted || !d->m_backendObject) {
        return d->volume;
    }
    return pow(INTERFACE_CALL(volume()), LOUDNESS_TO_VOLTAGE_EXPONENT);
}

#ifndef PHONON_LOG10OVER20
#define PHONON_LOG10OVER20
static const qreal log10over20 = 0.1151292546497022842; // ln(10) / 20
#endif // PHONON_LOG10OVER20

qreal AudioOutput::volumeDecibel() const
{
    K_D(const AudioOutput);
    if (d->muted || !d->m_backendObject) {
        return -log(d->volume) / log10over20;
    }
    return -0.67 * log(INTERFACE_CALL(volume())) / log10over20;
}

void AudioOutput::setVolumeDecibel(qreal newVolumeDecibel)
{
    setVolume(exp(-newVolumeDecibel * log10over20));
}

bool AudioOutput::isMuted() const
{
    K_D(const AudioOutput);
    return d->muted;
}

void AudioOutput::setMuted(bool mute)
{
    K_D(AudioOutput);
    if (d->muted != mute) {
        if (mute) {
            d->muted = mute;
            if (k_ptr->backendObject()) {
                INTERFACE_CALL(setVolume(0.0));
            }
        } else {
            if (k_ptr->backendObject()) {
                INTERFACE_CALL(setVolume(pow(d->volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));
            }
            d->muted = mute;
        }
        emit mutedChanged(mute);
    }
}

Category AudioOutput::category() const
{
    K_D(const AudioOutput);
    return d->category;
}

AudioOutputDevice AudioOutput::outputDevice() const
{
    K_D(const AudioOutput);
    int index;
    if (d->m_backendObject)
        index = INTERFACE_CALL(outputDevice());
    else
        index = d->outputDeviceIndex;
    return AudioOutputDevice::fromIndex(index);
}

bool AudioOutput::setOutputDevice(const AudioOutputDevice &newAudioOutputDevice)
{
    K_D(AudioOutput);
    if (!newAudioOutputDevice.isValid()) {
        d->outputDeviceOverridden = false;
        d->outputDeviceIndex = GlobalConfig().audioOutputDeviceFor(d->category);
    } else {
        d->outputDeviceOverridden = true;
        d->outputDeviceIndex = newAudioOutputDevice.index();
    }
    if (k_ptr->backendObject()) {
        return callSetOutputDevice(k_ptr, d->outputDeviceIndex);
    }
    return true;
}

bool AudioOutputPrivate::aboutToDeleteBackendObject()
{
    if (m_backendObject) {
        volume = pINTERFACE_CALL(volume());
    }
    return AbstractAudioOutputPrivate::aboutToDeleteBackendObject();
}

void AudioOutputPrivate::setupBackendObject()
{
    Q_Q(AudioOutput);
    Q_ASSERT(m_backendObject);
    AbstractAudioOutputPrivate::setupBackendObject();

    QObject::connect(m_backendObject, SIGNAL(volumeChanged(qreal)), q, SLOT(_k_volumeChanged(qreal)));
    QObject::connect(m_backendObject, SIGNAL(audioDeviceFailed()), q, SLOT(_k_audioDeviceFailed()));

    // set up attributes
    pINTERFACE_CALL(setVolume(pow(volume, VOLTAGE_TO_LOUDNESS_EXPONENT)));

    // if the output device is not available and the device was not explicitly set
    if (!outputDeviceOverridden && !callSetOutputDevice(this, outputDeviceIndex)) {
        // fall back in the preference list of output devices
        QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category);
        if (deviceList.isEmpty()) {
            return;
        }
        if (outputDeviceIndex == deviceList.takeFirst()) { // removing the first device so that
            // if it's the same device as the one we tried we only try all the following
            foreach (int devIndex, deviceList) {
                if (callSetOutputDevice(this, devIndex)) {
                    handleAutomaticDeviceChange(AudioOutputDevice::fromIndex(devIndex), AudioOutputPrivate::FallbackChange);
                    break; // found one that works
                }
            }
        }
    }
}

void AudioOutputPrivate::_k_volumeChanged(qreal newVolume)
{
    if (!muted) {
        Q_Q(AudioOutput);
        emit q->volumeChanged(pow(newVolume, qreal(0.67)));
    }
}

void AudioOutputPrivate::_k_revertFallback()
{
    if (deviceBeforeFallback == -1) {
        return;
    }
    outputDeviceIndex = deviceBeforeFallback;
    callSetOutputDevice(this, outputDeviceIndex);
    Q_Q(AudioOutput);
    emit q->outputDeviceChanged(AudioOutputDevice::fromIndex(outputDeviceIndex));
}

void AudioOutputPrivate::_k_audioDeviceFailed()
{
    pDebug() << Q_FUNC_INFO;
    // outputDeviceIndex identifies a failing device
    // fall back in the preference list of output devices
    QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category);
    foreach (int devIndex, deviceList) {
        // if it's the same device as the one that failed, ignore it
        if (outputDeviceIndex != devIndex) {
            const AudioOutputDevice info = AudioOutputDevice::fromIndex(devIndex);
            if (info.property("available").toBool()) {
                if (callSetOutputDevice(this, info)) {
                    handleAutomaticDeviceChange(info, FallbackChange);
                    break; // found one that works
                }
            }
        }
    }
}

void AudioOutputPrivate::_k_deviceListChanged()
{
    pDebug() << Q_FUNC_INFO;
    // let's see if there's a usable device higher in the preference list
    QList<int> deviceList = GlobalConfig().audioOutputDeviceListFor(category);
    DeviceChangeType changeType = HigherPreferenceChange;
    foreach (int devIndex, deviceList) {
        const AudioOutputDevice info = AudioOutputDevice::fromIndex(devIndex);
        if (!info.property("available").toBool()) {
            if (outputDeviceIndex == devIndex) {
                // we've reached the currently used device
                changeType = FallbackChange;
            }
            pDebug() << devIndex << "is not available";
            continue;
        }
        pDebug() << devIndex << "is available";
        if (outputDeviceIndex == devIndex) {
            // we've reached the currently used device, nothing to change
            break;
        }
        if (callSetOutputDevice(this, info)) {
            handleAutomaticDeviceChange(info, changeType);
            break; // found one with higher preference that works
        }
    }
}

static struct
{
    int first;
    int second;
} g_lastFallback = { 0, 0 };

void AudioOutputPrivate::handleAutomaticDeviceChange(const AudioOutputDevice &device2, DeviceChangeType type)
{
    Q_Q(AudioOutput);
    deviceBeforeFallback = outputDeviceIndex;
    outputDeviceIndex = device2.index();
    emit q->outputDeviceChanged(device2);
    const AudioOutputDevice &device1 = AudioOutputDevice::fromIndex(deviceBeforeFallback);
    switch (type) {
    case FallbackChange:
        if (g_lastFallback.first != device1.index() || g_lastFallback.second != device2.index()) {
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
            const QString text = AudioOutput::tr("<html>The audio playback device <b>%1</b> does not work.<br/>"
                    "Falling back to <b>%2</b>.</html>").arg(device1.name()).arg(device2.name());
            Platform::notification("AudioDeviceFallback", text);
#endif //QT_NO_PHONON_PLATFORMPLUGIN
            g_lastFallback.first = device1.index();
            g_lastFallback.second = device2.index();
        }
        break;
    case HigherPreferenceChange:
        {
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        const QString text = AudioOutput::tr("<html>Switching to the audio playback device <b>%1</b><br/>"
                "which just became available and has higher preference.</html>").arg(device2.name());
        Platform::notification("AudioDeviceFallback", text,
                QStringList(AudioOutput::tr("Revert back to device '%1'").arg(device1.name())),
                q, SLOT(_k_revertFallback()));
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        g_lastFallback.first = 0;
        g_lastFallback.second = 0;
        }
        break;
    }
}

} //namespace Phonon

QT_END_NAMESPACE

#include "moc_audiooutput.cpp"

#undef PHONON_CLASSNAME
#undef PHONON_INTERFACENAME
#undef IFACES2
#undef IFACES1
#undef IFACES0
