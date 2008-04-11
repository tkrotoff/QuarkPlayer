/*
 * MPlayer backend for the Phonon library
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

#include "MPlayerLoader.h"

#include "MPlayerProcess.h"

#include <QtCore/QtDebug>

MPlayerLoader * MPlayerLoader::_loader = NULL;

MPlayerLoader::MPlayerLoader(QObject * parent)
	: QObject(parent) {
}

MPlayerLoader::~MPlayerLoader() {
}

MPlayerLoader & MPlayerLoader::get() {
	if (!_loader) {
		_loader = new MPlayerLoader(NULL);
	}

	return *_loader;
}

MPlayerProcess * MPlayerLoader::createNewMPlayerProcess() {
	MPlayerProcess * process = new MPlayerProcess(this);

	_processList << process;

	return process;
}

void MPlayerLoader::restartMPlayerProcess(MPlayerProcess * process) {
	if (!process) {
		qCritical() << __FUNCTION__ << "Error: process cannot be NULL";
		return;
	}

	QStringList args;
	args << readMediaSettings();

	MediaData mediaData = process->getMediaData();
	if (!process->start(args, mediaData.filename, mediaData.videoWidgetId, mediaData.currentTime)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}
}

MPlayerProcess * MPlayerLoader::startMPlayerProcess(const QString & filename, int videoWidgetId, double seek) {
	MPlayerProcess * process = createNewMPlayerProcess();

	QStringList args;
	args << readMediaSettings();

	if (!process->start(args, filename, videoWidgetId, seek)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}

	return process;
}

MPlayerProcess * MPlayerLoader::loadMedia(const QString & filename) {
	MPlayerProcess * process = createNewMPlayerProcess();

	QStringList args;
	args << "-identify";
	args << "-frames";
	args << "0";

	if (!process->start(args, filename, 0, 0)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}

	return process;
}

QStringList MPlayerLoader::readMediaSettings() const {
	qDebug() << __FUNCTION__;

	QStringList args;

	args << "-noquiet";

	//Demuxer, audio and video codecs
	if (!_settings.forced_demuxer.isEmpty()) {
		args << "-demuxer";
		args << _settings.forced_demuxer;
	}
	if (!_settings.forced_audio_codec.isEmpty()) {
		args << "-ac";
		args << _settings.forced_audio_codec;
	}
	if (!_settings.forced_video_codec.isEmpty()) {
		args << "-vc";
		args << _settings.forced_video_codec;
	}

	args << "-sub-fuzziness";
	args << QString::number(1);

	args << "-identify";

	args << "-slave";

	args << "-vo";
#ifdef Q_OS_WIN
	args << "directx,";
#else
	args << "xv,";
#endif

	args << "-zoom";
	args << "-nokeepaspect";

	args << "-framedrop";

#ifndef Q_OS_WIN
	//args << "-input";
	//args << "conf=" + Helper::dataPath() +"/input.conf";
#endif

#ifndef Q_OS_WIN
	args << "-stop-xscreensaver";
#endif

	//Square pixels
	args << "-monitorpixelaspect";
	args << "1";

	args << "-subfont-autoscale";
	args << QString::number(1);

	args << "-subfont-text-scale";
	args << QString::number(_settings.sub_scale);

	args << "-subcp";
	args << "ISO-8859-1";

	/*if (_settings.current_audio_id != MediaSettings::NoneSelected) {
		args << "-aid";
		args << QString::number(_settings.current_audio_id);
	}*/

	args << "-subpos";
	args << QString::number(_settings.sub_pos);

	if (_settings.audio_delay != 0) {
		args << "-delay";
		args << QString::number((double) _settings.audio_delay / 1000);
	}

	if (_settings.sub_delay != 0) {
		args << "-subdelay";
		args << QString::number((double) _settings.sub_delay / 1000);
	}

	//Contrast, brightness...
	args << "-contrast";
	args << QString::number(_settings.contrast);

	args << "-brightness";
	args << QString::number(_settings.brightness);

	args << "-hue";
	args << QString::number(_settings.hue);

	args << "-saturation";
	args << QString::number(_settings.saturation);

	/*if (mdat.type == TYPE_DVD) {
		if (!dvd_folder.isEmpty()) {
			args << "-dvd-device";
			args << dvd_folder;
		} else {
			qWarning("Core::startMplayer: dvd device is empty!");
		}
	}*/

	/*if ((mdat.type==TYPE_VCD) || (mdat.type==TYPE_AUDIO_CD)) {
		if (!pref->cdrom_device.isEmpty()) {
			args << "-cdrom-device";
			args << pref->cdrom_device;
		}
	}*/

	if (_settings.speed != 1.0) {
		args << "-speed";
		args << QString::number(_settings.speed);
	}

	//Loads all the video filters
	QStringList videoFilters = _settings.videoFilters;
	if (!videoFilters.isEmpty()) {
		foreach (QString filter, videoFilters) {
			args << "-vf-add";
			args << filter;
		}
	}

	//Audio channels
	if (_settings.audio_use_channels != 0) {
		args << "-channels";
		args << QString::number(_settings.audio_use_channels);
	}

	//Stereo mode
	if (_settings.stereo_mode != 0) {
		args << "-stereo";
		args << QString::number(_settings.stereo_mode);
	}

	//Loads all the audio filters
	QStringList audioFilters = _settings.audioFilters;
	if (!audioFilters.isEmpty()) {
		foreach (QString filter, audioFilters) {
			args << "-af";
			args << filter;
		}
	}

	/*QString audioFilters = _settings.audioFilters.join(",");
	if (!audioFilters.isEmpty()) {
		args << "-af";
		args << audioFilters;
	}*/

	return args;
}
