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

void WinFileAssociations::updateNeededKeys(const QString & extension) {
	static const QString defaultStr("/Default");

	//extension: mp3, avi...
	QString ext(extension);
	if (!ext.startsWith(".")) {
		ext.prepend(".");
	}
	//.mp3, .avi
	_extKey = ext + defaultStr;
	//QuarkPlayer.mp3, QuarkPlayer.avi...
	_appKey = _applicationName + ext;
	//QuarkPlayer.mp3/shell/Play
	_appKeyPlay = _appKey + "/shell/Play/command" + defaultStr;
	//QuarkPlayer.mp3/DefaultIcon
	_appKeyDefaultIcon = _appKey + "/DefaultIcon" + defaultStr;
	//.mp3/QuarkPlayer.backup
	_backupKey = ext + "/" + _applicationName + ".backup";
}

void WinFileAssociations::addAssociation(const QString & extension) {
	if (checkError()) {
		//An error occured
		return;
	}

	updateNeededKeys(extension);

	//Saves a backup key (QuarkPlayer.backup) if the key (extension: .mp3, .avi...) is already assigned
	QString backupValue(_hkcr->value(_extKey).toString());
	if (!backupValue.isEmpty()) {
		_hkcr->setValue(_backupKey, backupValue);
	}

	//Put a "link" to QuarkPlayer.extension as default
	//Examples:
	//.mp3 -> QuarkPlayer.mp3
	//.avi -> QuarkPlayer.avi
	_hkcr->setValue(_extKey, _appKey);

	//Creates the needed key (or overwrite it)
	//Play command + default icon of the application
	static const QString quote("\"");
	static const QString app(quote + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + quote);
	_hkcr->setValue(_appKeyPlay, app + " \"%1\"");

	//Do not overwrite the default icon
	//_hkcr->setValue(_appKeyDefaultIcon, app + ",0");
}

void WinFileAssociations::deleteAssociation(const QString & extension) {
	if (checkError()) {
		//An error occured
		return;
	}

	updateNeededKeys(extension);

	//Looking for .mp3/QuarkPlayer.mp3
	QString extValue(_hkcr->value(_extKey).toString());

	if (!extValue.isEmpty() && _appKey == extValue) {
		QString backupValue(_hkcr->value(_backupKey).toString());
		if (!backupValue.isEmpty()) {
			//Restores the previous association
			_hkcr->setValue(_extKey, backupValue);
		}

		//Removes .mp3/QuarkPlayer.backup
		_hkcr->remove(_backupKey);
	}

	if (_hkcr->contains(_appKeyPlay)) {
		//Removes QuarkPlayer.mp3
		_hkcr->remove(_appKey);
	}
}

bool WinFileAssociations::isAssociated(const QString & extension) {
	updateNeededKeys(extension);

	if (_hkcr->value(_extKey).toString() == _appKey) {
		return true;
	} else {
		return false;
	}
}

bool WinFileAssociations::checkError() {
	if (_hkcr->isWritable() && _hkcr->status() == QSettings::NoError) {
		return false;
	} else {
		qCritical() << __FUNCTION__ << "Error: the registry key couldn't be read/write";
		return true;
	}
}
