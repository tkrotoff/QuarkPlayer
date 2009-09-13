// File_Id3 - Info for ID3v2 tagged files
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

//***************************************************************************
// Infos (Common)
//***************************************************************************

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_ID3V2_YES) || defined(MEDIAINFO_FLAC_YES) || defined(MEDIAINFO_VORBISCOM_YES) || defined(MEDIAINFO_OGG_YES)
//---------------------------------------------------------------------------

#include "ZenLib/Conf.h"
using namespace ZenLib;

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern const char* Id3v2_PictureType(int8u Type)
{
    switch (Type)
    {
        case 0x01 :
        case 0x02 : return "File icon";
        case 0x03 : return "Cover (front)";
        case 0x04 : return "Cover (back)";
        case 0x05 : return "Leaflet page";
        case 0x06 : return "Media";
        case 0x07 :
        case 0x08 : return "Performer";
        case 0x09 : return "Conductor";
        case 0x0A : return "Performer";
        case 0x0B : return "Composer";
        case 0x0C : return "Lyricist";
        case 0x0D : return "Recording Location";
        case 0x0E : return "During recording";
        case 0x0F : return "During performance";
        case 0x10 : return "Screen capture";
        case 0x12 : return "Illustration";
        case 0x13 : return "Performer logo";
        case 0x14 : return "Publisher logo";
        default   : return "";
    }
}

} //NameSpace

//---------------------------------------------------------------------------
#endif //...
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
#if defined(MEDIAINFO_ID3V2_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Tag/File_Id3v2.h"
#include "ZenLib/ZtringListList.h"
#include "ZenLib/Utils.h"
#include "ZenLib/Base64/base64.h"
#include <cstring>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Infos
//***************************************************************************

//---------------------------------------------------------------------------
const char* Id3v2_TextEnc[]=
{
    "ISO 8859-1",
    "UTF-16",
    "UTF-16BE",
    "UTF-8",
};

//---------------------------------------------------------------------------
const char* Id3v2_RGAD_Name_code[]=
{
    "",
    "Radio Gain Adjustment",
    "Audiophile Gain Adjustment",
    "",
    "",
    "",
    "",
    "",
};

//---------------------------------------------------------------------------
const char* Id3v2_RGAD_Originator_code[]=
{
    "",
    "Pre-set by artist/producer/mastering engineer",
    "Set by user",
    "Determined automatically",
    "",
    "",
    "",
    "",
};

//***************************************************************************
// Constants
//***************************************************************************

namespace Elements
{
    const int32u AENC=0x41454E47;
    const int32u APIC=0x41504943;
    const int32u ASPI=0x41535049;
    const int32u COMM=0x434F4D4D;
    const int32u COMR=0x434F4D52;
    const int32u ENCR=0x454E4352;
    const int32u EQU2=0x45515532;
    const int32u EQUA=0x45515541;
    const int32u ETCO=0x4554434F;
    const int32u GEOB=0x47454F42;
    const int32u GRID=0x47524944;
    const int32u IPLS=0x49504C53;
    const int32u LINK=0x4C494E4B;
    const int32u MCDI=0x4D434449;
    const int32u MLLT=0x4D4C4C54;
    const int32u OWNE=0x4F574E45;
    const int32u PCNT=0x50434E58;
    const int32u POPM=0x504F504D;
    const int32u POSS=0x504F5353;
    const int32u PRIV=0x50524956;
    const int32u RBUF=0x52425546;
    const int32u RGAD=0x52474144;
    const int32u RVA2=0x52564132;
    const int32u RVRB=0x52565242;
    const int32u SEEK=0x5345454B;
    const int32u SIGN=0x5349474E;
    const int32u SYLT=0x53594C54;
    const int32u SYTC=0x53595443;
    const int32u TALB=0x54414C42;
    const int32u TBPM=0x5442504D;
    const int32u TCMP=0x54434D50;
    const int32u TCOM=0x54434F4D;
    const int32u TCON=0x54434F4E;
    const int32u TCOP=0x54434F50;
    const int32u TDAT=0x54444154;
    const int32u TDEN=0x5444454E;
    const int32u TDLY=0x54444C59;
    const int32u TDOR=0x54444F52;
    const int32u TDRC=0x54445243;
    const int32u TDRL=0x5444524C;
    const int32u TDTG=0x54445447;
    const int32u TENC=0x54454E43;
    const int32u TEXT=0x54455854;
    const int32u TFLT=0x54464C54;
    const int32u TIME=0x54494D45;
    const int32u TIPL=0x5449504C;
    const int32u TIT1=0x54495431;
    const int32u TIT2=0x54495432;
    const int32u TIT3=0x54495433;
    const int32u TKEY=0x544B4559;
    const int32u TLAN=0x544C414E;
    const int32u TLEN=0x544C454E;
    const int32u TMCL=0x544D434C;
    const int32u TMED=0x544D4544;
    const int32u TMOO=0x544D4F4F;
    const int32u TOAL=0x544F414C;
    const int32u TOFN=0x544F464E;
    const int32u TOLY=0x544F4C59;
    const int32u TOPE=0x544F5045;
    const int32u TORY=0x544F5259;
    const int32u TOWN=0x544F574E;
    const int32u TPE1=0x54504531;
    const int32u TPE2=0x54504532;
    const int32u TPE3=0x54504533;
    const int32u TPE4=0x54504534;
    const int32u TPOS=0x54504F53;
    const int32u TPRO=0x5450524F;
    const int32u TPUB=0x54505542;
    const int32u TRCK=0x5452434B;
    const int32u TRDA=0x54524441;
    const int32u TRSN=0x5452534E;
    const int32u TRSO=0x5452534F;
    const int32u TSIZ=0x5453495A;
    const int32u TSO2=0x54534F32;
    const int32u TSOA=0x54534F41;
    const int32u TSOC=0x54534F43;
    const int32u TSOP=0x54534F50;
    const int32u TSOT=0x54534F54;
    const int32u TSRC=0x54535243;
    const int32u TSSE=0x54535345;
    const int32u TSST=0x54535354;
    const int32u TXXX=0x54585858;
    const int32u TYER=0x54594552;
    const int32u UFID=0x55464944;
    const int32u USER=0x55534552;
    const int32u USLT=0x55534C54;
    const int32u WCOM=0x57434F4D;
    const int32u WCOP=0x57434F50;
    const int32u WOAF=0x574F4146;
    const int32u WOAR=0x574F4152;
    const int32u WOAS=0x574F4153;
    const int32u WORS=0x574F5253;
    const int32u WPAY=0x57504159;
    const int32u WPUB=0x57505542;
    const int32u WXXX=0x57585858;
    const int32u XRVA=0x58525641;
    const int32u BUF=0x425546;
    const int32u CNT=0x434E56;
    const int32u COM=0x434F4D;
    const int32u CRA=0x435241;
    const int32u CRM=0x43524D;
    const int32u EQU=0x455155;
    const int32u ETC=0x455443;
    const int32u GEO=0x47454F;
    const int32u IPL=0x49504C;
    const int32u LNK=0x4C4E4B;
    const int32u MCI=0x4D4349;
    const int32u MLL=0x4D4C4C;
    const int32u PIC_=0x504943; //PIC is used by shared libs in GCC
    const int32u POP=0x504F50;
    const int32u REV=0x524556;
    const int32u RVA=0x525641;
    const int32u SLT=0x534C54;
    const int32u STC=0x535443;
    const int32u TAL=0x54414C;
    const int32u TBP=0x544250;
    const int32u TCM=0x54434D;
    const int32u TCO=0x54434F;
    const int32u TCP=0x544350;
    const int32u TCR=0x544352;
    const int32u TDA=0x544441;
    const int32u TDY=0x544459;
    const int32u TEN=0x54454E;
    const int32u TFT=0x544654;
    const int32u TIM=0x54494D;
    const int32u TKE=0x544B45;
    const int32u TLA=0x544C41;
    const int32u TLE=0x544C45;
    const int32u TMT=0x544D54;
    const int32u TOA=0x544F41;
    const int32u TOF=0x544F46;
    const int32u TOL=0x544F4C;
    const int32u TOR=0x544F52;
    const int32u TOT=0x544F54;
    const int32u TP1=0x545031;
    const int32u TP2=0x545032;
    const int32u TP3=0x545033;
    const int32u TP4=0x545034;
    const int32u TPA=0x545041;
    const int32u TPB=0x545042;
    const int32u TRC=0x545243;
    const int32u TRD=0x545244;
    const int32u TRK=0x54524B;
    const int32u TSI=0x545349;
    const int32u TSS=0x545353;
    const int32u TT1=0x545431;
    const int32u TT2=0x545432;
    const int32u TT3=0x545433;
    const int32u TXT=0x545854;
    const int32u TXX=0x545858;
    const int32u TYE=0x545945;
    const int32u UFI=0x554649;
    const int32u ULT=0x554C54;
    const int32u WAF=0x574146;
    const int32u WAR=0x574152;
    const int32u WAS=0x574153;
    const int32u WCM=0x57434D;
    const int32u WCP=0x574350;
    const int32u WPB=0x575042;
    const int32u WXX=0x575858;
}

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
File_Id3v2::File_Id3v2()
:File__Analyze()
{
    //Configuration
    Buffer_MaximumSize=8*1024*1024;
    
    //Temp
    Id3v2_Size=0;
    Unsynchronisation_Frame=false;
}

