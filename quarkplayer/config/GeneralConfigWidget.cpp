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
#include <quarkplayer/config.h>

#include <tkutil/Translator.h>
#include <tkutil/TkComboBox.h>
#include <tkutil/TkIcon.h>

#include <QtGui/QtGui>

#include <QtCore/QDebug>

GeneralConfigWidget::GeneralConfigWidget() {
	_ui = new Ui::GeneralConfigWidget();
	_ui->setupUi(this);

//#ifdef KDE4_FOUND
	//Not available under KDE, systemsettings will do it
	//FIXME Not available at all, this feature should be added to Phonon itself
	_ui->backendGroupBox->setEnabled(false);
//#endif	//KDE4_FOUND
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

	//Backend
	QString backendName = _ui->backendComboBox->currentText();
	config.setValue(Config::BACKEND_KEY, backendName.toLower());

	//Style
	QString styleName = _ui->styleComboBox->currentText();
	config.setValue(Config::STYLE_KEY, styleName);
	QApplication::setStyle(QStyleFactory::create(styleName));

	//Icon theme
	config.setValue(Config::ICON_THEME_KEY, _ui->iconThemeComboBox->currentText().toLower());
	TkIcon::setIconTheme(config.iconTheme());

	//Language
	QString language = _ui->languageComboBox->currentText();
	QString locale = languageList().key(language);
	config.setValue(Config::LANGUAGE_KEY, locale);
	Translator::instance().load(locale);
}

void GeneralConfigWidget::readConfig() {
	Config & config = Config::instance();

	//Backend
	_ui->backendComboBox->clear();
	_ui->backendComboBox->addItems(config.backendList());
	TkComboBox::setCurrentText(_ui->backendComboBox, config.backend());

	//Style
	_ui->styleComboBox->clear();
	_ui->styleComboBox->addItems(QStyleFactory::keys());
	TkComboBox::setCurrentText(_ui->styleComboBox, config.style());

	//Icon theme
	_ui->iconThemeComboBox->clear();
	_ui->iconThemeComboBox->addItems(config.iconThemeList());
	TkComboBox::setCurrentText(_ui->iconThemeComboBox, config.iconTheme());

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
	TkComboBox::setCurrentText(_ui->languageComboBox, languageList().value(language));
}

QMap<QString, QString> GeneralConfigWidget::languageList() {
	QMap<QString, QString> list;
	list["en"] = tr("English") + " (English)";
	list["fr"] = tr("French") + " (French)";
	list["de"] = tr("German") + " (German)";
	return list;
}

void GeneralConfigWidget::retranslate() {
	_ui->retranslateUi(this);
}
