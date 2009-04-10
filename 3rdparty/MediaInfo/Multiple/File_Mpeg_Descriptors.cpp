// File_Mpeg_Descriptors - Info for MPEG files
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
#if defined(MEDIAINFO_MPEGPS_YES) || defined(MEDIAINFO_MPEGTS_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Multiple/File_Mpeg_Descriptors.h"
#ifdef MEDIAINFO_MPEG4_YES
    #include "MediaInfo/Multiple/File_Mpeg4_Descriptors.h"
#endif
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Constants
//***************************************************************************

namespace Elements
{
    const int32u AC_3=0x41432D33; //Exactly AC-3
    const int32u BSSD=0x42535344; //PCM
    const int32u CUEI=0x43554549; //SCTE
    const int32u DTS1=0x44545331; //DTS
    const int32u DTS2=0x44545332; //DTS
    const int32u DTS3=0x44545333; //DTS
    const int32u GA94=0x47413934; //ATSC - Terrestrial
    const int32u HDMV=0x48444D56; //BluRay
    const int32u S14A=0x53313441; //ATSC - Satellite
    const int32u SCTE=0x53435445; //SCTE
    const int32u TSHV=0x54534856; //TSHV
    const int32u VC_1=0x56432D31; //Exactly VC-1
    const int32u drac=0x64726163; //Dirac

    const int32u DVB =0x00000001; //Forced value, does not exist is stream
}

//***************************************************************************
// Infos
//***************************************************************************

//---------------------------------------------------------------------------
//Extern
extern const char* Avc_profile_idc(int8u profile_idc);

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_audio_type(int8u ID)
{
    switch (ID)
    {
        case 0x00 : return "Undefined";
        case 0x01 : return "Clean effects";
        case 0x02 : return "Hearing impaired";
        case 0x03 : return "Visual impaired commentary";
        default   : return "Reserved";
    }
}

const char* Mpeg_Descriptors_alignment_type(int8u alignment_type)
{
    switch (alignment_type)
    {
        case 0x01 : return "Slice or video access unit (Video), or sync word (Audio)";
        case 0x02 : return "Video access unit";
        case 0x03 : return "GOP, or SEQ";
        case 0x04 : return "SEQ";
        default   : return "Reserved";
    }
}

