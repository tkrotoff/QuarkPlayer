/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef TKTEXTBROWSER_H
#define TKTEXTBROWSER_H

#include <QtGui/QTextBrowser>

class SyncHttp;

class QHttp;
class QHttpResponseHeader;

/**
 * Improves QTextBrowser to make it a "real" minimalist web browser.
 *
 * @see QTextBrowser
 * @author Tanguy Krotoff
 */
class TkTextBrowser : public QTextBrowser {
	Q_OBJECT
public:

	TkTextBrowser(QWidget * parent);

	~TkTextBrowser();

	QVariant loadResource(int type, const QUrl & name);

	void setCSSEnabled(bool enabled);

public slots:

	void setHtml(const QString & text);
	void setPlainText(const QString & text);
	void setText(const QString & text);
	void setSource(const QUrl & name);

private slots:

	void requestFinished(int id, bool error);

	void responseHeaderReceived(const QHttpResponseHeader & resp);

private:

	bool requestAlreadyLaunched(const QUrl & resourceName) const;

	/** Clears _resourceMap and _cacheMap. */
	void clearCache();

	/** Performs asynchronous downloads. */
	QHttp * _httpDownloader;

	/** Performs synchronous downloads. */
	SyncHttp * _httpSyncDownloader;

	/** A resource as a type (HTML, image, CSS) and a name (URL). */
	struct Resource {
		int type;
		QUrl name;
	};

	/** Assign a resource to a HTTP request Id. */
	QMap<int, Resource> _resourceMap;

	/** Keep in cache downloaded datas. */
	QMap<QUrl, QVariant> _cacheMap;

	bool _cssEnabled;
};

#endif	//TKTEXTBROWSER_H
