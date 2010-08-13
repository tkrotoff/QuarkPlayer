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

#ifndef FILECHOOSERWIDGET_H
#define FILECHOOSERWIDGET_H

#include <TkUtil/TkUtilExport.h>

#include <QtGui/QWidget>

class QLineEdit;
class QToolButton;

/**
 * Widget to choose a file or a directory in a nice way.
 *
 * Code factorization: agregates a QLineEdit with a QCompleter
 * and a button with an icon.
 *
 * @see QCompleter
 * @author Tanguy Krotoff
 */
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
		/** FileChooser is a file chooser. */
		DialogTypeFile,

		/** FileChooser is a directory chooser. */
		DialogTypeDir
	};

	/**
	 * Sets the type of dialog FileChooser should be.
	 *
	 * @param dialogType type of dialog
	 */
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

	/** Button to open a file or a directory. */
	QToolButton * _openFileButton;
};

#endif	//FILECHOOSERWIDGET_H
