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
static const int RESET_COLUMN = 5;

static const QString TYPE_STRING = "string";
static const QString TYPE_STRINGLIST = "stringlist";
static const QString TYPE_BOOLEAN = "boolean";
static const QString TYPE_INTEGER = "integer";

SettingsBrowser::SettingsBrowser(QWidget * parent)
	: IConfigWidget(parent) {

	_ui = new Ui::SettingsBrowser();
	_ui->setupUi(this);
}

SettingsBrowser::~SettingsBrowser() {
	delete _ui;
}

QString SettingsBrowser::name() const {
	return tr("Settings Browser");
}

QString SettingsBrowser::iconName() const {
	return "preferences-system";
}

void SettingsBrowser::saveConfig() {
	_settingsChangedAndUnsaved = false;

	Config & config = Config::instance();

	QStringList keys = config.allKeys();
	for (int row = 0; row < keys.size(); row++) {
		QString key = keys[row];

		QVariant defaultValue = config.defaultValue(key);
		//qDebug() << __FUNCTION__ << "defaultValue:" << defaultValue;

		QVariant value = item(defaultValue, config.value(key), row);
		//qDebug() << __FUNCTION__ << "value:" << value;

		//Do not save the value if it is the same as the previous one:
		//this is done already by IConfig
		config.setValue(key, value);
	}
}

void SettingsBrowser::showEvent(QShowEvent * event) {
	qDebug() << __FUNCTION__ << "showEvent";

	//Re-read the config before to show SettingsBrowser
	readConfig();

	QWidget::showEvent(event);
}

void SettingsBrowser::hideEvent(QHideEvent * event) {
	qDebug() << __FUNCTION__ << "hideEvent";

	QWidget::hideEvent(event);
}

void SettingsBrowser::closeEvent(QCloseEvent * event) {
	qDebug() << __FUNCTION__ << "closeEvent";

	//Ask for saving settings if needed
	/*if (_settingsChangedAndUnsaved) {
		QMessageBox::StandardButton button = QMessageBox::warning(QApplication::activeWindow(),
			tr("Settings Unsaved"),
			tr("The settings have been modified.\n"
			"Do you want to save your changes?"),
			QMessageBox::Save | QMessageBox::Ignore,
			QMessageBox::Save);
		if (button == QMessageBox::Save) {
			qDebug() << __FUNCTION__ << "Settings saved";
			saveConfig();
		}
	}*/

	QWidget::closeEvent(event);
}

void SettingsBrowser::readConfig() {
	_settingsChangedAndUnsaved = false;

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
			addResetButton(row);
		}

		setItem(defaultValue, value, row);

		//Makes the font bold or not in order to differentiate for the user
		//Same as Firefox about:config
		setRowBold(row, value != defaultValue);
	}
}

void SettingsBrowser::addResetButton(int row) const {
	QToolButton * resetButton = new QToolButton();
	resetButton->setText(tr("Reset"));
	_ui->tableWidget->setCellWidget(row, RESET_COLUMN, resetButton);
	connect(resetButton, SIGNAL(clicked()), SLOT(resetValue()));
}

void SettingsBrowser::removeResetButton(int row) const {
	_ui->tableWidget->removeCellWidget(row, RESET_COLUMN);
}

int SettingsBrowser::findRow(QWidget * widget, int column) const {
	int row = 0;
	int rows = _ui->tableWidget->rowCount();
	for (; row < rows; row++) {
		QWidget * cellWidget = _ui->tableWidget->cellWidget(row, column);
		if (!cellWidget) {
			//No reset button widget
			continue;
		}

		if (cellWidget == widget) {
			//We have found the proper row
			break;
		}
	}

	if (row == rows) {
		//No match between the reset button and the cell widget
		qCritical() << __FUNCTION__ << "Error: no widget and cell widget match";
		return -1;
	}

	//We have found the proper row
	return row;
}

QString SettingsBrowser::findKey(int row) const {
	QTableWidgetItem * item = _ui->tableWidget->item(row, KEY_NAME_COLUMN);
	if (!item) {
		qCritical() << __FUNCTION__ << "Error: no QTableWidgetItem";
		return QString();
	}

	QString key = item->text();
	if (key.isEmpty()) {
		qCritical() << __FUNCTION__ << "Error: empty key";
	}

	//We have the key
	return key;
}

void SettingsBrowser::resetValue() {
	QWidget * resetButton = qobject_cast<QWidget *>(sender());
	if (!resetButton) {
		//No reset button widget
		return;
	}

	int row = findRow(resetButton, RESET_COLUMN);
	if (row == -1) {
		return;
	}

	//Let's get the matching key
	//Yes this code is fucking boring...
	QString key = findKey(row);
	if (key.isEmpty()) {
		return;
	}

	//Resets the proper item
	Config & config = Config::instance();
	QVariant defaultValue = config.defaultValue(key);
	setItem(defaultValue, defaultValue, row);

	setRowBold(row, false);

	removeResetButton(row);
}

