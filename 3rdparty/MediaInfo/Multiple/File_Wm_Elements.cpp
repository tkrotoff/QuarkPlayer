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
// Elements part
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
#if defined(MEDIAINFO_VC1_YES)
    #include "MediaInfo/Video/File_Vc1.h"
#endif
#if defined(MEDIAINFO_MPEGV_YES)
    #include "MediaInfo/Video/File_Mpegv.h"
#endif
#if defined(MEDIAINFO_AC3_YES)
    #include "MediaInfo/Audio/File_Ac3.h"
#endif
#if defined(MEDIAINFO_MPEGA_YES)
    #include "MediaInfo/Audio/File_Mpega.h"
#endif
#if defined(MEDIAINFO_LATM_YES)
   #include "MediaInfo/Audio/File_Latm.h"
#endif
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

const char* Wm_CodecList_Kind(int32u Kind)
{
    switch (Kind)
    {
        case 0x01 : return "Video";
        case 0x02 : return "Audio";
        default   : return "Unknown";
    }
}

const char* Wm_BannerImageData_Type(int32u Type)
{
    switch (Type)
    {
    case 0x00 : return "";
        case 0x01 : return "Bitmap";
        case 0x02 : return "JPEG";
        case 0x03 : return "GIF";
        default   : return "Unknown";
    }
}

#define UUID(NAME, PART1, PART2, PART3, PART4, PART5) \
    const int64u NAME   =0x##PART3##PART2##PART1##ULL; \
    const int64u NAME##2=0x##PART4##PART5##ULL; \

namespace Elements
{
    UUID(Header,                                                75B22630, 668E, 11CF, A6D9, 00AA0062CE6C)
    UUID(Header_FileProperties,                                 8CABDCA1, A947, 11CF, 8EE4, 00C00C205365)
    UUID(Header_StreamProperties,                               B7DC0791, A9B7, 11CF, 8EE6, 00C00C205365)
    UUID(Header_StreamProperties_Audio,                         F8699E40, 5B4D, 11CF, A8FD, 00805F5C442B)
    UUID(Header_StreamProperties_Video,                         BC19EFC0, 5B4D, 11CF, A8FD, 00805F5C442B)
    UUID(Header_StreamProperties_Command,                       59DACFC0, 59E6, 11D0, A3AC, 00A0C90348F6)
    UUID(Header_StreamProperties_JFIF,                          B61BE100, 5B4E, 11CF, A8FD, 00805F5C442B)
    UUID(Header_StreamProperties_DegradableJPEG,                35907DE0, E415, 11CF, A917, 00805F5C442B)
    UUID(Header_StreamProperties_FileTransfer,                  91BD222C, F21C, 497A, 8B6D, 5AA86BFC0185)
    UUID(Header_StreamProperties_Binary,                        3AFB65E2, 47EF, 40F2, AC2C, 70A90D71D343)
    UUID(Header_StreamProperties_Binary_WebStreamMediaSubType,  776257D4, C627, 41CB, 8F81, 7AC7FF1C40CC)
    UUID(Header_StreamProperties_Binary_WebStreamFormat,        DA1E6B13, 8359, 4050, B398, 388E965BF00C)
    UUID(Header_HeaderExtension,                                5FBF03B5, A92E, 11CF, 8EE3, 00C00C205365)
    UUID(Header_HeaderExtension_ExtendedStreamProperties,       14E6A5CB, C672, 4332, 8399, A96952065B5A)
    UUID(Header_HeaderExtension_AdvancedMutualExclusion,        A08649CF, 4775, 4670, 8A16, 6E35357566CD)
    UUID(Header_HeaderExtension_GroupMutualExclusion,           D1465A40, 5A79, 4338, B71B, E36B8FD6C249)
    UUID(Header_HeaderExtension_StreamPrioritization,           D4FED15B, 88D3, 454F, 81F0, ED5C45999E24)
    UUID(Header_HeaderExtension_BandwidthSharing,               A69609E6, 517B, 11D2, B6AF, 00C04FD908E9)
    UUID(Header_HeaderExtension_LanguageList,                   7C4346A9, EFE0, 4BFC, B229, 393EDE415C85)
    UUID(Header_HeaderExtension_Metadata,                       C5F8CBEA, 5BAF, 4877, 8467, AA8C44FA4CCA)
    UUID(Header_HeaderExtension_MetadataLibrary,                44231C94, 9498, 49D1, A141, 1D134E457054)
    UUID(Header_HeaderExtension_IndexParameters,                D6E229DF, 35DA, 11D1, 9034, 00A0C90349BE)
    UUID(Header_HeaderExtension_MediaIndexParameters,           6B203BAD, 3F11, 48E4, ACA8, D7613DE2CFA7)
    UUID(Header_HeaderExtension_TimecodeIndexParameters,        F55E496D, 9797, 4B5D, 8C8B, 604DFE9BFB24)
    UUID(Header_HeaderExtension_Compatibility,                  26F18B5D, 4584, 47EC, 9F5F, 0E651F0452C9)
    UUID(Header_HeaderExtension_AdvancedContentEncryption,      43058533, 6981, 49E6, 9B74, AD12CB86D58C)
    UUID(Header_HeaderExtension_IndexPlaceholder,               D9AADE20, 7C17, 4F9C, BC28, 8555DD98E2A2)
    UUID(Header_CodecList,                                      86D15240, 311D, 11D0, A3A4, 00ACC90348F6)
    UUID(Header_ScriptCommand,                                  1EFB1A30, 0B62, 11D0, A39B, 00A0C90348F6)
    UUID(Header_Marker,                                         F487CD01, A951, 11CF, 8EE6, 00C00C205365)
    UUID(Header_BitRateMutualExclusion,                         D6E229DC, 35DA, 11D1, 9034, 00A0C90349BE)
    UUID(Header_ErrorCorrection,                                75B22635, 668E, 11CF, A6D9, 00AA0062CE6C)
    UUID(Header_ContentDescription,                             75B22633, 668E, 11CF, A6D9, 00AA0062CE6C)
    UUID(Header_ExtendedContentDescription,                     D2D0A440, E307, 11D2, 97F0, 00A0C95EA850)
    UUID(Header_StreamBitRate,                                  7BF875CE, 468D, 11D1, 8D82, 006097C9A2B2)
    UUID(Header_ContentBranding,                                2211B3FA, BD23, 11D2, B4B7, 00A0C955FC6E)
    UUID(Header_ContentEncryption,                              2211B3FB, BD23, 11D2, B4B7, 00A0C955FC6E)
    UUID(Header_ExtendedContentEncryption,                      298AE614, 2622, 4C17, B935, DAE07EE9289C)
    UUID(Header_DigitalSignature,                               2211B3FC, BD23, 11D2, B4B7, 00A0C955FC6E)
    UUID(Header_Padding,                                        1806D474, CADF, 4509, A4BA, 9AABCB96AAE8)
    UUID(Data,                                                  75B22636, 668E, 11CF, A6D9, 00AA0062CE6C)
    UUID(SimpleIndex,                                           33000890, E5B1, 11CF, 89F4, 00A0C90349CB)
    UUID(Index,                                                 D6E229D3, 35DA, 11D1, 9034, 00A0C90349BE)
    UUID(MediaIndex,                                            FEB103F8, 12AD, 4C64, 840F, 2A1D2F7AD48C)
    UUID(TimecodeIndex,                                         3CB73FD0, 0C4A, 4803, 953D, EDF7B6228F0C)

    UUID(Payload_Extension_System_TimeStamp,                    1135BEB7, 3A39, 478A, 98D9, 15C76B00EB69);
    UUID(Mutex_Language,                                        D6E22A00, 35DA, 11D1, 9034, 00A0C90349BE);
    UUID(Mutex_Bitrate,                                         D6E22A01, 35DA, 11D1, 9034, 00A0C90349BE);
}

const char* Wm_StreamType(int128u Kind)
{
    switch (Kind.hi)
    {
        case Elements::Header_StreamProperties_Audio :          return "Audio";
        case Elements::Header_StreamProperties_Video :          return "Video";
        case Elements::Header_StreamProperties_Command :        return "Command";
        case Elements::Header_StreamProperties_JFIF :           return "JFIF";
        case Elements::Header_StreamProperties_DegradableJPEG : return "Degradable JPEG";
        case Elements::Header_StreamProperties_FileTransfer :   return "File Transfer";
        case Elements::Header_StreamProperties_Binary :         return "Binary";
        default :                                               return "Unknown";
    }
}

