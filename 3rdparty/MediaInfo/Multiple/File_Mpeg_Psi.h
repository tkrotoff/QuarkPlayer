// File_Mpeg_Psi - Info for MPEG Stream files
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
//
// Information about MPEG files, Program Map Section
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_Mpeg_PsiH
#define MediaInfo_Mpeg_PsiH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Analyze.h"
#include "MediaInfo/Multiple/File_Mpeg_Descriptors.h"
#include <map>
using namespace ZenLib;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Mpeg_Psi
//***************************************************************************

class File_Mpeg_Psi : public File__Analyze
{
public :
    //In
    bool    From_TS;
    complete_stream* Complete_Stream;
    int16u  pid;

    //Temp
    int16u transport_stream_id;
    int16u table_id_extension;
    int16u xxx_id;
    int8u  version_number;
    bool   xxx_id_IsValid;
    bool   section_syntax_indicator;

    //Details
    const char* Mpeg_Psi_Element_Name();

public :
    File_Mpeg_Psi();
    ~File_Mpeg_Psi();

private :
    //Buffer - Per element
    void Header_Parse();
    void Data_Parse();

    //Elements - Base
    void Table_reserved();
    void Table_iso13818_6();
    void Table_user_private();
    void Table_forbidden();

    //Elements - MPEG
    void program_stream_map(); //From PS
    void Table_00();
    void Table_01();
    void Table_02();
    void Table_03();
    void Table_04() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_05() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_06() {Skip_XX(Element_Size-Element_Offset, "Data");};
    //Elements - DVB
    void Table_38() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_39() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3A() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3B() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3C() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3D() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3E() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_3F() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_40();
    void Table_41() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_42();
    void Table_46();
    void Table_4A() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_4E();
    void Table_4F();
    void Table_5F(); //50 --> 5F
    void Table_6F(); //60 --> 6F
    void Table_70();
    void Table_71() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_72() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_73();
    void Table_74() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_75() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_76() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_77() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_78() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_79() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_7E() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_7F();
    //Elements - ASTC
    void Table_C0() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C1() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C2() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C3() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C4() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C5() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C6() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_C7();
    void Table_C8() {Table_C9();};
    void Table_C9();
    void Table_CA();
    void Table_CB();
    void Table_CC();
    void Table_CD();
    void Table_CE() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_CF() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D0() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D1() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D2() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D3() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D4() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D5() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D6();
    void Table_D7() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D8() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_D9() {Skip_XX(Element_Size-Element_Offset, "Data");};
    void Table_DA() {Table_C9();};

    //Helpers
    int16u Descriptors_Size;
    void Descriptors();
    void ATSC_multiple_string_structure(Ztring &Value, const char* Info);
    Ztring Date_MJD(int16u Date);
    Ztring Time_BCD(int32u Time);

    //Data
    int8u pointer_field;
    int8u table_id;
    int32u CRC_32;
};

} //NameSpace

#endif
