// File_Id3 - Info for ID3 tagged files
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
#if defined(MEDIAINFO_ID3_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Tag/File_Id3.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Buffer - Global
//***************************************************************************

//---------------------------------------------------------------------------
void File_Id3::Read_Buffer_Continue()
{
    //Buffer size
    if (Buffer_Size<128)
        return;

    //Parsing
    Element_Offset=0;
    Element_Size=Buffer_Size;
    Ztring Title, Artist, Album, Year, Comment;
    int8u Track=0, Genre;
    Skip_C3   (                                                 "ID");
    Get_Local (30, Title,                                       "Title");
    Get_Local (30, Artist,                                      "Artist");
    Get_Local (30, Album,                                       "Album");
    Get_Local ( 4, Year,                                        "Year");
    Get_Local (30, Comment,                                     "Comment");
    if (Comment.size()<29) //Id3v1.1 specifications : Track number addition
    {
        Element_Offset-=2;
        int8u Zero;
        Peek_B1(Zero);
        if (Zero==0)
        {
            Skip_B1(                                            "Zero");
            Get_B1 (Track,                                      "Track");
        }
        else
            Element_Offset+=2;
    }
    Get_B1 (Genre,                                              "Genre");

    FILLING_BEGIN();
        Title.TrimRight();
        Artist.TrimRight();
        Album.TrimRight();
        Year.TrimRight();
        Comment.TrimRight();

        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Album, Album);
        Fill(Stream_General, 0, General_Track, Title);
        Fill(Stream_General, 0, General_Performer, Artist);
        Fill(Stream_General, 0, General_Comment, Comment);
        Fill(Stream_General, 0, General_Recorded_Date, Year);
        if (Genre && Genre!=(int8u)-1)
            Fill(Stream_General, 0, General_Genre, Genre);
        if (Track)
            Fill(Stream_General, 0, General_Track_Position, Track);

        Accept("Id3");
        Finish("Id3");
    FILLING_END();
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_ID3_YES