//***************************************************************************
// Static stuff
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Id3v2::Static_Synchronize_Tags(const int8u* Buffer, size_t Buffer_Offset, size_t Buffer_Size, bool &Tag_Found)
{
    //Buffer size
    if (Buffer_Offset+3>Buffer_Size)
        return false;

    //ID
    Tag_Found=CC3(Buffer+Buffer_Offset)==0x494433; //"ID3"

    //Continue
    return true;
}

//***************************************************************************
// Streams management
//***************************************************************************

//---------------------------------------------------------------------------
void File_Id3v2::Streams_Finish()
{
    if (Count_Get(Stream_General)==0)
        return;

    //Specific formats (multiple Id3v2 tags for one MI tag)
    Ztring Recorded_Date;
    if (Retrieve(Stream_General, 0, General_Recorded_Date).empty() && !Year.empty())
    {
        Recorded_Date+=Year;
        if (!Month.empty() && !Month.empty())
        {
            Recorded_Date+=_T("-");
            Recorded_Date+=Year;
            Recorded_Date+=_T("-");
            Recorded_Date+=Day;
            if (!Month.empty() && !Month.empty())
            {
                Recorded_Date+=_T(" ");
                Recorded_Date+=Hour;
                Recorded_Date+=_T(":");
                Recorded_Date+=Minute;
            }
        }
    }
    if (!Recorded_Date.empty())
        Fill(Stream_General, 0, "Recorded_Date", Recorded_Date);

    //Special cases
    //-Position and total parts
    if (Retrieve(Stream_General, 0, General_Part_Position).find(_T("/"))!=Error)
    {
        Ztring Temp=Retrieve(Stream_General, 0, General_Part_Position);
        Fill(Stream_General, 0, General_Part_Position_Total, Temp.SubString(_T("/"), _T("")));
        Fill(Stream_General, 0, General_Part_Position, Temp.SubString(_T(""), _T("/")));
    }
    if (Retrieve(Stream_General, 0, General_Track_Position).find(_T("/"))!=Error)
    {
        const Ztring &Temp=Retrieve(Stream_General, 0, General_Track_Position);
        Fill(Stream_General, 0, General_Track_Position_Total, Temp.SubString(_T("/"), _T("")), true);
        Fill(Stream_General, 0, General_Track_Position, Temp.SubString(_T(""), _T("/")), true);
    }
    //-Genre
    if (Retrieve(Stream_General, 0, General_Genre).find(_T("("))==0)
    {
        const Ztring &Genre=Retrieve(Stream_General, 0, General_Genre);
        Fill(Stream_General, 0, General_Genre, Genre.SubString(_T("("), _T(")")), true); //Replace (nn) by nn
    }
    if (Retrieve(Stream_General, 0, General_Genre)==_T("0") || Retrieve(Stream_General, 0, General_Genre)==_T("255"))
        Clear(Stream_General, 0, General_Genre);
}

//***************************************************************************
// Buffer - File header
//***************************************************************************

//---------------------------------------------------------------------------
void File_Id3v2::FileHeader_Parse()
{
    //Parsing
    int32u Size;
    int8u Flags;
    bool ExtendedHeader;
    Skip_C3(                                                    "identifier");
    Get_B1 (Id3v2_Version,                                      "version_major");
    Skip_B1(                                                    "version_revision");
    Get_B1 (Flags,                                              "flags");
        Get_Flags (Flags, 7, Unsynchronisation_Global,          "Unsynchronisation");
        Get_Flags (Flags, 6, ExtendedHeader,                    "Extended header");
        Skip_Flags(Flags, 5,                                    "Experimental indicator");
    Get_B4 (Size,                                               "Size");
    Id3v2_Size=((Size>>0)&0x7F)
             | ((Size>>1)&0x3F80)
             | ((Size>>2)&0x1FC000)
             | ((Size>>3)&0x0FE00000);
    Param_Info(Id3v2_Size);
    if (ExtendedHeader)
    {
        Element_Begin("Extended header");
        int32u Size_Extended;
        Get_B4 (Size_Extended,                                  "Size");
        Skip_XX(Size_Extended,                                  "Extended header");
        Element_End();
    }

    FILLING_BEGIN();
        //Versions
        switch (Id3v2_Version)
        {
            case 2 : break;
            case 3 : break;
            case 4 : break;
            default :
                Skip_XX(Id3v2_Size,                             "Data");
                return;
        }

        Stream_Prepare(Stream_General);
        Stream_Prepare(Stream_Audio);

        Accept("Id3v2");
    FILLING_END();
}

//***************************************************************************
// Buffer - Per element
//***************************************************************************

