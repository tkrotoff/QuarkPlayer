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

#include "videowidget.h"

#include "backend.h"

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
}

VideoWidget::~VideoWidget() {
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const {
	return Phonon::VideoWidget::AspectRatioAuto;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio) {
	/*
	MediaSettings::Aspect43
	MediaSettings::Aspect169
	MediaSettings::Aspect149
	MediaSettings::Aspect1610
	MediaSettings::Aspect54
	MediaSettings::Aspect235
	MediaSettings::AspectAuto
	*/

	switch(aspectRatio) {
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
	}
}

qreal VideoWidget::brightness() const {
	return 0.0;
}

void VideoWidget::setBrightness(qreal brightness) {
	Backend::getSMPlayerCore()->setBrightness(brightness);
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
	Backend::getSMPlayerCore()->setContrast(contrast);
}

qreal VideoWidget::hue() const {
	return 0.0;
}

void VideoWidget::setHue(qreal hue) {
	Backend::getSMPlayerCore()->setHue(hue);
}

qreal VideoWidget::saturation() const {
	return 0.0;
}

void VideoWidget::setSaturation(qreal staturation) {
	Backend::getSMPlayerCore()->setSaturation(staturation);
}

QWidget * VideoWidget::widget() {
	return Backend::getSMPlayerWindow();
}

}}	//Namespace Phonon::MPlayer