void SettingsBrowser::setRowBold(int row, bool bold) const {
	int columns = _ui->tableWidget->columnCount();
	for (int column = 0; column < columns; column++) {
		QTableWidgetItem * item = _ui->tableWidget->item(row, column);
		if (item) {
			QFont font = item->font();
			font.setBold(bold);
			item->setFont(font);
		}
	}
}

void SettingsBrowser::setItem(const QVariant & defaultValue, const QVariant & value, int row) {
	switch (defaultValue.type()) {

	case QVariant::Bool: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("Bool"));
		QComboBox * comboBox = new QComboBox(this);
		_ui->tableWidget->setCellWidget(row, VALUE_COLUMN, comboBox);
		comboBox->setEditable(false);
		QStringList items;
		items << tr("True");
		items << tr("False");
		comboBox->insertItems(-1, items);
		bool tmp = value.toBool();
		if (tmp) {
			comboBox->setCurrentIndex(0);
		} else {
			comboBox->setCurrentIndex(1);
		}
		connect(comboBox, SIGNAL(currentIndexChanged(int)), SLOT(valueChanged()));
		break;
	}

	case QVariant::Int: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("Int"));
		QSpinBox * spinBox = new QSpinBox(this);
		spinBox->setRange(INT_MIN, INT_MAX);
		_ui->tableWidget->setCellWidget(row, VALUE_COLUMN, spinBox);
		spinBox->setValue(value.toInt());
		connect(spinBox, SIGNAL(valueChanged(int)), SLOT(valueChanged()));
		break;
	}

	case QVariant::String: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("String"));
		QLineEdit * lineEdit = new QLineEdit(this);
		_ui->tableWidget->setCellWidget(row, VALUE_COLUMN, lineEdit);
		lineEdit->setText(value.toString());
		connect(lineEdit, SIGNAL(textChanged(const QString &)), SLOT(valueChanged()));
		break;
	}

	case QVariant::StringList: {
		_ui->tableWidget->setItem(row, TYPE_COLUMN, new QTableWidgetItem("StringList"));
		QLineEdit * lineEdit = new QLineEdit(this);
		_ui->tableWidget->setCellWidget(row, VALUE_COLUMN, lineEdit);
		lineEdit->setText(value.toStringList().join(";"));
		connect(lineEdit, SIGNAL(textChanged(const QString &)), SLOT(valueChanged()));
		break;
	}

	default:
		qCritical() << __FUNCTION__ << "Error: cannot convert value:" << defaultValue;
	}

	//FIXME Cannot do that, Qt 4.4.0 does not like it:
	//it does not show the content of the widget...
	//_ui->tableWidget->setCellWidget(row, VALUE_COLUMN, widget);
}

QVariant SettingsBrowser::item(const QVariant & defaultValue, const QVariant & value, int row) {
	QWidget * widget = _ui->tableWidget->cellWidget(row, VALUE_COLUMN);
	if (!widget) {
		//This means SettingsBrowser was not even show to the user,
		//thus widgets were not created and are NULL
		//Let's return the current value
		return value;
	}

	//We base value detection on default value has it contains for sure a clean QVariant::Type
	switch (defaultValue.type()) {

	case QVariant::Bool: {
		QComboBox * comboBox = qobject_cast<QComboBox *>(widget);
		if (!comboBox) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type:" << defaultValue;
			return value;
		}

		QString tmp = comboBox->currentText();
		if (tmp == tr("True")) {
			return true;
		} else {
			return false;
		}
		break;
	}

	case QVariant::Int: {
		QSpinBox * spinBox = qobject_cast<QSpinBox *>(widget);
		if (!spinBox) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type:" << defaultValue;
			return value;
		}

		return spinBox->value();
		break;
	}

	case QVariant::String: {
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(widget);
		if (!lineEdit) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type:" << defaultValue;
			return value;
		}

		return lineEdit->text();
		break;
	}

	case QVariant::StringList: {
		QLineEdit * lineEdit = qobject_cast<QLineEdit *>(widget);
		if (!lineEdit) {
			qCritical() << __FUNCTION__ << "Error: the widget does not match the QVariant::Type:" << defaultValue;
			return value;
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
		return value;
	}
}

void SettingsBrowser::valueChanged() {
	qDebug() << __FUNCTION__;

	QWidget * widget = qobject_cast<QWidget *>(sender());
	if (!widget) {
		//No widget
		return;
	}

	int row = findRow(widget, VALUE_COLUMN);
	if (row == -1) {
		return;
	}

	//Let's get the matching key
	//Yes this code is fucking boring...
	QString key = findKey(row);
	if (key.isEmpty()) {
		return;
	}

	//Resets the proper item
	Config & config = Config::instance();
	QVariant defaultValue = config.defaultValue(key);
	QVariant previousValue = config.value(key);

	QVariant value = item(defaultValue, previousValue, row);

	if (value != defaultValue) {
		setRowBold(row, true);
		addResetButton(row);
	} else {
		setRowBold(row, false);
		removeResetButton(row);
	}

	//Value has changed
	if (value != previousValue) {
		_settingsChangedAndUnsaved = true;
	}
}

void SettingsBrowser::retranslate() {
	_ui->retranslateUi(this);
}
