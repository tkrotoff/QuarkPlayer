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

#ifndef TKACTION_H
#define TKACTION_H

#include <tkutil/tkutil_export.h>

#include <QtGui/QAction>
#include <QtGui/QKeySequence>

/**
 * A TkAction is basically a QAction.
 *
 * This class was created because QAction does not provide a "setDefaultShortcut()" method.
 * KAction from KDElibs does provide such a function.
 *
 * @see QAction
 * @see KAction
 * @author Tanguy Krotoff
 */
class TKUTIL_API TkAction : public QAction {
	Q_OBJECT
public:

	explicit TkAction(QObject * parent,
			const QKeySequence & shortcut0 = QKeySequence(),
			const QKeySequence & shortcut1 = QKeySequence(),
			const QKeySequence & shortcut2 = QKeySequence(),
			const QKeySequence & shortcut3 = QKeySequence());
	TkAction(const QString & text, QObject * parent,
			const QKeySequence & shortcut0 = QKeySequence(),
			const QKeySequence & shortcut1 = QKeySequence(),
			const QKeySequence & shortcut2 = QKeySequence(),
			const QKeySequence & shortcut3 = QKeySequence());
	TkAction(const QIcon & icon, const QString & text, QObject * parent,
			const QKeySequence & shortcut0 = QKeySequence(),
			const QKeySequence & shortcut1 = QKeySequence(),
			const QKeySequence & shortcut2 = QKeySequence(),
			const QKeySequence & shortcut3 = QKeySequence());

	~TkAction();

	void setDefaultShortcuts(const QList<QKeySequence> & shortcuts);
	void setDefaultShortcuts(
			const QKeySequence & shortcut0,
			const QKeySequence & shortcut1 = QKeySequence(),
			const QKeySequence & shortcut2 = QKeySequence(),
			const QKeySequence & shortcut3 = QKeySequence()
	);
	QList<QKeySequence> defaultShortcuts() const;

private:

	QList<QKeySequence> _defaultShortcuts;
};

#endif	//TKACTION_H
