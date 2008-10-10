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

#ifndef CONFIG_H
#define CONFIG_H

#include <quarkplayer/quarkplayer_export.h>

#include <tkutil/TkConfig.h>
#include <tkutil/Singleton.h>

/**
 * Stores QuarkPlayer configuration.
 *
 * Pattern singleton.
 *
 * <pre>
 * //Example for key PLUGINS_DISABLED_KEY
 * QStringList pluginsDisabled = Config::instance().pluginsDisabled();
 * //Not recommended, can be written:
 * //QStringList pluginsDisabled = Config::instance().value(PLUGINS_DISABLED_KEY).toStringList();
 * pluginsDisabled += "pluginFilenameToBlackList";
 * Config::instance().setValue(Config::PLUGINS_DISABLED_KEY, pluginsDisabled);
 * </pre>
 *
 * @see TkConfig
 * @see QSettings
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API Config : public TkConfig, public Singleton<Config> {
	friend class Singleton<Config>;
public:

	/** List of available backends + selected backend. */
	static const char * BACKEND_KEY;
	QStringList backendList() const;
	QString backend() const;

	/** Language of the application (i.e en, fr, sp...). */
	static const char * LANGUAGE_KEY;
	QString language() const;

	/** Qt style name to be used. */
	static const char * STYLE_KEY;
	QString style() const;

	/** Icon theme to be used (silk, oxygen, tango...). */
	static const char * ICON_THEME_KEY;
	QStringList iconThemeList() const;
	QString iconTheme() const;

	/** The most recent files played. */
	static const char * RECENT_FILES_KEY;
	QStringList recentFiles() const;

	/** Last directory used to open a media. */
	static const char * LAST_DIRECTORY_USED_KEY;
	QString lastDirectoryUsed() const;

	/** Last media volume used. */
	static const char * LAST_VOLUME_USED_KEY;
	qreal lastVolumeUsed() const;

	/** Volume muted?. */
	static const char * VOLUME_MUTED_KEY;
	bool volumeMuted() const;

	/** Standard music location (i.e C:/blabla/My Music). */
	static const char * MUSIC_DIR_KEY;
	QString musicDir() const;

	/** Directory where the plugins *.(dll|so|dylib) are located. */
	static const char * PLUGINS_DIR_KEY;
	QString pluginsDir() const;

	/** Plugins blacklist (i.e the ones that shouldn't be loaded). */
	static const char * PLUGINS_DISABLED_KEY;
	QStringList pluginsDisabled() const;

	/** @see http://doc.trolltech.com/main-snapshot/qwidget.html#restoreGeometry */
	static const char * MAINWINDOW_GEOMETRY_KEY;
	QByteArray mainWindowGeometry() const;

private:

	Config();

	~Config();
};

#endif	//CONFIG_H
