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

#ifndef PHONON_VLC_VIDEOWIDGET_H
#define PHONON_VLC_VIDEOWIDGET_H

#include <phonon/videowidgetinterface.h>

#include <QtCore/QObject>

class QWidget;

namespace Phonon
{
namespace VLC
{

/**
 *
 *
 * @author Tanguy Krotoff
 */
class VideoWidget : public QObject, Phonon::VideoWidgetInterface {
	Q_OBJECT
	Q_INTERFACES(Phonon::VideoWidgetInterface)
public:

	VideoWidget(QWidget * parent);
	~VideoWidget();

	Phonon::VideoWidget::AspectRatio aspectRatio() const;
	void setAspectRatio(Phonon::VideoWidget::AspectRatio aspectRatio);

	qreal brightness() const;
	void setBrightness(qreal brightness);

	Phonon::VideoWidget::ScaleMode scaleMode() const;
	void setScaleMode(Phonon::VideoWidget::ScaleMode scaleMode);

	qreal contrast() const;
	void setContrast(qreal contrast);

	qreal hue() const;
	void setHue(qreal hue);

	qreal saturation() const;
	void setSaturation(qreal saturation);

	QWidget * widget();

private:

	QWidget * _widget;
};

}}	//Namespace Phonon::VLC

#endif	//PHONON_VLC_VIDEOWIDGET_H
