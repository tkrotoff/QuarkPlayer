/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2011  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "FindSubtitles.h"

#include "FindSubtitlesWindow.h"
#include "FindSubtitlesLogger.h"

#include <quarkplayer/QuarkPlayer.h>
#include <quarkplayer/PluginManager.h>
#include <quarkplayer/config/Config.h>

#include <quarkplayer-plugins/MainWindow/MainWindow.h>
#include <quarkplayer-plugins/MediaController/MediaController.h>

#include <TkUtil/Actions.h>
#include <TkUtil/LanguageChangeEventFilter.h>

#include <phonon/mediaobject.h>
#include <phonon/mediasource.h>
#include <phonon/mediacontroller.h>

#include <QtGui/QtGui>

Q_EXPORT_PLUGIN2(FindSubtitles, FindSubtitlesFactory);

const char * FindSubtitlesFactory::PLUGIN_NAME = "FindSubtitles";

QStringList FindSubtitlesFactory::dependencies() const {
	QStringList tmp;
	tmp += MainWindowFactory::PLUGIN_NAME;
	tmp += MediaControllerFactory::PLUGIN_NAME;
	return tmp;
}

PluginInterface * FindSubtitlesFactory::create(QuarkPlayer & quarkPlayer, const QUuid & uuid) const {
	return new FindSubtitles(quarkPlayer, uuid, MainWindowFactory::mainWindow());
}

FindSubtitles::FindSubtitles(QuarkPlayer & quarkPlayer, const QUuid & uuid, QWidget * mainWindow)
	: QObject(mainWindow),
	PluginInterface(quarkPlayer, uuid) {

	populateActionCollection();

	addMenusToMediaController();

	connect(Actions::get("FindSubtitles.FindSubtitles"), SIGNAL(triggered()),
		SLOT(findSubtitles()));
	connect(Actions::get("FindSubtitles.UploadSubtitles"), SIGNAL(triggered()),
		SLOT(uploadSubtitles()));

	RETRANSLATE(this);
	retranslate();
}

FindSubtitles::~FindSubtitles() {
}

void FindSubtitles::populateActionCollection() {
	QCoreApplication * app = QApplication::instance();
	Q_ASSERT(app);

	Actions::add("FindSubtitles.FindSubtitles", new QAction(app));
	Actions::add("FindSubtitles.UploadSubtitles", new QAction(app));
}

void FindSubtitles::retranslate() {
	Actions::get("FindSubtitles.FindSubtitles")->setText(tr("&Find Subtitles..."));
	Actions::get("FindSubtitles.FindSubtitles")->setIcon(QIcon::fromTheme("edit-find"));

	Actions::get("FindSubtitles.UploadSubtitles")->setText(tr("&Upload Subtitles..."));
}

void FindSubtitles::addMenusToMediaController() {
	MediaController * mediaController = MediaControllerFactory::mediaController();
	QMenu * menuSubtitle = mediaController->menuSubtitle();
	if (!menuSubtitle) {
		FindSubtitlesCritical() << "menuSubtitle is NULL";
		return;
	}

	menuSubtitle->addAction(Actions::get("FindSubtitles.FindSubtitles"));
	menuSubtitle->addAction(Actions::get("FindSubtitles.UploadSubtitles"));

	//Add find susbtitles action to the MediaController tool bar
	mediaController->toolBar()->addAction(Actions::get("FindSubtitles.FindSubtitles"));
}

void FindSubtitles::findSubtitles() {
	Phonon::MediaObject * mediaObject = quarkPlayer().currentMediaObject();
	if (mediaObject) {
		Phonon::MediaSource source = mediaObject->currentSource();
		QString fileName(source.fileName());

		static FindSubtitlesWindow * findSubtitlesWindow = NULL;
		if (!findSubtitlesWindow) {
			findSubtitlesWindow = new FindSubtitlesWindow(qobject_cast<QWidget *>(parent()));
			connect(findSubtitlesWindow, SIGNAL(subtitleDownloaded(const QString &)),
				SLOT(loadSubtitle(const QString &)));
		}
		findSubtitlesWindow->setVideoFileName(fileName);
		findSubtitlesWindow->setLanguage(Config::instance().language());
		findSubtitlesWindow->exec();
	}
}

void FindSubtitles::loadSubtitle(const QString & fileName) {
	if (fileName.isEmpty()) {
		return;
	}

	Phonon::MediaController * currentMediaController = quarkPlayer().currentMediaController();
	if (currentMediaController) {

		//Get a subtitle id that is not already taken/existing
		int newSubtitleId = 0;
		QList<Phonon::SubtitleDescription> subtitles = currentMediaController->availableSubtitles();
		foreach (Phonon::SubtitleDescription subtitle, subtitles) {
			int id = subtitle.index();
			QString type = subtitle.property("type").toString();
			QString name = subtitle.property("name").toString();
			FindSubtitlesDebug() << "Subtitle available:" << id << type << name;

			if (newSubtitleId <= id) {
				newSubtitleId = id + 1;
			}
		}

		//Create the new subtitle and add it to the list of available subtitles
		QHash<QByteArray, QVariant> properties;
		properties.insert("type", "file");
		properties.insert("name", fileName);
		Phonon::SubtitleDescription newSubtitle(newSubtitleId, properties);
		currentMediaController->setCurrentSubtitle(newSubtitle);
	}
}

void FindSubtitles::uploadSubtitles() {
	QDesktopServices::openUrl(QUrl("http://www.opensubtitles.org/upload"));
}
