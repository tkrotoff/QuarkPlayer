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

#include "SettingsBrowser.h"

#include "ui_SettingsBrowser.h"

#include "Config.h"

#include <QtGui/QtGui>

#include <QtCore/QVariant>

#include <climits>

static const int KEY_NAME_COLUMN = 0;
static const int STATUS_COLUMN = 1;
static const int TYPE_COLUMN = 2;
static const int DEFAULT_VALUE_COLUMN = 3;
static const int VALUE_COLUMN = 4;

static const QString TYPE_STRING = "string";
static const QString TYPE_STRINGLIST = "stringlist";
static const QString TYPE_BOOLEAN = "boolean";
static const QString TYPE_INTEGER = "integer";

SettingsBrowser::SettingsBrowser(QWidget * parent)
	: IConfigWidget(parent) {

	_ui = new Ui::SettingsBrowser();
	_ui->setupUi(this);

	readConfig();
}

SettingsBrowser::~SettingsBrowser() {
	delete _ui;
}

QString SettingsBrowser::name() const {
	return tr("Settings Browser");
}

QString SettingsBrowser::iconName() const {
	return tr("");
}

void SettingsBrowser::saveConfig() {
	Config & config = Config::instance();

	QStringList keys = config.allKeys();
	for (int row = 0; row < keys.size(); row++) {
		QString key = keys[row];

		QVariant defaultValue = config.defaultValue(key);
		//qDebug() << __FUNCTION__ << "defaultValue:" << defaultValue;

		QVariant value = item(defaultValue, row, VALUE_COLUMN);
		//qDebug() << __FUNCTION__ << "value:" << value;

		if (value != defaultValue) {
			//Do not save the value if it is the same as the default one
			config.setValue(key, value);
		}
	}
}

void SettingsBrowser::readConfig() {
	Config & config = Config::instance();

	QStringList keys = config.allKeys();
	for (int row = 0; row < keys.size(); row++) {
		if (row >= _ui->tableWidget->rowCount()) {
			_ui->tableWidget->insertRow(row);
		}

		QString key = keys[row];

		_ui->tableWidget->setItem(row, KEY_NAME_COLUMN, new QTableWidgetItem(key));

		QVariant defaultValue = config.defaultValue(key);
		_ui->tableWidget->setItem(row, DEFAULT_VALUE_COLUMN, new QTableWidgetItem(defaultValue.toString()));

		QVariant value = config.value(key);

		if (value == defaultValue) {
			_ui->tableWidget->setItem(row, STATUS_COLUMN, new QTableWidgetItem("Default"));
		} else {
			_ui->tableWidget->setItem(row, STATUS_COLUMN, new QTableWidgetItem("User"));
		}

		setItem(defaultValue, value, row, VALUE_COLUMN);
	}
}

void SettingsBrowser::setItem(const QVariant & defaultValue, const QVariant & value, const int row, int column) {
	switch (defaultValue.type()) {

	case QVariant::Bool: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("Bool"));
		QComboBox * comboBox = new QComboBox(this);
		_ui->tableWidget->setCellWidget(row, column, comboBox);
		comboBox->setEditable(false);
		QStringList items;
		items << "True";
		items << "False";
		comboBox->insertItems(-1, items);
		bool tmp = value.toBool();
		if (tmp) {
			comboBox->setCurrentIndex(0);
		} else {
			comboBox->setCurrentIndex(1);
		}
		break;
	}

	case QVariant::Int: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("Int"));
		QSpinBox * spinBox = new QSpinBox(this);
		spinBox->setRange(INT_MIN, INT_MAX);
		_ui->tableWidget->setCellWidget(row, column, spinBox);
		spinBox->setValue(value.toInt());
		break;
	}

	case QVariant::String: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("String"));
		QLineEdit * lineEdit = new QLineEdit(this);
		_ui->tableWidget->setCellWidget(row, column, lineEdit);
		lineEdit->setText(value.toString());
		break;
	}

	case QVariant::StringList: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("StringList"));
		QLineEdit * lineEdit = new QLineEdit(this);
		_ui->tableWidget->setCellWidget(row, column, lineEdit);
		lineEdit->setText(value.toStringList().join(";"));
		break;
	}

	default:
		qCritical() << __FUNCTION__ << "Error: cannot convert value:" << defaultValue;
	}

	//FIXME Cannot do that, Qt 4.4.0 does not like it:
	//it does not show the content of the widget...
	//_ui->tableWidget->setCellWidget(row, column, widget);
}

QVariant SettingsBrowser::item(const QVariant & defaultValue, int row, int column) {
	QWidget * widget = _ui->tableWidget->cellWidget(row, column);

	switch (defaultValue.type()) {

	case QVariant::Bool: {
		QComboBox * comboBox = qobject_cast<QComboBox *>(widget);
		if (!comboBox) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type";
			return defaultValue;
		}

		QString tmp = comboBox->currentText();
		if (tmp == "True") {
			return true;
		} else {
			return false;
		}
		break;
	}

	case QVariant::Int: {
		QSpinBox * spinBox = qobject_cast<QSpinBox *>(widget);
		if (!spinBox) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type";
			return defaultValue;
		}

		return spinBox->value();
		break;
	}

	case QVariant::String: {
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(widget);
		if (!lineEdit) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type";
			return defaultValue;
		}

		return lineEdit->text();
		break;
	}

	case QVariant::StringList: {
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(widget);
		if (!lineEdit) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type";
			return defaultValue;
		}

		QString tmp = lineEdit->text();
		if (tmp.isEmpty()) {
			//Otherwise return "" instead of nothing
			return QStringList();
		} else {
			return tmp.split(";");
		}
		break;
	}

	default:
		qCritical() << __FUNCTION__ << "Error: cannot convert value:" << defaultValue;
	}
}
