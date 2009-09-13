// File_DvDif - Info for DV-DIF files
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

//---------------------------------------------------------------------------
// Compilation conditions
#include "MediaInfo/Setup.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_DVDIF_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_DvDif.h"
#if defined(MEDIAINFO_EIA608_YES)
    #include "MediaInfo/Text/File_Eia608.h"
#endif
#include "MediaInfo/MediaInfo_Config_MediaInfo.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
const char*  Dv_sct[]=
{
    "Header",
    "Subcode",
    "VAUX",
    "Audio",
    "Video",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Dv_Ssyb_Pc0(int8u Pc0)
{
    switch (Pc0)
    {
        case 0x13 : return "Timecode";
        case 0x14 : return "Binary group";
        case 0x50 :
        case 0x60 : return "Source";
        case 0x51 :
        case 0x61 : return "Source control";
        default   : return "";
    }
}

//---------------------------------------------------------------------------
const char*  Dv_Disp[]=
{
    "4/3",
    "",
    "16/9",
    "Letterbox",
    "",
    "",
    "",
    "16/9",
};

//---------------------------------------------------------------------------
const int32u  Dv_Audio_SamplingRate[]=
{
    48000,
    44100,
    32000,
    0,
    0,
    0,
    0,
    0,
};

//---------------------------------------------------------------------------
const char*  Dv_StereoMode[]=
{
    "Multi-Stero",
    "Lumped",
};

//---------------------------------------------------------------------------
const int32u  Dv_Audio_Resolution[]=
{
    16,
    12,
    0,
    0,
    0,
    0,
    0,
    0,
};

//---------------------------------------------------------------------------
const int8u  Dv_ChannelsPerBlock[]=
{
    1,
    2,
    0,
    0,
};

//---------------------------------------------------------------------------
const char*  Dv_Pair[]=
{
    "One pair of channels",
    "Independant channels",
};

//---------------------------------------------------------------------------
const char*  Dv_CopyGenerationManagementSystem[]=
{
    "Unrestricted",
    "Not used",
    "One generation only",
    "No copy",
};

//---------------------------------------------------------------------------
const char*  Dv_InputType[]=
{
    "Analog",
    "Digital",
    "Reserved",
    "No information",
};

//---------------------------------------------------------------------------
const char*  Dv_CompressionTimes[]=
{
    "Once",
    "Twice",
    "Three+",
    "No information",
};

//---------------------------------------------------------------------------
const char*  Dv_Emphasis[]=
{
    "Enphasis off",
    "Enphasis on",
    "Reserved",
    "Reserved",
};

//---------------------------------------------------------------------------
const char*  Dv_consumer_camera_1_ae_mode[]=
{
    "full automatic",
    "gain priority mode",
    "shutter priority mode",
    "iris priority mode",
    "manual",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", //no info
};

//---------------------------------------------------------------------------
const char*  Dv_consumer_camera_1_wb_mode[]=
{
    "automatic",
    "hold",
    "one push",
    "pre-set",
    "",
    "",
    "",
    "", //no info
};

//---------------------------------------------------------------------------
const char* Dv_consumer_camera_1_white_balance(int8u white_balance)
{
    switch (white_balance)
    {
        case 0x00 : return "candle";
        case 0x01 : return "incandescent lamp";
        case 0x02 : return "low color temperature; florescent lamp";
        case 0x03 : return "high color temperature; florescent lamp";
        case 0x04 : return "sunlight";
        case 0x05 : return "cloudy weather";
        case 0x1F : return ""; //No info
        default   : return "";
    }
}

//---------------------------------------------------------------------------
const char*  Dv_consumer_camera_1_fcm[]=
{
    "auto focus",
    "manual focus",
};

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_DvDif::File_DvDif()
:File__Analyze()
{
    //In
    Frame_Count_Valid=14;
    AuxToAnalyze=0x00; //No Aux to analyze
    IgnoreAudio=false;

    //Temp
    FrameCount=0;
    FrameSize_Theory=0;
    Duration=0;
    TimeCode_First=(int64u)-1;
    SCT_Old=4; //Video
    DBN_Olds[0]=0;
    DBN_Olds[1]=1; //SubCode
    DBN_Olds[2]=2; //Vaux
    DBN_Olds[3]=8; //Audio
    DBN_Olds[4]=134; //Video
    DBN_Olds[5]=0;
    DBN_Olds[6]=0;
    DBN_Olds[7]=0;
    consumer_camera_1_Parsed=false;
    consumer_camera_2_Parsed=false;
    DSF_IsValid=false;
    APT=0xFF; //Impossible
    TF1=false; //Valid by default, for direct analyze
    TF2=false; //Valid by default, for direct analyze
    TF3=false; //Valid by default, for direct analyze

    #ifdef MEDIAINFO_DVDIF_ANALYZE_YES
    Analyze_Activated=false;
    Speed_FrameCount=0;
    Speed_FrameCount_Video_STA_Errors=0;
    Speed_FrameCount_Audio_Errors.resize(8);
    Speed_FrameCount_Timecode_Incoherency=0;
    Speed_FrameCount_Contains_NULL=0;
    Speed_Contains_NULL=0;
    Speed_FrameCount_Arb_Incoherency=0;
    Speed_FrameCount_Stts_Fluctuation=0;
    System_IsValid=false;
    Frame_AtLeast1DIF=false;
    QU=(int8u)-1;
    CH_IsPresent.resize(8);
    Mpeg4_stts=NULL;
    Mpeg4_stts_Pos=0;
    #endif //MEDIAINFO_DVDIF_ANALYZE_YES
}

//---------------------------------------------------------------------------
File_DvDif::~File_DvDif()
{
    #if defined(MEDIAINFO_EIA608_YES)
        for (size_t Pos=0; Pos<CC_Parsers.size(); Pos++)
            delete CC_Parsers[Pos]; //CC_Parsers[Pos]=NULL;
    #endif
    #if defined(MEDIAINFO_DVDIF_ANALYZE_YES)
        delete Mpeg4_stts; //Mpeg4_stts=NULL;
    #endif
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_DvDif::Streams_Finish()
{
    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);
    Fill(Stream_General, 0, General_Format, "Digital Video");

    if (!Recorded_Date_Date.empty())
    {
        if (Recorded_Date_Time.size()>4)
        {
            Recorded_Date_Time.resize(Recorded_Date_Time.size()-4); //Keep out milliseconds
            Recorded_Date_Date+=_T(" ");
            Recorded_Date_Date+=Recorded_Date_Time;
        }
        Fill(Stream_General, 0, General_Recorded_Date, Recorded_Date_Date);
    }
    if (!File_Name.empty() && Duration)
        Fill(Stream_General, 0, General_Duration, Duration);

    //Delay
    if (TimeCode_First!=(int64u)-1)
    {
        Fill(Stream_Video, 0, Video_Delay, TimeCode_First);
        for (size_t Pos=0; Pos<Count_Get(Stream_Audio); Pos++)
            Fill(Stream_Audio, Pos, Audio_Delay, TimeCode_First);
    }

    #if defined(MEDIAINFO_EIA608_YES)
        for (size_t Pos=0; Pos<CC_Parsers.size(); Pos++)
            if (CC_Parsers[Pos] && CC_Parsers[Pos]->Status[IsAccepted])
            {
                CC_Parsers[Pos]->Finish();
                Merge(*CC_Parsers[Pos]);
                Fill(Stream_Text, StreamPos_Last, Text_ID, Pos);
            }
    #endif

    #ifdef MEDIAINFO_DVDIF_ANALYZE_YES
        if (Config->File_DvDif_Analysis_Get())
        {
            //Errors stats
            Status[IsFinished]=true; //We need to fill it before the call to Errors_Stats_Update
            Errors_Stats_Update();
            Errors_Stats_Update_Finnish();
        }
    #endif //MEDIAINFO_DVDIF_ANALYZE_YES
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
void File_DvDif::Header_Parse()
#ifndef MEDIAINFO_MINIMIZESIZE
{
    if (AuxToAnalyze!=0x00)
    {
        Header_Fill_Code(AuxToAnalyze, Ztring::ToZtring(AuxToAnalyze, 16));
        Header_Fill_Size(4);
        return;
    }

    //Unsynch problems
    if (Element_Size<80)
    {
        Element_WaitForMoreData();
        return;
    }
    if (Buffer[Buffer_Offset  ]==0x00
     && Buffer[Buffer_Offset+1]==0x00
     && Buffer[Buffer_Offset+2]==0x00)
    {
        Header_Fill_Code((int64u)-1);
        Header_Fill_Size(80);
        return;
    }

    //Parsing
    BS_Begin();
    //0
    Get_S1 (3, SCT,                                             "SCT - Section Type"); Param_Info(Dv_sct[SCT]);
    Skip_SB(                                                    "Res - Reserved");
    Skip_S1(4,                                                  "Arb - Arbitrary bits");
    //1
    Get_S1 (4, Dseq,                                            "Dseq - DIF sequence number"); //0-9 for 525/60; 0-11 for 625/50
    Get_S1 (1, FSC,                                             "FSC - Channel number");
    Skip_S1(3,                                                  "Res - Reserved");
    BS_End();
    //2
    Get_B1 (DBN,                                                "DBN - DIF block number"); //Video: 0-134, Audio: 0-8

    Header_Fill_Code(SCT, Dv_sct[SCT]);
    Header_Fill_Size(80);
}
#else //MEDIAINFO_MINIMIZESIZE
{
    if (AuxToAnalyze!=0x00)
    {
        Header_Fill_Code(AuxToAnalyze);
        Header_Fill_Size(4);
        return;
    }

    //Unsynch problems
    if (Element_Size<80)
    {
        Element_WaitForMoreData();
        return;
    }
    if (Buffer[Buffer_Offset  ]==0x00
     && Buffer[Buffer_Offset+1]==0x00
     && Buffer[Buffer_Offset+2]==0x00)
    {
        Header_Fill_Code((int64u)-1);
        Header_Fill_Size(80);
        return;
    }
    
    //Parsing
    SCT =(Buffer[Buffer_Offset  ]&0xE0)>>5;
    Dseq=(Buffer[Buffer_Offset+1]&0xF0)>>4;
    FSC =(Buffer[Buffer_Offset+1]&0x08)>>3;
    DBN = Buffer[Buffer_Offset+2];
    Element_Offset+=3;

    Header_Fill_Code(SCT);
    Header_Fill_Size(80);
}
#endif //MEDIAINFO_MINIMIZESIZE

//---------------------------------------------------------------------------
void File_DvDif::Data_Parse()
{
    if (AuxToAnalyze!=0x00)
    {
        Element();
        return;
    }

    if (Element_Code==(int64u)-1)
    {
        Skip_XX(Element_Size,                                   "Junk");
        return;
    }

    //Integrity
    if (!Status[IsAccepted])
    {
        //DIF Sequence Numbers
        if (DSF_IsValid)
        {
            if (Dseq_Old!=Dseq)
            {
                if (Dseq==0
                 && !(!DSF && Dseq_Old==9)
                 && !( DSF && Dseq_Old==11))
                    Trusted_IsNot("Wrong order");
                Dseq_Old=Dseq;
            }
        }

        //DIF Block Numbers
        if (Element_Code>=8)
            return;    
        int8u Number=DBN_Olds[(size_t)Element_Code]+1;
        switch (SCT)
        {
            case 0 : //Header
                        if (SCT_Old!=4
                         || DBN!=0)
                            Trusted_IsNot("Wrong order");
                        break;
            case 1 : //Subcode
                        if (!((DBN==0 && SCT_Old==0) || (DBN!=0 && SCT_Old==1))
                         || Number!=DBN && !(Number==2 && DBN==0))
                            Trusted_IsNot("Wrong order");
                        break;
            case 2 : //VAUX
                        if (!((DBN==0 && SCT_Old==1) || (DBN!=0 && SCT_Old==2))
                         || Number!=DBN && !(Number==3 && DBN==0))
                            Trusted_IsNot("Wrong order");
                        break;
            case 3 : //Audio
                        if (!((DBN==0 && SCT_Old==2) || (DBN!=0 && SCT_Old==4))
                         || Number!=DBN && !(Number==9 && DBN==0))
                            Trusted_IsNot("Wrong order");
                        break;
            case 4 : //Video
                        if (!(SCT_Old==3 || SCT_Old==4)
                         || Number!=DBN && !(Number==135 && DBN==0))
                            Trusted_IsNot("Wrong order");
                        break;
            default: ;
        }
        SCT_Old=SCT;
        if (SCT<8)
            DBN_Olds[SCT]=DBN;
    }

    Element_Info(DBN);

    switch (SCT)
    {
        case 0 : Header(); break;
        case 1 : Subcode(); break;
        case 2 : VAUX(); break;
        case 3 : Audio(); break;
        #ifndef MEDIAINFO_MINIMIZESIZE
        case 4 : Video(); break;
        default: Skip_XX(Element_Size,                          "Unknown");
        #endif //MEDIAINFO_MINIMIZESIZE
    }
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_DvDif::Header()
#ifndef MEDIAINFO_MINIMIZESIZE
{
    BS_Begin();
    //3
    Get_SB (   DSF,                                             "DSF - DIF Sequence Flag"); //0=NTSC, 1=PAL
    Skip_SB(                                                    "Zero");
    Skip_S1(6,                                                  "Reserved");

    //4
    Skip_S1(5,                                                  "Reserved");
    Get_S1 (3, APT,                                             "APT"); //Track application ID, 0=4:2:0, 1=not 4:2:0

    //5
    Get_SB (   TF1,                                             "TF1 - Audio data is not valid");
    Skip_S1(4,                                                  "Reserved");
    Skip_S1(3,                                                  "AP1 - Audio application ID");

    //6
    Get_SB (  TF2,                                              "TF2 - Video data is not valid");
    Skip_S1(4,                                                  "Reserved");
    Skip_S1(3,                                                  "AP2 - Video application ID");

    //7
    Get_SB (  TF3,                                              "TF3 - Subcode is not valid");
    Skip_S1(4,                                                  "Reserved");
    Skip_S1(3,                                                  "AP3 - Subcode application ID");

    //8-79
    BS_End();
    Skip_XX(72,                                                 "Reserved"); //Should be filled with 0xFF

    FILLING_BEGIN();
        DSF_IsValid=true;
        Dseq_Old=DSF?11:9;
        FrameSize_Theory=(DSF?12:10)*150*80; //12 DIF sequences for PAL, 10 for NTSC

        if (TF1 && TF2)
        {
            //This is not logic, the header says no audio and no video! We do not trust the header, resetting all
            TF1=false;
            TF2=false;
            TF3=false;
        }

        FrameCount++;
        if (Count_Get(Stream_General)==0)
            Stream_Prepare(Stream_General);
        if (!Status[IsAccepted] && (FrameCount>=10 || IsSub))
            Accept("DV DIF");
        if (!Status[IsFilled] && FrameCount>=Frame_Count_Valid)
            #ifdef MEDIAINFO_DVDIF_ANALYZE_YES
                if (Config->File_DvDif_Analysis_Get())
                    Fill("DV DIF");
                else
                    Finish("DV DIF");
            #else //MEDIAINFO_DVDIF_ANALYZE_YES
                Finish("DV DIF");
            #endif //MEDIAINFO_DVDIF_ANALYZE_YES
    FILLING_END();
}
#else //MEDIAINFO_MINIMIZESIZE
{
    if (Element_Size<77)
    {
        Trusted_IsNot("Size is wrong");
        return;
    }

    DSF=(Buffer[Buffer_Offset  ]&0x80)?true:false;
    APT=(Buffer[Buffer_Offset+1]&0x07);
    TF1=(Buffer[Buffer_Offset+2]&0x80)?true:false;
    TF2=(Buffer[Buffer_Offset+3]&0x80)?true:false;
    TF3=(Buffer[Buffer_Offset+4]&0x80)?true:false;

    FILLING_BEGIN();
        DSF_IsValid=true;
        Dseq_Old=DSF?11:9;
        FrameSize_Theory=(DSF?12:10)*150*80; //12 DIF sequences for PAL, 10 for NTSC

        if (TF1 && TF2)
        {
            //This is not logic, the header says no audio and no video! We do not trust the header, resetting all
            TF1=false;
            TF2=false;
            TF3=false;
        }

        FrameCount++;
        if (Count_Get(Stream_General)==0)
            Stream_Prepare(Stream_General);
        if (!Status[IsAccepted] && (FrameCount>=10 || IsSub))
            Accept("DV DIF");
        if (!Status[IsFilled] && FrameCount>=Frame_Count_Valid)
            #ifdef MEDIAINFO_DVDIF_ANALYZE_YES
                if (Config->File_DvDif_Analysis_Get())
                    Fill("DV DIF");
                else
                    Finish("DV DIF");
            #else //MEDIAINFO_DVDIF_ANALYZE_YES
                Finish("DV DIF");
            #endif //MEDIAINFO_DVDIF_ANALYZE_YES
    FILLING_END();
}
#endif //MEDIAINFO_MINIMIZESIZE

//---------------------------------------------------------------------------
void File_DvDif::Subcode()
#ifndef MEDIAINFO_MINIMIZESIZE
{
    //Present?
    if (TF3)
    {
        Skip_XX(Element_Size,                                   "Unused");
        return;
    }

    //Parsing
    for (int8u syb_num=0; syb_num<6; syb_num++)
        Subcode_Ssyb(syb_num);
    Skip_XX(29,                                                 "Unused");
}
#else //MEDIAINFO_MINIMIZESIZE
{
    if (TF3)
        return;

    for (int8u syb_num=0; syb_num<6; syb_num++)
        Subcode_Ssyb(syb_num);
}
#endif //MEDIAINFO_MINIMIZESIZE

//---------------------------------------------------------------------------
void File_DvDif::Subcode_Ssyb(int8u syb_num)
{
    Element_Begin("ssyb");

    //Parsing
    BS_Begin();
    //ID0-ID1
    Skip_SB(                                                    "FR - Identification of half of channel"); //1=first half, 0=second
    if (syb_num==0)
        Skip_S1( 3,                                             "AP3 - Subcode application ID");
    else if (DBN==1 && syb_num==5)
        Skip_S1(3,                                              "APT - track application ID");
    else
        Skip_S1(3,                                              "Res - Reserved");
    Skip_S1(8,                                                  "Arb - Arbitrary bits");
    Skip_S1(4,                                                  "Syb - SSYSB number");
    BS_End();
    //FFh
    Skip_B1(                                                    "0xFF");
    //PC0-PC4
    Element();

    Element_End();
}

//---------------------------------------------------------------------------
void File_DvDif::VAUX()
{
    //Present?
    if (TF2)
    {
        Skip_XX(Element_Size,                                   "Unused");
        return;
    }

    //Parsing
    for (int8u i=0; i<15; i++)
        Element();
    Skip_XX(2,                                                  "Unused");
}

//---------------------------------------------------------------------------
void File_DvDif::Audio()
{
    //Present?
    if (TF1)
    {
        Skip_XX(Element_Size,                                   "Unused");
        return;
    }

    Element_Name("Audio");

    Element(); //First 5 bytes
    Skip_XX(Element_Size-Element_Offset,                        "Unknown");
}

//---------------------------------------------------------------------------
void File_DvDif::Video()
{
    //Present?
    if (TF2)
    {
        Skip_XX(Element_Size,                                   "Unused");
        return;
    }

    Element_Name("Video");

    //Parsing
    BS_Begin();
    Skip_S1(4,                                                  "STA");
    Skip_S1(4,                                                  "QNO");
    BS_End();
    Skip_XX(Element_Size-Element_Offset,                        "Unknown");
}

//---------------------------------------------------------------------------
void File_DvDif::Element()
{
    Element_Begin();
    int8u PackType;
    if (AuxToAnalyze==0x00)
        Get_B1 (PackType,                                       "Pack Type");
    else
        PackType=AuxToAnalyze; //Forced by parser

    switch(PackType)
    {
        case 0x13 : timecode(); break;
        case 0x50 : audio_source(); break;
        case 0x51 : audio_sourcecontrol(); break;
        case 0x52 : audio_recdate(); break;
        case 0x53 : audio_rectime(); break;
        case 0x60 : video_source(); break;
        case 0x61 : video_sourcecontrol(); break;
        case 0x62 : video_recdate(); break;
        case 0x63 : video_rectime(); break;
        case 0x65 : closed_captions(); break;
        case 0x70 : consumer_camera_1(); break;
        case 0x71 : consumer_camera_2(); break;
        case 0xFF : Element_Name(Ztring().From_Number(PackType, 16));
                    Skip_B4(                                    "Unused"); break;
        default   : Element_Name(Ztring().From_Number(PackType, 16));
                    Skip_B4(                                    "Unknown");
    }
    Element_End();
}

//---------------------------------------------------------------------------
void File_DvDif::timecode()
{
    Element_Name("timecode");

    if (Buffer[Buffer_Offset+(size_t)Element_Offset  ]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+1]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+2]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+3]==0x00
    )
    {
        Skip_XX(4,                                              "All zero");
        return;
    }

    //PArsing
    int8u Temp;
    int64u MilliSeconds=0;
    int8u  Frames=0;
    bool   DropFrame=false;
    BS_Begin();
    Skip_SB(                                                    "CF - Color fame");
    if (!DSF_IsValid)
        Skip_SB(                                                "Arbitrary bit or DP");
    else if (DSF)    //625/50
        Skip_SB(                                                "Arbitrary bit");
    else        //525/60
        Get_SB (DropFrame,                                      "DP - Drop frame"); //525/60
    Get_S1 (2, Temp,                                            "Frames (Tens)");
    Frames+=Temp*10;
    Get_S1 (4, Temp,                                            "Frames (Units)");
    Frames+=Temp;

    if (!DSF_IsValid)
        Skip_SB(                                                "BGF0 or PC");
    else if (DSF)    //625/50
        Skip_SB(                                                "BGF0 - Binary group flag");
    else        //525/60
        Skip_SB(                                                "PC - Biphase mark polarity correction"); //0=even; 1=odd
    Get_S1 (3, Temp,                                            "Seconds (Tens)");
    MilliSeconds+=Temp*10*1000;
    Get_S1 (4, Temp,                                            "Seconds (Units)");
    MilliSeconds+=Temp*1000;

    if (!DSF_IsValid)
        Skip_SB(                                                "BGF2 or BGF0");
    else if (DSF)    //625/50
        Skip_SB(                                                "BGF2 - Binary group flag");
    else        //525/60
        Skip_SB(                                                "BGF0 - Binary group flag");
    Get_S1 (3, Temp,                                            "Minutes (Tens)");
    MilliSeconds+=Temp*10*60*1000;
    Get_S1 (4, Temp,                                            "Minutes (Units)");
    MilliSeconds+=Temp*60*1000;

    if (!DSF_IsValid)
        Skip_SB(                                                "PC or BGF1");
    else if (DSF)    //625/50
        Skip_SB(                                                "PC - Biphase mark polarity correction"); //0=even; 1=odd
    else        //525/60
        Skip_SB(                                                "BGF1 - Binary group flag");
    Skip_SB(                                                    "BGF2 - Binary group flag");
    Get_S1 (2, Temp,                                            "Hours (Tens)");
    MilliSeconds+=Temp*10*60*60*1000;
    Get_S1 (4, Temp,                                            "Hours (Units)");
    MilliSeconds+=Temp*60*60*1000;
    Element_Info(Ztring().Duration_From_Milliseconds(MilliSeconds+((DSF_IsValid && Frames!=45)?((int64u)(Frames/(DSF?25.000:29.970)*1000)):0)));
    BS_End();

    if (TimeCode_First==(int64u)-1 && MilliSeconds!=167185000) //if all bits are set to 1, this is not a valid timestamp
    {
        TimeCode_First=MilliSeconds;
        if (DSF_IsValid && Frames!=45) //all bits are set to 1
            TimeCode_First+=(int64u)(Frames/(DSF?25.000:29.970)*1000);
    }
}

