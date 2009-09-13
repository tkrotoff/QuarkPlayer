// File_Ogg_SubElement - Info for OGG files
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
#if defined(MEDIAINFO_OGG_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Ogg_SubElement.h"
#include "MediaInfo/Tag/File_VorbisCom.h"
#include "ZenLib/ZtringListList.h"
#include "ZenLib/BitStream.h"
#include "ZenLib/Utils.h"
#include <cmath>
#include <memory>
#if defined(MEDIAINFO_DIRAC_YES)
    #include "MediaInfo/Video/File_Dirac.h"
#endif
#if defined(MEDIAINFO_FLAC_YES)
    #include "MediaInfo/Audio/File_Flac.h"
#endif
#if defined(MEDIAINFO_SPEEX_YES)
    #include "MediaInfo/Audio/File_Speex.h"
#endif
#if defined(MEDIAINFO_THEORA_YES)
    #include "MediaInfo/Video/File_Theora.h"
#endif
#if defined(MEDIAINFO_VORBIS_YES)
    #include "MediaInfo/Audio/File_Vorbis.h"
#endif
#if defined(MEDIAINFO_CMML_YES)
    #include "MediaInfo/Text/File_Cmml.h"
#endif
#if defined(MEDIAINFO_KATE_YES)
    #include "MediaInfo/Text/File_Kate.h"
#endif
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

#ifdef __BORLANDC__ //Borland converts int64u to int32u without error or warning
    #define OGG_ID(NAME, PART1, PART2, COUNT) \
        const int32u Identifier_##NAME##1=0x##PART1; \
        const int32u Identifier_##NAME##2=0x##PART2; \
        const size_t Identifier_##NAME##3=0x##COUNT; \

#elif defined(WINDOWS) //__BORLANDC__
    #define OGG_ID(NAME, PART1, PART2, COUNT) \
        const int64u Identifier_##NAME=(int64u)0x##PART1##PART2##UL; \
        const size_t Identifier_##NAME##3=0x##COUNT; \

#else //__BORLANDC__
    #define OGG_ID(NAME, PART1, PART2, COUNT) \
        const int64u Identifier_##NAME=(int64u)0x##PART1##PART2##LL; \
        const size_t Identifier_##NAME##3=0x##COUNT; \

#endif //__BORLANDC__

namespace Elements
{
    const int32u fLaC=0x664C6143;

    //http://wiki.xiph.org/index.php/MIMETypesCodecs
    OGG_ID(CELT,     43454C54, 20202020, 8)
    OGG_ID(CMML,     434D4D4C, 00000000, 8)
    OGG_ID(BBCD,           42, 42434400, 5)
    OGG_ID(FLAC,           7F, 464C4143, 5)
    OGG_ID(JNG,      8B4A4E47, 0D0A1A0A, 8)
    OGG_ID(kate,     806B6174, 65000000, 8)
    OGG_ID(KW_DIRAC, 4B572D44, 49524143, 8)
    OGG_ID(OggMIDI,  4D67674D, 49444900, 8)
    OGG_ID(MNG,      8A4D4E47, 0D0A1A0A, 8)
    OGG_ID(PCM,      50434D20, 20202020, 8)
    OGG_ID(PNG,      89504E47, 0D0A1A0A, 8)
    OGG_ID(Speex,    53706565, 78202020, 8)
    OGG_ID(theora,     807468, 656F7261, 7)
    OGG_ID(vorbis,     01766F, 72626973, 7)
    OGG_ID(YUV4MPEG, 59555634, 4D504547, 8)

    //Not Xiph registered, but found
    OGG_ID(video,     017669, 64656F00, 7)
    OGG_ID(audio,     016175, 64696F00, 7)
    OGG_ID(text,      017465, 78740000, 7)
    OGG_ID(fLaC,           0, 664C6143, 4)

