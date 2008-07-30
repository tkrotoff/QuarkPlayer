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

#include "PluginsConfigWidget.h"

#include "ui_PluginsConfigWidget.h"

#include "Config.h"

#include <quarkplayer/PluginManager.h>

#include <QtGui/QtGui>

#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>

static const int FILENAME_COLUMN = 0;
static const int NAME_COLUMN = 1;
static const int VERSION_COLUMN = 2;
static const int ENABLE_COLUMN = 3;

PluginsConfigWidget::PluginsConfigWidget() {
	_ui = new Ui::PluginsConfigWidget();
	_ui->setupUi(this);

	_ui->tableWidget->resizeColumnToContents(ENABLE_COLUMN);
	_ui->tableWidget->setColumnWidth(NAME_COLUMN, 300);
	_ui->tableWidget->verticalHeader()->hide();

	connect(_ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), SLOT(cellDoubleClicked(int, int)));
}

PluginsConfigWidget::~PluginsConfigWidget() {
	delete _ui;
}

QString PluginsConfigWidget::name() const {
	return tr("Plugins");
}

QString PluginsConfigWidget::iconName() const {
	return "preferences-plugin";
}

void PluginsConfigWidget::saveConfig() {
	QStringList plugins;
	QString pluginsDir;
}

void PluginsConfigWidget::readConfig() {
	QDir pluginsDir(Config::instance().pluginsDir());
	QStringList pluginList = pluginsDir.entryList(QDir::Files);

	int row = 0;
	foreach (QString filename, pluginList) {
		if (row >= _ui->tableWidget->rowCount()) {
			_ui->tableWidget->insertRow(row);
		}

		_ui->tableWidget->setItem(row, FILENAME_COLUMN, new QTableWidgetItem(filename));

		PluginManager::PluginMap pluginMap = PluginManager::instance().pluginMap();
		QPluginLoader * loader = pluginMap.value(filename);

		bool loaded = loader->isLoaded();

		QTableWidgetItem * item = new QTableWidgetItem();
		if (loaded) {
			//Green
			item->setText("loaded");
			_ui->tableWidget->setItem(row, ENABLE_COLUMN, item);
			//iconLabel->setStyleSheet("background-color: rgb(0, 170, 0);");
		} else {
			//Red
			item->setText("");
			_ui->tableWidget->setItem(row, ENABLE_COLUMN, item);
			//iconLabelsetStyleSheet("background-color: rgb(255, 0, 0);");
		}

		row++;
	}

	_ui->tableWidget->resizeColumnsToContents();
	_ui->tableWidget->resizeRowsToContents();
}

void PluginsConfigWidget::cellDoubleClicked(int row, int column) {
	if (column != ENABLE_COLUMN) {
		return;
	}

	QTableWidgetItem * item = _ui->tableWidget->item(row, FILENAME_COLUMN);
	QString filename = item->text();
	PluginManager::PluginMap pluginMap = PluginManager::instance().pluginMap();
	QPluginLoader * loader = pluginMap.value(filename);
	bool loaded = loader->isLoaded();
	if (loaded) {
		//Unloads the plugin
		//Do nothing, wait for the application to restart
	} else {
		//Loads the plugin
		bool ok = loader->load();
		qDebug() << __FUNCTION__ << "Plugin loaded?:" << ok;
	}
}

void PluginsConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}
