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

#include "BackendCapabilitiesWidget.h"

#include "ui_BackendCapabilitiesWidget.h"

#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/objectdescriptionmodel.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

BackendCapabilitiesWidget::BackendCapabilitiesWidget() {
	_ui = new Ui::BackendCapabilitiesWidget();
	_ui->setupUi(this);

	connect(Phonon::BackendCapabilities::notifier(), SIGNAL(capabilitiesChanged()),
		SLOT(updateCapabilities()));
	connect(Phonon::BackendCapabilities::notifier(), SIGNAL(availableAudioOutputDevicesChanged()),
		SLOT(updateCapabilities()));

	updateCapabilities();
}

BackendCapabilitiesWidget::~BackendCapabilitiesWidget() {
	delete _ui;
}

QString BackendCapabilitiesWidget::name() const {
	return tr("Backend Capabilities");
}

QString BackendCapabilitiesWidget::iconName() const {
	return "system-help";
}

void BackendCapabilitiesWidget::saveConfig() {
}

void BackendCapabilitiesWidget::readConfig() {
}

void BackendCapabilitiesWidget::retranslate() {
	_ui->retranslateUi(this);
	updateCapabilities();
}

void BackendCapabilitiesWidget::updateCapabilities() {
	//outputDevices
	_ui->audioDevicesListView->setModel(new QStandardItemModel());

//This code does not compile under MinGW :/
//See comments inside phonon/objectdescriptionmodel.h
//ICE means Internal Compiler Error
#ifndef Q_CC_MINGW
	Phonon::ObjectDescriptionModel<Phonon::AudioOutputDeviceType> * model =
			new Phonon::ObjectDescriptionModel<Phonon::AudioOutputDeviceType>();
	model->setModelData(Phonon::BackendCapabilities::availableAudioOutputDevices());
	_ui->audioDevicesListView->setModel(model);
#endif	//Q_CC_MINGW

	//mimeTypes
	_ui->mimeTypesListWidget->clear();
	QStringList mimeTypes = Phonon::BackendCapabilities::availableMimeTypes();
	foreach (QString mimeType, mimeTypes) {
		QListWidgetItem * item = new QListWidgetItem(_ui->mimeTypesListWidget);
		item->setText(mimeType);
	}

	//effects
	_ui->audioEffectsTreeWidget->clear();
	QList<Phonon::EffectDescription> effects = Phonon::BackendCapabilities::availableAudioEffects();
	foreach (Phonon::EffectDescription effect, effects) {
		QTreeWidgetItem * item = new QTreeWidgetItem(_ui->audioEffectsTreeWidget);
		item->setText(0, tr("Effect"));
		item->setText(1, effect.name());
		item->setText(2, effect.description());

		//effectsParameters
		Phonon::Effect * instance = new Phonon::Effect(effect, this);
		QList<Phonon::EffectParameter> params = instance->parameters();
		foreach (Phonon::EffectParameter param, params) {
			QVariant defaultValue = param.defaultValue();
			QVariant minimumValue = param.minimumValue();
			QVariant maximumValue = param.maximumValue();

			QString valueString = QString("%1 / %2 / %3")
					.arg(defaultValue.toString()).arg(minimumValue.toString())
					.arg(maximumValue.toString());

			QTreeWidgetItem * paramItem = new QTreeWidgetItem(item);
			paramItem->setText(0, tr("Parameter"));
			paramItem->setText(1, param.name());
			paramItem->setText(2, param.description());
			paramItem->setText(3, QVariant::typeToName(param.type()));
			paramItem->setText(4, valueString);
		}
	}

	for (int i = 0; i < _ui->audioEffectsTreeWidget->columnCount(); ++i) {
		if (i == 0) {
			_ui->audioEffectsTreeWidget->setColumnWidth(0, 150);
		} else if (i == 2) {
			_ui->audioEffectsTreeWidget->setColumnWidth(2, 350);
		} else {
			_ui->audioEffectsTreeWidget->resizeColumnToContents(i);
		}
	}
}
