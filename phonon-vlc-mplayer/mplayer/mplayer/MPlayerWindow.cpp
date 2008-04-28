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

#ifdef USE_GL_WIDGET
	#include <QtOpenGL/QGLWidget>
	typedef QGLWidget Widget;
#else
	typedef QWidget Widget;
#endif	//USE_GL_WIDGET

MPlayerWindow::MPlayerWindow(QWidget * parent)
	: QWidget(parent) {

	_videoLayer = new Widget(this);

	_videoLayer->setAutoFillBackground(true);

	//Black background color
	//TODO: MPlayer set color key !!!
	//MPlayer needs to have the same color key otherwise impossible to see the video
	//with some backends like directx
	//setBackgroundColor(_videoLayer, QColor(0, 0, 0));
	setBackgroundColor(_videoLayer, 0x020202);

	//Change attributes
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_StaticContents);
	//setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_PaintUnclipped);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setFocusPolicy(Qt::StrongFocus);

	_aspectRatio = (double) 4 / 3;
	_scaleAndCrop = false;

	setAutoFillBackground(true);
	setBackgroundColor(this, QColor(0, 0, 0));
}

MPlayerWindow::~MPlayerWindow() {
}

void MPlayerWindow::setBackgroundColor(QWidget * widget, const QColor & color) {
	QPalette palette = widget->palette();
	palette.setColor(widget->backgroundRole(), color);
	widget->setPalette(palette);
}

void MPlayerWindow::paintEvent(QPaintEvent * event) {
	QPainter painter(this);
	painter.eraseRect(event->rect());
	//painter.fillRect(event->rect(), QColor(255, 0, 0));
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

void MPlayerWindow::updateVideoWindow() {
	int w_width = size().width();
	int w_height = size().height();

	int pos1_w = w_width;
	int pos1_h = (int) (w_width / _aspectRatio + 0.5);

	int pos2_h = w_height;
	int pos2_w = (int) (w_height * _aspectRatio + 0.5);

	int w, h;
	int x = 0;
	int y = 0;

	if (pos1_h <= w_height) {
		w = pos1_w;
		h = pos1_h;

		y = (w_height - h) / 2;
	} else {
		w = pos2_w;
		h = pos2_h;

		x = (w_width - w) / 2;
	}

	if (!_scaleAndCrop) {
		//Respect the video aspect ratio
		_videoLayer->move(x, y);
		_videoLayer->resize(w, h);
	} else {
		//Expand the video to the maximum size of the parent
		_videoLayer->move(0, 0);
		_videoLayer->resize(w_width, w_height);
	}

	orig_x = x;
	orig_y = y;
	orig_width = w;
	orig_height = h;
}

void MPlayerWindow::setVideoSize(int videoWidth, int videoHeight) {
	_videoWidth = videoWidth;
	_videoHeight = videoHeight;

	updateVideoWindow();
}

QSize MPlayerWindow::sizeHint() const {
	return QSize(_videoWidth, _videoHeight);
}
