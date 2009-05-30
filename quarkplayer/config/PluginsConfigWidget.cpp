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

#include "PluginsConfigWidget.h"

#include "ui_PluginsConfigWidget.h"

#include <quarkplayer/PluginsManager.h>
#include <quarkplayer/PluginInterface.h>

#include <QtGui/QtGui>

#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>

static const int CHECKBOX_COLUMN = 0;
static const int FILENAME_COLUMN = 1;
static const int NAME_COLUMN = 2;
static const int VERSION_COLUMN = 3;
static const int STATE_COLUMN = 4;
static const int UUID_COLUMN = 5;

PluginsConfigWidget::PluginsConfigWidget() {
	_ui = new Ui::PluginsConfigWidget();
	_ui->setupUi(this);

	_ui->tableWidget->setColumnWidth(STATE_COLUMN, 100);
	_ui->tableWidget->verticalHeader()->hide();

	connect(_ui->tableWidget, SIGNAL(currentCellChanged(int, int, int, int)),
		SLOT(currentCellChanged(int, int)));
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

void PluginsConfigWidget::readConfig() {
	PluginDataList plugins(PluginsManager::instance().availablePlugins());

	for (int i = 0; i < plugins.size(); i++) {
		PluginData pluginData(plugins[i]);

		if (i >= _ui->tableWidget->rowCount()) {
			_ui->tableWidget->insertRow(i);
		}

		_ui->tableWidget->setItem(i, FILENAME_COLUMN, new QTableWidgetItem(pluginData.fileName()));

		PluginInterface * interface = pluginData.interface();
		if (interface) {
			_ui->tableWidget->setItem(i, NAME_COLUMN, new QTableWidgetItem(interface->name()));
			_ui->tableWidget->setItem(i, VERSION_COLUMN, new QTableWidgetItem(interface->version()));
		} else {
			_ui->tableWidget->setItem(i, NAME_COLUMN, new QTableWidgetItem(QString()));
			_ui->tableWidget->setItem(i, VERSION_COLUMN, new QTableWidgetItem(QString()));
		}

		QCheckBox * checkBox = new QCheckBox();
		_ui->tableWidget->setCellWidget(i, CHECKBOX_COLUMN, checkBox);

		if (!pluginData.isEnabled()) {
			//Don't proceed plugins that are blacklisted
			checkBox->setChecked(false);
			_ui->tableWidget->setItem(i, STATE_COLUMN, new QTableWidgetItem(tr("Disabled")));
		} else {
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
	_ui->tableWidget->resizeColumnToContents(NAME_COLUMN);
	_ui->tableWidget->resizeColumnToContents(VERSION_COLUMN);
	_ui->tableWidget->resizeColumnToContents(STATE_COLUMN);
	_ui->tableWidget->resizeColumnToContents(UUID_COLUMN);
	_ui->tableWidget->resizeRowsToContents();
}

void PluginsConfigWidget::saveConfig() {
	for (int row = 0; row < _ui->tableWidget->rowCount(); row++) {
		QCheckBox * checkBox = qobject_cast<QCheckBox *>(_ui->tableWidget->cellWidget(row, CHECKBOX_COLUMN));
		if (!checkBox) {
			//No checkbox
			qCritical() << __FUNCTION__ << "Error: couldn't get the checkbox";
			return;
		}

		QTableWidgetItem * item = _ui->tableWidget->item(row, UUID_COLUMN);
		QString uuid(item->text());

		PluginData pluginData = PluginsManager::instance().pluginData(uuid);

		bool loaded = pluginData.interface();
		if (checkBox->isChecked() && !loaded) {
			//Loads the plugin
			pluginData.setEnabled(true);
			PluginsManager::instance().loadPlugin(pluginData);
		} else if (!checkBox->isChecked() && loaded) {
			//Unloads the plugin
			pluginData.setEnabled(false);
			PluginsManager::instance().deletePlugin(pluginData);
		}
	}
}

void PluginsConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}

void PluginsConfigWidget::currentCellChanged(int row, int column) {
	Q_UNUSED(column);

	QTableWidgetItem * item = _ui->tableWidget->item(row, UUID_COLUMN);
	QString uuid(item->text());

	PluginData pluginData = PluginsManager::instance().pluginData(uuid);
	PluginInterface * interface = pluginData.interface();

	if (interface) {
		_ui->descriptionGroupBox->setTitle(interface->name());
		_ui->descriptionLabel->setText(interface->description());
		_ui->webpageLabel->setText("<a href=\"" + interface->webpage() + "\">" + interface->webpage() + "</a>");
		_ui->emailLabel->setText("<a href=\"mailto:" + interface->email() +
				"?subject=[QuarkPlayer plugin] " + pluginData.fileName() + "\">" + interface->email() + "</a>");
		_ui->authorsLabel->setText(interface->authors());
		_ui->licenseLabel->setText(interface->license());
		_ui->copyrightLabel->setText(interface->copyright());
	} else {
		_ui->descriptionGroupBox->setTitle(pluginData.fileName() + tr(": plugin not loaded"));
		_ui->descriptionLabel->setText("");
		_ui->webpageLabel->setText("");
		_ui->emailLabel->setText("");
		_ui->authorsLabel->setText("");
		_ui->licenseLabel->setText("");
		_ui->copyrightLabel->setText("");
	}
}
