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

#ifndef COVERARTWINDOW_H
#define COVERARTWINDOW_H

#include <QtGui/QDialog>

#include <QtCore/QString>

namespace Ui {
	class CoverArtWindow;
}

class WebBrowser;

class QUrl;
class QLabel;

/**
 * Shows the album cover art and other infos about the media playing.
 *
 * @author Tanguy Krotoff
 */
class CoverArtWindow : public QDialog {
	Q_OBJECT
public:

	CoverArtWindow(QWidget * parent);

	~CoverArtWindow();

	void setCoverArtFilename(const QString & coverArtFilename);

	void setMediaData(const QString & album, const QString & artist, const QString & title);

public slots:

	void show();

private slots:

	void lyricsFound(const QByteArray & lyrics, bool accuracy);

private:

	Ui::CoverArtWindow * _ui;

	WebBrowser * _webBrowser;

	QString _coverArtFilename;

	QString _album;
	QString _artist;
	QString _title;
};

#endif	//COVERARTWINDOW_H
