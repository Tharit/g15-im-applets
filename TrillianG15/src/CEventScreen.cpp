#include "stdafx.h"
#include "CConfig.h"
#include "CEventScreen.h"

#include "CAppletManager.h"

//************************************************************************
// Constructor
//************************************************************************
CEventScreen::CEventScreen()
{
}

//************************************************************************
// Destructor
//************************************************************************
CEventScreen::~CEventScreen()
{
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CEventScreen::Initialize()
{
	if(!CScreen::Initialize())
		return false;

	m_EventLog.Initialize();
	m_EventLog.SetOrigin(0,0);
	m_EventLog.SetSize(GetWidth()-5,GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?6:0));
	m_EventLog.SetFont(CConfig::GetFont(FONT_LOG));
	m_EventLog.SetAlignment(false);
	AddObject(&m_EventLog);

	m_Scrollbar.Initialize();
	m_Scrollbar.SetOrigin(GetWidth()-4,0);
	m_Scrollbar.SetSize(4,GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?5:0));
	m_EventLog.SetScrollbar(&m_Scrollbar);

	AddObject(&m_Scrollbar);

	SetButtonBitmap(0,IDB_UP);
	SetButtonBitmap(1,IDB_DOWN);
	SetButtonBitmap(2,IDB_CLIST);
	SetButtonBitmap(3,NULL);

	return true;
}

//************************************************************************
// Shutdown the screen
//************************************************************************
bool CEventScreen::Shutdown()
{
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CEventScreen::Update()
{
	if(!CScreen::Update())
		return false;

	return true;
}

//************************************************************************
// Draws the screen
//************************************************************************
bool CEventScreen::Draw(CLCDGfx *pGfx)
{
	if(!CScreen::Draw(pGfx))
		return false;

	return true;
}

//************************************************************************
// Called when the configuration has changed
//************************************************************************
void CEventScreen::OnConfigChanged()
{
	CScreen::OnConfigChanged();

	m_EventLog.SetFont(CConfig::GetFont(FONT_LOG));
	m_EventLog.SetSize(GetWidth()-5,GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?6:0));

	// Update all timestamps
	list<CListEntry<CEventLogEntry*>*>::iterator iter =  m_EventLog.begin();
	CListItem<CEventLogEntry*> *pItem = NULL;
	while(iter != m_EventLog.end())
	{
		pItem = static_cast<CListItem<CEventLogEntry*>*>(*iter);
		pItem->GetItemData()->strTimestamp = CAppletManager::GetFormattedTimestamp(&pItem->GetItemData()->Time) + _T(" ");
		iter++;
	}

	m_Scrollbar.SetSize(4,GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?5:0));
}

//************************************************************************
// Called when an event is received
//************************************************************************
void CEventScreen::OnEventReceived(CEvent *pEvent)
{
	// check if the event needs to be listed
	if(!pEvent->bLog)
		return;

	// create a list entry structure
	CEventLogEntry *pEntry = new CEventLogEntry();
	//pEntry->hContact = pEvent->hContact;
	pEntry->pContact = pEvent->pContact;
	pEntry->iConnectionID = pEvent->iConnectionID;
	pEntry->eType = pEvent->eType;

	pEntry->Time = pEvent->Time;
	pEntry->strTimestamp = CAppletManager::GetFormattedTimestamp(&pEvent->Time) + _T(" ");
	pEntry->strValue = pEvent->strDescription;
	
	// add it to the list and scroll to it
	m_EventLog.AddItem(pEntry);

	UpdateChatButton();
}

//************************************************************************
// Updates the 4th softkey label
//************************************************************************
void CEventScreen::UpdateChatButton()
{
	CListEntry<CEventLogEntry*> *pItem = m_EventLog.GetSelectedEntry();
	if(!pItem || pItem->GetType() != ITEM)
			return;
	CEventLogEntry *pEntry = ((CListItem<CEventLogEntry*>*)pItem)->GetItemData();
	if(pEntry->pContact)
		SetButtonBitmap(3,IDB_CHAT);
	else
		SetButtonBitmap(3,NULL);
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CEventScreen::OnLCDButtonDown(int iButton)
{
	CListEntry<CEventLogEntry*> *pItem = NULL;
	CEventLogEntry *pEntry = NULL;

	if(iButton == 1 || iButton == 5)
		m_EventLog.ScrollDown();
	else if(iButton == 0 || iButton == 4)
		m_EventLog.ScrollUp();
	else if(iButton == 2)
		CAppletManager::GetInstance()->ActivateContactlistScreen();
	else if(iButton == 3)
	{
		pItem = m_EventLog.GetSelectedEntry();
		if(!pItem || pItem->GetType() != ITEM)
			return;
		
		pEntry = ((CListItem<CEventLogEntry*>*)pItem)->GetItemData();
		if(!pEntry->pContact || toLower(pEntry->pContact->strMedium) == _T("metacontact"))
			return;
		CAppletManager::GetInstance()->ActivateChatScreen(pEntry->pContact,pEntry->iConnectionID);
	}

	if(iButton <=1 ||iButton >= 4)
		UpdateChatButton();
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CEventScreen::OnLCDButtonRepeated(int iButton)
{
	if(iButton == 1)
		m_EventLog.ScrollDown();
	else if(iButton == 0)
		m_EventLog.ScrollUp();
	
	if(iButton <=1 ||iButton >= 4)
		UpdateChatButton();
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CEventScreen::OnLCDButtonUp(int iButton)
{
	
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CEventScreen::OnActivation()
{

}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CEventScreen::OnDeactivation()
{
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CEventScreen::OnExpiration()
{
}