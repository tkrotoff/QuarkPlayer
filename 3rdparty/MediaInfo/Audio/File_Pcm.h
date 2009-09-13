// File_Pcm - Info for PCM files
// Copyright (C) 2007-2009 Jerome Martinez, Zen@MediaArea.net
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Information about PCM files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_File_PcmH
#define MediaInfo_File_PcmH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Analyze.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Pcm
//***************************************************************************

class File_Pcm : public File__Analyze
{
public :
    //In
    ZenLib::Ztring Codec;

private :
    //Streams management
    void Streams_Fill();

    //Buffer - Global
    void Read_Buffer_Continue ();

    //Buffer - Per element
    void Header_Parse();
    void Data_Parse();

    //Elements
    void VOB();
    void M2TS();
};

} //NameSpace

#endif
