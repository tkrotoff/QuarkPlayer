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

#include "ConfigWindow.h"

#include "ui_ConfigWindow.h"

#include "GeneralConfigWidget.h"
#include "SettingsBrowser.h"

#include <QtGui/QtGui>

ConfigWindow::ConfigWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::ConfigWindow();
	_ui->setupUi(this);

	//Add all config panels/widgets to the list
	_configWidgetList += new GeneralConfigWidget(this);
	_configWidgetList += new SettingsBrowser(this);

	//listWidget
	connect(_ui->listWidget, SIGNAL(currentRowChanged(int)),
		SLOT(showConfigWidget(int)));

	//stackedWidget
	foreach (IConfigWidget * configWidget, _configWidgetList) {
		_ui->stackedWidget->addWidget(configWidget);
		QString iconName = QString(":pics/config/%1.png").arg(configWidget->iconName());
		QIcon icon = QPixmap(iconName);
		new QListWidgetItem(icon, configWidget->name(), _ui->listWidget);
	}

	//Select the first (top one) config panel/widget from the list
	_ui->listWidget->setCurrentRow(0);
	showConfigWidget(0);

	//listWidget width
	_ui->listWidget->setFixedWidth(computeListViewMinimumWidth(_ui->listWidget) + 20);

	//saveButton
	connect(_ui->buttonBox, SIGNAL(accepted()),
		SLOT(saveConfig()));
}

ConfigWindow::~ConfigWindow() {
	delete _ui;
}

void ConfigWindow::showConfigWidget(int row) {
	if (row < 0 || row >= _configWidgetList.size()) {
		return;
	}

	IConfigWidget * configWidget = _configWidgetList.at(row);
	_ui->stackedWidget->setCurrentWidget(configWidget);
}

void ConfigWindow::saveConfig() {
	foreach (IConfigWidget * configWidget, _configWidgetList) {
		configWidget->saveConfig();
	}
}

int ConfigWindow::computeListViewMinimumWidth(QAbstractItemView * view) {
	int minWidth = 0;
	QAbstractItemModel * model = view->model();

	//Too bad view->viewOptions() is protected
	//FIXME: Handle the case where text is below icon
	QStyleOptionViewItem option;
	option.decorationSize = view->iconSize();

	int rowCount = model->rowCount();
	for (int row = 0; row < rowCount; ++row) {
		QModelIndex index = model->index(row, 0);
		QSize size = view->itemDelegate()->sizeHint(option, index);
		minWidth = qMax(size.width(), minWidth);
	}
	minWidth += 2 * view->frameWidth();
	return minWidth;
}
