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

MPlayerLoader::MPlayerLoader(QObject * parent)
	: QObject(parent) {

}

MPlayerLoader::~MPlayerLoader() {
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

	MediaData mediaData = process->getMediaData();
	startMPlayerProcess(mediaData.filename, mediaData.videoWidgetId, mediaData.currentTime);
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

	if (_settings.current_audio_id != MediaSettings::NoneSelected) {
		args << "-aid";
		args << QString::number(_settings.current_audio_id);
	}

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

	if (_settings.flip) {
		args << "-flip";
	}

	//Video filters:
	//Phase
	if (_settings.phase_filter) {
		args << "-vf-add";
		args << "phase=A";
	}

	//Deinterlace
	if (_settings.current_deinterlacer != MediaSettings::NoDeinterlace) {
		args << "-vf-add";
		switch (_settings.current_deinterlacer) {
		case MediaSettings::L5:
			args << "pp=l5";
			break;
		case MediaSettings::Yadif:
			args << "yadif";
			break;
		case MediaSettings::LB:
			args << "pp=lb";
			break;
		case MediaSettings::Yadif_1:
			args << "yadif=1";
			break;
		case MediaSettings::Kerndeint:
			args << "kerndeint=5";
			break;
		}
	}

	//Rotate
	if (_settings.rotate != MediaSettings::NoRotate) {
		args << "-vf-add";
		args << QString("rotate=%1").arg(_settings.rotate);
	}

	//Denoise
	if (_settings.current_denoiser != MediaSettings::NoDenoise) {
		args << "-vf-add";
		if (_settings.current_denoiser == MediaSettings::DenoiseSoft) {
			args << "hqdn3d=2:1:2";
		} else {
			args << "hqdn3d";
		}
	}

	//Deblock
	if (_settings.deblock_filter) {
		args << "-vf-add";
		args << "pp=vb/hb";
	}

	//Dering
	if (_settings.dering_filter) {
		args << "-vf-add";
		args << "pp=dr";
	}

	//Upscale
	/*if (_settings.upscaling_filter) {
		int width = DesktopInfo::desktop_size(mplayerwindow).width();
		args << "-sws";
		args << "9";
		args << "-vf-add";
		args << "scale=" + QString::number(width)+":-2";
	}*/

	//Addnoise
	if (_settings.noise_filter) {
		args << "-vf-add";
		args << "noise=9ah:5ah";
	}

	//Postprocessing
	if (_settings.postprocessing_filter) {
		args << "-vf-add";
		args << "pp";
		args << "-autoq";
		args << QString::number(6);
	}


	//Letterbox (expand)
	/*
	if (_settings.add_letterbox) {
		args << "-vf-add";
		args << QString("expand=:::::%1,harddup").arg(DesktopInfo::desktop_aspectRatio(mplayerwindow));
		//Note: on some videos (h264 for instance) the subtitles doesn't disappear,
		//appearing the new ones on top of the old ones. It seems adding another
		//filter after expand fixes the problem. I chose harddup 'cos I think
		//it will be harmless in mplayer.
		//Anyway, if you know a proper way to fix the problem, please tell me.
	}
	*/

	//Additional video filters, supplied by user
	//File
	if (!_settings.mplayer_additional_video_filters.isEmpty()) {
		args << "-vf-add";
		args << _settings.mplayer_additional_video_filters;
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

	//Audio filters
	QString af = "";
	if (_settings.karaoke_filter) {
		af = "karaoke";
	}

	if (_settings.extrastereo_filter) {
		if (!af.isEmpty()) af += ",";
		af += "extrastereo";
	}

	if (_settings.volnorm_filter) {
		if (!af.isEmpty()) af += ",";
		af += "volnorm=2";
	}

	//Additional audio filters, supplied by user
	//Global
	if (!_settings.mplayer_additional_audio_filters.isEmpty()) {
		if (!af.isEmpty()) af += ",";
		af += _settings.mplayer_additional_audio_filters;
	}

	if (!af.isEmpty()) {
		args << "-af";
		args << af;
	}

	//Load edl file
	/*if (true) {
		QString edl_f;
		QFileInfo f(filename);
		QString basename = f.path() + "/" + f.completeBaseName();

		qDebug("Core::startMplayer: file basename: '%s'", basename.toUtf8().data());

		if (QFile::exists(basename + ".edl")) {
			edl_f = basename + ".edl";
		} else if (QFile::exists(basename + ".EDL")) {
			edl_f = basename + ".EDL";
		}

		qDebug("Core::startMplayer: edl file: '%s'", edl_f.toUtf8().data());
		if (!edl_f.isEmpty()) {
			args << "-edl";
			args << edl_f;
		}
	}*/

	//Additional options supplied by the user
	//File
	if (!_settings.mplayer_additional_options.isEmpty()) {
		QStringList args = _settings.mplayer_additional_options.split(" ");
		QStringList::Iterator it = args.begin();
		while (it != args.end()) {
 			args << (*it);
			++it;
		}
	}

	return args;
}
