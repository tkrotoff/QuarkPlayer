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

#include "PluginConfigWidget.h"

#include "ui_PluginConfigWidget.h"

#include <quarkplayer/PluginManager.h>
#include <quarkplayer/PluginInterface.h>
#include <quarkplayer/PluginFactory.h>

#include <QtGui/QtGui>

#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>

static const int CHECKBOX_COLUMN = 0;
static const int FILENAME_COLUMN = 1;
static const int VERSION_COLUMN = 2;
static const int STATE_COLUMN = 3;
static const int UUID_COLUMN = 4;

PluginConfigWidget::PluginConfigWidget() {
	_ui = new Ui::PluginConfigWidget();
	_ui->setupUi(this);

	_ui->tableWidget->setColumnWidth(STATE_COLUMN, 100);
	_ui->tableWidget->verticalHeader()->hide();

	connect(_ui->tableWidget, SIGNAL(currentCellChanged(int, int, int, int)),
		SLOT(currentCellChanged(int, int)));
}

PluginConfigWidget::~PluginConfigWidget() {
	delete _ui;
}

QString PluginConfigWidget::name() const {
	return tr("Plugins");
}

QString PluginConfigWidget::iconName() const {
	return "preferences-plugin";
}

void PluginConfigWidget::readConfig() {
	PluginDataList plugins(PluginManager::instance().availablePlugins());

	for (int i = 0; i < plugins.size(); i++) {
		PluginData pluginData(plugins[i]);

		if (i >= _ui->tableWidget->rowCount()) {
			_ui->tableWidget->insertRow(i);
		}

		_ui->tableWidget->setItem(i, FILENAME_COLUMN, new QTableWidgetItem(pluginData.fileName()));

		PluginFactory * factory = pluginData.factory();
		if (factory) {
			_ui->tableWidget->setItem(i, VERSION_COLUMN, new QTableWidgetItem(factory->version()));
		} else {
			_ui->tableWidget->setItem(i, VERSION_COLUMN, new QTableWidgetItem(QString()));
		}

		QCheckBox * checkBox = new QCheckBox();
		_ui->tableWidget->setCellWidget(i, CHECKBOX_COLUMN, checkBox);

		if (!pluginData.isEnabled()) {
			//Don't proceed plugins that are blacklisted
			checkBox->setChecked(false);
			_ui->tableWidget->setItem(i, STATE_COLUMN, new QTableWidgetItem(tr("Disabled")));
		} else {
			PluginInterface * interface = pluginData.interface();
			if (interface) {
				checkBox->setChecked(true);
				_ui->tableWidget->setItem(i, STATE_COLUMN, new QTableWidgetItem(tr("Enabled")));
			} else {
				checkBox->setChecked(false);
				_ui->tableWidget->setItem(i, STATE_COLUMN, new QTableWidgetItem(tr("Error")));
			}
		}

		_ui->tableWidget->setItem(i, UUID_COLUMN, new QTableWidgetItem(pluginData.uuid().toString()));
	}

	_ui->tableWidget->resizeColumnToContents(CHECKBOX_COLUMN);
	_ui->tableWidget->resizeColumnToContents(FILENAME_COLUMN);
	_ui->tableWidget->resizeColumnToContents(VERSION_COLUMN);
	_ui->tableWidget->resizeColumnToContents(STATE_COLUMN);
	_ui->tableWidget->resizeColumnToContents(UUID_COLUMN);
	_ui->tableWidget->resizeRowsToContents();
}

void PluginConfigWidget::saveConfig() {
	PluginDataList pluginsToDelete;

	for (int row = 0; row < _ui->tableWidget->rowCount(); row++) {
		QCheckBox * checkBox = qobject_cast<QCheckBox *>(_ui->tableWidget->cellWidget(row, CHECKBOX_COLUMN));
		if (!checkBox) {
			//No checkbox
			qCritical() << __FUNCTION__ << "Error: couldn't get the checkbox";
			return;
		}

		QTableWidgetItem * item = _ui->tableWidget->item(row, UUID_COLUMN);
		QString uuid(item->text());

		PluginData pluginData = PluginManager::instance().pluginData(uuid);

		bool loaded = pluginData.interface();
		if (checkBox->isChecked() && !loaded) {
			//Loads the plugin
			pluginData.setEnabled(true);
			PluginManager::instance().loadPlugin(pluginData);
		} else if (!checkBox->isChecked() && loaded) {
			//Differ the unload/deleting of the plugins after the loop is over
			//This code is contained inside the "configwindow" plugin
			//Maybe the "configwindow" plugin is inside the list of plugins we want to delete,
			//this can lead to a crash
			//PluginManager::instance().deletePlugin(pluginData);
			///
			pluginsToDelete += pluginData;
		}
	}

	foreach (PluginData pluginData, pluginsToDelete) {
		//Unloads/deletes the plugin
		pluginData.setEnabled(false);
		PluginManager::instance().deletePlugin(pluginData);
	}
}

void PluginConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}

void PluginConfigWidget::currentCellChanged(int row, int column) {
	Q_UNUSED(column);

	QTableWidgetItem * item = _ui->tableWidget->item(row, UUID_COLUMN);
	QString uuid(item->text());

	PluginData pluginData = PluginManager::instance().pluginData(uuid);
	PluginFactory * factory = pluginData.factory();

	if (factory) {
		_ui->descriptionGroupBox->setTitle(factory->name());
		_ui->descriptionLabel->setText(factory->description());
		_ui->urlLabel->setText("<a href=\"" + factory->url() + "\">" + factory->url() + "</a>");
		_ui->vendorLabel->setText(factory->vendor());
		_ui->licenseLabel->setText(factory->license());
		_ui->copyrightLabel->setText(factory->copyright());
	} else {
		_ui->descriptionGroupBox->setTitle(pluginData.fileName() + tr(": plugin not loaded"));
		_ui->descriptionLabel->setText(QString());
		_ui->urlLabel->setText(QString());
		_ui->vendorLabel->setText(QString());
		_ui->licenseLabel->setText(QString());
		_ui->copyrightLabel->setText(QString());
	}
}
