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

#include "WinDefaultApplication.h"

#include <filetypes/FileTypes.h>

#include <QtCore/QSysInfo>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

//See GUUID from quarplayer-plugins/wincontextmenu/WinContextMenu.h
static const char * SHELLEX_GUUID = "{BC6D1C0E-ADF5-44a1-9940-978019DF7985}";

static const char * DEFAULT_STR = "/Default";

static const char * QUOTE = "\"";

WinDefaultApplication::WinDefaultApplication() {
}

WinDefaultApplication::~WinDefaultApplication() {
}

QString WinDefaultApplication::shellExDLLFilename() {
	static const QString filename(
		QDir::toNativeSeparators(QCoreApplication::applicationDirPath())
		+ "\\quarkplayercontextmenu.dll"
	);

	return filename;
}

QString WinDefaultApplication::shellExDLLRegistryFilename() {
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	QString shellExDLLFilenameKey("CLSID/" + QString(SHELLEX_GUUID) + "/InprocServer32" + DEFAULT_STR);
	return hkcr.value(shellExDLLFilenameKey).toString();
}

void WinDefaultApplication::registerShellExDLL() {
	QString previousShellEx(shellExDLLRegistryFilename());
	QString ourShellEx(shellExDLLFilename());
	Q_ASSERT(!ourShellEx.isEmpty());

	if (ourShellEx != previousShellEx) {

		//Unregister the previous shellex DLL
		unregisterShellExDLL(previousShellEx);

		QProcess regsvr32;
		QStringList args;
		args << "/s";	//silent
		args << ourShellEx;
		regsvr32.start("regsvr32", args);
		regsvr32.waitForFinished();
	}
}

void WinDefaultApplication::unregisterShellExDLL(const QString & shellExDLLFilename) {
	if (!shellExDLLFilename.isEmpty()) {
		QProcess regsvr32;
		QStringList args;
		args << "/s";	//silent
		args << "/u";	//uninstall
		args << shellExDLLFilename;
		regsvr32.start("regsvr32", args);
		regsvr32.waitForFinished();
	}
}

void WinDefaultApplication::install() {
	qDebug() << __FUNCTION__ << "Install QuarkPlayer as the default media application";
	registerAsDefaultMediaPlayer();
	addDefaultFileAssociations();
}

void WinDefaultApplication::uninstall() {
	qDebug() << __FUNCTION__ << "Uninstall QuarkPlayer as the default media application";
	unregisterAsDefaultMediaPlayer();
	deleteAllFileAssociations();
	unregisterShellExDLL(shellExDLLFilename());
}

