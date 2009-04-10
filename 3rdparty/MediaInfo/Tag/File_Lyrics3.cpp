// File_Lyrics3 - Info for Lyrics3 tagged files
// Copyright (C) 2005-2009 Jerome Martinez, Zen@MediaArea.net
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

//---------------------------------------------------------------------------
// Compilation conditions
#include "MediaInfo/Setup.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_LYRICS3_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Tag/File_Lyrics3.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Lyrics3::File_Lyrics3()
:File__Analyze()
{
    //Configuration
    TotalSize=(int64u)-1;
}

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Lyrics3::Read_Buffer_Continue()
{
    if (TotalSize==(int64u)-1)
        TotalSize=Buffer_Size;

    //Coherency
    if (TotalSize<20)
    {
        Reject("Lyrics3");
        return;
    }

    //Buffer size
    if (Buffer_Size<TotalSize)
        return;

    //Parsing
    Element_Offset=0;
    Element_Size=TotalSize;
    Skip_Local(11,                                              "Signature");
    Skip_Local(TotalSize-20,                                    "Lyrics");
    Skip_Local(9,                                               "Signature");

    //Filling
    Stream_Prepare(Stream_General);
    Stream_Prepare(Stream_Text);
    Fill(Stream_Text, 0, Text_Codec, "Lyrics3");

    Accept("Lyric3");
    Finish("Lyrics3");
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_LYRICS3_YES

