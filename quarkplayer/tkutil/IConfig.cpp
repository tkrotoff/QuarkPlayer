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

#include "IConfig.h"

#include <QtCore/QStringList>
#include <QtCore/QDebug>

IConfig::~IConfig() {
}

QStringList IConfig::allKeys() const {
	return _keyDefaultValueMap.keys();
}

QString IConfig::fileName() const {
	return _settings.fileName();
}

QSettings & IConfig::settings() {
	return _settings;
}

void IConfig::setValue(const QString & key, const QVariant & value) {
	QVariant previousValue = this->value(key);
	qDebug() << __FUNCTION__ << "previousValue:" << previousValue;

	QVariant defaultValue = this->defaultValue(key);
	qDebug() << __FUNCTION__ << "defaultValue:" << defaultValue;

	qDebug() << __FUNCTION__ << "value:" << value;

	if (value != previousValue) {
		qDebug() << __FUNCTION__ << "value saved:" << value;
		//Do not save the value if it is the same as the previous one
		_settings.setValue(key, value);
	}
}

QVariant IConfig::defaultValue(const QString & key) const {
	if (_keyDefaultValueMap.count(key) != 1) {
		qCritical() << __FUNCTION__ << "Error: invalid default value or key:" << key;
	}

	return _keyDefaultValueMap.value(key);
}

QVariant IConfig::value(const QString & key) const {
	return _settings.value(key, _keyDefaultValueMap.value(key));
}