//---------------------------------------------------------------------------
void File_Id3v2::Header_Parse()
{
    if (Id3v2_Size<10) //first 10 is minimum size of a tag, Second 10 is ID3v2 header size
    {
        //Not enough place for a tag, must be padding
        Header_Fill_Code((int64u)-1, "Padding");
        Header_Fill_Size(Id3v2_Size);
        return;
    }

    if (Buffer_Offset+10>Buffer_Size)
    {
        Element_WaitForMoreData();
        return; //Not enough buffer
    }

    //Testing padding
    int32u Frame_ID, Size;
    Frame_ID=CC1(Buffer+Buffer_Offset);
    if (Frame_ID==0x00)
    {
        //This is the padding
        Header_Fill_Code(0xFFFFFFFF, "Padding");
        Header_Fill_Size(Id3v2_Size);
        return;
    }

    //Parsing
    if (Id3v2_Version==2)
    {
        Get_C3 (Frame_ID,                                       "Frame ID");
        Get_B3 (Size,                                           "Size");
    }
    else
    {
        int16u Flags;
        Get_C4 (Frame_ID,                                       "Frame ID");
        Get_B4 (Size,                                           "Size");
        Get_B2 (Flags,                                          "Flags");
        if (Id3v2_Version==3)
        {
            Skip_Flags(Flags, 15,                               "Tag alter preservation");
            Skip_Flags(Flags, 14,                               "File alter preservation");
            Skip_Flags(Flags, 13,                               "Read only");
            Skip_Flags(Flags,  7,                               "Compression");
            Skip_Flags(Flags,  6,                               "Encryption");
            Skip_Flags(Flags,  5,                               "Grouping identity");
        }
        if (Id3v2_Version==4)
        {
            Skip_Flags(Flags, 14,                               "Tag alter preservation");
            Skip_Flags(Flags, 13,                               "File alter preservation");
            Skip_Flags(Flags, 12,                               "Read only");
            Skip_Flags(Flags,  6,                               "Grouping identity");
            Skip_Flags(Flags,  3,                               "Compression");
            Skip_Flags(Flags,  2,                               "Encryption");
            Get_Flags (Flags,  1, Unsynchronisation_Frame,      "Unsynchronisation");
            Skip_Flags(Flags,  0,                               "Data length indicator");
        }
        if (Id3v2_Version!=3)
        {
            Size=((Size>>0)&0x7F)
               | ((Size>>1)&0x3F80)
               | ((Size>>2)&0x1FC000)
               | ((Size>>3)&0x0FE00000);
            Param_Info(Size);
        }
    }

    //Filling
    Ztring ToShow;
    if (Id3v2_Version==2)
        ToShow.From_CC3(Frame_ID);
    else
        ToShow.From_CC4(Frame_ID);
    Header_Fill_Code(Frame_ID, ToShow);
    Header_Fill_Size(Element_Offset+Size);
}

