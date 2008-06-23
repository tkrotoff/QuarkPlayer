/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

TkConfig::TkConfig()
	:
#ifdef Q_OS_WIN
	_settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
#else
	_settings(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
#endif	//Q_OS_WIN
	{

	if (QCoreApplication::organizationName().isEmpty() || QCoreApplication::applicationName().isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: organizationName or applicationName empty";
	}

	qDebug() << __FUNCTION__ << "Config file:" << fileName();
}

TkConfig::~TkConfig() {
}

QStringList TkConfig::allKeys() const {
	return _defaultValues.keys();
}

QString TkConfig::fileName() const {
	return _settings.fileName();
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
		qCritical() << __FUNCTION__ << "Error: invalid default value or key:" << key;
	}

	return _defaultValues.value(key);
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
		qCritical() << __FUNCTION__ << "Error: access error occurred (e.g. trying to write to a read-only file)";
		break;
	case QSettings::FormatError:
		qCritical() << __FUNCTION__ << "Error: a format error occurred (e.g. loading a malformed INI file)";
		break;
	default:
		qCritical() << __FUNCTION__ << "Error: unknown QSettings::Status:" << status;
	}
}

void TkConfig::addKey(const QString & key, const QVariant & defaultValue) {
	if (_defaultValues.contains(key)) {
		qCritical() << __FUNCTION__ << "Error: this key already exists:" << key;
	}

	_defaultValues[key] = defaultValue;
}
