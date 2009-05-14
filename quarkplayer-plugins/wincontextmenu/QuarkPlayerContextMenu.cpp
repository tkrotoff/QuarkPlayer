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

//IContextMenu and IShellExtInit
#include <shlobj.h>

//Otherwise we get: error LNK2001: unresolved external symbol _CLSID_ShellExtension
#define INITGUID
#include <initguid.h>
#include <shlguid.h>

#include "QuarkPlayerContextMenu.h"

//Defines IDB_QUARKPLAYER
#include "resource.h"

//Our DLL reference count
UINT _dllCount = 0;

//HINSTANCE of our DLL
HINSTANCE _hModule = NULL;

static const LPTSTR menuItems[] = {
	TEXT("Play"),
	TEXT("Enqueue")
};

static const LPTSTR menuItemsHelp[] = {
	TEXT("Play with QuarkPlayer"),
	TEXT("Enqueue in QuarkPlayer")
};

void MsgBoxError(HWND hWnd, LPTSTR message) {
	MessageBox(hWnd, message,
		TEXT("QuarkPlayer Shell Extension"),
		MB_OK | MB_ICONSTOP
	);
}

CContextMenu::CContextMenu() {
	m_cRef = 0L;
	m_pDataObj = NULL;
	_dllCount++;
	m_hQuarkPlayerBmp = LoadBitmap(_hModule, MAKEINTRESOURCE(IDB_QUARKPLAYER));
}

