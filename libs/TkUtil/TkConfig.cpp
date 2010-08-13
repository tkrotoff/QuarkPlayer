/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TkConfig.h"

#include "TkUtilLogger.h"

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>

TkConfig::TkConfig()
	:
#ifdef Q_WS_WIN
	//Forces INI file format instead of using Windows registry database
	_settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
#else
	_settings(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
#endif	//Q_WS_WIN
	{

	if (QCoreApplication::organizationName().isEmpty() || QCoreApplication::applicationName().isEmpty()) {
		TkUtilCritical() << "QCoreApplication::organizationName or applicationName empty";
	}

	TkUtilDebug() << "Config file:" << fileName();
}

TkConfig::~TkConfig() {
}

QStringList TkConfig::allKeys() const {
	return _defaultValues.keys();
}

QString TkConfig::fileName() const {
	return _settings.fileName();
}

QString TkConfig::configDir() const {
	return QFileInfo(fileName()).absolutePath();
}

void TkConfig::setValue(const QString & key, const QVariant & value) {
	QVariant previousValue = this->value(key);
	QVariant defaultValue = this->defaultValue(key);

	if (value != previousValue) {
		//Do not save the value if it is the same as the previous one
		_settings.setValue(key, value);
		emit valueChanged(key, value);
	}

	checkStatus();
}

QVariant TkConfig::defaultValue(const QString & key) const {
	if (_defaultValues.count(key) != 1) {
		TkUtilCritical() << "Invalid default value or key:" << key;
	}

	return _defaultValues.value(key);
}

bool TkConfig::contains(const QString & key) const {
	return _defaultValues.contains(key);
}

QVariant TkConfig::value(const QString & key) const {
	QVariant value = _settings.value(key, _defaultValues.value(key));
	checkStatus();
	return value;
}

void TkConfig::checkStatus() const {
	//Don't do a sync() even if written inside Qt documentation
	//cf http://doc.trolltech.com/main-snapshot/qsettings.html#status
	QSettings::Status status = _settings.status();

	switch (status) {
	case QSettings::NoError:
		//Good to go
		break;
	case QSettings::AccessError:
		TkUtilCritical() << "Access error occurred (e.g. trying to write to a read-only file)";
		break;
	case QSettings::FormatError:
		TkUtilCritical() << "A format error occurred (e.g. loading a malformed INI file)";
		break;
	default:
		TkUtilCritical() << "Unknown QSettings::Status:" << status;
	}
}

void TkConfig::addKey(const QString & key, const QVariant & defaultValue) {
	if (_defaultValues.contains(key)) {
		TkUtilCritical() << "Key already exists:" << key;
	}

	_defaultValues[key] = defaultValue;
}

QSettings & TkConfig::settings() {
	return _settings;
}
