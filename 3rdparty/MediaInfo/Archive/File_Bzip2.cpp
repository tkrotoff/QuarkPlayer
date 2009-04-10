// File_Bzip2 - Info for BZIP2 files
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

//---------------------------------------------------------------------------
// Compilation conditions
#include "MediaInfo/Setup.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_BZIP2_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Archive/File_Bzip2.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Static stuff
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Bzip2::FileHeader_Begin()
{
    //Element_Size
    if (Buffer_Size<2)
        return false; //Must wait for more data

    if (CC2(Buffer)!=0x425A) //"BZ"
    {
        Reject("Bzip2");
        return false;
    }

    //All should be OK...
    return true;
}

//***************************************************************************
// Buffer - Global
//***************************************************************************

//---------------------------------------------------------------------------
void File_Bzip2::Read_Buffer_Continue()
{
    //Parsing
    Skip_B7(                                                    "Magic");
    Skip_XX(File_Size-2,                                        "Data");

    FILLING_BEGIN();
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "BZip2");
        Accept("Bzip2");
        Finish("Bzip2");
    FILLING_END();
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_BZIP2_YES
