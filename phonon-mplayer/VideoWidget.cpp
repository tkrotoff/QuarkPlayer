/*
 * MPlayer backend for the Phonon library
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

#include "VideoWidget.h"

#include "Backend.h"

#include "smplayer/core.h"
#include "smplayer/mplayerwindow.h"

#include <QtGui/QWidget>

namespace Phonon
{
namespace MPlayer
{

VideoWidget::VideoWidget(QWidget * parent)
	: QObject(parent) {

	MplayerWindow * smplayerWindow = Backend::getSMPlayerWindow();
	smplayerWindow->show();

	_aspectRatio = Phonon::VideoWidget::AspectRatioAuto;
	_brightness = 0;
	_scaleMode = Phonon::VideoWidget::FitInView;
	_contrast = 0;
	_hue = 0;
	_saturation = 0;
}

VideoWidget::~VideoWidget() {
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const {
	return _aspectRatio;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio) {
	_aspectRatio = aspectRatio;

	/*
	MediaSettings::Aspect43
	MediaSettings::Aspect169
	MediaSettings::Aspect149
	MediaSettings::Aspect1610
	MediaSettings::Aspect54
	MediaSettings::Aspect235
	MediaSettings::AspectAuto
	*/

	switch(_aspectRatio) {
	case Phonon::VideoWidget::AspectRatioWidget:
		Backend::getSMPlayerCore()->changeAspectRatio(MediaSettings::AspectAuto);
		break;
	case Phonon::VideoWidget::AspectRatioAuto:
		Backend::getSMPlayerCore()->changeAspectRatio(MediaSettings::AspectAuto);
		break;
	case Phonon::VideoWidget::AspectRatio4_3:
		Backend::getSMPlayerCore()->changeAspectRatio(MediaSettings::Aspect43);
		break;
	case Phonon::VideoWidget::AspectRatio16_9:
		Backend::getSMPlayerCore()->changeAspectRatio(MediaSettings::Aspect169);
		break;
	default:
		qWarning() << __FUNCTION__ << "unknow Phonon::VideoWidget::AspectRatio:" << _aspectRatio;
	}
}

qreal VideoWidget::brightness() const {
	return _brightness;
}

void VideoWidget::setBrightness(qreal brightness) {
	_brightness = brightness;
	Backend::getSMPlayerCore()->setBrightness(brightness * 100);
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const {
	return _scaleMode;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode) {
	_scaleMode = scaleMode;

	switch (_scaleMode) {
	case Phonon::VideoWidget::FitInView:
		break;
	case Phonon::VideoWidget::ScaleAndCrop:
		break;
	default:
		qWarning() << __FUNCTION__ << "unknow Phonon::VideoWidget::ScaleMode:" << _scaleMode;
	}
}

qreal VideoWidget::contrast() const {
	return _contrast;
}

void VideoWidget::setContrast(qreal contrast) {
	_contrast = contrast;
	Backend::getSMPlayerCore()->setContrast(_contrast * 100);
}

qreal VideoWidget::hue() const {
	return _hue;
}

void VideoWidget::setHue(qreal hue) {
	_hue = hue;
	Backend::getSMPlayerCore()->setHue(hue * 100);
}

qreal VideoWidget::saturation() const {
	return _saturation;
}

void VideoWidget::setSaturation(qreal saturation) {
	_saturation = saturation;
	Backend::getSMPlayerCore()->setSaturation(saturation * 100);
}

QWidget * VideoWidget::widget() {
	return Backend::getSMPlayerWindow();
}

}}	//Namespace Phonon::MPlayer
