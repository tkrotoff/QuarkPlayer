/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TkFileDialog.h"

#include "config.h"

#ifdef KDE4_FOUND
	#include <KFileDialog>
#endif	//KDE4_FOUND

QString TkFileDialog::getExistingDirectory(QWidget * parent, const QString & caption, const QString & dir, QFileDialog::Options options) {
#ifdef KDE4_FOUND
	return KFileDialog::getExistingDirectory(dir, parent, caption);
#else
	return QFileDialog::getExistingDirectory(parent, caption, dir, options);
#endif	//KDE4_FOUND
}

QString TkFileDialog::getOpenFileName(QWidget * parent, const QString & caption,
		const QString & dir, const QString & filter,
		QString * selectedFilter, QFileDialog::Options options) {

#ifdef KDE4_FOUND
	return KFileDialog::getOpenFileName(dir, filter, parent, caption);
#else
	return QFileDialog::getOpenFileName(parent, caption, dir, filter, selectedFilter, options);
#endif	//KDE4_FOUND
}

QStringList TkFileDialog::getOpenFileNames(QWidget * parent, const QString & caption,
		const QString & dir, const QString & filter,
		QString * selectedFilter, QFileDialog::Options options) {

#ifdef KDE4_FOUND
	return KFileDialog::getOpenFileNames(dir, filter, parent, caption);
#else
	return QFileDialog::getOpenFileNames(parent, caption, dir, filter, selectedFilter, options);
#endif	//KDE4_FOUND
}

QString TkFileDialog::getSaveFileName(QWidget * parent, const QString & caption,
		const QString & dir, const QString & filter,
		QString * selectedFilter, QFileDialog::Options options) {

#ifdef KDE4_FOUND
	return KFileDialog::getSaveFileName(dir, filter, parent, caption);
#else
	return QFileDialog::getSaveFileName(parent, caption, dir, filter, selectedFilter, options);
#endif	//KDE4_FOUND
}
