#include "stdafx.h"
#include "CConfig.h"
#include "CScreen.h"

//************************************************************************
// Constructor
//************************************************************************
CScreen::CScreen()
{
}

//************************************************************************
// Destructor
//************************************************************************
CScreen::~CScreen()
{
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CScreen::Initialize()
{
	if(!CLCDScreen::Initialize())
		return false;

	// initialize the four button labels
	for (int i = 0; i < 4; i++)
    {
        m_aButtons[i].Initialize();
        m_aButtons[i].SetSize(17, 3);
        m_aButtons[i].Show(0);
		m_abShowButtons[i] = false;
		m_ahBitmaps[i] = NULL;
		m_aButtons[i].SetOrigin(10+i*29+(i/2)*36, 40);
		AddObject(&m_aButtons[i]);
    }

	return true;
}

//************************************************************************
// Shutdown the scren
//************************************************************************
bool CScreen::Shutdown()
{
	if(!CLCDScreen::Shutdown())
		return false;
	
	for(int i=0; i < 4; i++)
		if(m_ahBitmaps[i] != NULL)
			DeleteObject(m_ahBitmaps[i]);

	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CScreen::Update()
{
	if(!CLCDScreen::Update())
		return false;

	return true;
}

//************************************************************************
// Draws the screen
//************************************************************************
bool CScreen::Draw(CLCDGfx *pGfx)
{
	if(!CLCDScreen::Draw(pGfx))
		return false;
	
	for(int i=0;i<4;i++)
		if(m_aButtons[i].IsVisible())
		{
			pGfx->DrawLine(0,GetHeight()-5,GetWidth(),GetHeight()-5);
			break;
		}

	return true;
}

//************************************************************************
// Set the specified button label
//************************************************************************
void CScreen::SetButtonBitmap(int iButton, int iBitmap)
{
	if(iButton <0 || iButton > 3)
		return;
	if(iBitmap == 0)
	{
		m_aButtons[iButton].Show(0);
		m_abShowButtons[iButton] = false;
	}
	else
	{
		if(m_ahBitmaps[iButton] != NULL)
			DeleteObject(m_ahBitmaps[iButton]);

		m_ahBitmaps[iButton] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(iBitmap),
												IMAGE_BITMAP,17, 3, LR_MONOCHROME);
		m_aButtons[iButton].SetBitmap(m_ahBitmaps[iButton]);

		if(CConfig::GetBoolSetting(SHOW_LABELS))
			m_aButtons[iButton].Show(1);
		m_abShowButtons[iButton] = true;
	}
}

//************************************************************************
// shows/hides the buttons
//************************************************************************
void CScreen::ShowButtons(bool bShow)
{
	m_bHideButtons = !bShow;
	UpdateButtons();
}

//************************************************************************
// Update the buttons
//************************************************************************
void CScreen::UpdateButtons()
{
	for (int i = 0; i < 4; i++)
	{
		if(m_abShowButtons[i])
			m_aButtons[i].Show(CConfig::GetBoolSetting(SHOW_LABELS) && !m_bHideButtons);
	}
}

//************************************************************************
// Called when an event is received
//************************************************************************
void CScreen::OnEventReceived(CEvent *pEvent)
{
}

//************************************************************************
// Called when the configuration has changed
//************************************************************************
void CScreen::OnConfigChanged()
{
	UpdateButtons();
}