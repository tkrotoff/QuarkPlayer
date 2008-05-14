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

#include "MPlayerWindow.h"

#include <QtGui/QPainter>

#include <QtCore/QDebug>

#ifdef USE_GL_WIDGET
	#include <QtOpenGL/QGLWidget>
	typedef QGLWidget Widget;
#else
	typedef QWidget Widget;
#endif	//USE_GL_WIDGET

MPlayerWindow::MPlayerWindow(QWidget * parent)
	: QWidget(parent) {

	_videoLayer = new Widget(this);

	//Background color is black
	setBackgroundColor(this, Qt::black);

	//When resizing fill with black (backgroundRole color) the rest is done by paintEvent
	_videoLayer->setAttribute(Qt::WA_OpaquePaintEvent);

	//Disable Qt composition management as MPlayer draws onto the widget directly using X calls
	_videoLayer->setAttribute(Qt::WA_PaintOnScreen);

	//Required for dvdnav
	setMouseTracking(true);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	_aspectRatio = (double) 4 / 3;
	_scaleAndCrop = false;

	_videoWidth = 0;
	_videoHeight = 0;
}

MPlayerWindow::~MPlayerWindow() {
}

void MPlayerWindow::setBackgroundColor(QWidget * widget, const QColor & color) {
	QPalette palette = widget->palette();
	palette.setColor(widget->backgroundRole(), color);
	widget->setPalette(palette);
}

void MPlayerWindow::paintEvent(QPaintEvent * event) {
	//Makes everything black (backgroundRole color)
	QPainter painter(this);
	painter.eraseRect(event->rect());
}

WId MPlayerWindow::winId() const {
	return _videoLayer->winId();
}

void MPlayerWindow::setAspectRatio(double aspectRatio) {
	_aspectRatio = aspectRatio;

	updateVideoWindow();
}

void MPlayerWindow::setScaleAndCropMode(bool scaleAndCrop) {
	_scaleAndCrop = scaleAndCrop;

	updateVideoWindow();
}

void MPlayerWindow::resizeEvent(QResizeEvent *) {
	updateVideoWindow();
}

void MPlayerWindow::updateVideoWindow() const {
	int parentWidth = size().width();
	int parentHeight = size().height();

	int width, height;
	int x = 0;
	int y = 0;

	int pos1_h = (int) (parentWidth / _aspectRatio + 0.5);

	if (pos1_h <= parentHeight) {
		width = parentWidth;
		height = pos1_h;

		y = (parentHeight - height) / 2;
	} else {
		width = (int) (parentHeight * _aspectRatio + 0.5);
		height = parentHeight;

		x = (parentWidth - width) / 2;
	}

	if (_scaleAndCrop) {
		//Expand the video to the maximum size of the parent
		_videoLayer->move(0, 0);
		_videoLayer->resize(parentWidth, parentHeight);
	} else {
		//Respect the video aspect ratio
		_videoLayer->move(x, y);
		_videoLayer->resize(width, height);
	}
}

void MPlayerWindow::setVideoSize(int videoWidth, int videoHeight) {
	_videoWidth = videoWidth;
	_videoHeight = videoHeight;
	_aspectRatio = (double) _videoWidth / _videoHeight;

	updateVideoWindow();
}

QSize MPlayerWindow::sizeHint() const {
	qDebug() << __FUNCTION__ << "_videoWidth:" << _videoWidth << "_videoHeight:" << _videoHeight;

	qDebug() << __FUNCTION__ << "size():" << size();
	qDebug() << __FUNCTION__ << "_videoLayer->size():" << _videoLayer->size();

	return QSize(_videoWidth, _videoHeight);
}
