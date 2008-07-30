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

#include "WinFileAssociationsConfigWidget.h"

#include "ui_WinFileAssociationsConfigWidget.h"

#include "Config.h"

#include <quarkplayer/PluginManager.h>
#include <quarkplayer/FileExtensions.h>

#include <winfileassociations/WinFileAssociations.h>

#include <QtGui/QtGui>

#include <QtCore/QPluginLoader>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

static const int COLUMN_EXTENSION = 0;

WinFileAssociationsConfigWidget::WinFileAssociationsConfigWidget() {
	_ui = new Ui::WinFileAssociationsConfigWidget();
	_ui->setupUi(this);

	_ui->treeWidget->setHeaderHidden(true);

	_winFileAssociations = new WinFileAssociations(QCoreApplication::applicationName());
}

WinFileAssociationsConfigWidget::~WinFileAssociationsConfigWidget() {
	delete _ui;
	delete _winFileAssociations;
}

QString WinFileAssociationsConfigWidget::name() const {
	return tr("File Associations");
}

QString WinFileAssociationsConfigWidget::iconName() const {
	return "video-x-generic";
}

void WinFileAssociationsConfigWidget::saveConfig() {
	for (int i = 0; i < _ui->treeWidget->topLevelItemCount(); i++) {
		QTreeWidgetItem * topLevelItem = _ui->treeWidget->topLevelItem(i);

		for (int j = 0; j < topLevelItem->childCount(); j++) {
			QTreeWidgetItem * item = topLevelItem->child(j);
			if (item) {
				QString extension(item->text(COLUMN_EXTENSION));
				if (!extension.isEmpty()) {
					if (item->checkState(COLUMN_EXTENSION) == Qt::Checked) {
						_winFileAssociations->addAssociation(extension);
					} else {
						_winFileAssociations->deleteAssociation(extension);
					}
				}
			}
		}
	}
}

void WinFileAssociationsConfigWidget::readConfig() {
	_ui->treeWidget->clear();

	QTreeWidgetItem * item = new QTreeWidgetItem(QStringList(tr("Video Files")));
	_ui->treeWidget->addTopLevelItem(item);
	addItems(item, FileExtensions::video());

	item = new QTreeWidgetItem(QStringList(tr("Audio Files")));
	_ui->treeWidget->addTopLevelItem(item);
	addItems(item, FileExtensions::audio());

	item = new QTreeWidgetItem(QStringList(tr("Subtitle Files")));
	_ui->treeWidget->addTopLevelItem(item);
	addItems(item, FileExtensions::subtitle());

	item = new QTreeWidgetItem(QStringList(tr("Playlist Files")));
	_ui->treeWidget->addTopLevelItem(item);
	addItems(item, FileExtensions::playlist());

	_ui->treeWidget->expandAll();
}

void WinFileAssociationsConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}

void WinFileAssociationsConfigWidget::addItems(QTreeWidgetItem * parent, const QStringList & extensions) {
	foreach (QString extension, extensions) {
		QTreeWidgetItem * item = new QTreeWidgetItem(parent);
		item->setText(COLUMN_EXTENSION, extension);

		if (_winFileAssociations->isAssociated(extension)) {
			item->setCheckState(COLUMN_EXTENSION, Qt::Checked);
		} else {
			item->setCheckState(COLUMN_EXTENSION, Qt::Unchecked);
		}
	}
}
