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

#include "SearchToolBar.h"

#include "UuidActionCollection.h"

#include <tkutil/TkIcon.h>
#include <tkutil/ClickMessageLineEdit.h>
#include <tkutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

SearchToolBar::SearchToolBar(QWidget * parent)
	: QToolBar(parent) {

	populateActionCollection();

	setIconSize(QSize(16, 16));

	_searchLineEdit = new ClickMessageLineEdit();
	connect(_searchLineEdit, SIGNAL(textChanged(const QString &)), SLOT(textChanged()));

	_clearSearchButton = new QToolButton();
	_clearSearchButton->setAutoRaise(true);
	_clearSearchButton->setEnabled(false);
	_clearSearchButton->setDefaultAction(uuidAction("searchToolBarClearSearch"));
	connect(_clearSearchButton, SIGNAL(clicked()), _searchLineEdit, SLOT(clear()));

	RETRANSLATE(this);
	retranslate();
}

SearchToolBar::~SearchToolBar() {
}

QString SearchToolBar::text() const {
	return _searchLineEdit->text().trimmed();
}

void SearchToolBar::addSearchWidgets() {
	addWidget(_searchLineEdit);
	addWidget(_clearSearchButton);
}

void SearchToolBar::textChanged() {
	_clearSearchButton->setEnabled(!text().isEmpty());
	emit textChanged(text());
}

void SearchToolBar::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();

	addUuidAction("searchToolBarClearSearch", new QAction(app));
}

void SearchToolBar::retranslate() {
	uuidAction("searchToolBarClearSearch")->setText(tr("Clear Search"));
	uuidAction("searchToolBarClearSearch")->setIcon(TkIcon("edit-delete"));

	_searchLineEdit->setToolTip(tr("Search files, use whitespaces to separate words"));
	_searchLineEdit->setClickMessage(tr("Enter search terms here"));
	QString pattern(_searchLineEdit->text().trimmed());
	_clearSearchButton->setEnabled(!pattern.isEmpty());

	setMinimumSize(sizeHint());
}
