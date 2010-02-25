/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#include <QtGui/QMainWindow>

class QTextEdit;

/**
 * Window showing a log file or any other simple text.
 *
 * This is used mostly for MPlayer and QuarkPlayer logs.
 *
 * @author Tanguy Krotoff
 */
class LogWindow : public QMainWindow {
	Q_OBJECT
public:

	LogWindow(QWidget * parent);

	~LogWindow();

	void setText(const QString & text);

public slots:

	void appendText(const QString & text);

private slots:

	void retranslate();

	void saveText();

private:

	void setupUi();

	void populateActionCollection();

	QToolBar * _toolBar;

	QTextEdit * _textEdit;
};

#endif	//LOGWINDOW_H
