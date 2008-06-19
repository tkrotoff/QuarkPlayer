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

#ifndef TKCONFIG_H
#define TKCONFIG_H

#include <QtCore/QSettings>
#include <QtCore/QMap>

/**
 * Stores QuarkPlayer configuration.
 *
 * Interface for class Config.
 *
 * TkConfig will be used for a future plugin system.
 *
 * @see QSettings
 * @author Tanguy Krotoff
 */
class TkConfig {
public:

	/** Associates a key to a default value. */
	typedef QMap<QString, QVariant> KeyDefaultValueMap;

	TkConfig(const KeyDefaultValueMap & defaultValues);

	virtual ~TkConfig();

	QString fileName() const;

	QStringList allKeys() const;

	void setValue(const QString & key, const QVariant & value);

	QVariant defaultValue(const QString & key) const;

	QVariant value(const QString & key) const;

private:

	void checkStatus() const;

	QSettings _settings;

	const KeyDefaultValueMap _defaultValues;
};

#endif	//TKCONFIG_H
