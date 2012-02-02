#ifndef _CSCREEN_H_
#define _CSCREEN_H_

#include "CLCDScreen.h"
#include "CLCDBitmap.h"

class CScreen : public CLCDScreen
{
public:
	// Constructor
	CScreen();
	// Destructor
	~CScreen();

	// Initializes the screen 
	bool Initialize();
	// Shutdown the scren
	bool Shutdown();
	// Updates the screen
	bool Update();
	// Draws the screen
	bool Draw(CLCDGfx *pGfx);

	// Called when an event is received
	virtual void OnEventReceived(CEvent *pEvent);
	// Called when the configuration has changed
	virtual void OnConfigChanged();
protected:
	// Set the specified button label
	void SetButtonBitmap(int iButton, int iBitmap);
	// Hide/Show the buttons
	void ShowButtons(bool bShow);
private:
	CLCDBitmap	m_aButtons[4];
	HBITMAP		m_ahBitmaps[4];
	bool		m_abShowButtons[4];
	bool		m_bHideButtons;

	void UpdateButtons();
};

#endif