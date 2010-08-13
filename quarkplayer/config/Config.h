/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <quarkplayer/QuarkPlayerExport.h>

#include <TkUtil/TkConfig.h>
#include <TkUtil/Singleton.h>

//Yes QUuid is a struct instead of a class inside src/corelib/plugin/quuid.h
struct QUuid;

/**
 * Stores QuarkPlayer configuration.
 *
 * Under Windows Vista: <pre>C:/Users/UserName/AppData/Roaming/QuarkPlayer/QuarkPlayer.ini</pre>
 * Under Windows XP: <pre>UserName/QuarkPlayer/QuarkPlayer.ini</pre>
 * Under UNIX: <pre>~/.config/QuarkPlayer/QuarkPlayer.conf</pre>
 *
 * Pattern singleton.
 * Transform it to CRTP pattern?
 * So one can inherit from this class
 * See http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern
 * See PluginConfig for an example of how to extend Config
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
class QUARKPLAYER_API Config : public TkConfig, public Singleton {
public:

	/** Singleton. */
	static Config & instance();

	/** Deletes all configuration files from the computer. */
	void deleteConfig();

	/** Language of the application (i.e en, fr, sp...). */
	static const char * LANGUAGE_KEY;
	QString language() const;

	/** Qt style name to be used or empty string. */
	static const char * STYLE_KEY;
	QString style() const;

	/**
	 * Icon theme to be used (silk, oxygen, tango...).
	 *
	 * If no icon theme (empty string) has been set by the user then
	 * it will use the default icon theme from the OS.
	 * Under GNOME and KDE there is a default icon theme.
	 * Under Windows and Mac OS X there isn't and thus an icon theme
	 * must be specified.
	 *
	 * @see QIcon::fromTheme()
	 * @see QIcon::setThemeName()
	 */
	static const char * ICON_THEME_KEY;
	QString iconTheme() const;

	/** The most recent files played. */
	static const char * RECENT_FILES_KEY;
	QStringList recentFiles() const;

	/** Last directory used to open a media. */
	static const char * LAST_DIR_OPENED_KEY;
	QString lastDirOpened() const;

	/**
	 * DVD path: DVD device.
	 *
	 * Can be a directory or a file depending on the platform.
	 */
	static const char * DVD_PATH_KEY;
	QString dvdPath() const;

	/**
	 * CDROM path: CDROM device.
	 *
	 * Can be a directory or a file depending on the platform.
	 */
	static const char * CDROM_PATH_KEY;
	QString cdromPath() const;

	/** Standard music location (i.e C:/blabla/My Music). */
	static const char * MUSIC_DIR_KEY;
	QString musicDir(const QUuid & uuid) /*const*/;
	void addMusicDir(const QString & musicDir, const QUuid & uuid);

	/** Directories where the plugins *.(dll|so|dylib) are located. */
	static const char * PLUGIN_DIR_KEY;
	QStringList pluginDirList() const;

	/** Directory where the resources are located. */
	static const char * RESOURCE_DIR_KEY;
	QString resourceDir() const;

	/** Last media volume used. */
	static const char * LAST_VOLUME_USED_KEY;
	qreal lastVolumeUsed() const;

	/** Volume muted?. */
	static const char * VOLUME_MUTED_KEY;
	bool volumeMuted() const;

	/** @see http://doc.trolltech.com/main-snapshot/qwidget.html#restoreGeometry */
	static const char * MAINWINDOW_GEOMETRY_KEY;
	QByteArray mainWindowGeometry() const;

	/**
	 * Enables/disables multiple QuarkPlayer instances.
	 *
	 * @see QtSingleApplication
	 */
	static const char * ALLOW_MULTIPLE_INSTANCES_KEY;
	bool allowMultipleInstances() const;

	/** Backend used by class FindFiles. */
	static const char * FINDFILES_BACKEND_KEY;
	int findFilesBackend() const;

private:

	Config();

	~Config();
};

#endif	//CONFIG_H
