// File_Vc1 - Info for MPEG Video files
// Copyright (C) 2004-2009 Jerome Martinez, Zen@MediaArea.net
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
// Information about MPEG Video files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_Vc1H
#define MediaInfo_Vc1H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Analyze.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Vc1
//***************************************************************************

class File_Vc1 : public File__Analyze
{
public :
    //In
    size_t Frame_Count_Valid;
    bool   FrameIsAlwaysComplete;
    bool   From_WMV3;
    bool   Only_0D;

    //Constructor/Destructor
    File_Vc1();

private :
    //Streams management
    void Streams_Fill();
    void Streams_Finish();

    //Buffer - File header
    bool FileHeader_Begin();

    //Buffer - Synchro
    bool Synchronize() {return Synchronize_0x000001();}
    bool Synched_Test();
    void Synched_Init();

    //Buffer - Per element
    void Header_Parse();
    bool Header_Parser_QuickSearch();
    bool Header_Parser_Fill_Size();
    void Data_Parse();

    //Elements
    void EndOfSequence();
    void Slice();
    void Field();
    void FrameHeader();
    void EntryPointHeader();
    void SequenceHeader();
    void UserDefinedSlice();
    void UserDefinedField();
    void UserDefinedFrameHeader();
    void UserDefinedEntryPointHeader();
    void UserDefinedSequenceHeader();

    //Count
    size_t Frame_Count;
    size_t Interlaced_Top;
    size_t Interlaced_Bottom;
    std::vector<size_t> PictureFormat_Count;

    //From SequenceHeader
    int16u coded_width;
    int16u coded_height;
    int16u framerateexp;
    int8u  frameratecode_enr;
    int8u  frameratecode_dr;
    int8u  profile;
    int8u  level;
    int8u  colordiff_format;
    int8u  AspectRatio;
    int8u  AspectRatioX;
    int8u  AspectRatioY;
    int8u  hrd_num_leaky_buckets;
    int8u  max_b_frames;
    bool   interlace;
    bool   tfcntrflag;
    bool   framerate_present;
    bool   framerate_form;
    bool   hrd_param_flag;
    bool   finterpflag;
    bool   rangered;
    bool   psf;
    bool   pulldown;
    bool   panscan_flag;
    
    //Stream
    struct stream
    {
        bool   Searching_Payload;
        bool   Searching_TimeStamp_Start;
        bool   Searching_TimeStamp_End;

        stream()
        {
            Searching_Payload=false;
            Searching_TimeStamp_Start=false;
            Searching_TimeStamp_End=false;
        }
    };
    std::vector<stream> Streams;

    //Temporal reference
    struct temporalreference
    {
        bool   top_field_first;
        bool   repeat_first_field;
    };
    std::map<int16u, temporalreference> TemporalReference; //int32u is the reference
    std::vector<temporalreference>      TemporalReference_Waiting; //First must be I and P-frames, other B-frames
    int16u TemporalReference_Offset;

    //Temp
    size_t Width;
    size_t Height;
    size_t RatioValue;
    size_t BitRate;
    bool   EntryPoint_Parsed;

    //Error controls
    std::vector<int8u> Frame_ShouldBe;
};

} //NameSpace

#endif
