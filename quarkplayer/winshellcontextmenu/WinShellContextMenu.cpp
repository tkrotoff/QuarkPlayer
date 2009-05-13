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

//Compteur de référence de notre DLL
UINT g_cRefDll = 0;

CContextMenu::CContextMenu() {
	m_cRef = 0L;
	m_pDataObj = NULL;
	g_cRefDll++;
}

CContextMenu::~CContextMenu() {
	if (m_pDataObj) {
		m_pDataObj->Release();
	}
	g_cRefDll--;
}

HRESULT STDMETHODCALLTYPE CContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
	UINT idCmd = idCmdFirst;

	// Ajouter notre sous-menu "Chemin":
	HMENU hSubmenu = CreatePopupMenu();
	InsertMenu(hSubmenu, 0, MF_BYPOSITION, idCmd++, "Copier");
	InsertMenu(hSubmenu, 1, MF_BYPOSITION, idCmd++, "Afficher");
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = idCmd++;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = "Chemin";
	InsertMenuItem(hMenu, indexMenu++, TRUE, &mii);

	// bureau ou arrière-plan d'une fenêtre de l'explorateur
	if (m_bBackGround) {
		// Ajouter l'élément "Créer un fichier FCH":
		InsertMenu(hMenu, indexMenu++, MF_BYPOSITION, idCmd++, "Créer un fichier FCH");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - idCmdFirst);
	}

	// fichier normal
	else {
		// le fichier sélectionné porte l'extension .fch
		if (m_bIsFCH && ((uFlags & 0x000F) == CMF_NORMAL)) {
			// Ajouter l'élément "Voir le fichier FCH":
			InsertMenu(hMenu, indexMenu++, MF_BYPOSITION, idCmd++, "Voir le fichier FCH");
		}
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - idCmdFirst);
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
	if (!HIWORD(pici->lpVerb)) {
		UINT idCmd = LOWORD(pici->lpVerb);

		switch (idCmd) {
		case 0: {
			//Copier dans le clipboard le chemin complet du fichier ou dossier sélectionné:
			OpenClipboard(pici->hwnd);
			UINT len = lstrlen(m_szPath)+1;
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, len);
			LPVOID lpText = GlobalLock(hGlobal);
			memcpy(lpText, m_szPath, len);
			EmptyClipboard();
			GlobalUnlock(hGlobal);
			SetClipboardData(CF_TEXT, hGlobal);
			CloseClipboard();
			GlobalFree(hGlobal);
			return S_OK;
		}

		case 1:
			// Afficher le chemin complet du fichier ou dossier sélectionné:
			MessageBox(pici->hwnd, m_szPath, "RACPP Shell Extension", MB_OK);
			return S_OK;

		case 3:
			if (!m_bBackGround) {
				// Ouvrir le fichier FCH sélectionné:
				ShellExecute(0, 0, m_szPath,0 ,0 ,1);
			} else {
				// Créer un nouveau fichier FCH vide:
				lstrcat(m_szPath, "\\Nouveau fichier FCH.fch");
				HANDLE hFile = CreateFile(m_szPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile != INVALID_HANDLE_VALUE) {
					CloseHandle(hFile);
				}
				else {
					MessageBox(pici->hwnd,"Echec de création du nouveau fichier FCH","RACPP Shell Extension",0);
				}
			}
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE CContextMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR * pwReserved, LPSTR pszName, UINT cchMax) {
	// Textes à afficher dans la barre d'état d'une fenêtre de l'explorateur quand
	// l'un de nos éléments du menu contextuel est pointé par la souris:
	PWCHAR helpstrW[] = {L"Copier le chemin complet dans le presse-papier",L"Afficher le chemin complet",L"Copier ou afficher le chemin complet",L"Ouvrir ce fichier avec le Bloc-notes",L"Créer un nouveau fichier FCH vide"};
	PCHAR helpstrA[] = {"Copier le chemin complet dans le presse-papier","Afficher le chemin complet","Copier ou afficher le chemin complet","Ouvrir ce fichier avec le Bloc-notes","Créer un nouveau fichier FCH vide"};
	int offset = 0;
	if (m_bBackGround && idCmd == 3) {
		offset++;
	}
	switch(uFlags) {
	case GCS_HELPTEXTA:
		lstrcpynA(pszName, helpstrA[idCmd + offset], cchMax);
		break;
	case GCS_HELPTEXTW:
		lstrcpynW((LPWSTR) pszName, helpstrW[idCmd + offset], cchMax);
		break;
	case GCS_VERBA:
		break;
	case GCS_VERBW:
		break;
	case GCS_VALIDATEA:
	case GCS_VALIDATEW:
		if (idCmd < 0 || idCmd > 3) {
			return S_FALSE;
		}
		break;
	}
	return S_OK;
}

