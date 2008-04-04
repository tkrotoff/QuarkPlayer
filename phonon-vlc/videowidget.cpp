/*
 * VLC backend for the Phonon library
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "videowidget.h"

#include "vlcloader.h"
#include "vlc_symbols.h"

#include <QtGui/QWidget>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC
{

VideoWidget::VideoWidget(QWidget * parent)
	: QObject(parent) {

	_widget = new QWidget(parent);

	_vlcMediaPlayerWidgetId = (int) _widget->winId();
}

VideoWidget::~VideoWidget() {
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const {
	if (_vlcCurrentMediaPlayer) {
		const char * aspectRatio = p_libvlc_video_get_aspect_ratio(_vlcCurrentMediaPlayer, _vlcException);
		qDebug() << "VideoWidget::aspectRatio():" << aspectRatio;
	}

	return Phonon::VideoWidget::AspectRatioAuto;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio) {
	switch(aspectRatio) {
	case Phonon::VideoWidget::AspectRatioAuto:
		break;
	case Phonon::VideoWidget::AspectRatioWidget:
		break;
	case Phonon::VideoWidget::AspectRatio4_3:
		break;
	case Phonon::VideoWidget::AspectRatio16_9:
		break;
	default:
		qCritical() << __FUNCTION__ << "error: unsupported AspectRatio:" << aspectRatio;
	}
}

qreal VideoWidget::brightness() const {
	return 0.0;
}

void VideoWidget::setBrightness(qreal brightness) {
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const {
	return Phonon::VideoWidget::ScaleAndCrop;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode) {
}

qreal VideoWidget::contrast() const {
	return 0.0;
}

void VideoWidget::setContrast(qreal contrast) {
}

qreal VideoWidget::hue() const {
	return 0.0;
}

void VideoWidget::setHue(qreal hue) {
}

qreal VideoWidget::saturation() const {
	return 0.0;
}

void VideoWidget::setSaturation(qreal staturation) {
}

QWidget * VideoWidget::widget() {
	return _widget;
}

}}	//Namespace Phonon::VLC
