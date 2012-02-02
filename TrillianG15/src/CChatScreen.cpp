#include "stdafx.h"
#include "CConfig.h"
#include "CChatScreen.h"
#include "CAppletManager.h"
#include "CConnectionData.h"

//************************************************************************
// Constructor
//************************************************************************
CChatScreen::CChatScreen()
{
	m_bHideTitle = false;
	m_bHideLabels = false;

	m_dwMaximizedTimer = 0;
	m_bMaximizedTimer = false;

	m_eReplyState = REPLY_STATE_NONE;
}

//************************************************************************
// Destructor
//************************************************************************
CChatScreen::~CChatScreen()
{
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CChatScreen::Initialize()
{
	if(!CScreen::Initialize())
		return false;

	m_ConnectionList.Initialize();
	m_InfoText.Initialize();
	m_UserName.Initialize();
	m_UserStatus.Initialize();
	m_UserProto.Initialize();
	m_Input.Initialize();
	m_TextLog.Initialize();
	m_Scrollbar.Initialize();

	
	// other attributes
		m_InfoText.SetAlignment(DT_CENTER);
		m_InfoText.SetWordWrap(TRUE);
		m_InfoText.SetText(_T(""));
		m_InfoText.Show(0);

		m_UserName.SetAlignment(DT_CENTER);
		m_UserName.SetWordWrap(TRUE);
		m_UserName.SetText(_T("Proto"));


		m_UserStatus.SetAlignment(DT_LEFT);
		m_UserStatus.SetWordWrap(TRUE);
		m_UserStatus.SetText(_T("Status"));
	
		
		m_UserProto.SetAlignment(DT_RIGHT);
		m_UserProto.SetWordWrap(TRUE);
		m_UserProto.SetText(_T("User"));

		m_Input.Show(0);

		m_ConnectionList.SetScrollbar(&m_Scrollbar);

		AddObject(&m_Scrollbar);
		AddObject(&m_TextLog);
		AddObject(&m_Input);
		AddObject(&m_InfoText);
		AddObject(&m_UserName);
		AddObject(&m_UserStatus);
		AddObject(&m_UserProto);
		AddObject(&m_ConnectionList);

	SetButtonBitmap(0,IDB_UP);
	SetButtonBitmap(1,IDB_DOWN);
	SetButtonBitmap(2,IDB_HISTORY);
	SetButtonBitmap(3,IDB_CHAT);

	return true;
}

//************************************************************************
// Shutdown the screen
//************************************************************************
bool CChatScreen::Shutdown()
{
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// update's the screens objects ( sizes, fonts etc)
//************************************************************************
void CChatScreen::UpdateObjects()
{
	m_bHideTitle = false;
	m_bHideLabels = !CConfig::GetBoolSetting(SHOW_LABELS);
	
	if(IsMaximized())
	{
		if(!m_bHideTitle && !CConfig::GetBoolSetting(MAXIMIZED_TITLE))
			m_bHideTitle = true;

		if(!m_bHideLabels && !CConfig::GetBoolSetting(MAXIMIZED_LABELS))
			m_bHideLabels = true;
	}

	// Fonts
	m_ConnectionList.SetFont(CConfig::GetFont(FONT_SESSION));
	m_TextLog.SetFont(CConfig::GetFont(FONT_SESSION));
	m_Input.SetFont(CConfig::GetFont(FONT_SESSION));
	m_InfoText.SetFont(CConfig::GetFont(FONT_SESSION));

	m_UserName.SetFont(CConfig::GetFont(FONT_TITLE));
	m_UserStatus.SetFont(CConfig::GetFont(FONT_TITLE));
	m_UserProto.SetFont(CConfig::GetFont(FONT_TITLE));
		
	// Sizes
	m_UserName.SetSize(80, CConfig::GetFontHeight(FONT_TITLE));
	m_UserStatus.SetSize(34, CConfig::GetFontHeight(FONT_TITLE));
	m_UserProto.SetSize(40, CConfig::GetFontHeight(FONT_TITLE));
	
	int iHeight =GetHeight();
	iHeight -= m_bHideTitle?0:CConfig::GetFontHeight(FONT_TITLE)+2;
	iHeight -= m_bHideLabels?0:5;

	m_Input.SetSize(GetWidth()-5, iHeight);
	m_TextLog.SetSize(GetWidth()-5, iHeight);
	
	m_InfoText.SetSize(160, 28);
	m_InfoText.SetOrigin(0,(iHeight-CConfig::GetFontHeight(FONT_SESSION))/2);

	// Origins
	m_ConnectionList.SetOrigin(0,2+(m_bHideTitle?0:CConfig::GetFontHeight(FONT_TITLE)+2));
	m_ConnectionList.SetSize(GetWidth()-4,iHeight-2);
	
	m_UserName.SetOrigin(40, 0);
	m_UserStatus.SetOrigin(8, 0);
	m_UserProto.SetOrigin(120, 0);
	
	m_TextLog.SetOrigin(0, m_bHideTitle?0:CConfig::GetFontHeight(FONT_TITLE)+2);
	m_Input.SetOrigin(0,m_bHideTitle?0:CConfig::GetFontHeight(FONT_TITLE)+2);
	
	m_InfoText.SetOrigin(0, 10);
	
	m_UserName.Show(!m_bHideTitle);
	m_UserStatus.Show(m_eMode != MODE_SELECTION && !m_bHideTitle);
	m_UserProto.Show(m_eMode != MODE_SELECTION && !m_bHideTitle);

	m_Scrollbar.SetOrigin(GetWidth()-4,(m_bHideTitle?0:CConfig::GetFontHeight(FONT_TITLE)+2));
	m_Scrollbar.SetSize(4,iHeight);

	// other options
	m_TextLog.SetLogSize(CConfig::GetIntSetting(SESSION_LOGSIZE));
		
	m_Input.SetBreakKeys(CConfig::GetBoolSetting(SESSION_SENDRETURN)?KEYS_RETURN:KEYS_CTRL_RETURN);
	m_Input.ShowSymbols(CConfig::GetBoolSetting(SESSION_SYMBOLS));

	// visibilitys
	if(m_eMode == MODE_SELECTION)
	{
		m_ConnectionList.Show(1);
		m_TextLog.Show(0);
		m_InfoText.Show(0);
	}
	else if(m_eMode == MODE_ERROR)
	{
		m_InfoText.Show(1);
		m_ConnectionList.Show(0);
		m_TextLog.Show(0);
	}
	else
	{
		if(!m_eReplyState == REPLY_STATE_INPUT)
			m_TextLog.Show(1);
		m_InfoText.Show(0);
		m_ConnectionList.Show(0);
	}
	ShowButtons(!m_bHideLabels);
}

//************************************************************************
// update's the screens title labels
//************************************************************************
void CChatScreen::UpdateLabels()
{
	if(!m_pContact)
	{
		m_UserName.SetText(_T(""));
		m_UserStatus.SetText(_T(""));
		m_UserProto.SetText(_T(""));
		return;
	}

	if(m_eMode == MODE_SELECTION)
		m_UserName.SetText(_T("Select a connection"));
	else
		m_UserName.SetText(m_pContact->strDisplayName);
	
	m_UserStatus.SetText(m_pContact->strStatus);
	m_UserProto.SetText(m_pContact->strMedium);

	if(m_bContactTyping)
		m_UserProto.SetText(_T("typing.."));
}

//************************************************************************
// return the chat contact
//************************************************************************
CContactData *CChatScreen::GetContact()
{
	return m_pContact;
}

//************************************************************************
// loads the contacts history
//************************************************************************
void CChatScreen::LoadHistory()
{
	if(!m_pContact || m_iConnectionID < 0)
		return;

	list<SMessage>::iterator iter = m_pContact->LMessages.begin();
	while(iter != m_pContact->LMessages.end())
	{
		if((*iter).iConnectionID != m_iConnectionID)
		{
			iter++;
			continue;
		}

		if((*iter).bIsMe)
			AddOutgoingMessage((*iter).strMessage,&((*iter).Time));
		else
			AddIncomingMessage((*iter).strMessage,&((*iter).Time));
		iter++;
	}
}

//************************************************************************
// sets the screen's chat contact
//************************************************************************
void CChatScreen::SetContact(CContactData *pContactData,int iConnectionID)
{
	m_pContact = pContactData;

	if(!pContactData)
		return;
	
	if(m_eReplyState != REPLY_STATE_NONE)
		DeactivateMessageMode();
	else if(IsMaximized())
		Minimize();

	SetButtonBitmap(0,IDB_UP);
	SetButtonBitmap(1,IDB_DOWN);
	SetButtonBitmap(3,IDB_REPLY);

	m_bContactTyping = false;
	m_TextLog.ClearLog();

	m_eMode = MODE_SELECTION;
	m_iConnectionID = iConnectionID;
	
	// For IRC pseudo contacts
	if(pContactData->iConnectionID != -1)
		m_iConnectionID = pContactData->iConnectionID;

	if(m_iConnectionID == -1)
	{
		int iConnections = 0;

		m_ConnectionList.Clear();
		vector<CConnectionData*> *pvConnectionData = CAppletManager::GetInstance()->GetConnectionDataVector();
		vector<CConnectionData*>::iterator coniter = pvConnectionData->begin();
		while(coniter != pvConnectionData->end())
		{
			if(toLower((*coniter)->strStatus) != _T("offline") &&
				(*coniter)->strMedium == pContactData->strMedium && 
				(*coniter)->strSection == pContactData->strSection)
			{
				m_ConnectionList.AddItem((*coniter)->strID);
				iConnections++;
			}
			coniter++;
		}				
	
		// No usable connections found
		if(iConnections == 0)
		{
			m_eMode = MODE_ERROR;
			m_InfoText.SetText(_T("No usable connections found!"));

			SetButtonBitmap(0,NULL);
			SetButtonBitmap(1,NULL);
			SetButtonBitmap(3,NULL);
		}
		else if(iConnections > 1)
		{
			m_eMode = MODE_SELECTION;
			m_ConnectionList.SetScrollbar(&m_Scrollbar);

			SetButtonBitmap(0,IDB_UP);
			SetButtonBitmap(1,IDB_DOWN);
		}
		else
		{
			tstring strID = ((CListItem<tstring>*)m_ConnectionList.GetSelectedEntry())->GetItemData();
			CConnectionData *pConnectionData = CAppletManager::GetInstance()->GetConnectionData(m_pContact->strMedium,strID);
			m_iConnectionID = pConnectionData->iConnection;
			m_eMode = MODE_CHAT;
		}
	}
	else
		m_eMode = MODE_CHAT;

	UpdateObjects();
	UpdateLabels();

	if(m_iConnectionID >= 0)
		LoadHistory();
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CChatScreen::Update()
{
	if(!CScreen::Update())
		return false;

	if(m_bMaximizedTimer && m_dwMaximizedDuration != INFINITE)
	{
		if(m_dwMaximizedTimer + m_dwMaximizedDuration <= GetTickCount())
			Minimize();
	}

	if(m_eReplyState == REPLY_STATE_INPUT && !m_Input.IsInputActive())
		SendCurrentMessage();
	
	return true;
}

//************************************************************************
// Draws the screen
//************************************************************************
bool CChatScreen::Draw(CLCDGfx *pGfx)
{
	if(!CScreen::Draw(pGfx))
		return false;

	if(!m_bHideTitle)
	{
		int iTitleHeight = CConfig::GetFontHeight(FONT_TITLE)+1;
		pGfx->DrawLine(0,iTitleHeight<6?6:iTitleHeight,GetWidth(),iTitleHeight<6?6:iTitleHeight);
		if(m_eMode != MODE_SELECTION)
		{
			int iOffset = (iTitleHeight-5)/2;
			pGfx->DrawBitmap(1,iOffset,5,5,CAppletManager::GetInstance()->GetStatusBitmap(m_pContact->strStatus));
		}
	}
	return true;
}

//************************************************************************
// Adds an outgoing message to the log
//************************************************************************
void CChatScreen::AddOutgoingMessage(tstring strMessage,tm *time)
{
	tstring strPrefix = _T(">> ");
	if(CConfig::GetBoolSetting(SESSION_TIMESTAMPS))
		strPrefix += CAppletManager::GetFormattedTimestamp(time) + _T(" ");

	// adjust the scroll mode
	m_TextLog.SetAutoscrollMode(SCROLL_LINE);

	// add the message
	m_TextLog.AddText(strPrefix + strMessage,true);

}

//************************************************************************
// Adds an incoming message to the log
//************************************************************************
void CChatScreen::AddIncomingMessage(tstring strMessage,tm *time)
{
	tstring strPrefix = _T("<< ");
	if(CConfig::GetBoolSetting(SESSION_TIMESTAMPS))
		strPrefix += CAppletManager::GetFormattedTimestamp(time) + _T(" ");

	
	// adjust the scroll mode
	EScrollMode eMode;
	switch(CConfig::GetIntSetting(SESSION_AUTOSCROLL))
	{
	case SESSION_AUTOSCROLL_NONE: eMode = SCROLL_NONE; break;
	case SESSION_AUTOSCROLL_FIRST: eMode = SCROLL_MESSAGE; break;
	case SESSION_AUTOSCROLL_LAST: eMode = SCROLL_LINE; break;
	}
	m_TextLog.SetAutoscrollMode(eMode);

	// add the message
	m_TextLog.AddText(strPrefix + strMessage);
}

//************************************************************************
// activates the input mode
//************************************************************************
void CChatScreen::ActivateMessageMode()
{
	m_InfoText.Show(0);
	m_TextLog.Show(0);
	m_TextLog.SetScrollbar(NULL);
	m_Input.SetScrollbar(&m_Scrollbar);

	if(m_eReplyState != REPLY_STATE_FAILED)
		m_Input.Reset();

	m_Input.Show(1);
	m_Input.ActivateInput();

	m_eReplyState = REPLY_STATE_INPUT;

	SetButtonBitmap(2,IDB_BACK);
	SetButtonBitmap(3,IDB_SEND);	

	if(CConfig::GetBoolSetting(SESSION_REPLY_MAXIMIZED))
		Maximize();
	else
		Minimize();
}

//************************************************************************
// sends the message
//************************************************************************
void CChatScreen::SendCurrentMessage()
{
	if(m_Input.GetText().empty())
	{
		DeactivateMessageMode();
		return;
	}
	//ASSERT(m_eReplyState == REPLY_STATE_INPUT);

	m_eReplyState = REPLY_STATE_SENDING;

	m_Input.DeactivateInput();

	m_InfoText.SetText(_T("Sending message..."));
	m_InfoText.Show(1);
	m_Input.Show(0);
	
	string strType = "outgoing";
	string strMedium = Utf8_Encode(m_pContact->strMedium.c_str());
	string strID = Utf8_Encode(m_pContact->strID.c_str());
	string strMessage = "<HTML><BODY BGCOLOR=\"#ffffff\">" + Utf8_Encode(m_Input.GetText().c_str()) + "</BODY></HTML>";
	string strSection = Utf8_Encode(m_pContact->strSection.c_str());
	
	// padd 1 char messages with a carriage return (trillian won't sent the message otherwise)
	if(strMessage.length() == 1)
		strMessage += "\r";

	message_t msg;

	string::size_type pos;

	while((pos = strMessage.find("\r\n")) != string::npos)
	{
		strMessage.replace(pos,2,"<br>");
	}

	trillianInitialize(msg);
	msg.medium = (char*)strMedium.c_str();
	msg.name = (char*)strID.c_str();
	msg.text = (char*)strMessage.c_str();
	msg.type = (char*)strType.c_str();

	msg.connection_id = m_iConnectionID;
	
	//msg.text_len = 3;//strMessage.length();

	if(strSection.empty())
		msg.section = NULL;
	else
		msg.section = (char*)strSection.c_str();

	int res = g_plugin_send(APP_GUID,"messageSend", &msg);
	
	//m_hMessage = CAppletManager::SendMessageToContact(m_hContact,m_Input.GetText());

	SetButtonBitmap(2,NULL);
	SetButtonBitmap(3,NULL);

	if(res < 0)
		InvalidateMessageMode(_T("Message could not be sent!"));
	else
	{
		// Somehow hacky bugfix - window id is set after message broadcast,
		// so the first messages recepient cant be identified
		if(m_pContact->iWindowID < 0)
		{
			time_t now;
			time(&now);
			tm tmnow;
			localtime_s(&tmnow,&now);
			AddOutgoingMessage(m_Input.GetText(),&tmnow);

			SMessage historymsg;
			historymsg.bIsMe = true;
			historymsg.Time = tmnow;
			historymsg.strMessage = m_Input.GetText();
			historymsg.iConnectionID = m_iConnectionID;
			m_pContact->LMessages.push_back(historymsg);
		}

		DeactivateMessageMode();
	}
}

//************************************************************************
// invalidates the message mode
//************************************************************************
void CChatScreen::InvalidateMessageMode(tstring strError)
{
	m_eReplyState = REPLY_STATE_FAILED;

	m_InfoText.SetText(strError);

	SetButtonBitmap(2,IDB_BACK);
	SetButtonBitmap(3,IDB_SEND);	

	if(IsMaximized())
		Minimize();
}

//************************************************************************
// deactivates the input mode
//************************************************************************
void CChatScreen::DeactivateMessageMode()
{
	m_Input.Reset();

	m_Input.SetScrollbar(NULL);
	m_TextLog.SetScrollbar(&m_Scrollbar);

	m_TextLog.Show(1);
	m_InfoText.Show(0);
	m_Input.Show(0);
	
	m_Input.DeactivateInput();

	m_eReplyState = REPLY_STATE_NONE;
	
	SetButtonBitmap(2,IDB_HISTORY);
	SetButtonBitmap(3,IDB_REPLY);

	if(IsMaximized())
		Minimize();
}

//************************************************************************
// maximizes the content object
//************************************************************************
void CChatScreen::Maximize(DWORD dwTimer)
{
	m_bMaximizedTimer = true;
	m_dwMaximizedTimer = GetTickCount();
	m_dwMaximizedDuration = dwTimer;
	UpdateObjects();
}

//************************************************************************
// minimizes the content object
//************************************************************************
void CChatScreen::Minimize()
{
	m_bMaximizedTimer = false;
	m_dwMaximizedTimer = 0;
	UpdateObjects();
}

//************************************************************************
// returns wether the content is maximized
//************************************************************************
bool CChatScreen::IsMaximized()
{
	return m_bMaximizedTimer;
}

//************************************************************************
// Called when the applet's configuration has changed
//************************************************************************
void CChatScreen::OnConfigChanged()
{
	CScreen::OnConfigChanged();

	UpdateObjects();

	m_TextLog.ClearLog();
	LoadHistory();
}

//************************************************************************
// Called when an event is received
//************************************************************************
void CChatScreen::OnEventReceived(CEvent *pEvent)
{
	if(!m_pContact || pEvent->pContact != m_pContact || pEvent->iConnectionID != m_iConnectionID)
		return;
	
	switch(pEvent->eType)
	{
	case EVENT_MSG_RECEIVED:
		// Add the message to the log
		AddIncomingMessage(pEvent->strValue,&pEvent->Time);
		// mark it as read if required
		//if(CConfig::GetBoolSetting(SESSION_MARKREAD) && !CAppletManager::IsMessageWindowOpen(m_hContact))
		//	CAppletManager::MarkMessageAsRead(m_hContact,pEvent->hValue);
		break;
	case EVENT_MSG_SENT:
		// Add the message to the log
		AddOutgoingMessage(pEvent->strValue,&pEvent->Time);
		break;
	case EVENT_CONTACT_DELETED:
		CAppletManager::GetInstance()->ActivateEventScreen();
		break;
	case EVENT_TYPING_NOTIFICATION:
		m_bContactTyping = pEvent->iValue != 0;
		UpdateLabels();
		break;
	case EVENT_STATUS:
	case EVENT_CONTACT_NICK:
	case EVENT_SIGNED_OFF:
	case EVENT_SIGNED_ON:
		UpdateLabels();
		break;
	}
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CChatScreen::OnLCDButtonDown(int iButton)
{
	if(m_eMode == MODE_SELECTION)
	{
		if(iButton < 2 || iButton > 3)
		{
			bool bRes = false;
			if(iButton == 0 || iButton == 4)
				bRes = m_ConnectionList.ScrollUp();
			else
				bRes = m_ConnectionList.ScrollDown();
		}
		else if(iButton == 2)
			CAppletManager::GetInstance()->ActivateEventScreen();
		else if(iButton == 3)
		{
			tstring strID = ((CListItem<tstring>*)m_ConnectionList.GetSelectedEntry())->GetItemData();
			CConnectionData *pConnectionData = CAppletManager::GetInstance()->GetConnectionData(m_pContact->strMedium,strID);
			m_iConnectionID = pConnectionData->iConnection;

			m_TextLog.SetScrollbar(&m_Scrollbar);

			SetButtonBitmap(3,IDB_REPLY);
			m_eMode = MODE_CHAT;
			UpdateObjects();
			UpdateLabels();
			LoadHistory();
		}
	}
	else if(m_eMode == MODE_CHAT)
	{
		switch(m_eReplyState)
		{
		case REPLY_STATE_NONE:
			if(iButton < 2 || iButton > 3)
			{
				bool bRes = false;
				if(iButton == 0 || iButton == 4)
					bRes = m_TextLog.ScrollUp();
				else
					bRes = m_TextLog.ScrollDown();

				if(bRes && CConfig::GetBoolSetting(SESSION_SCROLL_MAXIMIZED))
					Maximize(5000);
			}
			else if(iButton == 2)
				CAppletManager::GetInstance()->ActivateEventScreen();
			// enter reply mode
			else if(iButton == 3)
				ActivateMessageMode();
			break;
		case REPLY_STATE_FAILED:
			if(iButton == 2)
				DeactivateMessageMode();
			else if(iButton == 3)
				ActivateMessageMode();
			break;
		case REPLY_STATE_SENDING:
			break;
		case REPLY_STATE_INPUT:
			if(iButton == 0 || iButton == 4)
				m_Input.ScrollLine(0);
			else if(iButton == 1 || iButton == 5)
				m_Input.ScrollLine(1);
			// send the message
			else if(iButton == 3)
				SendCurrentMessage();
			// cancel message mode
			else if(iButton == 2)
				DeactivateMessageMode();
			break;
		}
	}
	// MODE_ERROR
	else
	{
		if(iButton == 2)
			CAppletManager::GetInstance()->ActivateEventScreen();
	}
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CChatScreen::OnLCDButtonRepeated(int iButton)
{
	switch(m_eReplyState)
	{
	case REPLY_STATE_NONE:
		if(iButton < 2)
		{
			bool bRes = false;
			if(iButton == 0)
				bRes = m_TextLog.ScrollUp();
			else
				bRes = m_TextLog.ScrollDown();

			if(bRes && CConfig::GetBoolSetting(SESSION_SCROLL_MAXIMIZED))
				Maximize(5000);
		}
		break;
	case REPLY_STATE_INPUT:
		if(iButton == 0)
			m_Input.ScrollLine(0);
		else if(iButton == 1)
			m_Input.ScrollLine(1);
	}
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CChatScreen::OnLCDButtonUp(int iButton)
{
	
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CChatScreen::OnActivation()
{

}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CChatScreen::OnDeactivation()
{
}	

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CChatScreen::OnExpiration()
{

}