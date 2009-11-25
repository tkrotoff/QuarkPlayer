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

#ifndef TKFILEDIALOG_H
#define TKFILEDIALOG_H

#include <tkutil/tkutil_export.h>

#include <QtGui/QFileDialog>

/**
 * Thin class above QFileDialog/KFileDialog.
 *
 * @see QFileDialog
 * @see KFileDialog
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkFileDialog {
public:

	/** Follows Qt API. */
	static QString getExistingDirectory(QWidget * parent = 0, const QString & caption = QString(),
		const QString & dir = QString(), QFileDialog::Options options = QFileDialog::ShowDirsOnly);

	/** Follows Qt API. */
	static QString getOpenFileName(QWidget * parent = 0, const QString & caption = QString(),
		const QString & dir = QString(), const QString & filter = QString(),
		QString * selectedFilter = 0, QFileDialog::Options options = 0);

	/** Follows Qt API. */
	static QStringList getOpenFileNames(QWidget * parent = 0, const QString & caption = QString(),
		const QString & dir = QString(), const QString & filter = QString(),
		QString * selectedFilter = 0, QFileDialog::Options options = 0);

	/** Follows Qt API. */
	static QString getSaveFileName(QWidget * parent = 0, const QString & caption = QString(),
		const QString & dir = QString(), const QString & filter = QString(),
		QString * selectedFilter = 0, QFileDialog::Options options = 0);

private:

	TkFileDialog();
};

#endif	//TKFILEDIALOG_H
