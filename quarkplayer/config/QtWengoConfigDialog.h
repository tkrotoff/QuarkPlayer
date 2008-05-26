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

#ifndef OWQTWENGOCONFIGDIALOG_H
#define OWQTWENGOCONFIGDIALOG_H

#include <util/NonCopyable.h>

#include <QtCore/QList>

#include <QtGui/QDialog>

class CWengoPhone;
class QtISettings;
namespace Ui { class WengoConfigDialog; }

/**
 * Main configuration window.
 *
 * @author Tanguy Krotoff
 */
class QtWengoConfigDialog : public QDialog, NonCopyable {
	Q_OBJECT
public:

	~QtWengoConfigDialog();

	/**
	 * Shows the config dialog or raise it if it already exists.
	 * @return the config dialog instance, so that caller can show a particular
	 * page
	 */
	static QtWengoConfigDialog* showInstance(CWengoPhone & cWengoPhone);

public Q_SLOTS:

	void save();

	void showGeneralPage();

	void showLanguagePage();

	void showAccountsPage();

	void showPrivacyPage();

	void showAudioPage();

	void showVideoPage();

	void showNotificationsPage();

	void showCallForwardPage();

	void showAdvancedPage();

	void showVoicemailPage();

	void showSecurityPage();

private Q_SLOTS:

	void showSettingPage(int row);

private:

	QtWengoConfigDialog(CWengoPhone & cWengoPhone);

	void showPage(const QString & pageName);

	Ui::WengoConfigDialog * _ui;

	typedef QList<QtISettings *> SettingsList;
	SettingsList _settingsList;
};

#endif	//OWQTWENGOCONFIGDIALOG_H
