#include "stdafx.h"
#include "CConfig.h"
#include "CAppletManager.h"

//************************************************************************
// returns the AppletManager's instance
//************************************************************************
CAppletManager *CAppletManager::GetInstance()
{
	return (CAppletManager*)CLCDOutputManager::GetInstance();
}

//************************************************************************
// Constructor
//************************************************************************
CAppletManager::CAppletManager()
{
	m_uiTimer = NULL;
	m_pLastScreen = NULL;
}

//************************************************************************
// Destructor
//************************************************************************
CAppletManager::~CAppletManager()
{
}

vector<CContactData*> *CAppletManager::GetContactDataVector()
{
	return &m_vContactData;
}

vector<CConnectionData*> *CAppletManager::GetConnectionDataVector()
{
	return &m_vConnectionData;
}

//************************************************************************
// connects to the specified event
//************************************************************************
void CAppletManager::RegisterEvent(char *szEvent,ttkCallback Callback)
{
	struct event_connect_t ect;
	trillianInitialize(ect);
	int iEventID = 0;

	ect.callback = Callback;
	ect.type = szEvent;
	iEventID = g_plugin_send(APP_GUID,"eventsConnect", &ect); 
	m_vRegisteredEvents.push_back(iEventID);
}

//************************************************************************
// Initializes the AppletManager
//************************************************************************
bool CAppletManager::Initialize(tstring strAppletName)
{
	if(!CLCDOutputManager::Initialize(strAppletName))
		return false;

	// set the volumewheel hook
	SetVolumeWheelHook();

	// enumerate all connections
	struct connection_enum_t	cet;
	trillianInitialize(cet);
	cet.callback = CAppletManager::ConnectionCallback;
	cet.medium = "all";					// Can also be "AIM", "ICQ", etc.
		
	g_plugin_send(APP_GUID, "connectionEnumerate", &cet);

	// enumerate all contacts
	EnumerateSections();

	// Initialize the screens
	m_EventScreen.Initialize();
	m_NotificationScreen.Initialize();
	m_ChatScreen.Initialize();
	m_ContactlistScreen.Initialize();
	m_CreditsScreen.Initialize();

	// Add the screens
	AddScreen(&m_EventScreen);
	AddScreen(&m_NotificationScreen);
	AddScreen(&m_ContactlistScreen);
	AddScreen(&m_ChatScreen);
	AddScreen(&m_CreditsScreen);

	// activate the event screen
	ActivateScreen(&m_EventScreen);

	// Initialize the callbacks
		// Message broadcast
		struct message_broadcast_t	mbt;
		trillianInitialize(mbt);
		mbt.callback = CAppletManager::BroadcastCallback;

		m_iBroadcastCallback =  g_plugin_send(APP_GUID, "messageBroadcastEnable",&mbt);
		
		// Connection: Change
		RegisterEvent("Connection: Change",CAppletManager::ConnectionChangeCallback);
		
		// Contact events
		RegisterEvent("Message: Chat Inbound (out of focus)",CAppletManager::ChatEventCallback);
		//RegisterEvent("Message: Chat Inbound (out of focus, unmuteable)",CAppletManager::ChatEventCallback);
		RegisterEvent("Message: Chat Inbound (in focus)",CAppletManager::ChatEventCallback);
		//RegisterEvent("Message: Chat Inbound (in focus, unmuteable)",CAppletManager::ChatEventCallback);
		RegisterEvent("Message: Chat Outbound (unmuteable)",CAppletManager::ChatEventCallback);
		
		RegisterEvent("Events: Screensaver Start",CAppletManager::ScreensaverEventCallback);
		RegisterEvent("Events: Screensaver End",CAppletManager::ScreensaverEventCallback);

		RegisterEvent("Contact List: Add",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Remove",CAppletManager::ContactEventCallback);

		RegisterEvent("Contact List: Contact Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Contact SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Contact SignOff",CAppletManager::ContactEventCallback);
		
		RegisterEvent("Contact List: MetaContact Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: MetaContact SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: MetaContact SignOff",CAppletManager::ContactEventCallback);
		
		RegisterEvent("Contact List: Subcontact Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Subcontact SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Subcontact SignOff",CAppletManager::ContactEventCallback);

		RegisterEvent("Contact List: Contact Bulk Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Contact Bulk SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Contact Bulk SignOff",CAppletManager::ContactEventCallback);
		
		RegisterEvent("Contact List: MetaContact Bulk Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: MetaContact Bulk SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: MetaContact Bulk SignOff",CAppletManager::ContactEventCallback);

		RegisterEvent("Contact List: Subcontact Bulk Status Change",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Subcontact Bulk SignOn",CAppletManager::ContactEventCallback);
		RegisterEvent("Contact List: Subcontact Bulk SignOff",CAppletManager::ContactEventCallback);

		// Window events
		RegisterEvent("Message: IM Window Creation",CAppletManager::WindowEventCallback);
		RegisterEvent("Message: IM Window Creation (unmuteable)",CAppletManager::WindowEventCallback);
		RegisterEvent("Message: IM Window Destruction",CAppletManager::WindowEventCallback);
		
	// load status bitmaps
	m_ahStatusBitmaps[0] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_OFFLINE),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[1] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_ONLINE),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[2] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_AWAY),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[3] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_NA),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[4] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_OCCUPIED),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[5] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_DND),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[6] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_INVISIBLE),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_ahStatusBitmaps[7] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STATUS_FFC),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	// Load event bitmaps
	m_ahEventBitmaps[0] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_MSG),
												IMAGE_BITMAP,6, 6, LR_MONOCHROME);
	m_ahEventBitmaps[1] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_CON),
												IMAGE_BITMAP,6, 6, LR_MONOCHROME);
	m_ahEventBitmaps[2] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_USER),
												IMAGE_BITMAP,6, 6, LR_MONOCHROME);
	m_ahEventBitmaps[3] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_INFO),
												IMAGE_BITMAP,6, 6, LR_MONOCHROME);

	m_ahLargeEventBitmaps[0] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_MSG_LARGE),
												IMAGE_BITMAP,8, 8, LR_MONOCHROME);
	m_ahLargeEventBitmaps[1] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_CON_LARGE),
												IMAGE_BITMAP,8, 8, LR_MONOCHROME);
	m_ahLargeEventBitmaps[2] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_USER_LARGE),
												IMAGE_BITMAP,8, 8, LR_MONOCHROME);
	m_ahLargeEventBitmaps[3] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_EVENT_INFO_LARGE),
												IMAGE_BITMAP,8, 8, LR_MONOCHROME);
	
	// start the update timer
	m_uiTimer = SetTimer(0,0,1000/10,CAppletManager::UpdateTimer);
	return true;
}

