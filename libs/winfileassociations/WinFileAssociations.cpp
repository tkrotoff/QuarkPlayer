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

#include "WinFileAssociations.h"

#include <QtCore/QSettings>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QDebug>

WinFileAssociations::WinFileAssociations(const QString & applicationName) {
	_hkcr = new QSettings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	_applicationName = applicationName;
}

WinFileAssociations::~WinFileAssociations() {
}

void WinFileAssociations::addAssociation(const QString & extension) {
	if (checkError()) {
		//An error occured
		return;
	}

	//.mp3, .avi
	QString extKey(extension + "/" + QString("Default"));
	//QuarkPlayer.mp3, QuarkPlayer.avi...
	QString appValue(_applicationName + extension);
	//.mp3/QuarkPlayer.backup
	QString backupKey(extension + "/" + _applicationName + ".backup");

	//Saves a backup key (QuarkPlayer.backup) if the key (extension: .mp3, .avi...) is already assigned
	QString backupValue = _hkcr->value(extKey).toString();
	if (!backupValue.isEmpty()) {
		_hkcr->setValue(backupKey, backupValue);
	}

	//Put a "link" to QuarkPlayer.extension as default
	//Examples:
	//.mp3 -> QuarkPlayer.mp3
	//.avi -> QuarkPlayer.avi
	_hkcr->setValue(extKey, appValue);

	//Creates the needed key if not already done
	if (!_hkcr->contains(appValue)) {
		_hkcr->setValue(extKey, appValue);

		//Play command + default icon of the application
		QString quote("\"");
		QString app(quote + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + quote);
		_hkcr->setValue(appValue + "/shell/Play/command/Default", app + " \"%1\"");
		_hkcr->setValue(appValue + "/DefaultIcon/Default", app + ",0");
	}
}

void WinFileAssociations::deleteAssociation(const QString & extension) {
	if (checkError()) {
		//An error occured
		return;
	}

	//.mp3, .avi
	QString extKey(extension + "/" + QString("Default"));
	//QuarkPlayer.mp3, QuarkPlayer.avi...
	QString appValue(_applicationName + extension);
	//.mp3/QuarkPlayer.backup
	QString backupKey(extension + "/" + _applicationName + ".backup");

	QString extValue(_hkcr->value(extKey).toString());

	if (!extValue.isEmpty() && appValue == extValue) {
		//Example of a backup key: .mp3/QuarkPlayer.backup
		QString backupValue(_hkcr->value(backupKey).toString());

		if (!backupValue.isEmpty()) {
			//Restores the previous association
			_hkcr->setValue(extKey, backupValue);
		}

		//Removes .mp3/QuarkPlayer.backup
		_hkcr->remove(backupKey);
	}
}

bool WinFileAssociations::isAssociated(const QString & extension) {
	if (_hkcr->value(extension).toString() == QString(_applicationName + extension)) {
		return true;
	} else {
		return false;
	}
}

QString WinFileAssociations::backupKey(const QString & extension) const {
	//Example of a backup key: .mp3/QuarkPlayer.backup
	return extension + "/" + _applicationName + ".backup";
}

bool WinFileAssociations::checkError() {
	if (_hkcr->isWritable() && _hkcr->status() == QSettings::NoError) {
		return false;
	} else {
		qCritical() << __FUNCTION__ << "Error: the registry key couldn't be read/write";
		return true;
	}
}
