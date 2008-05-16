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
#include "MediaSettings.h"

#include <QtCore/QtDebug>

MediaSettings MPlayerLoader::settings;

MPlayerProcess * MPlayerLoader::createNewMPlayerProcess(QObject * parent) {
	MPlayerProcess * process = new MPlayerProcess(parent);

	return process;
}

void MPlayerLoader::restart(MPlayerProcess * process, const QStringList & arguments, const QString & filename, qint64 seek) {
	if (!process) {
		qCritical() << __FUNCTION__ << "Error: process cannot be NULL";
		return;
	}

	QStringList args;
	args << readMediaSettings();
	if (!arguments.isEmpty()) {
		args << arguments;
	}

	MediaData mediaData = process->getMediaData();
	if (!filename.isEmpty()) {
		mediaData.filename = filename;
	}
	if (seek != -1) {
		mediaData.currentTime = seek;
	}

	if (!process->start(args, mediaData.filename, mediaData.videoWidgetId, mediaData.currentTime)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}
}

void MPlayerLoader::start(MPlayerProcess * process, const QString & filename, int videoWidgetId, qint64 seek) {
	if (!process) {
		qCritical() << __FUNCTION__ << "Error: process cannot be NULL";
		return;
	}

	QStringList args;
	args << readMediaSettings();

	//Optimisation: no -identify if we are reading a dvd
	if (filename.contains("dvd://")) {
		qDebug() << __FUNCTION__ << "DVD detected";
		//args.removeAll("-identify");
	}

	if (!process->start(args, filename, videoWidgetId, seek)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}
}

void MPlayerLoader::loadMedia(MPlayerProcess * process, const QString & filename) {
	if (!process) {
		qCritical() << __FUNCTION__ << "Error: process cannot be NULL";
		return;
	}

	//Optimisation: no -identify if we are reading a dvd
	if (filename.contains("dvd://")) {
		qDebug() << __FUNCTION__ << "DVD detected";
		return;
	}

	QStringList args;
	args << "-identify";
	args << "-frames";
	args << "0";
	args << "-vo";
	args << "null";
	args << "-ao";
	args << "null";

	if (!process->start(args, filename, 0, 0)) {
		//Error handling
		qCritical() << __FUNCTION__ << "error: MPlayer process couldn't start";
	}
}

QStringList MPlayerLoader::readMediaSettings() {
	qDebug() << __FUNCTION__;

	QStringList args;

	//Force "no quiet output", otherwise we can't get stream position for example
	//MPlayer default option is -quiet
	args << "-noquiet";

	//MPlayer slave mode
	args << "-slave";

	//Shorthand for -msglevel identify=4. Show file parameters in an easily parseable format.
	//Also prints more detailed information about subtitle and audio track languages and IDs.
	//In some cases you can get more information by using -msglevel identify=6.
	//For example, for a DVD it will list the chapters and time length of each title,
	//as well as a disk ID. Combine this with -frames 0 to suppress all output.
	//The wrapper script TOOLS/midentify suppresses the other MPlayer output and (hopefully)
	//shellescapes the filenames.
	args << "-identify";

	//Get mkv files informations
	args << "-msglevel";
	args << "demux=6";

	//Drops frames on a slow computer
	args << "-framedrop";

	//Do not keep window aspect ratio when resizing windows.
	//Only works with the x11, xv, xmga, xvidix, directx video output drivers.
	//Furthermore under X11 your window manager has to honor window aspect hints.
	args << "-nokeepaspect";

	//Allow software scaling, where available.
	//This will allow scaling with output drivers (like x11, fbdev) that do not support
	//hardware scaling where MPlayer disables scaling by default for performance reasons.
	args << "-zoom";

	//Compulsary, otherwise we can't see anything from the widget
	//that gets MPlayer video stream
	args << "-colorkey";
	args << "0x020202";

	//MPlayer has an ability to resize the video to fit the pixel size of your monitor.
	//For example, in some cases, a pixel may not be a square. In other words,
	//the width of a pixel may be less than its height. As a result,
	//the aspect ratio of the video displayed on the monitor might be incorrect.
	//MPlayer automatically detects this aspect ratio and corrects it silently.
	//Unfortunately, it is all wrong on my widescreen laptop. To correct this problem,
	//you have to specified the aspect ratio that fit your monitor using option -monitorpixelaspect
	args << "-monitorpixelaspect";
	args << "1";

	//Video contrast
	args << "-contrast";
	args << QString::number(settings.contrast);

	//Video brightness
	args << "-brightness";
	args << QString::number(settings.brightness);

	//Video hue
	args << "-hue";
	args << QString::number(settings.hue);

	//Video saturation
	args << "-saturation";
	args << QString::number(settings.saturation);

	//Loads all the video filters
	QStringList videoFilters = settings.videoFilters;
	if (!videoFilters.isEmpty()) {
		foreach (const QString filter, videoFilters) {
			args << "-vf-add";
			args << filter;
		}
	}

	//Loads all the audio filters
	QStringList audioFilters = settings.audioFilters;
	if (!audioFilters.isEmpty()) {
		foreach (const QString filter, audioFilters) {
			args << "-af";
			args << filter;
		}
	}

	return args;
}
