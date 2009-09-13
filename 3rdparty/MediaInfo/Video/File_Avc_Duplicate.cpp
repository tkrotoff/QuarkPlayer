// File__Duplicate - Duplication of some formats
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
// Duplication helper for some specific formats
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
#if defined(MEDIAINFO_AVC_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Video/File_Avc.h"
#include "MediaInfo/MediaInfo_Config.h"
#include "MediaInfo/MediaInfo_Config_MediaInfo.h"
#include "ZenLib/ZtringList.h"
#include "ZenLib/File.h"
#include <cstring>
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Options
//***************************************************************************

//---------------------------------------------------------------------------
void File_Avc::Option_Manage()
{
    //File__Duplicate configuration
    if (File__Duplicate_HasChanged())
    {
        //Autorisation of other streams
        Streams[0x07].ShouldDuplicate=true;
    }
}

//***************************************************************************
// Set
//***************************************************************************

//---------------------------------------------------------------------------
bool File_Avc::File__Duplicate_Set (const Ztring &Value)
{
    ZtringList List(Value);

    //Searching Target
    bool IsForUs=false;
    std::vector<ZtringList::iterator> Targets_ToAdd;
    std::vector<ZtringList::iterator> Targets_ToRemove;
    std::vector<ZtringList::iterator> Orders_ToAdd;
    std::vector<ZtringList::iterator> Orders_ToRemove;
    for (ZtringList::iterator Current=List.begin(); Current<List.end(); Current++)
    {
        //Detecting if we want to remove
        bool ToRemove=false;
        if (Current->find(_T('-'))==0)
        {
            ToRemove=true;
            Current->erase(Current->begin());
        }

        //Managing targets
        if (Current->find(_T("file:"))==0
         || Current->find(_T("memory:"))==0)
            (ToRemove?Targets_ToRemove:Targets_ToAdd).push_back(Current);
        //Parser name
        else if (Current->find(_T("parser=Avc"))==0)
            IsForUs=true;
        //Managing orders
        else
            (ToRemove?Orders_ToRemove:Orders_ToAdd).push_back(Current);
    }

    //For us?
    if (!IsForUs)
        return false;

    //Configuration of initial values
    frame_num_Old=(int32u)-1;
    Duplicate_Buffer_Size=0;
    SPS_PPS_AlreadyDone=false;
    FLV=false;

    //For each target to add
    for (std::vector<ZtringList::iterator>::iterator Target=Targets_ToAdd.begin(); Target<Targets_ToAdd.end(); Target++)
        Writer.Configure(**Target);

    //For each order to add
    for (std::vector<ZtringList::iterator>::iterator Order=Orders_ToAdd.begin(); Order<Orders_ToAdd.end(); Order++)
        if ((**Order)==_T("format=Flv"))
            FLV=true;

    return true;
}

//***************************************************************************
// Write
//***************************************************************************

