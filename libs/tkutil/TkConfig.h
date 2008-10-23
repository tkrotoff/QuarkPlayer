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

#include <tkutil/tkutil_export.h>

#include <QtCore/QObject>
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
class TKUTIL_API TkConfig : public QObject {
	Q_OBJECT
public:

	/**
	 * Creates a configuration file.
	 *
	 * Configuration file is
	 * "C:/Documents and Settings/UserName/Application Data/OrganizationName/ApplicationName.ini"
	 * For QuarkPlayer it will be QuarkPlayer/QuarkPlayer.ini
	 * The application name is taken from QCoreApplication::organizationName() and
	 * QCoreApplication::applicationName()
	 */
	TkConfig();

	virtual ~TkConfig();

	/** Gets the configuration filename. */
	QString fileName() const;

	/**
	 * Gets the path where the configuration file is saved.
	 *
	 * Last character is not a '/', you will have to add it yourself if needed.
	 *
	 * @return configuration file path
	 */
	QString configDir() const;

	QStringList allKeys() const;

	void setValue(const QString & key, const QVariant & value);

	QVariant defaultValue(const QString & key) const;

	bool contains(const QString & key) const;

	QVariant value(const QString & key) const;

	void addKey(const QString & key, const QVariant & defaultValue);

signals:

	void valueChanged(const QString & key, const QVariant & value);

private:

	void checkStatus() const;

	QSettings _settings;

	/** Associates a key to a default value. */
	typedef QMap<QString, QVariant> KeyDefaultValueMap;

	KeyDefaultValueMap _defaultValues;
};

#endif	//TKCONFIG_H
