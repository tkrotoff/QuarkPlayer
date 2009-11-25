/*
 * QuarkPlayer, a Phonon media player
 * Copyright (C) 2000-2008  Andre Burgaud <andre@burgaud.com>
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

#ifndef WINCONTEXTMENU_H
#define WINCONTEXTMENU_H

/**
 * Context menu shell extension for QuarkPlayer.
 *
 * DLL registration: "regsvr32 /s quarkplayercontextmenu.dll"
 * DLL unregistration: "regsvr32 /s /u quarkplayercontextmenu.dll"
 *
 * Original source code taken from:
 * - SciTE Context Menu by Andre Burgaud, see http://www.burgaud.com/category/wscitecm/
 *
 * Documentation links:
 * The Complete Idiot's Guite to Writing Shell Extensions, Part I: http://www.codeproject.com/KB/shell/shellextguide1.aspx
 * Creating Shell Extension Handlers: http://msdn.microsoft.com/en-us/library/cc144067(VS.85).aspx
 * Introduction to COM by Adrian Perez : http://www.flipcode.com/documents/doccom.html
 * Mike Panitz's Introduction to COM: http://faculty.cascadia.edu/mpanitz/com_tutorial/
 * Introduction to COM - What It Is and How to Use It: http://www.codeproject.com/KB/COM/comintro.aspx
 * Introduction to COM by Microsoft: http://msdn.microsoft.com/en-us/library/ms694363(VS.85).aspx
 * COM technical overview by Microsoft: http://msdn.microsoft.com/en-us/library/ms809980.aspx
 *
 * @author Tanguy Krotoff
 */

//GUID of our shell extension, generated using guidgen.exe provided with Visual C++
//Please do not copy-paste QuarkPlayer GUUID, just create your own to avoid conflicts
//{BC6D1C0E-ADF5-44a1-9940-978019DF7985}
//If changed, then update GUUID inside quarkplayer/WinDefaultApplication.cpp
DEFINE_GUID(CLSID_ShellExtension, 0xbc6d1c0e, 0xadf5, 0x44a1, 0x99, 0x40, 0x97, 0x80, 0x19, 0xdf, 0x79, 0x85);


class CClassFactory : public IClassFactory {
public:

	CClassFactory();
	virtual ~CClassFactory();

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


class CContextMenu : public IContextMenu, IShellExtInit {
public:

	CContextMenu();
	virtual ~CContextMenu();

	//Inherited from interface IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID FAR * ppvObject);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	//Inherited from interface IShellExtInit
	HRESULT STDMETHODCALLTYPE Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

	//Inherited from interface IContextMenu
	HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR * pwReserved, LPSTR pszName, UINT cchMax);
	HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

private:

	/** Execute quarkplayer.exe. */
	HRESULT STDMETHODCALLTYPE invokeQuarkPlayer(HWND hParent, LPTSTR args);

	ULONG m_cRef;
	LPDATAOBJECT m_pDataObj;

	/** QuarkPlayer icon that will be show inside the context menu. */
	HBITMAP m_hQuarkPlayerBmp;
};

#endif	//WINCONTEXTMENU_H