//************************************************************************
// Deinitializes the AppletManager
//************************************************************************
bool CAppletManager::Shutdown()
{
	if(!IsInitialized())
		return false;

	// delete status bitmaps
	for(int i=0;i<8;i++)
		DeleteObject(m_ahStatusBitmaps[i]);

	// delete event bitmaps
	for(int i=0;i<8;i++)
	{
		DeleteObject(m_ahLargeEventBitmaps[i]);	
		DeleteObject(m_ahEventBitmaps[i]);
	}


	// disable callbacks
	struct event_connect_t ect;
	trillianInitialize(ect);
		
		// Message broadcast
		struct message_broadcast_t	mbt;
		trillianInitialize(mbt);
		mbt.broadcast_id = m_iBroadcastCallback;

		g_plugin_send(APP_GUID, "messageBroadcastDisable",&mbt);
		// disconnect events
		vector<int>::iterator iter = m_vRegisteredEvents.begin();
		while(iter != m_vRegisteredEvents.end())
		{
			ect.event_id = *iter;
			g_plugin_send(APP_GUID,"eventsDisconnect", &ect); 
			iter++;
		}
		m_vRegisteredEvents.clear();
	
	// stop the update timer	
	KillTimer(0,m_uiTimer);

	// Shutdown the screens
	m_EventScreen.Shutdown();
	m_NotificationScreen.Shutdown();
	m_ContactlistScreen.Shutdown();
	m_ChatScreen.Shutdown();
	m_CreditsScreen.Shutdown();

	// Delete structures
	DeleteContactData();
	DeleteConnectionData();

	// deinitialize the outputmanager
	if(!CLCDOutputManager::Shutdown())
		return false;
	return true;
}

//************************************************************************
// Removes the contacts data structure
//************************************************************************
void CAppletManager::RemoveContactData(CContactData *pContact)
{
	vector<CContactData*>::iterator iter = m_vContactData.begin();
	while(iter != m_vContactData.end())
	{
		if((*iter) == pContact)
		{
			m_vContactData.erase(iter);
			m_ContactlistScreen.RegenerateList();
			break;
		}
		iter++;
	}
}

//************************************************************************
// Deletes the contact data structures
//************************************************************************
void CAppletManager::DeleteContactData()
{
	// delete the protocol information
	CContactData *pConData;
	for(int i=0;i<m_vContactData.size();i++)
	{
		pConData = m_vContactData[i];
		delete pConData;
	}
	m_vContactData.clear();
}

//************************************************************************
// Deletes the connection data structures
//************************************************************************
void CAppletManager::DeleteConnectionData()
{
	// delete the protocol information
	CConnectionData *pConData;
	for(int i=0;i<m_vConnectionData.size();i++)
	{
		pConData = m_vConnectionData[i];
		delete pConData;
	}
	m_vConnectionData.clear();
}

//************************************************************************
// Removes the specified connection data structure
//************************************************************************
void CAppletManager::RemoveConnection(CConnectionData *pConData)
{
	// delete the protocol information
	vector<CConnectionData*>::iterator iter = m_vConnectionData.begin();
	while(iter != m_vConnectionData.end())
	{
		if((*iter) == pConData)
		{
			m_vConnectionData.erase(iter);
			return;
		}
		iter++;
	}
}

//************************************************************************
// Requests a contactlist / contactdata update
//************************************************************************
void CAppletManager::RequestUpdate()
{
	m_bUpdateRequested = true;
}

//************************************************************************
// Regenerates the contact cache
//************************************************************************
void CAppletManager::RegenerateContactData()
{
	EnumerateSections();
	m_ContactlistScreen.RegenerateList();
}

//************************************************************************
// Enumerates all contacts
//************************************************************************
void CAppletManager::EnumerateContacts(int iConnection,tstring strMedium,tstring strSection)
{
	string medium = toNarrowString(strMedium);
	string section = toNarrowString(strSection);

	struct contactlist_enum_t	cet2;
	trillianInitialize(cet2);
	cet2.connection_id = -1;			// No specific CID requested.
	cet2.medium = (char*)medium.c_str();//(char*)toNarrowString(strMedium).c_str();
	cet2.callback = CAppletManager::ContactCallback;
	if(!strSection.empty())
		cet2.section = (char*)section.c_str();
	g_plugin_send(APP_GUID, "contactlistEnumerate", &cet2);
}

//************************************************************************
// Enumerates all sections
//************************************************************************
void CAppletManager::EnumerateSections()
{
	m_vSectionData.clear();

	struct contactlist_enum_t	cet2;
	trillianInitialize(cet2);
	cet2.callback = CAppletManager::EnumSectionCallback;
	g_plugin_send(APP_GUID, "contactlistSectionEnumerate", &cet2);
}

//************************************************************************
// Updates the AppletManager
//************************************************************************
bool CAppletManager::Update()
{
	if(!CLCDOutputManager::Update())
		return false;
	
	// If a contactdata / contactlist update was requested, do it now
	if(m_bUpdateRequested)
	{
		CAppletManager::GetInstance()->RegenerateContactData();
		m_bUpdateRequested = false;
	}

	return true;	
}

//************************************************************************
// the update timer's callback function
//************************************************************************
VOID CALLBACK CAppletManager::UpdateTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	CAppletManager::GetInstance()->Update();
}

