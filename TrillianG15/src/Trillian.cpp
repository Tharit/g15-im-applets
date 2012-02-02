#include "stdafx.h"
#include "CAppletManager.h"
#include "CConfig.h"

#define MYGUID "892fe160-9d0f-4e33-92e3-7ba4e7aaeecf"

CAppletManager g_AppletManager;
plugin_function_send g_plugin_send = NULL; 
char *g_home_directory = NULL;

HINSTANCE hInstance;

int Init();
int UnInit();

//************************************************************************
// Exported Functions
//************************************************************************
extern "C" 
{

/******************************************************************************
 p l u g i n _ v e r s i o n ()
******************************************************************************

  Currently supporting version 1 of plugins.
*/
 
int __declspec(dllexport)plugin_version(void)
{
	return 1;
}

/******************************************************************************
 p l u g i n _ m a i n ()
******************************************************************************

  plugin_main() gets called whenever the UI has something to tell us.  This includes
  telling the plugin when to start, stop, unload, show preferences, etc.
*/
 
int __declspec(dllexport)plugin_main(char *event, void *data) 
{   
	// Plugin is loaded
	if (!_stricmp(event,"load")) 
	{ 
	   struct plugin_info_t *pi = (struct plugin_info_t *)data;

	   if (pi)
	   {
			/* Fill in plugin information.
			 */
			strcpy(pi->guid, APP_GUID);
			strcpy(pi->name, APP_SHORTNAME); 
			strcpy(pi->company, "Tharit"); 
			strcpy(pi->version, "1.0.4"); 
			strcpy(pi->description, "Applet for the Logitech G15 LCD");
         
			/* Extract what we need and store locally.
			 */

			g_home_directory = (char *)malloc(strlen(pi->config_directory)+1);
			strcpy(g_home_directory, pi->config_directory);

			g_plugin_send = pi->plugin_send; 
	   } 
	}
	// Plugin starts
	else if (!_stricmp(event,"start") && g_plugin_send)
	{
		Init();

		struct plugin_prefs_t		ppTop;
		struct plugin_prefs_entry_t		ppAppearance;
		struct plugin_prefs_entry_t		ppContactlist;
		struct plugin_prefs_entry_t		ppNotifications;
		struct plugin_prefs_entry_t		ppChatsessions;
		
		
		trillianInitialize(ppTop);
		trillianInitialize(ppAppearance);
		trillianInitialize(ppContactlist);
		trillianInitialize(ppNotifications);
		trillianInitialize(ppChatsessions);

		ppAppearance.sub_name = "Appearance";
		ppAppearance.next = &ppNotifications;

		ppNotifications.sub_name = "Notifications";
		ppNotifications.next = &ppContactlist;

		ppContactlist.sub_name = "Contactlist";
		ppContactlist.next = &ppChatsessions;

		ppChatsessions.sub_name = "Chatsessions";

		ppTop.enabled						= 1; 
		ppTop.pref_name					= "TrillianG15"; 
		ppTop.sub_entry					= &ppAppearance; 

		g_plugin_send(MYGUID, "prefsInitialize", &ppTop); 
	}
	// Plugin stops
	else if (!_stricmp(event,"stop") && g_plugin_send)
	{
		UnInit();
		free(g_home_directory);
	}
	// Plugin is unloaded
	else if (!_stricmp(event, "unload"))
	{
	}
	else if (!_stricmp(event, "prefsShow"))
	{
		struct plugin_prefs_show_t	*pps = (struct plugin_prefs_show_t *)data;

		/* 
		 * Trillian is telling us to show our preferences dialog - do so only if we find our
		 * name.
		 */

		if (!strcmp(pps->pref_name, "TrillianG15"))
		{
			CConfig::HandlePreferencesDialogs(pps);
		}
	}
	else if (!_stricmp(event, "prefsAction"))
	{
		struct plugin_prefs_action_t	*ppa = (struct plugin_prefs_action_t *)data;
		CConfig::HandlePreferencesActions(ppa);
	}
	return 0;
}

}

/******************************************************************************
 D l l M a i n ()
******************************************************************************/
int WINAPI DllMain(HANDLE hinstDLL, unsigned long dwReason, void *reserved)
{
	hInstance = (HINSTANCE)hinstDLL;

	switch (dwReason) {

		case DLL_THREAD_DETACH:
			break;

        case DLL_PROCESS_DETACH:
			break;
	}

	return 1;
}

/******************************************************************************
Init()
******************************************************************************/
int Init()
{
	CConfig::Initialize();

	if(!g_AppletManager.Initialize(toTstring(APP_SHORTNAME)))
	{
		tstring text = _T("Failed to initialize the LCD connection\n Make sure you have the newest Logitech drivers installed (>=1.04)\n ");
		tstring title = _T(APP_SHORTNAME);
		MessageBoxW(NULL, text.c_str(), title.c_str(), MB_OK | MB_ICONEXCLAMATION);
		
		return 1;
	}
	return 0;
}

/******************************************************************************
Init()
******************************************************************************/
int UnInit()
{
	g_AppletManager.Shutdown();	
	CConfig::Shutdown();
	return 0;
}
