#include "stdafx.h"
#include "CConfig.h"
#include "CContactlistScreen.h"
#include "CAppletManager.h"

//************************************************************************
// Constructor
//************************************************************************
CContactlistScreen::CContactlistScreen()
{
}

//************************************************************************
// Destructor
//************************************************************************
CContactlistScreen::~CContactlistScreen()
{
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CContactlistScreen::Initialize()
{
	if(!CScreen::Initialize())
		return false;

	m_ContactList.Initialize();
	m_ContactList.SetOrigin(0, 0);
	m_ContactList.SetSize(GetWidth()-5, GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?6:0));
	m_ContactList.SetFont(CConfig::GetFont(FONT_CLIST));
	m_ContactList.SetDrawTreeLines(CConfig::GetBoolSetting(CLIST_DRAWLINES));
	m_ContactList.SetColumns(CConfig::GetBoolSetting(CLIST_COLUMNS)?2:1);

	AddObject(&m_ContactList);

	m_Scrollbar.Initialize();
	m_Scrollbar.SetOrigin(GetWidth()-4,0);
	m_Scrollbar.SetSize(4,GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?5:0));
	m_ContactList.SetScrollbar(&m_Scrollbar);

	AddObject(&m_Scrollbar);

	SetButtonBitmap(0,IDB_UP);
	SetButtonBitmap(1,IDB_DOWN);
	SetButtonBitmap(2,IDB_HISTORY);
	SetButtonBitmap(3,IDB_CHAT);

	return true;
}

//************************************************************************
// Shutdown the screen
//************************************************************************
bool CContactlistScreen::Shutdown()
{
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// regenerates the list
//************************************************************************
void CContactlistScreen::RegenerateList()
{
	m_ContactList.RegenerateList();
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CContactlistScreen::Update()
{
	if(!CScreen::Update())
		return false;

	return true;
}

//************************************************************************
// Draws the screen
//************************************************************************
bool CContactlistScreen::Draw(CLCDGfx *pGfx)
{
	if(!CScreen::Draw(pGfx))
		return false;

	return true;
}

//************************************************************************
// resets the position of the contactlist
//************************************************************************
void CContactlistScreen::ResetPosition()
{
	// collapse all groups if setting is active
	switch(CConfig::GetIntSetting(CLIST_GA))
	{
	case CLIST_GA_COLLAPSE:
		m_ContactList.CollapseAll();
		break;
	case CLIST_GA_EXPAND:
		m_ContactList.ExpandAll();
		break;
	}
	
	// select the first item if setting is active
	if(CConfig::GetBoolSetting(CLIST_POSITION))
		m_ContactList.SetPosition(m_ContactList.GetFirstEntry());

	UpdateUseSoftkeyLabel();
}

//************************************************************************
// Called when the configuration has changed
//************************************************************************
void CContactlistScreen::OnConfigChanged()
{
	CScreen::OnConfigChanged();

	m_ContactList.OnConfigChanged();
	m_ContactList.SetDrawTreeLines(CConfig::GetBoolSetting(CLIST_DRAWLINES));
	m_ContactList.SetSize(GetWidth()-5, GetHeight()-(CConfig::GetBoolSetting(SHOW_LABELS)?6:0));
	m_ContactList.SetFont(CConfig::GetFont(FONT_CLIST));
	m_ContactList.SetColumns(CConfig::GetBoolSetting(CLIST_COLUMNS)?2:1);
	
	m_Scrollbar.SetSize(4,GetHeight()-((CConfig::GetBoolSetting(SHOW_LABELS)?5:0)));

	ResetPosition();
}

//************************************************************************
// Called when an event is received
//************************************************************************
void CContactlistScreen::OnEventReceived(CEvent *pEvent)
{
	FILE *fp = NULL;
	int iStatus = 0;
	switch(pEvent->eType)
	{
	case EVENT_SIGNED_ON:
	case EVENT_SIGNED_OFF:
	case EVENT_STATUS:
		m_ContactList.OnStatusChange(pEvent->pContact,pEvent->strValue2);
		break;
	case EVENT_CONTACT_ADDED:
		m_ContactList.OnContactAdded(pEvent->pContact);
		break;
	case EVENT_CONTACT_DELETED:
		m_ContactList.OnContactDeleted(pEvent->pContact);
		break;
	}
}

//************************************************************************
// Called when a chat session was opened
//************************************************************************
void CContactlistScreen::OnSessionOpened(HANDLE hContact)
{
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CContactlistScreen::OnLCDButtonDown(int iButton)
{
	if(iButton == 0  || iButton == 4)
		m_ContactList.ScrollUp();
	else if(iButton == 1 || iButton == 5)
		m_ContactList.ScrollDown();
	else if(iButton == 2)
	{
		CAppletManager::GetInstance()->ActivateEventScreen();
		return;
	}
	else if(iButton == 3)
	{
		CListEntry<CContactData*,CContactListGroup*> *pEntry = m_ContactList.GetSelectedEntry();
		if(pEntry->GetType() == CONTAINER)
			((CListContainer<CContactData*,CContactListGroup*>*)pEntry)->ToggleOpen();
		else if(pEntry->GetType() == ITEM)
		{
			CContactData *pContact = ((CListItem<CContactData*,CContactListGroup*>*)pEntry)->GetItemData();
			if(!pContact)
				return;
			CAppletManager::GetInstance()->ActivateChatScreen(pContact);
			return;
		}
	}

	UpdateUseSoftkeyLabel();
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CContactlistScreen::OnLCDButtonRepeated(int iButton)
{
	if(iButton == 0)
		m_ContactList.ScrollUp();
	else if(iButton== 1)
		m_ContactList.ScrollDown();

	if(iButton != 2)
		UpdateUseSoftkeyLabel();
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CContactlistScreen::OnLCDButtonUp(int iButton)
{
	
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CContactlistScreen::OnActivation()
{

}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CContactlistScreen::OnDeactivation()
{
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CContactlistScreen::OnExpiration()
{
}

//************************************************************************
// updates the use softkey label
//************************************************************************
void CContactlistScreen::UpdateUseSoftkeyLabel()
{
	CListEntry<CContactData*,CContactListGroup*> *pEntry = m_ContactList.GetSelectedEntry();
	if(!pEntry)
		return;
	if(pEntry->GetType() == CONTAINER)
	{
		if(((CListContainer<CContactData*,CContactListGroup*>*)pEntry)->IsOpen())	
			SetButtonBitmap(3,IDB_MINUS);
		else
			SetButtonBitmap(3,IDB_PLUS);
	}
	else
	{
		SetButtonBitmap(3,IDB_CHAT);
	}
}