/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

	TkTextBrowser(QWidget * parent = 0);

	~TkTextBrowser();

	/**
	 * Loads a item (e.g image, css...) referenced by the main document.
	 *
	 * Inherited by QTextBrowser so must be public.
	 *
	 * @see QTextBrowser::loadResource()
	 */
	QVariant loadResource(int type, const QUrl & url);

public slots:

	/**
	 * Sets main document content with a given text.
	 *
	 * No network requests are performed.
	 *
	 * @see QTextEdit::setHtml()
	 */
	void setHtml(const QString & html);

	/**
	 * Loads an URL and shows its content inside QTextBrowser.
	 *
	 * Network requests are performed if needed via loadResource().
	 *
	 * @see QTextBrowser::setSource()
	 */
	void setUrl(const QUrl & url);

private slots:

	/**
	 * A network request is finished.
	 *
	 * @param reply a pointer to the reply that has just finished
	 * @see QNetworkAccessManager::finished()
	 */
	void finished(QNetworkReply * reply);

private:

	/** Clears _resourceMap. */
	void clearCache();

	/** Performs the downloads. */
	QNetworkAccessManager * _networkAccess;

	/** A resource has a type (HTML, image, CSS) and contains data (obtained via QNetworkReply *). */
	struct Resource {
		int type;
		QVariant data;
	};

	/** Keep in cache downloaded datas. */
	QMap<QUrl, Resource> _resourceMap;
};

#endif	//TKTEXTBROWSER_H
