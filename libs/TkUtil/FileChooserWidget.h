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

#ifndef FILECHOOSERWIDGET_H
#define FILECHOOSERWIDGET_H

#include <tkutil/tkutil_export.h>

#include <QtGui/QWidget>

class QLineEdit;
class QToolButton;

class TKUTIL_API FileChooserWidget : public QWidget {
	Q_OBJECT
public:

	/**
	 * Constructs a FileChooserWidget.
	 *
	 * @see QFileDialog
	 */
	FileChooserWidget(QWidget * parent);

	~FileChooserWidget();

	/**
	 * Gets the path selected by the user.
	 *
	 * @return path: can be a directory or a fileName depending on DialogType.
	 */
	QString path() const;

	enum DialogType {
		DialogTypeFile,
		DialogTypeDir
	};

	void setDialogType(DialogType dialogType);

	/**
	 * Sets the filter used by the QFileDialog.
	 *
	 * @param filter something like
	 *        <pre>tr("Subtitle") + FileTypes::toFilterFormat(FileTypes::extensions(FileType::Subtitle)) + ";;" + tr("All Files") + " (*.*)"</pre>
	 *        used only if type DialogTypeFile
	 */
	void setFilter(const QString & filter);

	/**
	 * Sets the path that will be show inside the QLineEdit.
	 *
	 * @param path can be a directory or a fileName depending on DialogType.
	 */
	void setPath(const QString & path);

	/**
	 * Change icon for the search button.
	 *
	 * @param icon new search button icon
	 */
	void setSearchButtonIcon(const QIcon & icon);

	QLineEdit * lineEdit() const;

signals:

	/**
	 * The path has been changed by the user.
	 *
	 * @param path can be a directory or a fileName depending on DialogType.
	 */
	void pathChanged(const QString & path);

private slots:

	void search();

	void returnPressed();

	void textChanged(const QString & text);

private:

	DialogType _dialogType;

	QString _path;

	QString _filter;

	QLineEdit * _pathLineEdit;

	QToolButton * _searchButton;
};

#endif	//FILECHOOSERWIDGET_H
