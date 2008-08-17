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

#include "ThumbnailView.h"

#include "ThumbnailManager.h"
#include "ThumbnailDirModel.h"
#include "ThumbnailListView.h"

#include <QtGui/QLineEdit>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStyle>
#include <QtGui/QVBoxLayout>

#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtCore/QtGlobal>

const int THUMBNAIL_SIZE = 200;

//Blank space between items
const int ITEM_SPACING = 0;

ThumbnailView::ThumbnailView(QWidget * parent)
	: QWidget(parent) {

	//Setup model
	_model = new ThumbnailDirModel(this);
	_model->setThumbnailSize(THUMBNAIL_SIZE);

	//Setup QListView
	_thumbnailListView = new ThumbnailListView(this);
	_thumbnailListView->setModel(_model);
	_thumbnailListView->setMovement(QListView::Static);
	_thumbnailListView->setFlow(QListView::LeftToRight);
	_thumbnailListView->setProperty("isWrapping", QVariant(true));
	_thumbnailListView->setResizeMode(QListView::Fixed);
	_thumbnailListView->setViewMode(QListView::IconMode);
	_thumbnailListView->ensurePolished();
	_thumbnailListView->setResizeMode(QListView::Adjust);
	_thumbnailListView->setSpacing(ITEM_SPACING);
	_thumbnailListView->setThumbnailSize(THUMBNAIL_SIZE);

	//This is a bit tricky, see showEvent documentation
	_thumbnailListView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	QVBoxLayout * layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->addWidget(_thumbnailListView);
	setLayout(layout);
}

ThumbnailView::~ThumbnailView() {
}

void ThumbnailView::showEvent(QShowEvent * event) {
	QWidget::showEvent(event);
	_thumbnailListView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void ThumbnailView::setDir(const QString & dir) {
	_dir = dir;
}

QString ThumbnailView::lastRefreshedDirectory() const {
	return _lastRefreshedDirectory;
}

void ThumbnailView::refresh() {
	_lastRefreshedDirectory = _dir;

	QFileInfo fileInfo(_dir);
	if (!fileInfo.exists()) {
		qCritical() << __FUNCTION__ << "Error: this directory does not exist:" << _dir;
		return;
	}

	if (!fileInfo.isDir()) {
		qCritical() << __FUNCTION__ << "Error: this is not a directory:" << _dir;
		return;
	}

	_model->setDir(_dir);
}
