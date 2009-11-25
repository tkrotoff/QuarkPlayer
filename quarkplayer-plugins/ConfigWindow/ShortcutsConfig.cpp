/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "ShortcutsConfig.h"

#include "ShortcutsConfigWidget.h"

#include <quarkplayer/config/Config.h>

#include <tkutil/TkAction.h>
#include <tkutil/ActionCollection.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

static const char * SETTINGS_GROUP = "Shortcuts";
static const char * SHORTCUT_ID = "ID";
static const char * SHORTCUT_KEYSEQUENCE = "KeySequence";

ShortcutsConfig::ShortcutsConfig() {
	//Config::instance().addKey(, );
}

ShortcutsConfig::~ShortcutsConfig() {
}

void ShortcutsConfig::load() const {
	QSettings & settings = Config::instance().settings();

	const int nbShortcuts = settings.beginReadArray(SETTINGS_GROUP);
	for (int i = 0; i < nbShortcuts; ++i) {
		settings.setArrayIndex(i);
		const QString sid = settings.value(QLatin1String(SHORTCUT_ID)).toString();
		const QList<QKeySequence> shortcuts = ShortcutsConfigWidget::fromString(settings.value(SHORTCUT_KEYSEQUENCE).toString());

		QAction * action = findAction(sid);
		if (action) {
			action->setShortcuts(shortcuts);
		}
	}

	settings.endArray();
}

QAction * ShortcutsConfig::findAction(const QString & name) const {
	QAction * result = NULL;

	QList<QAction *> actions = ActionCollection::actions();
	foreach (QAction * action, actions) {
		if (name == action->objectName()) {
			result = action;
			break;
		}
	}
	if (!result) {
		qCritical() << __FUNCTION__ << "Error: couldn't find action:" << name;
	}

	return result;
}

void ShortcutsConfig::save() {
	QSettings & settings = Config::instance().settings();

	settings.beginWriteArray(SETTINGS_GROUP);
	int count = 0;

	QList<QAction *> actions = ActionCollection::actions();
	foreach (QAction * action, actions) {
		QString name = action->objectName();
		QString shortcuts = ShortcutsConfigWidget::toString(action->shortcuts());
		TkAction * tkAction = qobject_cast<TkAction *>(action);
		if (tkAction) {
			QString defaultShortcuts = ShortcutsConfigWidget::toString(tkAction->defaultShortcuts());
			if (shortcuts != defaultShortcuts) {
				settings.setArrayIndex(count);
				settings.setValue(SHORTCUT_ID, name);
				settings.setValue(SHORTCUT_KEYSEQUENCE, shortcuts);
				count++;
			}
		}
	}

	settings.endArray();
}
