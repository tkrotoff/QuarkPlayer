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

#ifndef MEDIAINFOWIDGET_H
#define MEDIAINFOWIDGET_H

#include <MediaInfoWindow/mediainfowindowExport.h>

#include <QtGui/QWidget>

class MediaInfo;

class QFormLayout;
class QToolButton;
class QTimer;

/**
 * MediaInfo widget.
 *
 * A simple widget that shows the current playing file.
 *
 * @author Tanguy Krotoff
 */
class MEDIAINFOWINDOW_API MediaInfoWidget : public QWidget {
	Q_OBJECT
public:

	MediaInfoWidget(QWidget * parent);

	~MediaInfoWidget();

	QToolButton * coverArtButton() const;

public slots:

	void updateMediaInfo(const MediaInfo & mediaInfo);

	void updateCoverArts(const MediaInfo & mediaInfo);

private slots:

	void updateCoverArtPixmap();

private:

	void setupUi();

	/** Timer to switch the cover art picture. */
	QTimer * _coverArtSwitchTimer;

	/** List of cover art filenames. */
	QList<QString> _coverArtList;
	int _currentCoverArtIndex;

	QFormLayout * _formLayout;
	QToolButton * _coverArtButton;
};

#endif	//MEDIAINFOWIDGET_H
