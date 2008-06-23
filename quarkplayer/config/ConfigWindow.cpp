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
#include "BackendCapabilitiesWidget.h"

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/TkStackedWidget.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

ConfigWindow::ConfigWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::ConfigWindow();
	_ui->setupUi(this);

	RETRANSLATE(this);

	_lastConfigWindowOpenedIndex = 0;

	//Add all config panels/widgets to the list
	_configWidgetList += new GeneralConfigWidget(this);
	_configWidgetList += new BackendCapabilitiesWidget(this);
	_configWidgetList += new SettingsBrowser(this);
	//_configWidgetList += new FileBrowserConfigWidget(this);

	//listWidget
	connect(_ui->listWidget, SIGNAL(currentRowChanged(int)),
		SLOT(showConfigWidget(int)));

	populateStackedWidget();

	//Select the first (top one) config panel/widget from the list
	_ui->listWidget->setCurrentRow(0);
	showConfigWidget(0);

	//saveButton
	connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(saveConfig()));
}

ConfigWindow::~ConfigWindow() {
	delete _ui;
}

void ConfigWindow::populateStackedWidget() {
	//stackedWidget + read config for each config widget
	TkStackedWidget::removeAllWidgets(_ui->stackedWidget);
	_ui->listWidget->clear();
	foreach (IConfigWidget * configWidget, _configWidgetList) {
		configWidget->readConfig();
		QLayout * layout = configWidget->layout();
		if (layout) {
			layout->setMargin(0);
			layout->setSpacing(0);
		}
		_ui->stackedWidget->addWidget(configWidget);
		new QListWidgetItem(TkIcon(configWidget->iconName()), configWidget->name(), _ui->listWidget);
	}

	//listWidget width
	_ui->listWidget->setFixedWidth(computeListViewMinimumWidth(_ui->listWidget) + 30);
}

void ConfigWindow::showConfigWidget(int row) {
	if (row < 0 || row >= _configWidgetList.size()) {
		return;
	}

	IConfigWidget * configWidget = _configWidgetList.at(row);
	if (!configWidget) {
		qCritical() << __FUNCTION__ << "Error: configWidget cannot be NULL";
	}

	_configWidgetOpenedMap[configWidget] = _lastConfigWindowOpenedIndex;
	_lastConfigWindowOpenedIndex++;

	_ui->stackedWidget->setCurrentWidget(configWidget);
}

void ConfigWindow::saveConfig() {
	//Trick for not having saving conflicts between config widgets.
	//Saves the config widgets in the exact order they were last opened.
	for (int i = 0; i < _lastConfigWindowOpenedIndex; ++i) {
		IConfigWidget * configWidget = _configWidgetOpenedMap.key(i);
		if (configWidget) {
			configWidget->saveConfig();
		}
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

void ConfigWindow::retranslate() {
	_ui->retranslateUi(this);

	foreach (IConfigWidget * configWidget, _configWidgetList) {
		configWidget->retranslate();
	}

	populateStackedWidget();
}
