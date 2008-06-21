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

#ifndef FILEBROWSERCONFIGWIDGET_H
#define FILEBROWSERCONFIGWIDGET_H

#include <quarkplayer/config/IConfigWidget.h>

namespace Ui { class FileBrowserConfigWidget; }

/**
 * File browser plugin configuration widget.
 *
 * @author Tanguy Krotoff
 */
class FileBrowserConfigWidget : public IConfigWidget {
	Q_OBJECT
public:

	FileBrowserConfigWidget(QWidget * parent);

	~FileBrowserConfigWidget();

	QString name() const;

	QString iconName() const;

	void readConfig();

	void saveConfig();

	void retranslate();

private slots:

	void browseDir();

private:

	Ui::FileBrowserConfigWidget * _ui;
};

#endif	//FILEBROWSERCONFIGWIDGET_H
