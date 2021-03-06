/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ICONFIGWIDGET_H
#define ICONFIGWIDGET_H

#include <quarkplayer-plugins/ConfigWindow/ConfigWindowExport.h>

#include <QtGui/QWidget>

class QString;

/**
 * Interface for settings panel from the configuration window.
 *
 * @author Tanguy Krotoff
 */
class CONFIGWINDOW_API IConfigWidget : public QWidget {
	Q_OBJECT
public:

	virtual ~IConfigWidget() { }

	virtual QString name() const = 0;

	virtual QString iconName() const = 0;

	virtual void readConfig() = 0;

	virtual void saveConfig() = 0;

	virtual void retranslate() = 0;
};

#endif	//ICONFIGWIDGET_H
