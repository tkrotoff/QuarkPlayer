// File_TwinVQ - Info for TwiVQ files
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
// Source: http://wiki.multimedia.cx/index.php?title=VQF
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
// Compilation conditions
#include "MediaInfo/Setup.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_TWINVQ_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_TwinVQ.h"
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

//---------------------------------------------------------------------------
const char* TwinVQ_samplerate(int32u samplerate)
{
    switch (samplerate)
    {
        case 11 : return "11025";
        case 22 : return "22050";
        case 44 : return "44100";
        default : return "";
    }
}

//---------------------------------------------------------------------------
namespace Elements
{
    const int32u _c__=0x28632920;
    const int32u AUTH=0x41555448;
    const int32u COMM=0x434F4D4D;
    const int32u COMT=0x434F4D54;
    const int32u DATA=0x44415441;
    const int32u DSIZ=0x4453495A;
    const int32u FILE=0x46494C45;
    const int32u NAME=0x4E414D45;
}

//***************************************************************************
// Buffer - File header
//***************************************************************************

//---------------------------------------------------------------------------
bool File_TwinVQ::FileHeader_Begin()
{
    //Testing
    if (Buffer_Offset+4>Buffer_Size)
        return false;
    if (CC4(Buffer+Buffer_Offset)!=0x5457494E) //"TWIN"
    {
        Reject("TwinVQ");
        return false;
    }

    //All should be OK...
    return true;
}

//---------------------------------------------------------------------------
void File_TwinVQ::FileHeader_Parse()
{
    //Parsing
    Skip_C4(                                                    "magic");
    Skip_Local(8,                                               "version");
    Skip_B4(                                                    "subchunks_size");

    FILLING_BEGIN();
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "TwinVQ");
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "TwinVQ");
        Fill(Stream_Audio, 0, Audio_Codec, "TwinVQ");
        Accept("TwinVQ");
    FILLING_END();
}

//***************************************************************************
// Buffer - Per element
//***************************************************************************

//---------------------------------------------------------------------------
void File_TwinVQ::Header_Parse()
{
    //Parsing
    int32u id, size;
    Get_C4 (id,                                                 "id");
    Get_B4 (size,                                               "size");

    //Filling
    Header_Fill_Code(id, Ztring().From_CC4(id));
    Header_Fill_Size(8+(id==Elements::DATA?0:size)); //DATA chunk indicates the end of the header, with no chunk size
}

//---------------------------------------------------------------------------
void File_TwinVQ::Data_Parse()
{
    #define ELEMENT_CASE(_NAME, _DETAIL) \
        case Elements::_NAME : Element_Info(_DETAIL); _NAME(); break;

    //Parsing
    switch (Element_Code)
    {
        ELEMENT_CASE(_c__, "Copyright");
        ELEMENT_CASE(AUTH, "Author");
        ELEMENT_CASE(COMM, "Mandatory information");
        ELEMENT_CASE(COMT, "Comment");
        ELEMENT_CASE(DATA, "Data");
        ELEMENT_CASE(DSIZ, "Data size");
        ELEMENT_CASE(FILE, "Filename");
        ELEMENT_CASE(NAME, "Song title");
        default : Skip_XX(Element_Size,                         "Unknown");
    }
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_TwinVQ::COMM()
{
    //Parsing
    int32u channel_mode, bitrate, samplerate;
    Get_B4 (channel_mode,                                       "channel_mode");
    Get_B4 (bitrate,                                            "bitrate");
    Get_B4 (samplerate,                                         "samplerate");
    Skip_B4(                                                    "security_level");

    //Filling
    Fill(Stream_Audio, 0, Audio_Channel_s_, channel_mode+1);
    Fill(Stream_Audio, 0, Audio_BitRate, bitrate*1000);
    Fill(Stream_Audio, 0, Audio_SamplingRate, TwinVQ_samplerate(samplerate));
}

//---------------------------------------------------------------------------
void File_TwinVQ::DATA()
{
    //This is the end of the parsing (DATA chunk format is unknown)
    Finish("TwinVQ");
}

//---------------------------------------------------------------------------
void File_TwinVQ::DSIZ()
{
    //Parsing
    Skip_B4(                                                    "Value");
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void File_TwinVQ::_____char()
{
    //Parsing
    Skip_Local(Element_Size,                                    "Value");
}

//---------------------------------------------------------------------------
void File_TwinVQ::_____char(const char* Parameter)
{
    //Parsing
    Ztring Value;
    Get_Local(Element_Size, Value,                              "Value");

    //Filling
    Fill(Stream_General, 0, Parameter, Value);
}

} //Namespace

#endif //MEDIAINFO_TWINVQ_YES

