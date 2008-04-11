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

MPlayerProcess::MPlayerProcess(QObject * parent)
	: MyProcess(parent) {

	connect(this, SIGNAL(lineAvailable(const QByteArray &)),
		SLOT(parseLine(const QByteArray &)));

	connect(this, SIGNAL(finished(int, QProcess::ExitStatus)),
		SLOT(finished(int, QProcess::ExitStatus)));
}

MPlayerProcess::~MPlayerProcess() {
}

bool MPlayerProcess::start(const QStringList & arguments, const QString & filename, int videoWidgetId, double seek) {
	//Stop MPlayerProcess if it is already running
	if (isRunning()) {
		stop();
	}

	_data.clear();

	_notifiedMPlayerIsRunning = false;

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
		args << QString::number(_data.currentTime);
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

	writeToStdin("quit");

	qDebug() << __FUNCTION__ << "Finishing MPlayer...";
	if (!waitForFinished(5000)) {
		kill();
	}

	qDebug() << __FUNCTION__ << "MPlayer finished";
}

void MPlayerProcess::writeToStdin(const QString & command) {
	qDebug() << __FUNCTION__ << "Command:" << command;

	if (isRunning()) {
		write(command.toLocal8Bit() + "\n");
	} else {
		qWarning() << __FUNCTION__ << "Error: MPlayer process not running";
	}
}

