// File_Mxf - Info for MXF files
// Copyright (C) 2006-2009 Jerome Martinez, Zen@MediaArea.net
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
#if defined(MEDIAINFO_MXF_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mxf.h"
#if defined(MEDIAINFO_DVDIF_YES)
    #include "MediaInfo/Multiple/File_DvDif.h"
#endif
#if defined(MEDIAINFO_MPEGV_YES)
    #include "MediaInfo/Video/File_Mpegv.h"
#endif
#if defined(MEDIAINFO_MPEGA_YES)
    #include "MediaInfo/Audio/File_Mpega.h"
#endif
#include "MediaInfo/File_Unknown.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
//
//  PartitionPack
//  Primer
//  Preface
//      --> ContentStorage
//              --> Packages --> Package (Material, Source)
//                      --> Tracks --> Track
//                              --> Sequence
//                                      --> StructuralComponents --> StructuralComponent (Timecode, SourceClip)
//                      --> Descriptors --> Descriptor (Multiple, Essence)
//                              --> Descriptors --> Descriptor (Essence)
//              --> EssenceContainerData
//              --> Identifications --> Identification
//
//***************************************************************************

//***************************************************************************
// Constants
//***************************************************************************

#define UUID(NAME, PART1, PART2, PART3, PART4) \
    const int32u NAME##1=0x##PART1; \
    const int32u NAME##2=0x##PART2; \
    const int32u NAME##3=0x##PART3; \
    const int32u NAME##4=0x##PART4; \

namespace Elements
{
    UUID(AES3PCMDescriptor,                                     060E2B34, 02530101, 0D010101, 01014700)
    UUID(CDCIEssenceDescriptor,                                 060E2B34, 02530101, 0D010101, 01012800)
    UUID(ClosedCompleteHeader,                                  060E2B34, 02050101, 0D010201, 01020400)
    UUID(ClosedHeader,                                          060E2B34, 02050101, 0D010201, 01020200)
    UUID(CompleteBody,                                          060E2B34, 02050101, 0D010201, 01030400)
    UUID(CompleteFooter,                                        060E2B34, 02050101, 0D010201, 01040400)
    UUID(CompleteHeader,                                        060E2B34, 02050101, 0D010201, 01020400)
    UUID(ContentStorage,                                        060E2B34, 02530101, 0D010101, 01011800)
    UUID(DMSegment,                                             060E2B34, 02530101, 0D010101, 01014100)
    UUID(EssenceContainerData,                                  060E2B34, 02530101, 0D010101, 01012300)
    UUID(EventTrack,                                            060E2B34, 02530101, 0D010101, 01013900)
    UUID(GenericSoundEssenceDescriptor,                         060E2B34, 02530101, 0D010101, 01014200)
    UUID(Identification,                                        060E2B34, 02530101, 0D010101, 01013000)
    UUID(IndexTableSegment,                                     060E2B34, 02530101, 0D010201, 01100100)
    UUID(JPEG2000PictureSubDescriptor,                          060E2B34, 02530101, 0D010101, 01015A00)
    UUID(MaterialPackage,                                       060E2B34, 02530101, 0D010101, 01013600)
    UUID(MPEG2VideoDescriptor,                                  060E2B34, 02530101, 0D010101, 01015100)
    UUID(MultipleDescriptor,                                    060E2B34, 02530101, 0D010101, 01014400)
    UUID(NetworkLocator,                                        060E2B34, 02530101, 0D010101, 01013200)
    UUID(OpenHeader,                                            060E2B34, 02050101, 0D010201, 01020100)
    UUID(Padding,                                               060E2B34, 01010101, 03010210, 01000000)
    UUID(Preface,                                               060E2B34, 02530101, 0D010101, 01012F00)
    UUID(RandomIndexMetadata,                                   060E2B34, 02050101, 0D010201, 01110100)
    UUID(RGBAEssenceDescriptor,                                 060E2B34, 02530101, 0D010101, 01012900)
    UUID(Primer,                                                060E2B34, 02050101, 0D010201, 01050100)
    UUID(Sequence,                                              060E2B34, 02530101, 0D010101, 01010F00)
    UUID(SourceClip,                                            060E2B34, 02530101, 0D010101, 01011100)
    UUID(SourcePackage,                                         060E2B34, 02530101, 0D010101, 01013700)
    UUID(StaticTrack,                                           060E2B34, 02530101, 0D010101, 01013A00)
    UUID(TextLocator,                                           060E2B34, 02530101, 0D010101, 01013300)
    UUID(TimecodeComponent,                                     060E2B34, 02530101, 0D010101, 01011400)
    UUID(Track,                                                 060E2B34, 02530101, 0D010101, 01013B00)
    UUID(WaveAudioDescriptor,                                   060E2B34, 02530101, 0D010101, 01014800)

    //OperationalPatterns
    UUID(OP_MultiTrack,                                         060E2B34, 04010101, 0D010201, 01010900)
    UUID(OP_SingleTrack,                                        060E2B34, 04010101, 0D010201, 01010100)

    //EssenceContainer
    UUID(EssenceContainer_DV,                                   060E2B34, 04010101, 0D010301, 02020101)
    UUID(EssenceContainer_JPEG2000,                             060E2B34, 04010107, 0D010301, 020C0100)
    UUID(EssenceContainer_MPEG2,                                060E2B34, 04010102, 0D010301, 02046001)
    UUID(EssenceContainer_RV24,                                 060E2B34, 04010101, 0D010301, 02050001)
    UUID(EssenceContainer_Wave,                                 060E2B34, 04010101, 0D010301, 02060100)
    UUID(EssenceContainer_Generic,                              060E2B34, 04010103, 0D010301, 027F0100)

    //MPEG2VideoDescriptor
    UUID(MPEG2VideoDescriptor_SingleSequence,                   060E2B34, 01010105, 04010602, 01020000)
    UUID(MPEG2VideoDescriptor_ConstantBFrames,                  060E2B34, 01010105, 04010602, 01030000)
    UUID(MPEG2VideoDescriptor_CodedContentType,                 060E2B34, 01010105, 04010602, 01040000)
    UUID(MPEG2VideoDescriptor_LowDelay,                         060E2B34, 01010105, 04010602, 01050000)
    UUID(MPEG2VideoDescriptor_ClosedGOP,                        060E2B34, 01010105, 04010602, 01060000)
    UUID(MPEG2VideoDescriptor_IdenticalGOP,                     060E2B34, 01010105, 04010602, 01070000)
    UUID(MPEG2VideoDescriptor_MaxGOP,                           060E2B34, 01010105, 04010602, 01080000)
    UUID(MPEG2VideoDescriptor_BPictureCount,                    060E2B34, 01010105, 04010602, 01090000)
    UUID(MPEG2VideoDescriptor_ProfileAndLevel,                  060E2B34, 01010105, 04010602, 010A0000)
    UUID(MPEG2VideoDescriptor_BitRate,                          060E2B34, 01010105, 04010602, 010B0000)

    //EssenceCoding
    UUID(EssenceCoding_D10Video,                                060E2B34, 04010101, 04010202, 01020101)
    UUID(EssenceCoding_DV,                                      060E2B34, 04010101, 04010202, 02020200)
    UUID(EssenceCoding_JPEG2000,                                060E2B34, 04010107, 04010202, 03010100) //To be confirmed
    UUID(EssenceCoding_MPEG2Video,                              060E2B34, 04010103, 04010202, 01040300) //To be confirmed
    UUID(EssenceCoding_RV24,                                    060E2B34, 04010101, 04010201, 7F000000) //?
    UUID(EssenceCoding_PCM,                                     060E2B34, 04010101, 04020201, 7F000000) //To be confirmed

    //Sequence
    UUID(Sequence_Time,                                         060E2B34, 04010101, 01030201, 01000000)
    UUID(Sequence_Picture,                                      060E2B34, 04010101, 01030202, 01000000)
    UUID(Sequence_Sound,                                        060E2B34, 04010101, 01030202, 02000000)
    UUID(Sequence_Data,                                         060E2B34, 04010101, 01030202, 03000000)
}

//---------------------------------------------------------------------------
const char* Mxf_Category(int8u Category)
{
    switch(Category)
    {
        case 0x01 : return "Item";
        case 0x02 : return "Group (Set/Pack)";
        case 0x03 : return "Wrapper";
        case 0x04 : return "Value";
        default   : return "";
    }
}

//---------------------------------------------------------------------------
const char* Mxf_RegistryDesignator(int8u Category, int8u RegistryDesignator)
{
    switch(Category)
    {
        case 0x01 : //"Item"
                    switch(RegistryDesignator)
                    {
                        case 0x01 : return "Metadata dictionary";
                        case 0x02 : return "Essence dictionary";
                        default   : return "";
                    }
        case 0x02 : //"Group (Set/Pack)"
                    switch(RegistryDesignator)
                    {
                        case 0x05 : return "Contains a fixed number of predefined items with no tag of value indicators";
                        case 0x53 : return "The value of this KLV block contains other KLV-items with a two-byte tag and a two-byte length indicator";
                        default   : return "";
                    }
        case 0x04 : //"Value"
                    switch(RegistryDesignator)
                    {
                        case 0x01 : return "Fixed";
                        default   : return "";
                    }
        default   : return "";
    }
}

//---------------------------------------------------------------------------
const char* Mxf_MPEG2_CodedContentType(int8u CodedContentType)
{
    switch(CodedContentType)
    {
        case 0x01 : return "PPF";
        case 0x02 : return "Interlaced";
        case 0x03 : return ""; //Mixed
        default   : return "";
    }
}

//---------------------------------------------------------------------------
const char* Mxf_OperationalPattern(int128u OperationalPattern)
{
    int32u Code_Compare1=OperationalPattern.hi>>32;
    int32u Code_Compare2=(int32u)OperationalPattern.hi;
    int32u Code_Compare3=OperationalPattern.lo>>32;
    int32u Code_Compare4=(int32u)OperationalPattern.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT   (OP_MultiTrack,                                   "Multi-track")
    ELEMENT   (OP_SingleTrack,                                  "Single-track")
    else
        return "";
}

//---------------------------------------------------------------------------
const char* Mxf_EssenceContainer(int128u EssenceContainer)
{
    int32u Code_Compare1=EssenceContainer.hi>>32;
    int32u Code_Compare2=(int32u)EssenceContainer.hi;
    int32u Code_Compare3=EssenceContainer.lo>>32;
    int32u Code_Compare4=(int32u)EssenceContainer.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT   (EssenceContainer_DV,                             "Digital Video Descriptor")
    ELEMENT   (EssenceContainer_JPEG2000,                       "JPEG-2000 Picture Descriptor")
    ELEMENT   (EssenceContainer_MPEG2,                          "MPEG-2 Video Descriptor")
    ELEMENT   (EssenceContainer_RV24,                           "RV24 (RGBA?)")
    ELEMENT   (EssenceContainer_Wave,                           "Wave Audio Descriptor")
    ELEMENT   (EssenceContainer_Generic,                        "Generic Descriptor")
    else
        return "";
}

//---------------------------------------------------------------------------
const char* Mxf_EssenceCoding(int128u EssenceContainer)
{
    int32u Code_Compare1=EssenceContainer.hi>>32;
    int32u Code_Compare2=(int32u)EssenceContainer.hi;
    int32u Code_Compare3=EssenceContainer.lo>>32;
    int32u Code_Compare4=(int32u)EssenceContainer.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT(EssenceCoding_D10Video,                             "D-10 Video")
    ELEMENT(EssenceCoding_DV,                                   "DV")
    ELEMENT(EssenceCoding_JPEG2000,                             "JPEG-2000")
    ELEMENT(EssenceCoding_PCM,                                  "PCM")
    ELEMENT(EssenceCoding_RV24,                                 "RV24")
    ELEMENT(EssenceCoding_MPEG2Video,                           "MPEG-2 Video")
    else
        return "";
}