//************************************************************************
// Called when the active screen has expired
//************************************************************************
void CAppletManager::OnScreenExpired(CLCDScreen *pScreen)
{
	// If the notification screen has expired, activate the last active screen

	if(pScreen == (CLCDScreen*)&m_NotificationScreen)
	{
		ActivateScreen((CLCDScreen*)m_pLastScreen);
		if(CConfig::GetBoolSetting(TRANSITIONS))
			m_pGfx->StartTransition();
	}
	
}

tstring CAppletManager::ConvertHTMLEntities(tstring strHtml)
{
	tstring::size_type End=0,Start=0;
	tstring strEntity = _T("");
	tstring strReplace = _T("");
	int i = 0;
	while(i < strHtml.length())
	{
		Start = strHtml.find(_T("&"),i);
		if(Start != string::npos)
		{
			End = strHtml.find(_T(";"),Start);
			if(End == string::npos)
				break;
			strEntity = strHtml.substr(Start+1,End-(Start+1));
			if(strEntity == _T("lt"))
				strReplace = _T("<");
			else if(strEntity == _T("gt"))
				strReplace = _T(">");
			else if(strEntity == _T("amp"))
				strReplace = _T("&");

			strHtml.replace(Start,(End-Start)+1,strReplace);
			i = Start+1;
		}
		else
			break;
	}

	return strHtml.c_str();
}

tstring CAppletManager::StripHTML(char *szHtml)
{
	string strHtml = szHtml;
	string strText = "";
	string::size_type End=0,Start=0;
	int i = 0;

	bool bTag = false;

	while(i < strHtml.length())
	{
		Start = strHtml.find("<",i);
		if(Start != string::npos)
		{
			if(Start - End > 0)
				strText += strHtml.substr(End+1,Start-End-1);

			End = strHtml.find(">",Start);
			if(End == string::npos)
				break;
			else
			{
				string strTag = strHtml.substr(Start+1,End-Start-1);
				if(strTag == "br" || strTag == "br \\" || strTag == "br\\")
					strText += "\n";
			}

			i = End;
		}
		else
			break;
	}
	strText += strHtml.substr(End==0?0:End+1);
	
	// Don't replace html entities if no html markup was found
	if(End == 0)
		return Utf8_Decode(strText.c_str());
	else
		return ConvertHTMLEntities(Utf8_Decode(strText.c_str()));
}

//************************************************************************
// Activates the EventScreen
//************************************************************************
void CAppletManager::ActivateEventScreen()
{
	m_ChatScreen.SetContact(NULL);
	ActivateScreen(&m_EventScreen);

	if(CConfig::GetBoolSetting(TRANSITIONS))
		m_pGfx->StartTransition();
}

//************************************************************************
// Activates the Chatscreen
//************************************************************************
void CAppletManager::ActivateChatScreen(CContactData *pContactData,int iConnectionID)
{
	m_ChatScreen.SetContact(pContactData,iConnectionID);
	ActivateScreen(&m_ChatScreen);

	if(CConfig::GetBoolSetting(TRANSITIONS))
		m_pGfx->StartTransition();
}

//************************************************************************
// Activates the Contactlistscreen
//************************************************************************
void CAppletManager::ActivateContactlistScreen()
{
	m_ContactlistScreen.ResetPosition();
	m_ChatScreen.SetContact(NULL);
	ActivateScreen(&m_ContactlistScreen);

	if(CConfig::GetBoolSetting(TRANSITIONS))
		m_pGfx->StartTransition();
}

//************************************************************************
// Activates the Creditsscreen
//************************************************************************
void CAppletManager::ActivateCreditsScreen()
{
	m_CreditsScreen.Reset();
	ActivateScreen(&m_CreditsScreen);
}

//************************************************************************
// sets the volumewheel hook
//************************************************************************
void CAppletManager::SetVolumeWheelHook()
{
	if(GetLCDConnection() && GetLCDConnection()->GetConnectionType() == TYPE_LOGITECH)
	{
		CLCDConnectionLogitech *pLCDConnection = (CLCDConnectionLogitech*)GetLCDConnection();
		if(pLCDConnection->GetConnectionState() == CONNECTED)
			pLCDConnection->SetVolumeWheelHook(CConfig::GetBoolSetting(HOOK_VOLUMEWHEEL));
	}
}

//************************************************************************
// Called when the connection state has changed
//************************************************************************
void CAppletManager::OnConnectionChanged(int iConnectionState)
{
	if(iConnectionState == CONNECTED)
		SetVolumeWheelHook();
}

//************************************************************************
// called when the configuration has changed
//************************************************************************
void CAppletManager::OnConfigChanged()
{
	m_pGfx->StartTransition(TRANSITION_MORPH);

	// Set the volumewheel hook
	SetVolumeWheelHook();
	// notify screens
	m_EventScreen.OnConfigChanged();
	m_NotificationScreen.OnConfigChanged();
	m_ChatScreen.OnConfigChanged();
	m_ContactlistScreen.OnConfigChanged();
}


//************************************************************************
// returns the bitmap for the specified event
//************************************************************************
HBITMAP CAppletManager::GetEventBitmap(EventType eType, bool bLarge)
{
	switch(eType)
	{
	case EVENT_MSG_RECEIVED:
	case EVENT_MSG_SENT:
		if(bLarge)
			return m_ahLargeEventBitmaps[0];
		else
			return m_ahEventBitmaps[0];
	case EVENT_PROTO_STATUS:
	case EVENT_PROTO_CONNECTED:
	case EVENT_PROTO_DISCONNECTED:
		if(bLarge)
			return m_ahLargeEventBitmaps[1];
		else
			return m_ahEventBitmaps[1];
	case EVENT_STATUS:
	case EVENT_SIGNED_ON:
	case EVENT_SIGNED_OFF:
		if(bLarge)
			return m_ahLargeEventBitmaps[2];
		else
			return m_ahEventBitmaps[2];
	default:
		if(bLarge)
			return m_ahLargeEventBitmaps[3];
		else
			return m_ahEventBitmaps[3];
	}
}

