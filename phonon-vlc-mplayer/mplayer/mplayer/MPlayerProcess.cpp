/*
 * VLC and MPlayer backends for the Phonon library
 * Copyright (C) 2006-2008  Ricardo Villalba <rvm@escomposlinux.org>
 * Copyright (C) 2007-2008  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MPlayerProcess.h"

#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QtDebug>

#include "MPlayerVersion.h"

#ifdef Q_OS_WIN
	static const char * MPLAYER_EXE = "C:/Program Files/SMPlayer/mplayer/mplayer.exe";
#else
	static const char * MPLAYER_EXE = "mplayer";
#endif

/** MPlayer works using seconds, we prefer to work using milliseconds. */
static const double SECONDS_CONVERTION = 1000.0;

MPlayerProcess::MPlayerProcess(QObject * parent)
	: MyProcess(parent) {

	connect(this, SIGNAL(lineAvailable(const QByteArray &)),
		SLOT(parseLine(const QByteArray &)));

	connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));
}

MPlayerProcess::~MPlayerProcess() {
}

bool MPlayerProcess::start(const QStringList & arguments, const QString & filename, int videoWidgetId, qint64 seek) {
	//Stop MPlayerProcess if it is already running
	if (isRunning()) {
		stop();
	}

	_data.clear();

	//Not found yet
	_mplayerSvnRevision = -1;

	_endOfFileReached = false;


	QStringList args;
	args << arguments;

	//Attach MPlayer video output to our widget
	if (videoWidgetId > 0) {
		_data.videoWidgetId = videoWidgetId;
		args << "-wid";
		args << QString::number(_data.videoWidgetId);
	}

	//If seek < 5 it's better to allow the video to start from the beginning
	if (seek > 5) {
		_data.currentTime = seek;
		args << "-ss";
		args << QString::number(_data.currentTime / SECONDS_CONVERTION);
	}

	//File to play
	_data.filename = filename;
	args << filename;


	MyProcess::start(MPLAYER_EXE, args);
	return waitForStarted();
}

void MPlayerProcess::stop() {
	if (!isRunning()) {
		qWarning() << __FUNCTION__ << "MPlayer not running";
		return;
	}

	sendCommand("quit");

	qDebug() << __FUNCTION__ << "Finishing MPlayer...";
	if (!waitForFinished(5000)) {
		kill();
	}

	qDebug() << __FUNCTION__ << "MPlayer finished";
}

void MPlayerProcess::sendCommand(const QString & command) {
	qDebug() << __FUNCTION__ << "Command:" << command;

	if (command.isEmpty()) {
		return;
	}

	if (isRunning()) {
		write(command.toLocal8Bit() + "\n");
	} else {
		qWarning() << __FUNCTION__ << "Error: MPlayer process not running";
	}
}

const MediaData & MPlayerProcess::getMediaData() const {
	return _data;
}

bool MPlayerProcess::hasVideo() const {
	return _data.hasVideo;
}

bool MPlayerProcess::isSeekable() const {
	return _data.isSeekable;
}

qint64 MPlayerProcess::currentTime() const {
	return _data.currentTime;
}

qint64 MPlayerProcess::totalTime() const {
	return _data.totalTime;
}


static QRegExp rx_av("^[AV]: *([0-9,:.-]+)");
static QRegExp rx_frame("^[AV]:.* (\\d+)\\/.\\d+");// [0-9,.]+");
static QRegExp rx("^(.*)=(.*)");
static QRegExp rx_audio_mat("^ID_AID_(\\d+)_(LANG|NAME)=(.*)");
static QRegExp rx_title("^ID_DVD_TITLE_(\\d+)_(LENGTH|CHAPTERS|ANGLES)=(.*)");
static QRegExp rx_winresolution("^VO: \\[(.*)\\] (\\d+)x(\\d+) => (\\d+)x(\\d+)");
static QRegExp rx_ao("^AO: \\[(.*)\\]");
static QRegExp rx_paused("^ID_PAUSED");
static QRegExp rx_novideo("^Video: no video");
static QRegExp rx_cache("^Cache fill:.*");
static QRegExp rx_create_index("^Generating Index:.*");
static QRegExp rx_play("^Starting playback...");
static QRegExp rx_playing("^Playing");	//"Playing" does not mean the file is actually playing but only loading
static QRegExp rx_connecting("^Connecting to .*");
//Future messages to add:
//Connection timeout
//Failed, exiting
//No stream found to handle url
static QRegExp rx_resolving("^Resolving .*");
static QRegExp rx_screenshot("^\\*\\*\\* screenshot '(.*)'");
static QRegExp rx_endoffile("^Exiting... \\(End of file\\)");
static QRegExp rx_mkvchapters("\\[mkv\\] Chapter (\\d+) from.*, (.*)");