//---------------------------------------------------------------------------
const char* Mxf_EssenceCoding_Format(int128u EssenceContainer)
{
    int32u Code_Compare1=EssenceContainer.hi>>32;
    int32u Code_Compare2=(int32u)EssenceContainer.hi;
    int32u Code_Compare3=EssenceContainer.lo>>32;
    int32u Code_Compare4=(int32u)EssenceContainer.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT(EssenceCoding_D10Video,                             "MPEG Video")
    ELEMENT(EssenceCoding_DV,                                   "Digital Video")
    ELEMENT(EssenceCoding_JPEG2000,                             "JPEG-2000")
    ELEMENT(EssenceCoding_MPEG2Video,                           "MPEG Video")
    ELEMENT(EssenceCoding_PCM,                                  "PCM")
    ELEMENT(EssenceCoding_RV24,                                 "RV24")
    else
        return "";
}

//---------------------------------------------------------------------------
const char* Mxf_EssenceCoding_Format_Version(int128u EssenceContainer)
{
    int32u Code_Compare1=EssenceContainer.hi>>32;
    int32u Code_Compare2=(int32u)EssenceContainer.hi;
    int32u Code_Compare3=EssenceContainer.lo>>32;
    int32u Code_Compare4=(int32u)EssenceContainer.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT(EssenceCoding_D10Video,                             "Version 2")
    ELEMENT(EssenceCoding_MPEG2Video,                           "Version 2")
    else
        return "";
}

//---------------------------------------------------------------------------
const char* Mxf_Sequence_DataDefinition(int128u DataDefinition)
{
    int32u Code_Compare1=DataDefinition.hi>>32;
    int32u Code_Compare2=(int32u)DataDefinition.hi;
    int32u Code_Compare3=DataDefinition.lo>>32;
    int32u Code_Compare4=(int32u)DataDefinition.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
        return _NAME; \

    if (0) {}
    ELEMENT(Sequence_Time,                                      "Time")
    ELEMENT(Sequence_Picture,                                   "Picture")
    ELEMENT(Sequence_Sound,                                     "Sound")
    ELEMENT(Sequence_Data,                                      "Data")
    else
        return "";
}

//---------------------------------------------------------------------------
extern const char* Mpegv_profile_and_level_indication_profile[];
extern const char* Mpegv_profile_and_level_indication_level[];

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Mxf::File_Mxf()
:File__Analyze()
{
    //Configuration
    MustSynchronize=true;

    //Temp
    Streams_Count=(size_t)-1;
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mxf::Streams_Finish()
{
    //Per ContentStorage
    for (contentstorages::iterator ContentStorage=ContentStorages.begin(); ContentStorage!=ContentStorages.end(); ContentStorage++)
    {
        //Per Package
        for (size_t Packages_Pos=0; Packages_Pos<ContentStorage->second.Packages.size(); Packages_Pos++)
        {
            //Searching Package
            packages::iterator Package=Packages.find(ContentStorage->second.Packages[Packages_Pos]);
            if (Package!=Packages.end())
            {
                //Descriptor
                Streams_Finish_Descriptor(Package->second.Descriptor);

                //Tracks
                for(size_t Track_Pos=0; Track_Pos<Package->second.Tracks.size(); Track_Pos++)
                {
                    //Track
                    Streams_Finish_Track(Package->second.Tracks[Track_Pos]);
                }
            }
        }
    }
    
    //Per Identification
    for (identifications::iterator Identification=Identifications.begin(); Identification!=Identifications.end(); Identification++)
    {
        if (!Identification->second.ProductName.empty())
        {
            Ztring Encoded_Library_Name;
            if (!Identification->second.CompanyName.empty())
            {
                Encoded_Library_Name+=Identification->second.CompanyName;
                Encoded_Library_Name+=_T(' ');
            }
            Encoded_Library_Name+=Identification->second.ProductName;
            Ztring Encoded_Library_Version;
            if (!Identification->second.ProductVersion.empty())
            {
                Encoded_Library_Version=Identification->second.ProductVersion;
            }
            else if (!Identification->second.VersionString.empty())
            {
                Encoded_Library_Version=Identification->second.VersionString;
            }
            Ztring Encoded_Application=Encoded_Library_Name;
            if (!Encoded_Library_Version.empty())
            {
                Encoded_Application+=_T(' ');
                Encoded_Application+=Encoded_Library_Version;
            }
            Fill(Stream_General, 0, General_Encoded_Application, Encoded_Application, true);
            Fill(Stream_General, 0, General_Encoded_Library_Name, Encoded_Library_Name, true);
            Fill(Stream_General, 0, General_Encoded_Library_Version, Encoded_Library_Version, true);
        }
        for (std::map<std::string, Ztring>::iterator Info=Identification->second.Infos.begin(); Info!=Identification->second.Infos.end(); Info++)
            Fill(Stream_General, 0, Info->first.c_str(), Info->second, true);
    }
}

//---------------------------------------------------------------------------
void File_Mxf::Streams_Finish_Descriptor(int128u DescriptorUID)
{
    descriptors::iterator Descriptor=Descriptors.find(DescriptorUID);
    if (Descriptor==Descriptors.end())
        return;

    //Subs
    if (!Descriptor->second.SubDescriptors.empty())
    {
        for (size_t Pos=0; Pos<Descriptor->second.SubDescriptors.size(); Pos++)
            Streams_Finish_Descriptor(Descriptor->second.SubDescriptors[Pos]);
        return; //Is not a real descriptor
    }

    if (Descriptor->second.StreamKind!=Stream_Max)
    {
        if (TrackIDs[Descriptor->second.LinkedTrackID].StreamKind==Stream_Max)
        {
            Stream_Prepare(Descriptor->second.StreamKind);
            TrackIDs[Descriptor->second.LinkedTrackID].StreamKind=StreamKind_Last;
            TrackIDs[Descriptor->second.LinkedTrackID].StreamPos=StreamPos_Last;
        }
        else
        {
            StreamKind_Last=TrackIDs[Descriptor->second.LinkedTrackID].StreamKind;
            StreamPos_Last=TrackIDs[Descriptor->second.LinkedTrackID].StreamPos;
        }

        for (std::map<std::string, Ztring>::iterator Info=Descriptor->second.Infos.begin(); Info!=Descriptor->second.Infos.end(); Info++)
            Fill(StreamKind_Last, StreamPos_Last, Info->first.c_str(), Info->second, true);
    }
}

//---------------------------------------------------------------------------
void File_Mxf::Streams_Finish_Track(int128u TrackUID)
{
    tracks::iterator Track=Tracks.find(TrackUID);
    if (Track==Tracks.end())
        return;

    //TrackID/TrackNumber exists?
    if (TrackIDs[Track->second.TrackID].StreamKind==Stream_Max)
        return;
    StreamKind_Last=TrackIDs[Track->second.TrackID].StreamKind;
    StreamPos_Last=TrackIDs[Track->second.TrackID].StreamPos;

    //TrackNumber
    essences::iterator Temp=Essences.find(Track->second.TrackNumber);
    if (Temp!=Essences.end() && Retrieve(StreamKind_Last, StreamPos_Last, General_UniqueID).empty()) //if not yet done
    {
        for (std::map<std::string, Ztring>::iterator Info=Temp->second.Infos.begin(); Info!=Temp->second.Infos.end(); Info++)
            Fill(StreamKind_Last, StreamPos_Last, Info->first.c_str(), Info->second, true);
        Fill(StreamKind_Last, StreamPos_Last, General_UniqueID, Temp->first);

        Finish(Temp->second.Parser);
        Merge(*Temp->second.Parser, StreamKind_Last, 0, StreamPos_Last);

        //Bitrate (PCM)
        if (StreamKind_Last==Stream_Audio && Retrieve(Stream_Audio, StreamPos_Last, Audio_BitRate).empty() && Retrieve(Stream_Audio, StreamPos_Last, Audio_Format)==_T("PCM"))
        {
            int64u Channels=Retrieve(Stream_Audio, StreamPos_Last, Audio_Channel_s_).To_int64u();
            int64u SamplingRate=Retrieve(Stream_Audio, StreamPos_Last, Audio_SamplingRate).To_int64u();
            int64u Resolution=Retrieve(Stream_Audio, StreamPos_Last, Audio_Resolution).To_int64u();
            if (Channels && SamplingRate && Resolution)
               Fill(Stream_Audio, StreamPos_Last, Audio_BitRate, Channels*SamplingRate*Resolution);
        }

        //Bitrate (Video)
        if (StreamKind_Last==Stream_Video && Retrieve(Stream_Video, StreamPos_Last, Video_BitRate).empty())
        {
            //Until now, I only found CBR files
            Fill(Stream_Video, StreamPos_Last, Video_BitRate, Retrieve(Stream_Video, StreamPos_Last, Video_BitRate_Nominal));
        }

        //Special case - Digital Video
        #if defined(MEDIAINFO_DVDIF_YES)
            if (StreamKind_Last==Stream_Video && Retrieve(Stream_Video, StreamPos_Last, Video_Format)==_T("Digital Video"))
            {
                if (Retrieve(Stream_General, 0, General_Recorded_Date).empty())
                    Fill(Stream_General, 0, General_Recorded_Date, Temp->second.Parser->Retrieve(Stream_General, 0, General_Recorded_Date));

                //Video and Audio are together
                size_t Audio_Count=Temp->second.Parser->Count_Get(Stream_Audio);
                for (size_t Audio_Pos=0; Audio_Pos<Audio_Count; Audio_Pos++)
                {
                    Fill_Flush();
                    Stream_Prepare(Stream_Audio);
                    size_t Pos=Count_Get(Stream_Audio)-1;
                    Temp->second.Parser->Finish();
                    Merge(*Temp->second.Parser, Stream_Audio, Audio_Pos, StreamPos_Last);
                    Fill(Stream_Audio, Pos, Audio_MuxingMode, "DV");
                    Fill(Stream_Audio, Pos, Audio_Duration, Retrieve(Stream_Video, Temp->second.StreamPos, Video_Duration));
                    Fill(Stream_Audio, Pos, "MuxingMode_MoreInfo", _T("Muxed in Video #")+Ztring().From_Number(Temp->second.StreamPos+1));
                    Fill(Stream_Audio, Pos, Audio_StreamSize, "0"); //Included in the DV stream size
                    Ztring ID=Retrieve(Stream_Audio, Pos, Audio_ID);
                    Fill(Stream_Audio, Pos, Audio_ID, Retrieve(Stream_Video, Temp->second.StreamPos, Video_ID)+_T("-")+ID, true);
                }

                StreamKind_Last=Stream_Video;
                StreamPos_Last=Temp->second.StreamPos;
            }
        #endif

        //Special case - MPEG Video + Captions
        if (StreamKind_Last==Stream_Video && Temp->second.Parser && Temp->second.Parser->Count_Get(Stream_Text))
        {
            //Video and Text are together
            size_t Text_Count=Temp->second.Parser->Count_Get(Stream_Text);
            for (size_t Text_Pos=0; Text_Pos<Text_Count; Text_Pos++)
            {
                size_t Pos=Count_Get(Stream_Text)-Text_Count+Text_Pos;
                Ztring MuxingMode=Retrieve(Stream_Text, Pos, "MuxingMode");
                Fill(Stream_Text, Pos, "MuxingMode", Ztring(_T("MPEG Video / "))+MuxingMode, true);
                if (!IsSub)
                    Fill(Stream_Text, Pos, "MuxingMode_MoreInfo", _T("Muxed in Video #")+Ztring().From_Number(StreamPos_Last+1));
                Fill(Stream_Text, Pos, Text_StreamSize, 0);
                Ztring ID=Retrieve(Stream_Text, Pos, Text_ID);
                Fill(Stream_Text, Pos, Text_ID, Retrieve(Stream_Video, StreamPos_Last, Video_ID)+_T("-")+ID, true);
                Fill(Stream_Text, Pos, Text_ID_String, Retrieve(Stream_Video, StreamPos_Last, Video_ID_String)+_T("-")+ID, true);
                Fill(Stream_Text, Pos, Text_Delay, Retrieve(Stream_Video, StreamPos_Last, Video_Delay), true);
            }
        }
    }

    //Sequence
    Streams_Finish_Component(Track->second.Sequence, Track->second.EditRate);
}

//---------------------------------------------------------------------------
void File_Mxf::Streams_Finish_Component(int128u ComponentUID, float32 EditRate)
{
    components::iterator Component=Components.find(ComponentUID);
    if (Component==Components.end())
        return;

    //Duration
    if (EditRate)
        Fill(StreamKind_Last, StreamPos_Last, Fill_Parameter(StreamKind_Last, Generic_Duration), Component->second.Duration/EditRate*1000, 0, true);
}

//***************************************************************************
// Buffer - File header
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mxf::FileHeader_Begin()
{
    if (Buffer_Size<4)
        return false;
    if (CC4(Buffer)!=0x060E2B34)
    {
        Reject("MXF");
        return false;
    }
        Accept("MXF");
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "MXF");
    return true;
}