//************************************************************************
// returns the bitmap for the specified status
//************************************************************************
HBITMAP CAppletManager::GetStatusBitmap(tstring strStatus)
{
	strStatus = toLower(strStatus);

	if(strStatus == _T("offline"))
		return m_ahStatusBitmaps[0];
	if(strStatus ==  _T("online"))
		return m_ahStatusBitmaps[1];
	if(strStatus ==  _T("invisible"))
		return m_ahStatusBitmaps[6];
	if(strStatus ==  _T("occupied"))
		return m_ahStatusBitmaps[4];
	if(strStatus ==  _T("dnd"))
		return m_ahStatusBitmaps[5];
	if(strStatus ==  _T("not available"))
		return m_ahStatusBitmaps[4];
	if(strStatus ==  _T("free for chat"))
		return m_ahStatusBitmaps[7];
	
	return m_ahStatusBitmaps[2];
}

//************************************************************************
// returns a formatted timestamp string
//************************************************************************
tstring CAppletManager::GetFormattedTimestamp(tm *tm_time)
{
	time_t now;
	tm tm_now;
	time(&now);
	localtime_s(&tm_now,&now);
	setlocale( LC_ALL, "" );

	TCHAR buffer[128];
	if(tm_time->tm_mday != tm_now.tm_mday || tm_time->tm_mon != tm_now.tm_mon)
	{
		if(CConfig::GetBoolSetting(TIMESTAMP_SECONDS))
			_tcsftime(buffer,128,_T("[%x %H:%M:%S]"),tm_time);
		else
			_tcsftime(buffer,128,_T("[%x %H:%M]"),tm_time);
	}
	else
	{
		if(CConfig::GetBoolSetting(TIMESTAMP_SECONDS))
			_tcsftime(buffer,128,_T("[%H:%M:%S]"),tm_time);
		else
			_tcsftime(buffer,128,_T("[%H:%M]"),tm_time);
	}

	return toTstring(buffer);
}

//************************************************************************
// Handle the event
//************************************************************************
void CAppletManager::HandleEvent(CEvent *pEvent)
{
	TRACE(_T("<< Event: %i\n"),(int)pEvent->eType);
	
	// check if the event's timestamp needs to be set
	if(!pEvent->bTime)
	{
		time_t now;
		time(&now);
		localtime_s(&pEvent->Time,&now);
	}

	// activate notification screen if neccessary
	if(pEvent->bNotification)
		pEvent->bNotification = NeedsNotification(pEvent);
	
	pEvent->bLog = pEvent->bNotification;
	// if the applet is in foreground, skip notifications for the chatsession contact
	if(pEvent->pContact && GetLCDConnection()->IsForeground() && pEvent->pContact == m_ChatScreen.GetContact())
	{
		if(pEvent->eType == EVENT_STATUS  && CConfig::GetBoolSetting(NOTIFY_SKIP_STATUS))
			pEvent->bNotification = false;
		if( pEvent->eType == EVENT_SIGNED_ON && CConfig::GetBoolSetting(NOTIFY_SKIP_SIGNON))
			pEvent->bNotification = false;
		if(pEvent->eType == EVENT_SIGNED_OFF && CConfig::GetBoolSetting(NOTIFY_SKIP_SIGNOFF))
			pEvent->bNotification = false;
		if(pEvent->eType == EVENT_MSG_RECEIVED && CConfig::GetBoolSetting(NOTIFY_SKIP_MESSAGES))
			pEvent->bNotification = false;
	}

	m_EventScreen.OnEventReceived(pEvent);
	m_NotificationScreen.OnEventReceived(pEvent);
	m_ChatScreen.OnEventReceived(pEvent);
	m_ContactlistScreen.OnEventReceived(pEvent);

	if(pEvent->bNotification)
	{
		if(GetActiveScreen() != (CLCDScreen*)&m_NotificationScreen)
		{
			m_pLastScreen = GetActiveScreen();
		//	ASSERT(m_pLastScreen != NULL);
			m_NotificationScreen.SetAlert(true);
			m_NotificationScreen.SetExpiration(CConfig::GetIntSetting(NOTIFY_DURATION)*1000);
			ActivateScreen(&m_NotificationScreen);

			if(GetLCDConnection()->IsForeground() && CConfig::GetBoolSetting(TRANSITIONS))
				m_pGfx->StartTransition();
		}
	}
}

//************************************************************************
// checks wether a notification needs to be fired for this event
//************************************************************************
bool CAppletManager::NeedsNotification(CEvent *pEvent)
{
	// check for protocol filters
	if(pEvent->pContact != NULL)
	{
		if(!CConfig::GetProtocolNotificationFilter(pEvent->pContact->strMedium))
			return false;
	}

	switch(pEvent->eType)
	{
	case EVENT_PROTO_STATUS:
		if(!CConfig::GetBoolSetting(NOTIFY_PROTO_STATUS))
			return false;
		break;
	case EVENT_PROTO_CONNECTED:
		if(!CConfig::GetBoolSetting(NOTIFY_PROTO_SIGNON))
			return false;
		break;
	case EVENT_PROTO_DISCONNECTED:
		if(!CConfig::GetBoolSetting(NOTIFY_PROTO_SIGNOFF))
			return false;
		break;
	case EVENT_MSG_SENT:
		return false;
	case EVENT_MSG_RECEIVED:
		if(!CConfig::GetBoolSetting(NOTIFY_MESSAGES))
			return false;
		break;
	case EVENT_STATUS:
		if(!CConfig::GetBoolSetting(NOTIFY_STATUS))
			return false;
		break;
	case EVENT_SIGNED_ON:
		if(!CConfig::GetBoolSetting(NOTIFY_SIGNON))
			return false;
		break;
	case EVENT_SIGNED_OFF:
		if(!CConfig::GetBoolSetting(NOTIFY_SIGNOFF))
			return false;
		break;
	default:
		return false;
	}
	return true;
}

