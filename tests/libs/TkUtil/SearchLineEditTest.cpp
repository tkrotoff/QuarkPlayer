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

#include "SearchLineEditTest.h"

#include <QtGui/QtGui>

#include <TkUtil/SearchLineEdit.h>

QTEST_MAIN(SearchLineEditTest)

void SearchLineEditTest::initTestCase() {
	_originalSearchHistory << "Hello";
	_originalSearchHistory << "Bonjour";
	_originalSearchHistory << "Ciao";

	_searchLineEdit = new SearchLineEdit(_originalSearchHistory);
}

void SearchLineEditTest::cleanupTestCase() {
	delete _searchLineEdit;
}

void SearchLineEditTest::testGui() {
	//Test word list
	QCOMPARE(_searchLineEdit->wordList(), _originalSearchHistory);
	QStringList searchHistory;
	searchHistory << "Hallo";
	searchHistory << "Hola";
	searchHistory << _originalSearchHistory;
	_searchLineEdit->addWord("Hola");
	_searchLineEdit->addWord("Hallo");
	QCOMPARE(_searchLineEdit->wordList(), searchHistory);
	//

	//Test click message
	QString clickMessage("Grayed-out text");
	_searchLineEdit->setClickMessage(clickMessage);
	QCOMPARE(_searchLineEdit->clickMessage(), clickMessage);
	///

	//Test normal text
	QString normalText("Normal black text");
	_searchLineEdit->setText(normalText);
	QCOMPARE(_searchLineEdit->text(), normalText);
	///
}

void SearchLineEditTest::showGui() {
	QWidget * window = new QWidget();
	QHBoxLayout * layout = new QHBoxLayout();

	_searchLineEdit->setToolTip("Search, use whitespaces to separate words");

	layout->addWidget(_searchLineEdit);

	QLabel * textLabel = new QLabel("Text entered inside SearchLineEdit");
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)),
		textLabel, SLOT(setText(const QString &)));
	layout->addWidget(textLabel);

	QPushButton * addWordButton = new QPushButton("Add Word");
	connect(addWordButton, SIGNAL(clicked()),
		SLOT(addCurrentTextAsWord()));
	layout->addWidget(addWordButton);

	QPushButton * closeButton = new QPushButton("Close");
	connect(closeButton, SIGNAL(clicked()),
		window, SLOT(close()));
	layout->addWidget(closeButton);

	window->setLayout(layout);
	window->show();
	QApplication::exec();
}

void SearchLineEditTest::addCurrentTextAsWord() {
	_searchLineEdit->addWord(_searchLineEdit->text());
}