    //Ogg Skeleton
    OGG_ID(fishead, 66697368, 65616400, 8)
    OGG_ID(fisbone, 66697362, 6F6E6500, 8)
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Ogg_SubElement::File_Ogg_SubElement()
:File__Analyze()
{
    //In
    StreamKind=Stream_Max;
    MultipleStreams=false;
    InAnotherContainer=false;
    absolute_granule_position_Resolution=0;

    //Temp
    Parser=NULL;
    OldSize=0;
    Identified=false;
    WithType=true;
}

//---------------------------------------------------------------------------
File_Ogg_SubElement::~File_Ogg_SubElement()
{
    delete Parser; //Parser=NULL;
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Streams_Fill()
{
    if (Parser==NULL)
        return;

    Fill(Parser);
    Merge(*Parser, Stream_Video,    0, 0);
    Merge(*Parser, Stream_Audio,    0, 0);
    Merge(*Parser, Stream_Text,     0, 0);
    Merge(*Parser, Stream_Image,    0, 0);
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Streams_Finish()
{
    if (Parser==NULL)
        return;

    Finish(Parser);
    Merge(*Parser, Stream_Video,    0, 0);
    Merge(*Parser, Stream_Audio,    0, 0);
    Merge(*Parser, Stream_Text,     0, 0);
    Merge(*Parser, Stream_Image,    0, 0);
}

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Ogg_SubElement::FileHeader_Parse()
{
    Stream_Prepare(Stream_General);
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Ogg_SubElement::Header_Begin()
{
    //Already parsed (there is only one pass)
    if (Buffer_Offset!=0)
        return false;

    //We are waiting for the end of the stream, signaled by a empty buffer adding
    if (Buffer_Size!=OldSize)
    {
        OldSize=Buffer_Size;
        return false;
    }
    else
    {
        OldSize=0;
        return true;
    }
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Header_Parse()
{
    //Parsing
    int64u SamplesCount;
    int32u SamplesCount4;
    int16u SamplesCount2;
    int8u  SamplesCount1, Type;
    bool lenbytes0, lenbytes1, lenbytes2;
    if (Identified && WithType)
    {
        Get_L1 (Type,                                               "Type");
            Skip_Flags(Type, 0,                                     "Indicates data packet");
            Get_Flags (Type, 1, lenbytes2,                          "Bit 2 of lenbytes");
            Skip_Flags(Type, 2,                                     "unused");
            Skip_Flags(Type, 3,                                     "Keyframe");
            Skip_Flags(Type, 4,                                     "unused");
            Skip_Flags(Type, 5,                                     "unused");
            Get_Flags (Type, 6, lenbytes0,                          "Bit 0 of lenbytes");
            Get_Flags (Type, 7, lenbytes1,                          "Bit 1 of lenbytes");
        if ((Type&0x01)==0 && Type!=0x80) //TODO : find a better algo
        {
            if (lenbytes2)
            {
                if (lenbytes1)
                {
                    if (lenbytes0)
                        //Get_L7 (SamplesCount,                   "SamplesCount");
                        Element_Offset+=7;
                    else
                        //Get_L6 (SamplesCount,                   "SamplesCount");
                        Element_Offset+=6;
                }
                else
                {
                    if (lenbytes0)
                        //Get_L5 (SamplesCount,                   "SamplesCount");
                        Element_Offset+=5;
                    else
                    {
                        Get_L4 (SamplesCount4,                  "SamplesCount");
                        SamplesCount=SamplesCount4;
                    }
                }
            }
            else
            {
                if (lenbytes1)
                {
                    if (lenbytes0)
                    {
                        Get_L3 (SamplesCount4,                  "SamplesCount");
                        SamplesCount=SamplesCount4;
                    }
                    else
                    {
                        Get_L2 (SamplesCount2,                  "SamplesCount");
                        SamplesCount=SamplesCount2;
                    }
                }
                else
                {
                    if (lenbytes0)
                    {
                        Get_L1 (SamplesCount1,                  "SamplesCount");
                        SamplesCount=SamplesCount1;
                    }
                    //else
                    //    Get_L0 (SamplesCount,                   "SamplesCount");
                }
            }
        }
        //Filling
        Header_Fill_Code(Type, Ztring::ToZtring(Type, 16));
    }
    else
        //Filling
        Header_Fill_Code(0, "Identification");

    //Filling
    Header_Fill_Size(Element_Size);
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Data_Parse()
{
    //Parsing
    if (!Identified)
        Identification();
    else if (!WithType)
        Default();
    else
        switch (Element_Code)
        {
            case 0x03 :
            case 0x81 : Comment(); break;
            case 0x05 :
            case 0x82 : Default(); break;
            default   : Skip_XX(Element_Size,                       "Unknown");
                        Finish("OggSubElement");
        }
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification()
{
    Element_Name("Identification");

    //Parsing
    int64u ID_Identification;
    if (Element_Size==4)
    {
        int32u ID_Identification_32;
        Peek_B4(ID_Identification_32);
        ID_Identification=((int64u)ID_Identification_32)<<32;
    }
    else
        Peek_B8(ID_Identification);

    //Filling
    #undef ELEMENT_CASE
    #ifdef __BORLANDC__ //Borland converts int64u to int32u
        #define ELEMENT_CASE(_NAME) \
            else if (ID_Identification>>(64-8*Elements::Identifier_##_NAME##3)==(((int64u)Elements::Identifier_##_NAME##1)*0x100000000LL+Elements::Identifier_##_NAME##2)) Identification_##_NAME();

    #else //__BORLANDC__
        #define ELEMENT_CASE(_NAME) \
            else if (ID_Identification>>(64-8*Elements::Identifier_##_NAME##3)==Elements::Identifier_##_NAME) Identification_##_NAME();

    #endif //__BORLANDC__

    if (0) ;
    ELEMENT_CASE(CELT)
    ELEMENT_CASE(CMML)
    ELEMENT_CASE(BBCD)
    ELEMENT_CASE(FLAC)
    ELEMENT_CASE(JNG)
    ELEMENT_CASE(kate)
    ELEMENT_CASE(KW_DIRAC)
    ELEMENT_CASE(OggMIDI)
    ELEMENT_CASE(MNG)
    ELEMENT_CASE(PCM)
    ELEMENT_CASE(PNG)
    ELEMENT_CASE(Speex)
    ELEMENT_CASE(theora)
    ELEMENT_CASE(vorbis)
    ELEMENT_CASE(YUV4MPEG)
    ELEMENT_CASE(video)
    ELEMENT_CASE(audio)
    ELEMENT_CASE(text)
    ELEMENT_CASE(fLaC)
    ELEMENT_CASE(fishead)
    ELEMENT_CASE(fisbone)
    else
    {
        Skip_XX(Element_Size,                                   "Unkown");
        Accept("OggSubElement");
        Finish("OggSubElement");
        return;
    }
    Open_Buffer_Init(Parser);

    //Parsing
    Default();

    //Filling
    StreamKind=StreamKind_Last;
    if (0) ;
    ELEMENT_CASE(fishead)
    ELEMENT_CASE(fisbone)
    else
        Identified=true;
    Accept("OggSubElement");
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_CELT()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Celt;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "celt");
        Fill(Stream_Audio, 0, Audio_Codec, "celt");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_CMML()
{
    #if defined(MEDIAINFO_CMML_YES)
        StreamKind_Last=Stream_Text;
        Parser=new File_Cmml;
    #else
        Stream_Prepare(Stream_Text);
        Fill(Stream_Text, 0, Text_Format, "CMML");
        Fill(Stream_Text, 0, Text_Codec, "CMML");
    #endif
    WithType=false;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_BBCD()
{
    #if defined(MEDIAINFO_DIRAC_YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Dirac;
        ((File_Dirac*)Parser)->Ignore_End_of_Sequence=true;
    #else
        Stream_Prepare(Stream_Video);
        Fill(Stream_Video, 0, Video_Format, "Dirac");
        Fill(Stream_Video, 0, Video_Codec, "Dirac");
    #endif
    WithType=false;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_FLAC()
{
    #if defined(MEDIAINFO_FLAC_YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Flac;
        ((File_Flac*)Parser)->VorbisHeader=true;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "FLAC");
        Fill(Stream_Audio, 0, Audio_Codec, "FLAC");
    #endif
    WithType=false;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_JNG()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Jng;
    #else
        Stream_Prepare(Stream_Video);
        Fill(Stream_Video, 0, Video_Format, "JNG");
        Fill(Stream_Video, 0, Video_Codec, "JNG");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_kate()
{
    #if defined(MEDIAINFO_KATE_YES)
        StreamKind_Last=Stream_Text;
        Parser=new File_Kate;
    #else
        Stream_Prepare(Stream_Text);
        Fill(Stream_Text, 0, Text_Format, "Kate");
        Fill(Stream_Text, 0, Text_Codec, "Kate");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_KW_DIRAC()
{
    Identification_BBCD();
    Fill(Stream_Video, 0, Video_CodecID, "KW-DIRAC", Unlimited, true, true);
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_OggMIDI()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Midi;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "Midi");
        Fill(Stream_Audio, 0, Audio_Codec, "Midi");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_MNG()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Mng;
    #else
        Stream_Prepare(Stream_Video);
        Fill(Stream_Video, 0, Video_Format, "MNG");
        Fill(Stream_Video, 0, Video_Codec, "MNG");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_PCM()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Pcm;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "PCM");
        Fill(Stream_Audio, 0, Audio_Codec, "PCM");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_PNG()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Png;
    #else
        Stream_Prepare(Stream_Video);
        Fill(Stream_Video, 0, Video_Format, "PNG");
        Fill(Stream_Video, 0, Video_Codec, "PNG");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_Speex()
{
    #if defined(MEDIAINFO_SPEEX_YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Speex;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "Speex");
        Fill(Stream_Audio, 0, Audio_Codec, "Speex");
    #endif
    WithType=false;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_theora()
{
    #if defined(MEDIAINFO_THEORA_YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Theora;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Video, 0, Text_Format, "Theora");
        Fill(Stream_Video, 0, Text_Codec, "Theora");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_vorbis()
{
    #if defined(MEDIAINFO_VORBIS_YES)
        StreamKind_Last=Stream_Audio;
        Parser=new File_Vorbis;
    #else
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Text_Format, "Vorbis");
        Fill(Stream_Audio, 0, Text_Codec, "Vorbis");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_YUV4MPEG()
{
    #if defined(MEDIAINFO__YES)
        StreamKind_Last=Stream_Video;
        Parser=new File_Yuv;
    #else
        Stream_Prepare(Stream_Video);
        Fill(Stream_Video, 0, Video_Format, "YUV");
        Fill(Stream_Video, 0, Video_Codec, "YUV");
    #endif
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_video()
{
    Element_Info("Video");

    //Parsing
    int64u TimeUnit;
    int32u fccHandler, Width, Height;
    Skip_B1   (                                                 "Signature");
    Skip_Local(6,                                               "Signature");
    Skip_L2(                                                    "Reserved");
    Get_C4 (fccHandler,                                         "fccHandler");
    Skip_L4(                                                    "SizeOfStructure");
    Get_L8 (TimeUnit,                                           "TimeUnit"); //10000000/TimeUnit is stream tick rate in ticks/sec
    Skip_L4(                                                    "SamplesPerUnit");
    Skip_L8(                                                    "DefaultLengh"); //in media time
    Skip_L4(                                                    "BufferSize");
    Skip_L2(                                                    "BitsPerSample");
    Skip_L2(                                                    "Reserved");
    Get_L4 (Width,                                              "Width");
    Get_L4 (Height,                                             "Height");
    if (Element_Offset<Element_Size)
        Skip_XX(Element_Size-Element_Offset,                    "Unknown");

    //Filling
    Stream_Prepare(Stream_Video);
    CodecID_Fill(Ztring().From_CC4(fccHandler), Stream_Video, StreamPos_Last, InfoCodecID_Format_Riff);
    Fill(Stream_Video, StreamPos_Last, Video_Codec, Ztring().From_CC4(fccHandler));
    Fill(Stream_Video, StreamPos_Last, Video_FrameRate, (float)10000000/(float)TimeUnit, 3);
    Fill(Stream_Video, StreamPos_Last, Video_Width, Width);
    Fill(Stream_Video, StreamPos_Last, Video_Height, Height);
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_audio()
{
    Element_Info("Audio");

    //Parsing
    int64u TimeUnit, SamplesPerUnit;
    int32u fccHandler, AvgBytesPerSec;
    int16u Channels;
    Skip_B1   (                                                 "Signature");
    Skip_Local(6,                                               "Signature");
    Skip_L2(                                                    "Reserved");
    Get_C4 (fccHandler,                                         "fccHandler");
    Skip_L4(                                                    "SizeOfStructure");
    Get_L8 (TimeUnit,                                           "TimeUnit"); //10000000/TimeUnit is stream tick rate in ticks/sec
    Get_L8 (SamplesPerUnit,                                     "SamplesPerUnit");
    Skip_L4(                                                    "DefaultLengh"); //in media time
    Skip_L4(                                                    "BufferSize");
    Skip_L2(                                                    "BitsPerSample");
    Skip_L2(                                                    "Reserved");
    Get_L2 (Channels,                                           "Channels");
    Skip_L2(                                                    "BlockAlign");
    Get_L4 (AvgBytesPerSec,                                     "AvgBytesPerSec");
    if (Element_Offset<Element_Size)
        Skip_XX(Element_Size-Element_Offset,                    "Unknown");

    //Filling
    Stream_Prepare(Stream_Audio);
    Ztring Codec; Codec.From_CC4(fccHandler);
    Codec.TrimLeft(_T('0'));
    CodecID_Fill(Codec, Stream_Audio, StreamPos_Last, InfoCodecID_Format_Riff);
    Fill(Stream_Audio, StreamPos_Last, Audio_Codec, Codec);
    if (AvgBytesPerSec<0x80000000) //This is a signed value, and negative values are not OK
        Fill(Stream_Audio, StreamPos_Last, Audio_BitRate, AvgBytesPerSec*8);
    Fill(Stream_Audio, StreamPos_Last, Audio_Channel_s_, Channels==5?6:Channels); //5 channels are 5.1
    Fill(Stream_Audio, StreamPos_Last, Audio_SamplingRate, SamplesPerUnit);
    absolute_granule_position_Resolution=SamplesPerUnit;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_text()
{
    Element_Info("Text");

    //Parsing
    Skip_B1   (                                                 "Signature");
    Skip_Local(6,                                               "Signature");
    Skip_L2(                                                    "Reserved");
    if (Element_Offset<Element_Size)
        Skip_XX(Element_Size-Element_Offset,                    "Unknown");

    //Filling
    Stream_Prepare(Stream_Text);
    Fill(Stream_Text, StreamPos_Last, Text_Format, "Subrip");
    Fill(Stream_Text, StreamPos_Last, Text_Codec, "Subrip");
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_fLaC()
{
    #if defined(MEDIAINFO_FLAC_YES)
        Parser=new File_Flac;
        StreamKind_Last=Stream_Audio;
    #endif
    Stream_Prepare(Stream_Audio);
    Fill(Stream_Audio, 0, Audio_Format, "FLAC");
    Fill(Stream_Audio, 0, Audio_Codec, "FLAC");
    Fill(Stream_Audio, 0, Audio_MuxingMode, "pre-FLAC 1.1.1");
    WithType=false;
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_fishead()
{
    //Quick workaroud for Identification bool
    if (Element_Offset==Element_Size)
        return;

    Element_Info("Skeleton");

    //Parsing
    int16u VersionMajor;
    Skip_Local(7,                                               "Signature");
    Skip_B1   (                                                 "Signature");
    Get_L2 (VersionMajor,                                       "Version major");
    if (VersionMajor==3)
    {
        Skip_L2(                                                "Version minor");
        Skip_L8(                                                "Presentationtime numerator");
        Skip_L8(                                                "Presentationtime denominator");
        Skip_L8(                                                "Basetime numerator");
        Skip_L8(                                                "Basetime denominator");
        Skip_L16(                                               "UTC");
        Skip_L4(                                                "UTC");
    }
    if (Element_Offset<Element_Size)
        Skip_XX(Element_Size-Element_Offset,                    "Unknown");
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Identification_fisbone()
{
    //Quick workaroud for Identification bool
    if (Element_Offset==Element_Size)
        return;

    Element_Info("Skeleton");

    //Parsing
    int32u Offset;
    Skip_Local(7,                                               "Signature");
    Skip_B1   (                                                 "Signature");
    Get_L4 (Offset,                                             "Offset to message header fields");
    Skip_L4(                                                    "Serial number");
    Skip_L4(                                                    "Number of header packets");
    Skip_L8(                                                    "Granulerate numerator");
    Skip_L8(                                                    "Granulerate denominator");
    Skip_L8(                                                    "Basegranule");
    Skip_L4(                                                    "Preroll");
    Skip_L1(                                                    "Granuleshift");
    if (Element_Offset<8+Offset)
        Skip_XX(8+Offset-Element_Offset,                        "Unknown");
    if (Element_Offset<Element_Size)
        Skip_Local(Element_Size-Element_Offset,                 "Unknown");
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Comment()
{
    Element_Name("Comment");

    //Integrity
    if (Element_Size<8)
        return;

    //Parsing
    int64u ID_Identification;
    Peek_B8(ID_Identification);

    #undef ELEMENT_CASE
    #ifdef __BORLANDC__ //Borland converts int64u to int32u
        #define ELEMENT_CASE(_NAME) \
            else if (ID_Identification>>(64-8*Elements::Identifier_##_NAME##3)==(((((int64u)Elements::Identifier_##_NAME##1)*0x100000000LL+Elements::Identifier_##_NAME##2)&0x00FFFFFFFFFFFFFFLL)<<8))

    #else //__BORLANDC__
        #define ELEMENT_CASE(_NAME) \
            else if (ID_Identification>>(64-8*Elements::Identifier_##_NAME##3)==((Elements::Identifier_##_NAME&0x00FFFFFFFFFFFFFFLL)<<8))

    #endif //__BORLANDC__

    int32u ID_Identification_Size;
    if (0) ;
    ELEMENT_CASE(kate)      ID_Identification_Size=8;
    else                    ID_Identification_Size=6; //Default
    Skip_Local(ID_Identification_Size,                          "ID");


    //Preparing
    File_VorbisCom Vorbis;
    Vorbis.StreamKind_Specific=StreamKind;
    Vorbis.StreamKind_Multiple=MultipleStreams?StreamKind:Stream_General;
    Vorbis.StreamKind_Common=InAnotherContainer?StreamKind:Stream_General;
    Open_Buffer_Init(&Vorbis);

    //Parsing
    Open_Buffer_Continue(&Vorbis, Buffer+Buffer_Offset+(size_t)Element_Offset, (size_t)(Element_Size-Element_Offset));

    //Filling
    Finish(&Vorbis);
    Merge(Vorbis, Stream_General,  0, 0);
    Merge(Vorbis, StreamKind,      0, 0);
    Merge(Vorbis, Stream_Menu,     0, 0);

    //Testing if we must continue
    if (Identified && (Parser==NULL || Parser->Status[IsFinished]))
        Finish("OggSubElement");
}

//---------------------------------------------------------------------------
void File_Ogg_SubElement::Default()
{
    Element_Name("Frame");

    if (Parser)
    {
        Open_Buffer_Continue(Parser, Buffer+Buffer_Offset, (size_t)Element_Size);
        if (Identified && Parser->Status[IsFinished])
            Finish("OggSubElement");
    }
    else if (Element_Offset<Element_Size)
    {
        Skip_XX(Element_Size-Element_Offset,                    "Unknown");
        if (Identified)
            Finish("OggSubElement");
    }
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_OGG_YES
