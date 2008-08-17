/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2004-2007  Wengo
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

#ifndef IMAGESELECTOR_H
#define IMAGESELECTOR_H

#include <QtGui/QWidget>
#include <QtGui/QImage>

class QModelIndex;
class QShowEvent;

class ThumbnailDirModel;

namespace Ui {
	class ImageSelector;
}

/**
 * A dialog to select an avatar.
 *
 * @author Aurélien Gâteau
 * @author Tanguy Krotoff
 */
class ImageSelector : public QWidget {
	Q_OBJECT
public:

	ImageSelector(QWidget * parent);

	~ImageSelector();

	void setCurrentDir(const QString & currentDir);

private:

	/**
	 * This is a bit tricky: ImageSelector constructor sets the size policy of
	 * our thumbnail view to Minimum,Minimum so that the dialog gets resized to
	 * show the view at the view.sizeHint() size. In showEvent() we reset the
	 * size policy of the view back to Preferred,Preferred so that the user can
	 * resize the dialog smaller.
	 */
	void showEvent(QShowEvent * event);

	ThumbnailDirModel * _model;

	Ui::ImageSelector * _ui;
};

#endif	//IMAGESELECTOR_H
