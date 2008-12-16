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

#ifndef SEARCHTOOLBAR_H
#define SEARCHTOOLBAR_H

#include <QtGui/QToolBar>

class ClickMessageLineEdit;

class QToolButton;

/**
 * Search toolbar.
 *
 * @author Tanguy Krotoff
 */
class SearchToolBar : public QToolBar {
	Q_OBJECT
public:

	SearchToolBar(QWidget * parent);

	~SearchToolBar();

	QString text() const;

	/**
	 * Adds the search widgets (line edit, clean button) to this toolbar.
	 *
	 * This function exists because we cannot implement a function like:
	 * void prependWidget(QWidget * widget)
	 *
	 * @see QToolBar::addWidget()
	 * @see QToolBar::insertWidget()
	 */
	void addSearchWidgets();

signals:

	void textChanged(const QString & text);

private slots:

	void textChanged();

	void retranslate();

private:

	void populateActionCollection();

	void createToolBar();

	ClickMessageLineEdit * _searchLineEdit;

	QToolButton * _clearSearchButton;
};

#endif	//SEARCHTOOLBAR_H
