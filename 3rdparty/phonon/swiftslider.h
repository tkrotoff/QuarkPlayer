/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2008 Ricardo Villalba <rvm@escomposlinux.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SWIFTSLIDER_H
#define SWIFTSLIDER_H

#include <QtGui/QSlider>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_PHONON_SEEKSLIDER) && !defined(QT_NO_PHONON_VOLUMESLIDER)

namespace Phonon
{

/** \class SwiftSlider swiftslider.h Phonon/SwiftSlider
 * \short Modified QSlider that allows sudden/quick moves instead of stepped moves.
 *
 * This is an internal class used by SeekSlider and VolumeSlider.
 *
 * \ingroup PhononWidgets
 */
class SwiftSlider : public QSlider
{
	Q_OBJECT
public:
	SwiftSlider(Qt::Orientation orientation, QWidget * parent);
	~SwiftSlider();

private:
	void mousePressEvent(QMouseEvent *event);
	inline int pick(const QPoint &pt) const;
	int pixelPosToRangeValue(int pos) const;
};

} // namespace Phonon

#endif //QT_NO_PHONON_VOLUMESLIDER && QT_NO_PHONON_VOLUMESLIDER

QT_END_NAMESPACE

#endif //SWIFTSLIDER_H
