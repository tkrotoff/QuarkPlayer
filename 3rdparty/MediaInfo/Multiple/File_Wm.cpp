// File_Wm - Info for Windows Media files
// Copyright (C) 2002-2009 Jerome Martinez, Zen@MediaArea.net
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
// Main part
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
#ifdef MEDIAINFO_WM_YES
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Wm.h"
#if defined(MEDIAINFO_MPEGPS_YES)
    #include "MediaInfo/Multiple/File_MpegPs.h"
#endif
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
File_Wm::File_Wm()
:File__Analyze()
{
    //Configuration
    DataMustAlwaysBeComplete=false;

    //Stream
    Packet_Count=0;
    MaximumDataPacketSize=0;
    SizeOfMediaObject_BytesAlreadyParsed=0;
    FileProperties_Preroll=0;
    Codec_Description_Count=0;
    Stream_Number=0;
    Data_Parse_Padding=0;
    MaximumDataPacketSize=(int32u)-1;
    NumberPayloads=1;
    NumberPayloads_Pos=0;
    Data_Parse_Begin=true;
    IsDvrMs=false;
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Wm::Streams_Finish()
{
    //Encryption management
    /*const Ztring& Encryption=Retrieve(Stream_General, 0, General_Encryption);
    if (!Encryption.empty())
    {
        for (size_t StreamKind=Stream_General+1; StreamKind<Stream_Max; StreamKind++)
            for (size_t Pos=0; Pos<(*Stream[StreamKind]).size(); Pos++)
                Fill ((stream_t)StreamKind, 0, "Encryption", Encryption);
    }

        Fill("BitRate", CurrentBitRate[StreamNumber]);
    */

    std::map<int16u, stream>::iterator Temp=Stream.begin();
    while (Temp!=Stream.end())
    {
        for (std::map<std::string, ZenLib::Ztring>::iterator Info_Temp=Temp->second.Info.begin(); Info_Temp!=Temp->second.Info.end(); Info_Temp++)
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, Info_Temp->first.c_str(), Info_Temp->second, true);

        //Codec Info
        for (size_t Pos=0; Pos<CodecInfos.size(); Pos++)
        {
            if (CodecInfos[Pos].Type==1 && Temp->second.StreamKind==Stream_Video
             || CodecInfos[Pos].Type==2 && Temp->second.StreamKind==Stream_Audio)
            {
                Fill(Temp->second.StreamKind, Temp->second.StreamPos, "CodecID_Description", CodecInfos[Pos].Info, true);
                Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Codec_Description", CodecInfos[Pos].Info, true);
            }
        }

        if (Temp->second.StreamKind==Stream_Video)
        {
            //Some tests about the frame rate
            std::map<int32u, int32u> PresentationTime_Deltas_Problem;
            std::map<int32u, int32u> PresentationTime_Deltas_Most;
            for (std::map<ZenLib::int32u, ZenLib::int32u>::iterator PresentationTime_Delta=Temp->second.PresentationTime_Deltas.begin(); PresentationTime_Delta!=Temp->second.PresentationTime_Deltas.end(); PresentationTime_Delta++)
            {
                if (PresentationTime_Delta->second>=5)
                    PresentationTime_Deltas_Problem[PresentationTime_Delta->first]=PresentationTime_Delta->second;
                if (PresentationTime_Delta->second>=30)
                    PresentationTime_Deltas_Most[PresentationTime_Delta->first]=PresentationTime_Delta->second;
            }

            if (PresentationTime_Deltas_Most.size()==0
             || (PresentationTime_Deltas_Most.size()==1 && PresentationTime_Deltas_Problem.size()>1)
             || (PresentationTime_Deltas_Most.size()==2 && PresentationTime_Deltas_Problem.size()>2))
            {
                if (Temp->second.AverageTimePerFrame>0)
                    Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate, ((float)10000000)/Temp->second.AverageTimePerFrame, 3, true);
            }
            else if (PresentationTime_Deltas_Most.size()==1)
            {
                if (PresentationTime_Deltas_Most.begin()->first>1) //Not 0, we want to remove Delta incremented 1 per 1
                    Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate, 1000/((float64)PresentationTime_Deltas_Most.begin()->first), 3, true);
                if (Temp->second.AverageTimePerFrame>0)
                    Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate_Nominal, ((float)10000000)/Temp->second.AverageTimePerFrame, 3, true);
            }
            else if (PresentationTime_Deltas_Most.size()==2)
            {
                std::map<int32u, int32u>::iterator PresentationTime_Delta_Most=PresentationTime_Deltas_Most.begin();
                float64 PresentationTime_Deltas_1_Value=(float64)PresentationTime_Delta_Most->first;
                float64 PresentationTime_Deltas_1_Count=(float64)PresentationTime_Delta_Most->second;
                PresentationTime_Delta_Most++;
                float64 PresentationTime_Deltas_2_Value=(float64)PresentationTime_Delta_Most->first;
                float64 PresentationTime_Deltas_2_Count=(float64)PresentationTime_Delta_Most->second;
                float64 FrameRate_Real=1000/(((PresentationTime_Deltas_1_Value*PresentationTime_Deltas_1_Count)+(PresentationTime_Deltas_2_Value*PresentationTime_Deltas_2_Count))/(PresentationTime_Deltas_1_Count+PresentationTime_Deltas_2_Count));
                Fill(Temp->second.StreamKind, Temp->second.StreamPos, Video_FrameRate, FrameRate_Real, 3, true);
                if (Temp->second.AverageTimePerFrame>0)
                    Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate_Nominal, ((float)10000000)/Temp->second.AverageTimePerFrame, 3, true);
            }
            else
            {
                Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate_Mode, "VFR");
                if (Temp->second.AverageTimePerFrame>0)
                    Fill(Stream_Video, Temp->second.StreamPos, Video_FrameRate_Nominal, ((float)10000000)/Temp->second.AverageTimePerFrame, 3, true);
            }
        }
        if (Temp->second.AverageBitRate>0)
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, "BitRate", Temp->second.AverageBitRate, 10, true);
        if (Temp->second.LanguageID!=(int16u)-1 && Temp->second.LanguageID<(int16u)Languages.size())
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Language", Languages[Temp->second.LanguageID]);
        else if (!Language_ForAll.empty())
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Language", Language_ForAll);
        if (Temp->second.Parser)
        {
            if (Temp->second.StreamKind==Stream_Max)
                if (Temp->second.Parser->Count_Get(Stream_Audio))
                {
                    Stream_Prepare(Stream_Audio);
                    Temp->second.StreamKind=StreamKind_Last;
                    Temp->second.StreamPos=StreamPos_Last;
                }
            Ztring Format_Profile;
            if (Temp->second.StreamKind==Stream_Video)
                Format_Profile=Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format_Profile);
            Finish(Temp->second.Parser);
            Merge(*Temp->second.Parser, Temp->second.StreamKind, 0, Temp->second.StreamPos);
            if (!Format_Profile.empty() && Format_Profile.find(Retrieve(Stream_Video, Temp->second.StreamPos, Video_Format_Profile))==0)
                Fill(Stream_Video, Temp->second.StreamPos, Video_Format_Profile, Format_Profile, true);
        }

        //Delay (in case of MPEG-PS)
        if (Temp->second.StreamKind==Stream_Video)
        {
            Fill(Stream_Video, Temp->second.StreamPos, Video_Delay_Original, Retrieve(Temp->second.StreamKind, Temp->second.StreamPos, "Delay"));
            Fill(Stream_Video, Temp->second.StreamPos, Video_Delay_Original_Settings, Retrieve(Temp->second.StreamKind, Temp->second.StreamPos, "Delay_Settings"));
        }
        if (Temp->second.TimeCode_First!=(int64u)-1)
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Delay", Temp->second.TimeCode_First, 10, true);
        else
            Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Delay", "", Unlimited, true, true);
        Fill(Temp->second.StreamKind, Temp->second.StreamPos, "Delay_Settings", "", Unlimited, true, true);

        Temp++;
    }

    //Purge what is not needed anymore
    if (!File_Name.empty()) //Only if this is not a buffer, with buffer we can have more data
        Stream.clear();
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
void File_Wm::Header_Parse()
{
    if (!MustUseAlternativeParser)
    {
        //Parsing
        int128u Name;
        int64u Size;
        Get_GUID(Name,                                              "Name");
        Get_L8 (Size,                                               "Size");

        //Filling
        Header_Fill_Code(Name.hi, Ztring().From_GUID(Name));
        Header_Fill_Size(Size);
    }
    else
    {
        Header_Fill_Code(0, "Packet");
        Header_Fill_Size(MaximumDataPacketSize);
    }
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_WM_YES
