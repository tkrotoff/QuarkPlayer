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

#ifndef MEDIAINFOFETCHER_H
#define MEDIAINFOFETCHER_H

#include <mediainfowindow/mediainfowindow_export.h>

#include <mediainfowindow/MediaInfo.h>

#include <phonon/phononnamespace.h>
#include <phonon/mediasource.h>

#include <QtCore/QString>

namespace Phonon {
	class MediaObject;
}

/**
 * Shows the album cover art and other infos about the media playing.
 *
 * @author Tanguy Krotoff
 */
class MEDIAINFOWINDOW_API MediaInfoFetcher : public QObject {
	Q_OBJECT
public:

	/**
	 * Constructs a MediaInfoFetcher.
	 *
	 * @param parent QObject parent
	 */
	MediaInfoFetcher(QObject * parent);

	~MediaInfoFetcher();

	/**
	 * Taken from TagLib::FileRef()
	 *
	 * Reading audio properties from a file can sometimes be very time consuming
	 * and for the most accurate results can often involve reading the entire file.
	 * Because in many situations speed is critical or the accuracy of the values is
	 * not particularly important this allows the level of desired accuracy to be set.
	 *
	 * @see TagLib::FileRef()
	 */
	enum ReadStyle {
		/** Read as little of the file as possible. */
		ReadStyleFast,

		/** Read more of the file and make better values guesses. */
		ReadStyleAverage,

		/** Read as much of the file as needed to report accurate values. */
		ReadStyleAccurate
	};

	/**
	 * Starts info fetching given a media source.
	 *
	 * mediaSource cannot be a URL, only a file.
	 * If mediaSource is a URL then use start(Phonon::MediaObject * mediaObject)
	 *
	 * @param mediaSource Phonon media source
	 */
	void start(const Phonon::MediaSource & mediaSource, ReadStyle readStyle = ReadStyleAverage);

	/**
	 * Starts info fetching given a media object.
	 *
	 * If mediaSource is not a URL, use start(const Phonon::MediaSource & mediaSource)
	 * If mediaSource is a URL then use this method.
	 *
	 * @param mediaObject media object, use MediaObject::currentMediaSource() internally
	 */
	void start(Phonon::MediaObject * mediaObject);

	/** Gets the MediaInfo object that contains all the metadata. */
	MediaInfo mediaInfo() const;

signals:

	/**
	 * All the metadata have been retrieved/fetched.
	 *
	 * Now you can use mediaInfo()
	 * @see mediaInfo()
	 */
	void fetched();

private slots:

	void metaStateChanged(Phonon::State newState, Phonon::State oldState);

private:

	/**
	 * Use Phonon to find metadata.
	 */
	void startPhononResolver();

	/**
	 * Use TagLib to find metadata.
	 *
	 * Call startTagLibResolver() only when you are sure that the backend start to play the file.
	 *
	 * TagLib open files in read/write, opening a file in read/write prevents the backend to open the file too :/
	 *
	 * @see VideoWidgetPlugin::stateChanged()
	 * @see http://article.gmane.org/gmane.comp.kde.devel.taglib/918
	 */
	void startTagLibResolver();

	/**
	 * Use MediaInfoLib to find metadata and other informations.
	 */
	void startMediaInfoLibResolver();

	/** Determines file type from the file extension. */
	void determineFileTypeFromExtension();

	ReadStyle _readStyle;

	Phonon::MediaSource _mediaSource;

	/** Resolves media metadata/info. */
	Phonon::MediaObject * _metaObjectInfoResolver;

	/** Contains all the metadata. */
	MediaInfo _mediaInfo;
};

#endif	//MEDIAINFOFETCHER_H