CContextMenu::~CContextMenu() {
	if (m_pDataObj) {
		m_pDataObj->Release();
	}
	_dllCount--;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryInterface(REFIID riid, LPVOID FAR * ppvObject) {
	*ppvObject = NULL;
	if (IsEqualGUID(riid, IID_IShellExtInit) || IsEqualGUID(riid, IID_IUnknown)) {
		*ppvObject = (LPSHELLEXTINIT) this;
	}
	else if (IsEqualGUID(riid, IID_IContextMenu)) {
		*ppvObject = (LPCONTEXTMENU) this;
	}

	if (*ppvObject) {
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CContextMenu::AddRef() {
	return ++m_cRef;
}

ULONG STDMETHODCALLTYPE CContextMenu::Release() {
	if (--m_cRef) {
		return m_cRef;
	}
	delete this;
	return 0L;
}

HRESULT STDMETHODCALLTYPE CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey) {
	if (m_pDataObj) {
		m_pDataObj->Release();
	}
	if (pDataObj) {
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {

	//
	//If the flags include CMF_DEFAULTONLY then we shouldn't do anything
	//
	//CMF_DEFAULTONLY: This flag is set when the user is activating the default action,
	//typically by double-clicking.
	//This flag provides a hint for the shortcut menu extension to add nothing
	//if it does not modify the default item in the menu.
	//A shortcut menu extension or drag-and-drop handler should not add any menu items
	//if this value is specified.
	//A namespace extension should add only the default item (if any).
	//See http://msdn.microsoft.com/en-us/library/bb776097.aspx
	//
	//If a file class has a context menu associated with it,
	//double-clicking an object normally launches the default command.
	//The handler's IContextMenu::QueryContextMenu method is not called.
	//To specify that the handler's IContextMenu::QueryContextMenu method should be called when an object is double-clicked,
	//create a shellex\MayChangeDefaultMenu  subkey under the handler's CLSID key.
	//When an object associated with the handler is double-clicked,
	//IContextMenu::QueryContextMenu will be called with the CMF_DEFAULTONLY flag set in the uFlags parameter.
	//
	//Note: Setting the MayChangeDefaultMenu key forces the system to load the handler's DLL when an associated item is double-clicked.
	//If your handler does not change the default verb, you should not set MayChangeDefaultMenu.
	//Doing so causes the system to load your DLL unnecessarily.
	//Context menu handlers should set this key only if they might need to change the context menu's default verb.
	//See http://msdn.microsoft.com/en-us/library/bb776881(VS.85).aspx
	//
	/*if (uFlags & CMF_DEFAULTONLY) {
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}*/

	//idCmdFirst is always equal to 0 ?
	UINT idCmd = idCmdFirst;

	//nIndex is always equal to 0 ?
	UINT nIndex = indexMenu;

	InsertMenu(hMenu, nIndex, MF_STRING | MF_BYPOSITION, idCmd++, menuItems[nIndex]);
	/*if (m_hQuarkPlayerBmp) {
		SetMenuItemBitmaps(hMenu, nIndex, MF_BYPOSITION, m_hQuarkPlayerBmp, m_hQuarkPlayerBmp);
	}*/
	//Change the default menu item and make it at the top of the menu
	SetMenuDefaultItem(hMenu, nIndex, TRUE);

	nIndex++;
	InsertMenu(hMenu, nIndex, MF_STRING | MF_BYPOSITION, idCmd++, menuItems[nIndex]);
	/*if (m_hQuarkPlayerBmp) {
		SetMenuItemBitmaps(hMenu, nIndex, MF_BYPOSITION, m_hQuarkPlayerBmp, m_hQuarkPlayerBmp);
	}*/

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - idCmdFirst);
}

HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) {
	HRESULT hres = E_INVALIDARG;

	if (!HIWORD(lpcmi->lpVerb)) {
		UINT idCmd = LOWORD(lpcmi->lpVerb);

		switch (idCmd) {
		case 0:
		case 1:
			hres = invokeQuarkPlayer(lpcmi->hwnd, TEXT(""));
			break;
		case 2:
			hres = invokeQuarkPlayer(lpcmi->hwnd, TEXT("--playlist-enqueue"));
			break;
		}
	}
	return hres;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR * pwReserved, LPSTR pszName, UINT cchMax) {
	//Text to show inside the Explorer status bar
	//when the mouse points to an item from the context menu

	//See http://msdn.microsoft.com/en-us/library/bb431714.aspx
	//There is no need to deal with the different uFlags, just GCS_HELPTEXT
	switch (uFlags) {
	case GCS_HELPTEXT:
		lstrcpyn((LPWSTR) pszName, menuItemsHelp[idCmd], cchMax);
		break;
	default:
		return E_INVALIDARG;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::invokeQuarkPlayer(HWND hParent, LPTSTR args) {
	//Get the number of files selected by the user
	UINT nbFilesSelected;
	STGMEDIUM stgMedium;
	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	HRESULT hres = m_pDataObj->GetData(&fmte, &stgMedium);
	if (SUCCEEDED(hres)) {
		if (stgMedium.hGlobal) {
			nbFilesSelected = DragQueryFile((HDROP) stgMedium.hGlobal, (UINT) -1, 0, 0);
		}
	}
	///

	//Module full path
	TCHAR moduleFileName[MAX_PATH];
	GetModuleFileName(_hModule, moduleFileName, MAX_PATH);
	//Get folder name
	TCHAR moduleDrive[_MAX_DRIVE];
	TCHAR modulePath[MAX_PATH];
#ifdef UNICODE
	_wsplitpath(moduleFileName, moduleDrive, modulePath, NULL, NULL);
#else
	_splitpath(moduleFileName, moduleDrive, modulePath, NULL, NULL);
#endif

	//quarkplayer.exe full path
	LPTSTR commandLine;
	UINT commandLineSize = MAX_PATH * (nbFilesSelected + 1) * sizeof(TCHAR);
	commandLine = (LPTSTR) CoTaskMemAlloc(commandLineSize);
	if (!commandLine) {
		MsgBoxError(hParent, TEXT("Insufficient memory available."));
		return E_OUTOFMEMORY;
	}
	lstrcpy(commandLine, TEXT("\""));
	lstrcat(commandLine, moduleDrive);
	lstrcat(commandLine, modulePath);
	lstrcat(commandLine, TEXT("quarkplayer.exe"));
	lstrcat(commandLine, TEXT("\" "));
	lstrcat(commandLine, args);

	//Files selected by the user
	TCHAR fileUserClickedOn[MAX_PATH];

	for (UINT i = 0; i < nbFilesSelected; i++) {
		DragQueryFile((HDROP) stgMedium.hGlobal, i, fileUserClickedOn, MAX_PATH);
		lstrcat(commandLine, TEXT(" \""));
		lstrcat(commandLine, fileUserClickedOn);
		lstrcat(commandLine, TEXT("\""));
	}

	//Launch quarkplayer.exe with the list of files as arguments
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	//STARTF_USESHOWWINDOW: The wShowWindow member is valid
	startupInfo.dwFlags = STARTF_USESHOWWINDOW;
	//SW_RESTORE: Activates and displays the window.
	//If the window is minimized or maximized, the system restores it to its original size and position.
	startupInfo.wShowWindow = SW_RESTORE;

	PROCESS_INFORMATION processInfo;

	//See http://msdn.microsoft.com/en-us/library/ms682425.aspx
	BOOL result = CreateProcess(
		NULL,	//LPCTSTR lpApplicationName
		commandLine,	//LPTSTR lpCommandLine
		NULL,	//LPSECURITY_ATTRIBUTES lpProcessAttributes
		NULL,	//LPSECURITY_ATTRIBUTES lpThreadAttributes
		FALSE,	//BOOL bInheritHandles
		0,	//DWORD dwCreationFlags
		NULL,	//LPVOID lpEnvironment
		NULL,	//LPCTSTR lpCurrentDirectory
		&startupInfo,	//LPSTARTUPINFO lpStartupInfo
		&processInfo	//LPPROCESS_INFORMATION lpProcessInformation
	);

	//MsgBoxError(hParent, commandLine);

	CoTaskMemFree(commandLine);

	if (!result) {
		MsgBoxError(hParent,
			TEXT("Error creating process: quarpkayercontextmenu.dll needs to be in the same directory as quarkplayer.exe"));
		return E_FAIL;
	}

	return S_OK;
}



CClassFactory::CClassFactory() {
	m_cRef = 0L;
	_dllCount++;
}

CClassFactory::~CClassFactory() {
	_dllCount--;
}

HRESULT STDMETHODCALLTYPE CClassFactory::QueryInterface(REFIID riid, LPVOID FAR * ppvObject) {
	*ppvObject = NULL;

	//Any interface on this object is the object pointer
	if (IsEqualGUID(riid, IID_IUnknown) || IsEqualGUID(riid, IID_IClassFactory)) {
		*ppvObject = (LPCLASSFACTORY) this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CClassFactory::AddRef() {
	return ++m_cRef;
}

ULONG STDMETHODCALLTYPE CClassFactory::Release() {
	if (--m_cRef) {
		return m_cRef;
	}
	delete this;
	return 0L;
}

HRESULT STDMETHODCALLTYPE CClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, LPVOID * ppvObject) {
	*ppvObject = NULL;
	if (pUnkOuter) {
		return CLASS_E_NOAGGREGATION;
	}
	CContextMenu * pContextMenu = new CContextMenu();
	if (!pContextMenu) {
		return E_OUTOFMEMORY;
	}
	return pContextMenu->QueryInterface(riid, ppvObject);
}

HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL fLock) {
	return S_OK;
}



//Entrance function DllMain
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		_hModule = hInstance;
	}
	return 1;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvOut) {
	*ppvOut = NULL;
	if (IsEqualGUID(rclsid, CLSID_ShellExtension)) {
		CClassFactory * pcf = new CClassFactory;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void) {
	if (_dllCount == 0) {
		return S_OK;
	}
	return S_FALSE;
}

typedef struct {
	HKEY hRootKey;
	LPTSTR szSubKey;
	LPTSTR lpszValueName;
	LPTSTR szData;
} DOREGSTRUCT;

static const TCHAR shellExtensionTitle[] = TEXT("QuarkPlayer");

static const UINT GUID_SIZE = 128;

STDAPI DllRegisterServer(void) {
	//Module full path
	TCHAR moduleFileName[MAX_PATH];
	GetModuleFileName(_hModule, moduleFileName, MAX_PATH);

	//
	LPOLESTR pwsz;
	StringFromIID(CLSID_ShellExtension, &pwsz);
	TCHAR szCLSID[GUID_SIZE + 1];
#ifdef UNICODE
	lstrcpy(szCLSID, pwsz);
#else
	WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
#endif
	CoTaskMemFree(pwsz);
	///

	DOREGSTRUCT CLSIDEntries[] = {
		HKEY_CLASSES_ROOT, TEXT("CLSID\\%s"), NULL, (LPTSTR) shellExtensionTitle,
		HKEY_CLASSES_ROOT, TEXT("CLSID\\%s\\InprocServer32"), NULL, moduleFileName,
		HKEY_CLASSES_ROOT, TEXT("CLSID\\%s\\InprocServer32"), TEXT("ThreadingModel"), TEXT("Apartment"),
		HKEY_CLASSES_ROOT, TEXT("CLSID\\%s\\shellex\\MayChangeDefaultMenu"), NULL, NULL,
		NULL, NULL, NULL, NULL
	};

	for (UINT i = 0; CLSIDEntries[i].hRootKey; i++) {
		//Create the sub key string
		TCHAR szSubKey[MAX_PATH];
		wsprintf(szSubKey, CLSIDEntries[i].szSubKey, szCLSID);

		HKEY hKey;
		DWORD dwDisp;
		LONG result = RegCreateKeyEx(CLSIDEntries[i].hRootKey, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);

		if (result == ERROR_SUCCESS) {
			if (CLSIDEntries[i].szData) {
				//If necessary, create the value string
				RegSetValueEx(hKey,
					CLSIDEntries[i].lpszValueName, 0, REG_SZ,
					(const BYTE *) CLSIDEntries[i].szData,
					(lstrlen(CLSIDEntries[i].szData) + 1) * sizeof(TCHAR)
				);
			}
			RegCloseKey(hKey);
		} else {
			return E_FAIL;
		}
	}

	return S_OK;
}

STDAPI DllUnregisterServer(void) {
	//
	LPOLESTR pwsz;
	StringFromIID(CLSID_ShellExtension, &pwsz);
	TCHAR szCLSID[GUID_SIZE + 1];
#ifdef UNICODE
	lstrcpy(szCLSID, pwsz);
#else
	WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
#endif
	CoTaskMemFree(pwsz);
	///

	TCHAR szKeyTemp[MAX_PATH + GUID_SIZE];
	wsprintf(szKeyTemp, TEXT("CLSID\\%s\\InprocServer32"), szCLSID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	wsprintf(szKeyTemp, TEXT("CLSID\\%s\\shellex\\MayChangeDefaultMenu"), szCLSID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	wsprintf(szKeyTemp, TEXT("CLSID\\%s\\shellex"), szCLSID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	wsprintf(szKeyTemp, TEXT("CLSID\\%s"), szCLSID);
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);

	return S_OK;
}