//***************************************************************************
// Buffer - Synchro
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Mxf::Synchronize()
{
    //Synchronizing
    while (Buffer_Offset+16<=Buffer_Size
        && CC4(Buffer+Buffer_Offset)!=0x060E2B34)
        Buffer_Offset++;

    //Parsing last bytes if needed
    if (Buffer_Offset+5>Buffer_Size)
    {
        if (Buffer_Offset+4==Buffer_Size && CC4(Buffer+Buffer_Offset)!=0x060E2B34)
            Buffer_Offset++;
        if (Buffer_Offset+3==Buffer_Size && CC3(Buffer+Buffer_Offset)!=0x060E2B)
            Buffer_Offset++;
        if (Buffer_Offset+2==Buffer_Size && CC2(Buffer+Buffer_Offset)!=0x060E)
            Buffer_Offset++;
        if (Buffer_Offset+1==Buffer_Size && CC1(Buffer+Buffer_Offset)!=0x06)
            Buffer_Offset++;
        return false;
    }

    //Synched is OK
    return true;
}

//---------------------------------------------------------------------------
bool File_Mxf::Synched_Test()
{
    //Trailing 0x00
    while(Buffer_Offset+1<=Buffer_Size && Buffer[Buffer_Offset]==0x00)
        Buffer_Offset++;

    //Must have enough buffer for having header
    if (Buffer_Offset+16>Buffer_Size)
        return false;

    //Quick test of synchro
    if (CC4(Buffer+Buffer_Offset)!=0x060E2B34)
    {
        Synched=false;
        Trusted++;
    }

    //Quick search
    if (!Synched && !Synchronize())
        return false;

    //We continue
    return true;
}

//***************************************************************************
// Buffer - Per element
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mxf::Header_Parse()
{
    //Parsing
    int8u Length;
    Get_UL(Code,                                                "Code");
    Get_B1(Length,                                              "Length");
    if (Length<0x80)
    {
        Header_Fill_Size(Element_Offset+Length);
    }
    else
    {
        Length&=0x7F;
        switch (Length)
        {
            case 1 :
                    {
                    int8u  Length1;
                    Get_B1(Length1,                             "Length");
                    Header_Fill_Size(Element_Offset+Length1);
                    }
                    break;
            case 2 :
                    {
                    int16u Length2;
                    Get_B2(Length2,                             "Length");
                    Header_Fill_Size(Element_Offset+Length2);
                    }
                    break;
            case 3 :
                    {
                    int32u Length3;
                    Get_B3(Length3,                             "Length");
                    Header_Fill_Size(Element_Offset+Length3);
                    }
                    break;
            case 4 :
                    {
                    int32u Length4;
                    Get_B4(Length4,                             "Length");
                    Header_Fill_Size(Element_Offset+Length4);
                    }
                    break;
            case 5 :
                    {
                    int64u Length5;
                    Get_B5(Length5,                             "Length");
                    Header_Fill_Size(Element_Offset+Length5);
                    }
                    break;
            case 6 :
                    {
                    int64u Length6;
                    Get_B6(Length6,                             "Length");
                    Header_Fill_Size(Element_Offset+Length6);
                    }
                    break;
            case 7 :
                    {
                    int64u Length7;
                    Get_B7(Length7,                             "Length");
                    Header_Fill_Size(Element_Offset+Length7);
                    }
                    break;
            case 8 :
                    {
                    int64u Length8;
                    Get_B8(Length8,                             "Length");
                    Header_Fill_Size(Element_Offset+Length8);
                    }
                    break;
            default: Header_Fill_Size(Element_Offset);
        }
    }

    //Filling
    Ztring Temp=Ztring().From_Number(Code.hi, 16)+Ztring().From_Number(Code.lo, 16);
    Header_Fill_Code(0, Temp);
}

//---------------------------------------------------------------------------
void File_Mxf::Data_Parse()
{
    //Clearing
    InstanceUID=0;

    //Parsing
    int32u Code_Compare1=Code.hi>>32;
    int32u Code_Compare2=(int32u)Code.hi;
    int32u Code_Compare3=Code.lo>>32;
    int32u Code_Compare4=(int32u)Code.lo;
    #undef ELEMENT
    #define ELEMENT(_ELEMENT, _NAME) \
    else if (Code_Compare1==Elements::_ELEMENT##1 \
     && Code_Compare2==Elements::_ELEMENT##2 \
     && Code_Compare3==Elements::_ELEMENT##3 \
     && Code_Compare4==Elements::_ELEMENT##4) \
    { \
        Element_Name(_NAME); \
        switch (Code_Compare2>>24) \
        { \
            case 0x01 : _ELEMENT(); break; \
            case 0x02 : switch ((int8u)(Code_Compare2>>16)) \
                        { \
                            case 0x05 : _ELEMENT(); break; \
                            case 0x53 : \
                                        while(Element_Offset<Element_Size) \
                                        { \
                                            Element_Begin(); \
                                            Element_Begin("Header"); \
                                                Get_B2 (Code2,                                  "Code"); \
                                                Get_B2 (Length2,                                "Length"); \
                                            Element_End(); \
                                            Element_Name(Ztring().From_CC2(Code2)); \
                                            \
                                            int64u End=Element_Offset+Length2; \
                                            _ELEMENT(); \
                                            if (Element_Offset<End) \
                                                Skip_XX(End-Element_Offset,                     "Unknown"); \
                                            \
                                            Element_End(4+Length2); \
                                        } \
                                        break; \
                            default   : Skip_XX(Element_Size,                                   "Unknown"); \
                        } \
        } \
    } \


    if (0) {}
    ELEMENT(AES3PCMDescriptor,                                  "AES3 Descriptor")
    ELEMENT(CDCIEssenceDescriptor,                              "CDCI Essence Descriptor")
    ELEMENT(ClosedHeader,                                       "Closed Header Partition Pack")
    ELEMENT(ClosedCompleteHeader,                               "Closed Complete Header Partition Pack")
    ELEMENT(CompleteBody,                                       "CompleteBody")
    ELEMENT(CompleteFooter,                                     "CompleteFooter")
    ELEMENT(CompleteHeader,                                     "CompleteHeader")
    ELEMENT(ContentStorage,                                     "Content Storage")
    ELEMENT(DMSegment,                                          "Descriptive Metadata Segment")
    ELEMENT(EssenceContainerData,                               "Essence Container Data")
    ELEMENT(EventTrack,                                         "Event track")
    ELEMENT(GenericSoundEssenceDescriptor,                      "Generic Sound Essence Descriptor")
    ELEMENT(Identification,                                     "Identification")
    ELEMENT(IndexTableSegment,                                  "Index Table (Segment)")
    ELEMENT(JPEG2000PictureSubDescriptor,                       "JPEG-2000 Picture Sub Descriptor")
    ELEMENT(MaterialPackage,                                    "Material Package")
    ELEMENT(MultipleDescriptor,                                 "Multiple Descriptor")
    ELEMENT(MPEG2VideoDescriptor,                               "MPEG-2 Video Descriptor")
    ELEMENT(NetworkLocator,                                     "Network Locator")
    ELEMENT(OpenHeader,                                         "OpenHeader")
    ELEMENT(Preface,                                            "Preface")
    ELEMENT(Padding,                                            "Padding")
    ELEMENT(Primer,                                             "Primer")
    ELEMENT(RGBAEssenceDescriptor,                              "RGBA Essence Descriptor")
    ELEMENT(RandomIndexMetadata,                                "Random Index Metadata")
    ELEMENT(Sequence,                                           "Sequence")
    ELEMENT(SourceClip,                                         "Source Clip")
    ELEMENT(SourcePackage,                                      "Source Package")
    ELEMENT(StaticTrack,                                        "Static Track")
    ELEMENT(TextLocator,                                        "Text Locator")
    ELEMENT(TimecodeComponent,                                  "Timecode Component")
    ELEMENT(Track,                                              "Track")
    ELEMENT(WaveAudioDescriptor,                                "Wave Audio Descriptor")
    else if (Code_Compare1==0x060E2B34
          && Code_Compare2==0x01020101
          && Code_Compare3==0x0D010301)
    {
        switch (Code_Compare4&0xFF00FF00)
        {
            case 0x14000100 : Element_Name("Generic"        ); break;
            case 0x05000100 : Element_Name("D-10 Video"     ); break;
            case 0x15000500 : Element_Name("MPEG Video"     ); break;
            case 0x15000800 : Element_Name("JPEG-2000"      ); break;
            case 0x06001000 : Element_Name("D-10 Audio"     ); break;
            case 0x16000100 : Element_Name("BWF (PCM)"      ); break;
            case 0x16000300 : Element_Name("DV Audio (PCM)" ); break;
            case 0x16000500 : Element_Name("MPEG Audio"     ); break;
            case 0x18000100 : Element_Name("Digital Video"  ); break;
            default         : Element_Name("Unknown stream" );
        }

        if (Essences[Code_Compare4].Parser==NULL)
        {
            switch (Code_Compare4&0xFF00FF00)
            {
                case 0x05000100 : //D-10 Video
                case 0x15000500 : //MPEG Video
                                    Essences[Code_Compare4].StreamKind=Stream_Video;
                                    Essences[Code_Compare4].StreamPos=Code_Compare4&0x000000FF;
                                    #if defined(MEDIAINFO_MPEGV_YES)
                                        Essences[Code_Compare4].Parser=new File_Mpegv();
                                    #else
                                        Essences[Code_Compare4].Parser=new File_Unknown();
                                        Open_Buffer_Init(Essences[Code_Compare4].Parser);
                                        Essences[Code_Compare4].Parser->Stream_Prepare(Stream_Video);
                                        Essences[Code_Compare4].Parser->Fill(Stream_Video, 0, Video_Format, "MPEG Video");
                                        if (Streams_Count>0)
                                            Streams_Count--;
                                    #endif
                                    break;
                case 0x15000800 : //JPEG-2000
                                    Essences[Code_Compare4].StreamKind=Stream_Video;
                                    Essences[Code_Compare4].StreamPos=Code_Compare4&0x000000FF;
                                    #if defined(MEDIAINFO_JPEG2000_YES) //Not yet
                                        Essences[Code_Compare4].Parser=new File_Jpeg20000(); //Not yet
                                    #else
                                        Essences[Code_Compare4].Parser=new File_Unknown();
                                        Open_Buffer_Init(Essences[Code_Compare4].Parser);
                                        Essences[Code_Compare4].Parser->Stream_Prepare(Stream_Video);
                                        Essences[Code_Compare4].Parser->Fill(Stream_Video, 0, Video_Format, "JPEG-2000");
                                        if (Streams_Count>0)
                                            Streams_Count--;
                                    #endif
                                    break;
                case 0x06001000 : //D-10 Audio
                case 0x16000100 : //BWF (PCM)
                case 0x16000300 : //DV Audio (PCM)
                                    Essences[Code_Compare4].StreamKind=Stream_Audio;
                                    Essences[Code_Compare4].StreamPos=Code_Compare4&0x000000FF;
                                    Essences[Code_Compare4].Parser=new File_Unknown();
                                    Open_Buffer_Init(Essences[Code_Compare4].Parser);
                                    Essences[Code_Compare4].Parser->Stream_Prepare(Stream_Audio);
                                    Essences[Code_Compare4].Parser->Fill(Stream_Audio, 0, Audio_Format, "PCM");
                                    if (Streams_Count>0)
                                        Streams_Count--;
                                    break;
                case 0x16000500 : //MPEG Audio
                                    Essences[Code_Compare4].StreamKind=Stream_Audio;
                                    Essences[Code_Compare4].StreamPos=Code_Compare4&0x000000FF;
                                    #if defined(MEDIAINFO_MPEGA_YES)
                                        Essences[Code_Compare4].Parser=new File_Mpega();
                                    #else
                                        Essences[Code_Compare4].Parser=new File_Unknown();
                                        Open_Buffer_Init(Essences[Code_Compare4].Parser);
                                        Essences[Code_Compare4].Parser->Stream_Prepare(Stream_Audio);
                                        Essences[Code_Compare4].Parser->Fill(Stream_Audio, 0, Audio_Format, "MPEG Audio");
                                        if (Streams_Count>0)
                                            Streams_Count--;
                                    #endif
                                    break;
                case 0x18000100 : //Digital Video
                case 0x18000200 : //Digital Video
                                    Essences[Code_Compare4].StreamKind=Stream_Video;
                                    Essences[Code_Compare4].StreamPos=Code_Compare4&0x000000FF;
                                    #if defined(MEDIAINFO_DVDIF_YES)
                                        Essences[Code_Compare4].Parser=new File_DvDif();
                                        //Bitrate is precise
                                        if (Element_Size==144000)
                                            Essences[Code_Compare4].Infos["BitRate"].From_Number( 28800000);
                                        if (Element_Size==288000)
                                            Essences[Code_Compare4].Infos["BitRate"].From_Number( 57600000);
                                        if (Element_Size==576000)
                                            Essences[Code_Compare4].Infos["BitRate"].From_Number(115200000);
                                    #else
                                        Essences[Code_Compare4].Parser=new File_Unknown();
                                        Open_Buffer_Init(Essences[Code_Compare4].Parser);
                                        Essences[Code_Compare4].Parser->Stream_Prepare(Stream_Video);
                                        Essences[Code_Compare4].Parser->Fill(Stream_Video, 0, Video_Format, "Digital Video");
                                        if (Streams_Count>0)
                                            Streams_Count--;
                                    #endif
                                    break;
                default         :   Essences[Code_Compare4].Parser=new File__Analyze();
            }
            Open_Buffer_Init(Essences[Code_Compare4].Parser);
        }

        if (!(Essences[Code_Compare4].Parser->Status[IsFinished] || Essences[Code_Compare4].Parser->Status[IsFilled]))
        {
            //Parsing
            Open_Buffer_Continue(Essences[Code_Compare4].Parser, Buffer+Buffer_Offset, (size_t)Element_Size);

            //Disabling this Streams
            if (Essences[Code_Compare4].Parser->Status[IsFinished] || Essences[Code_Compare4].Parser->Status[IsFilled])
            {
                if (Streams_Count>0)
                    Streams_Count--;
            }
        }
        else
            Skip_XX(Element_Size,                               "Data");
    }
    else
        Skip_XX(Element_Size,                                   "Unknown");

    if (File_Offset>=0x4000000 //TODO: 64 MB by default (security), should be changed
     || (Streams_Count==0 && !Descriptors.empty()))
    {
        Finish("MXF");
    }
}

//***************************************************************************
// Elements
//***************************************************************************

#undef ELEMENT
#define ELEMENT(_CODE, _CALL, _NAME) \
    case 0x##_CODE :   Element_Name(_NAME); _CALL(); break; \

#define ELEMENT_UUID(_ELEMENT, _NAME) \
else if (Code_Compare1==Elements::_ELEMENT##1 \
      && Code_Compare2==Elements::_ELEMENT##2 \
      && Code_Compare3==Elements::_ELEMENT##3 \
      && Code_Compare4==Elements::_ELEMENT##4) \
{ \
    Element_Name(_NAME); \
    _ELEMENT(); \
}

//---------------------------------------------------------------------------
void File_Mxf::AES3PCMDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3D08, AES3PCMDescriptor_AuxBitsMode,            "Use of Auxiliary Bits")
        ELEMENT(3D0D, AES3PCMDescriptor_Emphasis,               "Emphasis")
        ELEMENT(3D0F, AES3PCMDescriptor_BlockStartOffset,       "Position of first Z preamble in essence stream")
        ELEMENT(3D10, AES3PCMDescriptor_ChannelStatusMode,      "Enumerated mode of carriage of channel status data")
        ELEMENT(3D11, AES3PCMDescriptor_FixedChannelStatusData, "Fixed data pattern for channel status data")
        ELEMENT(3D12, AES3PCMDescriptor_UserDataMode,           "Mode of carriage of user data")
        ELEMENT(3D13, AES3PCMDescriptor_FixedUserData,          "Fixed data pattern for user data")
        default: WaveAudioDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::CDCIEssenceDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3301, CDCIEssenceDescriptor_ComponentDepth,     "Active bits per sample")
        ELEMENT(3302, CDCIEssenceDescriptor_HorizontalSubsampling, "Horizontal colour subsampling")
        ELEMENT(3303, CDCIEssenceDescriptor_ColorSiting,        "Color siting")
        ELEMENT(3304, CDCIEssenceDescriptor_BlackRefLevel,      "Black refernece level")
        ELEMENT(3305, CDCIEssenceDescriptor_WhiteReflevel,      "White reference level")
        ELEMENT(3306, CDCIEssenceDescriptor_ColorRange,         "Color range")
        ELEMENT(3307, CDCIEssenceDescriptor_PaddingBits,        "Bits to round up each pixel to stored size")
        ELEMENT(3308, CDCIEssenceDescriptor_VerticalSubsampling,"Vertical colour subsampling")
        ELEMENT(3309, CDCIEssenceDescriptor_AlphaSampleDepth,   "Bits per alpha sample")
        ELEMENT(330B, CDCIEssenceDescriptor_ReversedByteOrder,  "Luma followed by Chroma")
        default: GenericPictureEssenceDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::ClosedCompleteHeader()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::ClosedHeader()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::CompleteBody()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::CompleteFooter()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::CompleteHeader()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::ContentStorage()
{
    switch(Code2)
    {
        ELEMENT(1901, ContentStorage_Packages,                  "Packages")
        ELEMENT(1902, ContentStorage_EssenceContainerData,      "EssenceContainerData")
        default: GenerationInterchangeObject();
    }

    if (Code2==0x3C0A && InstanceUID==Preface_ContentStorage_Data) //InstanceIUD
    {
        Element_Level--;
        Element_Info("Valid from Preface");
        Element_Level++;
    }
}