const char* Mpeg_Descriptors_teletext_type(int8u teletext_type)
{
    switch (teletext_type)
    {
        case 0x01 : return "initial Teletext page";
        case 0x02 : return "Teletext subtitle page";
        case 0x03 : return "additional information page";
        case 0x04 : return "programme schedule page";
        case 0x05 : return "Teletext subtitle page for hearing impaired people";
        default   : return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_1(int8u content_nibble_level_1)
{
    switch (content_nibble_level_1)
    {
        case 0x00 : return "undefined";
        case 0x01 : return "movie/drama";
        case 0x02 : return "news/current affairs";
        case 0x03 : return "show/game show";
        case 0x04 : return "sports";
        case 0x05 : return "children's/youth programmes";
        case 0x06 : return "music/ballet/dance";
        case 0x07 : return "arts/culture (without music)";
        case 0x08 : return "social/political issues/economics";
        case 0x09 : return "education/science/factual topics";
        case 0x0A : return "leisure hobbies";
        case 0x0B : return "Special characteristics:";
        default   :
            if (content_nibble_level_1==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_01(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "movie/drama";
        case 0x01 : return "detective/thriller";
        case 0x02 : return "adventure/western/war";
        case 0x03 : return "science fiction/fantasy/horror";
        case 0x04 : return "comedy";
        case 0x05 : return "soap/melodrama/folkloric";
        case 0x06 : return "romance";
        case 0x07 : return "serious/classical/religious/historical movie/drama";
        case 0x08 : return "adult movie/drama";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_02(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "news/current affairs";
        case 0x01 : return "news/weather report";
        case 0x02 : return "news magazine";
        case 0x03 : return "documentary";
        case 0x04 : return "discussion/interview/debate";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_03(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "show/game show";
        case 0x01 : return "game show/quiz/contest";
        case 0x02 : return "variety show";
        case 0x03 : return "talk show";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_04(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "sports";
        case 0x01 : return "special events";
        case 0x02 : return "sports magazines";
        case 0x03 : return "football/soccer";
        case 0x04 : return "tennis/squash";
        case 0x05 : return "team sports (excluding football)";
        case 0x06 : return "athletics";
        case 0x07 : return "motor sport";
        case 0x08 : return "water sport";
        case 0x09 : return "winter sports";
        case 0x0A : return "equestrian";
        case 0x0B : return "martial sports";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_05(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "children's/youth programmes";
        case 0x01 : return "pre-school children's programmes";
        case 0x02 : return "entertainment programmes for 6 to 14";
        case 0x03 : return "entertainment programmes for 10 to 16";
        case 0x04 : return "informational/educational/school programmes";
        case 0x05 : return "cartoons/puppets";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_06(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "music/ballet/dance";
        case 0x01 : return "rock/pop";
        case 0x02 : return "serious music/classical music";
        case 0x03 : return "folk/traditional music";
        case 0x04 : return "jazz";
        case 0x05 : return "musical/opera";
        case 0x06 : return "ballet";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_07(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "arts/culture (without music)";
        case 0x01 : return "performing arts";
        case 0x02 : return "fine arts";
        case 0x03 : return "religion";
        case 0x04 : return "popular culture/traditional arts";
        case 0x05 : return "literature";
        case 0x06 : return "film/cinema";
        case 0x07 : return "experimental film/video";
        case 0x08 : return "broadcasting/press";
        case 0x09 : return "new media";
        case 0x0A : return "arts/culture magazines";
        case 0x0B : return "fashion";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_08(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "social/political issues/economics";
        case 0x01 : return "magazines/reports/documentary";
        case 0x02 : return "economics/social advisory";
        case 0x03 : return "remarkable people";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_09(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "education/science/factual topics";
        case 0x01 : return "nature/animals/environment";
        case 0x02 : return "technology/natural sciences";
        case 0x03 : return "medicine/physiology/psychology";
        case 0x04 : return "foreign countries/expeditions";
        case 0x05 : return "social/spiritual sciences";
        case 0x06 : return "further education";
        case 0x07 : return "languages";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_0A(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "leisure hobbies";
        case 0x01 : return "tourism/travel";
        case 0x02 : return "handicraft";
        case 0x03 : return "motoring";
        case 0x04 : return "fitness and health";
        case 0x05 : return "cooking";
        case 0x06 : return "advertisement/shopping";
        case 0x07 : return "gardening";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2_0B(int8u content_nibble_level_2)
{
    switch (content_nibble_level_2)
    {
        case 0x00 : return "original language";
        case 0x01 : return "black and white";
        case 0x02 : return "unpublished";
        case 0x03 : return "live broadcast";
        default   :
            if (content_nibble_level_2==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_content_nibble_level_2(int8u content_nibble_level_1, int8u content_nibble_level_2)
{
    switch (content_nibble_level_1)
    {
        case 0x00 : return "undefined";
        case 0x01 : return Mpeg_Descriptors_content_nibble_level_2_01(content_nibble_level_2);
        case 0x02 : return Mpeg_Descriptors_content_nibble_level_2_02(content_nibble_level_2);
        case 0x03 : return Mpeg_Descriptors_content_nibble_level_2_03(content_nibble_level_2);
        case 0x04 : return Mpeg_Descriptors_content_nibble_level_2_04(content_nibble_level_2);
        case 0x05 : return Mpeg_Descriptors_content_nibble_level_2_05(content_nibble_level_2);
        case 0x06 : return Mpeg_Descriptors_content_nibble_level_2_06(content_nibble_level_2);
        case 0x07 : return Mpeg_Descriptors_content_nibble_level_2_07(content_nibble_level_2);
        case 0x08 : return Mpeg_Descriptors_content_nibble_level_2_08(content_nibble_level_2);
        case 0x09 : return Mpeg_Descriptors_content_nibble_level_2_09(content_nibble_level_2);
        case 0x0A : return Mpeg_Descriptors_content_nibble_level_2_0A(content_nibble_level_2);
        case 0x0B : return Mpeg_Descriptors_content_nibble_level_2_0B(content_nibble_level_2);
        default   :
            if (content_nibble_level_1==0x0F)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_linkage_type(int8u linkage_type)
{
    switch (linkage_type)
    {
        case 0x00 : return "reserved for future use";
        case 0x01 : return "information service";
        case 0x02 : return "Electronic Programme Guide (EPG) service";
        case 0x03 : return "CA replacement service";
        case 0x04 : return "transport stream containing complete Network/Bouquet SI";
        case 0x05 : return "service replacement service";
        case 0x06 : return "data broadcast service";
        case 0xFF : return "reserved for future use";
        default   :
            if (linkage_type>=0x80)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_dvb_service_type(int8u service_type)
{
    switch (service_type)
    {
        case 0x01 : return "digital television";
        case 0x02 : return "digital radio";
        case 0x03 : return "teletext";
        case 0x04 : return "NVOD reference";
        case 0x05 : return "NVOD time-shifted";
        case 0x06 : return "Mosaic";
        case 0x0A : return "advanced codec digital radio sound";
        case 0x0B : return "advanced codec mosaic service";
        case 0x0C : return "data broadcast";
        case 0x0D : return "reserved for Common Interface Usage";
        case 0x0E : return "RCS Map";
        case 0x0F : return "RCS FLS";
        case 0x10 : return "DVB MHP";
        case 0x11 : return "MPEG-2 HD digital television";
        case 0x16 : return "advanced codec SD digital television";
        case 0x17 : return "advanced codec SD NVOD time-shifted";
        case 0x18 : return "advanced codec SD NVOD reference";
        case 0x19 : return "advanced codec HD digital television";
        case 0x1A : return "advanced codec HD NVOD time-shifted";
        case 0x1B : return "advanced codec HD NVOD reference";
        case 0xFF : return "reserved for future use";
        default   :
            if (service_type>=0x80)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_stream_content(int8u stream_content)
{
    switch (stream_content)
    {
        case 0x01 : return "MPEG-2 Video";
        case 0x02 : return "MPEG-1 Audio L2";
        case 0x03 : return "Subtitle";
        case 0x04 : return "AC3";
        case 0x05 : return "AVC";
        case 0x06 : return "HE-AAC";
        case 0x07 : return "DTS";
        default   :
            if (stream_content>=0x0C)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O1(int8u component_type)
{
    switch (component_type)
    {
        case 0x01 : return "4:3 aspect ratio, 25 Hz";
        case 0x02 : return "16:9 aspect ratio with pan vectors, 25 Hz";
        case 0x03 : return "16:9 aspect ratio without pan vectors, 25 Hz";
        case 0x04 : return ">16:9 aspect ratio, 25 Hz";
        case 0x05 : return "4:3 aspect ratio, 30 Hz";
        case 0x06 : return "16:9 aspect ratio with pan vectors, 30 Hz";
        case 0x07 : return "16:9 aspect ratio without pan vectors, 30 Hz";
        case 0x08 : return ">16:9 aspect ratio, 30 Hz";
        case 0x09 : return "4:3 aspect ratio, 25 Hz (high definition)";
        case 0x0A : return "16:9 aspect ratio with pan vectors, 25 Hz (high definition)";
        case 0x0B : return "16:9 aspect ratio without pan vectors, 25 Hz (high definition)";
        case 0x0C : return ">16:9 aspect ratio, 25 Hz (high definition)";
        case 0x0D : return "4:3 aspect ratio, 30 Hz (high definition)";
        case 0x0E : return "16:9 aspect ratio with pan vectors, 30 Hz (high definition)";
        case 0x0F : return "16:9 aspect ratio without pan vectors, 30 Hz (high definition)";
        case 0x10 : return ">16:9 aspect ratio, 30 Hz (high definition)";
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O2(int8u component_type)
{
    switch (component_type)
    {
        case 0x01 : return "single mono channel";
        case 0x02 : return "dual mono channel";
        case 0x03 : return "stereo (2 channel)";
        case 0x04 : return "multi-lingual, multi-channel";
        case 0x05 : return "surround sound";
        case 0x40 : return "description for the visually impaired";
        case 0x41 : return "for the hard of hearing";
        case 0x42 : return "receiver-mixed supplementary audio";
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O3(int8u component_type)
{
    switch (component_type)
    {
        case 0x01 : return "EBU Teletext subtitles";
        case 0x02 : return "associated EBU Teletext";
        case 0x03 : return "VBI data";
        case 0x10 : return "DVB subtitles (normal) with no monitor aspect ratio criticality";
        case 0x11 : return "DVB subtitles (normal) for display on 4:3 aspect ratio monitor";
        case 0x12 : return "DVB subtitles (normal) for display on 16:9 aspect ratio monitor";
        case 0x13 : return "DVB subtitles (normal) for display on 2.21:1 aspect ratio monitor";
        case 0x20 : return "DVB subtitles (for the hard of hearing) with no monitor aspect ratio criticality";
        case 0x21 : return "DVB subtitles (for the hard of hearing) for display on 4:3 aspect ratio monitor";
        case 0x22 : return "DVB subtitles (for the hard of hearing) for display on 16:9 aspect ratio monitor";
        case 0x23 : return "DVB subtitles (for the hard of hearing) for display on 2.21:1 aspect ratio monitor";
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O4(int8u)
{
    return "Defined by AC3";
}

const char* Mpeg_Descriptors_component_type_O5(int8u component_type)
{
    switch (component_type)
    {
        case 0x01 : return "4:3 aspect ratio, 25 Hz";
        case 0x03 : return "16:9 aspect ratio, 25 Hz";
        case 0x04 : return ">16:9 aspect ratio, 25 Hz";
        case 0x05 : return "4:3 aspect ratio, 30 Hz";
        case 0x07 : return "16:9 aspect ratio, 30 Hz";
        case 0x08 : return ">16:9 aspect ratio, 30 Hz";
        case 0x0B : return "16:9 aspect ratio, 25 Hz (high definition)";
        case 0x0C : return ">16:9 aspect ratio, 25 Hz (high definition)";
        case 0x0F : return "16:9 aspect ratio, 30 Hz (high definition)";
        case 0x10 : return ">16:9 aspect ratio, 30 Hz (high definition)";
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O6(int8u component_type)
{
    switch (component_type)
    {
        case 0x01 : return "single mono channel";
        case 0x03 : return "stereo";
        case 0x05 : return "surround sound";
        case 0x40 : return "description for the visually impaired";
        case 0x41 : return "for the hard of hearing";
        case 0x42 : return "receiver-mixed supplementary audio";
        case 0x43 : return "astereo (v2)";
        case 0x44 : return "description for the visually impaired (v2)";
        case 0x45 : return "for the hard of hearing (v2)";
        case 0x46 : return "receiver-mixed supplementary audio (v2)";
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type_O7(int8u)
{
    return "Defined by DTS";
}

const char* Mpeg_Descriptors_codepage_1(int8u codepage)
{
    switch (codepage)
    {
        case 0x01 : return "ISO/IEC 8859-5 (Cyrillic)";
        case 0x02 : return "ISO/IEC 8859-6 (Arabic)";
        case 0x03 : return "ISO/IEC 8859-7 (Greek)";
        case 0x04 : return "ISO/IEC 8859-8 (Hebrew)";
        case 0x05 : return "ISO/IEC 8859-9 (Latin)";
        case 0x06 : return "ISO/IEC 8859-10 (Latin)";
        case 0x07 : return "ISO/IEC 8859-11 (Thai)";
        case 0x08 : return "ISO/IEC 8859-12 (Indian)";
        case 0x09 : return "ISO/IEC 8859-13 (Latin)";
        case 0x0A : return "ISO/IEC 8859-14 (Celtic)";
        case 0x0B : return "ISO/IEC 8859-15 (Latin)";
        case 0x11 : return "ISO/IEC 10646-1 (Basic Multilingual Plane)";
        case 0x12 : return "KSC5601-1987 (Korean)";
        case 0x13 : return "GB-2312-1980 (Simplified Chinese)";
        case 0x14 : return "Big5 (Traditional Chinese)";
        case 0x15 : return "UTF-8 (Basic Multilingual Plane)";
        default   : return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_component_type(int8u stream_content, int8u component_type)
{
    switch (stream_content)
    {
        case 0x01 : return Mpeg_Descriptors_component_type_O1(component_type);
        case 0x02 : return Mpeg_Descriptors_component_type_O2(component_type);
        case 0x03 : return Mpeg_Descriptors_component_type_O3(component_type);
        case 0x04 : return Mpeg_Descriptors_component_type_O4(component_type);
        case 0x05 : return Mpeg_Descriptors_component_type_O5(component_type);
        case 0x06 : return Mpeg_Descriptors_component_type_O6(component_type);
        case 0x07 : return Mpeg_Descriptors_component_type_O7(component_type);
        default   :
            if (component_type>=0xB0 && component_type<=0xFE)
                    return "user defined";
            else
                    return "reserved for future use";
    }
}

const char* Mpeg_Descriptors_registration_format_identifier_Format(int32u format_identifier)
{
    switch (format_identifier)
    {
        case Elements::AC_3 : return "AC-3";
        case Elements::BSSD : return "PCM"; //AES3
        case Elements::CUEI : return "SCTE 35 2003 - Digital Program Insertion Cueing Message for Cable";
        case Elements::DTS1 : return "DTS"; //512
        case Elements::DTS2 : return "DTS"; //1024
        case Elements::DTS3 : return "DTS"; //2048
        case Elements::GA94 : return "ATSC - Terrestrial";
        case Elements::HDMV : return "Blu-ray";
        case Elements::S14A : return "ATSC - Satellite";
        case Elements::SCTE : return "SCTE 54 2003 - Digital Video Service Multiplex and Transport System for Cable Television";
        case Elements::TSHV : return "Digital Video";
        case Elements::VC_1 : return "VC-1";
        default :                     return "";
    }
}

stream_t Mpeg_Descriptors_registration_format_identifier_StreamKind(int32u format_identifier)
{
    switch (format_identifier)
    {
        case Elements::AC_3 : return Stream_Audio;
        case Elements::BSSD : return Stream_Audio;
        case Elements::DTS1 : return Stream_Audio;
        case Elements::DTS2 : return Stream_Audio;
        case Elements::DTS3 : return Stream_Audio;
        case Elements::VC_1 : return Stream_Video;
        default :             return Stream_Max;
    }
}

const char* Mpeg_Descriptors_stream_Format(int8u descriptor_tag, int32u format_identifier)
{
    switch (descriptor_tag)
    {
        case 0x02 : return "MPEG Video";
        case 0x03 : return "MPEG Audio";
        case 0x1B : return "MPEG-4 Visual";
        case 0x1C : return "AAC";
        case 0x28 : return "AVC";
        case 0x2B : return "AAC";
        case 0x2D : return "Text";
        default :
            switch (format_identifier)
            {
                case Elements::CUEI :
                case Elements::SCTE : //SCTE
                case Elements::GA94 :
                case Elements::S14A : //ATSC
                        switch (descriptor_tag)
                        {
                            case 0x81 : return "AC-3";
                            default   : return "";
                        }
                case Elements::AC_3 : return "AC-3";
                case Elements::DTS1 : return "DTS";
                case Elements::DTS2 : return "DTS";
                case Elements::DTS3 : return "DTS";
                case Elements::VC_1 : return "VC-1";
                case Elements::drac : return "Dirac";
                default                     :
                        switch (descriptor_tag)
                        {
                            case 0x56 : return "Teletext";
                            case 0x59 : return "DVB Subtitles";
                            case 0x6A : return "AC-3";
                            case 0x7A : return "E-AC-3";
                            case 0x7B : return "DTS";
                            case 0x7C : return "AAC";
                            case 0x81 : return "AC-3";
                            default   : return "";
                        }
            }
    }
}

const char* Mpeg_Descriptors_stream_Codec(int8u descriptor_tag, int32u format_identifier)
{
    switch (descriptor_tag)
    {
        case 0x02 : return "MPEG-V";
        case 0x03 : return "MPEG-A";
        case 0x1B : return "MPEG-4V";
        case 0x1C : return "AAC";
        case 0x28 : return "AVC";
        case 0x2B : return "AAC";
        case 0x2D : return "Text";
        default :
            switch (format_identifier)
            {
                case Elements::CUEI :
                case Elements::SCTE : //SCTE
                case Elements::GA94 :
                case Elements::S14A : //ATSC
                        switch (descriptor_tag)
                        {
                            case 0x81 : return "AC3";
                            default   : return "";
                        }
                case Elements::AC_3 : return "AC3";
                case Elements::DTS1 : return "DTS";
                case Elements::DTS2 : return "DTS";
                case Elements::DTS3 : return "DTS";
                case Elements::VC_1 : return "VC-1";
                case Elements::drac : return "Dirac";
                default                     :
                        switch (descriptor_tag)
                        {
                            case 0x56 : return "Teletext";
                            case 0x59 : return "DVB Subtitles";
                            case 0x6A : return "AC3";
                            case 0x7A : return "AC3+";
                            case 0x7B : return "DTS";
                            case 0x7C : return "AAC";
                            case 0x81 : return "AC3";
                            default   : return "";
                        }
            }
    }
}

stream_t Mpeg_Descriptors_stream_Kind(int8u descriptor_tag, int32u format_identifier)
{
    switch (descriptor_tag)
    {
        case 0x02 : return Stream_Video;
        case 0x03 : return Stream_Audio;
        case 0x1B : return Stream_Video;
        case 0x1C : return Stream_Audio;
        case 0x28 : return Stream_Video;
        case 0x2B : return Stream_Audio;
        case 0x2D : return Stream_Text;
        default :
            switch (format_identifier)
            {
                case Elements::CUEI :
                case Elements::SCTE : //SCTE
                case Elements::GA94 :
                case Elements::S14A : //ATSC
                        switch (descriptor_tag)
                        {
                            case 0x81 : return Stream_Audio;
                            default   : return Stream_Max;
                        }
                case Elements::AC_3 : return Stream_Audio;
                case Elements::DTS1 : return Stream_Audio;
                case Elements::DTS2 : return Stream_Audio;
                case Elements::DTS3 : return Stream_Audio;
                case Elements::VC_1 : return Stream_Video;
                case Elements::drac : return Stream_Video;
                default                     :
                        switch (descriptor_tag)
                        {
                            case 0x56 : return Stream_Text;
                            case 0x59 : return Stream_Text;
                            case 0x6A : return Stream_Audio;
                            case 0x7A : return Stream_Audio;
                            case 0x7B : return Stream_Audio;
                            case 0x7C : return Stream_Audio;
                            case 0x81 : return Stream_Audio;
                            default   : return Stream_Max;
                        }
            }
    }
}

//---------------------------------------------------------------------------
extern const float32 Mpegv_frame_rate[]; //In Video/File_Mpegv.cpp
extern const char*  Mpegv_Colorimetry_format[]; //In Video/File_Mpegv.cpp
extern const char*  Mpegv_profile_and_level_indication_profile[]; //In Video/File_Mpegv.cpp
extern const char*  Mpegv_profile_and_level_indication_level[]; //In Video/File_Mpegv.cpp

//---------------------------------------------------------------------------
extern const char*  Mpega_Version[]; //In Audio/File_Mpega.cpp
extern const char*  Mpega_Layer[]; //In Audio/File_Mpega.cpp
extern const char*  Mpega_Format_Profile_Version[]; //In Audio/File_Mpega.cpp
extern const char*  Mpega_Format_Profile_Layer[]; //In Audio/File_Mpega.cpp

//---------------------------------------------------------------------------
extern const int32u AC3_SamplingRate[]; //In Audio/File_Ac3.cpp
extern const int16u AC3_BitRate[]; //In Audio/File_Ac3.cpp
extern const char*  AC3_ChannelPositions[]; //In Audio/File_Ac3.cpp
extern const int8u  AC3_Channels[]; //In Audio/File_Ac3.cpp
extern const char*  AC3_Mode[]; //In Audio/File_Ac3.cpp
extern const char*  AC3_Surround[]; //In Audio/File_Ac3.cpp

const char* Mpeg_Descriptors_AC3_Channels[]=
{
    "1",
    "2",
    "2",
    "2",
    "3+",
    "6+",
    "",
    "",
};

const char* Mpeg_Descriptors_AC3_Priority[]=
{
    "",
    "Primary Audio",
    "Other Audio",
    "",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_bandwidth[]=
{
    "8 MHz",
    "7 MHz",
    "6 MHz",
    "5 MHz",
    "",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_constellation[]=
{
    "QPSK",
    "16-QAM",
    "64-QAM",
    "",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_hierarchy_information[]=
{
    "non-hierarchical, native interleaver",
    "1, native interleaver",
    "2, native interleaver",
    "4, native interleaver",
    "non-hierarchical, in-depth interleaver",
    "1, in-depth interleaver",
    "2, in-depth interleaver",
    "4, in-depth interleaver",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_code_rate[]=
{
    "1/2",
    "2/3",
    "3/4",
    "5/6",
    "7/8",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_guard_interval[]=
{
    "1/32",
    "1/16",
    "1/8",
    "1/4",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_transmission_mode[]=
{
    "2k mode",
    "8k mode",
    "4k mode",
    "",
};

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_original_network_id(int16u original_network_id)
{
    switch (original_network_id)
    {
        case 0x0001 : return "Astra Satellite Network 19,2'E";
        case 0x0002 : return "Astra Satellite Network 28,2'E";
        case 0x0003 : return "Astra 1";
        case 0x0004 : return "Astra 2";
        case 0x0005 : return "Astra 3";
        case 0x0006 : return "Astra 4";
        case 0x0007 : return "Astra 5";
        case 0x0008 : return "Astra 6";
        case 0x0009 : return "Astra 7";
        case 0x000A : return "Astra 8";
        case 0x000B : return "Astra 9";
        case 0x000C : return "Astra 10";
        case 0x000D : return "Astra 11";
        case 0x000E : return "Astra 12";
        case 0x000F : return "Astra 13";
        case 0x0010 : return "Astra 14";
        case 0x0011 : return "Astra 15";
        case 0x0012 : return "Astra 16";
        case 0x0013 : return "Astra 17";
        case 0x0014 : return "Astra 18";
        case 0x0015 : return "Astra 19";
        case 0x0016 : return "Astra 20";
        case 0x0017 : return "Astra 21";
        case 0x0018 : return "Astra 22";
        case 0x0019 : return "Astra 23";
        case 0x0020 : return "ASTRA";
        case 0x0021 : return "Hispasat Network 1";
        case 0x0022 : return "Hispasat Network 2";
        case 0x0023 : return "Hispasat Network 3";
        case 0x0024 : return "Hispasat Network 4";
        case 0x0025 : return "Hispasat Network 5";
        case 0x0026 : return "Hispasat Network 6";
        case 0x0027 : return "Hispasat 30'W (FSS)";
        case 0x0028 : return "Hispasat 30'W (DBS)";
        case 0x0029 : return "Hispasat 30'W (America)";
        case 0x0030 : return "Canal+ Satellite Network";
        case 0x0031 : return "Hispasat � VIA DIGITAL";
        case 0x0032 : return "Hispasat Network 7";
        case 0x0033 : return "Hispasat Network 8";
        case 0x0034 : return "Hispasat Network 9";
        case 0x0035 : return "Nethold Main Mux System";
        case 0x0037 : return "STENTOR";
        case 0x0040 : return "HPT � Croatian Post and Telecommunications";
        case 0x0041 : return "Mindport";
        case 0x0046 : return "1 degree W (Telenor)";
        case 0x0047 : return "1 degree W (Telenor)";
        case 0x0050 : return "HRT � Croatian Radio and Television";
        case 0x0051 : return "Havas";
        case 0x0052 : return "Osaka Yusen Satellite";
        case 0x0055 : return "Sirius Satellite System";
        case 0x0058 : return "Thiacom 1 & 2 co-located 78.5'E (UBC Thailand)";
        case 0x005E : return "Sirius Satellite System (Nordic Coverage)";
        case 0x005F : return "Sirius Satellite System (FSS)";
        case 0x0060 : return "Deutsche Telekom";
        case 0x0069 : return "Optus B3 156'E";
        case 0x0070 : return "BONUM1 36 Degrees East (NTV+)";
        case 0x007E : return "Eutelsat Satellite System at 7'E";
        case 0x0073 : return "PanAmSat 4 68.5'E";
        case 0x0085 : return "BetaTechnik";
        case 0x0090 : return "TDF";
        case 0x00A0 : return "News Datacom";
        case 0x00A1 : return "News Datacom";
        case 0x00A2 : return "News Datacom";
        case 0x00A3 : return "News Datacom";
        case 0x00A4 : return "News Datacom";
        case 0x00A5 : return "News Datacom";
        case 0x00A6 : return "ART";
        case 0x00A7 : return "Globecast";
        case 0x00A8 : return "Foxtel";
        case 0x00A9 : return "Sky New Zealand";
        case 0x00B0 : return "TPS";
        case 0x00B1 : return "TPS";
        case 0x00B2 : return "TPS";
        case 0x00B3 : return "TPS";
        case 0x00B4 : return "Telesat 107.3'W";
        case 0x00B5 : return "Telesat 111.1'W";
        case 0x00BA : return "Satellite Express � 6 (80'E)";
        case 0x00C0 : return "Canal+";
        case 0x00C1 : return "Canal+";
        case 0x00C2 : return "Canal+";
        case 0x00C3 : return "Canal+";
        case 0x00C4 : return "Canal+";
        case 0x00C5 : return "Canal+";
        case 0x00C6 : return "Canal+";
        case 0x00C7 : return "Canal+";
        case 0x00C8 : return "Canal+";
        case 0x00C9 : return "Canal+";
        case 0x00CA : return "Canal+";
        case 0x00CB : return "Canal+";
        case 0x00CC : return "Canal+";
        case 0x00CD : return "Canal+";
        case 0x0100 : return "ExpressVu Express";
        case 0x010E : return "Eutelsat Satellite System at 10'E";
        case 0x0110 : return "Mediaset";
        case 0x013E : return "Eutelsat Satellite System at 13'E";
        case 0x016E : return "Eutelsat Satellite System at 16'E";
        case 0x029E : return "Eutelsat Satellite System at 29'E";
        case 0x02BE : return "Arabsat Arabsat (Scientific Atlanta, Eutelsat)";
        case 0x036E : return "Eutelsat Satellite System at 36'E";
        case 0x03E8 : return "Telia";
        case 0x048E : return "Eutelsat Satellite System at 48'E";
        case 0x0800 : return "Nilesat 101";
        case 0x0801 : return "Nilesat 101";
        case 0x0880 : return "MEASAT 1, 91.5'E";
        case 0x0882 : return "MEASAT 2, 91.5'E";
        case 0x0883 : return "MEASAT 2, 148.0'E";
        case 0x088F : return "MEASAT 3";
        case 0x1000 : return "Optus B3 156'E Optus Communications";
        case 0x1001 : return "DISH Network Echostar Communications";
        case 0x1002 : return "Dish Network 61.5 W Echostar Communications";
        case 0x1003 : return "Dish Network 83 W Echostar Communications";
        case 0x1004 : return "Dish Network 119 W Echostar Communications";
        case 0x1005 : return "Dish Network 121 W Echostar Communications";
        case 0x1006 : return "Dish Network 148 W Echostar Communications";
        case 0x1007 : return "Dish Network 175 W Echostar Communications";
        case 0x1008 : return "Dish Network W Echostar Communications";
        case 0x1009 : return "Dish Network X Echostar Communications";
        case 0x100A : return "Dish Network Y Echostar Communications";
        case 0x100B : return "Dish Network Z Echostar Communications";
        case 0x2000 : return "Thiacom 1 & 2 co-located 78.5'E";
        case 0x22D4 : return "Spanish Digital Terrestrial Television";
        case 0x22F1 : return "Swedish Digital Terrestrial Television";
        case 0x233A : return "UK Digital Terrestrial Television";
        case 0x2024 : return "Australian Digital Terrestrial Television";
        case 0x2114 : return "German Digital Terrestrial Television";
        case 0x3000 : return "PanAmSat 4 68.5'E";
        case 0x5000 : return "Irdeto Mux System";
        case 0xF000 : return "Small Cable networks";
        case 0xF001 : return "Deutsche Telekom";
        case 0xF010 : return "Telefonica Cable";
        case 0xF020 : return "Cable and Wireless Communication";
        case 0xFBFC : return "MATAV";
        case 0xFBFD : return "Telia Kabel-TV";
        case 0xFBFE : return "TPS";
        case 0xFBFF : return "Stream";
        case 0xFC00 : return "France Telecom Cable";
        case 0xFC10 : return "Rhone Vision Cable";
        case 0xFD00 : return "Lyonnaise Communications";
        case 0xFE00 : return "TeleDenmark Cable TV";
        default     : return "";
    }
}

//---------------------------------------------------------------------------
const char* Mpeg_Descriptors_CA_system_ID(int16u CA_ID)
{
    switch (CA_ID)
    {
        case 0x0100 : return "Seca Mediaguard 1/2";
        case 0x0101 : return "RusCrypto";
        case 0x0464 : return "EuroDec";
        case 0x0500 : return "TPS-Crypt  or Viaccess";
        case 0x0602 :
        case 0x0604 :
        case 0x0606 :
        case 0x0608 :
        case 0x0622 :
        case 0x0626 : return "Irdeto";
        case 0x0700 : return "DigiCipher 2";
        case 0x0911 :
        case 0x0919 :
        case 0x0960 :
        case 0x0961 : return "NDS Videoguard 1/2";
        case 0x0B00 : return "Conax CAS 5 /7";
        case 0x0D00 :
        case 0x0D02 :
        case 0x0D03 :
        case 0x0D05 :
        case 0x0D07 :
        case 0x0D20 : return "Cryptoworks";
        case 0x0E00 : return "PowerVu";
        case 0x1000 : return "RAS (Remote Authorisation System)";
        case 0x1702 :
        case 0x1722 :
        case 0x1762 : return "BetaCrypt 1 or Nagravision";
        case 0x1710 : return "BetaCrypt 2";
        case 0x1800 :
        case 0x1801 :
        case 0x1810 :
        case 0x1830 : return "Nagravision";
        case 0x22F0 : return "Codicrypt";
        case 0x2600 : return "BISS";
        case 0x4800 : return "Accessgate";
        case 0x4900 : return "China Crypt";
        case 0x4A10 : return "EasyCas";
        case 0x4A20 : return "AlphaCrypt";
        case 0x4A60 :
        case 0x4A61 :
        case 0x4A63 : return "SkyCrypt or Neotioncrypt or Neotion SHL";
        case 0x4A70 : return "DreamCrypt";
        case 0x4A80 : return "ThalesCrypt";
        case 0x4AA1 : return "KeyFly";
        case 0x4ABF : return "DG-Crypt";
        case 0x4AD0 :
        case 0x4AD1 : return "X-Crypt";
        case 0x4AD4 : return "OmniCrypt";
        case 0x4AE0 : return "RossCrypt";
        case 0x5500 : return "Z-Crypt or DRE-Crypt";
        case 0x5501 : return "Griffin";
        default     : return "Encrypted";
    }
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Mpeg_Descriptors::File_Mpeg_Descriptors()
{
    //In
    Complete_Stream=NULL;
    transport_stream_id=0x0000;
    table_id=0x00;
    table_id_extension=0x0000;
    xxx_id=0x0000;
    xxx_id_IsValid=false;
}

//***************************************************************************
// Buffer
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Header_Parse()
{
    int8u descriptor_tag=0, descriptor_length=0;
    Get_B1 (descriptor_tag,                                     "descriptor_tag");
    Get_B1 (descriptor_length,                                  "descriptor_length");

    //Size
    if (Element_Size)
        Header_Fill_Size(Element_Size);
    if (Element_Offset)
        Header_Fill_Size(Element_Offset);
    if (descriptor_length)
        Header_Fill_Size(descriptor_length);

    if (Element_Size<Element_Offset+descriptor_length)
    {
        Element_WaitForMoreData();
        return;
    }

    //Filling
    Header_Fill_Code(descriptor_tag, Ztring().From_Number(descriptor_tag, 16));
    Header_Fill_Size(2+descriptor_length);
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Data_Parse()
{
    #define ELEMENT_CASE(_NAME, _DETAIL) \
        case 0x##_NAME : Element_Name(_DETAIL); Descriptor_##_NAME(); break;

    //Parsing
         if (table_id> 0x00 && table_id<0x40)
    {
        switch (Element_Code)
        {
            ELEMENT_CASE(00, "Reserved");
            ELEMENT_CASE(01, "Reserved");
            ELEMENT_CASE(02, "video_stream");
            ELEMENT_CASE(03, "audio_stream");
            ELEMENT_CASE(04, "hierarchy");
            ELEMENT_CASE(05, "registration");
            ELEMENT_CASE(06, "data_stream_alignment");
            ELEMENT_CASE(07, "target_background_grid");
            ELEMENT_CASE(08, "Video_window");
            ELEMENT_CASE(09, "CA");
            ELEMENT_CASE(0A, "ISO_639_language");
            ELEMENT_CASE(0B, "System_clock");
            ELEMENT_CASE(0C, "Multiplex_buffer_utilization");
            ELEMENT_CASE(0D, "Copyright");
            ELEMENT_CASE(0E, "Maximum_bitrate");
            ELEMENT_CASE(0F, "Private_data_indicator");
            ELEMENT_CASE(10, "Smoothing_buffer");
            ELEMENT_CASE(11, "STD");
            ELEMENT_CASE(12, "IBP");
            ELEMENT_CASE(13, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(14, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(15, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(16, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(17, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(18, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(19, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(1A, "Defined in ISO/IEC 13818-6");
            ELEMENT_CASE(1B, "MPEG-4_video");
            ELEMENT_CASE(1C, "MPEG-4_audio");
            ELEMENT_CASE(1D, "IOD");
            ELEMENT_CASE(1E, "SL");
            ELEMENT_CASE(1F, "FMC");
            ELEMENT_CASE(20, "External_ES_ID");
            ELEMENT_CASE(21, "MuxCode");
            ELEMENT_CASE(22, "FmxBufferSize");
            ELEMENT_CASE(23, "MultiplexBuffer");
            ELEMENT_CASE(24, "Content_labeling_descriptor");
            ELEMENT_CASE(25, "Metadata_pointer_descriptor");
            ELEMENT_CASE(26, "Metadata_descriptor");
            ELEMENT_CASE(27, "Metadata_STD_descriptor");
            ELEMENT_CASE(28, "AVC video descriptor");
            ELEMENT_CASE(29, "IPMP_descriptor (defined in ISO/IEC 13818-11, MPEG-2 IPMP)");
            ELEMENT_CASE(2A, "AVC timing and HRD descriptor");
            ELEMENT_CASE(2B, "MPEG-2 AAC audio descriptor");
            ELEMENT_CASE(2C, "FlexMux_Timing_descriptor");
            ELEMENT_CASE(2D, "MPEG-4_text_descriptor");
            ELEMENT_CASE(2E, "MPEG-4_audio_extension_descriptor");
            ELEMENT_CASE(2F, "Auxiliary_video_data_descriptor");

            //Following is in private sections, in case there is not network type detected
            ELEMENT_CASE(40, "DVB - network_name_descriptor");
            ELEMENT_CASE(41, "DVB - service_list_descriptor");
            ELEMENT_CASE(42, "DVB - stuffing_descriptor");
            ELEMENT_CASE(43, "DVB - satellite_delivery_system_descriptor");
            ELEMENT_CASE(44, "DVB - cable_delivery_system_descriptor");
            ELEMENT_CASE(45, "DVB - VBI_data_descriptor");
            ELEMENT_CASE(46, "DVB - VBI_teletext_descriptor");
            ELEMENT_CASE(47, "DVB - bouquet_name_descriptor");
            ELEMENT_CASE(48, "DVB - service_descriptor");
            ELEMENT_CASE(49, "DVB - country_availability_descriptor");
            ELEMENT_CASE(4A, "DVB - linkage_descriptor");
            ELEMENT_CASE(4B, "DVB - NVOD_reference_descriptor");
            ELEMENT_CASE(4C, "DVB - time_shifted_service_descriptor");
            ELEMENT_CASE(4D, "DVB - short_event_descriptor");
            ELEMENT_CASE(4E, "DVB - extended_event_descriptor");
            ELEMENT_CASE(4F, "DVB - time_shifted_event_descriptor");
            ELEMENT_CASE(50, "DVB - component_descriptor");
            ELEMENT_CASE(51, "DVB - mosaic_descriptor");
            ELEMENT_CASE(52, "DVB - stream_identifier_descriptor");
            ELEMENT_CASE(53, "DVB - CA_identifier_descriptor");
            ELEMENT_CASE(54, "DVB - content_descriptor");
            ELEMENT_CASE(55, "DVB - parental_rating_descriptor");
            ELEMENT_CASE(56, "DVB - teletext_descriptor");
            ELEMENT_CASE(57, "DVB - telephone_descriptor");
            ELEMENT_CASE(58, "DVB - local_time_offset_descriptor");
            ELEMENT_CASE(59, "DVB - subtitling_descriptor");
            ELEMENT_CASE(5A, "DVB - terrestrial_delivery_system_descriptor");
            ELEMENT_CASE(5B, "DVB - multilingual_network_name_descriptor");
            ELEMENT_CASE(5C, "DVB - multilingual_bouquet_name_descriptor");
            ELEMENT_CASE(5D, "DVB - multilingual_service_name_descriptor");
            ELEMENT_CASE(5E, "DVB - multilingual_component_descriptor");
            ELEMENT_CASE(5F, "DVB - private_data_specifier_descriptor");
            ELEMENT_CASE(60, "DVB - service_move_descriptor");
            ELEMENT_CASE(61, "DVB - short_smoothing_buffer_descriptor");
            ELEMENT_CASE(62, "DVB - frequency_list_descriptor");
            ELEMENT_CASE(63, "DVB - partial_transport_stream_descriptor");
            ELEMENT_CASE(64, "DVB - data_broadcast_descriptor");
            ELEMENT_CASE(65, "DVB - scrambling_descriptor");
            ELEMENT_CASE(66, "DVB - data_broadcast_id_descriptor");
            ELEMENT_CASE(67, "DVB - transport_stream_descriptor");
            ELEMENT_CASE(68, "DVB - DSNG_descriptor");
            ELEMENT_CASE(69, "DVB - PDC_descriptor");
            ELEMENT_CASE(6A, "DVB - AC-3_descriptor");
            ELEMENT_CASE(6B, "DVB - ancillary_data_descriptor");
            ELEMENT_CASE(6C, "DVB - cell_list_descriptor");
            ELEMENT_CASE(6D, "DVB - cell_frequency_link_descriptor");
            ELEMENT_CASE(6E, "DVB - announcement_support_descriptor");
            ELEMENT_CASE(6F, "DVB - application_signalling_descriptor");
            ELEMENT_CASE(70, "DVB - adaptation_field_data_descriptor");
            ELEMENT_CASE(71, "DVB - service_identifier_descriptor");
            ELEMENT_CASE(72, "DVB - service_availability_descriptor");
            ELEMENT_CASE(73, "DVB - default_authority_descriptor");
            ELEMENT_CASE(74, "DVB - related_content_descriptor");
            ELEMENT_CASE(75, "DVB - TVA_id_descriptor");
            ELEMENT_CASE(76, "DVB - content_identifier_descriptor");
            ELEMENT_CASE(77, "DVB - time_slice_fec_identifier_descriptor");
            ELEMENT_CASE(78, "DVB - ECM_repetition_rate_descriptor");
            ELEMENT_CASE(79, "DVB - S2_satellite_delivery_system_descriptor");
            ELEMENT_CASE(7A, "DVB - enhanced_AC-3_descriptor");
            ELEMENT_CASE(7B, "DVB - DTS descriptor");
            ELEMENT_CASE(7C, "DVB - AAC descriptor");
            ELEMENT_CASE(7D, "DVB - reserved for future use");
            ELEMENT_CASE(7E, "DVB - reserved for future use");
            ELEMENT_CASE(7F, "DVB - extension descriptor");
            ELEMENT_CASE(80, "ATSC - stuffing");
            ELEMENT_CASE(81, "ATSC - AC-3 audio");
            ELEMENT_CASE(86, "ATSC - caption service");
            ELEMENT_CASE(87, "ATSC - content advisory");
            ELEMENT_CASE(A0, "ATSC - extended channel name");
            ELEMENT_CASE(A1, "ATSC - service location");
            ELEMENT_CASE(A2, "ATSC - time-shifted service");
            ELEMENT_CASE(A3, "ATSC - component name");
            ELEMENT_CASE(A8, "ATSC - DCC Departing Request");
            ELEMENT_CASE(A9, "ATSC - DCC Arriving Request");
            ELEMENT_CASE(AA, "ATSC - Redistribution Control");
            ELEMENT_CASE(AB, "ATSC - DCC Location Code");
            default: if (Element_Code>=0x40)
                        Element_Info("user private");
                     else
                        Element_Info("unknown");
                     Skip_XX(Element_Size,                          "Data");
                     break;
        }
    }
    else if (table_id>=0x40 && table_id<0x80)
    {
        switch (Element_Code)
        {
            ELEMENT_CASE(40, "DVB - network_name_descriptor");
            ELEMENT_CASE(41, "DVB - service_list_descriptor");
            ELEMENT_CASE(42, "DVB - stuffing_descriptor");
            ELEMENT_CASE(43, "DVB - satellite_delivery_system_descriptor");
            ELEMENT_CASE(44, "DVB - cable_delivery_system_descriptor");
            ELEMENT_CASE(45, "DVB - VBI_data_descriptor");
            ELEMENT_CASE(46, "DVB - VBI_teletext_descriptor");
            ELEMENT_CASE(47, "DVB - bouquet_name_descriptor");
            ELEMENT_CASE(48, "DVB - service_descriptor");
            ELEMENT_CASE(49, "DVB - country_availability_descriptor");
            ELEMENT_CASE(4A, "DVB - linkage_descriptor");
            ELEMENT_CASE(4B, "DVB - NVOD_reference_descriptor");
            ELEMENT_CASE(4C, "DVB - time_shifted_service_descriptor");
            ELEMENT_CASE(4D, "DVB - short_event_descriptor");
            ELEMENT_CASE(4E, "DVB - extended_event_descriptor");
            ELEMENT_CASE(4F, "DVB - time_shifted_event_descriptor");
            ELEMENT_CASE(50, "DVB - component_descriptor");
            ELEMENT_CASE(51, "DVB - mosaic_descriptor");
            ELEMENT_CASE(52, "DVB - stream_identifier_descriptor");
            ELEMENT_CASE(53, "DVB - CA_identifier_descriptor");
            ELEMENT_CASE(54, "DVB - content_descriptor");
            ELEMENT_CASE(55, "DVB - parental_rating_descriptor");
            ELEMENT_CASE(56, "DVB - teletext_descriptor");
            ELEMENT_CASE(57, "DVB - telephone_descriptor");
            ELEMENT_CASE(58, "DVB - local_time_offset_descriptor");
            ELEMENT_CASE(59, "DVB - subtitling_descriptor");
            ELEMENT_CASE(5A, "DVB - terrestrial_delivery_system_descriptor");
            ELEMENT_CASE(5B, "DVB - multilingual_network_name_descriptor");
            ELEMENT_CASE(5C, "DVB - multilingual_bouquet_name_descriptor");
            ELEMENT_CASE(5D, "DVB - multilingual_service_name_descriptor");
            ELEMENT_CASE(5E, "DVB - multilingual_component_descriptor");
            ELEMENT_CASE(5F, "DVB - private_data_specifier_descriptor");
            ELEMENT_CASE(60, "DVB - service_move_descriptor");
            ELEMENT_CASE(61, "DVB - short_smoothing_buffer_descriptor");
            ELEMENT_CASE(62, "DVB - frequency_list_descriptor");
            ELEMENT_CASE(63, "DVB - partial_transport_stream_descriptor");
            ELEMENT_CASE(64, "DVB - data_broadcast_descriptor");
            ELEMENT_CASE(65, "DVB - scrambling_descriptor");
            ELEMENT_CASE(66, "DVB - data_broadcast_id_descriptor");
            ELEMENT_CASE(67, "DVB - transport_stream_descriptor");
            ELEMENT_CASE(68, "DVB - DSNG_descriptor");
            ELEMENT_CASE(69, "DVB - PDC_descriptor");
            ELEMENT_CASE(6A, "DVB - AC-3_descriptor");
            ELEMENT_CASE(6B, "DVB - ancillary_data_descriptor");
            ELEMENT_CASE(6C, "DVB - cell_list_descriptor");
            ELEMENT_CASE(6D, "DVB - cell_frequency_link_descriptor");
            ELEMENT_CASE(6E, "DVB - announcement_support_descriptor");
            ELEMENT_CASE(6F, "DVB - application_signalling_descriptor");
            ELEMENT_CASE(70, "DVB - adaptation_field_data_descriptor");
            ELEMENT_CASE(71, "DVB - service_identifier_descriptor");
            ELEMENT_CASE(72, "DVB - service_availability_descriptor");
            ELEMENT_CASE(73, "DVB - default_authority_descriptor");
            ELEMENT_CASE(74, "DVB - related_content_descriptor");
            ELEMENT_CASE(75, "DVB - TVA_id_descriptor");
            ELEMENT_CASE(76, "DVB - content_identifier_descriptor");
            ELEMENT_CASE(77, "DVB - time_slice_fec_identifier_descriptor");
            ELEMENT_CASE(78, "DVB - ECM_repetition_rate_descriptor");
            ELEMENT_CASE(79, "DVB - S2_satellite_delivery_system_descriptor");
            ELEMENT_CASE(7A, "DVB - enhanced_AC-3_descriptor");
            ELEMENT_CASE(7B, "DVB - DTS descriptor");
            ELEMENT_CASE(7C, "DVB - AAC descriptor");
            ELEMENT_CASE(7D, "DVB - reserved for future use");
            ELEMENT_CASE(7E, "DVB - reserved for future use");
            ELEMENT_CASE(7F, "DVB - extension descriptor");
            default: if (Element_Code>=0x40)
                        Element_Info("user private");
                     else
                        Element_Info("unknown");
                     Skip_XX(Element_Size,                          "Data");
                     break;
        }
    }
    else if ((table_id>=0xC0 && table_id<0xE0))
    {
        switch (Element_Code)
        {
            ELEMENT_CASE(80, "ATSC - stuffing");
            ELEMENT_CASE(81, "ATSC - AC-3 audio");
            ELEMENT_CASE(86, "ATSC - caption service");
            ELEMENT_CASE(87, "ATSC - content advisory");
            ELEMENT_CASE(A0, "ATSC - extended channel name");
            ELEMENT_CASE(A1, "ATSC - service location");
            ELEMENT_CASE(A2, "ATSC - time-shifted service");
            ELEMENT_CASE(A3, "ATSC - component name");
            ELEMENT_CASE(A8, "ATSC - DCC Departing Request");
            ELEMENT_CASE(A9, "ATSC - DCC Arriving Request");
            ELEMENT_CASE(AA, "ATSC - Redistribution Control");
            ELEMENT_CASE(AB, "ATSC - DCC Location Code");
            default: if (Element_Code>=0x40)
                        Element_Info("user private");
                     else
                        Element_Info("unknown");
                     Skip_XX(Element_Size,                          "Data");
                     break;
        }
    }
    else
    {
        switch (Element_Code)
        {
            default: if (Element_Code>=0x40)
                        Element_Info("user private");
                     else
                        Element_Info("unknown");
                     Skip_XX(Element_Size,                          "Data");
                     break;
        }
    }
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_02()
{
    //Parsing
    int8u frame_rate_code;
    bool multiple_frame_rate_flag, MPEG_1_only_flag;
    int8u profile_and_level_indication_profile=4, profile_and_level_indication_level=10, chroma_format=1;
    bool frame_rate_extension_flag=false;
    BS_Begin();
    Get_SB (   multiple_frame_rate_flag,                        "multiple_frame_rate_flag");
    Get_S1 (4, frame_rate_code,                                 "frame_rate_code"); Param_Info(Mpegv_frame_rate[frame_rate_code]);
    Get_SB (   MPEG_1_only_flag,                                "MPEG_1_only_flag");
    Skip_SB(                                                    "constrained_parameter_flag");
    Skip_SB(                                                    "still_picture_flag");
    if (MPEG_1_only_flag==0)
    {
        Skip_SB(                                                "profile_and_level_indication_escape");
        Get_S1 (3, profile_and_level_indication_profile,        "profile_and_level_indication_profile"); Param_Info(Mpegv_profile_and_level_indication_profile[profile_and_level_indication_profile]);
        Get_S1 (4, profile_and_level_indication_level,          "profile_and_level_indication_level"); Param_Info(Mpegv_profile_and_level_indication_level[profile_and_level_indication_level]);
        Get_S1 (2, chroma_format,                               "chroma_format"); Param_Info(Mpegv_Colorimetry_format[chroma_format]);
        Get_SB (   frame_rate_extension_flag,                   "frame_rate_extension_flag");
        Skip_S1(5,                                              "reserved");
    }
    BS_End();

    //Filling
    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            if (!multiple_frame_rate_flag && !frame_rate_extension_flag && frame_rate_code)
                                Complete_Stream->Streams[xxx_id].Infos["FrameRate"]=Ztring::ToZtring(Mpegv_frame_rate[frame_rate_code]);
                            Complete_Stream->Streams[xxx_id].Infos["Format_Version"]=MPEG_1_only_flag?_T("Version 1"):_T("Version 2");
                            Complete_Stream->Streams[xxx_id].Infos["Colorimetry"]=Mpegv_Colorimetry_format[chroma_format];
                            if (profile_and_level_indication_profile)
                            {
                                Complete_Stream->Streams[xxx_id].Infos["Format_Profile"]=Ztring().From_Local(Mpegv_profile_and_level_indication_profile[profile_and_level_indication_profile])+_T("@")+Ztring().From_Local(Mpegv_profile_and_level_indication_level[profile_and_level_indication_level]);
                                Complete_Stream->Streams[xxx_id].Infos["Codec_Profile"]=Ztring().From_Local(Mpegv_profile_and_level_indication_profile[profile_and_level_indication_profile])+_T("@")+Ztring().From_Local(Mpegv_profile_and_level_indication_level[profile_and_level_indication_level]);
                            }
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_03()
{
    //Parsing
    int8u ID, layer;
    bool variable_rate_audio_indicator;
    BS_Begin();
    Skip_SB(                                                    "free_format_flag");
    Get_S1 (1, ID,                                              "ID"); Param_Info(Mpega_Version[2+ID]); //Mpega_Version is with MPEG2.5 hack
    Get_S1 (2, layer,                                           "layer");  Param_Info(Mpega_Layer[layer]);
    Get_SB (   variable_rate_audio_indicator,                   "variable_rate_audio_indicator");
    Skip_S1(3,                                                  "reserved");
    BS_End();

    FILLING_BEGIN()
        Complete_Stream->Streams[xxx_id].Infos["BitRate_Mode"]=variable_rate_audio_indicator?_T("VBR"):_T("CBR");
        Complete_Stream->Streams[xxx_id].Infos["Codec"]=Ztring(Mpega_Version[ID])+Ztring(Mpega_Layer[layer]);
        Complete_Stream->Streams[xxx_id].Infos["Format"]=_T("MPEG Audio");
        Complete_Stream->Streams[xxx_id].Infos["Format_Version"]=Mpega_Format_Profile_Version[ID];
        Complete_Stream->Streams[xxx_id].Infos["Format_Profile"]=Mpega_Format_Profile_Layer[layer];
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_05()
{
    //Parsing
    int32u format_identifier;
    Get_B4 (format_identifier,                                  "format_identifier"); Element_Info(Mpeg_Descriptors_registration_format_identifier_Format(format_identifier)); Param_Info(Mpeg_Descriptors_registration_format_identifier_Format(format_identifier));

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        switch (xxx_id_IsValid)
                        {
                            case false : //Per program
                                        Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].Programs[table_id_extension].registration_format_identifier=format_identifier;
                                        break;
                            case true : //Per PES
                                        Complete_Stream->Streams[xxx_id].registration_format_identifier=format_identifier;
                                        break;
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_06()
{
    //Parsing
    Info_B1(alignment_type,                                     "alignment_type"); Param_Info(Mpeg_Descriptors_alignment_type(alignment_type));
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_07()
{
    //Parsing
    BS_Begin();
    Skip_S1(14,                                                 "horizontal_size");
    Skip_S1(14,                                                 "vertical_size");
    Skip_S1( 4,                                                 "aspect_ratio_information"); //Same as ISO/IEC 13818-2
    BS_End();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_08()
{
    //Parsing
    BS_Begin();
    Skip_S1(14,                                                 "horizontal_offset");
    Skip_S1(14,                                                 "vertical_offset");
    Skip_S1( 4,                                                 "window_priority");
    BS_End();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_09()
{
    //Parsing
    int16u CA_system_ID, CA_PID;
    Get_B2 (CA_system_ID,                                       "CA_system_ID"); Param_Info(Mpeg_Descriptors_CA_system_ID(CA_system_ID));
    BS_Begin();
    Skip_S1( 3,                                                 "reserved");
    Get_S2 (13, CA_PID,                                         "CA_PID");
    BS_End();
    if (Element_Size-Element_Offset>0)
        Skip_XX(Element_Size-Element_Offset,                    "private_data_byte");

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x01 : //conditional_access_section
                        if (Complete_Stream->Streams[CA_PID].Kind!=complete_stream::stream::psi)
                        {
                            Complete_Stream->Streams[CA_PID].Kind=complete_stream::stream::psi;
                            #ifdef MEDIAINFO_MPEGTS_ALLSTREAMS_YES
                                Complete_Stream->Streams[CA_PID].Searching_Payload_Start_Set(true);
                            #endif
                        }
                        break;
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            if (Complete_Stream->Streams[CA_PID].Kind!=complete_stream::stream::psi)
                            {
                                Complete_Stream->Streams[CA_PID].Kind=complete_stream::stream::psi;
                                #ifdef MEDIAINFO_MPEGTS_ALLSTREAMS_YES
                                    Complete_Stream->Streams[CA_PID].Searching_Payload_Start_Set(true);
                                #endif
                            }
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_0A()
{
    //Parsing
    Ztring ISO_639_language_code;
    int8u audio_type;
    Get_Local(3, ISO_639_language_code,                         "ISO_639_language_code");
    Get_B1 (audio_type,                                         "audio_type"); Param_Info(Mpeg_Descriptors_audio_type(audio_type));

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].Infos["Language"]=ISO_639_language_code;
                            if (audio_type)
                                Complete_Stream->Streams[xxx_id].Infos["Language_More"]=Mpeg_Descriptors_audio_type(audio_type);
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_0E()
{
    //Parsing
    int32u maximum_bitrate;
    BS_Begin();
    Skip_S1( 2,                                                 "reserved");
    Get_S3 (22, maximum_bitrate,                                "maximum_bitrate"); Param_Info(maximum_bitrate*400, " bps");
    BS_End();

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                            Complete_Stream->Streams[xxx_id].Infos["BitRate_Maximum"]=Ztring::ToZtring(maximum_bitrate*400);
                        else
                            Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].Programs[table_id_extension].Infos["BitRate_Maximum"]=Ztring::ToZtring(maximum_bitrate*400);
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_0F()
{
    //Parsing
    Skip_B4(                                                    "private_data_indicator");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_10()
{
    //Parsing
    BS_Begin();
    Skip_S1( 2,                                                 "reserved");
    Info_S3(22, sb_leak_rate,                                   "sb_leak_rate"); Param_Info(sb_leak_rate*400, " bps");
    Skip_S1( 2,                                                 "reserved");
    Info_S3(22, sb_size,                                        "sb_size"); Param_Info(sb_size, " bytes");
    BS_End();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_11()
{
    //Parsing
    BS_Begin();
    Skip_S1( 7,                                                 "reserved");
    Skip_SB(                                                    "leak_valid_flag");
    BS_End();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_1D()
{
    //Parsing
    int8u IOD_label;
    Skip_B1(                                                    "Scope_of_IOD_label");
    Get_B1 (IOD_label,                                          "IOD_label");

    #ifdef MEDIAINFO_MPEG4_YES
        if (Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].IOD_ESs.find(IOD_label)==Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].IOD_ESs.end())
        {
            File_Mpeg4_Descriptors MI;
            MI.Parser_DoNotFreeIt=true;
            Open_Buffer_Init(&MI);
            Open_Buffer_Continue(&MI, Buffer+Buffer_Offset+(size_t)Element_Offset, (size_t)(Element_Size-Element_Offset));
            Open_Buffer_Finalize(&MI);
            Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].IOD_ESs[MI.ES_ID].Parser=MI.Parser;
        }
    #else
        Skip_XX(Element_Size-Element_Offset,                    "MPEG-4 Descriptor");
    #endif
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_1F()
{
    //Parsing
    int16u ES_ID;
    while (Element_Offset<Element_Size)
    {
        Element_Begin("FlexMux");
        Get_B2 (ES_ID,                                          "ES_ID");
        if (Element_Offset!=Element_Size)
            Skip_B1(                                            "FlexMuxChannel");
        Element_End();

        FILLING_BEGIN();
            switch (table_id)
            {
                case 0x02 : //program_map_section
                            if (xxx_id_IsValid)
                            {
                                Complete_Stream->Streams[xxx_id].FMC_ES_ID=ES_ID;
                                Complete_Stream->Streams[xxx_id].FMC_ES_ID_IsValid=true;
                            }
                            break;
                default    : ;
            }
        FILLING_END();
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_28()
{
    //Parsing
    int8u profile_idc, level_idc;
    Get_B1 (profile_idc,                                        "profile_idc"); Param_Info(Avc_profile_idc(profile_idc));
    BS_Begin();
    Element_Begin("constraints");
        Skip_SB(                                                "constraint_set0_flag");
        Skip_SB(                                                "constraint_set1_flag");
        Skip_SB(                                                "constraint_set2_flag");
        Skip_SB(                                                "constraint_set3_flag");
        Skip_SB(                                                "reserved_zero_4bits");
        Skip_SB(                                                "reserved_zero_4bits");
        Skip_SB(                                                "reserved_zero_4bits");
        Skip_SB(                                                "reserved_zero_4bits");
    Element_End();
    BS_End();
    Get_B1 (level_idc,                                          "level_idc");
    BS_Begin();
    Skip_SB(                                                    "AVC_still_present");
    Skip_SB(                                                    "AVC_24_hour_picture_flag");
    Skip_S1(6,                                                  "reserved");
    BS_End();

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].Infos["Format"]=_T("AVC");
                            Complete_Stream->Streams[xxx_id].Infos["Format_Profile"]=Ztring().From_Local(Avc_profile_idc(profile_idc))+_T("@")+Ztring().From_Number(((float)level_idc)/10, 1);
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_2F()
{
    //Parsing
    int8u aux_video_params_length;
    Skip_B1(                                                    "aux_video_type"); //ISO/IEC 23002-3
    Get_B1 (aux_video_params_length,                            "aux_video_params_length");
    Skip_XX(aux_video_params_length,                            "aux_video_params");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_40()
{
    //Parsing
    Ztring network_name;
    Get_DVB_Text(Element_Size, network_name,                    "network_name");

    FILLING_BEGIN();
        Complete_Stream->network_name=network_name;
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_41()
{
    //Parsing
    while (Element_Offset<Element_Size)
    {
        Element_Begin("service");
        int16u service_id;
        int8u service_type;
        Get_B2 (service_id,                                     "service_id"); Element_Info(Ztring::ToZtring(service_id, 16));
        Get_B1 (service_type,                                   "service_type"); Param_Info(Mpeg_Descriptors_dvb_service_type(service_type));
        Element_End(service_id);

        FILLING_BEGIN();
            Complete_Stream->Transport_Streams[table_id_extension].Programs[service_id].Infos["ServiceType"]=Mpeg_Descriptors_dvb_service_type(service_type);
        FILLING_END();
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_43()
{
    //Parsing
    int32u frequency, symbol_rate;
    int16u orbital_position;
    int8u polarization, roll_off, modulation_type, FEC_inner;
    bool west_east_flag, modulation_system;
    Get_B4 (frequency,                                          "frequency"); Param_Info(Frequency_DVB__BCD(frequency));
    Get_B2 (orbital_position,                                   "orbital_position"); Param_Info(OrbitalPosition_DVB__BCD(orbital_position));
    BS_Begin();
    Get_SB (    west_east_flag,                                 "west_east_flag"); Param_Info(west_east_flag?"E":"W");
    Get_S1 ( 2, polarization,                                   "polarization");
    Get_S1 ( 2, roll_off,                                       "roll_off");
    Get_SB (    modulation_system,                              "modulation_system");
    Get_S1 ( 2, modulation_type,                                "modulation_type");
    Get_S4 (28, symbol_rate,                                    "symbol_rate");
    Get_S1 ( 4, FEC_inner,                                      "FEC_inner");
    BS_End();

    FILLING_BEGIN();
        Complete_Stream->Transport_Streams[transport_stream_id].Infos["Frequency"]=Frequency_DVB__BCD(frequency);
        Complete_Stream->Transport_Streams[transport_stream_id].Infos["OrbitalPosition"]=OrbitalPosition_DVB__BCD(orbital_position)+(west_east_flag?_T('E'):_T('W'));
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_48()
{
    //Parsing
    Ztring service_provider_name, service_name;
    int8u service_type, service_provider_name_length, service_name_length;
    Get_B1 (service_type,                                       "service_type"); Param_Info(Mpeg_Descriptors_dvb_service_type(service_type));
    Get_B1 (service_provider_name_length,                       "service_provider_name_length");
    Get_DVB_Text(service_provider_name_length, service_provider_name, "service_provider_name");
    Get_B1 (service_name_length,                                "service_name_length");
    Get_DVB_Text(service_name_length, service_name,             "service_name");

    //Filling
    FILLING_BEGIN();
        Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceName"]=service_name;
        Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceProvider"]=service_provider_name;
        Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceType"]=Mpeg_Descriptors_dvb_service_type(service_type);
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_4A()
{
    //Parsing
    int8u linkage_type;
    Skip_B2(                                                    "transport_stream_id");
    Info_B2(original_network_id,                                "original_network_id"); Param_Info(Mpeg_Descriptors_original_network_id(original_network_id));
    Skip_B2(                                                    "service_id");
    Get_B1 (linkage_type,                                       "linkage_type"); Param_Info(Mpeg_Descriptors_linkage_type(linkage_type));
    if (Element_Size>7)
        Skip_XX(Element_Size-7,                                 "private_data");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_4D()
{
    //Parsing
    Ztring event_name, text;
    int8u event_name_length, text_length;
    Skip_Local(3,                                               "ISO_639_language_code");
    Get_B1 (event_name_length,                                  "event_name_length");
    Get_DVB_Text(event_name_length, event_name,                 "event_name"); Element_Info(event_name);
    Get_B1 (text_length,                                        "text_length");
    Get_DVB_Text(text_length, text,                             "text");

    FILLING_BEGIN();
        if (table_id>=0x4E && table_id<=0x6F) //event_information_section
        {
            if (xxx_id_IsValid)
            {
                Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].short_event.event_name=event_name;
                Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].short_event.text=text;
                Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks_IsUpdated=true;
                IsUpdated=true;
            }
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_50()
{
    //Parsing
    Ztring ISO_639_language_code;
    BS_Begin();
    Skip_S1(4,                                                  "reserved_future_use");
    Info_S1(4, stream_content,                                  "stream_content"); Param_Info(Mpeg_Descriptors_stream_content(stream_content)); Element_Info(Mpeg_Descriptors_stream_content(stream_content));
    BS_End();
    Info_B1(component_type,                                     "component_type"); Param_Info(Mpeg_Descriptors_component_type(stream_content, component_type)); Element_Info(Mpeg_Descriptors_component_type(stream_content, component_type));
    Info_B1(component_tag,                                      "component_tag");
    Get_Local(3, ISO_639_language_code,                         "ISO_639_language_code");
    Skip_DVB_Text(Element_Size-Element_Offset,                  "text");

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                            Complete_Stream->Streams[xxx_id].Infos["Language"]=ISO_639_language_code;
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_52()
{
    //Parsing
    Skip_B1(                                                    "component_tag");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_54()
{
    //Parsing
    while (Element_Offset<Element_Size)
    {
        BS_Begin();
        Info_S1(4, content_nibble_level_1,                      "content_nibble_level_1"); Param_Info(Mpeg_Descriptors_content_nibble_level_1(content_nibble_level_1)); Element_Info(Mpeg_Descriptors_content_nibble_level_1(content_nibble_level_1));
        Info_S1(4, content_nibble_level_2,                      "content_nibble_level_2"); Param_Info(Mpeg_Descriptors_content_nibble_level_2(content_nibble_level_1, content_nibble_level_2)); if (content_nibble_level_1==0xB || content_nibble_level_2!=0) Element_Info(Mpeg_Descriptors_content_nibble_level_2(content_nibble_level_1, content_nibble_level_2));
        Skip_S1(4,                                              "user_nibble");
        Skip_S1(4,                                              "user_nibble");
        BS_End();

        FILLING_BEGIN();
            Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].content=Ztring().From_UTF8(Mpeg_Descriptors_content_nibble_level_2(content_nibble_level_1, content_nibble_level_2))+_T(", ");
            Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks_IsUpdated=true;
            IsUpdated=true;
        FILLING_END();
    }

    FILLING_BEGIN();
        if (!Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].content.empty())
        {
            Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].content.resize(Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks[table_id].Events[xxx_id].content.size()-2);
            Complete_Stream->Transport_Streams[transport_stream_id].Programs[table_id_extension].DVB_EPG_Blocks_IsUpdated=true;
            IsUpdated=true;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_55()
{
    //Parsing
    while (Element_Offset<Element_Size)
    {
        Skip_Local(3,                                           "country_code");
        Info_B1(rating,                                         "rating"); Param_Info(rating+3, " years old"); Element_Info(rating+3, " years old");
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_56()
{
    //Parsing
    Ztring Languages;
    while (Element_Offset<Element_Size)
    {
        Element_Begin("teletext");
        Ztring ISO_639_language_code;
        Get_Local(3, ISO_639_language_code,                         "ISO_639_language_code");
        BS_Begin();
        Info_S1(5, teletext_type,                               "teletext_type"); Param_Info(Mpeg_Descriptors_teletext_type(teletext_type));
        Skip_S1(3,                                              "teletext_magazine_number");
        Skip_S1(4,                                              "teletext_page_number_1");
        Skip_S1(4,                                              "teletext_page_number_2");
        BS_End();

        FILLING_BEGIN();
            switch (table_id)
            {
                case 0x02 : //program_map_section
                            if (xxx_id_IsValid)
                                Languages+=ISO_639_language_code+_T(" / ");
                                //TODO: this stream is teletext. Be careful, multiple stream in a PID
                            break;
                default    : ;
            }
        FILLING_END();

        Element_End();
    }

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].StreamKind=Stream_Text;
                            if (!Languages.empty())
                                Languages.resize(Languages.size()-3);
                            Complete_Stream->Streams[xxx_id].Infos["Language"]=Languages;
                            Complete_Stream->Streams[xxx_id].Infos["Format"]=_T("Teletext");
                            Complete_Stream->Streams[xxx_id].Infos["Codec"]=_T("Teletext");
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_58()
{
    //Parsing
    while (Element_Offset<Element_Size)
    {
        int32u country_code;
        int8u country_region_id;
        Get_C3 (country_code,                                       "country_code");
        BS_Begin();
        Get_S1 (6, country_region_id,                               "country_region_id");
        Skip_SB(                                                    "reserved");
        Info_SB(local_time_offset_polarity,                         "local_time_offset_polarity"); Param_Info(local_time_offset_polarity?"-":"+");
        BS_End();
        Info_B2(local_time_offset,                                  "local_time_offset"); Param_Info(TimeHHMM_BCD(local_time_offset));
        Info_B2(date,                                               "time_of_change (date)"); Param_Info(Date_MJD(date));
        Info_B3(time,                                               "time_of_change (time)"); Param_Info(Time_BCD(time));
        Info_B2(next_time_offset,                                   "next_time_offset"); Param_Info(TimeHHMM_BCD(next_time_offset));

        FILLING_BEGIN();
            Ztring Country; Country.From_CC3(country_code);
            if (country_region_id)
                Country+=_T(" (")+Ztring::ToZtring(country_region_id)+_T(")");
            Complete_Stream->TimeZones[Country]=(local_time_offset_polarity?_T('-'):_T('+'))+TimeHHMM_BCD(local_time_offset);
        FILLING_END();
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_59()
{
    //Parsing
    Ztring Languages;
    while (Element_Offset<Element_Size)
    {
        Element_Begin("subtitle");
        Ztring ISO_639_language_code;
        Get_Local(3, ISO_639_language_code,                         "ISO_639_language_code");
        Info_B1(subtitling_type,                                    "subtitling_type"); Param_Info(Mpeg_Descriptors_component_type_O3(subtitling_type));
        Skip_B2(                                                    "composition_page_id");
        Skip_B2(                                                    "ancillary_page_id");

        FILLING_BEGIN();
            switch (table_id)
            {
                case 0x02 : //program_map_section
                            if (xxx_id_IsValid)
                            {
                                Languages+=ISO_639_language_code+_T(" / ");
                                //TODO: this stream is teletext. Be careful, multiple stream in a PID
                            }
                            break;
                default    : ;
            }
        FILLING_END();
        
        Element_End();
    }

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].StreamKind=Stream_Text;
                            if (!Languages.empty())
                                Languages.resize(Languages.size()-3);
                            Complete_Stream->Streams[xxx_id].Infos["Language"]=Languages;
                            Complete_Stream->Streams[xxx_id].Infos["Format"]=_T("DVB Subtitles");
                            Complete_Stream->Streams[xxx_id].Infos["Codec"]=_T("DVB Subtitles");
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_5A()
{
    //Parsing
    Info_B4(centre_frequency,                                   "centre_frequency"); Param_Info(((int64u)centre_frequency)*10, " Hz");
    BS_Begin();
    Info_S1(3, bandwidth,                                       "bandwidth"); Param_Info(Mpeg_Descriptors_bandwidth[bandwidth]);
    Info_SB(   priority,                                        "priority"); Param_Info(priority?"HP":"LP");
    Skip_SB(                                                    "Time_Slicing_indicator");
    Skip_SB(                                                    "MPE-FEC_indicator");
    Skip_S1(2,                                                  "reserved");
    Info_S1(2, constellation,                                   "constellation"); Param_Info(Mpeg_Descriptors_constellation[constellation]);
    Info_S1(3, hierarchy_information,                           "hierarchy_information"); Param_Info(Mpeg_Descriptors_hierarchy_information[hierarchy_information]);
    Info_S1(3, code_rate_HP,                                    "code_rate-HP_stream"); Param_Info(Mpeg_Descriptors_code_rate[code_rate_HP]);
    Info_S1(3, code_rate_LP,                                    "code_rate-LP_stream"); Param_Info(Mpeg_Descriptors_code_rate[code_rate_LP]);
    Info_S1(2, guard_interval,                                  "guard_interval"); Param_Info(Mpeg_Descriptors_guard_interval[guard_interval]);
    Info_S1(2, transmission_mode,                               "transmission_mode"); Param_Info(Mpeg_Descriptors_transmission_mode[transmission_mode]);
    Skip_SB(                                                    "other_frequency_flag");
    BS_End();
    Skip_B4(                                                    "reserved");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_5D()
{
    //Parsing
    Ztring ServiceProvider, ServiceName;
    while (Element_Offset<Element_Size)
    {
        Ztring service_provider_name, service_name;
        int32u ISO_639_language_code;
        int8u  service_provider_name_length, service_name_length;
        Get_C3 (ISO_639_language_code,                          "ISO_639_language_code");
        Get_B1 (service_provider_name_length,                   "service_provider_name_length");
        Get_DVB_Text(service_provider_name_length, service_provider_name, "service_provider_name");
        Get_B1 (service_name_length,                            "service_name_length");
        Get_DVB_Text(service_name_length, service_name,         "service_name");

        //Filling
        FILLING_BEGIN();
            ServiceProvider+=Ztring().From_CC3(ISO_639_language_code)+_T(':')+service_provider_name+_T( " - ");
            ServiceName+=Ztring().From_CC3(ISO_639_language_code)+_T(':')+service_name+_T( " - ");
        FILLING_END();
    }

    if (!ServiceProvider.empty())
    {
        ServiceProvider.resize(ServiceProvider.size()-3);
        Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceProvider"]=ServiceProvider;
    }
    if (!ServiceName.empty())
    {
        ServiceName.resize(ServiceName.size()-3);
        Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceName"]=ServiceName;
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_5F()
{
    //Parsing
    Info_B4(private_data_specifier,                             "private_data_specifier"); Param_Info(Ztring().From_CC4(private_data_specifier));
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_63()
{
    //Parsing
    int32u peak_rate;
    BS_Begin();
    Skip_S1( 2,                                                 "DVB_reserved_future_use");
    Get_S3 (22, peak_rate,                                      "peak_rate");
    Skip_S1( 2,                                                 "DVB_reserved_future_use");
    Skip_S3(22,                                                 "minimum_overall_smoothing_rate");
    Skip_S1( 2,                                                 "DVB_reserved_future_use");
    Skip_S2(14,                                                 "maximum_overall_smoothing_buffer");
    BS_End();

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                            Complete_Stream->Streams[xxx_id].Infos["OverallBitRate_Maximum"]=Ztring::ToZtring(peak_rate*400);
                        break;
            case 0x7F : //selection_information_section
                        if (!xxx_id_IsValid)
                            Complete_Stream->Transport_Streams[Complete_Stream->transport_stream_id].Infos["OverallBitRate_Maximum"]=Ztring::ToZtring(peak_rate*400);
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_66()
{
    //Parsing
    Ztring ISO_639_language_code;
    int8u selector_length, text_length;
    Skip_B2(                                                    "data_broadcast_id");
    Skip_B1(                                                    "component_tag");
    Get_B1 (selector_length,                                    "selector_length");
    Skip_XX(selector_length,                                    "selector_bytes");
    Get_Local(3, ISO_639_language_code,                         "ISO_639_language_code");
    Get_B1 (text_length,                                        "text_length");
    Skip_Local(text_length,                                     "text_chars");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_6A()
{
    //Parsing
    BS_Begin();
    bool component_type_flag, bsid_flag, mainid_flag, asvc_flag, enhanced_ac3=false;
    Get_SB (   component_type_flag,                             "component_type_flag");
    Get_SB (   bsid_flag,                                       "bsid_flag");
    Get_SB (   mainid_flag,                                     "mainid_flag");
    Get_SB (   asvc_flag,                                       "asvc_flag");
    Skip_SB(                                                    "reserved_flag");
    Skip_SB(                                                    "reserved_flag");
    Skip_SB(                                                    "reserved_flag");
    Skip_SB(                                                    "reserved_flag");
    BS_End();
    if (component_type_flag)
    {
        int8u service_type, number_of_channels;
        BS_Begin();
        Get_SB (   enhanced_ac3,                                "enhanced AC-3");
        Skip_SB(                                                "full_service");
        Get_S1 (3, service_type,                                "service_type"); Param_Info(AC3_Mode[service_type]);
        Get_S1 (3, number_of_channels,                          "number_of_channels"); Param_Info(Mpeg_Descriptors_AC3_Channels[number_of_channels], " channels");
        BS_End();

        FILLING_BEGIN();
            switch (table_id)
            {
                case 0x02 : //program_map_section
                            if (xxx_id_IsValid)
                                Complete_Stream->Streams[xxx_id].Infos["Channel(s)"]=Ztring().From_Local(Mpeg_Descriptors_AC3_Channels[number_of_channels]);
                            break;
                default    : ;
            }
        FILLING_END();
    }
    if (bsid_flag)
    {
        BS_Begin();
        Skip_S1(3,                                              "zero");
        Skip_S1(5,                                              "bsid");
        BS_End();
    }
    if (mainid_flag)
    {
        Skip_B1(                                                "mainid");
    }
    if (asvc_flag)
    {
        Skip_B1(                                                "asvc");
    }

    FILLING_BEGIN(); //Can be more
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].StreamKind=Stream_Audio;
                            Complete_Stream->Streams[xxx_id].Infos["Format"]=enhanced_ac3?_T("E-AC-3"):_T("AC-3");
                            Complete_Stream->Streams[xxx_id].Infos["Codec"]=_T("AC3+");
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_7A()
{
    //Parsing
    bool component_type_flag, bsid_flag, mainid_flag, asvc_flag, mixinfoexists, substream1_flag, substream2_flag, substream3_flag, enhanced_ac3=0;
    BS_Begin();
    Get_SB (  component_type_flag,                             "component_type_flag");
    Get_SB (  bsid_flag,                                       "bsid_flag");
    Get_SB (  mainid_flag,                                     "mainid_flag");
    Get_SB (  asvc_flag,                                       "asvc_flag");
    Get_SB (  mixinfoexists,                                   "mixinfoexists");
    Get_SB (  substream1_flag,                                 "substream1_flag");
    Get_SB (  substream2_flag,                                 "substream2_flag");
    Get_SB (  substream3_flag,                                 "substream3_flag");
    BS_End();
    if (component_type_flag)
    {
        int8u service_type, number_of_channels;
        BS_Begin();
        Get_SB (  enhanced_ac3,                                "enhanced AC-3");
        Skip_SB(                                               "full_service");
        Get_S1 (3, service_type,                               "service_type"); Param_Info(AC3_Mode[service_type]);
        Get_S1 (3, number_of_channels,                         "number_of_channels"); Param_Info(Mpeg_Descriptors_AC3_Channels[number_of_channels], " channels");
        FILLING_BEGIN();
            switch (table_id)
            {
                case 0x02 : //program_map_section
                            if (xxx_id_IsValid)
                                Complete_Stream->Streams[xxx_id].Infos["Channel(s)"]=Ztring().From_Local(Mpeg_Descriptors_AC3_Channels[number_of_channels]);
                            break;
                default    : ;
            }
        FILLING_END();
        BS_End();
    }
    if (bsid_flag)
    {
        BS_Begin();
        Skip_S1(3,                                              "zero");
        Skip_S1(5,                                              "bsid");
        BS_End();
    }
    if (mainid_flag)
    {
        Skip_B1(                                                "mainid");
    }
    if (asvc_flag)
    {
        Skip_B1(                                                "asvc");
    }
    if (substream1_flag)
    {
        Skip_B1(                                                "substream1");
    }
    if (substream2_flag)
    {
        Skip_B1(                                                "substream2");
    }
    if (substream3_flag)
    {
        Skip_B1(                                                "substream3");
    }

    FILLING_BEGIN(); //Can be more
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            Complete_Stream->Streams[xxx_id].StreamKind=Stream_Audio;
                            Complete_Stream->Streams[xxx_id].Infos["Format"]=enhanced_ac3?_T("E-AC-3"):_T("AC-3");
                            Complete_Stream->Streams[xxx_id].Infos["Codec"]=_T("AC3+");
                        }
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_7B()
{
    //Parsing
    BS_Begin();
    Skip_S1(6,                                                  "bit_rate_code");
    Skip_S2(7,                                                  "nblks");
    Skip_S2(14,                                                 "fsize");
    Skip_S1(6,                                                  "surround_mode");
    Skip_SB(                                                    "lfe_flag");
    Skip_S1(2,                                                  "extended_surround_flag");
    BS_End();
    //BS_End_CANBEMORE();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_7C()
{
    //Parsing
    bool AAC_type_flag;
    Skip_B1(                                                    "Profile_and_level");
    BS_Begin();
    Get_SB (   AAC_type_flag,                                   "AAC_type_flag");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    Skip_SB(                                                    "reserved");
    BS_End();
    if (AAC_type_flag)
    {
        Skip_B1(                                                "AAC_type");
    }
    //BS_End_CANBEMORE();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_81()
{
    //Parsing
    Ztring Text, Language1, Language2;
    int8u sample_rate_code, bit_rate_code, surround_mode, bsmod, num_channels, langcod, textlen, text_code;
    bool language_flag, language_flag_2;
    BS_Begin();
    Get_S1 (3, sample_rate_code,                                "sample_rate_code"); if (sample_rate_code<4) {Param_Info(AC3_SamplingRate[sample_rate_code], " Hz");}
    Skip_S1(5,                                                  "bsid");
    Get_S1 (6, bit_rate_code,                                   "bit_rate_code"); Param_Info(AC3_BitRate[bit_rate_code]*1000, " Kbps");
    Get_S1 (2, surround_mode,                                   "surround_mode"); Param_Info(AC3_Surround[surround_mode]);
    Get_S1 (3, bsmod,                                           "bsmod");
    Get_S1 (4, num_channels,                                    "num_channels"); if (num_channels<8) {Param_Info(AC3_Channels[num_channels], " channels");}
    Skip_SB(                                                    "full_svc");
    BS_End();

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                        {
                            if (sample_rate_code<4)
                                Complete_Stream->Streams[xxx_id].Infos["SamplingRate"]=Ztring::ToZtring(AC3_SamplingRate[sample_rate_code]);
                            Complete_Stream->Streams[xxx_id].Infos["BitRate"]=Ztring::ToZtring(AC3_BitRate[bit_rate_code]*1000);
                            if (num_channels<8)
                                Complete_Stream->Streams[xxx_id].Infos["Channel(s)"]=Ztring::ToZtring(AC3_Channels[num_channels]);
                        }
        }
    FILLING_END();

    //Parsing
    if (Element_Offset==Element_Size) return;
    Get_B1 (langcod,                                            "langcod");

    //Parsing
    if (Element_Offset==Element_Size) return;
    if (num_channels==0) //1+1 mode
        Skip_B1(                                                "langcod2");

    //Parsing
    if (Element_Offset==Element_Size) return;
    if (bsmod<2)
    {
        BS_Begin();
        Skip_S1(3,                                              "mainid");
        Info_BS(2, priority,                                    "priority"); Param_Info(Mpeg_Descriptors_AC3_Priority[priority]);
        Skip_S1(3,                                              "reserved");
        BS_End();
    }
    else
        Skip_B1(                                                "asvcflags");

    //Parsing
    if (Element_Offset==Element_Size) return;
    BS_Begin();
    Get_S1 (7, textlen,                                         "textlen");
    Get_S1 (1, text_code,                                       "text_code"); if (text_code) Param_Info("Unicode");
    BS_End();
    if (textlen)
        Get_Local(textlen, Text,                                "text");

    //Parsing
    if (Element_Offset==Element_Size) return;
    BS_Begin();
    Get_SB (   language_flag,                                   "language_flag");
    Get_SB (   language_flag_2,                                 "language_flag_2");
    Skip_S1(6,                                                  "reserved");
    BS_End();

    //Parsing
    if (Element_Offset==Element_Size) return;
    if (language_flag)
        Get_Local(3, Language1,                                 "language1");

    //Parsing
    if (Element_Offset==Element_Size) return;
    if (language_flag_2)
        Get_Local(3, Language2,                                 "language2");

    //Parsing
    if (Element_Offset==Element_Size) return;
    Skip_XX(Element_Size-Element_Offset,                        "additional_info");
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_86()
{
    //Parsing
    Ztring Text, Language1, Language2;
    int8u number_of_services;
    BS_Begin();
    Skip_S1(3,                                                  "reserved");
    Get_S1 (5, number_of_services,                              "number_of_services");
    BS_End();
    for (int8u Pos=0; Pos<number_of_services; Pos++)
    {
        Element_Begin("service");
        bool digital_cc;
        Skip_C3(                                                "language");
        BS_Begin();
        Get_SB (digital_cc,                                     "digital_cc");
        Skip_SB(                                                "reserved");
        if (digital_cc) //line21
        {
            Skip_S1(5,                                          "reserved");
            Skip_SB(                                            "line21_field");
        }
        else
            Skip_S1(6,                                          "caption_service_number");
        Skip_SB(                                                "easy_reader");
        Skip_SB(                                                "wide_aspect_ratio");
        Skip_S2(14,                                             "reserved");
        BS_End();
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_87()
{
    //Parsing
    int8u rating_region_count;
    BS_Begin();
    Skip_S1(2,                                                  "reserved");
    Get_S1 (6, rating_region_count,                             "rating_region_count");
    BS_End();
    for (int8u Pos=0; Pos<rating_region_count; Pos++)
    {
        Element_Begin("rating_region");
        int8u rated_dimensions;
        Skip_B1(                                                "rating_region");
        Get_B1 (rated_dimensions,                               "rated_dimensions");
        for (int8u Pos=0; Pos<rated_dimensions; Pos++)
        {
            Element_Begin("rated_dimension");
            Skip_B1(                                            "rating_dimension_j");
            BS_Begin();
            Skip_S1(4,                                          "reserved");
            Skip_S1(4,                                          "rating_value");
            BS_End();
            Element_End();
        }
        Element_End();
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_A0()
{
    //Parsing
    Ztring title;
    ATSC_multiple_string_structure(title,                       "title");

    FILLING_BEGIN(); //Can be more
        switch (table_id)
        {
            case 0xC8 : //TVCT
            case 0xC9 : //CVCT
            case 0xDA : //SVCT
                        if (xxx_id_IsValid)
                            if (!title.empty())
                                Complete_Stream->Transport_Streams[table_id_extension].Programs[xxx_id].Infos["ServiceName"]=title;
                        break;
            default    : ;
        }
    FILLING_END();
    FILLING_BEGIN();
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_A1()
{
    //Parsing
    int8u number_elements;
    BS_Begin();
    Skip_S1( 3,                                                 "reserved");
    Skip_S2(13,                                                 "PCR_PID");
    BS_End();
    Get_B1 (    number_elements,                                "number_elements");
    for (int8u Pos=0; Pos<number_elements; Pos++)
    {
        Element_Begin();
        int16u elementary_PID;
        Skip_B1(                                                "stream_type");
        BS_Begin();
        Skip_S1( 3,                                             "reserved");
        Get_S2 (13, elementary_PID,                             "elementary_PID");
        BS_End();
        Skip_Local(3,                                           "ISO_639_language_code");
        Element_End(Ztring().From_CC2(elementary_PID), 6);
    }
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_A3()
{
    //Parsing
    Ztring Value;
    ATSC_multiple_string_structure(Value,                       "name");

    FILLING_BEGIN();
        switch (table_id)
        {
            case 0x02 : //program_map_section
                        if (xxx_id_IsValid)
                            if (!Value.empty())
                                Complete_Stream->Streams[xxx_id].Infos["Name"]=Value;
                        break;
            default    : ;
        }
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Descriptor_AA()
{
    //Parsing
    Skip_XX(Element_Size,                                       "rc_information");
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::ATSC_multiple_string_structure(Ztring &Value, const char* Info)
{
    //Parsing
    Ztring string;
    int8u number_strings, number_segments;
    Element_Begin(Info);
    Get_B1(number_strings,                                      "number_strings");
    for (int8u Pos=0; Pos<number_strings; Pos++)
    {
        Element_Begin("String");
        int32u ISO_639_language_code;
        Get_C3(ISO_639_language_code,                           "ISO_639_language_code");
        Get_B1(number_segments,                                 "number_segments");
        for (int8u Pos=0; Pos<number_segments; Pos++)
        {
            Element_Begin("Segment");
            Ztring segment;
            int8u compression_type, mode, number_bytes;
            Get_B1 (compression_type,                           "compression_type");
            Get_B1 (mode,                                       "mode");
            Get_B1 (number_bytes,                               "number_bytes");
            switch (compression_type)
            {
                case 0x00 :
                            switch (mode)
                            {
                                case 0x00 : Get_Local(number_bytes, segment, "string"); break;
                                case 0x3F : Get_UTF16B(number_bytes, segment, "string"); break;
                                default   : Skip_XX(number_bytes, "Unknown");
                                            segment=_T("(Encoded with mode=0x")+Ztring::ToZtring(mode, 16)+_T(')');
                            }
                            break;
                default   : Skip_XX(number_bytes,               "(Compressed)");
                            segment=_T("(Compressed)");
            }
            Element_End(3+number_bytes);

            FILLING_BEGIN();
                string+=segment+_T(" - ");
            FILLING_END();
        }

        FILLING_BEGIN();
            if (!string.empty())
                string.resize(string.size()-3);
            Value+=Ztring().From_CC3(ISO_639_language_code)+_T(':')+string+_T(" - ");
        FILLING_END();

        Element_Info(string);
        Element_End("String");
    }

    if (!Value.empty())
        Value.resize(Value.size()-3);

    Element_Info(Value);
    Element_End();
}

//---------------------------------------------------------------------------
void File_Mpeg_Descriptors::Get_DVB_Text(int64u Size, Ztring &Value, const char* Info)
{
    if (Size<1)
    {
        Get_Local(Size, Value,                                  Info);
        return;
    }

    //Testing if there is a codepage
    int8u CodePage1;
    Peek_B1(CodePage1);
    if (CodePage1<0x20)
    {
        Skip_B1(                                                "CodePage"); Param_Info(Mpeg_Descriptors_codepage_1(CodePage1));
        if (CodePage1!=0x10)
        {
            Get_Local(Size-1, Value,                            Info);
        }
        else
        {
            if (Size<3)
            {
                Value.clear();
                return;
            }
            Skip_B2(                                            "CodePage2");
            Get_Local(Size-3, Value,                            Info);
        }

    }
    else
        Get_Local(Size, Value,                                  Info);
}

//---------------------------------------------------------------------------
//Modified Julian Date
Ztring File_Mpeg_Descriptors::Date_MJD(int16u Date_)
{
    //Calculating
    float64 Date=Date_;
    int Y2=(int)((Date-15078.2)/365.25);
    int M2=(int)(((Date-14956.1) - ((int)(Y2*365.25))) /30.6001);
    int D =(int)(Date-14956 - ((int)(Y2*365.25)) - ((int)(M2*30.6001)));
    int K=0;
    if (M2==14 || M2==15)
        K=1;
    int Y =Y2+K;
    int M =M2-1-K*12;

    //Formating
    return                       Ztring::ToZtring(1900+Y)+_T("-")
         + (M<10?_T("0"):_T(""))+Ztring::ToZtring(     M)+_T("-")
         + (D<10?_T("0"):_T(""))+Ztring::ToZtring(     D);
}

//---------------------------------------------------------------------------
//Form: HHMMSS, BCD
Ztring File_Mpeg_Descriptors::Time_BCD(int32u Time)
{
    return (((Time>>16)&0xFF)<10?_T("0"):_T("")) + Ztring::ToZtring((Time>>16)&0xFF, 16)+_T(":") //BCD
         + (((Time>> 8)&0xFF)<10?_T("0"):_T("")) + Ztring::ToZtring((Time>> 8)&0xFF, 16)+_T(":") //BCD
         + (((Time    )&0xFF)<10?_T("0"):_T("")) + Ztring::ToZtring((Time    )&0xFF, 16);        //BCD
}

//---------------------------------------------------------------------------
//Form: HHMM, BCD
Ztring File_Mpeg_Descriptors::TimeHHMM_BCD(int16u Time)
{
    return (((Time>> 8)&0xFF)<10?_T("0"):_T("")) + Ztring::ToZtring((Time>> 8)&0xFF, 16)+_T(":") //BCD
         + (((Time    )&0xFF)<10?_T("0"):_T("")) + Ztring::ToZtring((Time    )&0xFF, 16)+_T(":00"); //BCD
}

//---------------------------------------------------------------------------
//Form: Frequency in 10 KHz
Ztring File_Mpeg_Descriptors::Frequency_DVB__BCD(int32u Frequency)
{
    int64u ToReturn=((((int64u)Frequency)>>28)&0xF)*10000000
                  + ((((int64u)Frequency)>>24)&0xF)* 1000000
                  + ((((int64u)Frequency)>>20)&0xF)*  100000
                  + ((((int64u)Frequency)>>16)&0xF)*   10000
                  + ((((int64u)Frequency)>>12)&0xF)*    1000
                  + ((((int64u)Frequency)>> 8)&0xF)*     100
                  + ((((int64u)Frequency)>> 4)&0xF)*      10
                  + ((((int64u)Frequency)    )&0xF)*       1;
    return Ztring::ToZtring(ToReturn*10000);
}

//---------------------------------------------------------------------------
//Form: Orbital Position
Ztring File_Mpeg_Descriptors::OrbitalPosition_DVB__BCD(int32u OrbitalPosition)
{
    int64u ToReturn=((OrbitalPosition>>12)&0xF)*    1000
                  + ((OrbitalPosition>> 8)&0xF)*     100
                  + ((OrbitalPosition>> 4)&0xF)*      10
                  + ((OrbitalPosition    )&0xF)         ;
    return Ztring::ToZtring(((float)ToReturn)/10, 1);
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEGTS_YES
