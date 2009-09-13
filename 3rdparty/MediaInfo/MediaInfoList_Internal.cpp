// MediaInfoList_Internal - A list of MediaInfo
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
#include "MediaInfoList_Internal.h"
#include "MediaInfo/MediaInfo_Config.h"
#include "ZenLib/ZtringListList.h"
#include "ZenLib/Dir.h"
using namespace ZenLib;
using namespace std;
//---------------------------------------------------------------------------

namespace MediaInfoLib
{

//---------------------------------------------------------------------------
extern MediaInfo_Config Config;
//---------------------------------------------------------------------------

//***************************************************************************
// Gestion de la classe
//***************************************************************************

//---------------------------------------------------------------------------
//Constructeurs
MediaInfoList_Internal::MediaInfoList_Internal(size_t Count_Init)
{
    CriticalSectionLocker CSL(CS);
    //Initialisation
    Info.reserve(Count_Init);
    for (size_t Pos=0; Pos<Info.size(); Pos++)
        Info[Pos]=NULL;
    BlockMethod=0;
    State=0;
    ToParse_AlreadyDone=0;
    ToParse_Total=0;
    IsInThread=false;
}

//---------------------------------------------------------------------------
//Destructeur
MediaInfoList_Internal::~MediaInfoList_Internal()
{
    Close();

    CriticalSectionLocker CSL(CS);
    for (size_t Pos=0; Pos<Info.size(); Pos++)
        delete Info[Pos]; //Info[Pos]=NULL;
}

//***************************************************************************
// Fichiers
//***************************************************************************

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Open(const String &File, const fileoptions_t Options)
{
    //Option FileOption_Close
    if (Options & FileOption_CloseAll)
        Close(All);

    //Option Recursive
    //TODO

    //Get all filenames
    ZtringList List=Dir::GetAllFileNames(File, (Options&FileOption_NoRecursive)?Dir::Nothing:Dir::Parse_SubDirs);

    #ifdef MEDIAINFO_BDMV_YES
        //if there is a BDMV folder, this is blu-ray
        Ztring ToSearch=Ztring(1, PathSeparator)+_T("BDMV")+PathSeparator+_T("index.bdmv"); //"\BDMV\index.bdmv"
        for (size_t File_Pos=0; File_Pos<List.size(); File_Pos++)
        {
            size_t BDMV_Pos=List[File_Pos].find(ToSearch);
            if (BDMV_Pos!=string::npos && BDMV_Pos!=0 && BDMV_Pos+16==List[File_Pos].size())
            {
                //This is a BDMV index, parsing the directory only if index and movie objects are BOTH present
                ToSearch=List[File_Pos];
                ToSearch.resize(ToSearch.size()-10);
                ToSearch+=_T("MovieObject.bdmv");  //"%CompletePath%\BDMV\MovieObject.bdmv"
                if (List.Find(ToSearch)!=string::npos)
                {
                    //We want the folder instead of the files
                    List[File_Pos].resize(List[File_Pos].size()-11); //only %CompletePath%\BDMV
                    ToSearch=List[File_Pos];

                    for (size_t Pos=0; Pos<List.size(); Pos++)
                    {
                        if (List[Pos].find(ToSearch)==0 && List[Pos]!=ToSearch) //Remove all subdirs of ToSearch but not ToSearch
                        {
                            //Removing the file in the blu-ray directory
                            List.erase(List.begin()+Pos);
                            Pos--;
                        }
                    }
                }
            }   
        }
    #endif //MEDIAINFO_BDMV_YES

    //Registering files
    CS.Enter();
    if (ToParse.empty())
        CountValid=0;
    for (ZtringList::iterator L=List.begin(); L!=List.end(); L++)
        ToParse.push(*L);
    ToParse_Total+=List.size();
    if (ToParse_Total)
        State=ToParse_AlreadyDone*10000/ToParse_Total;
    else
        State=10000;
    CS.Leave();

    //Parsing
    if (BlockMethod==1)
    {
        if (!IsInThread) //If already created, the routine will read the new files
        {
            Run();
            IsInThread=true;
        }
        return 0;
    }
    else
    {
        Entry(); //Normal parsing
        return Count_Get();
    }
}

void MediaInfoList_Internal::Entry()
{
    if (ToParse_Total==0)
        return;

    while (1)
    {
        CS.Enter();
        if (!ToParse.empty())
        {
            MediaInfo* MI=new MediaInfo();
            for (std::map<String, String>::iterator Config_MediaInfo_Item=Config_MediaInfo_Items.begin(); Config_MediaInfo_Item!=Config_MediaInfo_Items.end(); Config_MediaInfo_Item++)
                MI->Option(Config_MediaInfo_Item->first, Config_MediaInfo_Item->second);
            MI->Open(ToParse.front());
            Info.push_back(MI);
            ToParse.pop();
            ToParse_AlreadyDone++;
            State=ToParse_AlreadyDone*10000/ToParse_Total;
        }
        CS.Leave();
        if (!IsInThread && State==10000)
            break; //This is not in a thread, we must stop alone
        Yield();
    }
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Open_Buffer_Init (int64u File_Size_, int64u File_Offset_)
{
    MediaInfo* MI=new MediaInfo();
    MI->Open_Buffer_Init(File_Size_, File_Offset_);

    CriticalSectionLocker CSL(CS);
    size_t Pos=Info.size();
    Info.push_back(MI);
    return Pos;
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Open_Buffer_Continue (size_t FilePos, const int8u* ToAdd, size_t ToAdd_Size)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos>=Info.size() || Info[FilePos]==NULL)
        return 0;

    return Info[FilePos]->Open_Buffer_Continue(ToAdd, ToAdd_Size);
}

//---------------------------------------------------------------------------
int64u MediaInfoList_Internal::Open_Buffer_Continue_GoTo_Get (size_t FilePos)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos>=Info.size() || Info[FilePos]==NULL)
        return (int64u)-1;

    return Info[FilePos]->Open_Buffer_Continue_GoTo_Get();
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Open_Buffer_Finalize (size_t FilePos)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos>=Info.size() || Info[FilePos]==NULL)
        return 0;

    return Info[FilePos]->Open_Buffer_Finalize();
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Save(size_t)
{
    CriticalSectionLocker CSL(CS);
    return 0; //Not yet implemented
}

