/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef FILECHOOSERWINDOW_H
#define FILECHOOSERWINDOW_H

#include <QtGui/QDialog>

namespace Ui { class FileChooserWindow; }

class QListWidgetItem;

/**
 * Provides a window with items to check/uncheck.
 *
 * In this case items are files.
 * I tried to make this class as generic as possible.
 * I don't know if it will be re-used one day...
 *
 * @author Tanguy Krotoff
 */
class FileChooserWindow : public QDialog {
	Q_OBJECT
public:

	FileChooserWindow(QWidget * parent);

	~FileChooserWindow();

	/**
	 * Add files inside the chooser window.
	 *
	 * @param files list of filenames that will be displayed inside the chooser window
	 */
	void addFiles(const QStringList & files);

	/** Change the default information text displayed at the top of the chooser window. */
	void setInformationText(const QString & text);

	/** Change the default Ok button text. */
	void setOkButtonText(const QString & text);

	/**
	 * Gets the files selected by the user.
	 *
	 * @return files selected if any
	 */
	QStringList selectedFiles() const;

private slots:

	void itemClicked(QListWidgetItem * item);

	void itemPressed(QListWidgetItem * item);

private:

	/** Enable the Ok button. */
	void enableOkButton();

	/** Disable the Ok button. */
	void disableOkButton();

	Ui::FileChooserWindow * _ui;

	Qt::CheckState _currentItemCheckState;
};

#endif	//FILECHOOSERWINDOW_H
