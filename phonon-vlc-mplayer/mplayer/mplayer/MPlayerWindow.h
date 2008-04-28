/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef MPLAYERWINDOW_H
#define MPLAYERWINDOW_H

#include <QtGui/QWidget>
#include <QtGui/QResizeEvent>

/**
 * Widget where to show MPlayer video.
 *
 * @author Tanguy Krotoff
 */
class MPlayerWindow : public QWidget {
	Q_OBJECT
public:

	MPlayerWindow(QWidget * parent);
	~MPlayerWindow();

	void setVideoSize(int videoWidth, int videoHeight);
	void setAspectRatio(double aspectRatio);

	void setScaleAndCropMode(bool scaleAndCrop);

	WId winId() const;

	QSize sizeHint() const;

private:

	void paintEvent(QPaintEvent * event);

	/**
	 * Sets the background color.
	 *
	 * This is compulsary otherwise MPlayerWindow won't display MPlayer video.
	 *
	 * I don't know which one is best: 0x020202 or QColor(0, 0, 0)...
	 */
	static void setBackgroundColor(QWidget * widget, const QColor & color);

	void updateVideoWindow();

	void resizeEvent(QResizeEvent * event);

	QWidget * _videoLayer;

	double _aspectRatio;

	/** Scale and crop mode. */
	bool _scaleAndCrop;

	/** Original pos and dimensions of the _videoLayer. */
	int orig_x, orig_y;
	int orig_width, orig_height;

	/** Original size of the video, needed for sizeHint(). */
	int _videoWidth, _videoHeight;
};

void setBackgroundColor(QWidget * widget, const QColor & color);

#endif	//MPLAYERWINDOW_H
