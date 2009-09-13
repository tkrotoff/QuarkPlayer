// File_Other - Use magic number to detect only the format
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
#if defined(MEDIAINFO_OTHER_YES)
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File_Other.h"
#include "ZenLib/Utils.h"
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Format
//***************************************************************************

//---------------------------------------------------------------------------
void File_Other::Read_Buffer_Continue()
{
    //Integrity
    if (Buffer_Size<16)
        return;

    Ztring Format, Format_String, Url, Extensions;
         if (CC4(Buffer)==0xC5C6CBC3) {Format=_T("RISC OS Chunk data");}
    else if (CC4(Buffer)==0x110000EF) {Format=_T("RISC OS AIF executable");}
    else if (CC4(Buffer)==CC4("Draw")) {Format=_T("RISC OS Draw");}
    else if (CC4(Buffer)==CC4("FONT")) {Format=_T("RISC OS Font");}
    else if (CC8(Buffer)==CC8("Maestro\r")) {Format=_T("RISC OS music file");}
    else if (CC4(Buffer)==CC4("FC14")) {Format=_T("Amiga Future Composer");}
    else if (CC4(Buffer)==CC4("SMOD")) {Format=_T("Amiga Future Composer");}
    else if (CC4(Buffer)==CC4("AON4")) {Format=_T("Amiga Art Of Noise");}
    else if (CC8(Buffer+1)==CC8("MUGICIAN")) {Format=_T("Amiga Mugician");}
    else if (Buffer_Size>=66 && CC8(Buffer+58)==CC8("SIDMON I")) {Format=_T("Amiga Sidmon");}
    else if (CC8(Buffer)==CC8("Synth4.0")) {Format=_T("Amiga Synthesis");}
    else if (CC4(Buffer)==CC4("ARP.")) {Format=_T("Amiga Holy Noise");}
    else if (CC4(Buffer)==CC4("BeEp")) {Format=_T("Amiga JamCracker");}
    else if (CC4(Buffer)==CC4("COSO")) {Format=_T("Amiga Hippel-COSO");}
    else if (CC3(Buffer)==CC3("LSX")) {Format=_T("Amiga LZX");}
    else if (CC4(Buffer)==CC4("MOVI")) {Format=_T("Silicon Graphics movie");}
    else if (CC4(Buffer+10)==CC4("Vivo")) {Format=_T("Vivo");}
    else if (CC4(Buffer+1)==CC4("VRML")) {Format=_T("VRML");}
    else if (CC5(Buffer)==CC5("HVQM4")) {Format=_T("GameCube Movie");}
    else if (CC8(Buffer)==CC8("KW-DIRAC")) {Format=_T("Dirac"); Url=_T("http://dirac.sourceforge.net/");}
    else if (CC5(Buffer)==CC5("ustar")) {Format=_T("Tar archive");}
    //TODO: all archive magic numbers
    else if (CC4(Buffer+1)==CC4("MSCB")) {Format=_T("MS Cabinet");}
    else if (CC4(Buffer)==CC4(".snd")) {Format=_T("SUN Audio");}
    else if (CC4(Buffer)==0x2E736400) {Format=_T("DEC Audio");}
    else if (CC4(Buffer)==CC4("MThd")) {Format=_T("MIDI");}
    else if (CC4(Buffer)==CC4("CTMF")) {Format=_T("CMF");}
    else if (CC3(Buffer)==CC3("SBI")) {Format=_T("SoundBlaster");}
    else if (CC4(Buffer)==CC4("EMOD")) {Format=_T("Ext. MOD");}
    else if (CC3(Buffer)==CC3("TTA")) {Format=_T("True Audio"); Url=_T("http://www.true-audio.com/site.download"); Extensions=_T("tta");}
    //TODO: Other Sound magic numbers
    else if (CC7(Buffer)==CC7("BLENDER")) {Format=_T("Blender"); Url=_T("http://www.blender3d.com"); Extensions=_T("blenders");}
    else if (CC4(Buffer)==CC4("AC10")) {Format=_T("AutoCAD"); Url=_T("http://www.autodesk.com"); Extensions=_T("dwg");}
    else if (CC2(Buffer)==0x1F9D) {Format=_T("Compress");}
    else if (CC2(Buffer)==0x1F8B) {Format=_T("GZip");}
    else if (CC2(Buffer)==0x1F1E) {Format=_T("Huffman");}
    else if (CC3(Buffer)==CC3("BZh")) {Format=_T("BZip2");}
    else if (CC2(Buffer)==CC2("BZ")) {Format=_T("BZip1");}
    else if (CC3(Buffer)==CC3("NES")) {Format=_T("NES ROM");}
    else if (Buffer_Size>=0x108 && CC4(Buffer+0x104)==0xCEED6666) {Format=_T("GameBoy");}
    else if (Buffer_Size>=0x104 && CC4(Buffer+0x100)==CC4("SEGA")) {Format=_T("MegaDrive");}
    else if (Buffer_Size>=0x284 && CC4(Buffer+0x280)==CC4("EAGN")) {Format=_T("SupeMegaDrive");}
    else if (Buffer_Size>=0x284 && CC4(Buffer+0x280)==CC4("EAMG")) {Format=_T("SupeMegaDrive");}
    else if (CC4(Buffer)==0x21068028) {Format=_T("Dreamcast");}
    else if (CC4(Buffer)==CC4("LCDi")) {Format=_T("Dreamcast");}
    else if (CC4(Buffer)==0x37804012) {Format=_T("Nintendo64");}
    else if (CC8(Buffer)==CC8("PS-X EXE")) {Format=_T("Playstation");}
    else if (CC4(Buffer)==CC4("LCDi")) {Format=_T("Dreamcast");}
    else if (CC4(Buffer)==CC4("XBEH")) {Format=_T("X-Box");}
    else if (CC4(Buffer)==CC4("XIP0")) {Format=_T("X-Box");}
    else if (CC4(Buffer)==CC4("XTF0")) {Format=_T("X-Box");}
    else if (CC2(Buffer)==0x8008) {Format=_T("Lynx");}
    else if (CC7(Buffer)==CC7("\x01ZZZZZ\x01")) {Format=_T("");}
    else if (CC6(Buffer)==CC6("1\x0A\x0D""00:")) {Format=_T("SRT"); Extensions=_T("srt"); Url=_T("http://ffdshow.sourceforge.net/tikiwiki/tiki-index.php?page=Getting+ffdshow");
                                           Stream_Prepare(Stream_General);
                                           Stream_Prepare(Stream_Text); Fill(Stream_Text, 0, Text_Format, "SRT");}
    else if (CC1(Buffer+0)==CC1("[") && CC1(Buffer+2)==CC1("S") && CC1(Buffer+22)==CC1("o") && CC1(Buffer+24)==CC1("]") //Unicode Text is : "[Script Info]
          || CC1(Buffer+2)==CC1("[") && CC1(Buffer+4)==CC1("S") && CC1(Buffer+24)==CC1("o") && CC1(Buffer+26)==CC1("]"))
                                          {Format=_T("SSA"); Extensions=_T("ssa"); Url=_T("http://ffdshow.sourceforge.net/tikiwiki/tiki-index.php?page=Getting+ffdshow");
                                           Stream_Prepare(Stream_General);
                                           Stream_Prepare(Stream_Text); Fill(Stream_Text, 0, Text_Format, "SSA");}
    else if (CC4(Buffer)==CC4("RIFF") && CC4(Buffer+8)==CC4("AMV ")) {Format=_T("AMV");}
    else if (CC4(Buffer)==0x414D5697) {Format=_T("MTV");}
    else if (CC6(Buffer)==CC6("Z\0W\0F\0"))
                {Format=_T("ZWF");
                Stream_Prepare(Stream_General);
                Stream_Prepare(Stream_Audio); Fill(Stream_Audio, 0, Audio_Format, "ZWF");}
    else if (CC4(Buffer)==0x616A6B67) //"ajkg"
    {
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "Shorten");
        Fill(Stream_General, 0, General_Format_Version, CC1(Buffer+4));
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "Shorten");
        Accept();
        Finish();
        return;
    }
    else if (CC4(Buffer)==0x7442614B) //"tBaK"
    {
        Stream_Prepare(Stream_General);
        Fill(Stream_General, 0, General_Format, "TAK");
        Stream_Prepare(Stream_Audio);
        Fill(Stream_Audio, 0, Audio_Format, "TAK");
        Accept();
        Finish();
        return;
    }
    else if (CC4(Buffer)==CC4("")) {Format=_T("");}

    if (Format.empty())
    {
        Reject();
        return;
    }

    if (Count_Get(Stream_General)==0)
        Stream_Prepare(Stream_General);
    Fill(Stream_General, 0, General_Format, Format);
    if (!Url.empty())
        Fill(Stream_General, 0, General_Format_Url, Url, true);
    if (!Extensions.empty())
        Fill(Stream_General, 0, General_Format_Extensions, Extensions, true);
    Accept();
    Finish();
}

} //NameSpace

#endif //MEDIAINFO_OTHER_YES
