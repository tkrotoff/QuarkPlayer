/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2010  Tanguy Krotoff <tkrotoff@gmail.com>
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

#ifndef PHONON_MPLAYER_MPLAYERPROCESS_H
#define PHONON_MPLAYER_MPLAYERPROCESS_H

#include "MyProcess.h"
#include "MediaData.h"

#include <phonon/phononnamespace.h>

//For WId
#include <QtGui/QWidget>

#include <QtCore/QString>
#include <QtCore/QMap>

namespace Phonon
{
namespace MPlayer
{

/**
 * Help recognize the MPlayer backend output from the other debug messages.
 *
 * This is used by the MPlayer backend with qDebug().
 *
 * Use qInstallMsgHandler() and parse MPLAYER_LOG in order to separate MPlayer
 * backend debug output from your own debug output.
 *
 * Declared extern so there is no need to include MPlayerProcess.h just for this.
 */
extern const char * MPLAYER_LOG;

/**
 * Stores AudioChannel informations/datas.
 * Hack because MPlayer sends several audio channel infos instead of everything
 * at the same time.
 *
 * @author Tanguy Krotoff
 */
class AudioChannelData {
public:
	QString lang;
	QString name;
};

class SubtitleData {
public:
	QString lang;
	QString name;
	QString type;
};

/**
 * Creates a new MPlayer process.
 *
 * Currently 1 MPlayerProcess => 1 real MPlayer process.
 * It might be good in the future to use MPlayer -idle option
 * in order to keep only 1 MPlayer process running.
 *
 * Permits to send commands to the MPlayer process via its slave mode
 * Permits to receive events from the MPlayer process
 *
 * Check the MPlayer slave mode documentation: http://www.mplayerhq.hu/DOCS/tech/slave.txt
 *
 * URL to test:
 * - Quicktime H.264: http://www.podtrac.com/pts/redirect.mov/zdpub.vo.llnwd.net/o2/crankygeeks/episode111/crankygeeks.111.mov
 * - Video IPod: http://www.podtrac.com/pts/redirect.mp4/zdpub.vo.llnwd.net/o2/crankygeeks/episode111/crankygeeks.111.i.mp4
 * - Windows Media Player: http://www.podtrac.com/pts/redirect.wmv/zdpub.vo.llnwd.net/o2/crankygeeks/episode111/crankygeeks.111.wmv
 * - MPEG4: http://zdpub.vo.llnwd.net/o2/crankygeeks/episode111/crankygeeks.111.mp4
 * - MP3 (audio only): http://www.podtrac.com/pts/redirect.mp3/zdpub.vo.llnwd.net/o2/crankygeeks/episode111/crankygeeks.111.mp3
 *
 * This code is from SMPlayer http://smplayer.sourceforge.net/
 * Ricardo Villalba did the hard work, I came later on to clean/refactor it.
 *
 * @see mplayer-input-cmdlist.txt
 * @see http://www.mplayerhq.hu/DOCS/tech/slave.txt
 * @author Ricardo Villalba
 * @author Tanguy Krotoff
 */
class MPlayerProcess : public MyProcess {
	Q_OBJECT
public:

	MPlayerProcess(QObject * parent);

	~MPlayerProcess();

	/**
	 * Starts the MPlayer process.
	 *
	 * @param arguments options to give to the MPlayer process
	 * @param fileName file/media/stream to play, can be an URL or dvd://
	 * @param videoWidgetId used with the -wid option
	 * @param seek position where to start inside the file/media/stream
	 */
	bool start(const QStringList & arguments, const QString & fileName, WId videoWidgetId, qint64 seek);

	/**
	 * Stops the MPlayer process.
	 */
	void stop();

	/**
	 * Sends a command to the MPlayer process.
	 *
	 * Example: sendCommand("pause");
	 *
	 * @see http://www.mplayerhq.hu/DOCS/tech/slave.txt
	 */
	bool sendCommand(const QString & command);

	const MediaData & mediaData() const;

	bool hasVideo() const;
	bool isSeekable() const;

	qint64 currentTime() const;
	qint64 totalTime() const;

	bool isRunning() const;

	Phonon::State currentState() const;
	Phonon::State previousState() const;


	static const int MPLAYER_VERSION_NOTFOUND = -1;
	static const int MPLAYER_VERSION_FAILED = 0;
	static const int MPLAYER_1_0_RC1_SVN = 20372;
	static const int MPLAYER_1_0_RC2_SVN = 24722;

	/**
	 * Gets the MPlayer SVN revision number.
	 *
	 * @return SVN revision number; or 0 if fail to parse; or -1 if not read yet
	 */
	static int getMPlayerVersion();

	/**
	 * Last MPlayer error message.
	 *
	 * Call this method when in ErrorState.
	 *
	 * @return MPlayer error message
	 */
	QString errorString() const;