//VCD
static QRegExp rx_vcd("^ID_VCD_TRACK_(\\d+)_MSF=(.*)");

//Audio CD
static QRegExp rx_cdda("^ID_CDDA_TRACK_(\\d+)_MSF=(.*)");

//Subtitles
static QRegExp rx_subtitle("^ID_(SUBTITLE|FILE_SUB|VOBSUB)_ID=(\\d+)");
static QRegExp rx_sid("^ID_(SID|VSID)_(\\d+)_(LANG|NAME)=(.*)");
static QRegExp rx_subtitle_file("^ID_FILE_SUB_FILENAME=(.*)");

//Clip info
static QRegExp rx_clip_name("^(name|title): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_artist("^artist: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_author("^author: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_album("^album: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_genre("^genre: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_date("^(creation date|year): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_track("^track: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_copyright("^copyright: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_comment("^comment: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_software("^software: (.*)", Qt::CaseInsensitive);

static QRegExp rx_stream_title("^.* StreamTitle='(.*)';StreamUrl='(.*)';");

void MPlayerProcess::parseLine(const QByteArray & tmp) {
	//qDebug() << __FUNCTION__ << tmp;

	QString line = QString::fromLocal8Bit(tmp);

	//Skip empty lines
	if (line.isEmpty()) {
		return;
	}

	//Skip repeatitive lines
	if (line.contains("Too many buffered pts") || line.contains("pts value <= previous")) {
		return;
	}

	//Parse A: V: line
	if (rx_av.indexIn(line) > -1) {
		_data.currentTime = (qint64) (rx_av.cap(1).toDouble() * SECONDS_CONVERTION);

		if (_state != PlayingState) {
			qDebug() << __FUNCTION__ << "Starting time:" << _data.currentTime;
			setState(PlayingState);

			//OK, now all the media datas should be in clean state
			emit mediaLoaded();
		}

		//qDebug() << __FUNCTION__ << "Tick:" << _data.currentTime;
		emit tick(_data.currentTime);

		//Check for frame number
		if (rx_frame.indexIn(line) > -1) {
			int frame = rx_frame.cap(1).toInt();
			//qDebug() << __FUNCTION__ << "Frame number:" << frame;
			emit currentFrameNumberReceived(frame);
		}
	}

	//Parse other things
	else {
		qDebug() << "MPlayer:" << line;

		//Loading the file/stream/media
		//Becarefull! "Playing" does not mean the file is playing but only loading
		if (rx_playing.indexIn(line) > -1) {
			setState(LoadingState);
		}

		//Screenshot
		else if (rx_screenshot.indexIn(line) > -1) {
			const QString filename = rx_screenshot.cap(1);
			qDebug() << __FUNCTION__ << "Screenshot:" << filename;
			emit screenshotSaved(filename);
		}

		//End of file
		else if (rx_endoffile.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "End of line detected";

			//In case of playing VCDs or DVDs, maybe the first title
			//is not playable, so the GUI doesn't get the info about
			//available titles. So if we received the end of file
			//first let's pretend the file has started so the GUI can have
			//the data.
			if (_state != PlayingState) {
				setState(PlayingState);
			}

			//Sends the stateChanged(EndOfFileState) signal once the process is finished, not now!
			_endOfFileReached = true;
		}

		//Window resolution
		else if (rx_winresolution.indexIn(line) > -1) {
			int width = rx_winresolution.cap(4).toInt();
			int height = rx_winresolution.cap(5).toInt();

			qDebug() << __FUNCTION__ << "Video driver:" << rx_winresolution.cap(1);

			emit videoWidgetSizeChanged(width, height);
			//emit mplayerFullyLoaded();
		}

		//No video
		else if (rx_novideo.indexIn(line) > -1) {
			_data.hasVideo = false;
			qDebug() << __FUNCTION__ << "Video:" << _data.hasVideo;
			emit hasVideoChanged(_data.hasVideo);
			//emit mplayerFullyLoaded();
		}

		//Pause
		else if (rx_paused.indexIn(line) > -1) {
			setState(PausedState);
		}

		//Stream title
		if (rx_stream_title.indexIn(line) > -1) {
			const QString title = rx_stream_title.cap(1);
			const QString url = rx_stream_title.cap(2);
			qDebug() << __FUNCTION__ << "Stream title:" << title;
			qDebug() << __FUNCTION__ << "Stream url:" << url;
			_data.stream_title = title;
			_data.stream_url = url;
			//emit streamTitleAndUrl(title, url);
		}

		//The following things are not sent when the file has started to play
		//(or if sent, GUI will ignore anyway...)
		//So not process anymore, if video is playing to save some time
		if (_state == PlayingState) {
			return;
		}

		if ((_mplayerSvnRevision == -1) && (line.startsWith("MPlayer "))) {
			_mplayerSvnRevision = MPlayerVersion::parse(line);
			if (_mplayerSvnRevision <= 0) {
				emit failedToParseMplayerVersion(line);
			}
		}

		//Subtitles
		//static QRegExp rx_subtitle("^ID_(SUBTITLE|FILE_SUB|VOBSUB)_ID=(\\d+)");
		//static QRegExp rx_sid("^ID_(SID|VSID)_(\\d+)_(LANG|NAME)=(.*)");
		//static QRegExp rx_subtitle_file("^ID_FILE_SUB_FILENAME=(.*)");
		else if (rx_subtitle.indexIn(line) > -1) {
			//DVD line example:
			//subtitle ( sid ): 1 language: en
			//ID_SUBTITLE_ID=1
			//ID_SID_1_LANG=en
			//subtitle ( sid ): 3 language: fr
			//ID_SUBTITLE_ID=3
			//ID_SID_3_LANG=fr

			//.str file example:
			//SUB: Detected subtitle file format: subviewer
			//SUB: Read 612 subtitles.
			//ID_FILE_SUB_ID=0
			//ID_FILE_SUB_FILENAME=D:/The Ring CD1.srt
			//SUB: Added subtitle file (1): D:/The Ring CD1.srt

			//Matroska line example:
			//ID_SUBTITLE_ID=0
			//ID_SID_0_NAME=Piste de présentation
			//[mkv] Track ID 4: subtitles (S_TEXT/UTF8) Piste de présentation, -sid 0, -slang mis
			//ID_SUBTITLE_ID=1
			//ID_SID_1_LANG=fre
			//[mkv] Track ID 5: subtitles (S_VOBSUB), -sid 1, -slang fre
			//ID_SUBTITLE_ID=2
			//ID_SID_2_LANG=eng

			int id = rx_subtitle.cap(2).toInt();
			const QString type = rx_subtitle.cap(1);

			if (type == "FILE_SUB") {
				//t = SubData::File;
			} else if (type == "VOBSUB") {
				//t = SubData::Vob;
			} else {
				//t = SubData::Sub;
			}

			//emit subtitleAdded(id, "", type);
		}

		else if (rx_sid.indexIn(line) > -1) {
			int id = rx_sid.cap(2).toInt();
			const QString lang = rx_sid.cap(4);
			const QString attr = rx_sid.cap(3);
			const QString type = rx_sid.cap(1);
			qDebug() << __FUNCTION__ << "Subtitle id:" << id << "lang:" << lang << "type:" << type << "attr:" << attr;

			if (type == "VSID") {
			} else if (type == "SID") {
			}

			if (attr == "NAME") {
			} else if (attr == "LANG") {
			}

			emit subtitleAdded(id, lang, type);
		}

		else if (rx_subtitle_file.indexIn(line) > -1) {
			const QString file = rx_subtitle_file.cap(1);
			/*if (subs.count() > 0) {
				int last = subs.count() -1;
				if (subs[last].type() == SubData::File) {
					subs[last].setFilename(file);
				}
			}*/

			emit subtitleAdded(0, file, "file");
		}

		//AO
		else if (rx_ao.indexIn(line) > -1) {
			//emit receivedAO(rx_ao.cap(1));
		}

		//DVD and Matroska audio tracks
		else if (rx_audio_mat.indexIn(line) > -1) {
			//DVD line example:
			//ID_AUDIO_ID=129
			//ID_AID_129_LANG=fr
			//ID_AUDIO_ID=128
			//ID_AID_128_LANG=en

			//Matroska line example:
			//[mkv] Track ID 1: video (V_REAL/RV40), -vid 0
			//ID_AUDIO_ID=0
			//ID_AID_0_LANG=fre
			//[mkv] Track ID 3: audio (A_AAC/MPEG4/LC/SBR), -aid 1, -alang eng
			//ID_SUBTITLE_ID=0
			//ID_SID_0_NAME=Piste de présentation
			//[mkv] Track ID 4: subtitles (S_TEXT/UTF8) Piste de présentation, -sid 0, -slang mis
			//ID_SUBTITLE_ID=1
			//ID_SID_1_LANG=fre
			//[mkv] Track ID 5: subtitles (S_VOBSUB), -sid 1, -slang fre
			//ID_SUBTITLE_ID=2
			//ID_SID_2_LANG=eng
			//[mkv] Track ID 6: subtitles (S_VOBSUB), -sid 2, -slang eng
			//[mkv] Will play video track 1.
			//Matroska file format detected.
			//VIDEO:  [RV40]  704x296  24bpp  25.000 fps    0.0 kbps ( 0.0 kbyte/s)

			int id = rx_audio_mat.cap(1).toInt();
			QString lang = rx_audio_mat.cap(3);
			QString attr = rx_audio_mat.cap(2);
			qDebug() << __FUNCTION__ << "Audio id:" << id << "lang:" << lang << "attr:" << attr;

			if (attr == "NAME") {
				//lang=english, spanish...
				//_data.audioTrackList.addName(id, lang);
			} else if (attr == "LANG") {
				//lang=en, fr...
				//_data.audioTrackList.addLang(id, lang);
			}

			emit audioChannelAdded(id, lang);
		}

		//Matroska chapters
		else if (rx_mkvchapters.indexIn(line) != -1) {
			int id = rx_mkvchapters.cap(1).toInt();
			QString title = rx_mkvchapters.cap(2);
			qDebug() << __FUNCTION__ << "mkv chapter:" << id << "title:" << title;

			emit mkvChapterAdded(id, title);
		}

		//VCD titles
		/*else if (rx_vcd.indexIn(line) > -1) {
			int ID = rx_vcd.cap(1).toInt();
			QString length = rx_vcd.cap(2);
			//_data.titles.addID(ID);
			_data.titles.addName(ID, length);
		}
		else

		//Audio CD titles
		if (rx_cdda.indexIn(line) > -1) {
			int ID = rx_cdda.cap(1).toInt();
			QString length = rx_cdda.cap(2);
			double duration = 0;
			QRegExp r("(\\d+):(\\d+):(\\d+)");
			if (r.indexIn(length) > -1) {
				duration = r.cap(1).toInt() * 60;
				duration += r.cap(2).toInt();
			}
			_data.titles.addID(ID);
			//QString name = QString::number(ID) + " (" + length + ")";
			//_data.titles.addName(ID, name);
			_data.titles.addDuration(ID, duration);
		}
		else*/

		//DVD titles
		else if (rx_title.indexIn(line) > -1) {
			int titleId = rx_title.cap(1).toInt();
			const QString attr = rx_title.cap(2);

			if (attr == "LENGTH") {
				double length = rx_title.cap(3).toDouble();
				qDebug() << __FUNCTION__ << "DVD titleId:" << titleId << "length:" << length << "attr:" << attr;
				//_data.titles.addDuration(id, length);

				emit titleAdded(titleId, (int) (length * SECONDS_CONVERTION));
			}

			else if (attr == "CHAPTERS") {
				int chapters = rx_title.cap(3).toInt();
				qDebug() << __FUNCTION__ << "DVD titleId:" << titleId << "chapters:" << chapters;
				//_data.titles.addChapters(id, chapters);

				emit chapterAdded(titleId, chapters);
			}

			else if (attr == "ANGLES") {
				int angles = rx_title.cap(3).toInt();
				qDebug() << __FUNCTION__ << "DVD titleId:" << titleId << "angles:" << angles;
				//_data.titles.addAngles(id, angles);

				emit angleAdded(titleId, angles);
			}
		}

		//Catch cache messages
		else if (rx_cache.indexIn(line) > -1) {
			emit receivedCacheMessage(line);
		}

		//Creating index
		else if (rx_create_index.indexIn(line) > -1) {
			emit receivedCreatingIndex(line);
		}

		//Catch connecting message
		else if (rx_connecting.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "Connecting message:" << line;
			emit connectingMessageReceived(line);
			setState(BufferingState);
		}

		//Catch resolving message
		else if (rx_resolving.indexIn(line) > -1) {
			qDebug() << __FUNCTION__ << "Resolving message:" << line;
			emit resolvingMessageReceived(line);
		}

		//Clip info

		//Name
		else if (rx_clip_name.indexIn(line) > -1) {
			const QString s = rx_clip_name.cap(2);
			qDebug() << __FUNCTION__ << "Clip name:" << s;
			_data.clip_name = s;
		}

		//Artist
		else if (rx_clip_artist.indexIn(line) > -1) {
			const QString s = rx_clip_artist.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_artist: '%s'", s.toUtf8().data());
			_data.clip_artist = s;
		}

		//Author
		else if (rx_clip_author.indexIn(line) > -1) {
			const QString s = rx_clip_author.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_author: '%s'", s.toUtf8().data());
			_data.clip_author = s;
		}

		//Album
		else if (rx_clip_album.indexIn(line) > -1) {
			const QString s = rx_clip_album.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_album: '%s'", s.toUtf8().data());
			_data.clip_album = s;
		}

		//Genre
		else if (rx_clip_genre.indexIn(line) > -1) {
			const QString s = rx_clip_genre.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_genre: '%s'", s.toUtf8().data());
			_data.clip_genre = s;
		}

		//Date
		else if (rx_clip_date.indexIn(line) > -1) {
			const QString s = rx_clip_date.cap(2);
			qDebug("MPlayerProcess::parseLine: clip_date: '%s'", s.toUtf8().data());
			_data.clip_date = s;
		}

		//Track
		else if (rx_clip_track.indexIn(line) > -1) {
			const QString s = rx_clip_track.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_track: '%s'", s.toUtf8().data());
			_data.clip_track = s;
		}

		//Copyright
		else if (rx_clip_copyright.indexIn(line) > -1) {
			const QString s = rx_clip_copyright.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_copyright: '%s'", s.toUtf8().data());
			_data.clip_copyright = s;
		}

		//Comment
		else if (rx_clip_comment.indexIn(line) > -1) {
			const QString s = rx_clip_comment.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_comment: '%s'", s.toUtf8().data());
			_data.clip_comment = s;
		}

		//Software
		else if (rx_clip_software.indexIn(line) > -1) {
			const QString s = rx_clip_software.cap(1);
			qDebug("MPlayerProcess::parseLine: clip_software: '%s'", s.toUtf8().data());
			_data.clip_software = s;
		}

		//Catch "Starting playback..." message
		else if (rx_play.indexIn(line) > -1) {
			//OK, now all the media datas should be in clean state
			//Second time we emit mediaLoaded(), this one is usefull for DVD with angles/chapters/subtitles...
			//This must be changed, see MPlayerMediaObject::mediaLoaded()
			emit mediaLoaded();

			setState(PlayingState);
		}

		//Generic things
		if (rx.indexIn(line) > -1) {
			const QString tag = rx.cap(1);
			const QString value = rx.cap(2);

			if (tag == "ID_VIDEO_ID") {
				//First string to tell us that the media contains a video track
				_data.hasVideo = true;
				qDebug() << __FUNCTION__ << "Video:" << _data.hasVideo;
				emit hasVideoChanged(_data.hasVideo);
			}

			else if (tag == "ID_AUDIO_ID") {
				//First string to tell us that the media contains an audio track
				/*int ID = value.toInt();
				qDebug("MplayerProcess::parseLine: ID_AUDIO_ID: %d", ID);
				md.audios.addID( ID );*/
			}

			else if (tag == "ID_LENGTH") {
				_data.totalTime = (qint64) (value.toDouble() * SECONDS_CONVERTION);
				qDebug() << __FUNCTION__ << "Media total time:" << _data.totalTime;
				emit totalTimeChanged(_data.totalTime);
			}

			else if (tag == "ID_SEEKABLE") {
				_data.isSeekable = value.toInt();
				qDebug() << __FUNCTION__ << "Media seekable:" << _data.isSeekable;
				emit seekableChanged(_data.isSeekable);
			}

			else if (tag == "ID_VIDEO_WIDTH") {
				_data.videoWidth = value.toInt();
				qDebug() << __FUNCTION__ << "Video width:" << _data.videoWidth;
			}

			else if (tag == "ID_VIDEO_HEIGHT") {
				_data.videoHeight = value.toInt();
				qDebug() << __FUNCTION__ << "Video height:" << _data.videoHeight;
			}

			else if (tag == "ID_VIDEO_ASPECT") {
				_data.videoAspectRatio = value.toDouble();
				if (_data.videoAspectRatio == 0.0) {
					//I hope width & height are already set
					_data.videoAspectRatio = (double) _data.videoWidth / _data.videoHeight;
				}
				qDebug() << __FUNCTION__ << "Video aspect:" << _data.videoAspectRatio;
			}

			else if (tag == "ID_DVD_DISC_ID") {
				//_data.dvd_id = value;
				qDebug() << __FUNCTION__ << "DVD disc Id:" << value;
			}

			else if (tag == "ID_DEMUXER") {
				_data.demuxer = value;
			}

			else if (tag == "ID_VIDEO_FORMAT") {
				_data.videoFormat = value;
			}

			else if (tag == "ID_AUDIO_FORMAT") {
				_data.audioFormat = value;
			}

			else if (tag == "ID_VIDEO_BITRATE") {
				_data.videoBitrate = value.toInt();
			}

			else if (tag == "ID_VIDEO_FPS") {
				_data.videoFPS = value.toDouble();
				qDebug() << __FUNCTION__ << "Video FPS:" << _data.videoFPS;
			}

			else if (tag == "ID_AUDIO_BITRATE") {
				_data.audioBitrate = value.toInt();
			}

			else if (tag == "ID_AUDIO_RATE") {
				_data.audioRate = value.toInt();
			}

			else if (tag == "ID_AUDIO_NCH") {
				_data.audioNbChannels = value.toInt();
			}

			else if (tag == "ID_VIDEO_CODEC") {
				_data.videoCodec = value;
			}

			else if (tag == "ID_AUDIO_CODEC") {
				_data.audioCodec = value;
			}
		}
	}
}

void MPlayerProcess::finished(int, QProcess::ExitStatus) {
	//Send the finished() signal before the EndOfFileState one, otherwise
	//the playlist will start to play next file before all
	//objects are notified that the process has exited
	if (_endOfFileReached) {
		setState(EndOfFileState);
	}
}

void MPlayerProcess::setState(State state) {
	_state = state;
	emit stateChanged(state);
}

/*
void MPlayerProcess::parseSubtitle(const QString & subtitle) {
	qDebug() << __FUNCTION__ << subtitle;

	if (rx_subtitle.indexIn(text) > -1) {
		int id = rx_subtitle.cap(2).toInt();
		const QString type = rx_subtitle.cap(1);

		//SubData::Type t;
		if (type == "FILE_SUB") {
			//t = SubData::File;
		} else if (type == "VOBSUB") {
			//t = SubData::Vob;
		} else
			//t = SubData::Sub;
		}

		if (find(t, id) > -1) {
			qWarning("SubTracks::process: subtitle type: %d, id: %d already exists!", t, id);
		} else {
			add(t,id);
		}
	}

	else if (rx_sid.indexIn(text) > -1) {
		int id = rx_sid.cap(2).toInt();
		const QString value = rx_sid.cap(4);
		const QString attr = rx_sid.cap(3);
		const QString type = rx_sid.cap(1);

		//SubData::Type t = SubData::Sub;
		if (type == "VSID") {
			//t = SubData::Vob;
		}

		if (find(t, id) == -1) {
			qWarning("SubTracks::process: subtitle type: %d, id: %d doesn't exist!", t, id);
		} else {
			if (attr == "NAME") {
				changeName(t, id, value);
			} else {
				changeLang(t, id, value);
			}
		}
	}

	else if (rx_subtitle_file.indexIn(text) > -1) {
		QString file = rx_subtitle_file.cap(1);
		if (subs.count() > 0) {
			int last = subs.count() -1;
			if (subs[last].type() == SubData::File) {
				subs[last].setFilename(file);
			}
		}
	}
}
*/
