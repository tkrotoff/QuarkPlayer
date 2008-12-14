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

#ifndef MEDIAINFOWINDOW_H
#define MEDIAINFOWINDOW_H

#include <mediainfowindow/mediainfowindow_export.h>

#include <QtGui/QDialog>

#include <QtCore/QString>

//Included only for the enum QNetworkReply::NetworkError
//Maybe duplicate this enum to avoid this include?
#include <QtNetwork/QNetworkReply>

namespace Ui {
	class MediaInfoWindow;
}
class MediaInfoFetcher;

class WebBrowser;

class QUrl;
class QLabel;
class QToolButton;

/**
 * Shows the album artwork and other infos about the media playing.
 *
 * FIXME This should be moved to a quarkplayer plugin. Needs a better
 * plugin support for that (see QtCreator)
 *
 * @author Tanguy Krotoff
 */
class MEDIAINFOWINDOW_API MediaInfoWindow : public QDialog {
	Q_OBJECT
public:

	/**
	 * Constructs a MediaInfoWindow.
	 *
	 * @param parent QWidget parent
	 */
	MediaInfoWindow(QWidget * parent);

	~MediaInfoWindow();

	void setMediaInfoFetcher(MediaInfoFetcher * mediaInfoFetcher);

	/**
	 * Sets the language.
	 *
	 * A language/locale is 'en', 'fr', 'sp'...
	 * It should follow ISO 639-1 naming
	 *
	 * @param language language of the MediaInfoWindow
	 */
	void setLanguage(const QString & language);

public slots:

	void show();

private slots:

	void networkError(QNetworkReply::NetworkError errorCode);

	void lyricsFound(const QByteArray & lyrics, bool accurate);

	void updateMediaInfo();

	void retranslate();

	/** Refreshes all informations displayed in the window. */
	void refresh();

	void openDirectory();

private:

	MediaInfoFetcher * _mediaInfoFetcher;

	Ui::MediaInfoWindow * _ui;

	WebBrowser * _webBrowser;

	QToolButton * _refreshButton;

	QToolButton * _openDirectoryButton;

	QString _language;
};

#endif	//MEDIAINFOWINDOW_H
