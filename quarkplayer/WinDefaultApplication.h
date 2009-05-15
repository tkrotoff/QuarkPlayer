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

#ifndef WINDEFAULTAPPLICATION_H
#define WINDEFAULTAPPLICATION_H

//Needed for the Qt translation feature
#include <QtCore/QObject>

/**
 * Makes QuarkPlayer the Windows default application.
 *
 * See Windows XP "Set Programs Access and Defaults" (SPAD)
 * Windows Media Player and Winamp use this feature.
 *
 * Inherits from QObject in order to be translated.
 *
 * @see http://msdn.microsoft.com/en-us/library/bb776851(VS.85).aspx
 * @see http://msdn.microsoft.com/en-us/library/cc144162(VS.85).aspx
 * @author Tanguy Krotoff
 */
class WinDefaultApplication : public QObject {
public:

	/** Makes QuarkPlayer the default Windows application. */
	static void install();

	/** Unsets QuarkPlayer as the default Windows application, revert to the previous one. */
	static void uninstall();

	/** Adds a QuarkPlayer file association given a file extension. */
	static void addFileAssociation(const QString & extension);

	/** Deletes a QuarkPlayer file association given a file extension. */
	static void deleteFileAssociation(const QString & extension);

	/** Contains a QuarkPlayer file association given a file extension. */
	static bool containsFileAssociation(const QString & extension);

	/** Adds QuarkPlayer context menu on a directory (right-clic on a directory). */
	static void addDirectoryContextMenu();

	/** Removes QuarkPlayer context menu on a directory (right-clic on a directory). */
	static void deleteDirectoryContextMenu();

	/**
	 * Notifies Windows that the file type associations have changed.
	 *
	 * This will update the Windows desktop file associations.
	 * Call this method once the file associations have been changed.
	 */
	static void notifyFileAssociationChanged();

private:

	WinDefaultApplication();

	~WinDefaultApplication();

	/** Sets QuarkPlayer as the default Windows application. */
	static void registerAsDefaultMediaPlayer();

	/** Adds QuarkPlayer default file associations. */
	static void addDefaultFileAssociations();

	/** Unsets QuarkPlayer as the default Windows application. */
	static void unregisterAsDefaultMediaPlayer();

	/** Deletes all QuarkPlayer file associations. */
	static void deleteAllFileAssociations();

	static void registerShellExDLL();
	static void unregisterShellExDLL();
	static bool isShellExDLLRegistered();
};

#endif	//WINDEFAULTAPPLICATION_H
