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

#ifndef SETTINGSBROWSER_H
#define SETTINGSBROWSER_H

#include "IConfigWidget.h"

#include <QtGui/QDialog>

namespace Ui { class SettingsBrowser; }

class QVariant;

/**
 * QSettings browser.
 *
 * Works the same way about:config for Firefox.
 *
 * SettingsBrowser is very generic and relies only on Config class.
 * TODO change SettingsBrowser so that it only relies on IConfig class?
 *
 * @see Config
 * @author Tanguy Krotoff
 */
class SettingsBrowser : public IConfigWidget {
	Q_OBJECT
public:

	SettingsBrowser(QWidget * parent);

	~SettingsBrowser();

	QString name() const;

	QString iconName() const;

	void saveConfig();

private:

	void readConfig();

	void setItem(const QVariant & defaultValue, const QVariant & value, int row, int column);

	QVariant item(const QVariant & defaultValue, int row, int column);

	Ui::SettingsBrowser * _ui;
};

#endif	//SETTINGSBROWSER_H
