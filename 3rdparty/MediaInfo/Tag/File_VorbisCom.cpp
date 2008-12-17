// File_VorbisCom - Info for VorbisComments tagged files
// Copyright (C) 2007-2008 Jerome Martinez, Zen@MediaArea.net
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
#if defined(MEDIAINFO_VORBISCOM_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Tag/File_VorbisCom.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
File_VorbisCom::File_VorbisCom()
:File__Analyze()
{
    //In
    StreamKind=Stream_General;
    StreamGoal=Stream_General;
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
void File_VorbisCom::FileHeader_Parse()
{
    //Parsing
    Element_Begin("Vorbis comment header");
        Ztring vendor_string;
        int32u vendor_length;
        Get_L4 (vendor_length,                                  "vendor_length");
        Get_Local(vendor_length, vendor_string,                 "vendor_string");
        Get_L4 (user_comment_list_length,                       "user_comment_list_length");
    Element_End();

    FILLING_BEGIN();
        Stream_Prepare(Stream_General);
        if (StreamKind!=Stream_General)
            Stream_Prepare(StreamKind);
        if (StreamGoal!=Stream_General && StreamGoal!=StreamKind)
            Stream_Prepare(StreamGoal);

        //vendor_string
        if (StreamKind!=Stream_Audio && vendor_string.find(_T("Xiph.Org libVorbis"))==0)
            vendor_string.clear(); //string was set "by default"
        Ztring Library_Name, Library_Version, Library_Date;
        Ztring vendor_string_Without=vendor_string; vendor_string_Without.FindAndReplace(_T(";"), _T(""), 0, Ztring_Recursive);
        Library_Version=MediaInfoLib::Config.Library_Get(InfoLibrary_Format_VorbisCom, vendor_string_Without, InfoLibrary_Version);
        Library_Date=MediaInfoLib::Config.Library_Get(InfoLibrary_Format_VorbisCom, vendor_string_Without, InfoLibrary_Date);
        if (Library_Version.empty())
        {
            if (vendor_string.find(_T(" I "))!=std::string::npos)
            {
                Library_Name=vendor_string.SubString(_T(""), _T(" I "));
                Library_Date=vendor_string.SubString(_T(" I "), _T(""));
                if (Library_Date.size()>9)
                {
                    Library_Version=Library_Date.substr(9, std::string::npos);
                    if (Library_Version.find(_T("("))==std::string::npos)
                    {
                        Library_Version.FindAndReplace(_T(" "), _T("."), 0, Ztring_Recursive);
                        Library_Date.resize(8);
                    }
                }
            }
            else if (vendor_string.size()>9 && Ztring(vendor_string.substr(vendor_string.size()-8, std::string::npos)).To_int32u()>20000000)
            {
                Library_Name=vendor_string.substr(0, vendor_string.size()-9);
                Library_Date=vendor_string.substr(vendor_string.size()-8, std::string::npos);
                if (!Library_Name.empty())
                {
                    std::string::size_type Pos=Library_Name.rfind(_T(' '));
                    if (Pos<Library_Name.size()-2 && Library_Name[Pos+1]>=_T('0') && Library_Name[Pos+1]<=_T('9'))
                    {
                        Library_Version=Library_Name.substr(Pos+1, std::string::npos);
                        Library_Name.resize(Pos);
                    }
                }
            }
            else if (vendor_string.find(_T("aoTuV "))!=std::string::npos)
            {
                Library_Name=_T("aoTuV");
                Library_Version=vendor_string.SubString(_T("aoTuV "), _T("["));
                Library_Date=vendor_string.SubString(_T("["), _T("]"));
            }
            else if (vendor_string.find(_T("Lancer "))!=std::string::npos)
            {
                Library_Name=_T("Lancer");
                Library_Date=vendor_string.SubString(_T("["), _T("]"));
            }
            if (Library_Version.empty())
                Library_Version=Library_Date;
            if (Library_Date.size()==8)
            {
                Library_Date.insert(Library_Date.begin()+6, _T('-'));
                Library_Date.insert(Library_Date.begin()+4, _T('-'));
                Library_Date.insert(0, _T("UTC "));
            }
        }
        if (vendor_string.find(_T("libFLAC"))!=std::string::npos) Library_Name="libFLAC";
        if (vendor_string.find(_T("libVorbis I"))!=std::string::npos) Library_Name="libVorbis";
        if (vendor_string.find(_T("libTheora I"))!=std::string::npos) Library_Name="libTheora";
        if (vendor_string.find(_T("AO; aoTuV"))==0) Library_Name="aoTuV";
        if (vendor_string.find(_T("BS; Lancer"))==0) Library_Name="Lancer";

        Fill(StreamKind, 0, "Encoded_Library", vendor_string);
        Fill(StreamKind, 0, "Encoded_Library/Name", Library_Name);
        Fill(StreamKind, 0, "Encoded_Library/Version", Library_Version);
        Fill(StreamKind, 0, "Encoded_Library/Date", Library_Date);

        //Comments
        for (int32u Pos=0; Pos<user_comment_list_length; Pos++)
            Comment();

        //Positionning
        Finnished();
    FILLING_END();
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_VorbisCom::Comment()
{
    //Parsing
    Ztring comment;
    Element_Begin("Vorbis comment");
        int32u user_comment_length;
        Get_L4 (user_comment_length,                            "length");
        Get_UTF8(user_comment_length, comment,                  "comment");
        Element_Info(comment);
    Element_End();

    FILLING_BEGIN();
        Ztring Key=comment.SubString(_T(""), _T("="));
        Key.MakeUpperCase();
        Ztring Value=comment.SubString(_T("="), _T(""));

             if (Key==_T("ADDED_TIMESTAMP"))        Fill(Stream_General, 0, "Added_Date", Ztring().Date_From_Milliseconds_1601(Value.To_int64u()/1000));
        else if (Key==_T("ALBUM ARTIST"))           {if (Value!=Retrieve(Stream_General, 0, General_Performer)) Fill(Stream_General, 0, General_Performer, Value);}
        else if (Key==_T("ALBUM"))                  Fill(Stream_General, 0, General_Album, Value);
        else if (Key==_T("ARTIST"))                 {if (Value!=Retrieve(Stream_General, 0, General_Performer)) Fill(Stream_General, 0, General_Performer, Value);}
        else if (Key==_T("AUTHOR"))                 Fill(Stream_General, 0, General_WrittenBy, Value);
        else if (Key==_T("COMMENT"))                Fill(Stream_General, 0, General_Comment, Value);
        else if (Key==_T("COMMENTS"))               Fill(Stream_General, 0, General_Comment, Value);
        else if (Key==_T("CONTACT"))                Fill(Stream_General, 0, General_Publisher, Value);
        else if (Key==_T("COPYRIGHT"))              Fill(Stream_General, 0, General_Copyright, Value);
        else if (Key==_T("DATE"))                   Fill(Stream_General, 0, General_Recorded_Date, Value);
        else if (Key==_T("DESCRIPTION"))            Fill(Stream_General, 0, General_Description, Value);
        else if (Key==_T("ENCODER"))                Fill(Stream_General, 0, General_Encoded_Application, Value);
        else if (Key==_T("ENCODED_USING"))          Fill(Stream_General, 0, General_Encoded_Application, Value);
        else if (Key==_T("ENCODER_URL"))            Fill(Stream_General, 0, General_Encoded_Application_Url, Value);
        else if (Key==_T("ENSEMBLE"))               Fill(Stream_General, 0, General_Accompaniment, Value);
        else if (Key==_T("GENRE"))                  Fill(Stream_General, 0, General_Genre, Value);
        else if (Key==_T("FIRST_PLAYED_TIMESTAMP")) Fill(StreamGoal,     0, "Played_First_Date", Ztring().Date_From_Milliseconds_1601(Value.To_int64u()/10000));
        else if (Key==_T("ISRC"))                   Fill(Stream_General, 0, General_ISRC, Value);
        else if (Key==_T("LANGUAGE"))               Fill(StreamKind,     0, "Language", Value);
        else if (Key==_T("LAST_PLAYED_TIMESTAMP"))  Fill(StreamGoal,     0, "Played_Last_Date", Ztring().Date_From_Milliseconds_1601(Value.To_int64u()/10000));
        else if (Key==_T("LICENCE"))                Fill(Stream_General, 0, General_TermsOfUse, Value);
        else if (Key==_T("LYRICS"))                 Fill(Stream_General, 0, General_Lyrics, Value);
        else if (Key==_T("LWING_GAIN"))             Fill(StreamGoal,     0, "ReplayGain_Gain", Value.To_float64(), 2);
        else if (Key==_T("LOCATION"))               Fill(Stream_General, 0, General_Recorded_Location, Value);
        else if (Key==_T("ORGANIZATION"))           Fill(Stream_General, 0, General_Producer, Value);
        else if (Key==_T("PERFORMER"))              Fill(Stream_General, 0, General_Performer, Value);
        else if (Key==_T("PLAY_COUNT"))             Fill(StreamGoal,     0, "Played_Count", Value.To_int64u());
        else if (Key==_T("REPLAYGAIN_ALBUM_GAIN"))  Fill(Stream_General, 0, "Album_ReplayGain_Gain", Value.To_float64(), 2);
        else if (Key==_T("REPLAYGAIN_ALBUM_PEAK"))  Fill(Stream_General, 0, "Album_ReplayGain_Peak", Value.To_float64(), 6);
        else if (Key==_T("REPLAYGAIN_TRACK_GAIN"))  Fill(StreamKind,     0, "ReplayGain_Gain",       Value.To_float64(), 2);
        else if (Key==_T("REPLAYGAIN_TRACK_PEAK"))  Fill(StreamKind,     0, "ReplayGain_Peak",       Value.To_float64(), 6);
        else if (Key==_T("TITLE"))                  Fill(Stream_General, 0, General_Title, Value);
        else if (Key==_T("TRACKNUMBER"))            Fill(Stream_General, 0, General_Track_Position, Value);
        else if (Key==_T("VERSION"))                Fill(Stream_General, 0, General_Track_More, Value);
        else if (Key.find(_T("CHAPTER"))==0)
        {
            if (Count_Get(Stream_Chapters)==0)
                Stream_Prepare(Stream_Chapters);
            if (Key.find(_T("NAME"))==Error)
            {
                Chapter_Pos=Key.SubString(_T("CHAPTER"), _T(""));
                Chapter_Time=Value;
            }
            else
            {
                Value.FindAndReplace(_T("\n"), _T(""), Count_Get(Stream_Text)-1); //Some chapters names have extra characters, not needed
                Value.FindAndReplace(_T("\r"), _T(""), Count_Get(Stream_Text)-1); //Some chapters names have extra characters, not needed
                Value.FindAndReplace(_T(" "),  _T(""), Count_Get(Stream_Text)-1); //Some chapters names have extra characters, not needed
                Fill(Stream_Chapters, 0, Chapter_Pos.To_Local().c_str(), Chapter_Time+_T(" ")+Value);
            }
        }
        else                                Fill(Stream_General, 0, comment.SubString(_T(""), _T("=")).To_Local().c_str(), Value);
    FILLING_END();
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_VORBISCOM_YES
