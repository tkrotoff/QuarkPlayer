/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef FINDSUBTITLES_H
#define FINDSUBTITLES_H

#include <quarkplayer/PluginInterface.h>

#include <QtCore/QObject>

class QuarkPlayer;

/**
 * Find subtitles plugin, uses http://www.opensubtitles.org website.
 *
 * Source code adapted from SMPlayer, original author: Ricardo Villalba
 * See also Totem source code: http://git.gnome.org/browse/totem/tree/src/plugins/opensubtitles
 *
 * @see FindSubtitlesWindow
 * @author Tanguy Krotoff
 */
class FindSubtitles : public QObject, public PluginInterface {
	Q_OBJECT
public:

	FindSubtitles(QuarkPlayer & quarkPlayer, const QUuid & uuid, QWidget * mainWindow);

	~FindSubtitles();

private slots:

	void findSubtitles();

	void loadSubtitle(const QString & fileName);

	void uploadSubtitles();

	void retranslate();

private:

	void populateActionCollection();

	void addMenusToMediaController();
};

#include <quarkplayer/PluginFactory.h>

/**
 * Creates FindSubtitles plugin.
 *
 * @author Tanguy Krotoff
 */
class FindSubtitlesFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("Find subtitles from http://www.opensubtitles.org website"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//FINDSUBTITLES_H
