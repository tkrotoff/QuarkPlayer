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

#ifndef MEDIADATAWIDGET_H
#define MEDIADATAWIDGET_H

#include <QtGui/QWidget>

#include <QtCore/QList>

namespace Ui {
	class MediaDataWidget;
}

class MediaInfoWindow;
class MediaInfoFetcher;

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
class MediaDataWidget : public QWidget {
	Q_OBJECT
public:

	MediaDataWidget(QWidget * parent);

	~MediaDataWidget();

	void startMediaInfoFetcher(Phonon::MediaObject * mediaObject);

private slots:

	void updateMediaInfo();

	void retranslate();

	void coverArtFound(const QByteArray & coverArt, bool accurate);

	void updateCoverArtPixmap();

private:

	void loadCoverArt(const QString & album, const QString & artist, const QString & amazonASIN);

	void setCoverArtPixmap(const QFile & coverArtFile);

	Ui::MediaDataWidget * _ui;

	MediaInfoWindow * _mediaInfoWindow;

	MediaInfoFetcher * _mediaInfoFetcher;

	QTimer * _coverArtSwitchTimer;

	QList<QString> _coverArtList;
	int _currentCoverArtIndex;

	QString _amazonCoverArtPath;
};

#endif	//MEDIADATAWIDGET_H
