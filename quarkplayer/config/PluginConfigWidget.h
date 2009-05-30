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

#ifndef PLUGINCONFIGWIDGET_H
#define PLUGINCONFIGWIDGET_H

#include "IConfigWidget.h"

namespace Ui { class PluginConfigWidget; }

/**
 * Plugins configuration widget.
 *
 * @see PluginsManager
 * @see PluginData
 * @author Tanguy Krotoff
 */
class PluginConfigWidget : public IConfigWidget {
	Q_OBJECT
public:

	PluginConfigWidget();

	~PluginConfigWidget();

	QString name() const;

	QString iconName() const;

	void readConfig();

	void saveConfig();

	void retranslate();

private slots:

	void currentCellChanged(int row, int column);

private:

	Ui::PluginConfigWidget * _ui;
};

#endif	//PLUGINCONFIGWIDGET_H
