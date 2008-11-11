/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
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

#ifndef FILECHOOSERWINDOW_H
#define FILECHOOSERWINDOW_H

#include <QtGui/QDialog>

namespace Ui { class FileChooserWindow; }

class QListWidgetItem;

class FileChooserWindow : public QDialog {
	Q_OBJECT
public:

	FileChooserWindow(QWidget * parent);

	~FileChooserWindow();

	void addFiles(const QStringList & files);

	QStringList selectedFiles();

private slots:

	void retranslate();

	void selectAll();

	void selectNone();

	void itemClicked(QListWidgetItem * item);

	void itemPressed(QListWidgetItem * item);

private:

	Ui::FileChooserWindow * _ui;

	Qt::CheckState _currentItemCheckState;
};

#endif	//FILECHOOSERWINDOW_H
