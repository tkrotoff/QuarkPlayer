/*
 * MPlayer backend for the Phonon library
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

#include "MPlayerLoader.h"

#include "MPlayerProcess.h"
#include "MediaSettings.h"
#include "MPlayerConfig.h"
#include "LibMPlayerLogger.h"

namespace Phonon
{
namespace MPlayer
{

MediaSettings MPlayerLoader::settings;

MPlayerProcess * MPlayerLoader::createNewMPlayerProcess(QObject * parent) {
	MPlayerProcess * process = new MPlayerProcess(parent);

	return process;
}

void MPlayerLoader::start(MPlayerProcess * process, const QStringList & arguments, const QString & fileName, WId videoWidgetId, qint64 seek) {
	if (!process) {
		LibMPlayerCritical() << "process cannot be NULL";
		return;
	}

	QStringList args;
	args << readMediaSettings();
	if (!arguments.isEmpty()) {
		args << arguments;
	}

	//Check for the optical device and add new arguments if possible
	if (fileName.contains("dvd://")) {
		LibMPlayerDebug() << "DVD detected";
		if (!settings.opticalDeviceName.isEmpty()) {
			args << "-dvd-device";
			args << settings.opticalDeviceName;
		}
	}
	if (fileName.contains("cdda://") || fileName.contains("vcd://")) {
		LibMPlayerDebug() << "CD Audio/VCD detected";
		if (!settings.opticalDeviceName.isEmpty()) {
			args << "-cdrom-device";
			args << settings.opticalDeviceName;
		}
	}
	///

	if (!process->start(args, fileName, videoWidgetId, seek)) {
		//Error handling
		LibMPlayerCritical() << "MPlayer process couldn't start";
	}
}

void MPlayerLoader::restart(MPlayerProcess * process, const QStringList & arguments, const QString & fileName, qint64 seek) {
	if (!process) {
		LibMPlayerCritical() << "process cannot be NULL";
		return;
	}

	QStringList args;
	if (!arguments.isEmpty()) {
		args << arguments;
	}

	MediaData mediaData = process->mediaData();
	if (!fileName.isEmpty()) {
		mediaData.fileName = fileName;
	}
	if (seek != -1) {
		mediaData.currentTime = seek;
	}

	start(process, args, mediaData.fileName, mediaData.videoWidgetId, mediaData.currentTime);
}

void MPlayerLoader::start(MPlayerProcess * process, const QString & fileName, WId videoWidgetId) {
	start(process, QStringList(), fileName, videoWidgetId, 0);
}

void MPlayerLoader::startMPlayerVersion(QObject * parent) {
	MPlayerProcess * process = createNewMPlayerProcess(parent);

	QStringList args;
	QString fileName("quarkplayerfakename");

	if (!process->start(args, fileName, 0, 0)) {
		//Error handling
		LibMPlayerCritical() << "MPlayer process couldn't start";
	}

	//Blocks until the command is done
	process->waitForFinished();
}

void MPlayerLoader::loadMedia(MPlayerProcess * process, const QString & fileName) {
	if (!process) {
		LibMPlayerCritical() << "process cannot be NULL";
		return;
	}

	//Optimisation: no -identify if we are reading a dvd
	if (fileName.contains("dvd://")) {
		LibMPlayerDebug() << "DVD detected";
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

	if (!process->start(args, fileName, 0, 0)) {
		//Error handling
		LibMPlayerCritical() << "MPlayer process couldn't start";
	}
}

QStringList MPlayerLoader::readMediaSettings() {
	LibMPlayerDebug();

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

#ifdef Q_WS_WIN
	if (MPlayerProcess::getMPlayerVersion() > 28121) {
		if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA) {
			//Direct3D video output driver only available after revision 28121
			//See http://svn.mplayerhq.hu/mplayer/trunk/libvo/vo_direct3d.c
			//r28121 Tue Dec 9 2008
			//The Direct3D video output driver does not very well yet under Windows XP
			//It works fine only with Windows >= Vista
			args << "-vo";
			args << "direct3d";
		}
	} else {
		LibMPlayerWarning()
			<< "Could not use Direct3D video output driver, your MPlayer version is too old:"
			<< MPlayerProcess::getMPlayerVersion();
	}
#endif	//Q_WS_WIN

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

	if (MPlayerProcess::getMPlayerVersion() > 27872) {
		//Volume startup command line only available after revision 27872
		//See http://svn.mplayerhq.hu/mplayer/trunk/mplayer.c?revision=27872&view=markup
		//r27872 Fri Oct 31 2008
		if (settings.volume >= 0) {
			args << "-volume";
			args << QString::number(settings.volume);
		}
	} else {
		LibMPlayerWarning()
			<< "Could not use MPlayer volume command line, your MPlayer version is too old:"
			<< MPlayerProcess::getMPlayerVersion();
	}

	//Speedup internet media by using IPv4
	//otherwise it uses IPv6 which makes the system slow
	//due to names resolving
	args << "-prefer-ipv4";

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

	//Subtitles font

	//Turn on SSA/ASS subtitle rendering. With this option, libass will be used for
	//SSA/ASS external subtitles and Matroska tracks. You may also want to use -embeddedfonts
	//NOTE: When fontconfig is compiled-in, -ass turns on -fontconfig unless explicitly
	//turned off with -nofontconfig
	args << "-ass";

	//Enables extraction of Matroska embedded fonts (default: disabled)
	//These fonts can be used for SSA/ASS subtitle rendering (-ass option)
	//Font files are created in the ~/.mplayer/fonts directory
	//NOTE: With FontConfig 2.4.2 or newer, embedded fonts are opened directly from memory,
	//and this option is enabled by default
	args << "-embeddedfonts";

	//Disable mouse button press/release input
	args << "-nomouseinput";

	//Adjust matching fuzziness when searching for subtitles:
	//0    exact match
	//1    Load all subs containing movie name
	//2    Load all subs in the current directory
	args << "-sub-fuzziness";
	args << "1";

	//Turns off xscreensaver at startup and turns it on again on exit.
	//If your screensaver supports neither the XSS nor XResetScreen-
	//Saver API please use -heartbeat-cmd instead.
	//args << "-stop-xscreensaver";

#ifndef Q_WS_WIN
	//Prevent MPlayer from messing up our shortcuts
	//We don't have this problem under Windows
	if (MPlayerProcess::getMPlayerVersion() > 29058) {
		//r29058 Wed, 25 Mar 2009
		args << "-input";
		args << "nodefault-bindings:conf=/dev/null";
	} else {
		LibMPlayerWarning()
			<< "Could not disable MPlayer key bindings (shortcuts), your MPlayer version is too old:"
			<< MPlayerProcess::getMPlayerVersion();
	}
#endif	//Q_WS_WIN


	//Sets MPlayer configuration file
	//By forcing the MPlayer config file, the user can overwrite
	//any of the parameters given to the MPlayer command line
	//Warning: should be the latest argument
	args << "-include";
	args << MPlayerConfig::instance().configPath();

	return args;
}

}}	//Namespace Phonon::MPlayer
