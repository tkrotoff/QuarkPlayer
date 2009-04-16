/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifdef Q_OS_WIN
	#include <shlobj.h>
#endif	//Q_OS_WIN

static const QString defaultStr("/Default");

WinFileAssociations::WinFileAssociations(const QString & applicationName) {
	_hkcr = new QSettings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	_applicationName = applicationName;
}

WinFileAssociations::~WinFileAssociations() {
}

void WinFileAssociations::updateKeysNeeded(const QString & extension) {
	//extension: mp3, avi...
	QString ext(extension);
	if (!ext.startsWith('.')) {
		ext.prepend('.');
	}

	//.mp3, .avi
	_extKey = ext + defaultStr;
	//QuarkPlayer.mp3, QuarkPlayer.avi...
	_appKey = _applicationName + ext;

	//.mp3/QuarkPlayer.backup
	_backupKey = ext + '/' + _applicationName + ".backup";
}

void WinFileAssociations::addCommand(const QString & extension, const QString & command, const QString & commandLine, const QString & description, const QString & icon) {
	if (checkError()) {
		return;
	}
	if (extension.isEmpty()) {
		qDebug() << __FUNCTION__ << "Error: the extension is empty";
		return;
	}
	updateKeysNeeded(extension);

	//Saves a backup key (QuarkPlayer.backup) if the key (extension: .mp3, .avi...) is already assigned
	QString extValue(_hkcr->value(_extKey).toString());
	QString backupValue(_hkcr->value(_extKey).toString());
	if (!backupValue.isEmpty() && extValue != _appKey) {
		_hkcr->setValue(_backupKey, backupValue);
	}

	//Create a "link" from .extension to QuarkPlayer.extension
	//Examples:
	//.mp3 -> QuarkPlayer.mp3
	//.avi -> QuarkPlayer.avi
	_hkcr->setValue(_extKey, _appKey);

	//Creates the needed key (or overwrite it)
	//Example: QuarkPlayer.mp3/shell/Play
	if (!description.isEmpty()) {
		QString descriptionKey(_appKey + "/shell/" + command + defaultStr);
		_hkcr->setValue(descriptionKey, description);
	}
	if (!commandLine.isEmpty()) {
		QString commandKey(_appKey + "/shell/" + command + "/command" + defaultStr);
		_hkcr->setValue(commandKey, commandLine);
	}
	if (!icon.isEmpty()) {
		QString iconKey(_appKey + "/DefaultIcon" + defaultStr);
		_hkcr->setValue(iconKey, icon);
	}
	//
}

void WinFileAssociations::addDirectoryCommand(const QString & command, const QString & commandLine, const QString & description) {
	if (checkError()) {
		return;
	}
	if (command.isEmpty()) {
		qDebug() << __FUNCTION__ << "Error: the command is empty";
		return;
	}

	if (!description.isEmpty()) {
		QString descriptionKey("Directory/shell/" + command + defaultStr);
		_hkcr->setValue(descriptionKey, description);
	}
	if (!commandLine.isEmpty()) {
		QString commandKey("Directory/shell/" + command + "/command" + defaultStr);
		_hkcr->setValue(commandKey, commandLine);
	}
}

void WinFileAssociations::deleteAssociation(const QString & extension) {
	if (checkError()) {
		return;
	}
	if (extension.isEmpty()) {
		qDebug() << __FUNCTION__ << "Error: the extension is empty";
		return;
	}
	updateKeysNeeded(extension);

	//Looking for .mp3/QuarkPlayer.mp3
	QString extValue(_hkcr->value(_extKey).toString());

	qDebug() << __FUNCTION__ << "extValue:" << extValue << "_appKey:" << _appKey;

	QString backupValue(_hkcr->value(_backupKey).toString());
	if (backupValue != _appKey) {
		//Normal case: restores the previous association
		qDebug() << __FUNCTION__ << "_extKey:" << _extKey << "backupValue:" << backupValue;
		_hkcr->setValue(_extKey, backupValue);
	} else {
		//backupValue == _appKey
		//or backupValue is empty
		//This means there were no other association with .mp3 from other application
		//In this case, let's remove .mp3/Default
		_hkcr->remove(_extKey);
	}

	//Removes .mp3/QuarkPlayer.backup
	_hkcr->remove(_backupKey);

	//Removes QuarkPlayer.mp3
	_hkcr->remove(_appKey);
}

void WinFileAssociations::deleteDirectoryCommand(const QString & command) {
	if (checkError()) {
		return;
	}
	if (command.isEmpty()) {
		qDebug() << __FUNCTION__ << "Error: the command is empty";
		return;
	}

	_hkcr->remove("Directory/shell/" + command);
}

bool WinFileAssociations::isAssociated(const QString & extension) {
	if (extension.isEmpty()) {
		qDebug() << __FUNCTION__ << "Error: the extension is empty";
		return false;
	}
	updateKeysNeeded(extension);

	QString extValue(_hkcr->value(_extKey).toString());
	if (extValue == _appKey) {
		return true;
	} else {
		return false;
	}
}

bool WinFileAssociations::containsDirectoryCommand(const QString & command) const {
	if (!command.isEmpty()) {
		return _hkcr->contains("Directory/shell/" + command + defaultStr);
	} else {
		return false;
	}
}

bool WinFileAssociations::checkError() const {
	if (_hkcr->isWritable() && _hkcr->status() == QSettings::NoError) {
		return false;
	} else {
		qCritical() << __FUNCTION__ << "Error: the registry key couldn't be read/write";
		return true;
	}
}

void WinFileAssociations::notifyFileAssociationChanged() {
#ifdef Q_OS_WIN
	//Calling SHChangeNotify() will update the file association icons
	//in case they had been reset.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
#endif	//Q_OS_WIN
}
