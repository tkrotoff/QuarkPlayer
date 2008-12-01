// File_ScreamTracker3 - Info for ScreamTracker3 files
// Copyright (C) 2008-2008 Jerome Martinez, Zen@MediaArea.net
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WXMHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILXMY or FXMNESS FOR A PARTICULAR PURPOSE.  See the
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
#if defined(MEDIAINFO_S3M_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/Audio/File_ScreamTracker3.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_ScreamTracker3::FileHeader_Parse()
{
    //Parsing
    Ztring SongName;
    int32u Signature;
    int16u OrdNum, InsNum, PatNum, Flags, Special;
    int8u  Mark, SoftwareVersionMajor, SoftwareVersionMinor, IS, TS;
    Get_Local(28, SongName,                                     "Song name");
    Get_L1 (Mark,                                               "0x1A");

    FILLING_BEGIN();
        if (Mark!=0x1A)
        {
            Finnished();
            return;
        }
    FILLING_END()

    Skip_L1(                                                    "Type");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Get_L2 (OrdNum,                                             "Orders count");
    Get_L2 (InsNum,                                             "Instruments count");
    Get_L2 (PatNum,                                             "Paterns count");
    Get_L2 (Flags,                                              "Flags");
        Skip_Flags(Flags, 0,                                    "st2vibrato");
        Skip_Flags(Flags, 1,                                    "st2tempo");
        Skip_Flags(Flags, 2,                                    "amigaslides");
        Skip_Flags(Flags, 3,                                    "0vol optimizations");
        Skip_Flags(Flags, 4,                                    "amiga limits");
        Skip_Flags(Flags, 5,                                    "enable filter/sfx with sb");
        Skip_Flags(Flags, 6,                                    "st3.00 volumeslides");
        Skip_Flags(Flags, 7,                                    "pecial custom data in file");
    Get_L1 (SoftwareVersionMajor,                               "Cwt/v (Major)");
    Get_L1 (SoftwareVersionMinor,                               "Cwt/v (Minor)");
    Skip_L2(                                                    "File format information");
    Get_B4(Signature,                                           "Signature");

    FILLING_BEGIN();
        if (Signature!=0x5343524D)
        {
            Finnished();
            return;
        }
    FILLING_END()

    Skip_L1(                                                    "global volume");
    Get_L1 (IS,                                                 "Initial Speed");
    Get_L1 (TS,                                                 "Initial Temp");
    Skip_L1(                                                    "master volume");
    Skip_L1(                                                    "ultra click removal");
    Skip_L1(                                                    "Default channel pan positions are present");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Skip_L1(                                                    "Unknown");
    Get_L2 (Special,                                            "Special");
    Skip_XX(32,                                                 "Channel settings");
    Skip_XX(OrdNum,                                             "Orders");
    Skip_XX(InsNum*2,                                           "Instruments");
    Skip_XX(PatNum*2,                                           "Patterns");

    FILLING_BEGIN();
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "Scream Tracker 3");
        Fill(Stream_General, 0, General_Track, SongName);
        if ((SoftwareVersionMajor&0xF0)==0x10)
            Fill(Stream_General, 0, General_Encoded_Application, Ztring(_T("Scream Tracker ")+Ztring::ToZtring(SoftwareVersionMajor)+_T(".")+Ztring::ToZtring(SoftwareVersionMinor/16)+Ztring::ToZtring(SoftwareVersionMinor%16)));
        Fill(Stream_General, 0, "BPM", TS);

        Stream_Prepare(Stream_Audio);

        Finnished();
    FILLING_END();

}

//***************************************************************************
// C++
//***************************************************************************

} //NameSpace

#endif //MEDIAINFO_S3M_YES
