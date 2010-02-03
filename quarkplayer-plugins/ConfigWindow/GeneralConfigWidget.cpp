/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include <quarkplayer/config/Config.h>

#include <quarkplayer/config.h>
#include <quarkplayer/Languages.h>

#include <TkUtil/Translator.h>
#include <TkUtil/TkComboBox.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

GeneralConfigWidget::GeneralConfigWidget() {
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
	return "preferences-desktop";
}

void GeneralConfigWidget::saveConfig() {
	Config & config = Config::instance();

	//Style
	QString styleName = _ui->styleComboBox->currentText();
	config.setValue(Config::STYLE_KEY, styleName);
	QApplication::setStyle(QStyleFactory::create(styleName));

	//Language
	QString language = Languages::availableTranslations().key(_ui->languageComboBox->currentText());
	config.setValue(Config::LANGUAGE_KEY, language);
	Translator::instance().load(language);
}

void GeneralConfigWidget::readConfig() {
	Config & config = Config::instance();

	//Style
	_ui->styleComboBox->clear();
	_ui->styleComboBox->addItems(QStyleFactory::keys());
	TkComboBox::setCurrentText(_ui->styleComboBox, config.style());

	//Language
	_ui->languageComboBox->clear();
	QMapIterator<QString, QString> it(Languages::availableTranslations());
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
	TkComboBox::setCurrentText(_ui->languageComboBox, Languages::availableTranslations().value(language));
}

void GeneralConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}
