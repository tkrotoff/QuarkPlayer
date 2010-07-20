/*
 * MPlayer backend for the Phonon library
 * Copyright (C) 2007-2009  Tanguy Krotoff <tkrotoff@gmail.com>
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

#include "MediaController.h"

#include "PhononMPlayerLogger.h"

#include "libmplayer/MPlayerLoader.h"
#include "libmplayer/MPlayerProcess.h"

#include <QtCore/QTime>

namespace Phonon
{
namespace MPlayer
{

MediaController::MediaController(QObject * parent)
	: QObject(parent) {

	//We could have done some 'lazy initizalition' here: only create a MPlayerProcess
	//when starting to read the file.
	//But we create a MPlayerProcess now so other classes like VideoWidget
	//can connect to the MPlayerProcess signals
	//1 MediaObject = 1 MPlayerProcess
	_process = MPlayerLoader::createNewMPlayerProcess(this);

	connect(_process, SIGNAL(audioChannelAdded(int, const AudioChannelData &)),
		SLOT(audioChannelAdded(int, const AudioChannelData &)));
	connect(_process, SIGNAL(subtitleAdded(int, const SubtitleData &)),
		SLOT(subtitleAdded(int, const SubtitleData &)));
	connect(_process, SIGNAL(subtitleChanged(int)),
		SLOT(subtitleChanged(int)));
	connect(_process, SIGNAL(titleAdded(int, qint64)),
		SLOT(titleAdded(int, qint64)));
	connect(_process, SIGNAL(titleChanged(int)),
		SIGNAL(titleChanged(int)));
	connect(_process, SIGNAL(chapterAdded(int, int)),
		SLOT(chapterAdded(int, int)));
	connect(_process, SIGNAL(mkvChapterAdded(int, const QString &, const QString &, const QString &)),
		SLOT(mkvChapterAdded(int, const QString &, const QString &, const QString &)));
	connect(_process, SIGNAL(angleAdded(int, int)),
		SLOT(angleAdded(int, int)));

	clearMediaController();
}

MediaController::~MediaController() {
}

void MediaController::clearMediaController() {
	_currentAudioChannel = Phonon::AudioChannelDescription();
	_availableAudioChannels.clear();

	_currentSubtitle = Phonon::SubtitleDescription();
	_availableSubtitles.clear();

	_currentAngle = 0;
	_availableAngles = 0;

#ifdef NEW_TITLE_CHAPTER_HANDLING
	_currentChapter = Phonon::ChapterDescription();
	_availableChapters.clear();
#else
	_currentChapter = 0;
	_availableChapters = 0;
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
	//FIXME if we use setCurrentTitle(), _currentTitle is changed
	//we don't want to loose the current title number
	//and get the default, first one
	//_currentTitle = Phonon::TitleDescription();
	///

	_availableTitles.clear();
#else
	//_currentTitle = 0;
	_availableTitles = 0;
#endif	//NEW_TITLE_CHAPTER_HANDLING

	_autoplayTitles = false;


	//Hack:
	//Create a fake subtitle called "None" with -1 for value
	//This special subtitle let's the user turn off subtitles cf ("sub_demux -1")
	SubtitleData subtitleData;
	subtitleData.name = "None";
	subtitleData.type = "SID";
	subtitleAdded(-1, subtitleData);
	subtitleChanged(-1);
}

bool MediaController::hasInterface(Interface iface) const {
	switch (iface) {
	case AddonInterface::NavigationInterface:
		return true;
		break;
	case AddonInterface::ChapterInterface:
		return true;
		break;
	case AddonInterface::AngleInterface:
		return true;
		break;
	case AddonInterface::TitleInterface:
		return true;
		break;
	case AddonInterface::SubtitleInterface:
		return true;
		break;
	case AddonInterface::AudioChannelInterface:
		return true;
		break;
	default:
		PhononMPlayerCritical() << "Unsupported AddonInterface::Interface" << iface;
	}

	return false;
}

QVariant MediaController::interfaceCall(Interface iface, int command, const QList<QVariant> & arguments) {
	switch (iface) {

	case AddonInterface::ChapterInterface:
		switch (static_cast<AddonInterface::ChapterCommand>(command)) {

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::availableChapters:
				return QVariant::fromValue(availableChapters());
#else
			case AddonInterface::availableChapters:
				return availableChapters();
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::currentChapter:
				return QVariant::fromValue(currentChapter());
#else
			case AddonInterface::chapter:
				return currentChapter();
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::setCurrentChapter:
				if (arguments.isEmpty() || !arguments.first().canConvert<ChapterDescription>()) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentChapter(arguments.first().value<ChapterDescription>());
				return true;
#else
			case AddonInterface::setChapter:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentChapter(arguments.first().toInt());
				return true;
#endif	//NEW_TITLE_CHAPTER_HANDLING

			default:
				PhononMPlayerCritical() << "Unsupported AddonInterface::ChapterInterface command:" << command;
		}
		break;

	case AddonInterface::TitleInterface:
		switch (static_cast<AddonInterface::TitleCommand>(command)) {

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::availableTitles:
				return QVariant::fromValue(availableTitles());
#else
			case AddonInterface::availableTitles:
				return availableTitles();
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::currentTitle:
				return QVariant::fromValue(currentTitle());
#else
			case AddonInterface::title:
				return currentTitle();
#endif	//NEW_TITLE_CHAPTER_HANDLING

#ifdef NEW_TITLE_CHAPTER_HANDLING
			case AddonInterface::setCurrentTitle:
				if (arguments.isEmpty() || !arguments.first().canConvert<TitleDescription>()) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentTitle(arguments.first().value<TitleDescription>());
				return true;
#else
			case AddonInterface::setTitle:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentTitle(arguments.first().toInt());
				return true;
#endif	//NEW_TITLE_CHAPTER_HANDLING

			case AddonInterface::autoplayTitles:
				return autoplayTitles();
			case AddonInterface::setAutoplayTitles:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Bool)) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setAutoplayTitles(arguments.first().toBool());
				return true;
			default:
				PhononMPlayerCritical() << "Unsupported AddonInterface::TitleInterface command:" << command;
		}
		break;

	case AddonInterface::AngleInterface:
		switch (static_cast<AddonInterface::AngleCommand>(command)) {
			case AddonInterface::availableAngles:
				return availableAngles();
			case AddonInterface::angle:
				return currentAngle();
			case AddonInterface::setAngle:
				if (arguments.isEmpty() || !arguments.first().canConvert(QVariant::Int)) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentAngle(arguments.first().toInt());
				return true;
				break;
			default:
				PhononMPlayerCritical() << "Unsupported AddonInterface::AngleInterface command:" << command;
		}
		break;

	case AddonInterface::SubtitleInterface:
		switch (static_cast<AddonInterface::SubtitleCommand>(command)) {
			case AddonInterface::availableSubtitles:
				return QVariant::fromValue(availableSubtitles());

			case AddonInterface::currentSubtitle:
				return QVariant::fromValue(currentSubtitle());

			case AddonInterface::setCurrentSubtitle:
				if (arguments.isEmpty() || !arguments.first().canConvert<SubtitleDescription>()) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentSubtitle(arguments.first().value<SubtitleDescription>());
				return true;

			default:
				PhononMPlayerCritical() << "Unsupported AddonInterface::SubtitleInterface command:" << command;
		}
		break;

	case AddonInterface::AudioChannelInterface:
		switch (static_cast<AddonInterface::AudioChannelCommand>(command)) {

			case AddonInterface::availableAudioChannels:
				return QVariant::fromValue(availableAudioChannels());

			case AddonInterface::currentAudioChannel:
				return QVariant::fromValue(currentAudioChannel());

			case AddonInterface::setCurrentAudioChannel:
				if (arguments.isEmpty() || !arguments.first().canConvert<AudioChannelDescription>()) {
					PhononMPlayerCritical() << "Arguments invalid";
					return false;
				}
				setCurrentAudioChannel(arguments.first().value<AudioChannelDescription>());
				return true;

			default:
				PhononMPlayerCritical() << "Unsupported AddonInterface::AudioChannelInterface command:" << command;
		}
		break;

	default:
		PhononMPlayerCritical() << "Unsupported AddonInterface::Interface:" << iface;
	}

	return QVariant();
}

//AudioChannel
void MediaController::audioChannelAdded(int id, const AudioChannelData & audioChannelData) {
	PhononMPlayerDebug();

	//This callback can get called 1 or several times,

	//The display string that will see the user
	QString displayString;
	displayString = audioChannelData.name;
	if (!audioChannelData.lang.isEmpty()) {
		if (displayString.isEmpty()) {
			displayString += audioChannelData.lang;
		} else {
			displayString += " [" + audioChannelData.lang + ']';
		}
	}
	///
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", displayString);
	properties.insert("description", QString());
	Phonon::AudioChannelDescription audioChannel(id, properties);

	//Check if the index already exist
	int existingIndex = -1;
	for (int i = 0; i < _availableAudioChannels.size(); i++) {
		if (_availableAudioChannels[i].index() == id) {
			existingIndex = i;
		}
	}
	if (existingIndex == -1) {
		_availableAudioChannels << audioChannel;
	} else {
		_availableAudioChannels.replace(existingIndex, audioChannel);
	}
}

void MediaController::setCurrentAudioChannel(const Phonon::AudioChannelDescription & audioChannel) {
	PhononMPlayerDebug();

	_currentAudioChannel = audioChannel;
	_process->sendCommand("switch_audio " + QString::number(_currentAudioChannel.index()));
}

QList<Phonon::AudioChannelDescription> MediaController::availableAudioChannels() const {
	return _availableAudioChannels;
}

Phonon::AudioChannelDescription MediaController::currentAudioChannel() const {
	return _currentAudioChannel;
}

//Subtitle
void MediaController::subtitleAdded(int id, const SubtitleData & subtitleData) {
	PhononMPlayerDebug();

	//This callback can get called 1 or several times,

	//The display string that will see the user
	QString displayString;
	displayString = subtitleData.name;
	if (!subtitleData.lang.isEmpty()) {
		if (displayString.isEmpty()) {
			displayString += subtitleData.lang;
		} else {
			displayString += " [" + subtitleData.lang + ']';
		}
	}
	///
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", displayString);
	properties.insert("description", QString());
	properties.insert("type", subtitleData.type);
	Phonon::SubtitleDescription subtitle(id, properties);

	//Check if the index already exist
	int existingIndex = -1;
	for (int i = 0; i < _availableSubtitles.size(); i++) {
		if (_availableSubtitles[i].index() == id) {
			existingIndex = i;
		}
	}
	if (existingIndex == -1) {
		_availableSubtitles << subtitle;
	} else {
		_availableSubtitles.replace(existingIndex, subtitle);
	}
}

void MediaController::subtitleChanged(int id) {
	foreach (Phonon::SubtitleDescription subtitle, _availableSubtitles) {
		if (subtitle.index() == id) {
			_currentSubtitle = subtitle;
			PhononMPlayerDebug() << "New current subtitle:" << _currentSubtitle;
			break;
		}
	}
}

void MediaController::loadSubtitleFile(const QString & fileName) {
	if (!fileName.isEmpty()) {
		//Loads the selected subtitle file
		QStringList args;
		args << "-sub";
		args << fileName;

		clearMediaController();
		MPlayerLoader::restart(_process, args);
	}
}

void MediaController::setCurrentSubtitle(const Phonon::SubtitleDescription & subtitle) {
	PhononMPlayerDebug();

	_currentSubtitle = subtitle;
	int id = _currentSubtitle.index();
	if (id == -1) {
		//sub_source [source]
		//  Display first subtitle from [source]. Here [source] is an integer:
		//  SUB_SOURCE_SUBS   (0) for file subs
		//  SUB_SOURCE_VOBSUB (1) for VOBsub files
		//  SUB_SOURCE_DEMUX  (2) for subtitle embedded in the media file or DVD subs.
		//  If [source] is -1, will turn off subtitle display. If [source] less than -1,
		//  will cycle between the first subtitle of each currently available sources.
		_process->sendCommand("sub_source -1");
	} else {
		QString type = _currentSubtitle.property("type").toString();
		if (type.compare("vob", Qt::CaseInsensitive) == 0) {
			_process->sendCommand("sub_vob " + QString::number(id));
		}

		else if (type.compare("sid", Qt::CaseInsensitive) == 0) {
			_process->sendCommand("sub_demux " + QString::number(id));
		}

		else if (type.compare("file", Qt::CaseInsensitive) == 0) {
			QString fileName = _currentSubtitle.property("name").toString();

			if (_availableSubtitles.contains(_currentSubtitle)) {
				//If already in the list of subtitles
				//then no need to load the subtitle and restart MPlayer
				_process->sendCommand("sub_file " + QString::number(id));
			} else {
				//This is a new subtitle file
				//We must load it and restart MPlayer
				loadSubtitleFile(fileName);
			}
		}

		else {
			PhononMPlayerCritical() << "Unknown subtitle type:" << type;
		}
	}
}

QList<Phonon::SubtitleDescription> MediaController::availableSubtitles() const {
	return _availableSubtitles;
}

Phonon::SubtitleDescription MediaController::currentSubtitle() const {
	return _currentSubtitle;
}

QString MediaController::convertMilliseconds(qint64 milliseconds) {
	QTime displayTime(
		(milliseconds / 3600000) % 60,
		(milliseconds / 60000) % 60,
		(milliseconds / 1000) % 60
	);

	return displayTime.toString("hh:mm:ss");
}

//Title
void MediaController::titleAdded(int id, qint64 length) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", id);
	properties.insert("description", convertMilliseconds(length));

	_availableTitles << Phonon::TitleDescription(id, properties);
#else
	Q_UNUSED(length);
	if (_availableTitles < id) {
		_availableTitles = id;
	}

	PhononMPlayerDebug() << "Titles: " << _availableTitles;
#endif	//NEW_TITLE_CHAPTER_HANDLING
}

#ifdef NEW_TITLE_CHAPTER_HANDLING

void MediaController::setCurrentTitle(const Phonon::TitleDescription & title) {
	//DVDNAV only
	//otherwise needs to restart MPlayerProcess
	//with parameter: dvd://titleNumber
	//FIXME No MPlayer build with DVDNAV support yet :/
	//_process->sendCommand("switch_title " + QString::number(titleNumber));

	clearMediaController();
	_currentTitle = title;
	MPlayerLoader::restart(_process, QStringList(), "dvd://" + QString::number(_currentTitle.index()), 0);
}

QList<Phonon::TitleDescription> MediaController::availableTitles() const {
	return _availableTitles;
}

Phonon::TitleDescription MediaController::currentTitle() const {
	return _currentTitle;
}

#else

void MediaController::setCurrentTitle(int titleNumber) {
	//DVDNAV only
	//otherwise needs to restart MPlayerProcess
	//with parameter: dvd://titleNumber
	//FIXME No MPlayer build with DVDNAV support yet :/
	//_process->sendCommand("switch_title " + QString::number(titleNumber));

	clearMediaController();
	_currentTitle = titleNumber;
	MPlayerLoader::restart(_process, QStringList(), "dvd://" + QString::number(_currentTitle), 0);
}

int MediaController::availableTitles() const {
	return _availableTitles;
}

int MediaController::currentTitle() const {
	return _currentTitle;
}

#endif	//NEW_TITLE_CHAPTER_HANDLING

void MediaController::setAutoplayTitles(bool autoplay) {
	_autoplayTitles = autoplay;
}

bool MediaController::autoplayTitles() const {
	return _autoplayTitles;
}

//Chapter

#ifdef NEW_TITLE_CHAPTER_HANDLING

void MediaController::chapterAdded(int titleId, int chapters) {
	//DVD chapter added
	if (_availableTitles.isEmpty() && (_currentTitle.index() == -1)) {
		//No default DVD title, let's set the default DVD title
		//This is because we get this from MPlayer:
		//
		//DVD example:
		//ID_DVD_TITLES=8
		//ID_DVD_TITLE_1_CHAPTERS=2
		//ID_DVD_TITLE_1_ANGLES=1
		//ID_DVD_TITLE_2_CHAPTERS=12
		//ID_DVD_TITLE_2_ANGLES=1
		//ID_DVD_TITLE_1_LENGTH=18.560
		//ID_DVD_TITLE_2_LENGTH=5055.000
		//ID_DVD_DISC_ID=6B5CDFED561E882B949047C87A88BCB4
		//ID_DVD_CURRENT_TITLE=1
		//
		//We get DVD chapters and angles infos before titles infos :/

		QHash<QByteArray, QVariant> properties;
		properties.insert("name", MPLAYER_DEFAULT_DVD_TITLE);

		_currentTitle = Phonon::TitleDescription(MPLAYER_DEFAULT_DVD_TITLE, properties);
	}

	if (titleId == _currentTitle.index()) {
		_availableChapters.clear();
		for (int i = 0; i < chapters; i++) {
			QHash<QByteArray, QVariant> properties;
			properties.insert("name", i + 1);
			properties.insert("description", QString());

			_availableChapters << Phonon::ChapterDescription(i, properties);
		}

		PhononMPlayerDebug() << "Chapters: " << _availableChapters.size();
	}
}

void MediaController::mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to) {
	Q_UNUSED(to);

	//HACK
	//id + 1 since MPlayer starts from 1
	id += 1;

	//Matroska chapter added
	QHash<QByteArray, QVariant> properties;
	properties.insert("name", QString::number(id) + ' ' + title + " (" + from + ')');
	properties.insert("description", QString());

	_availableChapters << Phonon::ChapterDescription(id, properties);
	PhononMPlayerDebug() << "MKV chapter id: " << id << "title:" << title;
}

void MediaController::setCurrentChapter(const Phonon::ChapterDescription & chapter) {
	PhononMPlayerDebug();

	_currentChapter = chapter;
	_process->sendCommand("seek_chapter " + QString::number(_currentChapter.index()) + " 1");
}

QList<Phonon::ChapterDescription> MediaController::availableChapters() const {
	return _availableChapters;
}

Phonon::ChapterDescription MediaController::currentChapter() const {
	return _currentChapter;
}

#else

void MediaController::chapterAdded(int titleId, int chapters) {
	//DVD chapter added
	if ((_availableTitles == 0) && (_currentTitle == 0)) {
		//No default DVD title, let's set the default DVD title
		//This is because we get this from MPlayer:

		//DVD example:
		//ID_DVD_TITLES=8
		//ID_DVD_TITLE_1_CHAPTERS=2
		//ID_DVD_TITLE_1_ANGLES=1
		//ID_DVD_TITLE_2_CHAPTERS=12
		//ID_DVD_TITLE_2_ANGLES=1
		//ID_DVD_TITLE_1_LENGTH=18.560
		//ID_DVD_TITLE_2_LENGTH=5055.000
		//ID_DVD_DISC_ID=6B5CDFED561E882B949047C87A88BCB4
		//ID_DVD_CURRENT_TITLE=1

		//We get DVD chapters and angles infos before titles infos :/

		_currentTitle = MPLAYER_DEFAULT_DVD_TITLE;
	}

	if (titleId == _currentTitle) {
		_availableChapters = chapters;
		PhononMPlayerDebug() << "Chapters: " << _availableChapters;
	}
}

void MediaController::mkvChapterAdded(int id, const QString & title, const QString & from, const QString & to) {
	Q_UNUSED(from);
	Q_UNUSED(to);

	//HACK
	//id + 1 since MPlayer starts from 1

	//Matroska chapter added
	if (_availableChapters < id + 1) {
		_availableChapters = id + 1;
		PhononMPlayerDebug() << "Chapter id: " << id << "title:" << title;
	}
}

void MediaController::setCurrentChapter(int chapterNumber) {
	PhononMPlayerDebug();

	_currentChapter = chapterNumber;
	//HACK
	//_currentChapter + 1
	//MPlayer seems to be buggy on this with .mkv files
	_process->sendCommand("seek_chapter " + QString::number(_currentChapter + 1) + " 1");
}

int MediaController::availableChapters() const {
	return _availableChapters;
}

int MediaController::currentChapter() const {
	return _currentChapter;
}

#endif	//NEW_TITLE_CHAPTER_HANDLING

//Angle
void MediaController::angleAdded(int titleId, int angles) {
#ifdef NEW_TITLE_CHAPTER_HANDLING
	if (titleId == _currentTitle.index()) {
#else
	if (titleId == _currentTitle) {
#endif	//NEW_TITLE_CHAPTER_HANDLING
		_availableAngles = angles;
		PhononMPlayerDebug() << "Angles: " << _availableAngles;
	}
}

void MediaController::setCurrentAngle(int angleNumber) {
	PhononMPlayerDebug();

	_currentAngle = angleNumber;
	_process->sendCommand("switch_angle " + QString::number(_currentAngle));
}

int MediaController::availableAngles() const {
	return _availableAngles;
}

int MediaController::currentAngle() const {
	return _currentAngle;
}

}}	//Namespace Phonon::MPlayer
