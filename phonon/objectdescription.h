/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef PHONON_OBJECTDESCRIPTION_H
#define PHONON_OBJECTDESCRIPTION_H

#include "phonon_export.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QtDebug>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QVariant>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace Phonon
{
    class ObjectDescriptionPrivate;

    /**
     * Defines the type of information that is contained in a ObjectDescription
     * object.
     *
     * \ingroup Backend
     */
    enum ObjectDescriptionType
    {
        /**
         * Audio output devices. This can be soundcards (with different drivers), soundservers or
         * other virtual outputs like playback on a different computer on the
         * network.
         *
         * For Hardware devices the backend should use libkaudiodevicelist (\ref
         * AudioDevice and \ref AudioDeviceEnumerator) which will list removable
         * devices even when they are unplugged and provide a unique identifier
         * that can make backends use the same identifiers.
         */
        AudioOutputDeviceType,

        /**
         * Lists all processing effects the backend supports.
         */
        EffectType,
        AudioChannelType,
        SubtitleType,

        /**
         * Audio capture devices. This can be soundcards (with different drivers), soundservers or
         * other virtual inputs like capture on a different computer on the
         * network.
         *
         * For Hardware devices the backend should use libkaudiodevicelist (\ref
         * AudioDevice and \ref AudioDeviceEnumerator) which will list removable
         * devices even when they are unplugged and provide a unique identifier
         * that can make backends use the same identifiers.
         */
        AudioCaptureDeviceType

        //VideoOutputDeviceType,
        //VideoCaptureDeviceType,
        //AudioCodecType,
        //VideoCodecType,
        //ContainerFormatType,
        //VisualizationType,
    };

/** \internal
 * \class ObjectDescriptionData objectdescription.h Phonon/ObjectDescription
 * \brief Data class for objects describing devices or features of the backend.
 *
 * \author Matthias Kretz <kretz@kde.org>
 * \see BackendCapabilities
 */
class PHONON_EXPORT ObjectDescriptionData : public QSharedData //krazy:exclude=dpointer (it's protected, which should be fine for this type of class)
{
    public:
        /**
         * Returns \c true if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c false.
         */
        bool operator==(const ObjectDescriptionData &otherDescription) const;

        /**
         * Returns the name of the capture source.
         *
         * \return A string that should be presented to the user to
         * choose the capture source.
         */
        QString name() const;

        /**
         * Returns a description of the capture source. This text should
         * make clear what sound source this is, which is sometimes hard
         * to describe or understand from just the name.
         *
         * \return A string describing the capture source.
         */
        QString description() const;

        /**
         * Returns a named property.
         *
         * If the property is not set an invalid value is returned.
         *
         * \see propertyNames()
         */
        QVariant property(const char *name) const;

        /**
         * Returns all names that return valid data when property() is called.
         *
         * \see property()
         */
        QList<QByteArray> propertyNames() const;

        /**
         * Returns \c true if the Tuple is valid (index != -1); otherwise returns
         * \c false.
         */
        bool isValid() const;

        /**
         * A unique identifier for this device/. Used internally
         * to distinguish between the devices/.
         *
         * \return An integer that uniquely identifies every device/
         */
        int index() const;

        static ObjectDescriptionData *fromIndex(ObjectDescriptionType type, int index);

        ~ObjectDescriptionData();

        ObjectDescriptionData(ObjectDescriptionPrivate * = 0);
        ObjectDescriptionData(int index, const QHash<QByteArray, QVariant> &properties);

    protected:
        ObjectDescriptionPrivate *const d;

    private:
        ObjectDescriptionData &operator=(const ObjectDescriptionData &rhs);
};

template<ObjectDescriptionType T> class ObjectDescriptionModel;

/** \class ObjectDescription objectdescription.h Phonon/ObjectDescription
 * \short Provides a tuple of enduser visible name and description.
 *
 * Some parts give the enduser choices, e.g. what source to capture audio from.
 * These choices are described by the name and description methods of this class
 * and identified with the id method. Subclasses then define additional
 * information like which audio and video choices belong together.
 *
 * \ingroup Frontend
 * \author Matthias Kretz <kretz@kde.org>
 */
template<ObjectDescriptionType T>
class ObjectDescription
{
    public:
        /**
         * Returns a new description object that describes the
         * device/effect/codec/...  with the given \p index.
         */
        static inline ObjectDescription<T> fromIndex(int index) { //krazy:exclude=inline
            return ObjectDescription<T>(QExplicitlySharedDataPointer<ObjectDescriptionData>(ObjectDescriptionData::fromIndex(T, index)));
        }

        /**
         * Returns \c true if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c false.
         */
        inline bool operator==(const ObjectDescription &otherDescription) const { //krazy:exclude=inline
            return *d == *otherDescription.d;
        }

        /**
         * Returns \c false if this ObjectDescription describes the same
         * as \p otherDescription; otherwise returns \c true.
         */
        inline bool operator!=(const ObjectDescription &otherDescription) const { //krazy:exclude=inline
            return !operator==(otherDescription);
        }

        /**
         * Returns the name of the capture source.
         *
         * \return A string that should be presented to the user to
         * choose the capture source.
         */
        inline QString name() const { return d->name(); } //krazy:exclude=inline

        /**
         * Returns a description of the capture source. This text should
         * make clear what sound source this is, which is sometimes hard
         * to describe or understand from just the name.
         *
         * \return A string describing the capture source.
         */
        inline QString description() const { return d->description(); } //krazy:exclude=inline

        /**
         * Returns a named property.
         *
         * If the property is not set an invalid value is returned.
         *
         * \see propertyNames()
         */
        inline QVariant property(const char *name) const { return d->property(name); } //krazy:exclude=inline

        /**
         * Returns all names that return valid data when property() is called.
         *
         * \see property()
         */
        inline QList<QByteArray> propertyNames() const { return d->propertyNames(); } //krazy:exclude=inline

        /**
         * Returns \c true if the Tuple is valid (index != -1); otherwise returns
         * \c false.
         */
        inline bool isValid() const { return d->isValid(); } //krazy:exclude=inline

        /**
         * A unique identifier for this device/. Used internally
         * to distinguish between the devices/.
         *
         * \return An integer that uniquely identifies every device/
         */
        inline int index() const { return d->index(); } //krazy:exclude=inline

        ObjectDescription() : d(new ObjectDescriptionData(0)) {}
        ObjectDescription(int index, const QHash<QByteArray, QVariant> &properties) : d(new ObjectDescriptionData(index, properties)) {}

    protected:
        friend class ObjectDescriptionModel<T>;
        ObjectDescription(const QExplicitlySharedDataPointer<ObjectDescriptionData> &dd) : d(dd) {}
        QExplicitlySharedDataPointer<ObjectDescriptionData> d;
};

template<ObjectDescriptionType T>
inline QDebug operator<<(QDebug s, const ObjectDescription<T> &o) //krazy:exclude=inline
{
    return s << o.name();
}

/**
 * \ingroup BackendInformation
 */
typedef ObjectDescription<AudioOutputDeviceType> AudioOutputDevice;
/**
 * \ingroup BackendInformation
 */
typedef ObjectDescription<AudioCaptureDeviceType> AudioCaptureDevice;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VideoOutputDeviceType> VideoOutputDevice;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VideoCaptureDeviceType> VideoCaptureDevice;
/**
 * \ingroup BackendInformation
 */
typedef ObjectDescription<EffectType> EffectDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<AudioCodecType> AudioCodecDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VideoCodecType> VideoCodecDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<ContainerFormatType> ContainerFormatDescription;
/**
 * \ingroup BackendInformation
 */
//typedef ObjectDescription<VisualizationType> VisualizationDescription;
typedef ObjectDescription<AudioChannelType> AudioChannelDescription;
typedef ObjectDescription<SubtitleType> SubtitleDescription;

} //namespace Phonon

Q_DECLARE_METATYPE(Phonon::AudioChannelDescription)
Q_DECLARE_METATYPE(Phonon::SubtitleDescription)
Q_DECLARE_METATYPE(Phonon::AudioOutputDevice)
Q_DECLARE_METATYPE(Phonon::AudioCaptureDevice)
Q_DECLARE_METATYPE(Phonon::EffectDescription)
Q_DECLARE_METATYPE(QList<Phonon::AudioChannelDescription>)
Q_DECLARE_METATYPE(QList<Phonon::SubtitleDescription>)
Q_DECLARE_METATYPE(QList<Phonon::AudioOutputDevice>)
Q_DECLARE_METATYPE(QList<Phonon::AudioCaptureDevice>)
Q_DECLARE_METATYPE(QList<Phonon::EffectDescription>)

QT_END_NAMESPACE
QT_END_HEADER

#endif // PHONON_OBJECTDESCRIPTION_H
