
/*
 * Miranda IM LCD Plugin
 * Displays incoming text messages on an LCD.
 *
 * Copyright (c) 2003 Martin Rubli, mrubli@gmx.net
 *
 ******************************************************************************
 * This file is part of Miranda IM LCD Plugin.
 *
 * Miranda IM LCD Plugin is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * Miranda IM LCD Plugin is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Miranda IM LCD Plugin; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ******************************************************************************
 *
 * Miranda.cpp: Miranda plugin initialisation
 */

/*
 * TODO:

*/
/*
 * CHANGES:
 */
/*
 * KNOWN BUGS:
 *
 */

/*
 * IDEAS:
 *
 */

#ifdef _DEBUG
	#include <crtdbg.h>
	#define _CRTDBG_MAP_ALLOC
#endif

#include "StdAfx.h"

#include "CAppletManager.h"
#include "CConfig.h"

#include "m_system.h"

struct MM_INTERFACE memoryManagerInterface;
struct UTF8_INTERFACE utfi;

// SETTINGS
#define LCD_FPS 10

//************************************************************************
// Variables
//************************************************************************
bool g_bInitialized;
bool g_bUnicode;
// AppletManager object
 CAppletManager* g_AppletManager;

// Plugin Information

HINSTANCE hInstance;
PLUGINLINK *pluginLink;

// Initialization Hook
static HANDLE hMIHookModulesLoaded;

// {58D63981-14C1-4099-A3F7-F4FAA4C8FC59}
#define MIID_G15APPLET	{ 0x58d63981, 0x14c1, 0x4099, { 0xa3, 0xf7, 0xf4, 0xfa, 0xa4, 0xc8, 0xfc, 0x59 } }

static const MUUID interfaces[] = {MIID_G15APPLET, MIID_LAST};
	
static PLUGININFOEX pluginInfo = {
	sizeof(PLUGININFOEX),
	APP_NAME,
	PLUGIN_MAKE_VERSION(0,1,2,0),
	"Provides an interface to use Miranda from the LCD of various Logitech devices",
	"Martin Kleinhans",
	"mail@mkleinhans.de",
	"� 2009 Martin Kleinhans",
	"http://www.mkleinhans.de",
	0,		// not transient
	0,		// doesn't replace anything built-in
	#if defined( _UNICODE )
	{ 0x798221e1, 0xe47a, 0x4dc8, { 0x90, 0x77, 0x1e, 0x57, 0x6f, 0x9c, 0x43, 0x7 } }
	// {798221E1-E47A-4dc8-9077-1E576F9C4307}
	#else
	{ 0xf3815a8d, 0x6816, 0x4959, { 0xb2, 0xc4, 0x6a, 0xfc, 0x2e, 0x79, 0x31, 0xa9 } }
	// {F3815A8D-6816-4959-B2C4-6AFC2E7931A9}
	#endif
};

// Function Prototypes
int Init(WPARAM,LPARAM);
void UnInit();



//************************************************************************
// Exported Functions
//************************************************************************
extern "C" {
	__declspec(dllexport) const MUUID * MirandaPluginInterfaces(void)
	{
		return interfaces;
	}

	// Called by Miranda < 0.7 to get information about the plugin.
	__declspec(dllexport) PLUGININFO* MirandaPluginInfo(DWORD mirandaVersion)
	{
		if (mirandaVersion < PLUGIN_MAKE_VERSION(0,8,0,0))
		{
			tstring text = _T("This plugin is made for Miranda >= 0.8.0\nPlease consider upgrading Miranda, or use an older plugin version until you do so.");
			tstring title = _T(APP_SHORTNAME);
			MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION);
			return NULL;
		}
		
		pluginInfo.cbSize = sizeof(PLUGININFO);
		return (PLUGININFO*)&pluginInfo;
	}

	// Called by Miranda >= 0.7 to get information about the plugin.
	__declspec(dllexport) PLUGININFOEX* MirandaPluginInfoEx(DWORD mirandaVersion)
	{
		// Are we running under Unicode Windows version ?
		g_bUnicode = (GetVersion() & 0x80000000) == 0;
		if (g_bUnicode) {
		  pluginInfo.flags = 1; // UNICODE_AWARE
		}
		return &pluginInfo;
	}
	
	
	// Called by Miranda to load the plugin.
	// We defer initialization until Miranda's module loading process completed and return 0 to
	// mark success, everything else will cause the plugin to be freed right away.
	int __declspec(dllexport) Load(PLUGINLINK *link)
	{
		g_bInitialized = false;
		InitDebug();
		TRACE(_T("Plugin loaded\n"));
		pluginLink = link;
		// Schedule actual initialization for later
		// (We don't really need the handle but want to be able to release it properly later ...)
		hMIHookModulesLoaded = HookEvent(ME_SYSTEM_MODULESLOADED, Init);
		if(hMIHookModulesLoaded == 0)
		{
			tstring text = _T("Failed to initialize the Applet.\nThe plugin will not be loaded. ");
			tstring title = _T(APP_SHORTNAME);
			MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION);
			return 1;
		}
		return 0;
	}

	// Called by Miranda when the plugin should unload itself.
	int __declspec(dllexport) Unload(void)
	{
		if(!g_bInitialized) {
			TRACE(_T("ERROR: Unload requested, but plugin is not initialized?!\n"));		
			return 0;
		}
		TRACE(_T("-------------------------------------------\nUnloading started\n"));
		UnInit();
		TRACE(_T("Unloading successful\n"));
		TRACE(_T("Cleaning up: "));
		UnhookEvent(hMIHookModulesLoaded);			// just to be really correct ...
		UnInitDebug();
		TRACE(_T("OK!\n"));
		return 0;
	}
}

//************************************************************************
// DllMain
//
// EntryPoint of the DLL
//************************************************************************
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	
	hInstance = hinstDLL;
	return TRUE;
}

//************************************************************************
// Init
//
// Called after Miranda has finished loading all modules
// This is where the main plugin initialization happens and the
// connection to the LCD is established,
//************************************************************************
int Init(WPARAM wParam,LPARAM lParam)
{
	g_AppletManager = new CAppletManager();
	// Memoryleak Detection
	#ifdef _DEBUG
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		
	#endif
	
	// set the memory manager
	mir_getMMI( &mmi );
	mir_getUTFI( &utfi );

	 // set up the LCD context as non-autostart, non-persist, callbacked
	CConfig::Initialize();

	// Initialize the output object
    if(!g_AppletManager->Initialize(toTstring(APP_SHORTNAME)))
	{
		if(CConfig::GetBoolSetting(SKIP_DRIVER_ERROR)) {
			tstring text = _T("Failed to initialize the LCD connection\n Make sure you have the newest Logitech drivers installed (>=1.03).\n");
			tstring title = _T(APP_SHORTNAME);
			MessageBox(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION);
		}
		
		TRACE(_T("Initialization failed!.\n"));
		return 0;
	}
	
	g_bInitialized = true;
	TRACE(_T("Initialization completed successfully.\n-------------------------------------------\n"));
	return 0;
}
//************************************************************************
// UnInit
//
// Called when the plugin is about to be unloaded
//************************************************************************
void UnInit(void)
{
	g_AppletManager->Shutdown();	
	delete g_AppletManager;
	UnhookEvent(hMIHookModulesLoaded);
	
//#ifdef _DEBUG
//	_CrtDumpMemoryLeaks();
//#endif
}
