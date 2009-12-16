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

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682521%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms691379%28VS.85%29.aspx
	 */
	ULONG STDMETHODCALLTYPE AddRef();

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682317%28VS.85%29.aspx
	 */
	ULONG STDMETHODCALLTYPE Release();

	/**
	 * Inherited from interface IClassFactory.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682215%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown * pUnkOuter, REFIID riid, void ** ppvObject);

	/**
	 * Inherited from interface IClassFactory.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682332%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock);

private:

	ULONG m_cRef;
};


class CContextMenu : public IContextMenu, IShellExtInit {
public:

	CContextMenu();
	virtual ~CContextMenu();

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682521%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject);

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms691379%28VS.85%29.aspx
	 */
	ULONG STDMETHODCALLTYPE AddRef();

	/**
	 * Inherited from interface IUnknown.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/ms682317%28VS.85%29.aspx
	 */
	ULONG STDMETHODCALLTYPE Release();

	/**
	 * Inherited from interface IShellExtInit.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/bb775094%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject * pDataObj, HKEY hKeyProgID);

	/**
	 * Inherited from interface IContextMenu.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/bb776094%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT * pwReserved, LPSTR pszName, UINT cchMax);

	/**
	 * Inherited from interface IContextMenu.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/bb776096%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE InvokeCommand(LPCMINVOKECOMMANDINFO pici);

	/**
	 * Inherited from interface IContextMenu.
	 *
	 * @see http://msdn.microsoft.com/en-us/library/bb776097%28VS.85%29.aspx
	 */
	HRESULT STDMETHODCALLTYPE QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);

private:

	/** Execute quarkplayer.exe. */
	HRESULT STDMETHODCALLTYPE InvokeQuarkPlayer(HWND hParent, TCHAR * args);

	ULONG m_cRef;
	IDataObject * m_pDataObj;

	/** QuarkPlayer icon that will be show inside the context menu. */
	HBITMAP m_hQuarkPlayerBmp;
};

#endif	//WINCONTEXTMENU_H
