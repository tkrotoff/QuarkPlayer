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
#include <quarkplayer/PluginInterface.h>

#include <QtGui/QtGui>

#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>

static const int FILENAME_COLUMN = 0;
static const int NAME_COLUMN = 1;
static const int VERSION_COLUMN = 2;
static const int STATE_COLUMN = 3;

PluginsConfigWidget::PluginsConfigWidget() {
	_ui = new Ui::PluginsConfigWidget();
	_ui->setupUi(this);

	_ui->tableWidget->setColumnWidth(STATE_COLUMN, 100);
	_ui->tableWidget->verticalHeader()->hide();
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
	QDir pluginsDir(Config::instance().pluginsDir());
	QStringList pluginList = pluginsDir.entryList(QDir::Files);

	int row = 0;
	foreach (QString filename, pluginList) {
		if (row >= _ui->tableWidget->rowCount()) {
			_ui->tableWidget->insertRow(row);
		}

		_ui->tableWidget->setItem(row, FILENAME_COLUMN, new QTableWidgetItem(filename));

		QToolButton * stateButton = new QToolButton();
		_ui->tableWidget->setCellWidget(row, STATE_COLUMN, stateButton);
		QSignalMapper * signalMapper = new QSignalMapper(this);
		signalMapper->setMapping(stateButton, row);
		connect(signalMapper, SIGNAL(mapped(int)), SLOT(stateButtonClicked(int)));
		connect(stateButton, SIGNAL(clicked()), signalMapper, SLOT(map()));

		if (PluginManager::instance().isPluginDisabled(filename)) {
			//Don't proceed plugins that are blacklisted
			stateButton->setText(tr("Disabled"));
			//Red
			//stateButton->setStyleSheet("background-color: rgb(255, 0, 0);");
		} else {
			if (PluginManager::instance().isPluginLoaded(filename)) {
				stateButton->setText(tr("Enabled"));
				//Green
				//stateButton->setStyleSheet("background-color: rgb(0, 170, 0);");
			} else {
				stateButton->setText(tr("Error"));
				stateButton->setEnabled(false);
				//Red
				//stateButton->setStyleSheet("background-color: rgb(255, 0, 0);");
			}
		}

		row++;
	}

	_ui->tableWidget->resizeColumnToContents(FILENAME_COLUMN);
	_ui->tableWidget->resizeColumnToContents(NAME_COLUMN);
	_ui->tableWidget->resizeColumnToContents(VERSION_COLUMN);
	_ui->tableWidget->resizeRowsToContents();
}

void PluginsConfigWidget::saveConfig() {
	for (int row = 0; row < _ui->tableWidget->rowCount(); row++) {
		QToolButton * stateButton = qobject_cast<QToolButton *>(_ui->tableWidget->cellWidget(row, STATE_COLUMN));
		if (!stateButton) {
			//No state button widget
			qCritical() << __FUNCTION__ << "Error: couldn't get the state button:" << sender();
			continue;
		}

		QTableWidgetItem * item = _ui->tableWidget->item(row, FILENAME_COLUMN);
		QString filename(item->text());

		qDebug() << row << filename << stateButton->text() << stateButton;

		if (stateButton->text() == tr("Disable")) {
			//Unloads the plugin
			PluginManager::instance().disablePlugin(filename);
			PluginManager::instance().deletePlugin(filename);
		} else if (stateButton->text() == tr("Enable")) {
			//Loads the plugin
			PluginManager::instance().enablePlugin(filename);
			PluginManager::instance().loadPlugin(filename);
		}
	}
}

void PluginsConfigWidget::stateButtonClicked(int row) {
	QToolButton * stateButton = qobject_cast<QToolButton *>(_ui->tableWidget->cellWidget(row, STATE_COLUMN));
	if (!stateButton) {
		//No state button widget
		qCritical() << __FUNCTION__ << "Error: couldn't get the state button:" << sender();
		return;
	}

	QTableWidgetItem * item = _ui->tableWidget->item(row, FILENAME_COLUMN);
	QString filename(item->text());

	if (PluginManager::instance().isPluginLoaded(filename)) {
		//Unloads the plugin
		stateButton->setText(tr("Disable"));
	} else {
		//Loads the plugin
		stateButton->setText(tr("Enable"));
	}

	//_ui->tableWidget->setCellWidget(row, STATE_COLUMN, stateButton);

	qDebug() << "z" << row << filename << stateButton->text() << stateButton;
}

void PluginsConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}
