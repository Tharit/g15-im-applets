#ifndef _CCHATSCREEN_H_
#define _CCHATSCREEN_H_

#include "CContactData.h"

#include "CScreen.h"

#include "CLCDLabel.h"
#include "CLCDTextLog.h"
#include "CLCDInput.h"
#include "CTextList.h"

class CChatScreen : public CScreen
{
public:
	// Constructor
	CChatScreen();
	// Destructor
	~CChatScreen();

	// Initializes the screen 
	bool Initialize();
	// Shutdown the scren
	bool Shutdown();
	// Updates the screen
	bool Update();
	// Draws the screen
	bool Draw(CLCDGfx *pGfx);

	// Called when an event is received
	void OnEventReceived(CEvent *pEvent);
	// Called when an LCD-button is pressed
	void OnLCDButtonDown(int iButton);
	// Called when an LCD-button event is repeated
	void OnLCDButtonRepeated(int iButton);
	// Called when an LCD-button is released
	void OnLCDButtonUp(int iButton);
	// Called when the screen is activated
	void OnActivation();
	// Called when the screen is deactivated
	void OnDeactivation();
	// Called when the screen has expired
	void OnExpiration();
	// Called when the applet's configuration has changed
	void OnConfigChanged();

	// Set's the chat contact
	void SetContact(CContactData *pContactData,int iConnectionID = -1);
	// return the chat contact
	CContactData *GetContact();

protected:
	// loads the contacts history
	void LoadHistory();

	// Adds an outgoing message to the log
	void AddOutgoingMessage(tstring strMessage,tm *time);
	// Adds an incoming message to the log
	void AddIncomingMessage(tstring strMessage,tm *time);

	// maximizes the content object
	void Maximize(DWORD dwTimer=INFINITE);
	// minimizes the content object
	void Minimize();
	// returns wether the content is maximized
	bool IsMaximized();

	// activates the input mode
	void ActivateMessageMode();
	// deactivates the input mode
	void DeactivateMessageMode();
	// sends the message
	void SendCurrentMessage();
	// invalidates the message mode
	void InvalidateMessageMode(tstring strError);

	// update's the screens objects ( sizes, fonts etc)
	void UpdateObjects();
	// update's the screens title labels
	void UpdateLabels();

	bool m_bHideLabels,m_bHideTitle;
	bool m_bMaximizedTimer;
	
	bool m_bContactTyping;

	enum { REPLY_STATE_NONE,REPLY_STATE_INPUT,REPLY_STATE_SENDING,REPLY_STATE_FAILED} m_eReplyState;
	enum { MODE_ERROR, MODE_SELECTION, MODE_CHAT} m_eMode;

	DWORD	m_dwMaximizedTimer;
	DWORD	m_dwMaximizedDuration;

	CContactData *m_pContact;

	CLCDLabel m_UserProto;
	CLCDLabel m_UserName;
	CLCDLabel m_UserStatus;
	CLCDLabel m_InfoText;
	CTextList m_ConnectionList;

	CLCDInput m_Input;
	CLCDTextLog m_TextLog;
	CLCDBar	m_Scrollbar;

	int	m_iConnectionID;
};

#endif