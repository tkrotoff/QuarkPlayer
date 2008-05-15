/*
 * VLC and MPlayer backends for the Phonon library
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

#include "VLCVideoWidget.h"

#include "vlc_loader.h"
#include "vlc_symbols.h"

#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtCore/QDebug>

#ifdef USE_GL_WIDGET
	#include <QtOpenGL/QGLWidget>
	typedef QGLWidget Widget;
#else
	typedef QWidget Widget;
#endif	//USE_GL_WIDGET

namespace Phonon
{
namespace VLC_MPlayer
{

WidgetPaintEvent::WidgetPaintEvent(QWidget * parent)
: QWidget(parent) {

	//Background color is black
	setBackgroundColor(this, Qt::black);

	//When resizing fill with black (backgroundRole color) the rest is done by paintEvent
	setAttribute(Qt::WA_OpaquePaintEvent);

	//Disable Qt composition management as MPlayer draws onto the widget directly
	setAttribute(Qt::WA_PaintOnScreen);

	//Indicates that the widget has no background, i.e. when the widget receives paint events,
	//the background is not automatically repainted.
	setAttribute(Qt::WA_NoSystemBackground);

	//Required for dvdnav
	setMouseTracking(true);
}

void WidgetPaintEvent::paintEvent(QPaintEvent * event) {
	//Makes everything backgroundRole color
	QPainter painter(this);
	painter.eraseRect(rect());
}

void WidgetPaintEvent::setBackgroundColor(QWidget * widget, const QColor & color) {
	QPalette palette = widget->palette();
	palette.setColor(widget->backgroundRole(), color);
	widget->setPalette(palette);
}


VLCVideoWidget::VLCVideoWidget(QWidget * parent)
: WidgetPaintEvent(parent) {
}

VLCVideoWidget::~VLCVideoWidget() {
}

void VLCVideoWidget::resizeEvent(QResizeEvent * event) {
	qDebug() << "event->size():" << event->size();
}

void VLCVideoWidget::setAspectRatio(double aspectRatio) {
}

void VLCVideoWidget::setScaleAndCropMode(bool scaleAndCrop) {
}

void VLCVideoWidget::setVideoSize(const QSize & videoSize) {
	_videoSize = videoSize;
}

QSize VLCVideoWidget::sizeHint() const {
	return _videoSize;
}

}}	//Namespace Phonon::VLC_MPlayer