void File_Avc::File__Duplicate_Write (int64u Element_Code, int32u frame_num)
{
    const int8u* ToAdd=Buffer+Buffer_Offset-(size_t)Header_Size+3;
    size_t ToAdd_Size=(size_t)(Element_Size+Header_Size-3);

    if (!SPS_PPS_AlreadyDone)
    {
        if (Element_Code==7)
        {
            std::memcpy(Duplicate_Buffer, ToAdd, ToAdd_Size);
            Duplicate_Buffer_Size=ToAdd_Size;

        }
        else if (Element_Code==8)
        {

            // Form:
            //  8 bytes : PTS
            //  8 bytes : DTS
            //  8 bytes : Size (without header)
            //  1 byte  : Type (0=Frame, 1=Header);
            //  7 bytes : Reserved
            size_t Extra;
            if (FLV)
                Extra=1; //FLV
            else
                Extra=0; //MPEG-4
            int8u Header[32];
            int64u2BigEndian(Header+ 0, PTS);
            int64u2BigEndian(Header+ 8, DTS);
            int64u2BigEndian(Header+16, 5+Extra+2+Duplicate_Buffer_Size+1+2+ToAdd_Size); //5+Extra for SPS_SQS header, 2 for SPS size, 1 for PPS count, 2 for PPS size
            Header[24]=1;
            int56u2BigEndian(Header+25, 0);
            Writer.Write(Header, 32);

            //SPS_PPS
            int8u* SPS_SQS=new int8u[5+Extra];
            if (Extra==1)
            {
                SPS_SQS[0]=0x01; //Profile FLV
                SPS_SQS[1]=profile_idc; //Compatible Profile
                SPS_SQS[2]=0x00; //Reserved
            }
            else
            {
                SPS_SQS[0]=profile_idc; //Profile MPEG-4
                SPS_SQS[1]=0x00; //Compatible Profile
            }
            SPS_SQS[2+Extra]=level_idc; //Level
            SPS_SQS[3+Extra]=0xFF; //Reserved + Size of NALU length minus 1
            SPS_SQS[4+Extra]=0xE1; //Reserved + seq_parameter_set count
            Writer.Write(SPS_SQS, 5+Extra);

            //NALU
            int8u NALU[2];
            NALU[0]=((Duplicate_Buffer_Size)>> 8)&0xFF;;
            NALU[1]=((Duplicate_Buffer_Size)>> 0)&0xFF;;
            Writer.Write(NALU, 2);

            //SPS
            Writer.Write(Duplicate_Buffer, Duplicate_Buffer_Size);
            Duplicate_Buffer_Size=0;

            //PPS count
            SPS_SQS[0]=0x01; //pic_parameter_set count
            Writer.Write(SPS_SQS, 1);
            delete[] SPS_SQS;

            //NALU
            NALU[0]=((ToAdd_Size)>> 8)&0xFF;;
            NALU[1]=((ToAdd_Size)>> 0)&0xFF;;
            Writer.Write(NALU, 2);

            //PPS
            Writer.Write(ToAdd, ToAdd_Size);

            SPS_PPS_AlreadyDone=true;
        }
    }
    else if (frame_num!=(int32u)-1)
    {
        if (frame_num!=frame_num_Old && frame_num_Old!=(int32u)-1 && frame_num!=(int32u)-1)
        {
            // Form:
            //  8 bytes : PTS
            //  8 bytes : DTS
            //  8 bytes : Size (without header)
            //  1 byte  : Type (0=Frame, 1=Header);
            //  7 bytes : Reserved
            int8u Header[32];
            int64u2BigEndian(Header+ 0, PTS);
            int64u2BigEndian(Header+ 8, DTS);
            int64u2BigEndian(Header+16, Duplicate_Buffer_Size);
            Header[24]=0;
            int56u2BigEndian(Header+25, 0);
            Writer.Write(Header, 32);

            Writer.Write(Duplicate_Buffer, Duplicate_Buffer_Size);
            Duplicate_Buffer_Size=0;
        }


        //NALU
        int32u2BigEndian(Duplicate_Buffer+Duplicate_Buffer_Size, (int32u)ToAdd_Size); //4 bytes for NALU header
        Duplicate_Buffer_Size+=4;

        //Frame (partial)
        std::memcpy(Duplicate_Buffer+Duplicate_Buffer_Size, ToAdd, ToAdd_Size);
        Duplicate_Buffer_Size+=ToAdd_Size;
        frame_num_Old=frame_num;
    }

}

//***************************************************************************
// Output_Buffer
//***************************************************************************

//---------------------------------------------------------------------------
size_t File_Avc::Output_Buffer_Get (const String &)
{
    return Writer.Output_Buffer_Get();
}

//---------------------------------------------------------------------------
size_t File_Avc::Output_Buffer_Get (size_t)
{
    return Writer.Output_Buffer_Get();
}

} //NameSpace

#endif //MEDIAINFO_AVC_YES

