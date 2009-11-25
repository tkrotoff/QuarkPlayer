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

#include "TkAction.h"

TkAction::TkAction(QObject * parent,
	const QKeySequence & shortcut0,
	const QKeySequence & shortcut1,
	const QKeySequence & shortcut2,
	const QKeySequence & shortcut3)
	: QAction(parent) {

	setDefaultShortcuts(shortcut0, shortcut1, shortcut2, shortcut3);
}

TkAction::TkAction(const QString & text, QObject * parent,
	const QKeySequence & shortcut0,
	const QKeySequence & shortcut1,
	const QKeySequence & shortcut2,
	const QKeySequence & shortcut3)
	: QAction(text, parent) {

	setDefaultShortcuts(shortcut0, shortcut1, shortcut2, shortcut3);
}

TkAction::TkAction(const QIcon & icon, const QString & text, QObject * parent,
	const QKeySequence & shortcut0,
	const QKeySequence & shortcut1,
	const QKeySequence & shortcut2,
	const QKeySequence & shortcut3)
	: QAction(icon, text, parent) {

	setDefaultShortcuts(shortcut0, shortcut1, shortcut2, shortcut3);
}

TkAction::~TkAction() {
}

void TkAction::setDefaultShortcuts(const QList<QKeySequence> & shortcuts) {
	_defaultShortcuts = shortcuts;
	if (this->shortcuts().isEmpty()) {
		setShortcuts(_defaultShortcuts);
	}
}

void TkAction::setDefaultShortcuts(const QKeySequence & shortcut0, const QKeySequence & shortcut1, const QKeySequence & shortcut2, const QKeySequence & shortcut3) {
	QList<QKeySequence> shortcuts;
	if (!shortcut0.isEmpty()) {
		shortcuts += shortcut0;
	}
	if (!shortcut1.isEmpty()) {
		shortcuts += shortcut1;
	}
	if (!shortcut2.isEmpty()) {
		shortcuts += shortcut2;
	}
	if (!shortcut3.isEmpty()) {
		shortcuts += shortcut3;
	}
	setDefaultShortcuts(shortcuts);
}

QList<QKeySequence> TkAction::defaultShortcuts() const {
	return _defaultShortcuts;
}
