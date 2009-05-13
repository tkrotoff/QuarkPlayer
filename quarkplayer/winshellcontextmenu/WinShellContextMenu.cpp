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

#include "WinShellContextMenu.h"

//Our DLL reference count
UINT _dllCount = 0;

//HINSTANCE of our DLL
HINSTANCE _hModule = NULL;

void MsgBoxError(HWND hWnd, LPTSTR message) {
	MessageBox(hWnd, message,
		"QuarkPlayer Shell Extension",
		MB_OK | MB_ICONSTOP
	);
}

CContextMenu::CContextMenu() {
	m_cRef = 0L;
	m_pDataObj = NULL;
	_dllCount++;
}

CContextMenu::~CContextMenu() {
	if (m_pDataObj) {
		m_pDataObj->Release();
	}
	_dllCount--;
}

//OK
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

//OK
ULONG STDMETHODCALLTYPE CContextMenu::AddRef() {
	return ++m_cRef;
}

//OK
ULONG STDMETHODCALLTYPE CContextMenu::Release() {
	if (--m_cRef) {
		return m_cRef;
	}
	delete this;
	return 0L;
}

//OK
//IShellExtInit::Initialize() implementation
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

//~OK
HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
	//Right click on a normal file

	UINT idCmd = idCmdFirst;
	InsertMenu(hMenu, indexMenu++, MF_STRING | MF_BYPOSITION, idCmd++, "WinShellContextMenu Play");

	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - idCmdFirst);
}

//OK
HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) {
	HRESULT hres = E_INVALIDARG;

	if (!HIWORD(lpcmi->lpVerb)) {
		UINT idCmd = LOWORD(lpcmi->lpVerb);

		switch (idCmd) {
		case 0:
			hres = invokeQuarkPlayer(lpcmi->hwnd);
			break;
		}
	}
	return hres;
}

//OK
HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR * pwReserved, LPSTR pszName, UINT cchMax) {
	//Text fo show inside the Explorer status bar
	//when the mouse points to an item from the context menu

	switch (uFlags) {
	case GCS_HELPTEXT:
		lstrcpy(pszName, "Open with QuarkPlayer");
		break;
	default:
		break;
	}
	return S_OK;
}

//~OK
HRESULT STDMETHODCALLTYPE CContextMenu::invokeQuarkPlayer(HWND hParent) {
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
	TCHAR * pDest = strrchr(moduleFileName, '\\');
	pDest++;
	pDest[0] = 0;

	//quarkplayer.exe full path
	LPTSTR commandLine;
	UINT size = MAX_PATH * (nbFilesSelected + 1) * sizeof(TCHAR);
	commandLine = (LPTSTR) CoTaskMemAlloc(size);
	if (!commandLine) {
		MsgBoxError(hParent, "Insufficient memory available.");
		return E_OUTOFMEMORY;
	}
	lstrcpy(commandLine, "\"");
	lstrcat(commandLine, moduleFileName);
	lstrcat(commandLine, "quarkplayer.exe");
	lstrcat(commandLine, "\"");

	//Files selected by the user
	TCHAR fileUserClickedOn[MAX_PATH];

	for (UINT i = 0; i < nbFilesSelected; i++) {
		DragQueryFile((HDROP) stgMedium.hGlobal, i, fileUserClickedOn, MAX_PATH);
		lstrcat(commandLine, " \"");
		lstrcat(commandLine, fileUserClickedOn);
		lstrcat(commandLine, "\"");
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

	MsgBoxError(hParent, commandLine);

	CoTaskMemFree(commandLine);

	if (!result) {
		MsgBoxError(hParent,
			"Error creating process: quarpkayercontextmenu.dll needs to be in the same directory than quarkplayer.exe");
		return E_FAIL;
	}

	return S_OK;
}





//OK
CClassFactory::CClassFactory() {
	m_cRef = 0L;
	_dllCount++;
}

//OK
CClassFactory::~CClassFactory() {
	_dllCount--;
}

//OK
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

//OK
ULONG STDMETHODCALLTYPE CClassFactory::AddRef() {
	return ++m_cRef;
}

//OK
ULONG STDMETHODCALLTYPE CClassFactory::Release() {
	if (--m_cRef) {
		return m_cRef;
	}
	delete this;
	return 0L;
}

//OK
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

//OK
HRESULT STDMETHODCALLTYPE CClassFactory::LockServer(BOOL fLock) {
	return S_OK;
}





//OK
//Entrance function DllMain
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		_hModule = hInstance;
	}
	return 1;
}

