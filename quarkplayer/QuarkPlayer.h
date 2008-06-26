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

#ifndef QUARKPLAYER_H
#define QUARKPLAYER_H

#include <quarkplayer/quarkplayer_export.h>

#include <QtCore/QObject>
#include <QtCore/QList>

#include <phonon/phononnamespace.h>
#include <phonon/path.h>

class MainWindow;
class Config;

namespace Phonon {
	class MediaObject;
	class AudioOutput;
	class MediaSource;
	class VideoWidget;
}

/**
 * Gives access to all important objects of the application.
 *
 * Pattern proxy.
 *
 * Main class for writing plugins.
 *
 * How to make dynamic plugins? i.e possibility to load/reload them at runtime?
 *
 * @author Tanguy Krotoff
 */
class QUARKPLAYER_API QuarkPlayer : public QObject {
	Q_OBJECT
public:

	QuarkPlayer(QObject * parent);

	~QuarkPlayer();

	/** Gets the QuarkPlayer main window. */
	MainWindow & mainWindow() const;

	/** Changes the current media object to be used. */
	void setCurrentMediaObject(Phonon::MediaObject * mediaObject);

	/**
	 * Gets the current media object used.
	 *
	 * @return current media object or NULL
	 */
	Phonon::MediaObject * currentMediaObject() const;

	/** Gets the list of all available media object. */
	QList<Phonon::MediaObject *> mediaObjectList() const;

	/**
	 * Gets the music/video title of the current media object.
	 *
	 * This is a utility function.
	 */
	QString currentMediaObjectTitle() const;

	/** Play a file, an url, whatever... using the current media object available. */
	void play(const Phonon::MediaSource & mediaSource);

	/** Returns the current audio output associated with the current media object. */
	Phonon::AudioOutput * currentAudioOutput() const;

	Phonon::Path currentAudioOutputPath() const;

	/** Returns the current video widget associated with the current media object. */
	Phonon::VideoWidget * currentVideoWidget() const;

	/** Gets QuarkPlayer configuration framework. */
	Config & config() const;

public slots:

	/** Creates a new media object. */
	Phonon::MediaObject * createNewMediaObject();

signals:

	/**
	 * The current media object has been changed.
	 *
	 * @param mediaObject new current media object to be used
	 * @see setCurrentMediaObject()
	 */
	void currentMediaObjectChanged(Phonon::MediaObject * mediaObject);

	/**
	 * A new media object has been added/created.
	 *
	 * @param mediaObject the new media object that has been added
	 * @see createNewMediaObject()
	 */
	void mediaObjectAdded(Phonon::MediaObject * mediaObject);

private slots:

	/**
	 * Volume output has been changed, let's save it.
	 * FIXME Moves this somewhere else?
	 */
	void volumeChanged(qreal volume);

	/** All plugins have been loaded. */
	void allPluginsLoaded();

private:

	MainWindow * _mainWindow;

	Phonon::MediaObject * _currentMediaObject;
	QList<Phonon::MediaObject *> _mediaObjectList;
};

#endif	//QUARKPLAYER_H
