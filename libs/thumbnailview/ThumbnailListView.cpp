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

#include "ThumbnailListView.h"

#include <tkutil/TkFile.h>

#include <QtGui/QApplication>
#include <QtGui/QHelpEvent>
#include <QtGui/QPainter>
#include <QtGui/QToolTip>
#include <QtGui/QDesktopServices>

#include <QtCore/QUrl>
#include <QtCore/QDebug>

//Space between the item outer rect and the content
static const int ITEM_MARGIN = 2;

static const int DEFAULT_COLUMN_COUNT = 2;
static const int DEFAULT_ROW_COUNT = 2;

/**
 * An ItemDelegate which generates thumbnails for images.
 *
 * It also makes sure all items are of the same size.
 */
class PreviewItemDelegate : public QAbstractItemDelegate {
public:

	PreviewItemDelegate(ThumbnailListView * view)
		: QAbstractItemDelegate(view),
		_view(view) {
	}

	QSize sizeHint(const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const {
		return QSize(_view->itemWidth(), _view->itemHeight());
	}

	bool eventFilter(QObject * object, QEvent * event) {
		switch (event->type()) {
		case QEvent::ToolTip: {
			QHelpEvent * helpEvent = static_cast<QHelpEvent *>(event);
			showToolTip(_view, helpEvent);
			return true;
		}
		case QEvent::MouseButtonPress: {
			QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
			openFileExternal(_view, mouseEvent);
			return true;
		}
		/*case QEvent::Enter: {
			return true;
		}
		case QEvent::Leave: {
			return true;
		}*/
		default:
			return false;
		}

		return false;
	}

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
		QVariant value = index.data(Qt::DecorationRole);
		QPixmap thumbnail = qvariant_cast<QPixmap>(value);
		QRect rect = option.rect;

		/*
		//Draw mouse over, does not work
		if (option.state & QStyle::State_MouseOver) {
			painter->setPen(Qt::black);
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(rect);
		}

		//Draw selection
		if (option.state & QStyle::State_Selected) {
			painter->setPen(Qt::black);
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(rect);
		}
		*/

		//Draw thumbnail
		painter->drawPixmap(
			rect.left() + (rect.width() - thumbnail.width()) / 2,
			rect.top() + (_view->thumbnailSize() - thumbnail.height()) / 2 + ITEM_MARGIN,
			thumbnail
		);
	}

private:

	/**
	 * Shows a tooltip.
	 */
	void showToolTip(QAbstractItemView * view, QHelpEvent * event) {
		QModelIndex index = view->indexAt(event->pos());
		if (!index.isValid()) {
			return;
		}

		QString filename(TkFile::fileName(index.data().toString()));

		QRect rect = view->visualRect(index);
		QPoint pos(rect.left() + ITEM_MARGIN, rect.top() + ITEM_MARGIN);
		QToolTip::showText(view->mapToGlobal(pos), filename, view);

		qDebug() << __FUNCTION__ << filename;
	}

	void openFileExternal(QAbstractItemView * view, QMouseEvent * event) {
		QModelIndex index = view->indexAt(event->pos());
		if (!index.isValid()) {
			return;
		}

		QString filename(index.data().toString());

		QUrl url = QUrl::fromLocalFile(filename);
		QDesktopServices::openUrl(url);

		qDebug() << __FUNCTION__ << url;
	}

	ThumbnailListView * _view;
};


ThumbnailListView::ThumbnailListView(QWidget * parent)
	: QListView(parent) {

	PreviewItemDelegate * delegate = new PreviewItemDelegate(this);
	setItemDelegate(delegate);
	viewport()->installEventFilter(delegate);

	setVerticalScrollMode(ScrollPerPixel);
	setHorizontalScrollMode(ScrollPerPixel);
}

ThumbnailListView::~ThumbnailListView() {
}

QSize ThumbnailListView::sizeHint() const {
	int viewBorderSize = frameWidth();
	int scrollBarSize = QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent);

	QSize hint;

	//MacOS X wants this amount of extra pixels for width, otherwise it won't
	//show DEFAULT_COLUMN_COUNT items
	static const int EXTRA_DELTA = 5;

	//Adjust width to view DEFAULT_COLUMN_COUNT columns
	hint.rwidth() = itemWidth() * DEFAULT_COLUMN_COUNT
		+ spacing() * (DEFAULT_COLUMN_COUNT * 2)
		+ viewBorderSize * 2
		+ scrollBarSize + EXTRA_DELTA;

	//Adjust height to view DEFAULT_ROW_COUNT rows
	hint.rheight() = itemHeight() * DEFAULT_ROW_COUNT
		+ spacing() * (DEFAULT_ROW_COUNT + 1)
		+ viewBorderSize * 2;

	return hint;
}

void ThumbnailListView::setThumbnailSize(int size) {
	_thumbnailSize = size;
}

int ThumbnailListView::thumbnailSize() const {
	return _thumbnailSize;
}

int ThumbnailListView::itemWidth() const {
	return (int) (_thumbnailSize * 1.4);
}

int ThumbnailListView::itemHeight() const {
	return _thumbnailSize + 3 * ITEM_MARGIN;
}