const char* Wm_ExclusionType(int128u ExclusionType)
{
    switch (ExclusionType.hi)
    {
        case Elements::Header_StreamProperties_Audio :          return "Language";
        case Elements::Header_StreamProperties_Video :          return "Bitrate";
        default :                                               return "Unknown";
    }
}

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
// Element parse
//
void File_Wm::Data_Parse()
{
    //Parsing
    DATA_BEGIN
    LIST(Header)
        ATOM_BEGIN
        ATOM(Header_FileProperties)
        ATOM(Header_StreamProperties)
        LIST(Header_HeaderExtension)
            ATOM_BEGIN
            ATOM(Header_HeaderExtension_ExtendedStreamProperties)
            ATOM(Header_HeaderExtension_AdvancedMutualExclusion)
            ATOM(Header_HeaderExtension_GroupMutualExclusion)
            ATOM(Header_HeaderExtension_StreamPrioritization)
            ATOM(Header_HeaderExtension_BandwidthSharing)
            ATOM(Header_HeaderExtension_LanguageList)
            ATOM(Header_HeaderExtension_Metadata)
            ATOM(Header_HeaderExtension_MetadataLibrary)
            ATOM(Header_HeaderExtension_IndexParameters)
            ATOM(Header_HeaderExtension_MediaIndexParameters)
            ATOM(Header_HeaderExtension_TimecodeIndexParameters)
            ATOM(Header_HeaderExtension_Compatibility)
            ATOM(Header_HeaderExtension_AdvancedContentEncryption)
            ATOM(Header_HeaderExtension_IndexPlaceholder)
            ATOM(Header_Padding)
            ATOM_END
        ATOM(Header_CodecList)
        ATOM(Header_ScriptCommand)
        ATOM(Header_Marker)
        ATOM(Header_BitRateMutualExclusion)
        ATOM(Header_ErrorCorrection)
        ATOM(Header_ContentDescription)
        ATOM(Header_ExtendedContentDescription)
        ATOM(Header_StreamBitRate)
        ATOM(Header_ContentBranding)
        ATOM(Header_ContentEncryption)
        ATOM(Header_ExtendedContentEncryption)
        ATOM(Header_DigitalSignature)
        ATOM(Header_Padding)
        ATOM_END
    LIST(Data)
        ATOM_DEFAULT_ALONE(Data_Packet)
    LIST_SKIP(SimpleIndex)
    LIST_SKIP(Index)
    ATOM(MediaIndex)
    ATOM(TimecodeIndex)
    DATA_END
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_Wm::Header()
{
    Data_Accept("Windows Media");
    Element_Name("Header");

    //Parsing
    Skip_L4(                                                    "Number of Header Objects");
    Skip_L1(                                                    "Alignment");
    Skip_L1(                                                    "Architecture");

    FILLING_BEGIN();
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "Windows Media");
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Wm::Header_FileProperties()
{
    Element_Name("File Properties");

    //Parsing
    int64u CreationDate, PlayDuration, SendDuration, Preroll;
    int32u Flags, MaximumBitRate;
    Skip_GUID(                                                  "File ID");
    Skip_L8(                                                    "File Size");
    Get_L8 (CreationDate,                                       "Creation Date"); Param_Info(Ztring().Date_From_Milliseconds_1601(CreationDate/10000));
    Skip_L8(                                                    "Data Packets Count");
    Get_L8 (PlayDuration,                                       "Play Duration"); Param_Info_From_Milliseconds(PlayDuration/10000);
    Get_L8 (SendDuration,                                       "Send Duration"); Param_Info_From_Milliseconds(SendDuration/10000);
    Get_L8 (Preroll,                                            "Preroll"); Param_Info_From_Milliseconds(Preroll);
    Get_L4 (Flags,                                              "Flags");
        Skip_Flags(Flags, 0,                                    "Broadcast");
        Skip_Flags(Flags, 1,                                    "Seekable");
        Skip_Flags(Flags, 2,                                    "Use Packet Template");
        Skip_Flags(Flags, 3,                                    "Live");
        Skip_Flags(Flags, 4,                                    "Recordable");
        Skip_Flags(Flags, 5,                                    "Unknown Data Size");
    Skip_L4(                                                    "Minimum Data Packet Size");
    Get_L4 (MaximumDataPacketSize,                              "Maximum Data Packet Size");
    Get_L4 (MaximumBitRate,                                     "Maximum Bitrate");

    //Filling
    if (MaximumBitRate)
        Fill(Stream_General, 0, General_OverallBitRate_Maximum, MaximumBitRate);
    Fill(Stream_General, 0, General_Encoded_Date, Ztring().Date_From_Milliseconds_1601(CreationDate/10000));
    if (PlayDuration/1000>Preroll)
        Fill(Stream_General, 0, General_Duration, PlayDuration/10000-Preroll);
    FileProperties_Preroll=(int32u)(Preroll);
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties ()
{
    Element_Name("Stream Properties");

    //Parsing
    int128u StreamType;
    int32u StreamTypeLength, ErrorCorrectionTypeLength;
    Get_GUID(StreamType,                                        "StreamType"); Param_Info(Wm_StreamType(StreamType)); Element_Info(Wm_StreamType(StreamType));
    Skip_GUID(                                                  "Error Correction Type");
    Skip_L8(                                                    "Time Offset");
    Get_L4 (StreamTypeLength,                                   "Type-Specific Data Length");
    Get_L4 (ErrorCorrectionTypeLength,                          "Error Correction Data Length");
    Get_L2 (Stream_Number,                                      "Stream Number");
    if (Stream_Number&0x8000)
    {
        Param_Info("Encrypted Content");
        Stream[Stream_Number&0x007F].Info["Encryption"]=_T("Encrypted");
    }
    Stream_Number&=0x007F; //Only 7bits
    Element_Info(Stream_Number);
    Skip_L4(                                                    "Reserved");
    switch (StreamType.hi)
    {
        case Elements::Header_StreamProperties_Audio :          Element_Begin(StreamTypeLength);
                                                                Header_StreamProperties_Audio();
                                                                Element_End(); break;
        case Elements::Header_StreamProperties_Video :          Element_Begin(StreamTypeLength);
                                                                Header_StreamProperties_Video();
                                                                Element_End(); break;
        case Elements::Header_StreamProperties_JFIF :           Element_Begin(StreamTypeLength);
                                                                Header_StreamProperties_JFIF();
                                                                Element_End(); break;
        case Elements::Header_StreamProperties_DegradableJPEG : Element_Begin(StreamTypeLength);
                                                                Header_StreamProperties_DegradableJPEG();
                                                                Element_End(); break;
        case Elements::Header_StreamProperties_FileTransfer :
        case Elements::Header_StreamProperties_Binary :         Element_Begin(StreamTypeLength);
                                                                Header_StreamProperties_Binary();
                                                                StreamKind_Last=Stream_Max; StreamPos_Last=(size_t)-1;
                                                                Element_End(); break;
        default :                                               if (StreamTypeLength>0)
                                                                    Skip_XX(StreamTypeLength, "Type-Specific Data");
                                                                StreamKind_Last=Stream_Max; StreamPos_Last=(size_t)-1;
    }
    if (ErrorCorrectionTypeLength)
        Skip_XX(ErrorCorrectionTypeLength,                      "Error Correction Data");

    //Filling
    Stream[Stream_Number].StreamKind=StreamKind_Last;
    Stream[Stream_Number].StreamPos=StreamPos_Last;
    Stream[Stream_Number].Info["ID"].From_Number(Stream_Number);
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_Audio ()
{
    Element_Name("Audio");
    
    //Parsing
    int32u SamplingRate, BytesPerSec;
    int16u CodecID, Channels, Data_Size, Resolution;
    Get_L2 (CodecID,                                            "Codec ID");
    Get_L2 (Channels,                                           "Number of Channels");
    Get_L4 (SamplingRate,                                       "Samples Per Second");
    Get_L4 (BytesPerSec,                                        "Average Number of Bytes Per Second");
    Skip_L2(                                                    "Block Alignment");
    Get_L2 (Resolution,                                         "Bits / Sample");
    Get_L2 (Data_Size,                                          "Codec Specific Data Size");

    //Filling
    Stream_Prepare(Stream_Audio);
    Stream[Stream_Number].IsCreated=true;
    Ztring Codec; Codec.From_Number(CodecID, 16);
    Codec.MakeUpperCase();
    CodecID_Fill(Codec, Stream_Audio, StreamPos_Last, InfoCodecID_Format_Riff);
    Fill(Stream_Audio, StreamPos_Last, Audio_Codec, Codec); //May be replaced by codec parser
    Fill(Stream_Audio, StreamPos_Last, Audio_Codec_CC, Codec);
    Fill(Stream_Audio, StreamPos_Last, Audio_Channel_s_, Channels);
    Fill(Stream_Audio, StreamPos_Last, Audio_SamplingRate, SamplingRate);
    Fill(Stream_Audio, StreamPos_Last, Audio_BitRate, BytesPerSec*8);
    Fill(Stream_Audio, StreamPos_Last, Audio_Resolution, Resolution);

    //Parsing
    if (Data_Size>0)
    {
        Element_Begin("Codec Specific Data", Data_Size);
        switch (CodecID)
        {
            case 0x0161 :
            case 0x0162 :
            case 0x0163 : Header_StreamProperties_Audio_WMA(); break;
            case 0x7A21 :
            case 0x7A22 : Header_StreamProperties_Audio_AMR(); break;
            default     : Skip_XX(Data_Size,                    "Unknown");
        }
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_Audio_WMA ()
{
    Element_Info("WMA");

    //Parsing
    Skip_L4(                                                    "SamplesPerBlock");
    Skip_L2(                                                    "EncodeOptions");
    Skip_L4(                                                    "SuperBlockAlign");    
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_Audio_AMR ()
{
    Element_Info("AMR");

    //Parsing
    int32u Flags;
    bool   VBR;
    Get_L4 (Flags,                                              "Flags");
        Skip_Flags(Flags, 0,                                    "SID is used");
        Get_Flags (Flags, 1, VBR,                               "Varying bitrate");

    //Filling
    Fill(Stream_Audio, StreamPos_Last, Audio_BitRate_Mode, VBR?"VBR":"CBR");
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_Video ()
{
    Element_Name("Video");

    //Parsing
    int32u Width, Height, Compression;
    int16u Data_Size, Resolution;
    Get_L4 (Width,                                              "Width");
    Get_L4 (Height,                                             "Height");
    Skip_L1(                                                    "Flags");
    Get_L2 (Data_Size,                                          "Format Data Size");
    Skip_L4(                                                    "Size");
    Get_L4 (Width,                                              "Width");
    Get_L4 (Height,                                             "Height");
    Skip_L2(                                                    "Planes");
    Get_L2 (Resolution,                                         "BitCount");
    Get_C4 (Compression,                                        "Compression");
    Skip_L4(                                                    "SizeImage");
    Skip_L4(                                                    "XPelsPerMeter");
    Skip_L4(                                                    "YPelsPerMeter");
    Skip_L4(                                                    "ClrUsed");
    Skip_L4(                                                    "ClrImportant");

    //Filling
    Stream_Prepare(Stream_Video);
    Stream[Stream_Number].IsCreated=true;
    CodecID_Fill(Ztring().From_CC4(Compression), Stream_Video, StreamPos_Last, InfoCodecID_Format_Riff);
    Fill(Stream_Video, StreamPos_Last, Video_Codec, Ztring().From_CC4(Compression)); //May be replaced by codec parser
    Fill(Stream_Video, StreamPos_Last, Video_Codec_CC, Ztring().From_CC4(Compression));
    Fill(Stream_Video, StreamPos_Last, Video_Width, Width);
    Fill(Stream_Video, StreamPos_Last, Video_Height, Height);
    if (Resolution>0)
        Fill(Stream_Video, StreamPos_Last, Video_Resolution, Resolution);
    if (Compression==CC4("DVR "))
        IsDvrMs=true;

    //Creating the parser
         if (0);
    #if defined(MEDIAINFO_VC1_YES)
    else if (MediaInfoLib::Config.CodecID_Get(Stream_Video, InfoCodecID_Format_Riff, Ztring().From_CC4(Compression), InfoCodecID_Format)==_T("VC-1"))
    {
        Stream[Stream_Number].Parser=new File_Vc1;
        if (Compression==CC4("WMV3"))
        {
            ((File_Vc1*)Stream[Stream_Number].Parser)->From_WMV3=true;
            ((File_Vc1*)Stream[Stream_Number].Parser)->MustSynchronize=false;
        }
        ((File_Vc1*)Stream[Stream_Number].Parser)->FrameIsAlwaysComplete=true; //Warning: this is not always the case, see data parsing
        Open_Buffer_Init(Stream[Stream_Number].Parser);
        if (Data_Size>40)
        {
            Open_Buffer_Continue(Stream[Stream_Number].Parser, Buffer+Buffer_Offset+(size_t)Element_Offset, (size_t)(Data_Size-40));
            if (Stream[Stream_Number].Parser->Status[IsFinished])
            {
                Finish(Stream[Stream_Number].Parser);
                Merge(*Stream[Stream_Number].Parser, Stream_Video, 0, StreamPos_Last);
                delete Stream[Stream_Number].Parser; Stream[Stream_Number].Parser=NULL;
            }
            else
            {
                ((File_Vc1*)Stream[Stream_Number].Parser)->Only_0D=true;
                ((File_Vc1*)Stream[Stream_Number].Parser)->MustSynchronize=false;
            }
            Element_Offset+=Data_Size-40;
        }
    }
    #endif
    #if defined(MEDIAINFO_MPEGV_YES)
    else if (MediaInfoLib::Config.Codec_Get(Ztring().From_CC4(Compression), InfoCodec_KindofCodec).find(_T("MPEG-2"))==0)
    {
        Stream[Stream_Number].Parser=new File_Mpegv;
        ((File_Mpegv*)Stream[Stream_Number].Parser)->Frame_Count_Valid=30; //For searching Pulldown
        Open_Buffer_Init(Stream[Stream_Number].Parser);
    }
    #endif
    else if (Data_Size>40) //TODO: see "The Mummy_e"
        Skip_XX(Data_Size-40,                                   "Codec Specific Data");
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_JFIF ()
{
    Element_Name("JFIF");

    //Parsing
    int32u Width, Height;
    Get_L4 (Width,                                              "Width");
    Get_L4 (Height,                                             "Height");
    Skip_L4(                                                    "Reserved");

    //Filling
    Stream_Prepare(Stream_Image);
    Fill(Stream_Video, StreamPos_Last, Video_Format, "M-JPEG");
    Fill(Stream_Video, StreamPos_Last, Video_Codec, "JPEG");
    Fill(Stream_Video, StreamPos_Last, Video_Width, Width);
    Fill(Stream_Video, StreamPos_Last, Video_Height, Height);
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_DegradableJPEG ()
{
    Element_Name("Degradable JPEG");

    int32u Width, Height;
    int16u InterchangeDataLength;
    Get_L4 (Width,                                              "Width");
    Get_L4 (Height,                                             "Height");
    Skip_L2(                                                    "Reserved");
    Skip_L2(                                                    "Reserved");
    Skip_L2(                                                    "Reserved");
    Get_L2 (InterchangeDataLength,                              "Interchange data length");
    if (InterchangeDataLength>0)
        Skip_XX(InterchangeDataLength,                          "Interchange data");
    else
        Skip_L1(                                                "Zero");

    //Filling
    Stream_Prepare(Stream_Image);
    Fill(Stream_Video, StreamPos_Last, Video_Format, "M-JPEG");
    Fill(Stream_Video, StreamPos_Last, Video_Codec, "JPEG");
    Fill(Stream_Video, StreamPos_Last, Video_Width, Width);
    Fill(Stream_Video, StreamPos_Last, Video_Height, Height);
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamProperties_Binary ()
{
    Element_Name("Binary");

    //Parsing
    int32u FormatDataLength;
    Skip_GUID(                                                  "Major media type");
    Skip_GUID(                                                  "Media subtype");
    Skip_L4(                                                    "Fixed-size samples");
    Skip_L4(                                                    "Temporal compression");
    Skip_L4(                                                    "Sample size");
    Skip_GUID(                                                  "Format type");
    Get_L4 (FormatDataLength,                                   "Format data size");
    if (FormatDataLength>0)
        Skip_XX(FormatDataLength,                               "Format data");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension()
{
    Element_Name("Header Extension");

    //Parsing
    int32u Size;
    Skip_GUID(                                                  "ClockType");
    Skip_L2(                                                    "ClockSize");
    Get_L4 (Size,                                               "Extension Data Size");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_ExtendedStreamProperties()
{
    Element_Name("Extended Stream Properties");

    //Parsing
    int64u AverageTimePerFrame;
    int32u DataBitrate, Flags;
    int16u StreamNumber, LanguageID, StreamNameCount, PayloadExtensionSystemCount;
    Info_L8(StartTime,                                          "Start Time"); Param_Info_From_Milliseconds(StartTime);
    Info_L8(EndTime,                                            "End Time"); Param_Info_From_Milliseconds(EndTime);
    Get_L4 (DataBitrate,                                        "Data Bitrate");
    Skip_L4(                                                    "Buffer Size");
    Skip_L4(                                                    "Initial Buffer Fullness");
    Skip_L4(                                                    "Alternate Data Bitrate");
    Skip_L4(                                                    "Alternate Buffer Size");
    Skip_L4(                                                    "Alternate Initial Buffer Fullness");
    Skip_L4(                                                    "Maximum Object Size");
    Get_L4 (Flags,                                              "Flags");
        Skip_Flags(Flags, 0,                                    "Reliable");
        Skip_Flags(Flags, 1,                                    "Seekable");
        Skip_Flags(Flags, 2,                                    "No Cleanpoints");
        Skip_Flags(Flags, 3,                                    "Resend Live Cleanpoints");
    Get_L2 (StreamNumber,                                       "Stream Number"); Element_Info(StreamNumber);
    Get_L2 (LanguageID,                                         "Stream Language ID Index");
    Get_L8 (AverageTimePerFrame,                                "Average Time Per Frame");
    Get_L2 (StreamNameCount,                                    "Stream Name Count");
    Get_L2 (PayloadExtensionSystemCount,                        "Payload Extension System Count");
    for (int16u Pos=0; Pos<StreamNameCount; Pos++)
    {
        Element_Begin("Stream Name");
        int16u StreamNameLength;
        Skip_L2(                                                "Language ID Index");
        Get_L2 (StreamNameLength,                               "Stream Name Length");
        Skip_UTF16L(StreamNameLength,                           "Stream Name");
        Element_End();
    }
    for (int16u Pos=0; Pos<PayloadExtensionSystemCount; Pos++)
    {
        Element_Begin("Payload Extension System");
        stream::payload_extension_system Payload_Extension_System;
        int32u ExtensionSystemInfoLength;
        Get_GUID(Payload_Extension_System.ID,                   "Extension System ID");
        Get_L2 (Payload_Extension_System.Size,                  "Extension Data Size");
        Get_L4 (ExtensionSystemInfoLength,                      "Extension System Info Length");
        if (ExtensionSystemInfoLength>0)
            Skip_XX(ExtensionSystemInfoLength,                  "Extension System Info");
        Element_End();

        //Filling
        Stream[StreamNumber].Payload_Extension_Systems.push_back(Payload_Extension_System);
    }

    //Header_StreamProperties
    if (Element_Offset<Element_Size)
    {
        //This could be everything, but in theory this is only Header_StreamProperties
        int128u Name;
        int64u Size;
        Element_Begin("Stream Properties Object", Element_Size-Element_Offset);
        Element_Begin("Header", 24);
            Get_GUID(Name,                                      "Name");
            Get_L8 (Size,                                       "Size");
        Element_End();
        if (Size>=24 && Element_Offset+Size-24==Element_Size)
        {
            switch (Name.hi)
            {
                case Elements::Header_StreamProperties :    Header_StreamProperties(); break;
                default :                                   Skip_XX(Size-24, "Unknown");
            }
        }
        else
            Skip_XX(Element_Size-Element_Offset,                "Problem");
        Element_End();
    }

    //Filling
    Stream[StreamNumber].LanguageID=LanguageID;
    Stream[StreamNumber].AverageBitRate=DataBitrate;
    Stream[StreamNumber].AverageTimePerFrame=AverageTimePerFrame;
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_AdvancedMutualExclusion()
{
    Element_Name("Advanced Mutual Exclusion");

    //Parsing
    int16u Count;
    Info_GUID(ExclusionType,                                    "Exclusion Type"); Param_Info(Wm_ExclusionType(ExclusionType));
    Get_L2 (Count,                                              "Stream Numbers Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Info_L2(StreamNumber,                                   "Stream Number"); Element_Info(StreamNumber);
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_GroupMutualExclusion()
{
    Element_Name("Group Mutual Exclusion");

    //Parsing
    Skip_XX(Element_Size,                                       "Unknown");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_StreamPrioritization()
{
    Element_Name("Stream Prioritization");

    //Parsing
    int16u Count;
    Get_L2 (Count,                                              "Stream Numbers Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        int16u Flags;
        Element_Begin("Stream");
        Info_L2(StreamNumber,                                   "Stream Number"); Element_Info(StreamNumber);
        Get_L2 (Flags,                                          "Flags");
            Skip_Flags(Flags, 0,                                "Mandatory");
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_BandwidthSharing()
{
    Element_Name("Bandwidth Sharing");

    //Parsing
    Skip_XX(Element_Size,                                       "Unknown");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_LanguageList()
{
    Element_Name("Language List");

    //Parsing
    Ztring LanguageID;
    int16u Count;
    int8u  LanguageID_Length;
    Get_L2 (Count,                                              "Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Language ID");
        Get_L1 (LanguageID_Length,                              "Language ID Length");
        if (LanguageID_Length>0)
        {
            Get_UTF16L(LanguageID_Length, LanguageID,           "Language ID");
            Element_Info(LanguageID);
        }
        Element_End();

        //Filling
        Languages.push_back(LanguageID);
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_MetadataLibrary()
{
    Element_Name("Metadata Library");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_Metadata()
{
    Element_Name("Metadata");

    //Parsing
    float32 AspectRatioX=0, AspectRatioY=0;
    int16u Count;
    Get_L2 (Count,                                              "Description Records Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Description Record");
        Ztring Name, Data;
        int64u Data_Int64=0;
        int32u Data_Length;
        int16u StreamNumber, Name_Length, Data_Type;
        Skip_L2(                                                "Reserved");
        Get_L2 (StreamNumber,                                   "Stream Number");
        Get_L2 (Name_Length,                                    "Name Length");
        Get_L2 (Data_Type,                                      "Data Type");
        Get_L4 (Data_Length,                                    "Data Length");
        Get_UTF16L(Name_Length, Name,                           "Name Length");
        switch (Data_Type)
        {
            case 0x00 : Get_UTF16L(Data_Length, Data,           "Data"); break;
            case 0x01 : Skip_XX(Data_Length,                    "Data"); Data=_T("(Binary)"); break;
            case 0x02 : {int16u Data_Int; Get_L2 (Data_Int,     "Data"); Data=(Data_Int==0)?_T("No"):_T("Yes"); Data_Int64=Data_Int;} break;
            case 0x03 : {int32u Data_Int; Get_L4 (Data_Int,     "Data"); Data.From_Number(Data_Int); Data_Int64=Data_Int;} break;
            case 0x04 : {int64u Data_Int; Get_L8 (Data_Int,     "Data"); Data.From_Number(Data_Int); Data_Int64=Data_Int;} break;
            case 0x05 : {int16u Data_Int; Get_L2 (Data_Int,     "Data"); Data.From_Number(Data_Int); Data_Int64=Data_Int;} break;
            default   : Skip_XX(Data_Length,                    "Data"); Data=_T("(Unknown)"); break;
        }
        Element_Info(Name);
        Element_Info(Data);
        Element_End();

        if (Name==_T("IsVBR"))
            Stream[StreamNumber].Info["BitRate_Mode"]=(Data_Int64==0)?"CBR":"VBR";
        else if (Name==_T("AspectRatioX"))
        {
            AspectRatioX=Data.To_float32();
            if (AspectRatioX && AspectRatioY)
                Stream[StreamNumber].Info["PixelAspectRatio"].From_Number(AspectRatioX/AspectRatioY, 3);
        }
        else if (Name==_T("AspectRatioY"))
        {
            AspectRatioY=Data.To_float32();
            if (AspectRatioX && AspectRatioY)
                Stream[StreamNumber].Info["PixelAspectRatio"].From_Number(AspectRatioX/AspectRatioY, 3);
        }
        else if (Name==_T("DeviceConformanceTemplate"))
        {
            if (Data!=_T("@"))
                Stream[StreamNumber].Info["Format_Profile"]=Data;
        }
        else if (Name==_T("WM/WMADRCPeakReference")) {}
        else if (Name==_T("WM/WMADRCAverageReference")) {}
        else if (Name==_T("WM/WMADRCAverageTarget")) {}
        else if (Name==_T("WM/WMADRCPeakTarget")) {}
        else
            Stream[StreamNumber].Info[Name.To_Local()]=Data;
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_IndexParameters()
{
    Element_Name("Index Parameters");

    //Parsing
    int16u Count;
    Skip_L4(                                                    "Index Entry Time Interval");
    Get_L2 (Count,                                              "Index Specifiers Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Index Specifier");
        int16u IndexType;
        Skip_L2(                                                "Stream Number");
        Get_L2 (IndexType,                                      "Index Type");
        Element_Info(IndexType);
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_MediaIndexParameters()
{
    Element_Name("MediaIndex Parameters");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_TimecodeIndexParameters()
{
    Element_Name("Timecode Index Parameters");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_Compatibility()
{
    Element_Name("Compatibility");

    //Parsing
    Skip_L1(                                                    "Profile");
    Skip_L1(                                                    "Mode");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_AdvancedContentEncryption()
{
    Element_Name("Advanced Content Encryption");
}

//---------------------------------------------------------------------------
void File_Wm::Header_HeaderExtension_IndexPlaceholder()
{
    Element_Name("Index Placeholder");
}

//---------------------------------------------------------------------------
void File_Wm::Header_CodecList()
{
    Element_Name("Codec List");

    //Parsing
    Ztring CodecName, CodecDescription;
    int32u Count32;
    int16u Count, Type, CodecNameLength, CodecDescriptionLength, CodecInformationLength;
    Skip_GUID(                                                  "Reserved");
    Get_L4 (Count32,                                            "Codec Entries Count");
    Count=(int16u)Count32;
    CodecInfos.resize(Count);
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Codec Entry");
        Get_L2 (Type,                                           "Type"); Param_Info(Wm_CodecList_Kind(Type));
        Get_L2 (CodecNameLength,                                "Codec Name Length");
        Get_UTF16L(CodecNameLength*2, CodecName,                "Codec Name");
        Get_L2 (CodecDescriptionLength,                         "Codec Description Length");
        Get_UTF16L(CodecDescriptionLength*2, CodecDescription,  "Codec Description");
        Get_L2 (CodecInformationLength,                         "Codec Information Length");
        if (Type==2 && CodecInformationLength==2) //Audio and 2CC
            Skip_L2(                                            "2CC"); //Not used, we have it elsewhere
        else if (Type==1 && CodecInformationLength==4) //Video and 4CC
            Skip_C4(                                            "4CC"); //Not used, we have it elsewhere
        else
            Skip_XX(CodecInformationLength,                     "Codec Information");
        Element_End();

        //Filling
        CodecInfos[Pos].Type=Type;
        CodecInfos[Pos].Info=CodecName;
        if (!CodecDescription.empty())
        {
            CodecInfos[Pos].Info+=_T(" - ");
            CodecInfos[Pos].Info+=CodecDescription;
        }

        Codec_Description_Count++;
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_ScriptCommand()
{
    Element_Name("Script Command");

    //Parsing
    Skip_GUID(                                                  "Reserved");
    int16u Commands_Count, CommandTypes_Count;
    Get_L2 (Commands_Count,                                     "Commands Count");
    Get_L2 (CommandTypes_Count,                                 "Command Types Count");
    for (int16u Pos=0; Pos<CommandTypes_Count; Pos++)
    {
        Element_Begin("Command Type");
        int16u Length;
        Get_L2 (Length,                                         "Command Type Length");
        if (Length>0)
            Skip_UTF16L(Length*2,                               "Command Type");
        Element_End();
    }
    for (int16u Pos=0; Pos<Commands_Count; Pos++)
    {
        Element_Begin("Command");
        int16u Length;
        Skip_L2(                                                "Type Index");
        Get_L2 (Length,                                         "Command Length");
        if (Length>0)
            Skip_UTF16L(Length*2,                               "Command");
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_Marker()
{
    Element_Name("Markers");

    //Parsing
    Skip_GUID(                                                  "Reserved");
    int32u Markers_Count;
    int16u Name_Length;
    Get_L4 (Markers_Count,                                      "Markers Count");
    Skip_L2(                                                    "Reserved");
    Get_L2 (Name_Length,                                        "Name Length");
    if (Name_Length>0)
        Skip_UTF16L(Name_Length,                                "Name");
    
    //Filling
    if (Markers_Count>0)
        Stream_Prepare(Stream_Menu);

    //Parsing
    for (int32u Pos=0; Pos<Markers_Count; Pos++)
    {
        Element_Begin("Marker");
        Ztring Marker;
        int32u Marker_Length;
        Skip_L8(                                                "Offset");
        Info_L8(PresentationTime,                               "Presentation Time"); Param_Info_From_Milliseconds(PresentationTime/10000);
        Skip_L2(                                                "Entry Length");
        Info_L4(SendTime,                                       "Send Time"); Param_Info_From_Milliseconds(SendTime);
        Skip_L4(                                                "Flags");
        Get_L4 (Marker_Length,                                  "Marker Description Length");
        if (Marker_Length>0)
            Get_UTF16L(Marker_Length*2, Marker,                 "Marker Description");
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_BitRateMutualExclusion()
{
    Element_Name("BitRate Mutual Exclusion");

    //Parsing
    int16u Count;
    Skip_GUID(                                                  "Exclusion Type");
    Get_L2 (Count,                                              "Stream Numbers Count");
    for (int16u Pos=0; Pos<Count; Pos++)
        Skip_L2(                                                "Stream Number");
}

//---------------------------------------------------------------------------
void File_Wm::Header_ErrorCorrection()
{
    Element_Name("Error Correction");
}

//---------------------------------------------------------------------------
void File_Wm::Header_ContentDescription()
{
    Element_Name("Content Description");

    //Parsing
    Ztring Title, Author, Copyright, Description, Rating;
    int16u TitleLength, AuthorLength, CopyrightLength, DescriptionLength, RatingLength;
    Get_L2 (TitleLength,                                        "TitleLength");
    Get_L2 (AuthorLength,                                       "AuthorLength");
    Get_L2 (CopyrightLength,                                    "CopyrightLength");
    Get_L2 (DescriptionLength,                                  "DescriptionLength");
    Get_L2 (RatingLength,                                       "RatingLength");
    if (TitleLength>0)
        Get_UTF16L(TitleLength, Title,                          "Title");
    if (AuthorLength>0)
        Get_UTF16L(AuthorLength, Author,                        "Author");
    if (CopyrightLength>0)
        Get_UTF16L(CopyrightLength, Copyright,                  "Copyright");
    if (DescriptionLength>0)
        Get_UTF16L(DescriptionLength, Description,              "Description");
    if (RatingLength>0)
        Get_UTF16L(RatingLength, Rating,                        "Rating");

    //Filling
    Fill(Stream_General, 0, General_Title, Title);
    Fill(Stream_General, 0, General_Performer, Author);
    Fill(Stream_General, 0, General_Copyright, Copyright);
    Fill(Stream_General, 0, General_Comment, Description);
    Fill(Stream_General, 0, General_Rating, Rating);
}

//---------------------------------------------------------------------------
void File_Wm::Header_ExtendedContentDescription()
{
    Element_Name("Extended Content Description");

    //Parsing
    int16u Count;
    Get_L2 (Count,                                              "Content Descriptors Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Content Descriptor");
        Ztring Name, Value;
        int64u Value_Int64=0;
        int16u Name_Length, Value_Type, Value_Length;
        Get_L2 (Name_Length,                                    "Name Length");
        Get_UTF16L(Name_Length, Name,                           "Name");
        Get_L2 (Value_Type,                                     "Value Data Type");
        Get_L2 (Value_Length,                                   "Value Length");
        switch (Value_Type)
        {
            case 0x00 : Get_UTF16L(Value_Length, Value,         "Value"); break;
            case 0x01 :
                        if (Name==_T("ASFLeakyBucketPairs")) Header_ExtendedContentDescription_ASFLeakyBucketPairs(Value_Length);
                        else {Skip_XX(Value_Length,             "Value"); Value=_T("(Binary)");} break;
            case 0x02 : {int32u Value_Int; Get_L4 (Value_Int,   "Value"); Value=(Value_Int==0)?_T("No"):_T("Yes"); Value_Int64=Value_Int;} break;
            case 0x03 : {int32u Value_Int; Get_L4 (Value_Int,   "Value"); Value.From_Number(Value_Int); Value_Int64=Value_Int;} break;
            case 0x04 : {int64u Value_Int; Get_L8 (Value_Int,   "Value"); Value.From_Number(Value_Int); Value_Int64=Value_Int;} break;
            case 0x05 : {int16u Value_Int; Get_L2 (Value_Int,   "Value"); Value.From_Number(Value_Int); Value_Int64=Value_Int;} break;
            default   : Skip_XX(Value_Length,                   "Value"); Value=_T("(Unknown)"); break;
        }
        Element_Info(Name);
        Element_Info(Value);
        Element_End();

        //Filling
        if (!Value.empty())
        {
                 if (Name==_T("Agility FPS")) {}
            else if (Name==_T("ASFLeakyBucketPairs")) {} //Already done elsewhere
            else if (Name==_T("Buffer Average")) {}
            else if (Name==_T("DVR Index Granularity")) {}
            else if (Name==_T("DVR File Version")) {}
            else if (Name==_T("IsVBR"))
                Fill(Stream_General, 0, General_OverallBitRate_Mode, Value_Int64==0?"CBR":"VBR");
            else if (Name==_T("VBR Peak")) {} //Already in "Stream Bitrate" chunk
            else if (Name==_T("WMFSDKVersion")) {}
            else if (Name==_T("WMFSDKNeeded")) {}
            else if (Name==_T("WM/AlbumTitle"))
                Fill(Stream_General, 0, General_Album, Value);
            else if (Name==_T("WM/AlbumArtist"))
            {
                Fill(Stream_General, 0, General_Performer, "");
                Fill(Stream_General, 0, General_Performer, Value, true); //Clear last value, like Author (Content Description)
            }
            else if (Name==_T("WM/ArtistSortOrder"))
                Fill(Stream_General, 0, General_Performer_Sort, Value);
            else if (Name==_T("WM/AuthorURL"))
                Fill(Stream_General, 0, "Author/Url", Value);
            else if (Name==_T("WM/BeatsPerMinute"))
                Fill(Stream_General, 0, General_BPM, Value);
            else if (Name==_T("WM/Binary"))
                Fill(Stream_General, 0, General_Cover, "Y");
            else if (Name==_T("WM/Comments"))
                Fill(Stream_General, 0, General_Comment, Value, true); //Clear last value
            else if (Name==_T("WM/Composer"))
                Fill(Stream_General, 0, General_Composer, Value);
            else if (Name==_T("WM/Conductor"))
                Fill(Stream_General, 0, General_Conductor, Value);
            else if (Name==_T("WM/EncodedBy"))
                Fill(Stream_General, 0, General_EncodedBy, Value);
            else if (Name==_T("WM/EncoderSettings"))
                Fill(Stream_General, 0, General_Encoded_Library_Settings, Value);
            else if (Name==_T("WM/EncodingTime"))
                Fill(Stream_General, 0, General_Encoded_Date, Ztring().Date_From_Seconds_1601(Value_Int64));
            else if (Name==_T("WM/Genre"))
                Fill(Stream_General, 0, General_Genre, Value, true); //Clear last value
            else if (Name==_T("WM/GenreID"))
            {
                if (Retrieve(Stream_General, 0, General_Genre).empty())
                    Fill(Stream_General, 0, General_Genre, Value);
            }
            else if (Name==_T("WM/Language"))
                Language_ForAll=Value;
            else if (Name==_T("WM/MediaCredits"))
                Fill(Stream_General, 0, General_ThanksTo, Value);
            else if (Name==_T("WM/MediaPrimaryClassID")) {}
            else if (Name==_T("WM/MCDI")) {}
            else if (Name==_T("WM/ModifiedBy"))
                Fill(Stream_General, 0, General_RemixedBy, Value);
            else if (Name==_T("WM/OriginalAlbumTitle"))
                Fill(Stream_General, 0, "Original/Album", Value);
            else if (Name==_T("WM/OriginalReleaseTime"))
                Fill(Stream_General, 0, "Original/Released_Date", Value);
            else if (Name==_T("WM/ParentalRating"))
                Fill(Stream_General, 0, General_LawRating, Value);
            else if (Name==_T("WM/ParentalRatingReason"))
                Fill(Stream_General, 0, General_LawRating_Reason, Value);
            else if (Name==_T("WM/Picture"))
                Fill(Stream_General, 0, General_Cover, "Y");
            else if (Name==_T("WM/Provider"))
                Fill(Stream_General, 0, "Provider", Value);
            else if (Name==_T("WM/Publisher"))
                Fill(Stream_General, 0, General_Publisher, Value);
            else if (Name==_T("WM/RadioStationName"))
                Fill(Stream_General, 0, General_ServiceName, Value);
            else if (Name==_T("WM/RadioStationOwner"))
                Fill(Stream_General, 0, General_ServiceProvider, Value);
            else if (Name==_T("WM/SubTitle"))
                Fill(Stream_General, 0, General_Title_More, Value);
            else if (Name==_T("WM/SubTitleDescription"))
                Fill(Stream_General, 0, General_Title_More, Value);
            else if (Name==_T("WM/ToolName"))
                Fill(Stream_General, 0, General_Encoded_Application, Value);
            else if (Name==_T("WM/ToolVersion"))
                Fill(Stream_General, 0, General_Encoded_Application, Retrieve(Stream_General, 0, General_Encoded_Application)+_T(" ")+Value, true);
            else if (Name==_T("WM/TrackNumber"))
                Fill(Stream_General, 0, General_Track_Position, Value, true); //Clear last value, like WM/Track
            else if (Name==_T("WM/Track"))
            {
                if (Retrieve(Stream_General, 0, General_Track_Position).empty())
                    Fill(Stream_General, 0, General_Track_Position, Value.To_int32u()+1);
            }
            else if (Name==_T("WM/UniqueFileIdentifier"))
            {
                if (Value.empty() || Value[0]!=_T(';')) //Test if there is only the separator
                {
                    Value.FindAndReplace(_T(";"), MediaInfoLib::Config.TagSeparator_Get());
                    Fill(Stream_General, 0, General_UniqueID, Value);
                }
            }
            else if (Name==_T("WM/Writer"))
                Fill(Stream_General, 0, General_WrittenBy, Value);
            else if (Name==_T("WM/Year"))
                Fill(Stream_General, 0, General_Recorded_Date, Value);
            else
                Fill(Stream_General, 0, Name.To_Local().c_str(), Value);
        }
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_ExtendedContentDescription_ASFLeakyBucketPairs(int16u Value_Length)
{
    Element_Begin("ASFLeakyBucketPairs", Value_Length);
    Skip_L2(                                                    "Reserved");
    for (int16u Pos=2; Pos<Value_Length; Pos+=8)
    {
        Element_Begin("Bucket", 8);
        Skip_L4(                                                "BitRate");
        Skip_L4(                                                "msBufferWindow");
        Element_End();
    }
    Element_End();
}

//---------------------------------------------------------------------------
void File_Wm::Header_StreamBitRate()
{
    Element_Name("Stream Bitrate");

    //Parsing
    int16u Count;
    Get_L2 (Count,                                              "Count");
    for (int16u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Stream", 6);
        int32u AverageBitRate;
        int16u StreamNumber;
        Get_L2 (StreamNumber,                                   "Stream Number"); Element_Info(StreamNumber);
        Get_L4 (AverageBitRate,                                 "Average Bitrate"); Element_Info(AverageBitRate);
        Element_End();

        //Filling
        if (Stream[StreamNumber].AverageBitRate==0) //Prefere Average bitrate of Extended Stream Properties if present
            Stream[StreamNumber].AverageBitRate=AverageBitRate;
    }
}

//---------------------------------------------------------------------------
void File_Wm::Header_ContentBranding()
{
    Element_Name("Content Branding");

    //Parsing
    Ztring CopyrightURL, BannerImageURL;
    int32u BannerImageData_Type, BannerImageData_Length, BannerImageURL_Length, CopyrightURL_Length;
    Get_L4 (BannerImageData_Type,                               "Banner Image Data Type"); Param_Info(Wm_BannerImageData_Type(BannerImageData_Type));
    Get_L4 (BannerImageData_Length,                             "Banner Image Data Length");
    if (BannerImageData_Length>0)
        Skip_XX(BannerImageData_Length,                         "Banner Image Data");
    Get_L4 (BannerImageURL_Length,                              "Banner Image URL Length");
    if (BannerImageURL_Length>0)
    Get_Local(BannerImageURL_Length, BannerImageURL,            "Banner Image URL");
        Get_L4 (CopyrightURL_Length,                            "Copyright URL Length");
    if (CopyrightURL_Length>0)
        Get_Local(CopyrightURL_Length, CopyrightURL,            "Copyright URL");
}

//---------------------------------------------------------------------------
void File_Wm::Header_ContentEncryption()
{
    Element_Name("Content Encryption");

    //Parsing
    Ztring LicenseURL;
    int32u SecretDataLength, ProtectionTypeLength, KeyIDLength, LicenseURLLength;
    Get_L4 (SecretDataLength,                                   "Secret Data Length");
    Skip_XX(SecretDataLength,                                   "Secret Data");
    Get_L4 (ProtectionTypeLength,                               "Protection Type Length");
    Skip_Local(ProtectionTypeLength,                            "Protection Type");
    Get_L4 (KeyIDLength,                                        "Key ID Length");
    Skip_Local(KeyIDLength,                                     "Key ID Type");
    Get_L4 (LicenseURLLength,                                   "License URL Length");
    Get_Local(LicenseURLLength, LicenseURL,                     "License URL");

    //Filling
    Fill(Stream_General, 0, "Encryption", LicenseURL);
}

//---------------------------------------------------------------------------
void File_Wm::Header_ExtendedContentEncryption()
{
    Element_Name("Extended Content Encryption");

    //Parsing
    int32u DataLength;
    Get_L4 (DataLength,                                         "Data Length");
    Skip_XX(DataLength,                                         "Data");
}

//---------------------------------------------------------------------------
void File_Wm::Header_DigitalSignature()
{
    Element_Name("Digital Signature");

    //Parsing
    int32u DataLength;
    Skip_L4(                                                    "Signature Type");
    Get_L4 (DataLength,                                         "Signature Data Length");
    Skip_XX(DataLength,                                         "Signature Data");
}

//---------------------------------------------------------------------------
void File_Wm::Header_Padding()
{
    Element_Name("Padding");

    //Parsing
    Skip_XX(Element_Size,                                       "Padding");
}

//---------------------------------------------------------------------------
void File_Wm::Data()
{
    Element_Name("Data");

    //Parsing
    Skip_GUID(                                                  "File ID");
    Skip_L8(                                                    "Total Data Packets");
    Skip_L1(                                                    "Alignment");
    Skip_L1(                                                    "Packet Alignment");

    //Filling
    Fill(Stream_General, 0, General_HeaderSize, File_Offset+Buffer_Offset-24);
    Fill(Stream_General, 0, General_DataSize, Element_TotalSize_Get()+24);

    //For each stream
    Streams_Count=0;
    std::map<int16u, stream>::iterator Temp=Stream.begin();
    while (Temp!=Stream.end())
    {
        #if defined(MEDIAINFO_MPEGA_YES)
        if (IsDvrMs && !Temp->second.Parser && Temp->second.AverageBitRate>=32768)
        {
            Temp->second.Parser=new File_Mpega; //No stream properties, trying to detect it in datas...
            ((File_Mpega*)Temp->second.Parser)->Frame_Count_Valid=8;
            Open_Buffer_Init(Temp->second.Parser);
        }
        #endif
        if (Temp->second.Parser || Temp->second.StreamKind==Stream_Video) //We need Stream_Video for Frame_Rate computing
        {
            Temp->second.SearchingPayload=true;
            Streams_Count++;
        }
        Temp++;
    }

    //Enabling the alternative parser
    MustUseAlternativeParser=true;
    Data_AfterTheDataChunk=File_Offset+Buffer_Offset+Element_TotalSize_Get();
}

//---------------------------------------------------------------------------
void File_Wm::Data_Packet()
{
    //Counting
    Packet_Count++;
    Element_Info(Packet_Count);
    size_t Element_Show_Count=0;

    //Parsing
    int32u PacketLength=0, SizeOfMediaObject=0;
    int8u  Flags, ErrorCorrectionData_Length, ErrorCorrectionLengthType, SequenceType, PaddingLengthType, PacketLengthType;
    bool   ErrorCorrectionPresent;
    Element_Begin("Error Correction");
        Get_L1 (Flags,                                          "Flags");
            Get_Flags (Flags&0x0F, ErrorCorrectionData_Length,  "Error Correction Data Length"); //4 lowest bits
            Skip_Flags(Flags, 4,                                "Opaque Data Present");
            Get_Flags ((Flags>>5)&0x03, ErrorCorrectionLengthType, "Error Correction Length Type"); //bits 6 and 7
            Get_Flags (Flags, 7, ErrorCorrectionPresent,        "Error Correction Present");
        if (ErrorCorrectionPresent && ErrorCorrectionLengthType==0 && ErrorCorrectionData_Length==2)
        {
            int8u  TypeNumber;
            Get_L1 (TypeNumber,                                 "Type/Number");
                Skip_Flags((TypeNumber>>4)&0x0F, "Type");
                Skip_Flags( TypeNumber    &0x0F, "Number");
            Skip_L1(                                            "Cycle");
        }
    Element_End();

    Element_Begin("Payload Parsing Information");
        Get_L1 (Flags,                                          "Length Type Flags");
            Get_Flags (Flags, 0, MultiplePayloadsPresent,       "Multiple Payloads Present");
            Get_Flags ((Flags>>1)&0x3, SequenceType,            "Sequence Type");
            Get_Flags ((Flags>>3)&0x3, PaddingLengthType,       "Padding Length Type");
            Get_Flags ((Flags>>5)&0x3, PacketLengthType,        "Packet Length Type");
            Skip_Flags(Flags, 7,                                "Error Correction Present");
        Get_L1 (Flags,                                          "Property Flags");
            Get_Flags ( Flags    &0x3, ReplicatedDataLengthType, "Replicated Data Length Type");
            Get_Flags ((Flags>>2)&0x3, OffsetIntoMediaObjectLengthType, "Offset Into Media Object Length Type");
            Get_Flags ((Flags>>4)&0x3, MediaObjectNumberLengthType, "Media Object Number Length Type");
            Get_Flags ((Flags>>6)&0x3, StreamNumberLengthType,  "Stream Number Length Type");
        switch (PacketLengthType)
        {
            case 1 : {int8u  Data; Get_L1(Data,                 "Packet Length"); PacketLength=Data;} break;
            case 2 : {int16u Data; Get_L2(Data,                 "Packet Length"); PacketLength=Data;} break;
            case 3 :               Get_L4(PacketLength,         "Packet Length");                     break;
            default: ;
        }
        switch (SequenceType)
        {
            case 1 : Skip_L1(                                   "Sequence"); break;
            case 2 : Skip_L2(                                   "Sequence"); break;
            case 3 : Skip_L4(                                   "Sequence"); break;
            default: ;
        }
        switch (PaddingLengthType)
        {
            case 1 : {int8u  Data; Get_L1(Data,                 "Padding Length"); Data_Parse_Padding=Data;} break;
            case 2 : {int16u Data; Get_L2(Data,                 "Padding Length"); Data_Parse_Padding=Data;} break;
            case 3 :               Get_L4(Data_Parse_Padding,   "Padding Length");                           break;
            default: Data_Parse_Padding=0;
        }
        Skip_L4(                                                "Send Time");
        Skip_L2(                                                "Duration");
    Element_End();

    if (MultiplePayloadsPresent)
    {
        //Parsing
        Element_Begin("Multiple Payloads additional flags");
            int8u AdditionalFlags;
            Get_L1 (AdditionalFlags,                                     "Flags");
                Get_Flags ( AdditionalFlags    &0x3F, NumberPayloads,    "Number of Payloads"); //6 bits
                Get_Flags ((AdditionalFlags>>6)&0x03, PayloadLengthType, "Payload Length Type"); //bits 6 and 7
        Element_End();
    }
    else
    {
        SizeOfMediaObject=(int32u)(Element_Size-Element_Offset-Data_Parse_Padding);
        NumberPayloads=1;
    }

    for (NumberPayloads_Pos=0; NumberPayloads_Pos<NumberPayloads; NumberPayloads_Pos++)
    {
        Element_Begin("Payload");
        int32u ReplicatedDataLength=0, PayloadLength=0;
        int8u  StreamNumber;
        Get_L1 (StreamNumber,                                   "Stream Number");
        Stream_Number=StreamNumber&0x7F; //For KeyFrame
        Element_Info(Stream_Number);
        switch (MediaObjectNumberLengthType)
        {
            case 1 : Skip_L1(                                   "Media Object Number"); break;
            case 2 : Skip_L2(                                   "Media Object Number"); break;
            case 3 : Skip_L4(                                   "Media Object Number"); break;
            default: Trusted_IsNot("Media Object Number"); return; //Problem
        }
        switch (OffsetIntoMediaObjectLengthType)
        {
            case 1 : Skip_L1(                                   "Offset Into Media Object"); break;
            case 2 : Skip_L2(                                   "Offset Into Media Object"); break;
            case 3 : Skip_L4(                                   "Offset Into Media Object"); break;
            default: Trusted_IsNot("Offset Into Media Object"); return; //Problem
        }
        switch (ReplicatedDataLengthType)
        {
            case 1 : {int8u  Data; Get_L1(Data,                 "Replicated Data Length"); ReplicatedDataLength=Data;} break;
            case 2 : {int16u Data; Get_L2(Data,                 "Replicated Data Length"); ReplicatedDataLength=Data;} break;
            case 3 :               Get_L4(ReplicatedDataLength, "Replicated Data Length");                             break;
            default: Trusted_IsNot("Replicated Data Length"); return; //Problem
        }
        if (ReplicatedDataLengthType!=0 && ReplicatedDataLength>0)
        {
            if (ReplicatedDataLength>=8)
            {
                int32u PresentationTime;
                Get_L4 (SizeOfMediaObject,                      "Size Of Media Object");
                Get_L4 (PresentationTime,                       "Presentation Time");
                if (ReplicatedDataLength>8)
                    Data_Packet_ReplicatedData(ReplicatedDataLength-8);

                //Presentation time delta
                std::map<int16u, stream>::iterator Strea=Stream.find(Stream_Number);
                if (Strea!=Stream.end() && Strea->second.StreamKind==Stream_Video)
                {
                    if (Strea->second.PresentationTime_Old==0)
                        Strea->second.PresentationTime_Old=FileProperties_Preroll;
                    if (PresentationTime!=Strea->second.PresentationTime_Old)
                    {
                        Strea->second.PresentationTime_Deltas[PresentationTime-Strea->second.PresentationTime_Old]++;
                        Strea->second.PresentationTime_Old=PresentationTime;
                        Strea->second.PresentationTime_Count++;
                    }
                }
            }
            else if (ReplicatedDataLength==1)
            {
                Skip_L1(                                        "Presentation Time Delta");
                //TODO
            }
            else
            {
                Skip_XX(ReplicatedDataLength,                   "Replicated Data");
            }
        }

        if (MultiplePayloadsPresent)
        {
            switch (PayloadLengthType)
            {
                case 1 : {int8u  Data; Get_L1(Data,             "Payload Length"); PayloadLength=Data;} break;
                case 2 : {int16u Data; Get_L2(Data,             "Payload Length"); PayloadLength=Data;} break;
                case 3 :               Get_L4(PayloadLength,    "Payload Length");                      break;
                default: Trusted_IsNot("Payload Length"); return; //Problem
            }
        }
        else if (Element_Size-Element_Offset>Data_Parse_Padding)
            PayloadLength=(int32u)(Element_Size-(Element_Offset+Data_Parse_Padding));
        else
        {
            Trusted_IsNot("Padding size problem");
            return; //Problem
        }
        if (Element_Offset+PayloadLength+Data_Parse_Padding>Element_Size)
        {
            Trusted_IsNot("Payload Length problem");
            return; //problem
        }

        //Analyzing
        if (Stream[Stream_Number].Parser && Stream[Stream_Number].SearchingPayload)
        {
            //Handling of spanned on multiple chunks
            bool FrameIsAlwaysComplete=true;
            if (PayloadLength!=SizeOfMediaObject)
            {
                if (SizeOfMediaObject_BytesAlreadyParsed==0)
                    SizeOfMediaObject_BytesAlreadyParsed=SizeOfMediaObject-PayloadLength;
                else
                    SizeOfMediaObject_BytesAlreadyParsed-=PayloadLength;
                if (SizeOfMediaObject_BytesAlreadyParsed!=0)
                    FrameIsAlwaysComplete=false;
                else
                    Element_Show_Count++;
            }
            else
                Element_Show_Count++;

            //Codec specific
            #if defined(MEDIAINFO_VC1_YES)
            if (Retrieve(Stream[Stream_Number].StreamKind, Stream[Stream_Number].StreamPos, Fill_Parameter(Stream[Stream_Number].StreamKind, Generic_Format))==_T("VC-1"))
                ((File_Vc1*)Stream[Stream_Number].Parser)->FrameIsAlwaysComplete=FrameIsAlwaysComplete;
            #endif

            Open_Buffer_Continue(Stream[Stream_Number].Parser, Buffer+Buffer_Offset+(size_t)Element_Offset, (size_t)PayloadLength);
            if (Stream[Stream_Number].Parser->Status[IsFinished]
             || Stream[Stream_Number].PresentationTime_Count>=300)
            {
                Stream[Stream_Number].Parser->Open_Buffer_Unsynch();
                Stream[Stream_Number].SearchingPayload=false;
                Streams_Count--;
            }

            Element_Offset+=PayloadLength;
            Element_Show();
        }
        else
        {
            Skip_XX(PayloadLength,                              "Data");
            if (Stream[Stream_Number].SearchingPayload
             && (Stream[Stream_Number].StreamKind==Stream_Video && Stream[Stream_Number].PresentationTime_Count>=300))
            {
                Stream[Stream_Number].SearchingPayload=false;
                Streams_Count--;
            }
        }
        Element_End();
    }

    if (Data_Parse_Padding)
        Skip_XX(Data_Parse_Padding,                             "Padding");

    //Jumping if needed
    if (Streams_Count==0 || Packet_Count>=1000)
    {
        Info("Data, Jumping to end of chunk");
        GoTo(Data_AfterTheDataChunk, "Windows Media");
    }

    if (Element_Show_Count>0)
        Element_Show();
}

//---------------------------------------------------------------------------
void File_Wm::Data_Packet_ReplicatedData(int32u Size)
{
    Element_Begin("Replicated Data", Size);
    int64u Element_Offset_Final=Element_Offset+Size;
    for (size_t Pos=0; Pos<Stream[Stream_Number].Payload_Extension_Systems.size(); Pos++)
    {
        Element_Begin();
        switch (Stream[Stream_Number].Payload_Extension_Systems[Pos].ID.hi)
        {
            case Elements::Payload_Extension_System_TimeStamp :     Data_Packet_ReplicatedData_TimeStamp(); break;
            default :                                               //Not enough info to validate this algorithm
                                                                    //if (Stream[Stream_Number].Payload_Extension_Systems[Pos].Size!=(int16u)-1)
                                                                    //{
                                                                    //    Element_Name("Unknown");
                                                                    //    Skip_XX(Stream[Stream_Number].Payload_Extension_Systems[Pos].Size, "Unknown");
                                                                    //}
                                                                    //else
                                                                        Pos=Stream[Stream_Number].Payload_Extension_Systems.size(); //Disabling the rest, all is unknown
        }
        Element_End();
    }

    if (Element_Offset<Element_Offset_Final)
    {
        Element_Begin("Other chunks");
        Skip_XX(Element_Offset_Final-Element_Offset, "Unknown");
        Element_End();
    }
    Element_End();
}

//---------------------------------------------------------------------------
void File_Wm::Data_Packet_ReplicatedData_TimeStamp()
{
    Element_Name("TimeStamp");

    //Parsing
    Skip_L2(                                                    "Unknown");
    Skip_L4(                                                    "Unknown");
    Skip_L4(                                                    "Unknown");
    Info_L8(TS0,                                                "TS0"); if (TS0!=(int64u)-1) Param_Info(TS0/10000);
    Info_L8(TS1,                                                "TS1"); if (TS1!=(int64u)-1) Param_Info(TS1/10000);
    Skip_L4(                                                    "Unknown");
    Skip_L4(                                                    "Unknown");
    Skip_L4(                                                    "Unknown");
    Skip_L4(                                                    "Unknown");

    if (Stream[Stream_Number].TimeCode_First==(int64u)-1 && TS0!=(int64u)-1)
        Stream[Stream_Number].TimeCode_First=TS0/10000;
}

//---------------------------------------------------------------------------
void File_Wm::SimpleIndex()
{
    Element_Name("Simple Index");

    //Parsing
    /*
    int32u Count;
    Skip_GUID(                                                  "File ID");
    Skip_L8(                                                    "Index Entry Time Interval");
    Skip_L4(                                                    "Maximum Packet Count");
    Get_L4 (Count,                                              "Index Entries Count");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Index Entry", 6);
        int32u PacketNumber;
        int16u PacketCount;
        Get_L4 (PacketNumber,                                   "Packet Number");
        Get_L2 (PacketCount,                                    "Packet Count");
        Element_End();
    }
    */
    Skip_XX(Element_TotalSize_Get()-Element_Offset,             "Indexes");
}

//---------------------------------------------------------------------------
void File_Wm::Index()
{
    Element_Name("Index");

    //Parsing
    /*
    int32u Blocks_Count;
    int16u Specifiers_Count;
    Skip_L4(                                                    "Index Entry Time Interval");
    Get_L2 (Specifiers_Count,                                   "Index Specifiers Count");
    Get_L4 (Blocks_Count,                                       "Index Blocks Count");
    for (int16u Pos=0; Pos<Specifiers_Count; Pos++)
    {
        Element_Begin("Specifier");
            Skip_L2(                                            "Stream Number");
            Skip_L2(                                            "Index Type");
        Element_End();
    }
    for (int32u Pos=0; Pos<Blocks_Count; Pos++)
    {
        Element_Begin("Block");
        int32u Entry_Count;
        Get_L4 (Entry_Count,                                    "Index Entry Count");
        Element_Begin("Block Positions");
            for (int16u Pos=0; Pos<Specifiers_Count; Pos++)
                Skip_L4(                                        "Position");
        Element_End();
        for (int32u Pos=0; Pos<Entry_Count; Pos++)
        {
            Element_Begin("Entry");
            for (int16u Pos=0; Pos<Specifiers_Count; Pos++)
                Skip_L4(                                        "Offset");
            Element_End();
        }
        Element_End();
    }
    */
    Skip_XX(Element_TotalSize_Get()-Element_Offset,             "Indexes");
}

//---------------------------------------------------------------------------
void File_Wm::MediaIndex()
{
    Element_Name("MediaIndex");
}

//---------------------------------------------------------------------------
void File_Wm::TimecodeIndex()
{
    Element_Name("TimecodeIndex");
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_WM_YES
