/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "IconEngine.h"

#include <QtGui/QtGui>

void IconEngine::paint(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state) {
/*
	Q_UNUSED(state)

	const int kstate = qIconModeToKIconState(mode);
	KIconLoader::Group group = KIconLoader::Desktop;

	if (QWidget * targetWidget = dynamic_cast<QWidget *>(painter->device())) {
		if (qobject_cast<QMenu *>(targetWidget)) {
			group = KIconLoader::Small;
		} else if (qobject_cast<QToolBar*>(targetWidget->parent())) {
			group = KIconLoader::Toolbar;
		}
	}

	const int iconSize = qMin(rect.width(), rect.height());
	QPixmap pix = iconLoader()->loadIcon(d->iconName, group, iconSize, kstate, d->overlays);
	painter->drawPixmap(rect, pix);
*/

	//QIconEngineV2::paint(painter, rect, mode, state);
}
