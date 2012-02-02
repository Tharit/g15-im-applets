#ifndef _CAPPLETMANAGER_H_
#define _CAPPLETMANAGER_H_

#include "CConnectionData.h"
#include "CContactData.h"

#include "CLCDOutputManager.h"
#include "CEventScreen.h"
#include "CNotificationScreen.h"
#include "CChatScreen.h"
#include "CContactlistScreen.h"
#include "CCreditsScreen.h"

#include "CLCDConnectionLogitech.h"

class CEventAdd
{
	tstring strMedium;
	tstring strID;
};

class CAppletManager : public CLCDOutputManager
{
public:
	// returns the AppletManager's instance
	static CAppletManager *GetInstance();

	// Constructor
	CAppletManager();
	// Destructor
	~CAppletManager();

	// Activates the EventScreen
	void ActivateEventScreen();
	// Activates the Chatscreen
	void ActivateChatScreen(CContactData *pContactData,int iConnectionID = -1);
	// Activates the Contactlistscreen
	void ActivateContactlistScreen();
	// Activates the Creditsscreen
	void ActivateCreditsScreen();

	// Initializes the AppletManager
	bool Initialize(tstring strAppletName);
	// Deinitializes the AppletManager
	bool Shutdown();
	// Updates the AppletManager
	bool Update();
	
	// called when the configuration has changed
	void OnConfigChanged();

	// Regenerates the contact cache
	void RegenerateContactData();

	// the update timer's callback function
	static VOID CALLBACK UpdateTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	
	// Enumerations
	static int ConnectionCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int ContactCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int EnumSectionCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	// Events
	static int ChatEventCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int BroadcastCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int ConnectionChangeCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int ContactEventCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int WindowEventCallback(int windowID, char *subwindow, char *event, void *data, void *userData);
	static int ScreensaverEventCallback(int windowID, char *subwindow, char *event, void *data, void *userData);

	CConnectionData* GetConnectionData(tstring strMedium,int iConnection);
	CConnectionData* GetConnectionData(tstring strMedium,tstring strID);
	CContactData* GetContactDataByWindowID(int iWindowID);
	CContactData* GetContactDataByID(tstring strID,tstring strMedium,tstring strSection);
	
	CContactData* GetMetacontactBySubcontact(CContactData *pContactData);

	vector<CConnectionData*> *GetConnectionDataVector();
	vector<CContactData*> *GetContactDataVector();

	int	GetSectionCount();

	// returns a formatted timestamp string
	static tstring GetFormattedTimestamp(tm *time);
	// returns the bitmap for the specified status
	HBITMAP GetStatusBitmap(tstring strStatus);
	// returns the bitmap for the specified event
	HBITMAP GetEventBitmap(EventType eType, bool bLarge);
protected:
	// creates a message event
	static void CreateMessageEvent(CContactData *pContact,int iConnectionID, tstring strMessage, bool bSent);
	// sets the volumewheel hook
	void SetVolumeWheelHook();

	// stored Lightstate
	SG15LightStatus m_G15LightStatus;

	// Requests a contactdata / contactlist update
	void RequestUpdate();

	// checks wether a notification needs to be fired for this event
	bool NeedsNotification(CEvent *pEvent);

	// Handle the event
	void HandleEvent(CEvent *pEvent);
	
	// Called when the connection state has changed
	void OnConnectionChanged(int iConnectionState);
	// Called when the active screen has expired
	void OnScreenExpired(CLCDScreen *pScreen);

private:
	// connects to the specified event
	void RegisterEvent(char* szEvent,ttkCallback Callback);

	// enumerates all contacts
	void EnumerateContacts(int iConnection = -1,tstring strMedium=_T("all"),tstring strSection = _T(""));
	void EnumerateSections();

	// protocol data
	void DeleteConnectionData();
	void RemoveConnection(CConnectionData *pConData);

	vector<CConnectionData*> m_vConnectionData;
	vector<tstring> m_vSectionData;

	// contact data
	void DeleteContactData();
	void RemoveContactData(CContactData *pContact);
	vector<CContactData*> m_vContactData;

	static tstring StripHTML(char *szHtml);
	static tstring ConvertHTMLEntities(tstring strHtml);

	// update timer handle
	UINT					m_uiTimer;

	// last active screen
	CLCDScreen *m_pLastScreen;
	
	// Screens
	CEventScreen m_EventScreen;
	CNotificationScreen m_NotificationScreen;
	CChatScreen m_ChatScreen;
	CContactlistScreen m_ContactlistScreen;
	CCreditsScreen m_CreditsScreen;

	// registered callback events
	vector<int> m_vRegisteredEvents;
	int		m_iBroadcastCallback;
	bool m_bUpdateRequested;

	HBITMAP m_ahStatusBitmaps[8];
	HBITMAP m_ahEventBitmaps[4];
	HBITMAP m_ahLargeEventBitmaps[4];
};

#endif