//IShellExtInit::Initialize() implementation
HRESULT STDMETHODCALLTYPE CContextMenu::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hRegKey) {
	if (m_pDataObj) {
		m_pDataObj->Release();
	}
	m_szPath[0] = 0;

	//clic droit sur fichier
	if (pDataObj) {
		m_bBackGround = 0;
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
		STGMEDIUM medium;
		FORMATETC fe = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
		HRESULT hres = pDataObj->GetData(&fe, &medium);

		// Obtenir le nombre de fichiers sélectionnés:
		UINT nombre = DragQueryFile((HDROP) medium.hGlobal, 0xFFFFFFFF, 0, 0);
		if (nombre != 1) {
			ReleaseStgMedium(&medium);
			return E_FAIL;
		}

		// Récupérer le chemin complet du fichier ou dossier sélectionné:
		DragQueryFile((HDROP) medium.hGlobal, 0,  m_szPath,  MAX_PATH);
		ReleaseStgMedium(&medium);

		// Mettre le flag mbIsFCH à 1 si l'extension est .fch sinon à 0:
		int len = lstrlen(m_szPath);
		if (len > 4) {
			m_bIsFCH =! lstrcmpi(m_szPath + (len - 4), ".fch");
		} else {
			m_bIsFCH=0;
		}
	}

	// clic droit sur bureau ou arrière-plan d'une fenêtre de l'explorateur:
	else {
		m_bBackGround = 1;

		// Récupérer le chemin complet du dossier courant:
		SHGetPathFromIDList(pIDFolder, m_szPath);
	}

	return NOERROR;
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

CClassFactory::CClassFactory() {
	m_cRef = 0L;
	g_cRefDll++;
}

CClassFactory::~CClassFactory() {
	g_cRefDll--;
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




//HINSTANCE de notre DLL
HINSTANCE g_hmodDll = 0;

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID * ppvOut) {
	*ppvOut = NULL;
	if (IsEqualGUID(rclsid, CLSID_ShellExtension)) {
		CClassFactory * pcf = new CClassFactory;
		return pcf->QueryInterface(riid, ppvOut);
	}
	return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow(void) {
	if (g_cRefDll == 0) {
		return S_OK;
	}
	return S_FALSE;
}

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
	GetModuleFileName(g_hmodDll,buffer,MAX_PATH);
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	lstrcpy(buffer,"Apartment");
	RegSetValueEx(hKey,"ThreadingModel",0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
	RegCloseKey(hKey);
	// Enregistrer notre "Extension Shell" pour tous les types de fichiers:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"*\\shellex\\ContextMenuHandlers\\RACPPMenu",0,0,0, KEY_WRITE, 0, &hKey,0);
	lstrcpy(buffer,"{DCD2E883-A2E4-11DD-B747-000D9D95332B}");
	RegSetValueEx(hKey,0,0,REG_SZ,(LPBYTE)buffer,lstrlen(buffer)+1);
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
	GetModuleFileName(g_hmodDll,buffer,MAX_PATH);
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
	// Enregistrer notre "Extension Shell" pour le bureau ou l'arrière-plan d'une fenêtre de l'explorateur:
	RegCreateKeyEx(HKEY_CLASSES_ROOT,"Directory\\Background\\ShellEx\\ContextMenuHandlers\\RACPPMenu",0,0,0, KEY_WRITE, 0, &hKey,0);
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
	// Retirer notre "Extension Shell" du bureau et de l'arrière-plan des fenêtres de l'explorateur:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"Directory\\Background\\ShellEx\\ContextMenuHandlers",0,DELETE,&hKey);
	RegDeleteKey(hKey,"RACPPMenu");
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
	// Retirer notre "Extension Shell" pour tous les types de fichiers:
	RegOpenKeyEx(HKEY_CLASSES_ROOT,"*\\shellex\\ContextMenuHandlers",0,DELETE,&hKey);
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

//Entrance function DllMain
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		g_hmodDll = hInstance;
	}
	return 1;
}
