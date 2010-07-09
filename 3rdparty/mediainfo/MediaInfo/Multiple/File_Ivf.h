// File_Ivf - Info for Ivf files
// Copyright (C) 2004-2010 MediaArea.net SARL, Info@MediaArea.net
// Copyright (C) 2010 Lionel DUCHATEAU, kurtnoise at free dot fr
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Information about IVF files
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef MediaInfo_IvfH
#define MediaInfo_IvfH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "MediaInfo/File__Analyze.h"
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//***************************************************************************
// Class File_Ivf
//***************************************************************************

class File_Ivf : public File__Analyze
{
public :
    //In
    size_t Frame_Count_Valid;
private :
    //Buffer
    bool FileHeader_Begin();
    void FileHeader_Parse();
};

} //NameSpace

#endif

