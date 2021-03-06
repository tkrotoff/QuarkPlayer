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

#include "FileTypes.h"

#include "FileTypesLogger.h"

#include <QtCore/QHash>
#include <QtCore/QFileInfo>
#include <QtCore/QSet>

static const int FILETYPELIST_SIZE = 64;

//See http://en.wikipedia.org/wiki/List_of_file_formats
static const FileType FILETYPELIST[FILETYPELIST_SIZE] = {
	//{ FileType::Category, FileType::Name, "fullName", "wikipediaArticle", "extensions", "mimeTypes" },

	//Empty type
	{ FileType::CategoryUnknown, FileType::NameUnknown, "", "", "", "" },

	//Video
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Video
	{ FileType::Video, FileType::_3GP, "3GP", "3GP", "3gp,3g2", "video/3gpp,audio/3gpp" },
	{ FileType::Video, FileType::ASF, "Advanced Systems Format (ASF)", "Advanced_Systems_Format", "asf", "video/x-ms-asf,application/vnd.ms-asf" },
	{ FileType::Video, FileType::AVI, "Audio Video Interleave (AVI)", "AVI", "avi", "video/avi,video/msvideo,video/x-msvideo" },
	{ FileType::Video, FileType::MPEG1, "MPEG-1", "MPEG-1", "mpg,mpeg,mp1,m1v,m1a,m2a,mpa,mpv", "audio/mpeg,video/mpeg" },
	{ FileType::Video, FileType::FLV, "Flash Video (FLV)", "Flash_Video", "flv,f4v,f4p,f4a,f4b", "video/x-flv,video/mp4,video/x-m4v,audio/mp4a-latm,video/3gpp,video/quicktime,audio/mp4" },
	{ FileType::Video, FileType::SWF, "Adobe Flash (SWF)", "SWF", "swf", "application/x-shockwave-flash" },
	{ FileType::Video, FileType::MKV, "Matroska (MKV)", "Matroska", "mkv,mka,mks", "video/x-matroska,audio/x-matroska" },
	{ FileType::Video, FileType::MOV, "QuickTime Movie (MOV)", ".mov", "mov,qt", "video/quicktime" },
	{ FileType::Video, FileType::MP4, "MPEG-4 Part 14 (MP4)", "MPEG-4_Part_14", "mp4", "video/mp4" },
	{ FileType::Video, FileType::NSV, "Nullsoft Streaming Video (NSV)", "Nullsoft_Streaming_Video", "nsv", "application/x-winamp" },
	{ FileType::Video, FileType::OGM, "Ogg Media (OGM)", "Ogg_Media", "ogm", "application/ogg,application/x-ogg,video/x-ogg" },
	{ FileType::Video, FileType::Tarkin, "Tarkin", "Tarkin_(codec)", "", "video/x-ogg-tarkin,application/ogg-tarkin" },
	{ FileType::Video, FileType::Theora, "Theora", "Theora", "ogv,ogg", "video/ogg,video/x-ogg" },
	{ FileType::Video, FileType::RealMedia, "RealMedia", "RealMedia", "rv,rmvb", "application/vnd.rn-realmedia" },
	{ FileType::Video, FileType::WMV, "Windows Media Video (WMV)", "Windows_Media_Video", "wmv", "video/x-ms-wmv" },
	{ FileType::Video, FileType::DivX, "DivX Media Format (DMF)", ".divx", "divx", "video/divx" },
	{ FileType::Video, FileType::Xvid, "Xvid", "Xvid", "avi,xvid", "video/x-xvid" },
	{ FileType::Video, FileType::NUT, "NUT", "NUT_Container", "nut", "" },
	{ FileType::Video, FileType::Smacker, "Smacker", "Smacker_video", "smk", "video/x-smacker" },
	{ FileType::Video, FileType::Bink, "Bink", "Bink_Video", "bik", "video/x-bink" },
	{ FileType::Video, FileType::VOB, "VOB (DVD Video)", "VOB", "VOB", "video/vob,video/mp2t" },
	{ FileType::Video, FileType::EVO, "EVO (HD DVD Video)", "Enhanced_VOB", "EVO", "" },

	//Audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Sound_and_music

	//Lossy audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Lossy_audio
	{ FileType::Audio, FileType::MP2, "MPEG-1 Audio Layer 2 (MP2)", "MPEG-1_Audio_Layer_II", "mp2", "audio/mpeg" },
	{ FileType::Audio, FileType::MP3, "MPEG-1 Audio Layer 3 (MP3)", "MP3", "mp3", "audio/mpeg" },
	{ FileType::Audio, FileType::Speex, "Speex", "Speex", "spx", "audio/speex" },
	{ FileType::Audio, FileType::Vorbis, "Vorbis", "Vorbis", "ogg,oga", "audio/ogg,audio/x-ogg,audio/vorbis" },
	{ FileType::Audio, FileType::AAC, "Advanced Audio Coding (AAC)", "Advanced_Audio_Coding", "m4a,m4b,m4p,m4v,m4r,3gp,mp4,aac", "audio/aac,audio/aacp" },
	{ FileType::Audio, FileType::WMA, "Windows Media Audio (WMA)", "Windows_Media_Audio", "wma", "audio/x-ms-wma" },
	{ FileType::Audio, FileType::MPC, "Musepack (MPC)", "Musepack", "mpc,mp+,mpp", "audio/x-musepack,audio/musepack" },
	{ FileType::Audio, FileType::VQF, "TwinVQ (VQF)", "TwinVQ", "vqf", "audio/x-twinvq" },
	{ FileType::Audio, FileType::RealAudio, "RealAudio", "RealAudio", "ra,ram,rm", "audio/vnd.rn-realaudio,audio/x-pn-realaudio,audio/x-pn-realaudioplugin" },
	{ FileType::Audio, FileType::MID, "MIDI File (SMF)", "Musical_Instrument_Digital_Interface", "mid,midi", "application/x-midi,audio/midi,audio/x-mid,audio/x-midi" },
	{ FileType::Audio, FileType::MOD, "MOD", "MOD_(file_format)", "mod", "audio/mod,audio/x-mod" },

	//Lossless audio
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Lossless_audio
	{ FileType::Audio, FileType::AIFF, "Audio Interchange File Format (AIFF)", "Audio_Interchange_File_Format", "aiff,aif,aifc", "audio/x-aiff,audio/aiff" },
	{ FileType::Audio, FileType::AU, "Au", "Au_file_format", "au,snd", "audio/basic" },
	{ FileType::Audio, FileType::CDDA, "CD Audio Track", "Red_Book_(audio_CD_standard)", "cda", "audio/x-cda" },
	{ FileType::Audio, FileType::WAV, "Waveform (WAV)", "WAV", "wav", "audio/wav,audio/wave,audio/x-wav" },
	{ FileType::Audio, FileType::FLAC, "Free Lossless Audio Codec (FLAC)", "Free_Lossless_Audio_Codec", "flac", "audio/x-flac,audio/x-oggflac" },
	{ FileType::Audio, FileType::AppleLossless, "Apple Lossless (M4A)", "Apple_Lossless", "m4a", "" },
	{ FileType::Audio, FileType::APE, "Monkey's Audio (APE)", "Monkey's_Audio", "ape", "audio/x-ape,application/x-extension-ape" },
	{ FileType::Audio, FileType::TTA, "True Audio (TTA)", "TTA_(codec)", "tta", "audio/x-tta,audio/tta" },
	{ FileType::Audio, FileType::WavPack, "WavPack (WV)", "WavPack", "wv", "audio/x-wavpack" },
	{ FileType::Audio, FileType::OptimFROG, "OptimFROG", "OptimFROG", "ofr", "" },
	{ FileType::Audio, FileType::Shorten, "Shorten (SHN)", "Shorten", "shn", "application/x-shorten" },

	//Playlist
	//See http://en.wikipedia.org/wiki/List_of_file_formats#Playlist_formats
	{ FileType::Playlist, FileType::ASX, "Advanced Stream Redirector (ASX)", "Advanced_Stream_Redirector", "asx,wax,wvx", "video/x-ms-asf" },
	{ FileType::Playlist, FileType::M3U, "M3U", "M3U", "m3u,m3u8", "audio/x-mpegurl" },
	{ FileType::Playlist, FileType::PLS, "PLS", "PLS_(file_format)", "pls", "audio/x-scpls" },
	{ FileType::Playlist, FileType::XSPF, "XML Shareable Playlist Format (XSPF)", "XML_Shareable_Playlist_Format", "xspf", "application/xspf+xml" },
	{ FileType::Playlist, FileType::WPL, "Windows Media Player Playlist (WPL)", "WPL", "wpl", "application/vnd.ms-wpl" },
	{ FileType::Playlist, FileType::CUE, "CUE Sheet", ".cue", "cue", "application/x-cue" },

	//Subtitle
	//See http://en.wikipedia.org/wiki/Subtitle_(captioning)#Subtitle_formats
	{ FileType::Subtitle, FileType::AQTitle, "AQTitle (AQT)", "AQTitle", "aqt", "" },
	{ FileType::Subtitle, FileType::MicroDVD, "MicroDVD", "MicroDVD", "sub", "" },
	{ FileType::Subtitle, FileType::MPsub, "MPsub", "MPlayer", "sub", "" },
	{ FileType::Subtitle, FileType::RealText, "RealText", "Synchronized_Multimedia_Integration_Language", "rt", "" },
	{ FileType::Subtitle, FileType::SAMI, "Synchronized Accessible Media Interchange (SAMI)", "SAMI", "smi,sami", "" },
	{ FileType::Subtitle, FileType::SSF, "Structured Subtitle Format (SSF)", "Structured_Subtitle_Format", "ssf", "" },
	{ FileType::Subtitle, FileType::SubRip, "SubRip", "SubRip", "srt", "" },
	{ FileType::Subtitle, FileType::SSA, "SubStation Alpha (SSA)", "SubStation_Alpha", "ssa,ass", "" },
	{ FileType::Subtitle, FileType::SubViewer, "SubViewer", "SubViewer", "sub", "" },
	{ FileType::Subtitle, FileType::USF, "Universal Subtitle Format (USF)", "Universal_Subtitle_Format", "usf", "" },
	{ FileType::Subtitle, FileType::VOBsub, "VOBsub", "VSFilter", "sub,idx", "" },
	{ FileType::Subtitle, FileType::NameUnknown, "", "", "utf", "" },
	{ FileType::Subtitle, FileType::NameUnknown, "", "", "txt", "" }
};


