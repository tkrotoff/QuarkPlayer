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

#ifndef WINFILEASSOCIATIONS_H
#define WINFILEASSOCIATIONS_H

#include <winfileassociations/winfileassociations_export.h>

#include <QtCore/QString>

class QSettings;

/**
 * Handles file associations with an application under Windows Vista/XP/2000.
 *
 * @author Tanguy Krotoff
 */
class WINFILEASSOCIATIONS_API WinFileAssociations {
public:

	/**
	 * Starts associating file extensions with an application.
	 *
	 * @param applicationName application name, example: QuarkPlayer
	 */
	WinFileAssociations(const QString & applicationName);

	~WinFileAssociations();

	/**
	 * Adds a file association.
	 *
	 * @param extension file extension to associates with the application
	 */
	void addAssociation(const QString & extension);

	/**
	 * Removes a file association.
	 *
	 * @param extension file extension to remove from being associated with the application
	 */
	void deleteAssociation(const QString & extension);

	/**
	 * Checks if the file extension is associated with the given application.
	 *
	 * @param extension file extension to check
	 */
	bool isAssociated(const QString & extension);

private:

	bool checkError();

	QString backupKey(const QString & extension) const;

	QSettings * _hkcr;

	/** Application name, example: QuarkPlayer. */
	QString _applicationName;
};

#endif	//WINFILEASSOCIATIONS_H