	/**
	 * Describes the severity when an error has occurred during playback.
	 *
	 * @see Phonon::ErrorType
	 */
	Phonon::ErrorType errorType() const;

signals:

	/**
	 * Emitted when the state of the media has changed.
	 *
	 * @param newState current state
	 * @param oldState previous state
	 * @see Phonon::State
	 */
	void stateChanged(Phonon::State newState, Phonon::State oldState);

	/**
	 * MPlayer reached the end of the stream/media/file.
	 */
	void endOfFileReached();

	/**
	 * Gives the current position in the stream in milliseconds.
	 *
	 * Example: "MPlayer is playing at 28,5 seconds from a video file of a 45,0 seconds length"
	 *
	 * @param time position in the stream in milliseconds (i.e 28,5 seconds)
	 * @see Phonon::MediaObject::tick()
	 */
	void tick(qint64 time);

	/**
	 * Gives the media/stream/file duration in milliseconds.
	 *
	 * @param totalTime media duration in milliseconds
	 */
	void totalTimeChanged(qint64 totalTime);

	/** A new frame has been received, gives the frame number (starts from 0). */
	void currentFrameNumberReceived(int number);

	/**
	 * If the stream contains a video or not.
	 *
	 * @see Phonon::MediaObject::hasVideoChanged()
	 */
	void hasVideoChanged(bool hasVideo);

	/**
	 * A screenshot has been taken.
	 *
	 * TODO not catched because of Phonon
	 *
	 * @param fileName screenshot filename
	 */
	void screenshotSaved(const QString & fileName);

	/**
	 * If the stream/media/file is seekable or not.
	 *
	 * @see Phonon::MediaObject::seekableChanged()
	 */
	void seekableChanged(bool isSeekable);

	/**
	 * Media has been loaded: everything is ready to do a 'play'.
	 */
	void mediaLoaded();

	/**
	 * Media/meta (artist, title, album, url...) data just changed.
	 */
	void mediaDataChanged(const MediaData & mediaData);

	/**
	 * New widget size computed by MPlayer.
	 *
	 * Should be applied to the widget that contains the MPlayer video.
	 */
	void videoWidgetSizeChanged(int width, int height);

	/**
	 * MPlayer tries to connect to a server.
	 *
	 * Only when the user tries to play an URL.
	 *
	 * Example: "Connecting to server www.podtrac.com[69.16.233.67]: 80..."
	 *
	 * TODO not catched because of Phonon
	 *
	 * @param message connecting message from MPlayer
	 */
	void connectingMessageReceived(const QString & message);

	/**
	 * MPlayer tries to resolve a domain name.
	 *
	 * Only when the user tries to play an URL.
	 *
	 * Example: "Resolving www.podtrac.com for AF_INET..."
	 *
	 * TODO not catched because of Phonon
	 *
	 * @param message resolving message from MPlayer
	 */
	void resolvingMessageReceived(const QString & message);

	/**
	 * A new audio stream/channel has been detected from the media/file/stream.
	 *
	 * Some files can contain several audio streams/channels.
	 * Example: DVD, Matroska
	 *
	 * @see http://en.wikipedia.org/wiki/Matroska
	 * @param id audio id to select if we choose this lang
	 * @param audioChannelData audio channel name/desciption and
	 *        lang fr, en... / fre, eng... / french, english...
	 */
	void audioChannelAdded(int id, const AudioChannelData & audioChannelData);

	/**
	 * A new subtitle has been detected from the media/file/stream.
	 *
	 * Some files can contain several subtitles.
	 * Example: DVD, Matroska
	 *
	 * @see http://en.wikipedia.org/wiki/Matroska
	 * @param id subtitle id to select, start at number 0
	 * @param subtitleData the file name or the subtitle
	 *        language (fr, en... / french, english...) or anything else
	 */
	void subtitleAdded(int id, const SubtitleData & subtitleData);

	/**
	 * Current subtitle has changed.
	 *
	 * @param id the new current subtitle
	 */
	void subtitleChanged(int id);

	/**
	 * A new title has been detected from the media/file/stream.
	 *
	 * Title/chapter/angle DVD.
	 *
	 * @param id title id
	 * @param length title duration in milliseconds
	 */
	void titleAdded(int id, qint64 length);

	/**
	 * Current DVD/MKV title has changed.
	 *
	 * @param id the new current DVD/MKV title
	 */
	void titleChanged(int id);

	/**
	 * A new chapter has been detected from the media/file/stream.
	 *
	 * Title/chapter/angle DVD.
	 *
	 * @param titleId title id
	 * @param chapters number of chapters (or chapter id mkv)
	 */
	void chapterAdded(int titleId, int chapters);

	/**
	 * A new Matroska chapter has been detected from the media/file/stream.
	 *
	 * @param id chapter id
	 * @param title chapter' title
	 * @param from chapter beginning
	 * @param to chapter end
	 */
	void mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to);

