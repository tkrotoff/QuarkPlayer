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

#include "GeneralConfigWidget.h"

#include "ui_GeneralConfigWidget.h"

#include "Config.h"
#include "../Translator.h"
#include "../ComboBoxUtil.h"

#include <QtGui/QtGui>

#include <QtCore/QDebug>

GeneralConfigWidget::GeneralConfigWidget(QWidget * parent)
	: IConfigWidget(parent) {

	_ui = new Ui::GeneralConfigWidget();
	_ui->setupUi(this);
}

GeneralConfigWidget::~GeneralConfigWidget() {
	delete _ui;
}

QString GeneralConfigWidget::name() const {
	return tr("General");
}

QString GeneralConfigWidget::iconName() const {
	return "";
}

void GeneralConfigWidget::saveConfig() {
	Config & config = Config::instance();

	//Style
	QString styleName = _ui->iconThemeComboBox->currentText();
	QApplication::setStyle(QStyleFactory::create(styleName));
	config.setValue(Config::STYLE_KEY, styleName);

	//Icon theme
	config.setValue(Config::ICON_THEME_KEY, _ui->iconThemeComboBox->currentText().toLower());

	//Language
	QString language = _ui->languageComboBox->currentText();
	QString locale = languageList().key(language);
	config.setValue(Config::LANGUAGE_KEY, locale);
	qDebug() << __FUNCTION__ << "language:" << language << "locale:" << locale;
	Translator::instance().load(locale);
}

void GeneralConfigWidget::readConfig() {
	Config & config = Config::instance();

	//Style
	_ui->styleComboBox->clear();
	_ui->styleComboBox->addItems(QStyleFactory::keys());

	//Icon theme
	_ui->iconThemeComboBox->clear();
	_ui->iconThemeComboBox->addItems(config.iconThemeList());
	ComboBoxUtil::setCurrentText(_ui->iconThemeComboBox, config.iconTheme());

	//Language
	_ui->languageComboBox->clear();
	QMapIterator<QString, QString> it(languageList());
	while (it.hasNext()) {
		it.next();
		//Add each language to the combobox
		_ui->languageComboBox->addItem(it.value());
	}
	QString language = config.language();
	if (language.isEmpty()) {
		//Default language is English
		language = "en";
	}
	ComboBoxUtil::setCurrentText(_ui->languageComboBox, languageList().value(language));
}

QMap<QString, QString> GeneralConfigWidget::languageList() {
	QMap<QString, QString> list;
	list["en"] = tr("English");
	list["fr"] = tr("French");
	return list;
}