//************************************************************************
// Connection callback
//************************************************************************
int CAppletManager::ConnectionCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	if (!strcmp(event, "enum_start"))
	{
		CAppletManager::GetInstance()->DeleteConnectionData();
	}
	else if (!strcmp(event, "enum_end"))
	{

	}
	else if (!strcmp(event, "enum_add"))
	{
		struct connection_entry_t	*entry = (struct connection_entry_t *)data;
		CConnectionData *data = new CConnectionData();
		data->iConnection = entry->connection_id;
		
		data->strSection = _T("");
		if(entry->section != NULL)
			data->strSection = Utf8_Decode(entry->section);

		data->strSection = Utf8_Decode(entry->section);
		data->strID = Utf8_Decode(entry->name);
		data->strMedium = Utf8_Decode(entry->medium);
		data->strStatus = Utf8_Decode(entry->status);
		CAppletManager::GetInstance()->m_vConnectionData.push_back(data);
	}
	return 0;
}

//************************************************************************
// Section callback
//************************************************************************
int	CAppletManager::GetSectionCount()
{
	return m_vSectionData.size();
}

//************************************************************************
// Section callback
//************************************************************************
int CAppletManager::EnumSectionCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	if (!strcmp(event, "enum_start"))
	{
		
	}
	else if (!strcmp(event, "enum_end"))
	{

	}
	else if (!strcmp(event, "enum_add"))
	{
		struct contactlist_entry_t *entry = (struct contactlist_entry_t *)data;
		tstring strSection = _T("");
		if(entry->section)
			strSection = toTstring(entry->section);
		CAppletManager::GetInstance()->EnumerateContacts(-1,_T("all"),strSection);
		// cache the section
		CAppletManager::GetInstance()->m_vSectionData.push_back(strSection);
	}
	return 0;
}

//************************************************************************
// Contact callback
//************************************************************************
int CAppletManager::ContactCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	if (!strcmp(event, "enum_start"))
	{
	}
	else if (!strcmp(event, "enum_end"))
	{

	}
	else if (!strcmp(event, "enum_add"))
	{
		struct contactlist_entry_t	*entry = (struct contactlist_entry_t *)data;
		
		tstring strID = Utf8_Decode(entry->real_name);
		tstring strMedium = Utf8_Decode(entry->medium);
		tstring strSection = _T("");
		if(entry->section != NULL)
			strSection = Utf8_Decode(entry->section);

		CContactData *pConData = NULL;
		pConData = CAppletManager::GetInstance()->GetContactDataByID(strID,strMedium,strSection);
		// Contact isnt listed, create a new structure
		if(!pConData)
		{
			pConData = new CContactData();
			CAppletManager::GetInstance()->m_vContactData.push_back(pConData);
		}
		
		pConData->strSection = strSection;
		pConData->strStatus = Utf8_Decode(entry->status);
		pConData->iConnectionID = -1;
		pConData->strID = strID;
		pConData->strDisplayName = Utf8_Decode(entry->name);
		pConData->strMedium = strMedium;

		group_entry_t *groupentry = entry->group;
		if(entry->group)
		{
			tstring strGroup = Utf8_Decode(entry->group->name);
			while(groupentry && groupentry->child)
			{
				groupentry = groupentry->child;
				strGroup += _T("\\") + Utf8_Decode(groupentry->name);
			}
			pConData->strGroup = strGroup;
		}
		else
			pConData->strGroup = _T("");

		if(groupentry && groupentry->is_metacontact)
			pConData->bIsSubContact = true;
		else
			pConData->bIsSubContact = false;

	}
	return 0;
}

//************************************************************************
// returns the connection data for a specific medium
//************************************************************************
CConnectionData* CAppletManager::GetConnectionData(tstring strMedium,tstring strID)
{
	vector<CConnectionData*>::iterator iter = m_vConnectionData.begin();
	while(iter != m_vConnectionData.end())
	{
		if((*iter)->strMedium == strMedium && (*iter)->strID == strID)
			return (*iter);
		iter++;
	}
	return NULL;
}

CConnectionData* CAppletManager::GetConnectionData(tstring strMedium,int iConnection)
{
	vector<CConnectionData*>::iterator iter = m_vConnectionData.begin();
	while(iter != m_vConnectionData.end())
	{
		if((*iter)->strMedium == strMedium && (*iter)->iConnection == iConnection)
			return (*iter);
		iter++;
	}
	return NULL;
}

//************************************************************************
// returns the contact data for a specific contact
//************************************************************************
CContactData* CAppletManager::GetContactDataByWindowID(int iWindowID)
{
	vector<CContactData*>::iterator iter = m_vContactData.begin();
	while(iter != m_vContactData.end())
	{
		if((*iter)->iWindowID == iWindowID)
			return (*iter);
		iter++;
	}
	return NULL;
}

CContactData* CAppletManager::GetContactDataByID(tstring strID,tstring strMedium,tstring strSection)
{
	vector<CContactData*>::iterator iter = m_vContactData.begin();
	while(iter != m_vContactData.end())
	{
		if((*iter)->strID == strID &&
			toLower((*iter)->strMedium) == toLower(strMedium) &&
			toLower((*iter)->strSection) == toLower(strSection))
			return (*iter);
		iter++;
	}
	return NULL;
}

CContactData* CAppletManager::GetMetacontactBySubcontact(CContactData *pContactData)
{
	if(!pContactData->bIsSubContact)
		return NULL;
	
	tstring strParse = pContactData->strGroup;
	
	tstring::size_type pos = strParse.rfind('\\');
	if(pos ==  tstring::npos )
		pos = 0;
	else
		pos+=1;

	strParse = strParse.substr(pos);
	return GetContactDataByID(strParse,_T("metacontact"),pContactData->strSection);
}