QList<FileType> FileTypes::fileTypes() {
	static QList<FileType> list;

	if (list.isEmpty()) {
		for (int i = 0; i < FILETYPELIST_SIZE; i++) {
			FileType fileType = FILETYPELIST[i];
			list += fileType;
		}
	}

	return list;
}

QStringList FileTypes::extensions(FileType::Category category) {
	static QHash<FileType::Category, QStringList> hash;

	if (!hash.contains(category)) {
		//We use QSet then there is no duplicated element
		QSet<QString> set;
		foreach (FileType fileType, fileTypes()) {
			if (fileType.category == category) {
				set += QSet<QString>::fromList(fileType.extensions.split(",", QString::SkipEmptyParts));
			}
		}
		hash[category] = QStringList::fromSet(set);
	}

	return hash.value(category);
}

QStringList FileTypes::extensions(FileType::Category category1, FileType::Category category2) {
	int superCategory = (int) category1 + (int) category2;
	static QHash<int, QStringList> hash;

	if (!hash.contains(superCategory)) {
		//We use QSet then there is no duplicated element
		QSet<QString> set;
		set += QSet<QString>::fromList(extensions(category1));
		set += QSet<QString>::fromList(extensions(category2));
		hash[superCategory] = QStringList::fromSet(set);
	}

	return hash.value(superCategory);
}