//---------------------------------------------------------------------------
void File_Id3v2::Data_Parse()
{
    Id3v2_Size-=Header_Size+Element_Size;

    //Unsynchronisation
    int8u* Buffer_Unsynch=NULL;
    const int8u* Save_Buffer=Buffer;
    size_t Save_Buffer_Offset=Buffer_Offset;
    int64u Save_Element_Size=Element_Size;
    size_t Element_Offset_Unsynch=0;
    std::vector<size_t> Unsynch_List;
    if (Unsynchronisation_Global || Unsynchronisation_Frame)
    {
        while (Element_Offset_Unsynch+3<=Element_Size)
        {
            if (CC2(Buffer+Buffer_Offset+(size_t)Element_Offset_Unsynch)==0xFF00)
                Unsynch_List.push_back(Element_Offset_Unsynch+1);
            Element_Offset_Unsynch++;
        }
        if (!Unsynch_List.empty())
        {
            //We must change the buffer for keeping out
            Element_Size=Save_Element_Size-Unsynch_List.size();
            Buffer_Offset=0;
            Buffer_Unsynch=new int8u[(size_t)Element_Size];
            for (size_t Pos=0; Pos<=Unsynch_List.size(); Pos++)
            {
                size_t Pos0=(Pos==Unsynch_List.size())?(size_t)Save_Element_Size:(Unsynch_List[Pos]);
                size_t Pos1=(Pos==0)?0:(Unsynch_List[Pos-1]+1);
                size_t Buffer_Unsynch_Begin=Pos1-Pos;
                size_t Save_Buffer_Begin  =Pos1;
                size_t Size=               Pos0-Pos1;
                std::memcpy(Buffer_Unsynch+Buffer_Unsynch_Begin, Save_Buffer+Save_Buffer_Offset+Save_Buffer_Begin, Size);
            }
            Buffer=Buffer_Unsynch;
        }
    }

    #define CASE_INFO(_NAME, _DETAIL) \
        case Elements::_NAME : Element_Info(_DETAIL); _NAME(); break;

    //Parsing
    Element_Value.clear();
    Element_Values.clear();
    switch (Element_Code)
    {
        CASE_INFO(AENC,                                         "Audio encryption");
        CASE_INFO(APIC,                                         "Attached picture");
        CASE_INFO(ASPI,                                         "Audio seek point index");
        CASE_INFO(COMM,                                         "Comments");
        CASE_INFO(COMR,                                         "Commercial frame");
        CASE_INFO(ENCR,                                         "Encryption method registration");
        CASE_INFO(EQU2,                                         "Equalisation (2)");
        CASE_INFO(EQUA,                                         "Equalization");
        CASE_INFO(ETCO,                                         "Event timing codes");
        CASE_INFO(GEOB,                                         "General encapsulated object");
        CASE_INFO(GRID,                                         "Group identification registration");
        CASE_INFO(IPLS,                                         "Involved people list");
        CASE_INFO(LINK,                                         "Linked information");
        CASE_INFO(MCDI,                                         "Music CD identifier");
        CASE_INFO(MLLT,                                         "MPEG location lookup table");
        CASE_INFO(OWNE,                                         "Ownership frame");
        CASE_INFO(PCNT,                                         "Play counter");
        CASE_INFO(POPM,                                         "Popularimeter");
        CASE_INFO(POSS,                                         "Position synchronisation frame");
        CASE_INFO(PRIV,                                         "Private frame");
        CASE_INFO(RBUF,                                         "Recommended buffer size");
        CASE_INFO(RGAD,                                         "Replay Gain Adjustment");
        CASE_INFO(RVA2,                                         "Relative volume adjustment (2)");
        CASE_INFO(RVRB,                                         "Reverb");
        CASE_INFO(SEEK,                                         "Seek frame");
        CASE_INFO(SIGN,                                         "Signature frame");
        CASE_INFO(SYLT,                                         "Synchronised lyric/text");
        CASE_INFO(SYTC,                                         "Synchronised tempo codes");
        CASE_INFO(TALB,                                         "Album/Movie/Show title");
        CASE_INFO(TBPM,                                         "BPM (beats per minute)");
        CASE_INFO(TCMP,                                         "iTunes Compilation Flag");
        CASE_INFO(TCOM,                                         "Composer");
        CASE_INFO(TCON,                                         "Content type");
        CASE_INFO(TCOP,                                         "Copyright message");
        CASE_INFO(TDAT,                                         "Date");
        CASE_INFO(TDEN,                                         "Encoding time");
        CASE_INFO(TDLY,                                         "Playlist delay");
        CASE_INFO(TDOR,                                         "Original release time");
        CASE_INFO(TDRC,                                         "Recording time");
        CASE_INFO(TDRL,                                         "Release time");
        CASE_INFO(TDTG,                                         "Tagging time");
        CASE_INFO(TENC,                                         "Encoded by");
        CASE_INFO(TEXT,                                         "Lyricist/Text writer");
        CASE_INFO(TFLT,                                         "File type");
        CASE_INFO(TIME,                                         "Time");
        CASE_INFO(TIPL,                                         "Involved people list");
        CASE_INFO(TIT1,                                         "Content group description");
        CASE_INFO(TIT2,                                         "Title/songname/content description");
        CASE_INFO(TIT3,                                         "Subtitle/Description refinement");
        CASE_INFO(TKEY,                                         "Initial key");
        CASE_INFO(TLAN,                                         "Language(s)");
        CASE_INFO(TLEN,                                         "Length");
        CASE_INFO(TMCL,                                         "Musician credits list");
        CASE_INFO(TMED,                                         "Media type");
        CASE_INFO(TMOO,                                         "Mood");
        CASE_INFO(TOAL,                                         "Original album/movie/show title");
        CASE_INFO(TOFN,                                         "Original filename");
        CASE_INFO(TOLY,                                         "Original lyricist(s)/text writer(s)");
        CASE_INFO(TOPE,                                         "Original artist(s)/performer(s)");
        CASE_INFO(TORY,                                         "Original release year");
        CASE_INFO(TOWN,                                         "File owner/licensee");
        CASE_INFO(TPE1,                                         "Lead performer(s)/Soloist(s)");
        CASE_INFO(TPE2,                                         "Band/orchestra/accompaniment");
        CASE_INFO(TPE3,                                         "Conductor/performer refinement");
        CASE_INFO(TPE4,                                         "Interpreted, remixed, or otherwise modified by");
        CASE_INFO(TPOS,                                         "Part of a set");
        CASE_INFO(TPRO,                                         "Produced notice");
        CASE_INFO(TPUB,                                         "Publisher");
        CASE_INFO(TRCK,                                         "Track number/Position in set");
        CASE_INFO(TRDA,                                         "Recording dates");
        CASE_INFO(TRSN,                                         "Internet radio station name");
        CASE_INFO(TRSO,                                         "Internet radio station owner");
        CASE_INFO(TSIZ,                                         "Size");
        CASE_INFO(TSO2,                                         "Performer order");
        CASE_INFO(TSOA,                                         "Album sort order");
        CASE_INFO(TSOC,                                         "Composer sort order");
        CASE_INFO(TSOP,                                         "Performer sort order");
        CASE_INFO(TSOT,                                         "Title sort order");
        CASE_INFO(TSRC,                                         "ISRC (international standard recording code)");
        CASE_INFO(TSSE,                                         "Software/Hardware and settings used for encoding");
        CASE_INFO(TSST,                                         "Set subtitle");
        CASE_INFO(TXXX,                                         "User defined text information frame");
        CASE_INFO(TYER,                                         "Year");
        CASE_INFO(UFID,                                         "Unique file identifier");
        CASE_INFO(USER,                                         "Terms of use");
        CASE_INFO(USLT,                                         "Unsynchronised lyric/text transcription");
        CASE_INFO(WCOM,                                         "Commercial information");
        CASE_INFO(WCOP,                                         "Copyright/Legal information");
        CASE_INFO(WOAF,                                         "Official audio file webpage");
        CASE_INFO(WOAR,                                         "Official artist/performer webpage");
        CASE_INFO(WOAS,                                         "Official audio source webpage");
        CASE_INFO(WORS,                                         "Official Internet radio station homepage");
        CASE_INFO(WPAY,                                         "Payment");
        CASE_INFO(WPUB,                                         "Publishers official webpage");
        CASE_INFO(WXXX,                                         "User defined URL link frame");
        CASE_INFO(XRVA,                                         "Relative volume adjustment (2)");
        CASE_INFO(BUF,                                          "Recommended buffer size");
        CASE_INFO(CNT,                                          "Play counter");
        CASE_INFO(COM,                                          "Comments");
        CASE_INFO(CRA,                                          "Audio encryption");
        CASE_INFO(CRM,                                          "Encrypted meta frame");
        CASE_INFO(EQU,                                          "Equalization");
        CASE_INFO(ETC,                                          "Event timing codes");
        CASE_INFO(GEO,                                          "General encapsulated object");
        CASE_INFO(IPL,                                          "Involved people list");
        CASE_INFO(LNK,                                          "Linked information");
        CASE_INFO(MCI,                                          "Music CD Identifier");
        CASE_INFO(MLL,                                          "MPEG location lookup table");
        CASE_INFO(PIC_,                                         "Attached picture");
        CASE_INFO(POP,                                          "Popularimeter");
        CASE_INFO(REV,                                          "Reverb");
        CASE_INFO(RVA,                                          "Relative volume adjustment");
        CASE_INFO(SLT,                                          "Synchronized lyric/text");
        CASE_INFO(STC,                                          "Synced tempo codes");
        CASE_INFO(TAL,                                          "Album/Movie/Show title");
        CASE_INFO(TBP,                                          "BPM (Beats Per Minute)");
        CASE_INFO(TCM,                                          "Composer");
        CASE_INFO(TCO,                                          "Content type");
        CASE_INFO(TCP,                                          "iTunes Compilation Flag");
        CASE_INFO(TCR,                                          "Copyright message");
        CASE_INFO(TDA,                                          "Date");
        CASE_INFO(TDY,                                          "Playlist delay");
        CASE_INFO(TEN,                                          "Encoded by");
        CASE_INFO(TFT,                                          "File type");
        CASE_INFO(TIM,                                          "Time");
        CASE_INFO(TKE,                                          "Initial key");
        CASE_INFO(TLA,                                          "Language(s)");
        CASE_INFO(TLE,                                          "Length");
        CASE_INFO(TMT,                                          "Media type");
        CASE_INFO(TOA,                                          "Original artist(s)/performer(s)");
        CASE_INFO(TOF,                                          "Original filename");
        CASE_INFO(TOL,                                          "Original Lyricist(s)/text writer(s)");
        CASE_INFO(TOR,                                          "Original release year");
        CASE_INFO(TOT,                                          "Original album/Movie/Show title");
        CASE_INFO(TP1,                                          "Lead artist(s)/Lead performer(s)/Soloist(s)/Performing group");
        CASE_INFO(TP2,                                          "Band/Orchestra/Accompaniment");
        CASE_INFO(TP3,                                          "Conductor/Performer refinement");
        CASE_INFO(TP4,                                          "Interpreted,                                          remixed,                                          or otherwise modified by");
        CASE_INFO(TPA,                                          "Part of a set");
        CASE_INFO(TPB,                                          "Publisher");
        CASE_INFO(TRC,                                          "ISRC (International Standard Recording Code)");
        CASE_INFO(TRD,                                          "Recording dates");
        CASE_INFO(TRK,                                          "Track number/Position in set");
        CASE_INFO(TSI,                                          "Size");
        CASE_INFO(TSS,                                          "Software/hardware and settings used for encoding");
        CASE_INFO(TT1,                                          "Content group description");
        CASE_INFO(TT2,                                          "Title/Songname/Content description");
        CASE_INFO(TT3,                                          "Subtitle/Description refinement");
        CASE_INFO(TXT,                                          "Lyricist/text writer");
        CASE_INFO(TXX,                                          "User defined text information frame");
        CASE_INFO(TYE,                                          "Year");
        CASE_INFO(UFI,                                          "Unique file identifier");
        CASE_INFO(ULT,                                          "Unsychronized lyric/text transcription");
        CASE_INFO(WAF,                                          "Official audio file webpage");
        CASE_INFO(WAR,                                          "Official artist/performer webpage");
        CASE_INFO(WAS,                                          "Official audio source webpage");
        CASE_INFO(WCM,                                          "Commercial information");
        CASE_INFO(WCP,                                          "Copyright/Legal information");
        CASE_INFO(WPB,                                          "Publishers official webpage");
        CASE_INFO(WXX,                                          "User defined URL link frame");
        default : Skip_XX(Element_Size,                         "Data");
    }

    if (!Unsynch_List.empty())
    {
        //We must change the buffer for keeping out
        Element_Size=Save_Element_Size;
        Buffer_Offset=Save_Buffer_Offset;
        delete[] Buffer; Buffer=Save_Buffer;
        Buffer_Unsynch=NULL; //Same as Buffer...
        Element_Offset+=Unsynch_List.size();
    }

    if (!Id3v2_Size)
        Finish("Id3v2");
}

