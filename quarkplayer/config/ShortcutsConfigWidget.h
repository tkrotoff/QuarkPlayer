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

#ifndef SHORTCUTSCONFIGWIDGET_H
#define SHORTCUTSCONFIGWIDGET_H

#include "IConfigWidget.h"

#include <QtCore/QString>

namespace Ui { class ShortcutsConfigWidget; }

class QTreeWidgetItem;

struct ShortcutItem {
	QAction * action;
	QList<QKeySequence> shortcuts;
	QTreeWidgetItem * treeItem;
};

/**
 * Shortcuts configuration widget.
 *
 * Source code taken/inspired by shortcutsettings.cpp from Qt Creator 0.9.2 (beta).
 *
 * @author Tanguy Krotoff
 */
class ShortcutsConfigWidget : public IConfigWidget {
	Q_OBJECT
public:

	ShortcutsConfigWidget();

	~ShortcutsConfigWidget();

	QString name() const;

	QString iconName() const;

	void readConfig();

	void saveConfig();

	static QString toString(const QList<QKeySequence> & shortcuts);

	static QList<QKeySequence> fromString(const QString & shortcuts);

private slots:

	void actionChanged(QTreeWidgetItem * currentItem);

	void searchChanged(const QString & pattern);

	void shortcutChanged();

	void resetShortcuts();
	void removeShortcuts();

	void loadShortcuts();
	void saveShortcuts();

	void revertShortcutsToDefault();

	void retranslate();

private:

	bool eventFilter(QObject * object, QEvent * event);

	void setShortcuts(const QList<QKeySequence> & shortcuts);

	bool search(const QString & pattern, const QTreeWidgetItem * item);

	void handleKeyEvent(QKeyEvent * event);

	int translateModifiers(Qt::KeyboardModifiers state, const QString & text);

	QList<ShortcutItem *> _shortcutItems;

	int _key[4];
	int _keyNum;

	Ui::ShortcutsConfigWidget * _ui;
};

#endif	//SHORTCUTSCONFIGWIDGET_H
