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

#ifndef FINDSUBTITLES_H
#define FINDSUBTITLES_H

#include <quarkplayer/PluginInterface.h>

#include <QtCore/QObject>

class QuarkPlayer;

/**
 * Find subtitles plugin, uses http://www.opensubtitles.org website.
 *
 * Source code taken from SMPlayer, original author: Ricardo Villalba
 *
 * @see FindSubtitlesWindow
 * @author Tanguy Krotoff
 */
class FindSubtitles : public QObject, public PluginInterface {
	Q_OBJECT
public:

	FindSubtitles(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~FindSubtitles();

	QString name() const { return tr("Find subtitles"); }
	QString description() const { return tr("Find subtitles from http://www.opensubtitles.org website"); }
	QString version() const { return "0.0.1"; }
	QString webpage() const { return "http://quarkplayer.googlecode.com/"; }
	QString email() const { return "quarkplayer@googlegroups.com"; }
	QString authors() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

private slots:

	void findSubtitles();

	void loadSubtitle(const QString & fileName);

	void uploadSubtitles();

private:
};

#include <quarkplayer/PluginFactory.h>

class FindSubtitlesFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	QString pluginName() const;

	QStringList dependencies() const;

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;
};

#endif	//FINDSUBTITLES_H
