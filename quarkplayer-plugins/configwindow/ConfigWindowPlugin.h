/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef CONFIGWINDOWPLUGIN_H
#define CONFIGWINDOWPLUGIN_H

#include <quarkplayer-plugins/configwindow/configwindow_export.h>

#include <quarkplayer/PluginInterface.h>

#include <QtCore/QObject>

class ConfigWindow;

/**
 * Config window plugin.
 *
 * @author Tanguy Krotoff
 */
class CONFIGWINDOW_API ConfigWindowPlugin : public QObject, public PluginInterface {
	Q_OBJECT
public:

	ConfigWindowPlugin(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~ConfigWindowPlugin();

	/** Returns the ConfigWindow or NULL if not yet created. */
	ConfigWindow * configWindow() const;

signals:

	/**
	 * The ConfigWindow has been created, meaning user asked for the configuration window to be showed.
	 *
	 * This is useful for plugins with ConfigWidget inside the ConfigWindow.
	 * This signal is only sent once (configuration window is constructed only once and then
	 * show and hide each time the user wants it).
	 *
	 * Example:
	 * <pre>
	 * #include <quarkplayer-plugins/configwindow/ConfigWindowPlugin.h>
	 * #include <quarkplayer-plugins/configwindow/ConfigWindow.h>
	 *
	 * TotoPlugin::TotoPlugin(QuarkPlayer & quarkPlayer) {
	 * 	connect(&quarkPlayer.mainWindow(), SIGNAL(configWindowCreated(ConfigWindow *)),
	 * 		SLOT(configWindowCreated(ConfigWindow *)));
	 * }
	 *
	 * TotoPlugin::configWindowCreated(ConfigWindow * configWindow) {
	 * 	configWindow->addConfigWidget(new TotoPluginConfigWidget());
	 * }
	 * </pre>
	 *
	 * @param configWindow the configuration window that has been created
	 */
	void configWindowCreated(ConfigWindow * configWindow);

private slots:

	void showConfigWindow();

private:

	void loadSettings() const;

	void saveSettings() const;

	ConfigWindow * _configWindow;
};

#include <quarkplayer/PluginFactory.h>

class CONFIGWINDOW_API ConfigWindowPluginFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Config window"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;

	static ConfigWindowPlugin * configWindowPlugin();
};

#endif	//CONFIGWINDOWPLUGIN_H
