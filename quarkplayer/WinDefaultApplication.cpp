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

#include <winfileassociations/WinFileAssociations.h>

#include <QtCore/QSysInfo>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

WinDefaultApplication::WinDefaultApplication() {
}

WinDefaultApplication::~WinDefaultApplication() {
}

void WinDefaultApplication::install() {
	qDebug() << __FUNCTION__ << "Install QuarkPlayer as the default media application";
	registerAsDefaultMediaPlayer();
	addDefaultFileAssociations();
}

void WinDefaultApplication::registerAsDefaultMediaPlayer() {
	//Register QuarkPlayer as the default media player under Windows
	//See http://msdn.microsoft.com/en-us/library/bb776851(VS.85).aspx
	//for full explanations
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

	static QString defaultMediaPlayerKey("Software/Clients/Media/" + QCoreApplication::applicationName());

	static const QString defaultStr("/Default");
	hklm.setValue(defaultMediaPlayerKey + defaultStr, QCoreApplication::applicationName());
	hklm.setValue(defaultMediaPlayerKey + "/DefaultIcon" + defaultStr, applicationFilePath() + ",0");
	hklm.setValue(defaultMediaPlayerKey + "/shell/open/command" + defaultStr, applicationFilePath());
	static const QString unregisterCmd(applicationFilePath() + " --windows-uninstall");
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/HideIconsCommand", unregisterCmd);
	static const QString registerCmd(applicationFilePath() + " --windows-install");
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/ReinstallCommand", registerCmd);
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/ShowIconsCommand", registerCmd);
	hklm.setValue(defaultMediaPlayerKey + "/InstallInfo/IconsVisible", 1);
}

void WinDefaultApplication::addFileAssociation(const QString & extension, bool addPlayContextMenu, bool addEnqueueContextMenu) {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());

	if (extension.isEmpty()) {
		qWarning() << __FUNCTION__ << "Error: extension is empty";
		return;
	}

	if (FileTypes::extensions(FileType::Subtitle).contains(extension, Qt::CaseInsensitive)) {
		//Do not change icon nor default application for subtitle files
		return;
	}

#ifdef Q_OS_WIN
	QString icon;
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

	//File association
	winFileAssociations.addCommand(
			extension,
			"Play",
			applicationFilePath() + " \"%1\"",
			QString(),
			icon
	);

	//File context menu
	if (addPlayContextMenu) {
		winFileAssociations.addCommand(
				extension,
				"QuarkPlayer.Play",
				applicationFilePath() + " \"%1\"",
				tr("Play with QuarkPlayer"),
				QString()
		);
	}
	if (addEnqueueContextMenu) {
		winFileAssociations.addCommand(
				extension,
				"QuarkPlayer.Enqueue",
				applicationFilePath() + " --playlist-enqueue \"%1\"",
				tr("Enqueue in QuarkPlayer"),
				QString()
		);
	}
}

void WinDefaultApplication::addPlayDirectoryContextMenu() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());
	winFileAssociations.addDirectoryCommand("QuarkPlayer.Play", applicationFilePath() + " \"%1\"", tr("Play with QuarkPlayer"));
}

void WinDefaultApplication::deletePlayDirectoryContextMenu() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());
	winFileAssociations.deleteDirectoryCommand("QuarkPlayer.Play");
}

void WinDefaultApplication::addEnqueueDirectoryContextMenu() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());
	winFileAssociations.addDirectoryCommand("QuarkPlayer.Enqueue", applicationFilePath() + " --playlist-enqueue \"%1\"", tr("Enqueue in QuarkPlayer"));
}

void WinDefaultApplication::deleteEnqueueDirectoryContextMenu() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());
	winFileAssociations.deleteDirectoryCommand("QuarkPlayer.Enqueue");
}

void WinDefaultApplication::addDefaultFileAssociations() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());

	QStringList extensions;
	extensions += FileTypes::extensions(FileType::Video);
	extensions += FileTypes::extensions(FileType::Audio);
	extensions += FileTypes::extensions(FileType::Playlist);
	//extensions += FileTypes::extensions(FileType::Subtitle);

	foreach (QString extension, extensions) {
		addFileAssociation(extension, true, true);
	}

	addPlayDirectoryContextMenu();
	addEnqueueDirectoryContextMenu();

	//Update Windows file associations
	WinFileAssociations::notifyFileAssociationChanged();
}

void WinDefaultApplication::uninstall() {
	qDebug() << __FUNCTION__ << "Uninstall QuarkPlayer as the default media application";
	unregisterAsDefaultMediaPlayer();
	deleteAllFileAssociations();
}

void WinDefaultApplication::unregisterAsDefaultMediaPlayer() {
	//Detele the registry keys associated with "Set Programs Access and Defaults" (SPAD)
	QSettings hklm("HKEY_LOCAL_MACHINE", QSettings::NativeFormat);

	static QString defaultMediaPlayerKey("Software/Clients/Media/" + QCoreApplication::applicationName());

	//Removes HKEY_LOCAL_MACHINE/Software/Clients/Media/QuarkPlayer
	hklm.remove(defaultMediaPlayerKey);
}

void WinDefaultApplication::deleteAllFileAssociations() {
	WinFileAssociations winFileAssociations(QCoreApplication::applicationName());

	QStringList extensions;
	extensions += FileTypes::extensions(FileType::Video);
	extensions += FileTypes::extensions(FileType::Audio);
	extensions += FileTypes::extensions(FileType::Subtitle);
	extensions += FileTypes::extensions(FileType::Playlist);

	foreach (QString extension, extensions) {
		winFileAssociations.deleteAssociation(extension);
	}

	//Update Windows file associations
	WinFileAssociations::notifyFileAssociationChanged();
}

static const QString quote("\"");

QString WinDefaultApplication::applicationFilePath() {
	static const QString filePath(quote + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + quote);
	return filePath;
}
