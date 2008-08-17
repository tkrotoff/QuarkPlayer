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

#include "ImageSelector.h"

#include "ui_ImageSelector.h"

#include "ThumbnailManager.h"
#include "ThumbnailDirModel.h"

#include <QtGui/QLineEdit>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStyle>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/qglobal.h>

const int THUMBNAIL_SIZE = 200;

//Blank space between items
const int ITEM_SPACING = 0;

ImageSelector::ImageSelector(QWidget * parent)
	: QWidget(parent) {

	_ui = new Ui::ImageSelector();
	_ui->setupUi(this);
	_ui->thumbnailListView->ensurePolished();

	//Setup model
	_model = new ThumbnailDirModel(this);
	_model->setThumbnailSize(THUMBNAIL_SIZE);

	//Init thumbnailListView
	_ui->thumbnailListView->setModel(_model);

	_ui->thumbnailListView->setResizeMode(QListView::Adjust);
	_ui->thumbnailListView->setSpacing(ITEM_SPACING);
	_ui->thumbnailListView->setThumbnailSize(THUMBNAIL_SIZE);

	//This is a bit tricky, see showEvent documentation
	_ui->thumbnailListView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

ImageSelector::~ImageSelector() {
	delete _ui;
}

void ImageSelector::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);
	_ui->thumbnailListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void ImageSelector::setCurrentDir(const QString & dir) {
	QFileInfo fileInfo(dir);
	if (!fileInfo.exists()) {
		return;
	}

	if (!fileInfo.isDir()) {
		return;
	}

	_model->setDir(dir);
}