//***************************************************************************
// Elements
//***************************************************************************

//---------------------------------------------------------------------------
//
void File_Id3v2::T___()
{
    int8u Encoding;
    Get_B1 (Encoding,                                           "Text_encoding");
    switch (Encoding)
    {
        case 0 : Get_Local (Element_Size-1, Element_Value,      "Information"); break;
        case 1 : Get_UTF16 (Element_Size-1, Element_Value,      "Information"); break;
        case 2 : Get_UTF16B(Element_Size-1, Element_Value,      "Information"); break;
        case 3 : Get_UTF8  (Element_Size-1, Element_Value,      "Information"); break;
        default : ;
    }

    //Filling
    Fill_Name();
}

//---------------------------------------------------------------------------
//
void File_Id3v2::T__X()
{
    //Integrity
    if (Element_Size<(Elements::TXXX?4:1))
    {
        Element_Values(1).clear();
        Element_Values(0).clear();
        return;
    }

    int8u Encoding;
    Get_B1 (Encoding,                                           "Text_encoding");
    if (Element_Code!=Elements::TXXX)
        Skip_C3(                                                "Language");
    size_t Value0_Size=0;
    switch (Encoding)
    {
        case 0 :
        case 3 : //1-byte char
                while (Element_Offset+Value0_Size<Element_Size && Buffer[Buffer_Offset+(size_t)Element_Offset+Value0_Size]!='\0')
                    Value0_Size++;
                if (Element_Offset+Value0_Size>=Element_Size)
                    return; //Problem
                switch (Encoding)
                {
                    case 0 : Get_Local (Value0_Size, Element_Values(0), "Short_content_descrip"); break;
                    case 3 : Get_UTF8  (Value0_Size, Element_Values(0), "Short_content_descrip"); break;
                    default : ;
                }
                Skip_B1(                                        "Null");
                switch (Encoding)
                {
                    case 0 : Get_Local (Element_Size-Element_Offset, Element_Values(1), "The_actual_text"); break;
                    case 3 : Get_UTF8  (Element_Size-Element_Offset, Element_Values(1), "The_actual_text"); break;
                    default : ;
                }
                break;
        case 1 :
        case 2 : //2-byte char
                while (Element_Offset+Value0_Size+1<Element_Size
                    && !(Buffer[Buffer_Offset+(size_t)Element_Offset+Value0_Size  ]=='\0'
                      && Buffer[Buffer_Offset+(size_t)Element_Offset+Value0_Size+1]=='\0')) //2-byte zero
                    Value0_Size+=2;
                if (Element_Offset+Value0_Size>=Element_Size)
                    return; //Problem
                switch (Encoding)
                {
                    case 1 : Get_UTF16 (Value0_Size, Element_Values(0), "Short_content_descrip"); break;
                    case 2 : Get_UTF16B(Value0_Size, Element_Values(0), "Short_content_descrip"); break;
                    default : ;
                }
                Skip_B2(                                        "Null");
                switch (Encoding)
                {
                    case 1 : Get_UTF16 (Element_Size-Element_Offset, Element_Values(1), "The_actual_text"); break;
                    case 2 : Get_UTF16B(Element_Size-Element_Offset, Element_Values(1), "The_actual_text"); break;
                    default : ;
                }
                break;
        default: //Unknown
                Skip_XX(Element_Size-Element_Offset,            "Unknown");
        ;
    }
}

//---------------------------------------------------------------------------
void File_Id3v2::W___()
{
    Get_Local(Element_Size, Element_Value,                      "URL");

    //Filling
    Fill_Name();
}

//---------------------------------------------------------------------------
void File_Id3v2::W__X()
{
    if (Element_Size<1)
        return; //Problem

    int8u Encoding;
    Get_B1 (Encoding,                                           "Text_encoding");
    switch (Encoding)
    {
        case 0 : Get_Local (Element_Size-1, Element_Values(0),  "Description"); break;
        case 1 : Get_UTF16 (Element_Size-1, Element_Values(0),  "Description"); break;
        case 2 : Get_UTF16B(Element_Size-1, Element_Values(0),  "Description"); break;
        case 3 : Get_UTF8  (Element_Size-1, Element_Values(0),  "Description"); break;
        default : ;
    }
    Element_Offset=1;
    if (Encoding==1)
        Element_Offset+=Element_Values(0).size()*2+4; //UTF-16 BOM + UTF-16 NULL
    else if (Encoding==1 || Encoding==2)
        Element_Offset+=Element_Values(0).size()*2+2; //UTF-16 NULL
    else
        Element_Offset+=Element_Values(0).size()+1;   //UTF-8 NULL
    Get_Local(Element_Size-Element_Offset, Element_Values(1),      "URL");
}