//---------------------------------------------------------------------------
void MediaInfoList_Internal::Close(size_t FilePos)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos==Unlimited)
    {
        for (size_t Pos=0; Pos<Info.size(); Pos++)
        {
            delete Info[Pos]; Info[Pos]=NULL;
        }
        Info.clear();
    }
    else if (FilePos<Info.size())
    {
        delete Info[FilePos]; Info[FilePos]=NULL;
        Info.erase(Info.begin()+FilePos);
    }
    
    ToParse_AlreadyDone=0;
    ToParse_Total=0;
}

//***************************************************************************
// Get File info
//***************************************************************************

//---------------------------------------------------------------------------
String MediaInfoList_Internal::Inform(size_t FilePos, size_t)
{
    if (FilePos==Error)
    {
        Ztring Retour;
        FilePos=0;
        ZtringListList MediaInfo_Custom_View; MediaInfo_Custom_View.Write(Option(_T("Inform_Get")));
        bool XML=false;
        if (MediaInfoLib::Config.Inform_Get()==_T("XML"))
            XML=true;
        if (XML) Retour+=_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Mediainfo>\n");
        else Retour+=MediaInfo_Custom_View(Stream_Max+2, 1);//Page_Begin
        while (FilePos<Info.size())
        {
            Retour+=Inform(FilePos);
            if (FilePos<Info.size()-1)
            {
                Retour+=MediaInfo_Custom_View(Stream_Max+3, 1);//Page_Middle
            }
            FilePos++;
        }
        if (XML) Retour+=_T("</Mediainfo>\n");
        else Retour+=MediaInfo_Custom_View(Stream_Max+4, 1);//Page_End
        //Retour.FindAndReplace(_T("\\n"),_T( "\n"), 0, Ztring_Recursive);
        return Retour.c_str();
    }

    CriticalSectionLocker CSL(CS);

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return MediaInfoLib::Config.EmptyString_Get();

    return Info[FilePos]->Inform();
}

