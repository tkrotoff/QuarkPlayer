/*
 * MPlayer backend for the Phonon library
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

namespace Phonon
{
namespace MPlayer
{

MediaSettings::MediaSettings() {
	clear();
}

MediaSettings::~MediaSettings() {
}

void MediaSettings::clear() {
	//From MPlayer manpage: a value of -1 (the default) will not change the volume
	volume = -1;

	brightness = 0;
	contrast = 0;
	hue = 0;
	saturation = 0;

	audioFilters.clear();
	videoFilters.clear();

	opticalDeviceName.clear();
}

}}	//Namespace Phonon::MPlayer