// creates a message event
void CAppletManager::CreateMessageEvent(CContactData *pContact,int iConnectionID, tstring strMessage, bool bSent)
{
	CEvent event;
	
	if(bSent)
	{
		event.eType = EVENT_MSG_SENT;
	}
	else
	{
		event.eType = EVENT_MSG_RECEIVED;
		event.bNotification = true;
	}
	tstring strDisplayName = pContact->strDisplayName;
	if(CConfig::GetBoolSetting(NOTIFY_NICKCUTOFF) && strDisplayName.length() > CConfig::GetIntSetting(NOTIFY_NICKCUTOFF_OFFSET))
		strDisplayName = strDisplayName.erase(CConfig::GetIntSetting(NOTIFY_NICKCUTOFF_OFFSET)) + _T("...");

	event.pContact = pContact;
	event.iConnectionID = iConnectionID;

	event.strValue = strMessage;
	event.strDescription = strDisplayName + _T(": ") + strMessage;
	
	if(CConfig::GetBoolSetting(NOTIFY_SHOWPROTO))
		event.strDescription = _T("(") + pContact->strMedium + _T(") ") + event.strDescription;

	event.strSection = pContact->strSection;

	event.bTime = true;
	time_t now;
	time(&now);
	localtime_s(&event.Time,&now);

	// add the message to the history
	SMessage historymsg;
	historymsg.bIsMe = (event.eType == EVENT_MSG_SENT);
	historymsg.Time = event.Time;
	historymsg.strMessage = event.strValue;
	historymsg.iConnectionID = event.iConnectionID;
	pContact->LMessages.push_back(historymsg);
	
	// Limit the size to the session logsize
	if(pContact->LMessages.size() > CConfig::GetIntSetting(SESSION_LOGSIZE))
		pContact->LMessages.pop_front();

	CAppletManager::GetInstance()->HandleEvent(&event);
}

//************************************************************************
// Chat callback: Handles incoming/outgoing chat messages/events
//************************************************************************
int CAppletManager::ChatEventCallback(int windowID, char *subwindow, char *event, void *data, void *userData)
{
	event_variables_t *eventVariablesPtr = (event_variables_t*)data;

	tstring strAddress = _T("");
	tstring strMedium = _T("");
	tstring strDisplayName = _T("");
	tstring strLocation = _T("");
	int iConnection = 0;
	char *szMessage = NULL;

	// Loop through and store the event variables
	for (; eventVariablesPtr != NULL; eventVariablesPtr = eventVariablesPtr->next_evt)
	{
		// Check for all the strings
		if (!_stricmp(eventVariablesPtr->variable_type, "string"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "medium"))
				strMedium = Utf8_Decode((char*)eventVariablesPtr->variable_data);	
			else if(!_stricmp(eventVariablesPtr->variable_name, "address"))
				strAddress = Utf8_Decode((char*)eventVariablesPtr->variable_data);	
			else if(!_stricmp(eventVariablesPtr->variable_name, "message"))
				szMessage = (char*)eventVariablesPtr->variable_data;
			else if(!_stricmp(eventVariablesPtr->variable_name, "displayname"))
				strDisplayName = Utf8_Decode((char*)eventVariablesPtr->variable_data);	
			else if(!_stricmp(eventVariablesPtr->variable_name, "location"))
				strLocation = Utf8_Decode((char*)eventVariablesPtr->variable_data);	
		}
		else if(!_stricmp(eventVariablesPtr->variable_type, "integer"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "cid"))
				iConnection = *((int*)eventVariablesPtr->variable_data);
		}
	}

	CConnectionData *pConnectionData = CAppletManager::GetInstance()->GetConnectionData(strMedium,iConnection);
	if(!pConnectionData)
		return 0;

	CContactData *pConData  = CAppletManager::GetInstance()->GetContactDataByID(strAddress,strMedium,pConnectionData->strSection);
	if(!pConData)
		return 0;

	bool bSent = true;
	if(strstr(event,"Inbound"))
		bSent = false;
	
	tstring strMessage = _T("");
	if(strDisplayName != strAddress)
		strMessage += strDisplayName + _T(": ");
	strMessage += StripHTML(szMessage);

	CAppletManager::CreateMessageEvent(pConData,iConnection,strMessage,bSent);
	
	return 0;
}

//************************************************************************
// Broadcast callback: Handles incoming/outgoing messages
//************************************************************************
int CAppletManager::BroadcastCallback(int windowID, char *subwindow, char *szEvent, void *data, void *userData)
{
	struct message_t	*msg = (struct message_t *)data;

	bool bSent = false;

	
	CContactData *pConData = NULL;
	
	if(!strcmp(msg->type, "incoming_privateMessage") || !(msg->type, "incoming_groupMessage"))
	{
		tstring strSection = _T("");
		if(msg->section)
			strSection = Utf8_Decode(msg->section);
		// section string fix
		if(strSection == _T("My Contacts"))
			strSection = _T("");

		pConData = CAppletManager::GetInstance()->GetContactDataByID(Utf8_Decode(msg->name),Utf8_Decode(msg->medium),strSection);
		// If the contact is not found, it might have been just added to the list => refresh data
		if(!pConData)
		{
			CAppletManager::GetInstance()->RequestUpdate();
			return 0;
		}
	}
	else if(!strcmp(msg->type, "outgoing_privateMessage") || !(msg->type, "outgoing_groupMessage"))
	{
		bSent = true;

		pConData = CAppletManager::GetInstance()->GetContactDataByWindowID(msg->window_id);
		if(pConData == NULL)
			return 0;
	}
	else
		return 0;

	CAppletManager::CreateMessageEvent(pConData,msg->connection_id,StripHTML(msg->text),bSent);

	return 0;
}

