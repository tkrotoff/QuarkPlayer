/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2008-2009  Tanguy Krotoff <tkrotoff@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WINSHELLCONTEXTMENU_H
#define WINSHELLCONTEXTMENU_H

//#include <windows.h>
//#include <initguid.h>
//#include <shlguid.h>

//For the interfaces IContextMenu and IShellExtInit
#include <shlobj.h>


//GUID of our shell extension
GUID CLSID_ShellExtension = {0xDCD2E883,0xA2E4,0x11DD,{0xB7,0x47,0x0,0xD,0x9D,0x95,0x33,0x2B}};

/**
 *
 * Tested under Windows XP and Windows Vista.
 *
 * DLL registration: "regsvr32 winshellcontextmenu.dll"
 * DLL unregistration: "regsvr32 -u winshellcontextmenu.dll"
 *
 * Source code taken from:
 * C++ implementation: http://www.cppfrance.com/codes/EXTENSION-SHELL-MENU-CONTEXTUEL-WIN32-API_48345.aspx
 * C implementation: http://www.cppfrance.com/codes/EXTENSION-SHELL-MENU-CONTEXTUEL-WIN32-API_48410.aspx
 * I guess all of this is underpublic domain
 *
 * MSDN documentation, "Creating Shell Extension Handlers": http://msdn.microsoft.com/en-us/library/cc144067(VS.85).aspx
 *
 * Other links about COM interfaces:
 * http://www.flipcode.com/documents/doccom.html
 * http://faculty.cascadia.edu/mpanitz/com_tutorial/
 * http://msdn.microsoft.com/en-us/library/ms694363(VS.85).aspx
 * http://msdn.microsoft.com/en-us/library/ms809980.aspx
 * http://www.codeproject.com/KB/COM/comintro.aspx
 * http://graphcomp.com/info/specs/com/comch01.htm
 */

/**
 * Interface IClassFactory.
 *
 * @author Tanguy Krotoff
 */
class CClassFactory : public IClassFactory {
public:

	CClassFactory();
	~CClassFactory();

	//Inherited from interface IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR * ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//Inherited from interface IClassFactory
	HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID * ppvObject);
	HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);

private:

	ULONG m_cRef;
};


/**
 * Interface IContextMenu.
 *
 * @author Tanguy Krotoff
 */
class CContextMenu : public IContextMenu, IShellExtInit {
public:

	CContextMenu();
	~CContextMenu();

	//Inherited from interface IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR * ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//Inherited from interface IContextMenu
	HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR * pwReserved, LPSTR pszName, UINT cchMax);
	HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO pici);
	HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

	//Inherited from interface IShellExtInit
	HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

private:

	ULONG m_cRef;
	LPDATAOBJECT m_pDataObj;
	char m_szPath[MAX_PATH];
	BOOL m_bBackGround;
	BOOL m_bIsFCH;
};

#endif	//WINSHELLCONTEXTMENU_H
