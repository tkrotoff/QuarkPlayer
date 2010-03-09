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

#ifndef OSDWIDGET_H
#define OSDWIDGET_H

#include <quarkplayer/PluginInterface.h>

#include <QtGui/QDialog>

class MediaInfoWidget;
class MediaInfoFetcher;
class MediaInfo;

namespace Phonon {
	class MediaObject;
}
class QTimer;

/**
 * QuarkPlayer OSD.
 *
 * Shows the current playing file.
 *
 * @author Tanguy Krotoff
 */
class OSDWidget : public QDialog, public PluginInterface {
	Q_OBJECT
public:

	OSDWidget(QuarkPlayer & quarkPlayer, const QUuid & uuid);

	~OSDWidget();

public slots:

	void show();

private slots:

	void updateMediaInfo(const MediaInfo & mediaInfo);

	void metaDataChanged();

	void mediaObjectAdded(Phonon::MediaObject * mediaObject);

private:

	void setupUi();

	void startMediaInfoFetcher(Phonon::MediaObject * mediaObject);

	MediaInfoWidget * _mediaInfoWidget;

	MediaInfoFetcher * _mediaInfoFetcher;
};

#include <quarkplayer/PluginFactory.h>

/**
 * Creates OSDWidget plugin.
 *
 * @author Tanguy Krotoff
 */
class OSDWidgetFactory : public QObject, public PluginFactory {
	Q_OBJECT
	Q_INTERFACES(PluginFactory)
public:

	static const char * PLUGIN_NAME;

	QString name() const { return PLUGIN_NAME; }
	QStringList dependencies() const;
	QString description() const { return tr("On-screen display"); }
	QString version() const { return "0.0.1"; }
	QString url() const { return "http://quarkplayer.googlecode.com/"; }
	QString vendor() const { return "Tanguy Krotoff"; }
	QString license() const { return "GNU GPLv3+"; }
	QString copyright() const { return "Copyright (C) Tanguy Krotoff"; }

	PluginInterface * create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const;

	static OSDWidget * osdWidget();
};

#endif	//OSDWIDGET_H