	/**
	 * A new angle has been detected from the media/file/stream.
	 *
	 * Title/chapter/angle DVD.
	 *
	 * @param titleId title id
	 * @param angles number of angles
	 */
	void angleAdded(int titleId, int angles);

	/**
	 * Percentage of MPlayer cache that has been filled.
	 *
	 * This is bufferization of a media over a network.
	 *
	 * @param percentFilled cache filled
	 */
	void bufferStatus(int percentFilled);

	/**
	 * MPlayer is scanning the fonts available on the system.
	 *
	 * This is done the first time MPlayer is launched.
	 *
	 * TODO not catched because of Phonon
	 */
	void scanningFonts();

	/**
	 * MPlayer is updating the font cache.
	 *
	 * Can take quite some time...
	 * This is done the first time MPlayer is launched.
	 *
	 * In order to reproduce this event, delete directory <pre>~/fontconfig</pre>
	 * Under Windows Vista/7: <pre>C:/Users/UserName/fontconfig</pre>
	 *
	 * TODO not catched because of Phonon
	 */
	void updatingFontCache();


	void receivedCreatingIndex(QString);

private slots:

	/**
	 * Parses a line from the MPlayer process.
	 *
	 * @param line line to parse
	 */
	void parseLine(const QString & line);

	void finished(int exitCode, QProcess::ExitStatus exitStatus);

	void error(QProcess::ProcessError error);

private:

	/** Initializes/resets the private variables of this class. */
	void init();

	void changeState(Phonon::State newState);

	/**
	 * Converts a normal filename to a short filename (8+3 format).
	 *
	 * This is needed when launching MPlayer process under Windows.
	 * Taken from Scribus.
	 *
	 * @see http://docs.scribus.net/devel/util_8cpp-source.html#l00112
	 * @see http://scribus.info/svn/Scribus/trunk/Scribus/scribus/util.cpp
	 */
	static QString shortPathName(const QString & longPath);

	bool _endOfFileReached;

	MediaData _mediaData;

	/** MPlayer SVN revision number. */
	static int _mplayerVersion;

	/** Previous state. */
	Phonon::State _previousState;

	/** Current state. */
	Phonon::State _currentState;

	/** Last MPlayer error message. */
	QString _errorString;

	/** Describes the severity when an error has occurred during playback. */
	Phonon::ErrorType _errorType;

	/** Current DVD/MKV title id. */
	int _currentTitleId;

	/** Audio channel list, int = audio channel id/index. */
	QMap<int, AudioChannelData> _audioChannelList;

	/** Subtitle list, int = subtitle id/index. */
	QMap<int, SubtitleData> _subtitleList;


	/**
	 * @name Regular expression for parsing MPlayer output
	 * @see http://regexlib.com/DisplayPatterns.aspx
	 * @{
	 */

	//General
	QRegExp rx_av;
	QRegExp rx_frame;
	QRegExp rx_generic;
	QRegExp rx_audio_mat;
	QRegExp rx_winresolution;
	QRegExp rx_ao;
	QRegExp rx_paused;
	QRegExp rx_novideo;
	QRegExp rx_play;
	QRegExp rx_playing;
	QRegExp rx_file_not_found;
	QRegExp rx_endoffile;
	QRegExp rx_slowsystem;

	//Fonts
	QRegExp rx_fontcache;
	QRegExp rx_scanning_font;

	//Streaming
	QRegExp rx_connecting;
	QRegExp rx_resolving;
	QRegExp rx_resolving_failed;
	QRegExp rx_cache_fill;
	QRegExp rx_read_failed;
	QRegExp rx_stream_not_found;

	//Screenshot
	QRegExp rx_screenshot;

	//DVD/Mkv titles/chapters/angles
	QRegExp rx_titles;
	QRegExp rx_title;
	QRegExp rx_mkvchapters;
	QRegExp rx_create_index;

	//VCD
	QRegExp rx_vcd;

	//Audio CD
	QRegExp rx_cdda;

	//Subtitles
	QRegExp rx_subtitle;
	QRegExp rx_sid;
	QRegExp rx_subtitle_file;

	//Meta data infos
	QRegExp rx_clip_title;
	QRegExp rx_clip_artist;
	QRegExp rx_clip_author;
	QRegExp rx_clip_album;
	QRegExp rx_clip_genre;
	QRegExp rx_clip_date;
	QRegExp rx_clip_track;
	QRegExp rx_clip_copyright;
	QRegExp rx_clip_comment;
	QRegExp rx_clip_software;

	//Radio streaming infos
	QRegExp rx_stream_title;
	QRegExp rx_stream_title_only;
	QRegExp rx_stream_name;
	QRegExp rx_stream_genre;
	QRegExp rx_stream_website;

	/** @} */
};

}}	//Namespace Phonon::MPlayer

#endif	//PHONON_MPLAYER_MPLAYERPROCESS_H