//---------------------------------------------------------------------------
void File_Id3v2::APIC()
{
    int8u Encoding, PictureType;
    Ztring Mime, Description;
    Get_B1 (Encoding,                                           "Text_encoding");
    if (Id3v2_Version==2)
    {
        int32u Image_format;
        Get_C3(Image_format,                                    "Image_format");
        switch (Image_format)
        {
            case 0x504E47 : Mime="image/png";
            case 0x4A5047 : Mime="image/jpeg";
            default       : ;
        }
    }
    else
        Get_Local(Element_Size-1, Mime,                         "MIME_type");
    Element_Offset=1+Mime.size()+1;
    Get_B1 (PictureType,                                        "Picture_type"); Element_Info(Id3v2_PictureType(PictureType));
    Get_Local(Element_Size-Element_Offset, Description,         "Description");
    Element_Offset=1+Mime.size()+1+1+Description.size()+1;
    if (Element_Offset>Element_Size)
        return; //There is a problem
    std::string Data_Raw((const char*)(Buffer+(size_t)(Buffer_Offset+Element_Offset)), (size_t)(Element_Size-Element_Offset));
    std::string Data_Base64(Base64::encode(Data_Raw));

    //Filling
    Fill_Name();
    Fill(Stream_General, 0, General_Cover_Description, Description);
    Fill(Stream_General, 0, General_Cover_Type, Id3v2_PictureType(PictureType));
    Fill(Stream_General, 0, General_Cover_Mime, Mime);
    Fill(Stream_General, 0, General_Cover_Data, Data_Base64);
}

//---------------------------------------------------------------------------
void File_Id3v2::COMM()
{
    T__X();

    //Testing
         if (Element_Values(0)==_T("Songs-DB_Tempo")) return;
    else if (Element_Values(0)==_T("Songs-DB_Preference")) return;
    else if (Element_Values(0)==_T("MusicMatch_Tempo")) return;
    else if (Element_Values(0)==_T("MusicMatch_Mood"))
    {
        if (Retrieve(Stream_General, 0, General_Mood).empty())
            Element_Values(0)==_T("Mood");
        else
            return;
    }
    else if (Element_Values(0)==_T("MusicMatch_Preference")) return;

    //Filling
    if (Element_Values(0).empty())
        Element_Values(0)=_T("Comment");
    Fill_Name();
}

//---------------------------------------------------------------------------
void File_Id3v2::RGAD()
{
    //Parsing
    float32 Peak_Amplitude;
    Get_BF4 (Peak_Amplitude,                                    "Peak Amplitude");
    while (Element_Offset+2<=Element_Size)
    {
        Element_Begin("Gain Adjustement", 2);
        int16u Replay_Gain_Adjustment;
        int8u  Name_code;
        bool   Sign_bit;
        BS_Begin();
        Get_S1 (3, Name_code,                                   "Name code"); Param_Info(Id3v2_RGAD_Name_code[Name_code]);
        Info_S1(3, Originator_code,                             "Originator code"); Param_Info(Id3v2_RGAD_Originator_code[Originator_code]);
        Get_SB (Sign_bit,                                       "Sign bit");
        Get_S2 (9, Replay_Gain_Adjustment,                      "Replay Gain Adjustment"); Param_Info ((Sign_bit?-1:1)*(float)Replay_Gain_Adjustment/10, 1, " dB");
        BS_End();
        Element_End();

        FILLING_BEGIN();
            switch (Name_code)
            {
                case 1 : if (Retrieve(Stream_Audio, 0, Audio_ReplayGain_Gain).empty()) //this tag is not precise, we prefer other RG tags
                            Fill(Stream_Audio, 0, Audio_ReplayGain_Gain, (Sign_bit?-1:1)*(float)Replay_Gain_Adjustment/10, 1);
                case 2 : if (Retrieve(Stream_General, 0, General_Album_ReplayGain_Gain).empty()) //this tag is not precise, we prefer other RG tags
                            Fill(Stream_General, 0, General_Album_ReplayGain_Gain, (Sign_bit?-1:1)*(float)Replay_Gain_Adjustment/10, 1);
            }
        FILLING_END();
    }

    FILLING_BEGIN();
        if (Peak_Amplitude && Retrieve(Stream_Audio, 0, Audio_ReplayGain_Peak).empty()) //this tag is not precise, we prefer other RG tags
            Fill(Stream_Audio, 0, Audio_ReplayGain_Peak, Peak_Amplitude, 6);
    FILLING_END();
}

//---------------------------------------------------------------------------
void File_Id3v2::PRIV()
{
    //Parsing
    Ztring Owner;
    Get_Local(Element_Size, Owner,                              "Owner identifier");
    Element_Offset=Owner.size()+1;
    Skip_XX(Element_Size-Element_Offset,                        "Data");
}

//---------------------------------------------------------------------------
void File_Id3v2::USLT()
{
    T__X();

    //Filling
    if (!Element_Values(0).empty())
        Element_Values(1)=Element_Values(0)+MediaInfoLib::Config.Language_Get(_T(": "))+Element_Values(1);
    Element_Values(0)=_T("Lyrics");

    Fill_Name();
}

//---------------------------------------------------------------------------
void File_Id3v2::TXXX()
{
    T__X();

    //Filling
    if (Element_Values(0).empty())
        Element_Values(0)=_T("Comment");
    Fill_Name();
}

//---------------------------------------------------------------------------
void File_Id3v2::SYLT()
{
    int8u Encoding;
    Get_B1 (Encoding,                                           "Text encoding");
    Skip_C3(                                                    "Language");
    Skip_B1(                                                    "Time_stamp_format");
    Skip_B1(                                                    "Content_type");
    switch (Encoding)
    {
        case 0 : Get_Local (Element_Size-6, Element_Value,      "Short_content_descrip"); break;
        case 1 : Get_UTF16 (Element_Size-6, Element_Value,      "Short_content_descrip"); break;
        case 2 : Get_UTF16B(Element_Size-6, Element_Value,      "Short_content_descrip"); break;
        case 3 : Get_UTF8  (Element_Size-6, Element_Value,      "Short_content_descrip"); break;
        default : ;
    }

    //Filling
    Fill_Name();
}

//---------------------------------------------------------------------------
void File_Id3v2::WXXX()
{
    W__X();

    //Filling
    if (Element_Values(0).empty())
        Element_Values(0)=_T("URL");
    Fill_Name();
}

//***************************************************************************
// Helpers
//***************************************************************************