//OK
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvOut) {
	*ppvOut = NULL;
	if (IsEqualGUID(rclsid, CLSID_ShellExtension)) {
		CClassFactory * pcf = new CClassFactory;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

//OK
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

static const TCHAR shellExtensionTitle[] = "QuarkPlayer";

static const UINT GUID_SIZE = 128;

STDAPI DllRegisterServer(void) {
	//Module full path
	TCHAR moduleFileName[MAX_PATH];
	GetModuleFileName(_hModule, moduleFileName, MAX_PATH);

	LPOLESTR pwsz;
	StringFromIID(CLSID_ShellExtension, &pwsz);
	TCHAR szCLSID[GUID_SIZE + 1];
	//lstrcpy(szCLSID, pwsz);
	WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
	CoTaskMemFree(pwsz);

	DOREGSTRUCT CLSIDEntries[] = {
		HKEY_CLASSES_ROOT, "CLSID\\%s", NULL, (LPTSTR) shellExtensionTitle,
		HKEY_CLASSES_ROOT, "CLSID\\%s\\InprocServer32", NULL, moduleFileName,
		HKEY_CLASSES_ROOT, "CLSID\\%s\\InprocServer32", "ThreadingModel", "Apartment",
		HKEY_CLASSES_ROOT, "*\\shellex\\ContextMenuHandlers\\QuarkPlayer", NULL, szCLSID,
		NULL, NULL, NULL, NULL
	};

	for (UINT i = 0; CLSIDEntries[i].hRootKey; i++) {
		//Create the sub key string - for this case, insert the file extension
		TCHAR szSubKey[MAX_PATH];
		wsprintf(szSubKey, CLSIDEntries[i].szSubKey, szCLSID);

		HKEY hKey;
		DWORD dwDisp;
		LONG result = RegCreateKeyEx(CLSIDEntries[i].hRootKey, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp);

		if (result == ERROR_SUCCESS) {
			//If necessary, create the value string
			TCHAR szData[MAX_PATH];
			wsprintf(szData, CLSIDEntries[i].szData, moduleFileName);

			RegSetValueEx(hKey, CLSIDEntries[i].lpszValueName, 0, REG_SZ, (LPBYTE) szData, (lstrlen(szData) + 1) * sizeof(TCHAR));
			RegCloseKey(hKey);
		} else {
			return E_FAIL;
		}
	}

	return S_OK;
}

STDAPI DllUnregisterServer(void) {
	TCHAR szKeyTemp[MAX_PATH + GUID_SIZE];
	wsprintf(szKeyTemp, "*\\shellex\\ContextMenuHandlers\\%s", shellExtensionTitle);
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);

	TCHAR szCLSIDKey[GUID_SIZE + 32];
	lstrcpy(szCLSIDKey, "CLSID\\");

	LPOLESTR pwsz;
	StringFromIID(CLSID_ShellExtension, &pwsz);
	TCHAR szCLSID[GUID_SIZE + 1];
	//lstrcpy(szCLSID, pwsz);
	WideCharToMultiByte(CP_ACP, 0, pwsz, -1, szCLSID, ARRAYSIZE(szCLSID), NULL, NULL);
	CoTaskMemFree(pwsz);

	lstrcat(szCLSIDKey, szCLSID);

	wsprintf(szKeyTemp, "%s\\%s", szCLSIDKey, TEXT("InprocServer32"));
	RegDeleteKey(HKEY_CLASSES_ROOT, szKeyTemp);
	RegDeleteKey(HKEY_CLASSES_ROOT, szCLSIDKey);

	return S_OK;
}

/*
STDAPI DllRegisterServer(void) {
	char buffer[MAX_PATH];
	HKEY hKey;

	// Ajouter le CLSID de notre DLL COM à la base de registres:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"CLSID\\{DCD2E883-A2E4-11DD-B747-000D9D95332B}",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"RACPP Shell Extension");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Définir le chemin et les paramètres de notre DLL COM:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"CLSID\\{DCD2E883-A2E4-11DD-B747-000D9D95332B}\\InProcServer32",0,0,0, KEY_WRITE, 0, &hKey,0);
	GetModuleFileName(_hModule,buffer,MAX_PATH);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	lstrcpy(buffer,"Apartment");
	RegSetValueEx(hKey,"ThreadingModel",0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Enregistrer notre "Extension Shell" pour les dossiers:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"Directory\\shellex\\ContextMenuHandlers\\RACPPMenu",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Ajouter l'extenstion de notre type de fichier à la base de registres:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,".fch",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"FCHFile");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Ajouter la description de notre type de fichier:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"FCHFile",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"Fichier FCH");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Définir l'icone par défaut de notre type de fichier:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\DefaultIcon",0,0,0, KEY_WRITE, 0, &hKey,0);
	GetModuleFileName(_hModule,buffer,MAX_PATH);
	lstrcat(buffer,",0");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Notifier le Shell pour qu'il prenne en compte la définition de l'icone par défaut:
	SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

	// Définir le Bloc-notes comme programme d'ouverture par défaut:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\Shell\\Open\\Command",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"%SystemRoot%\\System32\\notepad.exe %1");
	RegSetValueEx(hKey,0,0,REG_EXPAND_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Enregistrer notre "Extension Shell" pour notre type de fichier:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\shellex\\ContextMenuHandlers\\RACPPMenu",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	// Ajouter notre "Extension Shell" à la liste approuvée du système:
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"RACPP Shell Extension");
	RegSetValueEx(hKey,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}",0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);

	return S_OK;
}

STDAPI DllUnregisterServer(void) {
	HKEY hKey;

	// Retirer notre "Extension Shell" de la liste approuvée du système:
	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved",0,DELETE,&hKey);
	RegDeleteValue(hKey,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}");
	RegCloseKey(hKey);

	// Retirer notre "Extension Shell" pour notre type de fichier:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\shellex\\ContextMenuHandlers",0,DELETE,&hKey);
	RegDeleteKey(hKey,"RACPPMenu");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\shellex",0,DELETE,&hKey);
	RegDeleteKey(hKey,"ContextMenuHandlers");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile",0,DELETE,&hKey);
	RegDeleteKey(hKey,"shellex");
	RegCloseKey(hKey);

	// Effacer les sous-clés de définition du programme d'ouverture par défaut de notre type de fichier:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\Shell\\Open",0,DELETE,&hKey);
	RegDeleteKey(hKey,"Command");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile\\Shell",0,DELETE,&hKey);
	RegDeleteKey(hKey,"Open");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile",0,DELETE,&hKey);
	RegDeleteKey(hKey,"Shell");
	RegCloseKey(hKey);

	// Effacer la sous-clé de définition de l'icone par défaut de notre type de fichier:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"FCHFile",0,DELETE,&hKey);
	RegDeleteKey(hKey,"DefaultIcon");
	RegCloseKey(hKey);

	// Notifier le Shell pour qu'il prenne en compte la suppression de l'icone par défaut:
	SHChangeNotify(SHCNE_ASSOCCHANGED,0,0,0);

	// Effacer notre type de fichier de la base de registres:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,0,0,DELETE,&hKey);
	RegDeleteKey(hKey,"FCHFile");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,0,0,DELETE,&hKey);
	RegDeleteKey(hKey,".fch");
	RegCloseKey(hKey);

	// Retirer notre "Extension Shell" des dossiers:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"Directory\\shellex\\ContextMenuHandlers",0,DELETE,&hKey);
	RegDeleteKey(hKey,"RACPPMenu");
	RegCloseKey(hKey);

	// Retirer le CLSID de notre DLL COM de la base de registres:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID\\{DCD2E883-A2E4-11DD-B747-000D9D95332B}",0,DELETE,&hKey);
	RegDeleteKey(hKey,"InProcServer32");
	RegCloseKey(hKey);
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID",0,DELETE,&hKey);
	RegDeleteKey(hKey,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}");
	RegCloseKey(hKey);

	return S_OK;
}
*/
