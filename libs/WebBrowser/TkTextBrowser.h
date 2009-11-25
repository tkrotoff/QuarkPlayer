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

#include <QtCore/QMap>

class QNetworkAccessManager;
class QNetworkReply;

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

public slots:

	void setHtml(const QString & text);

	void setSource(const QUrl & name);

private slots:

	void finished(QNetworkReply * reply);

private:

	/** Clears _resourceMap. */
	void clearCache();

	/** Performs the downloads. */
	QNetworkAccessManager * _networkAccess;

	/** A resource has a type (HTML, image, CSS) and contains data (via QNetworkReply *). */
	struct Resource {
		int type;
		QVariant data;
	};

	/** Keep in cache downloaded datas. */
	QMap<QUrl, Resource> _resourceMap;
};

#endif	//TKTEXTBROWSER_H