//************************************************************************
// connection event callback: Handles connects/disconnects status changes
//************************************************************************
int CAppletManager::ConnectionChangeCallback(int windowID, char *subwindow, char *szEvent, void *data, void *userData)
{
	event_variables_t *eventVariablesPtr = (event_variables_t*)data;

	tstring strMedium = _T("");
	tstring strName = _T("");
	int	iConnection;
	tstring strStatus;
	tstring strSection = _T("");

	// Loop through and store the event variables
	for (; eventVariablesPtr != NULL; eventVariablesPtr = eventVariablesPtr->next_evt)
	{
		// Check for all the strings
		if (!_stricmp(eventVariablesPtr->variable_type, "string"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "medium"))
				strMedium = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "name"))
				strName = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "status"))
				strStatus = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "section"))
				strSection = Utf8_Decode((char*)eventVariablesPtr->variable_data);
		}
		else if(!_stricmp(eventVariablesPtr->variable_type, "integer"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "cid"))
				iConnection = *((int*)eventVariablesPtr->variable_data);
		}
	}
	// if no status is found, set it to offline
	if(strStatus.empty())
		strStatus = _T("offline");
	
	// if no medium string is found, skip this event
	if(strMedium.empty())
		return 0;

	CEvent event;

	CConnectionData *pConData = CAppletManager::GetInstance()->GetConnectionData(strMedium,iConnection);
	// Connection just came online so it needs to be added
	if(!pConData)
	{	
		pConData = new CConnectionData();
		pConData->strStatus = _T("offline");
		pConData->iConnection = iConnection;
		pConData->strMedium = strMedium;
		pConData->strID = strName;
		CAppletManager::GetInstance()->m_vConnectionData.push_back(pConData);
		CAppletManager::GetInstance()->EnumerateContacts(iConnection,strMedium,strSection);
	}

	if(pConData->strStatus != strStatus)
	{
		if(toLower(pConData->strStatus) == _T("offline") && toLower(strStatus) != _T("offline"))
		{
			event.eType = EVENT_PROTO_CONNECTED;
			event.strDescription = _T("(") + strMedium +  _T(") You are now online");
			pConData->strStatus = strStatus;
			pConData->lTimeStamp = GetTickCount();
		}
		else if(toLower(pConData->strStatus) != _T("offline") && toLower(strStatus) == _T("offline"))
		{
			event.eType = EVENT_PROTO_DISCONNECTED;
			event.strDescription = _T("(") + strMedium +  _T(") You are now offline");
			CAppletManager::GetInstance()->RemoveConnection(pConData);
			delete pConData;
			CAppletManager::GetInstance()->EnumerateContacts(iConnection,strMedium,strSection);
		}
		else
		{
			event.eType = EVENT_PROTO_STATUS;
			if(toLower(strStatus) == _T("mail"))
				event.strDescription = _T("(") + strMedium +  _T(") You have new mail(s)");
			else
				event.strDescription = _T("(") + strMedium +  _T(") You are now ") + strStatus;
			pConData->strStatus = strStatus;
		}

		event.strValue = strStatus;
		event.bNotification = true;
		event.strSection = strSection;

		CAppletManager::GetInstance()->HandleEvent(&event);

	}
	else
		return 0;
	
	return 0;
}

//************************************************************************
// contact event callback: Handles signon/signoff and status changes
//************************************************************************
int CAppletManager::ContactEventCallback(int windowID, char *subwindow, char *szEvent, void *data, void *userData)
{
	event_variables_t *eventVariablesPtr = (event_variables_t*)data;

	tstring strDisplayName = _T("");
	tstring strStatus = _T("");
	tstring strMedium = _T("");
	tstring strAddress = _T("");
	tstring strSection = _T("");

	// Loop through and store the event variables
	for (; eventVariablesPtr != NULL; eventVariablesPtr = eventVariablesPtr->next_evt)
	{
		// Check for all the strings
		if (!_stricmp(eventVariablesPtr->variable_type, "string"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "medium"))
				strMedium = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "displayname"))
				strDisplayName = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "address"))
				strAddress = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "status"))
				strStatus = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "section"))
				strSection = Utf8_Decode((char*)eventVariablesPtr->variable_data);
		}
	}
	// section string fix
	if(strSection == _T("My Contacts"))
		strSection = _T("");

	if(strStatus.empty())
		strStatus = _T("offline");

	strDisplayName = ConvertHTMLEntities(strDisplayName);
	if(CConfig::GetBoolSetting(NOTIFY_NICKCUTOFF) && strDisplayName.length() > CConfig::GetIntSetting(NOTIFY_NICKCUTOFF_OFFSET))
		strDisplayName = strDisplayName.erase(CConfig::GetIntSetting(NOTIFY_NICKCUTOFF_OFFSET)) + _T("...");

	CEvent event;
	CContactData *pConData = NULL;

	// On contact added reenumerate all of the mediums contacts to get its connection_id :|
	if(!stricmp(szEvent,"Contact List: Add"))
	{
		CAppletManager::GetInstance()->RequestUpdate();
		return 0;
	}
	
	pConData = CAppletManager::GetInstance()->GetContactDataByID(strAddress,strMedium,strSection);
	// If the contact is not found, it might have been just added to the list => refresh data
	if(!pConData)
	{
		CAppletManager::GetInstance()->RequestUpdate();
		return 0;
	}
	
	// On contact removed, remove the data from the list
	if(!stricmp(szEvent,"Contact List: Remove"))
	{
		CAppletManager::GetInstance()->RemoveContactData(pConData);
		return 0;
	}
	

	event.pContact = pConData;
	event.strValue = strStatus;
	event.strValue2 = pConData->strStatus;
	event.strSection = pConData->strSection;

	pConData->strStatus = strStatus;
	
	if(strstr(szEvent,"Subcontact"))
	{
		CContactData *pMetaData = CAppletManager::GetInstance()->GetMetacontactBySubcontact(pConData);
		if(!pMetaData)
			return 0;
		strDisplayName = pMetaData->strDisplayName;
	}

	if(strstr(szEvent,"SignOn") || strstr(szEvent,"signon"))
	{
		event.eType = EVENT_SIGNED_ON;
		event.strDescription = strDisplayName + _T(" signed on (") + strStatus + _T(")");
	}
	else if(strstr(szEvent,"SignOff")  || strstr(szEvent,"signoff"))
	{
		event.eType = EVENT_SIGNED_OFF;
		event.strDescription = strDisplayName + _T(" signed off");
	}
	else
	{
		event.eType = EVENT_STATUS;
		event.strDescription = strDisplayName + _T(" is now ") + strStatus;
	}
	
	if(CConfig::GetBoolSetting(NOTIFY_SHOWPROTO))
		event.strDescription = _T("(") + pConData->strMedium + _T(") ") + event.strDescription;

	if(strstr(szEvent,"Subcontact") || strstr(szEvent,"bulk") || strstr(szEvent,"Bulk"))
		event.bNotification = false;
	else
		event.bNotification = true;

	CAppletManager::GetInstance()->HandleEvent(&event);

	return 0;
}

