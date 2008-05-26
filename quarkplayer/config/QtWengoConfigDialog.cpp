/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "QtWengoConfigDialog.h"

#include "ui_WengoConfigDialog.h"

#include "QtNotificationSettings.h"
#include "QtGeneralSettings.h"
#include "QtAccountSettings.h"
#include "QtPrivacySettings.h"
#include "QtAudioSettings.h"
#include "QtVideoSettings.h"
#include "QtAdvancedSettings.h"
#include "QtCallForwardSettings.h"
#include "QtLanguagesSettings.h"
#include "QtVoicemailSettings.h"
#include "QtSecuritySettings.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfileHandler.h>

#include <util/Logger.h>

#include <qtutil/WidgetUtils.h>

#include <QtGui/QtGui>

#include <QtCore/QPointer>

QtWengoConfigDialog::QtWengoConfigDialog(CWengoPhone & cWengoPhone)
	: QDialog(0) {

	_ui = new Ui::WengoConfigDialog();
	_ui->setupUi(this);
	_ui->retranslateUi(this);

	// Set bold ourself: if we do it from Designer it alters the font name (at
	// least with Qt 4.1)
	QFont font(_ui->titleLabel->font());
	font.setBold(true);
	_ui->titleLabel->setFont(font);

	_settingsList += new QtGeneralSettings(cWengoPhone, 0);
	if (cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		_settingsList += new QtAccountSettings(cWengoPhone, 0);
	}
	_settingsList += new QtLanguagesSettings(0);
	_settingsList += new QtAudioSettings(cWengoPhone, 0);
	_settingsList += new QtVideoSettings(cWengoPhone, 0);
	_settingsList += new QtCallForwardSettings(cWengoPhone, 0);
	_settingsList += new QtNotificationSettings(0);
	_settingsList += new QtVoicemailSettings(cWengoPhone, 0);
	_settingsList += new QtSecuritySettings(cWengoPhone, 0);
	_settingsList += new QtAdvancedSettings(0);

	//stackedWidget
	Q_FOREACH(QtISettings* settings, _settingsList) {
		QString iconName = QString(":pics/config/%1.png").arg(settings->getIconName());
		QIcon icon = QPixmap(iconName);
		new QListWidgetItem(icon, settings->getName(), _ui->listWidget);
		QWidget* widget = settings->getWidget();
		widget->layout()->setMargin(0);
		_ui->stackedWidget->addWidget(widget);
	}
	_ui->listWidget->setCurrentRow(0);
	showSettingPage(0);

	int minWidth = WidgetUtils::computeListViewMinimumWidth(_ui->listWidget);
	_ui->listWidget->setFixedWidth(minWidth);

	//listWidget
	connect(_ui->listWidget, SIGNAL(currentRowChanged(int)), SLOT(showSettingPage(int)));

	//saveButton
	connect(_ui->saveButton, SIGNAL(clicked()), SLOT(save()));
}

QtWengoConfigDialog::~QtWengoConfigDialog() {
	delete _ui;
}

void QtWengoConfigDialog::showSettingPage(int row) {
	if (row<0 || row >= _settingsList.size()) {
		return;
	}
	QtISettings* settings = _settingsList.at(row);
	_ui->titleLabel->setText(settings->getTitle());
	QWidget* widget = settings->getWidget();
	_ui->stackedWidget->setCurrentWidget(widget);
	settings->postInitialize();
}

void QtWengoConfigDialog::save() {
	Q_FOREACH(QtISettings* settings, _settingsList) {
		settings->saveConfig();
	}
}

void QtWengoConfigDialog::showPage(const QString & pageName) {
	int count = _settingsList.count();
	for (int row = 0; row < count; ++row) {
		if (_settingsList[row]->getName() == pageName) {
			_ui->listWidget->setCurrentRow(row);
			return;
		}
	}
	LOG_WARN("unknown page name=" + pageName.toStdString());
}

void QtWengoConfigDialog::showGeneralPage() {
	showPage(tr("General"));
}

void QtWengoConfigDialog::showLanguagePage() {
	showPage(tr("Language"));
}

void QtWengoConfigDialog::showAccountsPage() {
	showPage(tr("Accounts"));
}

void QtWengoConfigDialog::showPrivacyPage() {
	showPage(tr("Privacy"));
}

void QtWengoConfigDialog::showAudioPage() {
	showPage(tr("Audio"));
}

void QtWengoConfigDialog::showVideoPage() {
	showPage(tr("Video"));
}

void QtWengoConfigDialog::showNotificationsPage() {
	showPage(tr("Notifications & Sounds"));
}

void QtWengoConfigDialog::showCallForwardPage() {
	showPage(tr("Call Forward"));
}

void QtWengoConfigDialog::showAdvancedPage() {
	showPage(tr("Advanced"));
}

void QtWengoConfigDialog::showVoicemailPage() {
	showPage(tr("Voicemail"));
}

void QtWengoConfigDialog::showSecurityPage() {
	showPage(tr("security"));
}

QtWengoConfigDialog* QtWengoConfigDialog::showInstance(CWengoPhone& cWengoPhone) {
	static QPointer<QtWengoConfigDialog> instance;
	if (!instance) {
		instance = new QtWengoConfigDialog(cWengoPhone);
		instance->setAttribute(Qt::WA_DeleteOnClose);
		instance->show();
	} else {
		instance->raise();
	}
	return instance;
}
