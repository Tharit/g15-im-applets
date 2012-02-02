#include "stdafx.h"
#include "CConfig.h"
#include "CNotificationScreen.h"
#include "CAppletManager.h"

//************************************************************************
// Constructor
//************************************************************************
CNotificationScreen::CNotificationScreen()
{
	m_pEntry = NULL;
}

//************************************************************************
// Destructor
//************************************************************************
CNotificationScreen::~CNotificationScreen()
{
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CNotificationScreen::Initialize()
{
	if(!CScreen::Initialize())
		return false;

	m_EventText.Initialize();
	m_MessageText.Initialize();
	m_TitleText.Initialize();
	m_Scrollbar.Initialize();
	m_Timestamp.Initialize();

	m_Timestamp.SetAlignment(DT_RIGHT);

	m_TitleText.SetText(_T("Trillian"));
	m_TitleText.SetAlignment(DT_LEFT);
	
	m_EventText.SetAlignment(DT_CENTER);
	m_EventText.SetWordWrap(TRUE);

	m_MessageText.SetScrollbar(&m_Scrollbar);

	UpdateObjects();

	AddObject(&m_Scrollbar);
	AddObject(&m_EventText);
	AddObject(&m_MessageText);
	AddObject(&m_TitleText);
	AddObject(&m_Timestamp);

	SetButtonBitmap(0,IDB_UP);
	SetButtonBitmap(1,IDB_DOWN);

	return true;
}

//************************************************************************
// Shutdown the screen
//************************************************************************
bool CNotificationScreen::Shutdown()
{
	CNotificationEntry *pEntry = NULL;
	while(!m_LNotifications.empty())
	{
		pEntry = *(m_LNotifications.begin());
		m_LNotifications.pop_front();
		delete pEntry;
	}

	if(!CScreen::Shutdown())
		return false;
	
	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CNotificationScreen::Update()
{
	if(!CScreen::Update())
		return false;

	return true;
}

//************************************************************************
// Draws the screen
//************************************************************************
bool CNotificationScreen::Draw(CLCDGfx *pGfx)
{
	if(!CScreen::Draw(pGfx))
		return false;
	
	if(CConfig::GetIntSetting(NOTIFY_TITLE) != NOTIFY_TITLE_HIDE)
	{
		int iTitleHeight = CConfig::GetFontHeight(FONT_TITLE);
		pGfx->DrawLine(0,iTitleHeight < 7?7:iTitleHeight,GetWidth(),iTitleHeight < 7?7:iTitleHeight);
		if(m_pEntry)
		{
			int iOffset = (iTitleHeight-(iTitleHeight>8?8:6))/2;
			HBITMAP hBitmap = CAppletManager::GetInstance()->GetEventBitmap(m_pEntry->eType,iTitleHeight>8);
			pGfx->DrawBitmap(0,iOffset,iTitleHeight>8?8:6,iTitleHeight>8?8:6,hBitmap);
		}
	}
	return true;
}

//************************************************************************
// displays the specified notification
//************************************************************************
void CNotificationScreen::DisplayNotification(CNotificationEntry *pEntry)
{
	if(m_pEntry)
		delete m_pEntry;
	
	m_pEntry = pEntry;

	
	tstring strTime = CAppletManager::GetFormattedTimestamp(&pEntry->Time);
	if(CConfig::GetIntSetting(NOTIFY_TITLE) == NOTIFY_TITLE_NAME)
	{
		if(CConfig::GetBoolSetting(NOTIFY_SECTION))
			m_TitleText.SetText(_T(" Trillian: ") + (pEntry->strSection.empty()?_T("My Contacts"):pEntry->strSection));
		else
			m_TitleText.SetText(_T(" Trillian"));
	}

	if(CConfig::GetBoolSetting(NOTIFY_TIMESTAMPS))
		m_Timestamp.SetText(strTime);
	else
		m_Timestamp.SetText(_T(""));

	if(pEntry->pContact)
		SetButtonBitmap(3,IDB_CHAT);
	else
		SetButtonBitmap(3,NULL);

	if(pEntry->bMessage)
	{
		SetButtonBitmap(0,IDB_UP);
		SetButtonBitmap(1,IDB_DOWN);

		m_Scrollbar.Show(1);
		m_MessageText.Show(1);
		m_EventText.Show(0);
		m_MessageText.SetText(pEntry->strText.c_str());
	}
	else
	{
		SetButtonBitmap(0,NULL);
		SetButtonBitmap(1,NULL);

		m_Scrollbar.Show(0);
		m_MessageText.Show(0);
		m_EventText.Show(1);

		m_EventText.SetText(pEntry->strText.c_str());
	}
}

//************************************************************************
// updates all objects
//************************************************************************
void CNotificationScreen::UpdateObjects()
{
	int iHeight = GetHeight() - (CConfig::GetBoolSetting(SHOW_LABELS)?6:0);
	int iOrigin = 0;

	if(CConfig::GetIntSetting(NOTIFY_TITLE) == NOTIFY_TITLE_HIDE)
	{
		m_TitleText.Show(false);
		m_Timestamp.Show(false);
	}	
	else
	{
		iOrigin = CConfig::GetFontHeight(FONT_TITLE);
		iHeight -= iOrigin;
		m_TitleText.Show(true);
		m_Timestamp.Show(true);
	}

	m_Timestamp.SetSize((GetWidth()/3),CConfig::GetFontHeight(FONT_TITLE));
	m_Timestamp.SetOrigin((GetWidth()/3)*2,0);

	m_TitleText.SetSize(((GetWidth()/3)*2) -5,CConfig::GetFontHeight(FONT_TITLE));
	m_TitleText.SetOrigin(7,0);

	m_MessageText.SetFont(CConfig::GetFont(FONT_NOTIFICATION));
	m_TitleText.SetFont(CConfig::GetFont(FONT_TITLE));
	m_EventText.SetFont(CConfig::GetFont(FONT_NOTIFICATION));
	m_Timestamp.SetFont(CConfig::GetFont(FONT_TITLE));
	
	m_EventText.SetOrigin(0,iOrigin + 1);
	m_EventText.SetSize(GetWidth()-4, iHeight);

	m_MessageText.SetOrigin(0, iOrigin+1);
	m_MessageText.SetSize(GetWidth()-4, iHeight);

	m_Scrollbar.SetOrigin(GetWidth()-4,iOrigin+1);
	m_Scrollbar.SetSize(4,iHeight);
}

//************************************************************************
// Called when the configuration has changed
//************************************************************************
void CNotificationScreen::OnConfigChanged()
{
	CScreen::OnConfigChanged();

	UpdateObjects();
}

//************************************************************************
// Called when an event is received
//************************************************************************
void CNotificationScreen::OnEventReceived(CEvent *pEvent)
{
	// check wether this events needs notification
	if(!pEvent->bNotification)
		return;

	CNotificationEntry *pEntry = new CNotificationEntry();
	if(pEvent->eType == EVENT_MSG_RECEIVED)
	{
		pEntry->bMessage = true;
		//tstring strUser = toTstring((char*)CallService(MS_CLIST_GETCONTACTDISPLAYNAME, (WPARAM)pEvent->hContact, NULL));

		//if(CConfig::GetIntSetting(NOTIFY_TITLE) == NOTIFY_TITLE_INFO)
		//	pEntry->strText = pEvent->strValue;
		//else
		pEntry->strText = pEvent->pContact->strDisplayName + _T(": ") + pEvent->strValue;
	}
	else
	{
		pEntry->bMessage = false;
		pEntry->strText = pEvent->strDescription;
	}
	pEntry->strSection = pEvent->strSection;
	pEntry->Time = pEvent->Time;
	pEntry->pContact = pEvent->pContact;
	pEntry->iConnectionID = pEvent->iConnectionID;
	pEntry->eType = pEvent->eType;

	if(m_pEntry)
	{
		m_LNotifications.push_back(pEntry);
		SetButtonBitmap(2,IDB_NEXT);
	}
	else
	{
		DisplayNotification(pEntry);
		SetButtonBitmap(2,NULL);
	}
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CNotificationScreen::OnLCDButtonDown(int iButton)
{
	CScreen::OnLCDButtonDown(iButton);	

	if(m_MessageText.IsVisible() &&
		( iButton <= 1 || iButton >=4))
	{
		if(iButton== 1 || iButton == 5)
			m_MessageText.ScrollDown();
		else
			m_MessageText.ScrollUp();

		SetExpiration(CConfig::GetIntSetting(NOTIFY_DURATION)*1000);
	}
	else if(iButton == 2 && m_LNotifications.size() >= 1)
	{
		CNotificationEntry *pEntry = *(m_LNotifications.begin());
		m_LNotifications.pop_front();
		
		if(m_LNotifications.size() >= 1)
			SetButtonBitmap(2,IDB_NEXT);
		else
			SetButtonBitmap(2,NULL);

		DisplayNotification(pEntry);
		SetExpiration(CConfig::GetIntSetting(NOTIFY_DURATION)*1000);
	}
	else if(iButton == 3 && m_pEntry && m_pEntry->pContact)
	{
		SetExpiration(0);

		CLCDConnection *pLCDCon =  CAppletManager::GetInstance()->GetLCDConnection();
		pLCDCon->SetAsForeground(1);
		pLCDCon->SetAsForeground(0);
		CAppletManager::GetInstance()->ActivateChatScreen(m_pEntry->pContact,m_pEntry->iConnectionID);
	}
	else
		SetExpiration(0);
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CNotificationScreen::OnLCDButtonRepeated(int iButton)
{
	CScreen::OnLCDButtonDown(iButton);	
	if(m_MessageText.IsVisible() &&
		iButton <= 1)
	{
		if(iButton== 1)
			m_MessageText.ScrollDown();
		else
			m_MessageText.ScrollUp();

		SetExpiration(CConfig::GetIntSetting(NOTIFY_DURATION)*1000);
	}
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CNotificationScreen::OnLCDButtonUp(int iButton)
{
	
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CNotificationScreen::OnActivation()
{

}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CNotificationScreen::OnDeactivation()
{
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CNotificationScreen::OnExpiration()
{
	// clear the cached events 
	CNotificationEntry *pEntry = NULL;
	while(!m_LNotifications.empty())
	{
		pEntry = *(m_LNotifications.begin());
		m_LNotifications.pop_front();
		delete pEntry;
	}
	// reset the object's content
	m_EventText.SetText(_T(""));
	m_MessageText.SetText(_T(""));

	m_pEntry = NULL;
}