//************************************************************************
// window event callback: Handles window opened/closed events
//************************************************************************
int CAppletManager::WindowEventCallback(int windowID, char *subwindow, char *szEvent, void *data, void *userData)
{
	if(stricmp(szEvent,"Message: IM Window Creation") &&
		stricmp(szEvent,"Message: IM Window Creation (unmuteable)") &&
		stricmp(szEvent,"Message: IM Window Destruction"))
		return 0;
	
	tstring strDisplayName = _T("");
	int	iConnection = -1;
	int iWindowID = -1;
	tstring strMedium = _T("");
	tstring strAddress = _T("");
	
	event_variables_t *eventVariablesPtr = (event_variables_t*)data;
	// Loop through and store the event variables
	for (; eventVariablesPtr != NULL; eventVariablesPtr = eventVariablesPtr->next_evt)
	{
		// Check for all the strings
		if (!_stricmp(eventVariablesPtr->variable_type, "string"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "medium"))
				strMedium = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "displayname"))
				strDisplayName = Utf8_Decode((char*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "address"))
				strAddress = Utf8_Decode((char*)eventVariablesPtr->variable_data);
		}
		else if(!_stricmp(eventVariablesPtr->variable_type, "integer"))
		{
			if(!_stricmp(eventVariablesPtr->variable_name, "cid"))
				iConnection = *((int*)eventVariablesPtr->variable_data);
			else if(!_stricmp(eventVariablesPtr->variable_name, "window_id"))
				iWindowID = *((int*)eventVariablesPtr->variable_data);
		}
	}

	// fetch the contacts section from the connection id
	CConnectionData *pConnectionData = CAppletManager::GetInstance()->GetConnectionData(strMedium,iConnection);
	if(!pConnectionData)
		return 0;

	strDisplayName = ConvertHTMLEntities(strDisplayName);
	
	CContactData *pConData = CAppletManager::GetInstance()->GetContactDataByID(strAddress,strMedium,pConnectionData->strSection);
	// If the contact is not found, it might have been just added to the list => refresh data
	if(!pConData)
	{
		if(strMedium == _T("IRC") && strDisplayName[0] == '#')
		{
			// Create an IRC channel contact
			pConData  = new CContactData();
			CAppletManager::GetInstance()->m_vContactData.push_back(pConData);

			pConData->strSection = pConnectionData->strSection;
			pConData->strStatus = _T("offline");
			pConData->strID = strAddress;
			pConData->strDisplayName = strDisplayName;
			pConData->strMedium = strMedium;

			pConData->strGroup = _T("IRC Channels");
			
			pConData->bIsSubContact = false;
			pConData->iConnectionID = iConnection;

			// regenerate the contactlist
			CAppletManager::GetInstance()->m_ContactlistScreen.RegenerateList();

			// fire an event
			pConData->strStatus = _T("Online");

			CEvent event;
			event.bNotification = true;
			event.pContact = pConData;
			event.eType = EVENT_SIGNED_ON;
			event.strValue = _T("online");
			event.strValue2 = _T("offline");
			event.strDescription = _T("(IRC) joined ") + strDisplayName;
			event.strSection = pConnectionData->strSection;

			CAppletManager::GetInstance()->HandleEvent(&event);			
		}
		else
		{
			CAppletManager::GetInstance()->RequestUpdate();
			return 0;
		}
	}

	if(!stricmp(szEvent,"Message: IM Window Destruction"))
	{
		pConData->iWindowID = -1;
		if(strMedium == _T("IRC") && strDisplayName[0] == '#')
		{
			// fire an event
			CEvent event;
			event.pContact = pConData;
			event.eType = EVENT_SIGNED_OFF;
			event.strValue = _T("online");
			event.strValue2 = _T("offline");
			event.strDescription = _T("(IRC) left ") + strDisplayName;
			event.strSection = pConnectionData->strSection;

			CAppletManager::GetInstance()->HandleEvent(&event);

			// remove the irc channel contact
			CAppletManager::GetInstance()->RemoveContactData(pConData);
		}
	}
	else
		pConData->iWindowID = iWindowID;
	
	return 0;
}

int CAppletManager::ScreensaverEventCallback(int windowID, char *subwindow, char *szEvent, void *data, void *userData)
{
	if(!CConfig::GetBoolSetting(CONTROL_BACKLIGHTS) ||
		!CAppletManager::GetInstance()->GetLCDConnection() ||
		CAppletManager::GetInstance()->GetLCDConnection()->GetConnectionType() != TYPE_LOGITECH)
		return 0;

	CLCDConnectionLogitech *pLCDConnection = (CLCDConnectionLogitech*)CAppletManager::GetInstance()->GetLCDConnection();
	
	SG15LightStatus *pG15LightStatus = &CAppletManager::GetInstance()->m_G15LightStatus;
	// Screensaver starts
	if(!stricmp(szEvent,"Events: Screensaver Start"))
	{
		*pG15LightStatus = pLCDConnection->GetLightStatus();
		pLCDConnection->SetLCDBacklight(LCD_OFF);
		pLCDConnection->SetKBDBacklight(KBD_OFF);
		pLCDConnection->SetMKeyLight(0,0,0,0);
	}
	// Screensaver ends
	// only restore states that haven't changed
	else
	{
		SG15LightStatus currentStatus = pLCDConnection->GetLightStatus();
		if(currentStatus.eLCDBrightness == LCD_OFF)
			pLCDConnection->SetLCDBacklight(pG15LightStatus->eLCDBrightness);
		if(currentStatus.eKBDBrightness == KBD_OFF)
			pLCDConnection->SetKBDBacklight(pG15LightStatus->eKBDBrightness);
		if(!currentStatus.bMRKey && !currentStatus.bMKey[0] && !currentStatus.bMKey[1]
			&& !currentStatus.bMKey[2])
			pLCDConnection->SetMKeyLight(pG15LightStatus->bMKey[0],pG15LightStatus->bMKey[1],pG15LightStatus->bMKey[2],pG15LightStatus->bMRKey);
	}

	return 0;
}