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
#include "PluginsConfigWidget.h"
#include "WinFileAssociationsConfigWidget.h"

#include <tkutil/TkIcon.h>
#include <tkutil/LanguageChangeEventFilter.h>
#include <tkutil/TkStackedWidget.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

QList<ConfigWindow::ConfigWidget> ConfigWindow::_configWidgetList;

static const int COLUMN = 0;

ConfigWindow::ConfigWindow(QWidget * parent)
	: QDialog(parent) {

	_ui = new Ui::ConfigWindow();
	_ui->setupUi(this);

	RETRANSLATE(this);

	_lastConfigWindowOpenedIndex = 0;

	//Add all config panels/widgets to the list
	_configWidgetList.prepend(ConfigWidget(new PluginsConfigWidget()));
	_configWidgetList.prepend(ConfigWidget(new SettingsBrowser()));
	_configWidgetList.prepend(ConfigWidget(new WinFileAssociationsConfigWidget()));
	_configWidgetList.prepend(ConfigWidget(new BackendCapabilitiesWidget()));
	_configWidgetList.prepend(ConfigWidget(new GeneralConfigWidget()));

	//treeWidget item activated/clicked
	connect(_ui->treeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
		SLOT(showConfigWidget(QTreeWidgetItem *)));
	connect(_ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
		SLOT(showConfigWidget(QTreeWidgetItem *)));

	populateStackedWidget();

	_ui->treeWidget->setHeaderHidden(true);

	//saveButton
	connect(_ui->buttonBox, SIGNAL(accepted()), SLOT(saveConfig()));
}

ConfigWindow::~ConfigWindow() {
}

void ConfigWindow::addConfigWidget(IConfigWidget * configWidget) {
	_configWidgetList += ConfigWidget(configWidget, true);
}

void ConfigWindow::populateStackedWidget() {
	static QTreeWidgetItem * pluginsItem = NULL;
	static bool firstTime = true;

	//stackedWidget + read config for each config widget
	TkStackedWidget::removeAllWidgets(_ui->stackedWidget);
	_ui->treeWidget->clear();
	foreach (ConfigWidget tmp, _configWidgetList) {
		tmp.configWidget->readConfig();
		QLayout * layout = tmp.configWidget->layout();
		if (layout) {
			layout->setMargin(0);
			//layout->setSpacing(0);
		}
		_ui->stackedWidget->addWidget(tmp.configWidget);
		QTreeWidgetItem * item;
		if (tmp.isPlugin) {
			item = new QTreeWidgetItem(pluginsItem);
		} else {
			item = new QTreeWidgetItem(_ui->treeWidget);
		}
		item->setExpanded(true);
		item->setIcon(COLUMN, TkIcon(tmp.configWidget->iconName()));
		item->setText(COLUMN, tmp.configWidget->name());

		if (qobject_cast<PluginsConfigWidget *>(tmp.configWidget)) {
			//Saves the QTreeWidgetItem for plugins
			//then we can add plugins to this item
			//This works because PluginsConfigWidget is added to the list before the plugins
			pluginsItem = item;
		}

		if (firstTime) {
			//Select the first item in the list
			firstTime = false;
			_ui->treeWidget->setCurrentItem(item);
			showConfigWidget(item);
		}
	}

	//treeWidget width
	_ui->treeWidget->setFixedWidth(computeListViewMinimumWidth(_ui->treeWidget) + 50);
}

void ConfigWindow::showConfigWidget(QTreeWidgetItem * item) {
	IConfigWidget * configWidget = NULL;
	foreach (ConfigWidget tmp, _configWidgetList) {
		if (tmp.configWidget->name() == item->text(COLUMN)) {
			//We found the associated config widget
			configWidget = tmp.configWidget;
			break;
		}
	}

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

	foreach (ConfigWidget tmp, _configWidgetList) {
		tmp.configWidget->retranslate();
	}

	populateStackedWidget();
}
