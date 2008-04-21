/*
 * VLC and MPlayer backends for the Phonon library
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

#ifndef PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H
#define PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H

#include "../MediaController.h"

namespace Phonon
{
namespace VLC_MPlayer
{

/**
 *
 *
 * @author Tanguy Krotoff
 */
class VLCMediaController /*: public MediaController*/ {
public:

	VLCMediaController();
	virtual ~VLCMediaController();

protected:

private:
};

}}	//Namespace Phonon::VLC_MPlayer

#endif	//PHONON_VLC_MPLAYER_VLCMEDIACONTROLLER_H
