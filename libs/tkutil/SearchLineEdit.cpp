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

#include "SearchLineEdit.h"

#include <tkutil/TkIcon.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

SearchLineEdit::SearchLineEdit(const QStringList & wordList, QWidget * parent)
	: LineEdit(parent) {

	//For click message
	_enableClickMessage = false;
	_drawClickMessage = false;

	//Clear button
	_clearButton = new QToolButton(NULL);
	_clearButton->setCursor(Qt::ArrowCursor);
	_clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	_clearButton->hide();
	connect(_clearButton, SIGNAL(clicked()), SLOT(clear()));
	connect(this, SIGNAL(textChanged(const QString &)), SLOT(updateClearButton(const QString &)));

	//Show word list button
	_showWordListButton = new QToolButton(NULL);
	_showWordListButton->setCursor(Qt::ArrowCursor);
	_showWordListButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	connect(_showWordListButton, SIGNAL(clicked()), SLOT(showWordList()));
	_showWordListButton->setEnabled(!wordList.isEmpty());
	addWidget(_showWordListButton, LeftSide);

	//Search completion
	_stringListModel = new QStringListModel();
	_stringListModel->setStringList(wordList);
	QCompleter * completer = new QCompleter(_stringListModel, this);
	completer->setCaseSensitivity(Qt::CaseInsensitive);
	setCompleter(completer);
}

SearchLineEdit::~SearchLineEdit() {
}

QToolButton * SearchLineEdit::clearButton() const {
	return _clearButton;
}

QToolButton * SearchLineEdit::showWordListButton() const {
	return _showWordListButton;
}

void SearchLineEdit::paintEvent(QPaintEvent * event) {
	LineEdit::paintEvent(event);

	if (_enableClickMessage && _drawClickMessage && !hasFocus() && text().isEmpty()) {
		QPainter p(this);
		//No italic, difficult to read
		//QFont f = font();
		//f.setItalic(true);
		//p.setFont(f);
		///

		p.setPen(palette().color(QPalette::Disabled, QPalette::Text));

		//FIXME: fugly alert!
		// qlineedit uses an internal qstyleoption set to figure this out
		// and then adds a hardcoded 2 pixel interior to that.
		// probably requires fixes to Qt itself to do this cleanly
		// see define horizontalMargin 2 in qlineedit.cpp
		QStyleOptionFrame opt;
		initStyleOption(&opt);
		QRect cr = style()->subElementRect(QStyle::SE_LineEditContents, &opt, this);
		cr.setLeft(cr.left() + 2);
		cr.setRight(cr.right() - 2);

		p.drawText(cr, Qt::AlignLeft | Qt::AlignVCenter, _clickMessage);
	}
}

void SearchLineEdit::focusInEvent(QFocusEvent * event) {
	if (_enableClickMessage && _drawClickMessage) {
		_drawClickMessage = false;
		update();
	}
	LineEdit::focusInEvent(event);
}

void SearchLineEdit::focusOutEvent(QFocusEvent * event) {
	if (_enableClickMessage && text().isEmpty()) {
		_drawClickMessage = true;
		update();
	}
	LineEdit::focusOutEvent(event);
}

void SearchLineEdit::setClickMessage(const QString & message) {
	_enableClickMessage = true;
	_clickMessage = message;
	_drawClickMessage = text().isEmpty();
	update();
}

void SearchLineEdit::setText(const QString & text) {
	if (_enableClickMessage) {
		_drawClickMessage = text.isEmpty();
		update();
	}
	LineEdit::setText(text);
}

void SearchLineEdit::updateClearButton(const QString & text) {
	if (!text.isEmpty()) {
		addWidget(_clearButton, RightSide);
		_clearButton->show();
	} else {
		removeWidget(_clearButton);
		_clearButton->hide();
	}
}

void SearchLineEdit::showWordList() {
	setFocus();
	//Empty string means show the entire list
	completer()->setCompletionPrefix(QString());
	completer()->complete();
}

QStringList SearchLineEdit::wordList() const {
	return _stringListModel->stringList();
}

void SearchLineEdit::addWord(const QString & word) {
	QStringList wordList(_stringListModel->stringList());
	if (!wordList.contains(word, Qt::CaseInsensitive)) {
		wordList.prepend(word);
	} else {
		//'word' already contained inside the QStringListModel
		//Let's make it first in the list and remove the all previous 'word' occurrences
		wordList.removeAll(word);
		wordList.prepend(word);
	}
	_stringListModel->setStringList(wordList);

	_showWordListButton->setEnabled(!wordList.isEmpty());
}