//---------------------------------------------------------------------------
void File_Mxf::DMSegment()
{
    switch(Code2)
    {
        default: StructuralComponent();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::EssenceContainerData()
{
    switch(Code2)
    {
        ELEMENT(2701, EssenceContainerData_LinkedPackageUID,    "LinkedPackageUID")
        ELEMENT(3F06, EssenceContainerData_IndexSID,            "IndexSID")
        ELEMENT(3F07, EssenceContainerData_BodySID,             "BodySID")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::EventTrack()
{
    switch(Code2)
    {
        ELEMENT(4901, EventTrack_EventEditRate,                 "Edit Rate of Event Track")
        ELEMENT(4902, EventTrack_EventOrigin,                   "Offset used to resolved timeline references to this event track")
        default: GenericTrack();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::FileDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3001, FileDescriptor_SampleRate,                "SampleRate")
        ELEMENT(3002, FileDescriptor_ContainerDuration,         "ContainerDuration")
        ELEMENT(3004, FileDescriptor_EssenceContainer,          "EssenceContainer")
        ELEMENT(3005, FileDescriptor_Codec,                     "Codec")
        ELEMENT(3006, FileDescriptor_LinkedTrackID,             "LinkedTrackID")
        default: GenericDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::Identification()
{
    switch(Code2)
    {
        ELEMENT(3C01, Identification_CompanyName,               "CompanyName")
        ELEMENT(3C02, Identification_ProductName,               "ProductName")
        ELEMENT(3C03, Identification_ProductVersion,            "ProductVersion")
        ELEMENT(3C04, Identification_VersionString,             "VersionString")
        ELEMENT(3C05, Identification_ProductUID,                "ProductUID")
        ELEMENT(3C06, Identification_ModificationDate ,         "ModificationDate ")
        ELEMENT(3C07, Identification_ToolkitVersion,            "ToolkitVersion")
        ELEMENT(3C08, Identification_Platform,                  "Platform")
        ELEMENT(3C09, Identification_ThisGenerationUID,         "ThisGenerationUID")
        default: InterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::IndexTableSegment()
{
    switch(Code2)
    {
        ELEMENT(3F05, IndexTableSegment_EditUnitByteCount,      "EditUnitByteCount")
        ELEMENT(3F06, IndexTableSegment_IndexSID,               "IndexSID")
        ELEMENT(3F07, IndexTableSegment_BodySID,                "BodySID")
        ELEMENT(3F08, IndexTableSegment_SliceCount,             "SliceCount")
        ELEMENT(3F0B, IndexTableSegment_IndexEditRate,          "IndexEditRate")
        ELEMENT(3F0C, IndexTableSegment_IndexStartPosition,     "IndexStartPosition")
        ELEMENT(3F0D, IndexTableSegment_IndexDuration,          "IndexDuration")
        ELEMENT(3F0E, IndexTableSegment_PosTableCount ,         "PosTableCount ")
        default: InterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenericDescriptor()
{
    switch(Code2)
    {
        ELEMENT(2F01, GenericDescriptor_Locators,               "Locators")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::JPEG2000PictureSubDescriptor()
{
    switch(Code2)
    {
        ELEMENT(8001, JPEG2000PictureSubDescriptor_Rsiz,        "Decoder capabilities")
        ELEMENT(8002, JPEG2000PictureSubDescriptor_Xsiz,        "Width")
        ELEMENT(8003, JPEG2000PictureSubDescriptor_Ysiz,        "Height")
        ELEMENT(8004, JPEG2000PictureSubDescriptor_XOsiz,       "Horizontal offset")
        ELEMENT(8005, JPEG2000PictureSubDescriptor_YOsiz,       "Vertical offset ")
        ELEMENT(8006, JPEG2000PictureSubDescriptor_XTsiz,       "Width of one reference tile ")
        ELEMENT(8007, JPEG2000PictureSubDescriptor_YTsiz,       "Height of one reference tile ")
        ELEMENT(8008, JPEG2000PictureSubDescriptor_XTOsiz,      "Horizontal offset of the first tile")
        ELEMENT(8009, JPEG2000PictureSubDescriptor_YTOsiz,      "Vertical offset of the first tile")
        ELEMENT(800A, JPEG2000PictureSubDescriptor_Csiz,        "Number of components in the picture")
        ELEMENT(800B, JPEG2000PictureSubDescriptor_PictureComponentSizing, "Picture components")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenerationInterchangeObject()
{
    //Parsing
    switch(Code2)
    {
        ELEMENT(0102, GenerationInterchangeObject_GenerationUID, "GenerationUID")
        default: InterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenericPackage()
{
    switch(Code2)
    {
        ELEMENT(4401, GenericPackage_PackageUID,                "PackageUID")
        ELEMENT(4402, GenericPackage_Name,                      "Name")
        ELEMENT(4403, GenericPackage_Tracks,                    "Tracks")
        ELEMENT(4404, GenericPackage_PackageModifiedDate,       "PackageModifiedDate")
        ELEMENT(4405, GenericPackage_PackageCreationDate,       "PackageCreationDate")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenericPictureEssenceDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3201, GenericPictureEssenceDescriptor_PictureEssenceCoding, "Identifier of the Picture Compression Scheme")
        ELEMENT(3202, GenericPictureEssenceDescriptor_StoredHeight, "Vertical Field Size")
        ELEMENT(3203, GenericPictureEssenceDescriptor_StoredWidth, "Horizontal Size")
        ELEMENT(3204, GenericPictureEssenceDescriptor_SampledHeight, "Sampled height supplied to codec")
        ELEMENT(3205, GenericPictureEssenceDescriptor_SampledWidth, "Sampled width supplied to codec")
        ELEMENT(3206, GenericPictureEssenceDescriptor_SampledXOffset, "Offset from sampled to stored width")
        ELEMENT(3207, GenericPictureEssenceDescriptor_SampledYOffset, "Offset from sampled to stored")
        ELEMENT(3208, GenericPictureEssenceDescriptor_DisplayHeight, "Displayed Height placed in Production Aperture")
        ELEMENT(3209, GenericPictureEssenceDescriptor_DisplayWidth, "Displayed Width placed in Production Aperture")
        ELEMENT(320A, GenericPictureEssenceDescriptor_DisplayXOffset,"Horizontal offset from the of the picture as displayed")
        ELEMENT(320B, GenericPictureEssenceDescriptor_DisplayYOffset,"Vertical offset of the picture as displayed")
        ELEMENT(320C, GenericPictureEssenceDescriptor_FrameLayout, "Interlace or Progressive layout")
        ELEMENT(320D, GenericPictureEssenceDescriptor_VideoLineMap, "First active line in each field")
        ELEMENT(320E, GenericPictureEssenceDescriptor_AspectRatio, "Aspect ratio")
        ELEMENT(320F, GenericPictureEssenceDescriptor_AlphaTransparency, "Is Alpha Inverted")
        ELEMENT(3210, GenericPictureEssenceDescriptor_Gamma,    "Gamma")
        ELEMENT(3211, GenericPictureEssenceDescriptor_ImageAlignmentOffset, "Byte Boundary alignment required for Low Level Essence Storage")
        ELEMENT(3212, GenericPictureEssenceDescriptor_FieldDominance,"Number of the field which is considered temporally to come first")
        ELEMENT(3213, GenericPictureEssenceDescriptor_ImageStartOffset, "Unused bytes before start of stored data")
        ELEMENT(3214, GenericPictureEssenceDescriptor_ImageEndOffset,"Unused bytes before start of stored data")
        ELEMENT(3215, GenericPictureEssenceDescriptor_SignalStandard, "Underlying signal standard")
        ELEMENT(3216, GenericPictureEssenceDescriptor_StoredF2Offset, "Topness Adjustment for stored picture")
        ELEMENT(3217, GenericPictureEssenceDescriptor_DisplayF2Offset, "Topness Adjustment for Displayed Picture")
        ELEMENT(3218, GenericPictureEssenceDescriptor_ActiveFormatDescriptor, "Specifies the intended framing of the content within the displayed image")
        default: FileDescriptor();
    }

    if (Code2==0x3C0A) //InstanceUID
    {
        Descriptors[InstanceUID].StreamKind=Stream_Video;
        if (Streams_Count==(size_t)-1)
            Streams_Count=0;
        Streams_Count++;
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenericSoundEssenceDescriptor()
{
    //Parsing
    switch(Code2)
    {
        ELEMENT(3D01, GenericSoundEssenceDescriptor_QuantizationBits, "QuantizationBits")
        ELEMENT(3D02, GenericSoundEssenceDescriptor_Locked ,    "Locked ")
        ELEMENT(3D03, GenericSoundEssenceDescriptor_AudioSamplingRate, "AudioSamplingRate")
        ELEMENT(3D04, GenericSoundEssenceDescriptor_AudioRefLevel, "AudioRefLevel")
        ELEMENT(3D05, GenericSoundEssenceDescriptor_ElectroSpatialFormulation, "ElectroSpatialFormulation")
        ELEMENT(3D06, GenericSoundEssenceDescriptor_SoundEssenceCompression, "SoundEssenceCompression")
        ELEMENT(3D07, GenericSoundEssenceDescriptor_ChannelCount, "ChannelCount")
        ELEMENT(3D0C, GenericSoundEssenceDescriptor_DialNorm, "DialNorm")
        default: FileDescriptor();
    }

    if (Code2==0x3C0A) //InstanceUID
    {
        Descriptors[InstanceUID].StreamKind=Stream_Audio;
        if (Streams_Count==(size_t)-1)
            Streams_Count=0;
        Streams_Count++;
    }
}

//---------------------------------------------------------------------------
void File_Mxf::GenericTrack()
{
    //Parsing
    switch(Code2)
    {
        ELEMENT(4801, GenericTrack_TrackID,                     "TrackID")
        ELEMENT(4802, GenericTrack_TrackName,                   "TrackName")
        ELEMENT(4803, GenericTrack_Sequence,                    "Sequence")
        ELEMENT(4804, GenericTrack_TrackNumber,                 "TrackNumber")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::InterchangeObject()
{
    //Parsing
    switch(Code2)
    {
        ELEMENT(3C0A, InterchangeObject_InstanceUID,            "InstanceUID")
        default: ;
    }
}

//---------------------------------------------------------------------------
void File_Mxf::MaterialPackage()
{
    GenericPackage();

    if (Code2==0x3C0A)
    {
        if (InstanceUID==Preface_PrimaryPackage_Data) //InstanceIUD
        {
            Element_Level--;
            Element_Info("Primary package");
            Element_Level++;
        }
        for (contentstorages::iterator ContentStorage=ContentStorages.begin(); ContentStorage!=ContentStorages.end(); ContentStorage++)
        {
            for (size_t Pos=0; Pos<ContentStorage->second.Packages.size(); Pos++)
                if (InstanceUID==ContentStorage->second.Packages[Pos])
                {
                    Element_Level--;
                    Element_Info("Valid from Content storage");
                    Element_Level++;
                }
        }
    }
}

//---------------------------------------------------------------------------
void File_Mxf::MPEG2VideoDescriptor()
{
    std::map<int16u, int128u>::iterator Primer_Value=Primer_Values.find(Code2);
    if (Primer_Value==Primer_Values.end()) //if not a standard code or unknown user defined code
    {
        CDCIEssenceDescriptor();
        return;
    }

    int32u Code_Compare1=Primer_Value->second.hi>>32;
    int32u Code_Compare2=(int32u)Primer_Value->second.hi;
    int32u Code_Compare3=Primer_Value->second.lo>>32;
    int32u Code_Compare4=(int32u)Primer_Value->second.lo;
    if(0);
    ELEMENT_UUID(MPEG2VideoDescriptor_SingleSequence,           "Single sequence")
    ELEMENT_UUID(MPEG2VideoDescriptor_ConstantBFrames,          "Number of B frames always constant")
    ELEMENT_UUID(MPEG2VideoDescriptor_CodedContentType,         "Coded content type")
    ELEMENT_UUID(MPEG2VideoDescriptor_LowDelay,                 "Low delay")
    ELEMENT_UUID(MPEG2VideoDescriptor_ClosedGOP,                "Closed GOP")
    ELEMENT_UUID(MPEG2VideoDescriptor_IdenticalGOP,             "Identical GOP")
    ELEMENT_UUID(MPEG2VideoDescriptor_MaxGOP,                   "Maximum occurring spacing between I frames")
    ELEMENT_UUID(MPEG2VideoDescriptor_BPictureCount,            "Maximum number of B pictures between P or I frames")
    ELEMENT_UUID(MPEG2VideoDescriptor_ProfileAndLevel,          "Profile and level")
    ELEMENT_UUID(MPEG2VideoDescriptor_BitRate,                  "Maximum bit rate")
}

//---------------------------------------------------------------------------
void File_Mxf::MultipleDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3F01, MultipleDescriptor_SubDescriptorUIDs,     "SubDescriptorUIDs")
        default: FileDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::NetworkLocator()
{
    switch(Code2)
    {
        ELEMENT(4001, NetworkLocator_URLString,                 "A URL indicating where the essence may be found.")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::OpenHeader()
{
    //Parsing
    PartitionMetadata();
}

//---------------------------------------------------------------------------
void File_Mxf::Padding()
{
    //Parsing
    Skip_XX(Element_Size,                                       "Padding");
}

//---------------------------------------------------------------------------
void File_Mxf::Preface()
{
    switch(Code2)
    {
        ELEMENT(3B02, Preface_LastModifiedDate,                 "LastModifiedDate")
        ELEMENT(3B03, Preface_ContentStorage,                   "ContentStorage")
        ELEMENT(3B05, Preface_Version,                          "Version")
        ELEMENT(3B06, Preface_Identifications,                  "Identifications")
        ELEMENT(3B07, Preface_ObjectModelVersion,               "ObjectModelVersion")
        ELEMENT(3B08, Preface_PrimaryPackage,                   "PrimaryPackage")
        ELEMENT(3B09, Preface_OperationalPattern,               "OperationalPattern")
        ELEMENT(3B0A, Preface_EssenceContainers,                "EssenceContainers")
        ELEMENT(3B0B, Preface_DMSchemes,                        "DMSchemes")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::Primer()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("LocalTagEntryBatch", Length);
        int16u LocalTag;
        int128u UID;
        Get_B2 (LocalTag,                                       "LocalTag"); Element_Info(Ztring().From_CC2(LocalTag));
        Get_UL (UID,                                            "UID"); Element_Info(Ztring().From_UUID(UID));
        Element_End();

        FILLING_BEGIN();
            if (LocalTag>=0x8000) //user defined
                Primer_Values[LocalTag]=UID;
        FILLING_END();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::RGBAEssenceDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3401, RGBAEssenceDescriptor_PixelLayout,        "Pixel Layout")
        ELEMENT(3403, RGBAEssenceDescriptor_Palette,            "Palette")
        ELEMENT(3404, RGBAEssenceDescriptor_PaletteLayout,      "Palette Layout")
        ELEMENT(3405, RGBAEssenceDescriptor_ScanningDirection,  "Enumerated Scanning Direction")
        ELEMENT(3406, RGBAEssenceDescriptor_ComponentMaxRef,    "Maximum value for RGB components")
        ELEMENT(3407, RGBAEssenceDescriptor_ComponentMinRef,    "Minimum value for RGB components")
        ELEMENT(3408, RGBAEssenceDescriptor_AlphaMaxRef,        "Maximum value for alpha component")
        ELEMENT(3409, RGBAEssenceDescriptor_AlphaMinRef,        "Minimum value for alpha component")
        default: GenericPictureEssenceDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::RandomIndexMetadata()
{
    //Parsing
    while (Element_Offset+4<Element_Size)
    {
        Element_Begin("PartitionArray", 12);
        Info_B4(BodySID,                                        "BodySID"); Element_Info(BodySID);
        Info_B8(ByteOffset,                                     "ByteOffset"); Element_Info(Ztring::ToZtring(ByteOffset, 16));
        Element_End();
    }
    Skip_B4(                                                    "Length");
}

//---------------------------------------------------------------------------
void File_Mxf::Sequence()
{
    switch(Code2)
    {
        ELEMENT(1001, Sequence_StructuralComponents,            "StructuralComponents")
        default: StructuralComponent();
    }

    if (Code2==0x3C0A)
    {
        for (std::map<int128u, track>::iterator Track=Tracks.begin(); Track!=Tracks.end(); Track++)
        {
            if (InstanceUID==Track->second.Sequence)
            {
                Element_Level--;
                Element_Info("Valid from track");
                Element_Level++;
            }
        }
    }
}

//---------------------------------------------------------------------------
void File_Mxf::SourceClip()
{
    switch(Code2)
    {
        ELEMENT(1101, SourceClip_SourcePackageID,               "SourcePackageID")
        ELEMENT(1102, SourceClip_SourceTrackID,                 "SourceTrackID")
        ELEMENT(1201, SourceClip_StartPosition,                 "StartPosition")
        default: StructuralComponent();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::SourcePackage()
{
    switch(Code2)
    {
        //SourcePackage
        ELEMENT(4701, SourcePackage_Descriptor,                 "Descriptor")
        default: GenericPackage();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::StaticTrack()
{
    GenericTrack();
}

//---------------------------------------------------------------------------
void File_Mxf::StructuralComponent()
{
    switch(Code2)
    {
        ELEMENT(0201, StructuralComponent_DataDefinition,       "DataDefinition")
        ELEMENT(0202, StructuralComponent_Duration,             "Duration")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::TextLocator()
{
    switch(Code2)
    {
        ELEMENT(4101, TextLocator_LocatorName,                  "Human-readable locator text string for manual location of essence")
        default: GenerationInterchangeObject();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::TimecodeComponent()
{
    switch(Code2)
    {
        ELEMENT(1501, TimecodeComponent_StartTimecode,          "StartTimecode")
        ELEMENT(1502, TimecodeComponent_RoundedTimecodeBase,    "RoundedTimecodeBase")
        ELEMENT(1503, TimecodeComponent_DropFrame,              "DropFrame")
        default: StructuralComponent();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::WaveAudioDescriptor()
{
    switch(Code2)
    {
        ELEMENT(3D09, WaveAudioDescriptor_AvgBps,               "Average Bytes per second")
        ELEMENT(3D0A, WaveAudioDescriptor_BlockAlign,           "Sample Block alignment")
        ELEMENT(3D0B, WaveAudioDescriptor_SequenceOffset,       "Frame number of first essence")
        ELEMENT(3D29, WaveAudioDescriptor_PeakEnvelopeVersion,  "Peak envelope version information")
        ELEMENT(3D2A, WaveAudioDescriptor_PeakEnvelopeFormat,   "Format of a peak point")
        ELEMENT(3D2B, WaveAudioDescriptor_PointsPerPeakValue,   "Number of peak points per peak value")
        ELEMENT(3D2C, WaveAudioDescriptor_PeakEnvelopeBlockSize,"Number of audio samples used to generate each peak frame")
        ELEMENT(3D2D, WaveAudioDescriptor_PeakChannels,         "Number of peak channels")
        ELEMENT(3D2E, WaveAudioDescriptor_PeakFrames,           "Number of peak frames")
        ELEMENT(3D2F, WaveAudioDescriptor_PeakOfPeaksPosition,  "Offset to the first audio sample whose absolute value is the maximum value of the entire audio file")
        ELEMENT(3D30, WaveAudioDescriptor_PeakEnvelopeTimestamp,"Time stamp of the creation of the peak data")
        ELEMENT(3D31, WaveAudioDescriptor_PeakEnvelopeData ,    "Peak envelope data")
        ELEMENT(3D32, WaveAudioDescriptor_ChannelAssignment,    "Channel assignment in use")
        default: GenericSoundEssenceDescriptor();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::Track()
{
    //Parsing
    switch(Code2)
    {
        ELEMENT(4B01, Track_EditRate,                           "EditRate")
        ELEMENT(4B02, Track_Origin,                             "Origin")
        default: GenericTrack();
    }

    if (Code2==0x3C0A)
    {
        for (packages::iterator Package=Packages.begin(); Package!=Packages.end(); Package++)
        {
            if (Package->first==Preface_PrimaryPackage_Data) //InstanceIUD
            {
                Element_Level--;
                Element_Info("Primary package");
                Element_Level++;
            }
            for (size_t Pos=0; Pos<Package->second.Tracks.size(); Pos++)
                if (InstanceUID==Package->second.Tracks[Pos])
                {
                    Element_Level--;
                    Element_Info("Valid from Package");
                    Element_Level++;
                }
        }
    }
}

//***************************************************************************
// Base
//***************************************************************************

//---------------------------------------------------------------------------
// 0x3D08
void File_Mxf::AES3PCMDescriptor_AuxBitsMode()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D0D
void File_Mxf::AES3PCMDescriptor_Emphasis()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D0F
void File_Mxf::AES3PCMDescriptor_BlockStartOffset()
{
    //Parsing
    Info_B2(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D10
void File_Mxf::AES3PCMDescriptor_ChannelStatusMode()
{
    //Parsing
    Skip_XX(Length2,                                            "Batch");
}

//---------------------------------------------------------------------------
// 0x3D11
void File_Mxf::AES3PCMDescriptor_FixedChannelStatusData()
{
    //Parsing
    Skip_XX(Length2,                                           "Data");
}

//---------------------------------------------------------------------------
// 0x3D12
void File_Mxf::AES3PCMDescriptor_UserDataMode()
{
    //Parsing
    Skip_XX(Length2,                                           "Data");
}

//---------------------------------------------------------------------------
// 0x3D13
void File_Mxf::AES3PCMDescriptor_FixedUserData()
{
    //Parsing
    Skip_XX(Length2,                                           "Data");
}

//---------------------------------------------------------------------------
// 0x3301
void File_Mxf::CDCIEssenceDescriptor_ComponentDepth()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Resolution"].From_Number(Data*3);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3302
void File_Mxf::CDCIEssenceDescriptor_HorizontalSubsampling()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3303
void File_Mxf::CDCIEssenceDescriptor_ColorSiting()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3304
void File_Mxf::CDCIEssenceDescriptor_BlackRefLevel()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3305
void File_Mxf::CDCIEssenceDescriptor_WhiteReflevel()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3306
void File_Mxf::CDCIEssenceDescriptor_ColorRange()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3307
void File_Mxf::CDCIEssenceDescriptor_PaddingBits()
{
    //Parsing
    Info_B2(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3308
void File_Mxf::CDCIEssenceDescriptor_VerticalSubsampling()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3309
void File_Mxf::CDCIEssenceDescriptor_AlphaSampleDepth()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x330B
void File_Mxf::CDCIEssenceDescriptor_ReversedByteOrder()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1901
void File_Mxf::ContentStorage_Packages()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Package", Length);
        int128u Data;
        Get_UL (Data,                                           "Data");

        FILLING_BEGIN();
            if (Data==Preface_PrimaryPackage_Data)
                Element_Info("Primary package");
            ContentStorages[InstanceUID].Packages.push_back(Data);
        FILLING_END();

        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x1902
void File_Mxf::ContentStorage_EssenceContainerData()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Info_UL (EssenceContainer,                              "EssenceContainer"); Param_Info(Mxf_EssenceContainer(EssenceContainer));
    }
}

//---------------------------------------------------------------------------
// 0x2701
void File_Mxf::EssenceContainerData_LinkedPackageUID()
{
    //Parsing
    Skip_UMID();
}

//---------------------------------------------------------------------------
// 0x3F06
void File_Mxf::EssenceContainerData_IndexSID()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F07
void File_Mxf::EssenceContainerData_BodySID()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x4901
void File_Mxf::EventTrack_EventEditRate()
{
    //Parsing
    Info_Rational();
}

//---------------------------------------------------------------------------
// 0x4902
void File_Mxf::EventTrack_EventOrigin()
{
    //Parsing
    Info_B8(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3001
void File_Mxf::FileDescriptor_SampleRate()
{
    //Parsing
    Get_Rational(Descriptors[InstanceUID].SampleRate); Element_Info(Descriptors[InstanceUID].SampleRate);

    FILLING_BEGIN();
        switch (Descriptors[InstanceUID].StreamKind)
        {
            case Stream_Video   : Descriptors[InstanceUID].Infos["FrameRate"]=Ztring().From_Number(Descriptors[InstanceUID].SampleRate, 3); break;
            default             : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3002
void File_Mxf::FileDescriptor_ContainerDuration()
{
    //Parsing
    int64u Data;
    Get_B8 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        float32 SampleRate=Descriptors[InstanceUID].SampleRate;
        if (SampleRate)
            Descriptors[InstanceUID].Infos["Duration"].From_Number(Data/SampleRate*1000);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3004
void File_Mxf::FileDescriptor_EssenceContainer()
{
    //Parsing
    Info_UL(EssenceContainer,                                   "EssenceContainer"); Element_Info(Mxf_EssenceContainer(EssenceContainer));
}

//---------------------------------------------------------------------------
// 0x3005
void File_Mxf::FileDescriptor_Codec()
{
    //Parsing
    Skip_UL(                                                    "UUID");
}

//---------------------------------------------------------------------------
// 0x3006
void File_Mxf::FileDescriptor_LinkedTrackID()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].LinkedTrackID=Data;
        Descriptors[InstanceUID].Infos["ID"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C0A
void File_Mxf::InterchangeObject_InstanceUID()
{
    //Parsing
    Get_UUID(InstanceUID,                                       "UUID");
}

//---------------------------------------------------------------------------
// 0x0102
void File_Mxf::GenerationInterchangeObject_GenerationUID()
{
    //Parsing
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
// 0x2F01
void File_Mxf::GenericDescriptor_Locators()
{
    //Parsing
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
// 0x4401
void File_Mxf::GenericPackage_PackageUID()
{
    //Parsing
    Skip_UMID();
}

//---------------------------------------------------------------------------
// 0x4402
void File_Mxf::GenericPackage_Name()
{
    //Parsing
    Info_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x4403
void File_Mxf::GenericPackage_Tracks()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Track", Length);
        int128u Data;
        Get_UUID(Data,                                          "Data");

        FILLING_BEGIN();
            Packages[InstanceUID].Tracks.push_back(Data);
        FILLING_END();

        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x4404
void File_Mxf::GenericPackage_PackageModifiedDate()
{
    //Parsing
    Info_Timestamp();
}

//---------------------------------------------------------------------------
// 0x4405
void File_Mxf::GenericPackage_PackageCreationDate()
{
    //Parsing
    Info_Timestamp();
}

//---------------------------------------------------------------------------
// 0x3201
void File_Mxf::GenericPictureEssenceDescriptor_PictureEssenceCoding()
{
    //Parsing
    int128u Data;
    Get_UL(Data,                                                "Data"); Element_Info(Mxf_EssenceCoding(Data));

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Format"]=Mxf_EssenceCoding_Format(Data);
        Descriptors[InstanceUID].Infos["Format_Version"]=Mxf_EssenceCoding_Format_Version(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3202
void File_Mxf::GenericPictureEssenceDescriptor_StoredHeight()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        if (Descriptors[InstanceUID].Infos["ScanType"]==_T("Interlaced"))
            Data*=2; //This is per field
        Descriptors[InstanceUID].Infos["Height"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3203
void File_Mxf::GenericPictureEssenceDescriptor_StoredWidth()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Width"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3204
void File_Mxf::GenericPictureEssenceDescriptor_SampledHeight()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        if (Descriptors[InstanceUID].Infos["ScanType"]==_T("Interlaced"))
            Data*=2; //This is per field
        Descriptors[InstanceUID].Infos["Height"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3205
void File_Mxf::GenericPictureEssenceDescriptor_SampledWidth()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Width"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3206
void File_Mxf::GenericPictureEssenceDescriptor_SampledXOffset()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3207
void File_Mxf::GenericPictureEssenceDescriptor_SampledYOffset()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3208
void File_Mxf::GenericPictureEssenceDescriptor_DisplayHeight()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3209
void File_Mxf::GenericPictureEssenceDescriptor_DisplayWidth()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x320A
void File_Mxf::GenericPictureEssenceDescriptor_DisplayXOffset()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x320B
void File_Mxf::GenericPictureEssenceDescriptor_DisplayYOffset()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x320C
void File_Mxf::GenericPictureEssenceDescriptor_FrameLayout()
{
    //Parsing
    int8u Data;
    Get_B1 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["ScanType"]=Data?"Interlaced":"PPF";
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x320D
void File_Mxf::GenericPictureEssenceDescriptor_VideoLineMap()
{
    //Parsing
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Skip_B4(                                                "VideoLineMapEntry");
    }
}

//---------------------------------------------------------------------------
// 0x320E
void File_Mxf::GenericPictureEssenceDescriptor_AspectRatio()
{
    //Parsing
    float32 Data;
    Get_Rational(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["DisplayAspectRatio"].From_Number(Data, 3);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x320F
void File_Mxf::GenericPictureEssenceDescriptor_AlphaTransparency()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3210
void File_Mxf::GenericPictureEssenceDescriptor_Gamma()
{
    //Parsing
    Skip_UL(                                                    "Data");
}

//---------------------------------------------------------------------------
// 0x3211
void File_Mxf::GenericPictureEssenceDescriptor_ImageAlignmentOffset()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3212
void File_Mxf::GenericPictureEssenceDescriptor_FieldDominance()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3213
void File_Mxf::GenericPictureEssenceDescriptor_ImageStartOffset()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3214
void File_Mxf::GenericPictureEssenceDescriptor_ImageEndOffset()
{
    //Parsing
    Info_B4(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3215
void File_Mxf::GenericPictureEssenceDescriptor_SignalStandard()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3216
void File_Mxf::GenericPictureEssenceDescriptor_StoredF2Offset()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3217
void File_Mxf::GenericPictureEssenceDescriptor_DisplayF2Offset()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3218
void File_Mxf::GenericPictureEssenceDescriptor_ActiveFormatDescriptor()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D01
void File_Mxf::GenericSoundEssenceDescriptor_QuantizationBits()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Resolution"].From_Number(Data);
    FILLING_END();

}

//---------------------------------------------------------------------------
// 0x3D02
void File_Mxf::GenericSoundEssenceDescriptor_Locked()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x3D03
void File_Mxf::GenericSoundEssenceDescriptor_AudioSamplingRate()
{
    //Parsing
    float32 Data;
    Get_Rational(Data); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["SamplingRate"].From_Number(Data, 0);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3D04
void File_Mxf::GenericSoundEssenceDescriptor_AudioRefLevel()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data, " dB");
}

//---------------------------------------------------------------------------
// 0x3D05
void File_Mxf::GenericSoundEssenceDescriptor_ElectroSpatialFormulation()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data); //Enum
}

//---------------------------------------------------------------------------
// 0x3D06
void File_Mxf::GenericSoundEssenceDescriptor_SoundEssenceCompression()
{
    //Parsing
    int128u Data;
    Get_UL(Data,                                                "Data"); Element_Info(Mxf_EssenceContainer(Data));

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Format"]=Mxf_EssenceCoding_Format(Data);
        Descriptors[InstanceUID].Infos["Format_Version"]=Mxf_EssenceCoding_Format_Version(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3D07
void File_Mxf::GenericSoundEssenceDescriptor_ChannelCount()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["Channel(s)"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3D0C
void File_Mxf::GenericSoundEssenceDescriptor_DialNorm()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data, " dB");
}

//---------------------------------------------------------------------------
// 0x4801
void File_Mxf::GenericTrack_TrackID()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Tracks[InstanceUID].TrackID=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x4802
void File_Mxf::GenericTrack_TrackName()
{
    //Parsing
    Info_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x4803
void File_Mxf::GenericTrack_Sequence()
{
    //Parsing
    int128u Data;
    Get_UUID(Data,                                              "Data");

    FILLING_BEGIN();
        Tracks[InstanceUID].Sequence=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x4804
void File_Mxf::GenericTrack_TrackNumber()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                                "Data"); Element_Info(Ztring::ToZtring(Data, 16));

    FILLING_BEGIN();
        Tracks[InstanceUID].TrackNumber=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C01
void File_Mxf::Identification_CompanyName()
{
    //Parsing
    Ztring Data;
    Get_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Identifications[InstanceUID].CompanyName=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C02
void File_Mxf::Identification_ProductName()
{
    //Parsing
    Ztring Data;
    Get_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Identifications[InstanceUID].ProductName=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C03
void File_Mxf::Identification_ProductVersion()
{
    //Parsing
    int16u Major, Minor, Patch, Build, Release;
    Get_B2 (Major,                                              "Major");
    Get_B2 (Minor,                                              "Minor");
    Get_B2 (Patch,                                              "Patch");
    Get_B2 (Build,                                              "Build");
    Get_B2 (Release,                                            "Release");
    Ztring Version=Ztring::ToZtring(Major)+_T('.')
                  +Ztring::ToZtring(Minor)+_T('.')
                  +Ztring::ToZtring(Patch)+_T('.')
                  +Ztring::ToZtring(Build)+_T('.')
                  +Ztring::ToZtring(Release)      ;
    Element_Info(Version);

    FILLING_BEGIN();
        Identifications[InstanceUID].ProductVersion=Version;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C04
void File_Mxf::Identification_VersionString()
{
    //Parsing
    Ztring Data;
    Get_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Identifications[InstanceUID].VersionString=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3C05
void File_Mxf::Identification_ProductUID()
{
    //Parsing
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
// 0x3C06
void File_Mxf::Identification_ModificationDate()
{
    //Parsing
    Info_Timestamp();
}

//---------------------------------------------------------------------------
// 0x3C07
void File_Mxf::Identification_ToolkitVersion()
{
    //Parsing
    //Parsing
    Info_B2(Major,                                              "Major");
    Info_B2(Minor,                                              "Minor");
    Info_B2(Patch,                                              "Patch");
    Info_B2(Build,                                              "Build");
    Info_B2(Release,                                            "Release");
    Element_Info(Ztring::ToZtring(Major)+_T('.')
                +Ztring::ToZtring(Minor)+_T('.')
                +Ztring::ToZtring(Patch)+_T('.')
                +Ztring::ToZtring(Build)+_T('.')
                +Ztring::ToZtring(Release)      );
}

//---------------------------------------------------------------------------
// 0x3C08
void File_Mxf::Identification_Platform()
{
    //Parsing
    Info_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3C09
void File_Mxf::Identification_ThisGenerationUID()
{
    //Parsing
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
// 0x3F05
void File_Mxf::IndexTableSegment_EditUnitByteCount()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F06
void File_Mxf::IndexTableSegment_IndexSID()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F07
void File_Mxf::IndexTableSegment_BodySID()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F08
void File_Mxf::IndexTableSegment_SliceCount()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F0B
void File_Mxf::IndexTableSegment_IndexEditRate()
{
    //Parsing
    Info_Rational();
}

//---------------------------------------------------------------------------
// 0x3F0C
void File_Mxf::IndexTableSegment_IndexStartPosition()
{
    //Parsing
    Info_B8(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F0D
void File_Mxf::IndexTableSegment_IndexDuration()
{
    //Parsing
    Info_B8(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F0E
void File_Mxf::IndexTableSegment_PosTableCount()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8001
void File_Mxf::JPEG2000PictureSubDescriptor_Rsiz()
{
    //Parsing
    Info_B2(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8002
void File_Mxf::JPEG2000PictureSubDescriptor_Xsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8003
void File_Mxf::JPEG2000PictureSubDescriptor_Ysiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8004
void File_Mxf::JPEG2000PictureSubDescriptor_XOsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8005
void File_Mxf::JPEG2000PictureSubDescriptor_YOsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8006
void File_Mxf::JPEG2000PictureSubDescriptor_XTsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8007
void File_Mxf::JPEG2000PictureSubDescriptor_YTsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8008
void File_Mxf::JPEG2000PictureSubDescriptor_XTOsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x8009
void File_Mxf::JPEG2000PictureSubDescriptor_YTOsiz()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x800A
void File_Mxf::JPEG2000PictureSubDescriptor_Csiz()
{
    //Parsing
    Info_B2(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x800B
void File_Mxf::JPEG2000PictureSubDescriptor_PictureComponentSizing()
{
    //Parsing
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("PictureComponentSize", Length);
        Info_B1(Ssiz,                                           "Component sample precision"); Element_Info(Ssiz);
        Info_B1(XRsiz,                                          "Horizontal separation of a sample"); Element_Info(XRsiz);
        Info_B1(YRsiz,                                          "Vertical separation of a sample"); Element_Info(YRsiz);
        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x3B02
void File_Mxf::Preface_LastModifiedDate()
{
    //Parsing
    Ztring Value;
    Get_Timestamp(Value); Element_Info(Value);

    FILLING_BEGIN();
        Fill(Stream_General, 0, General_Encoded_Date, Value, true);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_SingleSequence()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_ConstantBFrames()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_CodedContentType()
{
    //Parsing
    int8u Data;
    Get_B1 (Data,                                               "Data"); Element_Info(Mxf_MPEG2_CodedContentType(Data));

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["ScanType"]=Mxf_MPEG2_CodedContentType(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_LowDelay()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_ClosedGOP()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_IdenticalGOP()
{
    //Parsing
    Info_B1(Data,                                               "Data"); Element_Info(Data?"Yes":"No");
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_MaxGOP()
{
    //Parsing
    Info_B2(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_BPictureCount()
{
    //Parsing
    Info_B2(Data,                                               "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_ProfileAndLevel()
{
    //Parsing
    int8u profile_and_level_indication_profile, profile_and_level_indication_level;
    BS_Begin();
    Skip_SB(                                                    "profile_and_level_indication_escape");
    Get_S1 ( 3, profile_and_level_indication_profile,           "profile_and_level_indication_profile"); Param_Info(Mpegv_profile_and_level_indication_profile[profile_and_level_indication_profile]);
    Get_S1 ( 4, profile_and_level_indication_level,             "profile_and_level_indication_level"); Param_Info(Mpegv_profile_and_level_indication_level[profile_and_level_indication_level]);
    BS_End();

    FILLING_BEGIN();
        if (profile_and_level_indication_profile && profile_and_level_indication_level)
            Descriptors[InstanceUID].Infos["Format_Profile"]=Ztring().From_Local(Mpegv_profile_and_level_indication_profile[profile_and_level_indication_profile])+_T("@")+Ztring().From_Local(Mpegv_profile_and_level_indication_level[profile_and_level_indication_level]);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x
void File_Mxf::MPEG2VideoDescriptor_BitRate()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["BitRate"].From_Number(Data);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x4001
void File_Mxf::NetworkLocator_URLString()
{
    //Parsing
    Info_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3F01
void File_Mxf::MultipleDescriptor_SubDescriptorUIDs()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        //Parsing
        int128u Data;
        Get_UUID(Data,                                          "UUID");

        FILLING_BEGIN();
            Descriptors[InstanceUID].SubDescriptors.push_back(Data);
        FILLING_END();
    }
}

//---------------------------------------------------------------------------
void File_Mxf::PartitionMetadata()
{
    //Parsing
    Skip_B2(                                                    "MajorVersion");
    Skip_B2(                                                    "MinorVersion");
    Skip_B4(                                                    "KAGSize");
    Skip_B8(                                                    "ThisPartition");
    Skip_B8(                                                    "PreviousPartition");
    Skip_B8(                                                    "FooterPartition");
    Skip_B8(                                                    "HeaderByteCount");
    Skip_B8(                                                    "IndexByteCount");
    Skip_B4(                                                    "IndexSID");
    Skip_B8(                                                    "BodyOffset");
    Skip_B4(                                                    "BodySID");
    Info_UL(OperationalPattern,                                 "OperationalPattern"); Param_Info(Mxf_OperationalPattern(OperationalPattern));

    Element_Begin("EssenceContainers"); //Vector
        int32u Count, Length;
        Get_B4 (Count,                                          "Count");
        Get_B4 (Length,                                         "Length");
        for (int32u Pos=0; Pos<Count; Pos++)
        {
            Info_UL(EssenceContainer,                           "EssenceContainer"); Param_Info(Mxf_EssenceContainer(EssenceContainer));
        }
    Element_End();
}

//---------------------------------------------------------------------------
// 0x3B03
void File_Mxf::Preface_ContentStorage()
{
    //Parsing
    int128u Data;
    Get_UUID(Data,                                              "Data");

    FILLING_BEGIN();
        Preface_ContentStorage_Data=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3B05
void File_Mxf::Preface_Version()
{
    //Parsing
    Info_B1(Major,                                              "Major"); //1
    Info_B1(Minor,                                              "Minor"); //2
    Element_Info(Ztring::ToZtring(Major)+_T('.')+Ztring::ToZtring(Minor));
}

//---------------------------------------------------------------------------
// 0x3B06
void File_Mxf::Preface_Identifications()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("Identification", Length);
        Skip_UUID(                                              "UUID");
        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x3B07
void File_Mxf::Preface_ObjectModelVersion()
{
    //Parsing
    Skip_B4(                                                    "Data");
}

//---------------------------------------------------------------------------
// 0x3B08
void File_Mxf::Preface_PrimaryPackage()
{
    //Parsing
    int128u Data;
    Get_UUID(Data,                                              "Data");

    FILLING_BEGIN();
        Preface_PrimaryPackage_Data=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3B09
void File_Mxf::Preface_OperationalPattern()
{
    //Parsing
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
// 0x3B0A
void File_Mxf::Preface_EssenceContainers()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Info_UL(EssenceContainer,                               "EssenceContainer"); Param_Info(Mxf_EssenceContainer(EssenceContainer));
    }
}

//---------------------------------------------------------------------------
// 0x3B0B
void File_Mxf::Preface_DMSchemes()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("DMScheme", Length);
        Skip_UL(                                                "UUID");
        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x3401
void File_Mxf::RGBAEssenceDescriptor_PixelLayout()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3403
void File_Mxf::RGBAEssenceDescriptor_Palette()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3404
void File_Mxf::RGBAEssenceDescriptor_PaletteLayout()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3405
void File_Mxf::RGBAEssenceDescriptor_ScanningDirection()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3406
void File_Mxf::RGBAEssenceDescriptor_ComponentMaxRef()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3407
void File_Mxf::RGBAEssenceDescriptor_ComponentMinRef()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3408
void File_Mxf::RGBAEssenceDescriptor_AlphaMaxRef()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3409
void File_Mxf::RGBAEssenceDescriptor_AlphaMinRef()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1001
void File_Mxf::Sequence_StructuralComponents()
{
    //Parsing
    //Vector
    int32u Count, Length;
    Get_B4 (Count,                                              "Count");
    Get_B4 (Length,                                             "Length");
    for (int32u Pos=0; Pos<Count; Pos++)
    {
        Element_Begin("StructuralComponent", Length);
        Skip_UUID(                                              "UUID");
        Element_End();
    }
}

//---------------------------------------------------------------------------
// 0x1101
void File_Mxf::SourceClip_SourcePackageID()
{
    //Parsing
    Skip_UMID();
}

//---------------------------------------------------------------------------
// 0x1102
void File_Mxf::SourceClip_SourceTrackID()
{
    //Parsing
    Info_B4(Data,                                                "ID"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1201
void File_Mxf::SourceClip_StartPosition()
{
    //Parsing
    Info_B8(Duration,                                           "Duration"); Element_Info(Duration); //units of edit rate
}

//---------------------------------------------------------------------------
// 0x4701
void File_Mxf::SourcePackage_Descriptor()
{
    //Parsing
    int128u Data;
    Get_UUID(Data,                                              "Data");

    FILLING_BEGIN();
        Packages[InstanceUID].Descriptor=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x0201
void File_Mxf::StructuralComponent_DataDefinition()
{
    //Parsing
    Info_UL(Data,                                               "Data"); Element_Info(Mxf_Sequence_DataDefinition(Data));
}

//---------------------------------------------------------------------------
// 0x0202
void File_Mxf::StructuralComponent_Duration()
{
    //Parsing
    int64u Data;
    Get_B8 (Data,                                               "Data"); Element_Info(Data); //units of edit rate

    FILLING_BEGIN();
        Components[InstanceUID].Duration=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x4101
void File_Mxf::TextLocator_LocatorName()
{
    //Parsing
    Info_UTF16B(Length2, Data,                                  "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1501
void File_Mxf::TimecodeComponent_StartTimecode()
{
    //Parsing
    Info_B8(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1502
void File_Mxf::TimecodeComponent_RoundedTimecodeBase()
{
    //Parsing
    Info_B2(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x1503
void File_Mxf::TimecodeComponent_DropFrame()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x4B01
void File_Mxf::Track_EditRate()
{
    //Parsing
    float32 Data;
    Get_Rational(Data); Element_Info(Data);

    FILLING_BEGIN();
        Tracks[InstanceUID].EditRate=Data;
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x4B02
void File_Mxf::Track_Origin()
{
    //Parsing
    Info_B8(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D09
void File_Mxf::WaveAudioDescriptor_AvgBps()
{
    //Parsing
    int32u Data;
    Get_B4 (Data,                                               "Data"); Element_Info(Data);

    FILLING_BEGIN();
        Descriptors[InstanceUID].Infos["BitRate"].From_Number(Data*8);
    FILLING_END();
}

//---------------------------------------------------------------------------
// 0x3D0A
void File_Mxf::WaveAudioDescriptor_BlockAlign()
{
    //Parsing
    Info_B2(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D0B
void File_Mxf::WaveAudioDescriptor_SequenceOffset()
{
    //Parsing
    Info_B1(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D29
void File_Mxf::WaveAudioDescriptor_PeakEnvelopeVersion()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2A
void File_Mxf::WaveAudioDescriptor_PeakEnvelopeFormat()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2B
void File_Mxf::WaveAudioDescriptor_PointsPerPeakValue()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2C
void File_Mxf::WaveAudioDescriptor_PeakEnvelopeBlockSize()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2D
void File_Mxf::WaveAudioDescriptor_PeakChannels()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2E
void File_Mxf::WaveAudioDescriptor_PeakFrames()
{
    //Parsing
    Info_B4(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D2F
void File_Mxf::WaveAudioDescriptor_PeakOfPeaksPosition()
{
    //Parsing
    Info_B8(Data,                                                "Data"); Element_Info(Data);
}

//---------------------------------------------------------------------------
// 0x3D30
void File_Mxf::WaveAudioDescriptor_PeakEnvelopeTimestamp()
{
    //Parsing
    Info_Timestamp();
}

//---------------------------------------------------------------------------
// 0x3D31
void File_Mxf::WaveAudioDescriptor_PeakEnvelopeData()
{
    //Parsing
    Skip_XX(Length2,                                            "Data");
}

//---------------------------------------------------------------------------
// 0x3D32
void File_Mxf::WaveAudioDescriptor_ChannelAssignment()
{
    //Parsing
    Skip_B16(                                                   "Label");
}

//***************************************************************************
// Basic types
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mxf::Get_Rational(float32 &Value)
{
    //Parsing
    int32u N, D;
    Get_B4 (N,                                                  "Numerator");
    Get_B4 (D,                                                  "Denominator");
    if (D)
        Value=((float32)N)/D;
    else
        Value=0; //Error
}

//---------------------------------------------------------------------------
void File_Mxf::Skip_Rational()
{
    //Parsing
    Skip_B4(                                                    "Numerator");
    Skip_B4(                                                    "Denominator");
}

//---------------------------------------------------------------------------
void File_Mxf::Info_Rational()
{
    //Parsing
    Info_B4(N,                                                  "Numerator");
    Info_B4(D,                                                  "Denominator");
    if (D)
        Element_Info(((float32)N)/D);
}

//---------------------------------------------------------------------------
void File_Mxf::Get_UL(int128u &Value, const char* Name)
{
    #ifdef MEDIAINFO_MINIMIZE_SIZE
        Skip_UUID();
    #else
    //Parsing
    Element_Begin(Name);
    int64u Value_hi, Value_lo;
    Peek_B8(Value_hi);
    Skip_B1(                                                    "Start (0x06)");
    Skip_B1(                                                    "Length of the remaining key (0x0E)");
    Skip_B1(                                                    "ISO, ORG (0x2B)");
    Skip_B1(                                                    "SMPTE (0x34)");
    Info_B1(Category,                                           "Category"); Param_Info(Mxf_Category(Category));
    Info_B1(RegistryDesignator,                                 "RegistryDesignator"); Param_Info(Mxf_RegistryDesignator(Category, RegistryDesignator));
    Skip_B1(                                                    "0x01");
    Skip_B1(                                                    "Version");
    Peek_B8(Value_lo);
    Info_B1(Code1,                                              "Code (1)");
    switch (Code1)
    {
        case 0x01 :
            {
            Info_B1(Code2,                                      "Code (2)");
            switch (Code2)
            {
                case 0x03 :
                    {
                    Info_B1(Code3,                              "Code (3)");
                    switch (Code3)
                    {
                        case 0x02 :
                            Skip_B5(                            "Track identifier");
                            break;
                        default   :
                            Skip_B5(                            "Unknown");
                    }
                    }
                    break;
                default   :
                    Skip_B6(                                    "Unknown");
            }
            }
            break;
        case 0x04 :
            {
            Info_B1(Code2,                                      "Code (2)");
            switch (Code2)
            {
                case 0x01 :
                    {
                    Info_B1(Code3,                              "Code (3)");
                    switch (Code3)
                    {
                        case 0x02 :
                            Skip_B5(                            "Picture coding or compression");
                            break;
                        default   :
                            Skip_B5(                            "Unknown");
                    }
                    }
                    break;
                case 0x02 :
                    {
                    Info_B1(Code3,                              "Code (3)");
                    switch (Code3)
                    {
                        case 0x02 :
                            Skip_B5(                            "Sound coding or compression");
                            break;
                        default   :
                            Skip_B5(                            "Unknown");
                    }
                    }
                    break;
                default   :
                    Skip_B6(                                    "Unknown");
            }
            }
            break;
        case 0x0D :
            {
            Info_B1(Code2,                                      "Code (2)");
            switch (Code2)
            {
                case 0x01 :
                    {
                    Info_B1(Code3,                              "Code (3)");
                    switch (Code3)
                    {
                        case 0x02 :
                            {
                            Info_B1(Code4,                      "Code (4)");
                            switch (Code4)
                            {
                                case 0x01 :
                                    Skip_B4(                    "Operational pattern");
                                    break;
                                case 0x02 :
                                    {
                                    Info_B1(Code5,              "Code (5)");
                                    switch (Code5)
                                    {
                                        case 0x01 :
                                            Skip_B3(            "Essence container kind");
                                            break;
                                        default   :
                                            Skip_B3(            "Unknown");
                                    }
                                    }
                                    break;
                                default   :
                                    Skip_B4(                    "Unknown");
                            }
                            }
                            break;
                        case 0x03 :
                            {
                            Info_B1(Code4,                      "Code (4)");
                            switch (Code4)
                            {
                                case 0x01 :
                                    Skip_B1(                    "Format?");
                                    Skip_B1(                    "Essence element count");
                                    Skip_B1(                    "Essence element type");
                                    Skip_B1(                    "Essence element number");
                                    break;
                                default   :
                                    Skip_B4(                    "Unknown");
                            }
                            }
                            break;
                        default   :
                            Skip_B5(                            "Unknown");
                    }
                    }
                    break;
                default   :
                    Skip_B6(                                    "Unknown");
            }
            }
            break;
        default   :
            Skip_B7(                                            "Unknown");
    }
    Element_End();

    Value.hi=Value_hi;
    Value.lo=Value_lo;
    #endif
}

//---------------------------------------------------------------------------
void File_Mxf::Skip_UL(const char* Name)
{
    #ifdef MEDIAINFO_MINIMIZE_SIZE
        Skip_UUID();
    #else
        int128u Value;
        Get_UL(Value, Name);
    #endif
}

//---------------------------------------------------------------------------
void File_Mxf::Skip_UMID()
{
    //Parsing
    Skip_UUID(                                                  "Fixed");
    Skip_UUID(                                                  "UUID");
}

//---------------------------------------------------------------------------
void File_Mxf::Get_Timestamp(Ztring &Value)
{
    //Parsing
    Info_B2(Year,                                               "Year");
    Info_B1(Month,                                              "Month");
    Info_B1(Day,                                                "Day");
    Info_B1(Hours,                                              "Hours");
    Info_B1(Minutes,                                            "Minutes");
    Info_B1(Seconds,                                            "Seconds");
    Info_B1(Milliseconds,                                       "Milliseconds/4"); Param_Info(Milliseconds*4, " ms");
    Value.From_Number(Year);
    Value+=_T('-');
    Ztring Temp;
    Temp.From_Number(Month);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
    Value+=_T('-');
    Temp.From_Number(Day);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
    Value+=_T(' ');
    Temp.From_Number(Hours);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
    Value+=_T(':');
    Temp.From_Number(Minutes);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
    Value+=_T(':');
    Temp.From_Number(Seconds);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
    Value+=_T('.');
    Temp.From_Number(Milliseconds*4);
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    if (Temp.size()<2)
        Temp.insert(Temp.begin(), _T('0'));
    Value+=Temp;
}

//---------------------------------------------------------------------------
void File_Mxf::Skip_Timestamp()
{
    //Parsing
    Skip_B2(                                                    "Year");
    Skip_B1(                                                    "Month");
    Skip_B1(                                                    "Day");
    Skip_B1(                                                    "Hours");
    Skip_B1(                                                    "Minutes");
    Skip_B1(                                                    "Seconds");
    Info_B1(Milliseconds,                                       "Milliseconds/4"); Param_Info(Milliseconds*4, " ms");
}

//---------------------------------------------------------------------------
void File_Mxf::Info_Timestamp()
{
    //Parsing
    Info_B2(Year,                                               "Year");
    Info_B1(Month,                                              "Month");
    Info_B1(Day,                                                "Day");
    Info_B1(Hours,                                              "Hours");
    Info_B1(Minutes,                                            "Minutes");
    Info_B1(Seconds,                                            "Seconds");
    Info_B1(Milliseconds,                                       "Milliseconds/4"); Param_Info(Milliseconds*4, " ms");
    Element_Info(Ztring::ToZtring(Year          )+_T('-')+
                 Ztring::ToZtring(Month         )+_T('-')+
                 Ztring::ToZtring(Day           )+_T(' ')+
                 Ztring::ToZtring(Hours         )+_T(':')+
                 Ztring::ToZtring(Minutes       )+_T(':')+
                 Ztring::ToZtring(Seconds       )+_T('.')+
                 Ztring::ToZtring(Milliseconds*4)         );
}

} //NameSpace

#endif //MEDIAINFO_MXF_*