void WinDefaultApplication::registerAsDefaultMediaPlayer() {
	//Register QuarkPlayer as the default media player under Windows
	//See http://msdn.microsoft.com/en-us/library/bb776851(VS.85).aspx
	//The user must be root in order to modify HKEY_LOCAL_MACHINE
	//If the user is not root, HKEY_CURRENT_USER should be used
	//Registry keys needed:
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/Default QuarkPlayer
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/DefaultIcon/Default quarkplayer.exe,0
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/shell/open/command/Default quarkplayer.exe
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/InstallInfo/HideIconsCommand quarkplayer.exe --windows-uninstall
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/InstallInfo/ReinstallCommand quarkplayer.exe --windows-install
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/InstallInfo/ShowIconsCommand quarkplayer.exe --windows-install
	//HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer/InstallInfo/IconsVisible = 1

	QSettings hklm("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
	if (!hklm.isWritable() && hklm.status() != QSettings::NoError) {
		qWarning() << __FUNCTION__ << "Error: cannot access HKLM, user is not root";
		//Switch to current user registry since local machine registry
		//is not writable. The user is probably not root.
		//hklm = QSettings("HKEY_CURRENT_USER", QSettings::NativeFormat);
	}

	static const QString appFilePath(QUOTE + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + QUOTE);
	static const QString defaultMediaPlayerKey("Software/Clients/Media/" + QCoreApplication::applicationName());

	hklm.setValue(defaultMediaPlayerKey + DEFAULT_STR, QCoreApplication::applicationName());
	hklm.setValue(defaultMediaPlayerKey + "/DefaultIcon" + DEFAULT_STR, appFilePath + ",0");
	hklm.setValue(defaultMediaPlayerKey + "/shell/open/command" + DEFAULT_STR, appFilePath);
	static const QString unregisterCmd(appFilePath + " --windows-uninstall");
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/HideIconsCommand", unregisterCmd);
	static const QString registerCmd(appFilePath + " --windows-install");
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/ReinstallCommand", registerCmd);
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/ShowIconsCommand", registerCmd);
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/IconsVisible", 1);
}

void WinDefaultApplication::unregisterAsDefaultMediaPlayer() {
	//Detele the registry keys associated with "Set Programs Access and Defaults" (SPAD)
	QSettings hklm("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);
	hklm.remove("Software/Clients/Media/" + QCoreApplication::applicationName());
}

void WinDefaultApplication::addFileAssociation(const QString & extension) {
	if (extension.isEmpty()) {
		qWarning() << __FUNCTION__ << "Error: extension is empty";
		return;
	}

	if (FileTypes::extensions(FileType::Subtitle).contains(extension, Qt::CaseInsensitive)) {
		//Do not change icon nor default application for subtitle files
		return;
	}

	QString icon;
#ifdef Q_OS_WIN
	if (QSysInfo::windowsVersion() < QSysInfo::WV_VISTA) {
		//Do this only under Windows versions inferior to Vista.
		//Under Vista, mimetypes icons are fine, no need to change them
		//Under Windows XP it is better to change the mimetypes icons
		//since they depend on the application icon
		static const QString applicationDirPath(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()));
		if (FileTypes::extensions(FileType::Video).contains(extension, Qt::CaseInsensitive)) {
			icon = applicationDirPath + "\\icons\\mimetypes\\video-x-generic.ico";
		} else if (FileTypes::extensions(FileType::Audio).contains(extension, Qt::CaseInsensitive)) {
			icon = applicationDirPath + "\\icons\\mimetypes\\audio-x-generic.ico";
		} else if (FileTypes::extensions(FileType::Playlist).contains(extension, Qt::CaseInsensitive)) {
			icon = applicationDirPath + "\\icons\\mimetypes\\playlist-x-generic.ico";
		}/* else if (FileTypes::extensions(FileType::Subtitle).contains(extension, Qt::CaseInsensitive)) {
			icon = QString();
		}*/
	}
#endif	//Q_OS_WIN

	registerShellExDLL();

	//HKEY_CLASSES_ROOT/QuarkPlayer.mp3
	//HKEY_CLASSES_ROOT/QuarkPlayer.mp3/DefaultIcon/Default icon
	//HKEY_CLASSES_ROOT/QuarkPlayer.mp3/shellex/ContextMenuHandlers/QuarkPlayer/GUUID

	//
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	//mp3 -> .mp3
	QString ext(extension);
	ext.prepend('.');
	//QuarkPlayer.mp3
	QString appKey = QCoreApplication::applicationName() + ext;
	//.mp3
	QString extKey = ext + DEFAULT_STR;
	//.mp3/QuarkPlayer.backup
	QString backupKey = ext + '/' + QCoreApplication::applicationName() + ".backup";
	//.mp3/Default
	QString extValue(hkcr.value(extKey).toString());
	///

	if (!icon.isEmpty()) {
		hkcr.setValue(appKey + "/DefaultIcon" + DEFAULT_STR, icon);
	}
	hkcr.setValue(appKey + "/shellex/ContextMenuHandlers/QuarkPlayer" + DEFAULT_STR, SHELLEX_GUUID);

	//Saves a backup key (QuarkPlayer.backup) if the key (extension: .mp3, .avi...) is already assigned
	if (!extValue.isEmpty() && extValue != appKey) {
		hkcr.setValue(backupKey, extValue);
	}

	//Create a "link" .mp3 -> QuarkPlayer.mp3
	hkcr.setValue(extKey, appKey);
}

void WinDefaultApplication::deleteFileAssociation(const QString & extension) {
	//
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	//mp3 -> .mp3
	QString ext(extension);
	ext.prepend('.');
	//QuarkPlayer.mp3
	QString appKey = QCoreApplication::applicationName() + ext;
	//.mp3
	QString extKey = ext + DEFAULT_STR;
	//.mp3/QuarkPlayer.backup
	QString backupKey = ext + '/' + QCoreApplication::applicationName() + ".backup";
	//.mp3/Default
	QString extValue(hkcr.value(extKey).toString());
	///

	QString backupValue(hkcr.value(backupKey).toString());
	if (backupValue != appKey) {
		//Normal case: restores the previous association
		hkcr.setValue(extKey, backupValue);
	} else {
		//backupValue == appKey
		//or backupValue is empty
		//This means there were no other association with .mp3 from other application
		//In this case, let's remove .mp3/Default
		hkcr.remove(extKey);
	}

	//Removes .mp3/QuarkPlayer.backup
	hkcr.remove(backupKey);

	//Removes QuarkPlayer.mp3
	hkcr.remove(appKey);
}

bool WinDefaultApplication::containsFileAssociation(const QString & extension) {
	//
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	//mp3 -> .mp3
	QString ext(extension);
	ext.prepend('.');
	//QuarkPlayer.mp3
	QString appKey = QCoreApplication::applicationName() + ext;
	//.mp3
	QString extKey = ext + DEFAULT_STR;
	//.mp3/QuarkPlayer.backup
	QString backupKey = ext + '/' + QCoreApplication::applicationName() + ".backup";
	//.mp3/Default
	QString extValue(hkcr.value(extKey).toString());
	///

	return (extValue == appKey);
}

void WinDefaultApplication::addDirectoryContextMenu() {
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	hkcr.setValue("Directory/shellex/ContextMenuHandlers/QuarkPlayer" + QString(DEFAULT_STR), SHELLEX_GUUID);
}

void WinDefaultApplication::deleteDirectoryContextMenu() {
	QSettings hkcr("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
	hkcr.remove("Directory/shellex/ContextMenuHandlers/QuarkPlayer");
}

void WinDefaultApplication::addDefaultFileAssociations() {
	QStringList extensions;
	extensions += FileTypes::extensions(FileType::Video);
	extensions += FileTypes::extensions(FileType::Audio);
	extensions += FileTypes::extensions(FileType::Playlist);
	//extensions += FileTypes::extensions(FileType::Subtitle);

	foreach (QString extension, extensions) {
		addFileAssociation(extension);
	}

	addDirectoryContextMenu();

	//Update Windows file associations
	notifyFileAssociationChanged();
}

void WinDefaultApplication::deleteAllFileAssociations() {
	QStringList extensions;
	extensions += FileTypes::extensions(FileType::Video);
	extensions += FileTypes::extensions(FileType::Audio);
	extensions += FileTypes::extensions(FileType::Subtitle);
	extensions += FileTypes::extensions(FileType::Playlist);

	foreach (QString extension, extensions) {
		deleteFileAssociation(extension);
	}

	deleteDirectoryContextMenu();

	//Update Windows file associations
	notifyFileAssociationChanged();
}

#ifdef Q_OS_WIN
	#include <shlobj.h>
#endif	//Q_OS_WIN

void WinDefaultApplication::notifyFileAssociationChanged() {
#ifdef Q_OS_WIN
	//Calling SHChangeNotify() will update the file association icons
	//in case they had been reset.
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
#endif	//Q_OS_WIN
}
