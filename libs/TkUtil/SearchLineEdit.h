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

#ifndef SEARCHLINEEDIT_H
#define SEARCHLINEEDIT_H

#include <TkUtil/tkutil_export.h>

#include <QtGui/QLineEdit>

#include <QtCore/QString>

class QToolButton;
class QStringListModel;

/**
 * Special QLineEdit for searches.
 *
 * QLineEdit with a tooltip that disappears when the user clicks on it.
 * Contains 2 buttons: one to clear the QLineEdit and one to list the previous searches.
 *
 * Inspired by KLineEdit.
 *
 * @see http://blog.forwardbias.in/2009/11/research-cleanup-part-2.html
 * @see http://labs.trolltech.com/blogs/2007/06/06/lineedit-with-a-clear-button/
 * @see QLineEdit
 * @see KLineEdit
 * @author Tanguy Krotoff
 */
class TKUTIL_API SearchLineEdit : public QLineEdit {
	Q_OBJECT
public:

	SearchLineEdit(QWidget * parent = 0);

	/** Creates a SearchLineEdit given a list of words. */
	SearchLineEdit(const QStringList & wordList, QWidget * parent = 0);

	~SearchLineEdit();

	/** Gets the list of previous searches. */
	QStringList wordList() const;

	/** Gets the clear button. */
	QToolButton * clearButton() const;

	/** Gets the button that list the previous searches. */
	QToolButton * wordListButton() const;

public slots:

	/**
	 * This makes the line edit display a grayed-out hinting text as long as the user didn't enter any text.
	 * It is often used as indication about the purpose of the line edit.
	 */
	void setClickMessage(const QString & message);

	/** Add some standard text that won't disappear when clicking inside the QLineEdit. */
	void setText(const QString & text);

	/** Add a word to the list of words showed by the wordListButton. */
	void addWord(const QString & word);

private slots:

	/** Hide or show the clear button depending if the QLineEdit contains some text or not. */
	void updateClearButton(const QString & text);

	/** Shows the previous searches inside the wordListButton. */
	void showWordList();

private:

	void init(const QStringList & wordList);

	/** Compute the positions of both the clear and word list buttons. */
	void resizeEvent(QResizeEvent * event);

	/** Gray-out the QLineEdit text. */
	void paintEvent(QPaintEvent * event);

	void focusInEvent(QFocusEvent * event);

	void focusOutEvent(QFocusEvent * event);

	/** Button to clear current QLineEdit text. */
	QToolButton * _clearButton;

	/** Button that shows the previous searches. */
	QToolButton * _wordListButton;

	bool _enableClickMessage;

	QString _clickMessage;

	bool _drawClickMessage;

	QStringListModel * _stringListModel;
};

#endif	//SEARCHLINEEDIT_H
