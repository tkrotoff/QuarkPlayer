/*
 * This file was generated by dbusidl2cpp version 0.4
 * when processing input file org.kde.Phonon.AudioOutput.xml
 *
 * dbusidl2cpp is Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file. This file has been hand-edited.
 */

#ifndef AUDIOOUTPUTADAPTOR_P_H
#define AUDIOOUTPUTADAPTOR_P_H

#include <QtCore/QObject>

#ifndef QT_NO_DBUS
#include <QtDBus/QtDBus>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;

/*
 * Adaptor class for interface org.kde.Phonon.AudioOutput
 */
class AudioOutputAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.Phonon.AudioOutput")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.kde.Phonon.AudioOutput\">\n"
"    <property name=\"volume\" type=\"d\" access=\"readwrite\"/>\n"
"    <property name=\"muted\" type=\"b\" access=\"readwrite\"/>\n"
"    <method name=\"category\">\n"
"      <arg type=\"s\" direction=\"out\"/>\n"
"    </method>\n"
"    <method name=\"name\">\n"
"      <arg type=\"s\" direction=\"out\"/>\n"
"    </method>\n"
"  </interface>\n"
        "")
public:
    AudioOutputAdaptor(QObject *parent);
    virtual ~AudioOutputAdaptor();

public: // PROPERTIES
    Q_PROPERTY(double volume READ volume WRITE setVolume)
    double volume() const;
    void setVolume(double value);
    Q_PROPERTY(bool muted READ muted WRITE setMuted)
    bool muted() const;
    void setMuted(bool value);

public Q_SLOTS: // METHODS
    QString category();
    QString name();
Q_SIGNALS: // SIGNALS
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_DBUS

#endif // AUDIOOUTPUTADAPTOR_P_H