bool FileTypes::fileExtensionMatches(const QString & fileName, const QStringList & extensions) {
	bool match = false;

	if (extensions.isEmpty()) {
		match = true;
	} else {
		//QFileInfo::completeSuffix() -> archive.tar.gz -> tar.gz
		//QFileInfo::suffix() -> archive.tar.gz -> gz
		foreach (QString extension, extensions) {
			if (QFileInfo(fileName).completeSuffix().contains(extension, Qt::CaseInsensitive)) {
				match = true;
				break;
			}
		}
	}

	return match;
}

FileType FileTypes::fileType(const QString & extension) {
	static QHash<QString, FileType> hash;

	QString ext = extension.toLower();

	if (!hash.contains(ext)) {
		foreach (FileType fileType, fileTypes()) {
			if (fileType.extensions.contains(ext)) {
				//The first one that we found
				hash[ext] = fileType;
				break;
			}
		}

		//The ext string was not found :/
		if (!hash.contains(ext)) {
			FileType unknownFileType;
			unknownFileType.category = FileType::CategoryUnknown;
			unknownFileType.name = FileType::NameUnknown;
			unknownFileType.fullName = ext;
			unknownFileType.extensions += ext;
			hash[ext] = unknownFileType;
		}
	}

	return hash.value(ext);
}

FileType FileTypes::fileType(FileType::Name name) {
	static QHash<FileType::Name, FileType> hash;

	if (!hash.contains(name)) {
		foreach (FileType fileType, fileTypes()) {
			if (fileType.name == name) {
				hash[name] = fileType;
				break;
			}
		}

		//The extension string was not found :/
		if (!hash.contains(name)) {
			FileType unknownFileType;
			unknownFileType.category = FileType::CategoryUnknown;
			unknownFileType.name = FileType::NameUnknown;
			hash[name] = unknownFileType;
		}
	}

	return hash.value(name);
}

QString FileTypes::toFilterFormat(const QStringList & extensions) {
	QString tmp;
	foreach (QString ext, extensions) {
		tmp += "*." + ext + ' ';
	}
	tmp = tmp.trimmed();
	tmp.prepend(" (");
	tmp.append(')');
	FileTypesDebug() << tmp;
	return tmp;
}

QString FileTypes::toSaveFilterFormat(const QStringList & extensions, const QString & defaultExtension) {
	QString tmp;
	foreach (QString ext, extensions) {
		FileType type = fileType(ext);
		QString name(type.fullName);
		name.replace(QRegExp("\\(.*\\)"), QString());
		name = name.trimmed();
		if (ext != defaultExtension) {
			tmp += name + " (*." + ext + ");;";
		} else {
			tmp.prepend(name + " (*." + ext + ");;");
		}
	}
	return tmp;
}
