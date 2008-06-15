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

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include <QtGui/QFileDialog>

/**
 * Provides a dialog that allow users to select files or directories.
 *
 * Internally selects either QFileDialog or KFileDialog.
 *
 * @see QFileDialog
 * @see KFileDialog
 * @author Tanguy Krotoff
 */
class FileDialog {
public:

	/** Follows Qt API. */
	static QString getOpenFileName(QWidget * parent = 0, const QString & caption = QString(),
		const QString & dir = QString(), const QString & filter = QString(),
		QString * selectedFilter = 0, QFileDialog::Options options = 0);

private:

	FileDialog();
};

#endif	//FILEDIALOG_H
