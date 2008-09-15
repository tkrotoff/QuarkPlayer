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

namespace Ui {
	class MediaInfoWindow;
}
class MediaInfoFetcher;

class WebBrowser;
class ThumbnailView;

class QUrl;
class QLabel;
class QToolButton;

/**
 * Shows the album cover art and other infos about the media playing.
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
	 * Sets the locale.
	 *
	 * A locale is 'en', 'fr', 'sp'...
	 * It should follow Wikipedia locale naming
	 *
	 * @param locale locale of the MediaInfoWindow
	 */
	void setLocale(const QString & locale);

public slots:

	void show();

private slots:

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

	ThumbnailView * _thumbnailView;

	QToolButton * _refreshButton;

	QToolButton * _openDirectoryButton;

	QString _coverArtDir;

	QString _locale;
};

#endif	//MEDIAINFOWINDOW_H