//---------------------------------------------------------------------------
void File_Id3v2::Fill_Name()
{
    switch (Element_Code)
    {
        case Elements::AENC : break;
        case Elements::APIC : Fill(Stream_General, 0, General_Cover, "Yes"); break;
        case Elements::ASPI : break;
        case Elements::COMM : Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1)); break;
        case Elements::COMR : Fill(Stream_General, 0, "Commercial frame", Element_Value); break;
        case Elements::ENCR : break;
        case Elements::EQU2 : break;
        case Elements::EQUA : break;
        case Elements::ETCO : break;
        case Elements::GEOB : break;
        case Elements::GRID : Fill(Stream_General, 0, "Group identification registration", Element_Value); break;
        case Elements::IPLS : Fill(Stream_General, 0, "Involved people list", Element_Value); break;
        case Elements::LINK : Fill(Stream_General, 0, "Linked information", Element_Value); break;
        case Elements::MCDI : Fill(Stream_General, 0, "MCDI", "Yes"); break;
        case Elements::MLLT : break;
        case Elements::OWNE : Fill(Stream_General, 0, General_Owner, Element_Value); break;
        case Elements::PCNT : break;
        case Elements::POPM : break;
        case Elements::POSS : break;
        case Elements::PRIV : break;
        case Elements::RBUF : break;
        case Elements::RVA2 : break;
        case Elements::RVRB : break;
        case Elements::SEEK : break;
        case Elements::SIGN : break;
        case Elements::SYLT : Fill(Stream_General, 0, General_Lyrics, Element_Value); break;
        case Elements::SYTC : break;
        case Elements::TALB : Fill(Stream_General, 0, General_Album, Element_Value); break;
        case Elements::TBPM : Fill(Stream_General, 0, General_BPM, Element_Value); break;
        case Elements::TCOM : Fill(Stream_General, 0, General_Composer, Element_Value); break;
        case Elements::TCON : Fill(Stream_General, 0, General_Genre, Element_Value); break;
        case Elements::TCOP : Fill(Stream_General, 0, General_Copyright, Element_Value); break;
        case Elements::TDA  :
        case Elements::TDAT : if (Element_Value.size()==4)
                         {
                            Month.assign(Element_Value.c_str(), 0, 2);
                            Day.assign  (Element_Value.c_str(), 2, 2); break;
                         }
        case Elements::TDEN : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Encoded_Date", Element_Value); break;
        case Elements::TDLY : break;
        case Elements::TDOR : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Original/Released_Date", Element_Value); break;
        case Elements::TDRC : Normalize_Date(Element_Value); Fill(Stream_General, 0, General_Recorded_Date, Element_Value); break;
        case Elements::TDRL : Normalize_Date(Element_Value); Fill(Stream_General, 0, General_Released_Date, Element_Value); break;
        case Elements::TDTG : Normalize_Date(Element_Value); Fill(Stream_General, 0, General_Tagged_Date, Element_Value); break;
        case Elements::TENC : Fill(Stream_General, 0, General_Encoded_Library, Element_Value); break;
        case Elements::TEXT : Fill(Stream_General, 0, General_Lyricist, Element_Value); break;
        case Elements::TFLT : Fill(Stream_General, 0, "File type", Element_Value); break;
        case Elements::TIM  :
        case Elements::TIME : if (Element_Value.size()==4)
                         {
                            Hour.assign  (Element_Value.c_str(), 0, 2);
                            Minute.assign(Element_Value.c_str(), 2, 2); break;
                         }
        case Elements::TIPL : Fill(Stream_General, 0, General_ThanksTo, Element_Value); break;
        case Elements::TIT1 : Fill(Stream_General, 0, General_ContentType, Element_Value); break;
        case Elements::TIT2 : Fill(Stream_General, 0, General_Track, Element_Value); break;
        case Elements::TIT3 : Fill(Stream_General, 0, General_Track_More, Element_Value); break;
        case Elements::TKEY : Fill(Stream_General, 0, "Initial key", Element_Value); break;
        case Elements::TLAN : Fill(Stream_Audio,   0, Audio_Language, Element_Value); break;
        case Elements::TLEN : break; //Fill(Stream_General, 0, "Length", Element_Value); break;
        case Elements::TMCL : Fill(Stream_General, 0, "Musician Credit List", Element_Value); break;
        case Elements::TMED : Fill(Stream_General, 0, "Media Type", Element_Value); break;
        case Elements::TMOO : Fill(Stream_General, 0, "Mood", Element_Value); break;
        case Elements::TOAL : Fill(Stream_General, 0, "Original/Album", Element_Value); break;
        case Elements::TOFN : Fill(Stream_General, 0, "Original/FileName", Element_Value); break;
        case Elements::TOLY : Fill(Stream_General, 0, "Original/Lyricist", Element_Value); break;
        case Elements::TOPE : Fill(Stream_General, 0, "Original/Performer", Element_Value); break;
        case Elements::TORY : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Original/Released_Date", Element_Value); break;
        case Elements::TOWN : Fill(Stream_General, 0, General_Owner, Element_Value); break;
        case Elements::TPE1 : Fill(Stream_General, 0, General_Performer, Element_Value); break;
        case Elements::TPE2 : Fill(Stream_General, 0, General_Accompaniment, Element_Value); break;
        case Elements::TPE3 : Fill(Stream_General, 0, General_Conductor, Element_Value); break;
        case Elements::TPE4 : Fill(Stream_General, 0, General_RemixedBy, Element_Value); break;
        case Elements::TPOS : Fill(Stream_General, 0, General_Part_Position, Element_Value); break;
        case Elements::TPRO : Fill(Stream_General, 0, General_Producer_Copyright, Element_Value); break;
        case Elements::TPUB : Fill(Stream_General, 0, General_Publisher, Element_Value); break;
        case Elements::TRCK : Fill(Stream_General, 0, General_Track_Position, Element_Value); break;
        case Elements::TRDA : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Recorded_Date", Element_Value); break;
        case Elements::TRSN : Fill(Stream_General, 0, General_ServiceName, Element_Value); break;
        case Elements::TRSO : Fill(Stream_General, 0, General_ServiceProvider, Element_Value); break;
        case Elements::TSIZ : Fill(Stream_General, 0, "Size", Element_Value); break;
        case Elements::TSO2 : Fill(Stream_General, 0, General_Performer_Sort, Element_Value); break;
        case Elements::TSOA : Fill(Stream_General, 0, General_Album_Sort, Element_Value); break;
        case Elements::TSOC : Fill(Stream_General, 0, "Composer/Sort", Element_Value); break;
        case Elements::TSOP : Fill(Stream_General, 0, General_Performer_Sort, Element_Value); break;
        case Elements::TSOT : Fill(Stream_General, 0, General_Track_Sort, Element_Value); break;
        case Elements::TSRC : Fill(Stream_General, 0, General_ISRC, Element_Value); break;
        case Elements::TSSE : Fill(Stream_General, 0, General_Encoded_Library_Settings, Element_Value); break;
        case Elements::TSST : Fill(Stream_General, 0, "Set subtitle", Element_Value); break;
        case Elements::TXXX :      if (Element_Values(0)==_T("CT_GAPLESS_DATA"))        ;
                         else if (Element_Values(0)==_T("DISCNUMBER"))             Fill(Stream_General, 0, General_Part_Position,           Element_Values(1), true);
                         else if (Element_Values(0)==_T("DISCTOTAL"))              Fill(Stream_General, 0, General_Part_Position_Total,     Element_Values(1), true);
                         else if (Element_Values(0)==_T("first_played_timestamp")) Fill(Stream_General, 0, General_Played_First_Date,       Ztring().Date_From_Milliseconds_1601(Element_Values(1).To_int64u()/10000));
                         else if (Element_Values(0)==_T("last_played_timestamp"))  Fill(Stream_General, 0, General_Played_Last_Date,        Ztring().Date_From_Milliseconds_1601(Element_Values(1).To_int64u()/10000));
                         else if (Element_Values(0)==_T("play_count"))             Fill(Stream_General, 0, General_Played_Count,            Element_Values(1).To_int64u());
                         else if (Element_Values(0)==_T("added_timestamp"))        Fill(Stream_General, 0, General_Added_Date,              Ztring().Date_From_Milliseconds_1601(Element_Values(1).To_int64u()/10000));
                         else if (Element_Values(0)==_T("replaygain_album_gain"))  Fill(Stream_General, 0, General_Album_ReplayGain_Gain,   Element_Values(1).To_float64(), 2, true);
                         else if (Element_Values(0)==_T("replaygain_album_peak"))  Fill(Stream_General, 0, General_Album_ReplayGain_Peak,   Element_Values(1).To_float64(), 6, true);
                         else if (Element_Values(0)==_T("replaygain_track_gain"))  Fill(Stream_Audio,   0, Audio_ReplayGain_Gain,           Element_Values(1).To_float64(), 2, true);
                         else if (Element_Values(0)==_T("replaygain_track_peak"))  Fill(Stream_Audio,   0, Audio_ReplayGain_Peak,           Element_Values(1).To_float64(), 6, true);
                         else if (Element_Values(0)==_T("TRACKTOTAL"))             Fill(Stream_General, 0, General_Track_Position_Total,    Element_Values(1), true);
                         else
                            Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1));
                         break;
        case Elements::TYER : Year=Element_Value; break;
        case Elements::UFID : Fill(Stream_Audio,   0, "UID", Element_Value); break;
        case Elements::USER : Fill(Stream_General, 0, General_TermsOfUse, Element_Value); break;
        case Elements::USLT : Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1)); break;
        case Elements::WCOM : Fill(Stream_General, 0, "Commercial information", Element_Value); break;
        case Elements::WCOP : Fill(Stream_General, 0, "Copyright/Legal information", Element_Value); break;
        case Elements::WOAF : Fill(Stream_General, 0, "Official audio file webpage", Element_Value); break;
        case Elements::WOAR : Fill(Stream_General, 0, "Performer/Url", Element_Value); break;
        case Elements::WOAS : Fill(Stream_General, 0, "Official audio source webpage", Element_Value); break;
        case Elements::WORS : Fill(Stream_General, 0, General_Service_Url, Element_Value); break;
        case Elements::WPAY : Fill(Stream_General, 0, "Payment", Element_Value); break;
        case Elements::WPUB : Fill(Stream_General, 0, "Publisher/Url", Element_Value); break;
        case Elements::WXXX : Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1)); break;
        case Elements::BUF  : break;
        case Elements::CNT  : break;
        case Elements::COM  : Fill(Stream_General, 0, "Comment", Element_Value); break;
        case Elements::CRA  : break;
        case Elements::CRM  : break;
        case Elements::EQU  : break;
        case Elements::ETC  : break;
        case Elements::GEO  : break;
        case Elements::IPL  : Fill(Stream_General, 0, "Involved people list", Element_Value); break;
        case Elements::LNK  : Fill(Stream_General, 0, "Linked information,", Element_Value); break;
        case Elements::MCI  : Fill(Stream_General, 0, "MCDI", Element_Value); break;
        case Elements::MLL  : break;
        case Elements::PIC_ : Fill(Stream_General, 0, "Cover", "Yes"); break;
        case Elements::POP  : break;
        case Elements::REV  : break;
        case Elements::RVA  : break;
        case Elements::SLT  : break;
        case Elements::STC  : break;
        case Elements::TAL  : Fill(Stream_General, 0, "Album", Element_Value); break;
        case Elements::TBP  : Fill(Stream_General, 0, "BPM", Element_Value); break;
        case Elements::TCM  : Fill(Stream_General, 0, "Composer", Element_Value); break;
        case Elements::TCO  : Fill(Stream_General, 0, "Genre", Element_Value); break;
        case Elements::TCR  : Fill(Stream_General, 0, "Copyright", Element_Value); break;
        case Elements::TDY  : break;
        case Elements::TEN  : Fill(Stream_General, 0, "Encoded_Library", Element_Value); break;
        case Elements::TFT  : Fill(Stream_General, 0, "File type", Element_Value); break;
        case Elements::TKE  : Fill(Stream_General, 0, "Initial key", Element_Value); break;
        case Elements::TLA  : Fill(Stream_Audio,   0, Audio_Language, Element_Value); break;
        case Elements::TLE  : break;
        case Elements::TMT  : Fill(Stream_General, 0, "Media type", Element_Value); break;
        case Elements::TOA  : Fill(Stream_General, 0, "Original/Performer", Element_Value); break;
        case Elements::TOF  : Fill(Stream_General, 0, "Original/FileName", Element_Value); break;
        case Elements::TOL  : Fill(Stream_General, 0, "Original/Lyricist", Element_Value); break;
        case Elements::TOR  : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Original/Released_Date", Element_Value); break;
        case Elements::TOT  : Fill(Stream_General, 0, "Original/Album", Element_Value); break;
        case Elements::TP1  : Fill(Stream_General, 0, "Performer", Element_Value); break;
        case Elements::TP2  : Fill(Stream_General, 0, "Accompaniment", Element_Value); break;
        case Elements::TP3  : Fill(Stream_General, 0, "Conductor", Element_Value); break;
        case Elements::TP4  : Fill(Stream_General, 0, "RemixedBy", Element_Value); break;
        case Elements::TPA  : Fill(Stream_General, 0, "Part/Position", Element_Value); break;
        case Elements::TPB  : Fill(Stream_General, 0, "Publisher", Element_Value); break;
        case Elements::TRC  : Fill(Stream_General, 0, "ISRC", Element_Value); break;
        case Elements::TRD  : Normalize_Date(Element_Value); Fill(Stream_General, 0, "Recorded_Date", Element_Value); break;
        case Elements::TRK  : Fill(Stream_General, 0, "Track/Position", Element_Value); break;
        case Elements::TSI  : break;
        case Elements::TSS  : break;
        case Elements::TT1  : Fill(Stream_General, 0, "ContentType", Element_Value); break;
        case Elements::TT2  : Fill(Stream_General, 0, "Track", Element_Value); break;
        case Elements::TT3  : Fill(Stream_General, 0, "Track/More", Element_Value); break;
        case Elements::TXT  : Fill(Stream_General, 0, "Lyricist", Element_Value); break;
        case Elements::TXX  : Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1)); break;
        case Elements::TYE  : Year=Element_Value; break;
        case Elements::UFI  : Fill(Stream_Audio,   0, "UID", Element_Value); break;
        case Elements::ULT  : break;
        case Elements::WAF  : break;
        case Elements::WAR  : Fill(Stream_General, 0, General_Service_Url, Element_Value); break;
        case Elements::WAS  : Fill(Stream_General, 0, "Official audio source webpage", Element_Value); break;
        case Elements::WCM  : Fill(Stream_General, 0, "Commercial information", Element_Value); break;
        case Elements::WCP  : Fill(Stream_General, 0, "Copyright/Legal information", Element_Value); break;
        case Elements::WPB  : Fill(Stream_General, 0, "Publisher/Url", Element_Value); break;
        case Elements::WXX  : Fill(Stream_General, 0, Element_Values(0).To_UTF8().c_str(), Element_Values(1)); break;
        default : ;
    }
}

//---------------------------------------------------------------------------
void File_Id3v2::Normalize_Date(Ztring& Date)
{
    if (Date.size()<=8)
        return; //Format unknown
    Date[8]=_T(' '); //could be "T"
    Date=Ztring(_T("UTC "))+Date; //Id3v2 specify a UTC date
}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_MPEGA_YES

