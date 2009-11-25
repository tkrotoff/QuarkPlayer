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

#ifndef SHORTCUTSCONFIG_H
#define SHORTCUTSCONFIG_H

#include <tkutil/Singleton.h>

class QAction;
class QString;

/**
 * Load and save QuarkPlayer shortcuts configuration using QSettings.
 *
 * @see Config
 * @author Tanguy Krotoff
 */
class ShortcutsConfig : public Singleton<ShortcutsConfig> {
	friend class Singleton<ShortcutsConfig>;
public:

	void load() const;

	void save();

private:

	QAction * findAction(const QString & name) const;

	ShortcutsConfig();

	~ShortcutsConfig();
};

#endif	//SHORTCUTSCONFIG_H