//---------------------------------------------------------------------------
void File_DvDif::audio_source()
{
    if (TF1)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("audio_source");

    int8u stype, SamplingRate, Resolution;
    BS_Begin();
    //PC1
    Skip_SB(                                                    "LF - Locked mode");
    Skip_SB(                                                    "Reserved");
    Skip_S1(6,                                                  "AF - Samples in this frame");

    //PC2
    Info_S1(1, StereoMode,                                      "SM - Stereo mode"); Param_Info(Dv_StereoMode[StereoMode]);
    Info_S1(2, ChannelsPerBlock,                                "CHN - Channels per block"); Param_Info(Dv_ChannelsPerBlock[ChannelsPerBlock]);
    Info_S1(1, Pair,                                            "PA - Pair"); Param_Info(Dv_Pair[Pair]);
    Skip_S1(4,                                                  "AM - Audio mode");

    Skip_SB(                                                    "Reserved");
    Skip_SB(                                                    "ML - Multi-language");
    Skip_SB(                                                    "50/60");
    Get_S1 (5, stype,                                           "STYPE - audio blocks per video frame"); Param_Info(stype==0?"2 channels":(stype==2?"4 channels":"Unknown")); //0=25 Mbps, 2=50 Mbps

    Skip_SB(                                                    "EF - Emphasis off");
    Skip_SB(                                                    "TC - Time constant of emphasis");
    Get_S1 (3, SamplingRate,                                    "SMP - Sampling rate"); Param_Info(Dv_Audio_SamplingRate[SamplingRate]);
    Get_S1 (3, Resolution,                                      "QU - Resolution"); Param_Info(Dv_Audio_Resolution[Resolution]);
    BS_End();

    FILLING_BEGIN();
        if (!IgnoreAudio && (FrameCount==1 || AuxToAnalyze)) //Only the first time
        {
            Stream_Prepare(Stream_Audio);
            Fill(Stream_Audio, 0, Audio_ID, 0);
            Fill(Stream_Audio, 0, Audio_Format, "PCM");
            Fill(Stream_Audio, 0, Audio_Codec, "PCM");
            Fill(Stream_Audio, 0, Audio_Channel_s_, 2);
            Fill(Stream_Audio, 0, Audio_SamplingRate, Dv_Audio_SamplingRate[SamplingRate]);
            Fill(Stream_Audio, 0, Audio_Resolution, Dv_Audio_Resolution[Resolution]);
            Fill(Stream_Audio, 0, Audio_BitRate, 2*Dv_Audio_SamplingRate[SamplingRate]*Dv_Audio_Resolution[Resolution]);

            if (stype==2 || (Resolution==1 && SamplingRate==2)) //stype=? or (Resolution=12 bits and SamplingRate=32 KHz)
            {
                Stream_Prepare(Stream_Audio);
                Fill(Stream_Audio, 1, Audio_ID, 1);
                Fill(Stream_Audio, 1, Audio_Format, "PCM");
                Fill(Stream_Audio, 1, Audio_Codec, "PCM");
                Fill(Stream_Audio, 1, Audio_Channel_s_, 2);
                Fill(Stream_Audio, 1, Audio_SamplingRate, Dv_Audio_SamplingRate[SamplingRate]);
                Fill(Stream_Audio, 1, Audio_Resolution, Dv_Audio_Resolution[Resolution]);
                Fill(Stream_Audio, 1, Audio_BitRate, 2*Dv_Audio_SamplingRate[SamplingRate]*Dv_Audio_Resolution[Resolution]);
            }
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_DvDif::audio_sourcecontrol()
{
    if (TF1)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("audio_control");

    BS_Begin();

    //PC1
    Info_S1(2, CopyGenerationManagementSystem,                  "CGMS - Copy generation management system"); Param_Info(Dv_CopyGenerationManagementSystem[CopyGenerationManagementSystem]);
    Info_S1(2, InputType,                                       "ISR - Input type"); Param_Info(Dv_InputType[InputType]);
    Info_S1(2, CompressionTimes,                                "CMP - Compression times"); Param_Info(Dv_CompressionTimes[CompressionTimes]);
    Info_S1(2, Emphasis,                                        "EFC - Emphasis"); Param_Info(Dv_Emphasis[Emphasis]);

    //PC2
    Skip_SB(                                                    "REC S Non-recording start point");
    Skip_SB(                                                    "REC E - Non-recording end point");
    Skip_SB(                                                    "FADE S - Recording mode"); //1=Original
    Skip_SB(                                                    "FADE E - Unknown");
    Skip_SB(                                                    "Reserved");
    Skip_SB(                                                    "Reserved");
    Skip_SB(                                                    "Reserved");
    Skip_SB(                                                    "Reserved");

    //PC3
    Skip_SB(                                                    "DRF - Direction"); //1=Forward
    Skip_S1(7,                                                  "SPD - Speed");

    //PC4
    Skip_SB(                                                    "Reserved");
    Skip_S1(7,                                                  "GEN - Category");

    BS_End();
}

//---------------------------------------------------------------------------
void File_DvDif::audio_recdate()
{
    if (TF1)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("audio_recdate");

    recdate();
}

//---------------------------------------------------------------------------
void File_DvDif::audio_rectime()
{
    if (TF1)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("audio_rectime");

    rectime();
}

//---------------------------------------------------------------------------
void File_DvDif::video_source()
{
    if (TF2)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("video_source");

    int8u stype;
    bool system;
    BS_Begin();
    //PC1
    Skip_S1(4,                                                  "TVCH (tens of units, 0�9)");
    Skip_S1(4,                                                  "TVCH (units, 0�9)");

    //PC2
    Skip_SB(                                                    "B/W - Black and White"); //0=Black and White, 1=Color
    Skip_SB(                                                    "EN - Color Frames is not valid");
    Skip_S1(2,                                                  "CLF - Color frames id");
    Skip_S1(4,                                                  "TVCH (hundreds of units, 0�9)");

    //PC3
    Skip_S1(2,                                                  "SRC");
    Get_SB (   system,                                          "50/60 - System");
    Get_S1 (4, stype,                                           "STYPE - Signal type of video signal"); //0=not 4:2:2, 4=4:2:2

    //PC4
    BS_End();
    Skip_B1(                                                    "TUN/VISC");

    FILLING_BEGIN();
        if ((FrameCount==1 || AuxToAnalyze) && Count_Get(Stream_Video)==0) //Only the first time
        {
            Stream_Prepare(Stream_Video);
            Fill(Stream_Video, 0, Video_Format, "Digital Video");
            Fill(Stream_Video, 0, Video_Codec, "DV");
            Fill(Stream_Video, 0, Video_Standard, system?"PAL":"NTSC");
            Fill(Stream_Video, 0, Video_Width, 720);
            Fill(Stream_Video, 0, Video_Height, system?576:480);
            Fill(Stream_Video, 0, Video_FrameRate, system?25.000:29.970);
            Fill(Stream_Video, 0, Video_FrameRate_Mode, "CFR");

            if (system==false && stype==4) //NTSC and 4:2:2
                Fill(Stream_Video, 0, Video_Colorimetry, "4:2:2");       //NTSC 50 Mbps
            else if (system==false) //NTSC and not 4:2:2 (--> 4:1:1)
                Fill(Stream_Video, 0, Video_Colorimetry, "4:1:1");       //NTSC 25 Mbps
            else if (stype==4) //PAL and 4:2:2
                Fill(Stream_Video, 0, Video_Colorimetry, "4:2:2");       //PAL  50 Mbps
            else if (APT==0) //PAL and 4:2:0
                Fill(Stream_Video, 0, Video_Colorimetry, "4:2:0");       //PAL  25 Mbps 4:2:0
            else if (APT==1) //PAL and not 4:2:0 (--> 4:1:1)
                Fill(Stream_Video, 0, Video_Colorimetry, "4:1:1");       //PAL  25 Mbps 4:1:1

            if (FrameSize_Theory)
                Duration=(int64u)(File_Size*1000/(FrameSize_Theory*(DSF?25.000:29.970)));
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_DvDif::video_sourcecontrol()
{
    if (TF2)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("video_control");

    BS_Begin();
    //PC1
    Info_S1(2, CopyGenerationManagementSystem,                  "CGMS - Copy generation management system"); Param_Info(Dv_CopyGenerationManagementSystem[CopyGenerationManagementSystem]);
    Skip_S1(2,                                                  "ISR");
    Skip_S1(2,                                                  "CMP");
    Skip_S2(2,                                                  "SS");

    //PC2
    Skip_SB(                                                    "REC S");
    Skip_SB(                                                    "Reserved");
    Skip_S1(2,                                                  "REC M");
    Skip_SB(                                                    "Reserved");
    Get_S1 (3, aspect,                                          "DISP - Aspect ratio"); Param_Info(Dv_Disp[aspect]);

    //PC3
    Skip_SB(                                                    "FF - Frame/Field"); //1=Frame, 0=Field
    Skip_SB(                                                    "FS - First/second field"); //0=Field 2, 1=Field 1, if FF=0 x is output twice, if FF=1, Field x fisrst, other second
    Skip_SB(                                                    "FC - Frame Change"); //0=Same picture as before
    Get_SB (   Interlaced,                                      "IL - Interlaced"); //1=Interlaced
    Skip_SB(                                                    "SF");
    Skip_SB(                                                    "SC");
    Skip_S1(2,                                                  "BCS");

    //PC4
    Skip_SB(                                                    "Reserved");
    Skip_S1(7,                                                  "GEN - Category");

    BS_End();

    FILLING_BEGIN();
        if ((FrameCount==1 || AuxToAnalyze) && Retrieve(Stream_Video, 0, Video_ScanType).empty()) //Only the first time
        {
            Fill(Stream_Video, 0, Video_ScanType, Interlaced?"Interlaced":"Progressive");
            Fill(Stream_Video, 0, Video_Interlacement, Interlaced?"Interlaced":"PFF");
            switch (aspect)
            {
                case 0 : Fill(Stream_Video, 0, Video_DisplayAspectRatio, 4.0/3.0, 3, true); break;
                case 2 :
                case 7 : Fill(Stream_Video, 0, Video_DisplayAspectRatio, 16.0/9.0, 3, true); break;
                default: ;
            }
        }

        if (!Status[IsAccepted] && AuxToAnalyze)
        {
            Accept("DV DIF");
            Fill("DV DIF");
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_DvDif::video_recdate()
{
    if (TF2)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("video_recdate");

    Ztring Date=recdate();
    if (Recorded_Date_Date.empty())
        Recorded_Date_Date=Date;
}

//---------------------------------------------------------------------------
void File_DvDif::video_rectime()
{
    if (TF2)
    {
        Skip_XX(4,                                              "Unused");
        return;
    }

    Element_Name("video_rectime");

    Ztring Date=rectime();
    if (Recorded_Date_Time.empty())
        Recorded_Date_Time=Date;
}

//---------------------------------------------------------------------------
void File_DvDif::closed_captions()
{
    Element_Name("closed_captions");

    #if defined(MEDIAINFO_EIA608_YES)
        if (CC_Parsers.empty())
        {
            CC_Parsers.resize(2);
            for (size_t Pos=0; Pos<2; Pos++)
                CC_Parsers[Pos]=new File_Eia608();
            Frame_Count_Valid*=10; //More frames
        }
        if (Dseq==0) //CC are duplicated for each DIF sequence!
        {
            for (size_t Pos=0; Pos<2; Pos++)
            {
                Open_Buffer_Init(CC_Parsers[Pos]);
                Open_Buffer_Continue(CC_Parsers[Pos], Buffer+Buffer_Offset+(size_t)Element_Offset, 2);
                Element_Offset+=2;
            }
        }

    #else
        Skip_XX(4,                                              "Captions");
    #endif
}

//---------------------------------------------------------------------------
void File_DvDif::consumer_camera_1()
{
    Element_Name("consumer_camera_1");

    //Parsing
    BS_Begin();
    Mark_1_NoTrustError();
    Mark_1_NoTrustError();
    Skip_S1(6,                                                  "iris");
    Info_S1(4, ae_mode,                                         "ae mode"); Param_Info(Dv_consumer_camera_1_ae_mode[ae_mode]);
    Skip_S1(4,                                                  "agc(Automatic Gain Control)");
    Info_S1(3, wb_mode,                                         "wb mode (white balance mode)"); Param_Info(Dv_consumer_camera_1_wb_mode[wb_mode]);
    Info_S1(5, white_balance,                                   "white balance"); Param_Info(Dv_consumer_camera_1_white_balance(white_balance));
    Info_S1(1, fcm,                                             "fcm (Focus mode)"); Param_Info(Dv_consumer_camera_1_fcm[fcm]);
    Skip_S1(7,                                                  "focus (focal point)");
    BS_End();

    if (!consumer_camera_1_Parsed)
    {
        if (ae_mode<0x0F) Fill(Stream_Video, 0, Video_Encoded_Library_Settings, _T("ae mode=")+Ztring(Dv_consumer_camera_1_ae_mode[ae_mode]));
        if (wb_mode<0x08) Fill(Stream_Video, 0, Video_Encoded_Library_Settings, _T("wb mode=")+Ztring(Dv_consumer_camera_1_wb_mode[wb_mode]));
        if (wb_mode<0x1F) Fill(Stream_Video, 0, Video_Encoded_Library_Settings, _T("white balance=")+Ztring(Dv_consumer_camera_1_white_balance(white_balance)));
                          Fill(Stream_Video, 0, Video_Encoded_Library_Settings, _T("fcm=")+Ztring(Dv_consumer_camera_1_fcm[fcm]));
        consumer_camera_1_Parsed=true;
    }
}

//---------------------------------------------------------------------------
void File_DvDif::consumer_camera_2()
{
    Element_Name("consumer_camera_2");

    //Parsing
    BS_Begin();
    Mark_1_NoTrustError();
    Mark_1_NoTrustError();
    Skip_S1(1,                                                  "vpd");
    Skip_S1(5,                                                  "vertical panning speed");
    Skip_S1(1,                                                  "is");
    Skip_S1(1,                                                  "hpd");
    Skip_S1(6,                                                  "horizontal panning speed");
    Skip_S1(8,                                                  "focal length");
    Skip_S1(1,                                                  "zen");
    Info_S1(3, zoom_U,                                          "units of e-zoom");
    Info_S1(4, zoom_D,                                          "1/10 of e-zoom"); if (zoom_D!=0xF) Param_Info(_T("zoom=")+Ztring().From_Number(zoom_U+((float32)zoom_U)/10, 2));
    BS_End();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
Ztring File_DvDif::recdate()
{
    BS_Begin();

    int8u Temp;
    int16u Year=0;
    int8u  Month=0, Day=0;
    Skip_S1(8,                                                  "Time zone specific"); //ds, tm, tens of time zone, units of time zone, 0xFF for Unknwon
    Skip_SB(                                                    "1");
    Skip_SB(                                                    "1");
    Get_S1 (2, Temp,                                            "Days (Tens)");
    Day+=Temp*10;
    Get_S1 (4, Temp,                                            "Days (Units)");
    Day+=Temp;
    Skip_SB(                                                    "1");
    Skip_SB(                                                    "1");
    Skip_SB(                                                    "1");
    Get_S1 (1, Temp,                                            "Month (Tens)");
    Month+=Temp*10;
    Get_S1 (4, Temp,                                            "Month (Units)");
    Month+=Temp;
    Get_S1 (4, Temp,                                            "Year (Tens)");
    Year+=Temp*10;
    Get_S1 (4, Temp,                                            "Year (Units)");
    Year+=Temp;
    Year+=Year<25?2000:1900;
    Element_Info(Ztring::ToZtring(Year)+_T("-")+Ztring::ToZtring(Month)+_T("-")+Ztring::ToZtring(Day));

    BS_End();

    if (Month>12 || Day>31)
        return Ztring(); //If all bits are set to 1, this is invalid
    Ztring MonthString;
    if (Month<10)
        MonthString=_T("0");
    MonthString+=Ztring::ToZtring(Month);
    Ztring DayString;
    if (Day<10)
        DayString=_T("0");
    DayString+=Ztring::ToZtring(Day);
    return Ztring::ToZtring(Year)+_T("-")+MonthString+_T("-")+DayString;
}

//---------------------------------------------------------------------------
Ztring File_DvDif::rectime()
{
    if (!DSF_IsValid)
    {
        Trusted_IsNot("Not in right order");
        return Ztring();
    }

    BS_Begin();

    if (Buffer[Buffer_Offset+(size_t)Element_Offset  ]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+1]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+2]==0x00
     && Buffer[Buffer_Offset+(size_t)Element_Offset+3]==0x00
    )
    {
        Skip_XX(4,                                              "All zero");
        return Ztring();
    }

    int8u Temp;
    int64u Time=0;
    int8u Frames=0;
    Skip_SB(                                                    "Unknown");
    Skip_SB(                                                    "1");
    Get_S1 (2, Temp,                                            "Frames (Tens)");
    Frames+=Temp*10;
    Get_S1 (4, Temp,                                            "Frames (Units)");
    Frames+=Temp;
    if (Temp!=0xF && DSF_IsValid)
        Time+=(int64u)(Frames/(DSF?25.000:29.970));
    Skip_SB(                                                    "1");
    Get_S1 (3, Temp,                                            "Seconds (Tens)");
    Time+=Temp*10*1000;
    Get_S1 (4, Temp,                                            "Seconds (Units)");
    Time+=Temp*1000;
    Skip_SB(                                                    "1");
    Get_S1 (3, Temp,                                            "Minutes (Tens)");
    Time+=Temp*10*60*1000;
    Get_S1 (4, Temp,                                            "Minutes (Units)");
    Time+=Temp*60*1000;
    Skip_SB(                                                    "1");
    Skip_SB(                                                    "1");
    Get_S1 (2, Temp,                                            "Hours (Tens)");
    Time+=Temp*10*60*60*1000;
    Get_S1 (4, Temp,                                            "Hours (Units)");
    Time+=Temp*60*60*1000;
    Element_Info(Ztring().Duration_From_Milliseconds(Time));

    BS_End();

    if (Time!=167185000)
        return Ztring().Duration_From_Milliseconds(Time);
    else
        return Ztring(); //If all bits are set to 1, this is invalid
}

} //NameSpace

#endif //MEDIAINFO_DV_YES

