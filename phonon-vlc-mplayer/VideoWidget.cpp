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

#include "VideoWidget.h"

#include "MediaObject.h"

#ifdef PHONON_VLC
	#include "vlc_loader.h"
	#include "vlc_symbols.h"
#endif	//PHONON_VLC

#ifdef PHONON_MPLAYER
	#include "MPlayerMediaObject.h"

	#include <mplayer/MPlayerProcess.h>
	#include <mplayer/MPlayerWindow.h>
#endif	//PHONON_MPLAYER

#include <QtGui/QWidget>
#include <QtCore/QtDebug>

namespace Phonon
{
namespace VLC_MPlayer
{

VideoWidget::VideoWidget(QWidget * parent)
	: SinkNode(parent) {

#ifdef PHONON_MPLAYER
	_widget = new MPlayerWindow(parent);
#endif	//PHONON_MPLAYER

#ifdef PHONON_VLC
	_widget = new QWidget(parent);
#endif	//PHONON_VLC

	_aspectRatio = Phonon::VideoWidget::AspectRatioAuto;
	_scaleMode = Phonon::VideoWidget::FitInView;

	_brightness = 0;
	_contrast = 0;
	_hue = 0;
	_saturation = 0;
}

VideoWidget::~VideoWidget() {
}

void VideoWidget::connectToMediaObject(MediaObject * mediaObject) {
	SinkNode::connectToMediaObject(mediaObject);

#ifdef PHONON_MPLAYER
	MPlayerProcess * process = _mediaObject->getPrivateMediaObject().getMPlayerProcess();
	connect(process, SIGNAL(videoWidgetSizeChanged(int, int)),
		SLOT(videoWidgetSizeChanged(int, int)));
#endif	//PHONON_MPLAYER

	_mediaObject->setVideoWidgetId((int) _widget->winId());
}

Phonon::VideoWidget::AspectRatio VideoWidget::aspectRatio() const {
#ifdef PHONON_VLC
	if (_vlcCurrentMediaPlayer) {
		const char * aspectRatio = p_libvlc_video_get_aspect_ratio(_vlcCurrentMediaPlayer, _vlcException);
		qDebug() << "VideoWidget::aspectRatio():" << aspectRatio;
	}
#endif	//PHONON_VLC

	return _aspectRatio;
}

void VideoWidget::setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio) {
	qDebug() << __FUNCTION__ << "aspectRatio:" << aspectRatio;

	_aspectRatio = aspectRatio;
	double ratio = (double) 4 / 3;

	switch (_aspectRatio) {

	//Let the decoder find the aspect ratio automatically from the media file (this is the default).
	case Phonon::VideoWidget::AspectRatioAuto:

	//Fits the video into the widget making the aspect ratio depend solely on the size of the widget.
	//This way the aspect ratio is freely resizeable by the user.
	case Phonon::VideoWidget::AspectRatioWidget:
#ifdef PHONON_MPLAYER
		if (_mediaObject) {
			MPlayerProcess * process = _mediaObject->getPrivateMediaObject().getMPlayerProcess();
			if (process) {
				ratio = process->getMediaData().videoAspectRatio;
			}
		}
#endif	//PHONON_MPLAYER
		break;

	case Phonon::VideoWidget::AspectRatio4_3:
		ratio = (double) 4 / 3;
		break;

	case Phonon::VideoWidget::AspectRatio16_9:
		ratio = (double) 16 / 9;
		break;

	default:
		qCritical() << __FUNCTION__ << "error: unsupported AspectRatio:" << aspectRatio;
	}

#ifdef PHONON_MPLAYER
	_widget->setAspectRatio(ratio);
#endif	//PHONON_MPLAYER
}

qreal VideoWidget::brightness() const {
	return _brightness;
}

void VideoWidget::setBrightness(qreal brightness) {
	_brightness = brightness;

	sendMPlayerCommand("brightness " + QString::number(_brightness * 100) + " 1");
}

Phonon::VideoWidget::ScaleMode VideoWidget::scaleMode() const {
	return _scaleMode;
}

void VideoWidget::setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode) {
	//The ScaleMode enum describes how to treat aspect ratio during resizing of video

	_scaleMode = scaleMode;

	switch (_scaleMode) {

	//The video will be fitted to fill the view keeping aspect ratio
	case Phonon::VideoWidget::FitInView:
#ifdef PHONON_MPLAYER
		_widget->setScaleAndCropMode(false);
#endif	//PHONON_MPLAYER
		break;

	//The video is scaled
	case Phonon::VideoWidget::ScaleAndCrop:
#ifdef PHONON_MPLAYER
		_widget->setScaleAndCropMode(true);
#endif	//PHONON_MPLAYER
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

	sendMPlayerCommand("contrast " + QString::number(_contrast * 100) + " 1");
}

qreal VideoWidget::hue() const {
	return _hue;
}

void VideoWidget::setHue(qreal hue) {
	_hue = hue;

	sendMPlayerCommand("hue " + QString::number(_hue * 100) + " 1");
}

qreal VideoWidget::saturation() const {
	return _saturation;
}

void VideoWidget::setSaturation(qreal saturation) {
	_saturation = saturation;

	sendMPlayerCommand("saturation " + QString::number(_saturation * 100) + " 1");
}

Widget * VideoWidget::widget() {
	return _widget;
}

void VideoWidget::videoWidgetSizeChanged(int width, int height) {
#ifdef PHONON_MPLAYER
	double aspectRatio = (double) width / height;

	qDebug() << __FUNCTION__ << "aspect ratio:" << aspectRatio << "width:" << width << "height:" << height;

	_widget->setAspectRatio(aspectRatio);
	_widget->setVideoSize(width, height);
#endif	//PHONON_MPLAYER
}

}}	//Namespace Phonon::VLC_MPlayer