MediaData MPlayerProcess::getMediaData() const {
	return _data;
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
static QRegExp rx_connecting("^Connecting to .*");
static QRegExp rx_resolving("^Resolving .*");
static QRegExp rx_screenshot("^\\*\\*\\* screenshot '(.*)'");
static QRegExp rx_endoffile("^Exiting... \\(End of file\\)");
static QRegExp rx_mkvchapters("\\[mkv\\] Chapter (\\d+) from");
static QRegExp rx_aspect2("^Movie-Aspect is ([0-9,.]+):1");

//VCD
static QRegExp rx_vcd("^ID_VCD_TRACK_(\\d+)_MSF=(.*)");

//Audio CD
static QRegExp rx_cdda("^ID_CDDA_TRACK_(\\d+)_MSF=(.*)");

//Subtitles
static QRegExp rx_subtitle("^ID_(SUBTITLE|FILE_SUB|VOBSUB)_ID=(\\d+)");
static QRegExp rx_sid("^ID_(SID|VSID)_(\\d+)_(LANG|NAME)=(.*)");
static QRegExp rx_subtitle_file("^ID_FILE_SUB_FILENAME=(.*)");

//Clip info
static QRegExp rx_clip_name("^ (name|title): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_artist("^ artist: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_author("^ author: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_album("^ album: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_genre("^ genre: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_date("^ (creation date|year): (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_track("^ track: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_copyright("^ copyright: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_comment("^ comment: (.*)", Qt::CaseInsensitive);
static QRegExp rx_clip_software("^ software: (.*)", Qt::CaseInsensitive);

static QRegExp rx_stream_title("^.* StreamTitle='(.*)';StreamUrl='(.*)';");

void MPlayerProcess::parseLine(const QByteArray & tmp) {
	//qDebug() << __FUNCTION__ << "Line:" << tmp;

	QString line = QString::fromLocal8Bit(tmp);

	//Skip empty lines
	if (line.isEmpty()) {
		return;
	}

	//Parse A: V: line
	//qDebug("%s", line.toUtf8().data());
	if (rx_av.indexIn(line) > -1) {
		double seconds = rx_av.cap(1).toDouble();
		//qDebug("cap(1): '%s'", rx_av.cap(1).toUtf8().data());
		//qDebug("sec: %f", sec);

		if (!_notifiedMPlayerIsRunning) {
			qDebug("MPlayerProcess::parseLine: starting sec: %f", seconds);
			emit receivedStartingTime(seconds);
			emit mplayerFullyLoaded();
			_notifiedMPlayerIsRunning = true;
		}

		_data.currentTime = seconds;
		emit tick(seconds);

		//Check for frame
		if (rx_frame.indexIn(line) > -1) {
			int frame = rx_frame.cap(1).toInt();
			//qDebug(" frame: %d", frame);
			emit receivedCurrentFrame(frame);
		}
	}

	else {
		//Emulates mplayer version in Ubuntu:
		//if (line.startsWith("MPlayer 1.0rc1")) line = "MPlayer 2:1.0~rc1-0ubuntu13.1 (C) 2000-2006 MPlayer Team";

		emit lineAvailable(line);

		//Parse other things
		qDebug() << __FUNCTION__ << "Line:" << line;

		//Screenshot
		if (rx_screenshot.indexIn(line) > -1) {
			QString filename = rx_screenshot.cap(1);
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
			if (!_notifiedMPlayerIsRunning) {
				emit mplayerFullyLoaded();
			}

			//Sends the endOfFile signal once the process is finished, not now!
			_endOfFileReached = true;
		}

		//Window resolution
		else if (rx_winresolution.indexIn(line) > -1) {
			/*
			_data.win_width = rx_winresolution.cap(4).toInt();
			_data.win_height = rx_winresolution.cap(5).toInt();
			_data.video_aspect = (double) _data.win_width / _data.win_height;
			*/

			int width = rx_winresolution.cap(4).toInt();
			int height = rx_winresolution.cap(5).toInt();

			emit receivedVO(rx_winresolution.cap(1));
			emit receivedWindowResolution(width, height);
			//emit mplayerFullyLoaded();
		}

		//No video
		else if (rx_novideo.indexIn(line) > -1) {
			_data.novideo = TRUE;
			emit hasNoVideo();
			//emit mplayerFullyLoaded();
		}

		//Pause
		else if (rx_paused.indexIn(line) > -1) {
			emit pause();
		}

		//Stream title
		if (rx_stream_title.indexIn(line) > -1) {
			QString title = rx_stream_title.cap(1);
			QString url = rx_stream_title.cap(2);
			qDebug() << __FUNCTION__ << "Stream title:" << title;
			qDebug() << __FUNCTION__ << "Stream url:" << url;
			_data.stream_title = title;
			_data.stream_url = url;
			emit streamTitleAndUrl(title, url);
		}

		//The following things are not sent when the file has started to play
		//(or if sent, GUI will ignore anyway...)
		//So not process anymore, if video is playing to save some time
		if (_notifiedMPlayerIsRunning) {
			return;
		}

		if ((_mplayerSvnRevision == -1) && (line.startsWith("MPlayer "))) {
			_mplayerSvnRevision = MPlayerVersion::parse(line);
			if (_mplayerSvnRevision <= 0) {
				emit failedToParseMplayerVersion(line);
			}
		}

		//Subtitles
		/*if (rx_subtitle.indexIn(line) > -1) {
			_data.subs.process(line);
		}
		else
		if (rx_sid.indexIn(line) > -1) {
			_data.subs.process(line);
		}
		else
		if (rx_subtitle_file.indexIn(line) > -1) {
			_data.subs.process(line);
		}*/

		//AO
		if (rx_ao.indexIn(line) > -1) {
			emit receivedAO(rx_ao.cap(1));
		}
		else

		//Matroska audio
		/*if (rx_audio_mat.indexIn(line) > -1) {
			int ID = rx_audio_mat.cap(1).toInt();
			QString lang = rx_audio_mat.cap(3);
			QString t = rx_audio_mat.cap(2);
			qDebug("MPlayerProcess::parseLine: Audio: ID: %d, Lang: '%s' Type: '%s'",
				ID, lang.toUtf8().data(), t.toUtf8().data());

			if (t == "NAME")
				_data.audios.addName(ID, lang);
			else
				_data.audios.addLang(ID, lang);
		}
		else

		//Matroshka chapters
		if (rx_mkvchapters.indexIn(line)!=-1) {
			int c = rx_mkvchapters.cap(1).toInt();
			qDebug("MPlayerProcess::parseLine: mkv chapters: %d", c);
			if ((c+1) > _data.mkv_chapters) {
				_data.mkv_chapters = c+1;
				qDebug("MPlayerProcess::parseLine: mkv_chapters set to: %d", _data.mkv_chapters);
			}
		}
		else

		//VCD titles
		if (rx_vcd.indexIn(line) > -1) {
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
		else

		//DVD titles
		if (rx_title.indexIn(line) > -1) {
			int ID = rx_title.cap(1).toInt();
			QString t = rx_title.cap(2);

			if (t=="LENGTH") {
				double length = rx_title.cap(3).toDouble();
				qDebug("MPlayerProcess::parseLine: Title: ID: %d, Length: '%f'", ID, length);
				_data.titles.addDuration(ID, length);
			}
			else
			if (t=="CHAPTERS") {
				int chapters = rx_title.cap(3).toInt();
				qDebug("MPlayerProcess::parseLine: Title: ID: %d, Chapters: '%d'", ID, chapters);
				_data.titles.addChapters(ID, chapters);
			}
			else
			if (t=="ANGLES") {
				int angles = rx_title.cap(3).toInt();
				qDebug("MPlayerProcess::parseLine: Title: ID: %d, Angles: '%d'", ID, angles);
				_data.titles.addAngles(ID, angles);
			}
		}
		else*/

		//Catch cache messages
		if (rx_cache.indexIn(line) > -1) {
			emit receivedCacheMessage(line);
		}
		else

		//Creating index
		if (rx_create_index.indexIn(line) > -1) {
			emit receivedCreatingIndex(line);
		}
		else

		//Catch connecting message
		if (rx_connecting.indexIn(line) > -1) {
			emit receivedConnectingToMessage(line);
		}
		else

		//Catch resolving message
		if (rx_resolving.indexIn(line) > -1) {
			emit receivedResolvingMessage(line);
		}
		else

		//Aspect ratio for old versions of mplayer
		if (rx_aspect2.indexIn(line) > -1) {
			_data.video_aspect = rx_aspect2.cap(1).toDouble();
			qDebug("MPlayerProcess::parseLine: _data.video_aspect set to %f", _data.video_aspect);
		}
		else

		//Clip info

		//QString::trimmed() is used for removing leading and trailing whitespaces
		//Some .mp3 files contain tags with starting and ending whitespaces
		//Unfortunately MPlayer gives us leading and trailing whitespaces,
		//Winamp for example doesn't show them

		//Name
		if (rx_clip_name.indexIn(line) > -1) {
			QString s = rx_clip_name.cap(2).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_name: '%s'", s.toUtf8().data());
			_data.clip_name = s;
		}

		//Artist
		else if (rx_clip_artist.indexIn(line) > -1) {
			QString s = rx_clip_artist.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_artist: '%s'", s.toUtf8().data());
			_data.clip_artist = s;
		}

		//Author
		else if (rx_clip_author.indexIn(line) > -1) {
			QString s = rx_clip_author.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_author: '%s'", s.toUtf8().data());
			_data.clip_author = s;
		}

		//Album
		else if (rx_clip_album.indexIn(line) > -1) {
			QString s = rx_clip_album.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_album: '%s'", s.toUtf8().data());
			_data.clip_album = s;
		}

		//Genre
		else if (rx_clip_genre.indexIn(line) > -1) {
			QString s = rx_clip_genre.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_genre: '%s'", s.toUtf8().data());
			_data.clip_genre = s;
		}

		//Date
		else if (rx_clip_date.indexIn(line) > -1) {
			QString s = rx_clip_date.cap(2).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_date: '%s'", s.toUtf8().data());
			_data.clip_date = s;
		}

		//Track
		else if (rx_clip_track.indexIn(line) > -1) {
			QString s = rx_clip_track.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_track: '%s'", s.toUtf8().data());
			_data.clip_track = s;
		}

		//Copyright
		else if (rx_clip_copyright.indexIn(line) > -1) {
			QString s = rx_clip_copyright.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_copyright: '%s'", s.toUtf8().data());
			_data.clip_copyright = s;
		}

		//Comment
		else if (rx_clip_comment.indexIn(line) > -1) {
			QString s = rx_clip_comment.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_comment: '%s'", s.toUtf8().data());
			_data.clip_comment = s;
		}

		//Software
		else if (rx_clip_software.indexIn(line) > -1) {
			QString s = rx_clip_software.cap(1).trimmed();
			qDebug("MPlayerProcess::parseLine: clip_software: '%s'", s.toUtf8().data());
			_data.clip_software = s;
		}
		else

		// Catch starting message
		/*
		pos = rx_play.indexIn(line);
		if (pos > -1) {
			emit mplayerFullyLoaded();
		}
		*/

		//Generic things
		if (rx.indexIn(line) > -1) {
			QString tag = rx.cap(1);
			QString value = rx.cap(2);
			//qDebug("MPlayerProcess::parseLine: tag: %s, value: %s", tag.toUtf8().data(), value.toUtf8().data());

			// Generic audio
			/*
			if (tag == "ID_AUDIO_ID") {
				int ID = value.toInt();
				qDebug("MPlayerProcess::parseLine: ID_AUDIO_ID: %d", ID);
				_data.audios.addID(ID);
			}
			else*/
			if (tag == "ID_LENGTH") {
				_data.totalTime = value.toDouble();
				qDebug() << __FUNCTION__ << "Media total time:" << _data.totalTime;
			}
			else if (tag == "ID_VIDEO_WIDTH") {
				_data.video_width = value.toInt();
				qDebug("MPlayerProcess::parseLine: _data.video_width set to %d", _data.video_width);
			}
			else if (tag == "ID_VIDEO_HEIGHT") {
				_data.video_height = value.toInt();
				qDebug("MPlayerProcess::parseLine: _data.video_height set to %d", _data.video_height);
			}
			else if (tag == "ID_VIDEO_ASPECT") {
				_data.video_aspect = value.toDouble();
				if (_data.video_aspect == 0.0) {
					//I hope width & height are already set
					_data.video_aspect = (double) _data.video_width / _data.video_height;
				}
				qDebug("MPlayerProcess::parseLine: _data.video_aspect set to %f", _data.video_aspect);
			}
			else if (tag == "ID_DVD_DISC_ID") {
				_data.dvd_id = value;
				qDebug("MPlayerProcess::parseLine: _data.dvd_id set to '%s'", _data.dvd_id.toUtf8().data());
			}
			else if (tag == "ID_DEMUXER") {
				_data.demuxer = value;
			}
			else if (tag == "ID_VIDEO_FORMAT") {
				_data.video_format = value;
			}
			else if (tag == "ID_AUDIO_FORMAT") {
				_data.audio_format = value;
			}
			else if (tag == "ID_VIDEO_BITRATE") {
				_data.video_bitrate = value.toInt();
			}
			else if (tag == "ID_VIDEO_FPS") {
				_data.video_fps = value;
			}
			else if (tag == "ID_AUDIO_BITRATE") {
				_data.audio_bitrate = value.toInt();
			}
			else if (tag == "ID_AUDIO_RATE") {
				_data.audio_rate = value.toInt();
			}
			else if (tag == "ID_AUDIO_NCH") {
				_data.audio_nch = value.toInt();
			}
			else if (tag == "ID_VIDEO_CODEC") {
				_data.video_codec = value;
			}
			else if (tag == "ID_AUDIO_CODEC") {
				_data.audio_codec = value;
			}
		}
	}
}

void MPlayerProcess::finished(int exitCode, QProcess::ExitStatus exitStatus) {
	//Send this signal before the endOfFile one, otherwise
	//the playlist will start to play next file before all
	//objects are notified that the process has exited
	emit finished();
	if (_endOfFileReached) {
		emit endOfFile();
	}
}
