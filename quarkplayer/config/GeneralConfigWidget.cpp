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

#include <QtGui/QApplication>
#include <QtGui/QStyleFactory>
#include <QtGui/QStyle>

GeneralConfigWidget::GeneralConfigWidget(QWidget * parent)
	: IConfigWidget(parent) {

	_ui = new Ui::GeneralConfigWidget();
	_ui->setupUi(this);

	connect(_ui->styleComboBox, SIGNAL(activated(const QString &)),
		SLOT(styleChanged(const QString &)));

	readConfig();
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
}

void GeneralConfigWidget::readConfig() {
	_ui->styleComboBox->addItems(QStyleFactory::keys());
}

void GeneralConfigWidget::styleChanged(const QString & styleName) {
	QStyle * style = QStyleFactory::create(styleName);
	QApplication::setStyle(style);
}
