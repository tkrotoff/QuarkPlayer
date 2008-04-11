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

#include "MediaSettings.h"

MediaSettings::MediaSettings() {
	clear();
}

MediaSettings::~MediaSettings() {
}

void MediaSettings::clear() {
	current_sec = 0;
	//current_sub_id = SubNone;
	current_sub_id = NoneSelected;
	current_audio_id = NoneSelected;
	current_title_id = NoneSelected;
	current_chapter_id = NoneSelected;
	current_angle_id = NoneSelected;

	aspect_ratio_id = AspectAuto;

	//fullscreen = FALSE;
	volume = 40;
	mute = false;
	external_subtitles.clear();
	external_audio.clear();
	sub_delay = 0;
	audio_delay = 0;
	sub_pos = 100; // 100% by default
	sub_scale = 5;

	sub_scale_ass = 1;

	brightness = 0;
	contrast = 0;
	gamma = 0;
	hue = 0;
	saturation = 0;

	speed = 1.0;

	phase_filter = false;
	current_denoiser = NoDenoise;
	deblock_filter = false;
	dering_filter = false;
	noise_filter = false;
	postprocessing_filter = false;
	upscaling_filter = false;

	current_deinterlacer = NoDeinterlace;

	add_letterbox = false;

	karaoke_filter = false;
	extrastereo_filter = false;
	volnorm_filter = false;

	audio_use_channels = ChDefault; // (0)
	stereo_mode = Stereo; // (0)

	panscan_factor = 1.0;

	// Not set yet.
	starting_time = -1;

	rotate = NoRotate;
	flip = false;

	is264andHD = false;

	forced_demuxer.clear();
	forced_video_codec.clear();
	forced_audio_codec.clear();

	original_demuxer.clear();
	original_video_codec.clear();
	original_audio_codec.clear();

	mplayer_additional_options.clear();
	mplayer_additional_video_filters.clear();
	mplayer_additional_audio_filters.clear();

	win_width = 400;
	win_height = 300;
}

double MediaSettings::win_aspect() {
	return (double) win_width / win_height;
}

void MediaSettings::print() {
	qDebug("MediaSettings::print");

	qDebug("  current_sec: %f", current_sec);
	qDebug("  current_sub_id: %d", current_sub_id);
	qDebug("  current_audio_id: %d", current_audio_id);
	qDebug("  current_title_id: %d", current_title_id);
	qDebug("  current_chapter_id: %d", current_chapter_id);
	qDebug("  current_angle_id: %d", current_angle_id);

	qDebug("  aspect_ratio_id: %d", aspect_ratio_id);
	//qDebug("  fullscreen: %d", fullscreen);
	qDebug("  volume: %d", volume);
	qDebug("  mute: %d", mute);
	qDebug("  external_subtitles: '%s'", external_subtitles.toUtf8().data());
	qDebug("  external_audio: '%s'", external_audio.toUtf8().data());
	qDebug("  sub_delay: %d", sub_delay);
	qDebug("  audio_delay: %d", sub_delay);
	qDebug("  sub_pos: %d", sub_pos);
	qDebug("  sub_scale: %f", sub_scale);

	qDebug("  sub_scale_ass: %f", sub_scale_ass);

	qDebug("  brightness: %d", brightness);
	qDebug("  contrast: %d", contrast);
	qDebug("  gamma: %d", gamma);
	qDebug("  hue: %d", hue);
	qDebug("  saturation: %d", saturation);

	qDebug("  speed: %f", speed);

	qDebug("  phase_filter: %d", phase_filter);
	qDebug("  current_denoiser: %d", current_denoiser);
	qDebug("  deblock_filter: %d", deblock_filter);
	qDebug("  dering_filter: %d", dering_filter);
	qDebug("  noise_filter: %d", noise_filter);
	qDebug("  postprocessing_filter: %d", postprocessing_filter);
	qDebug("  upscaling_filter: %d", upscaling_filter);

	qDebug("  current_deinterlacer: %d", current_deinterlacer);

	qDebug("  add_letterbox: %d", add_letterbox);

	qDebug("  karaoke_filter: %d", karaoke_filter);
	qDebug("  extrastereo_filter: %d", extrastereo_filter);
	qDebug("  volnorm_filter: %d", volnorm_filter);

	qDebug("  audio_use_channels: %d", audio_use_channels);
	qDebug("  stereo_mode: %d", stereo_mode);

	qDebug("  panscan_factor: %f", panscan_factor);

	qDebug("  rotate: %d", rotate);
	qDebug("  flip: %d", flip);

	qDebug("  forced_demuxer: '%s'", forced_demuxer.toUtf8().data());
	qDebug("  forced_video_codec: '%s'", forced_video_codec.toUtf8().data());
	qDebug("  forced_audio_codec: '%s'", forced_video_codec.toUtf8().data());

	qDebug("  original_demuxer: '%s'", original_demuxer.toUtf8().data());
	qDebug("  original_video_codec: '%s'", original_video_codec.toUtf8().data());
	qDebug("  original_audio_codec: '%s'", original_video_codec.toUtf8().data());

	qDebug("  mplayer_additional_options: '%s'", mplayer_additional_options.toUtf8().data());
	qDebug("  mplayer_additional_video_filters: '%s'", mplayer_additional_video_filters.toUtf8().data());
	qDebug("  mplayer_additional_audio_filters: '%s'", mplayer_additional_audio_filters.toUtf8().data());

	qDebug("  win_width: %d", win_width);
	qDebug("  win_height: %d", win_height);
	qDebug("  win_aspect(): %f", win_aspect());

	qDebug("  starting_time: %f", starting_time);
	qDebug("  is264andHD: %d", is264andHD);
}