//---------------------------------------------------------------------------
String MediaInfoList_Internal::Get(size_t FilePos, stream_t KindOfStream, size_t StreamNumber, size_t Parameter, info_t KindOfInfo)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos==Error || FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return MediaInfoLib::Config.EmptyString_Get();

    return Info[FilePos]->Get(KindOfStream, StreamNumber, Parameter, KindOfInfo);
}

//---------------------------------------------------------------------------
String MediaInfoList_Internal::Get(size_t FilePos, stream_t KindOfStream, size_t StreamNumber, const String &Parameter, info_t KindOfInfo, info_t KindOfSearch)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos==Error || FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return MediaInfoLib::Config.EmptyString_Get();

    return Info[FilePos]->Get(KindOfStream, StreamNumber, Parameter, KindOfInfo, KindOfSearch);
}

//***************************************************************************
// Set File info
//***************************************************************************

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Set(const String &ToSet, size_t FilePos, stream_t StreamKind, size_t StreamNumber, size_t Parameter, const String &OldValue)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos==(size_t)-1)
        FilePos=0; //TODO : average

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return 0;

    return Info[FilePos]->Set(ToSet, StreamKind, StreamNumber, Parameter, OldValue);
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Set(const String &ToSet, size_t FilePos, stream_t StreamKind, size_t StreamNumber, const String &Parameter, const String &OldValue)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos==(size_t)-1)
        FilePos=0; //TODO : average

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return 0;

    return Info[FilePos]->Set(ToSet, StreamKind, StreamNumber, Parameter, OldValue);
}

//***************************************************************************
// Output buffer
//***************************************************************************

/*
//---------------------------------------------------------------------------
char* MediaInfoList_Internal::Output_Buffer_Get (size_t FilePos, size_t &Output_Buffer_Size)
{
    if (FilePos==(size_t)-1)
        FilePos=0; //TODO : average

    if (FilePos>=Info.size() || Info[FilePos]==NULL || Info[FilePos]->Count_Get(Stream_General)==0)
        return 0;

    return Info[FilePos]->Output_Buffer_Get(Output_Buffer_Size);
}
*/

//***************************************************************************
// Information
//***************************************************************************

//---------------------------------------------------------------------------
String MediaInfoList_Internal::Option (const String &Option, const String &Value)
{
    CriticalSectionLocker CSL(CS);
    Ztring OptionLower=Option; OptionLower.MakeLowerCase();
         if (Option==_T(""))
        return _T("");
    else if (OptionLower==_T("manguage_update"))
    {
        //Special case : Language_Update must update all MediaInfo classes
        for (unsigned int Pos=0; Pos<Info.size(); Pos++)
            if (Info[Pos])
                Info[Pos]->Option(_T("language_update"), Value);

        return _T("");
    }
    else if (OptionLower==_T("create_dummy"))
    {
        Info.resize(Info.size()+1);
        Info[Info.size()-1]=new MediaInfo();
        Info[Info.size()-1]->Option(Option, Value);
        return _T("");
    }
    else if (OptionLower==_T("thread"))
    {
        BlockMethod=1;
        return _T("");
    }
    else if (OptionLower.find(_T("file_"))==0)
    {
        Config_MediaInfo_Items[Option]=Value;
        return _T("");
    }
    else
        return MediaInfo::Option_Static(Option, Value);
}

//---------------------------------------------------------------------------
String MediaInfoList_Internal::Option_Static (const String &Option, const String &Value)
{
    return MediaInfo::Option_Static(Option, Value);
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::State_Get()
{
    CriticalSectionLocker CSL(CS);
    if (State==10000)
    {
        //Pause();
        IsInThread=false;
    }
    return State;
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Count_Get (size_t FilePos, stream_t StreamKind, size_t StreamNumber)
{
    CriticalSectionLocker CSL(CS);
    if (FilePos>=Info.size() || Info[FilePos]==NULL)
        return 0;

    return Info[FilePos]->Count_Get(StreamKind, StreamNumber);
}

//---------------------------------------------------------------------------
size_t MediaInfoList_Internal::Count_Get()
{
    CriticalSectionLocker CSL(CS);
    return Info.size();
}

//***************************************************************************
// Internal
//***************************************************************************

//---------------------------------------------------------------------------
void MediaInfoList_Internal::State_Set(size_t State_)
{
    CriticalSectionLocker CSL(CS);
    State=State_;
}

} //NameSpace

