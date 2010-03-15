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

#ifndef MEDIADATAWIDGET_H
#define MEDIADATAWIDGET_H

#include <MediaInfoWindow/MediaInfoWidget.h>

#include <QtCore/QList>

//Included because of enum QNetworkReply::NetworkError
#include <QtNetwork/QNetworkReply>

class MediaInfo;
class MediaInfoWindow;
class MediaInfoFetcher;
class AmazonCoverArt;
class ContentFetcherTrack;

namespace Phonon {
	class MediaObject;
}

class QLabel;
class QToolButton;
class QFile;
class QTimer;

/**
 * Shows the media data inside a widget.
 *
 * @author Tanguy Krotoff
 */
class MediaDataWidget : public MediaInfoWidget {
	Q_OBJECT
public:

	MediaDataWidget(QWidget * parent);

	~MediaDataWidget();

	void startMediaInfoFetcher(Phonon::MediaObject * mediaObject);

private slots:

	void showMediaInfoWindow();

	void updateMediaInfo(const MediaInfo & mediaInfo);

	void retranslate();

	void amazonCoverArtFound(QNetworkReply::NetworkError error, const QUrl & url, const QByteArray & amazonCoverArt, const ContentFetcherTrack & track);

private:

	void downloadAmazonCoverArt(const MediaInfo & mediaInfo);

	/** Shows a status message about the cover art downloading. */
	void showCoverArtStatusMessage(const QString & message) const;

	MediaInfoWindow * _mediaInfoWindow;

	MediaInfoFetcher * _mediaInfoFetcher;

	AmazonCoverArt * _amazonCoverArt;

	/**
	 * Current album.
	 *
	 * Used to avoid a bug: writing a cover art inside the wrong
	 * directory due to network lag since HTTP requests are asynchronous.
	 * One could think about a unique id for each network request but it is less user friendly?
	 */
	QString _currentAlbum;

	QString _amazonCoverArtPath;
};

#endif	//MEDIADATAWIDGET_H
