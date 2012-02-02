#include "stdafx.h"
#include "CListScreen.h"
#include "CAppletManager.h"


//************************************************************************
// Constructor
//************************************************************************
CListScreen::CListScreen() {
	m_bFocused = false;
}

//************************************************************************
// Destructor
//************************************************************************
CListScreen::~CListScreen() {
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CListScreen::Initialize() {
	if(!CScreen::Initialize())
		return false;
	
	m_Controller = CHeliumController::getInstance();

	m_Playlist.Initialize();
	
	m_Playlist.SetOrigin(0,0);
	m_Playlist.SetSize(GetWidth(),GetHeight()-6);

	m_ListPositionLabel.Initialize();
	m_ListPositionLabel.SetAlignment(DT_CENTER);
	m_ListPositionLabel.SetWordWrap(TRUE);
	m_ListPositionLabel.SetOrigin(GetWidth()/2-20,36);
	m_ListPositionLabel.SetSize(40,8);
	m_ListPositionLabel.SetFontFaceName(_T("Small Fonts"));
	m_ListPositionLabel.SetFontPointSize(5);
	m_ListPositionLabel.SetText(_T("-/-"));

	AddObject(&m_ListPositionLabel);
	AddObject(&m_Playlist);

	SetButtonBitmap(0,IDB_BACK);
	SetButtonBitmap(1,IDB_PLAYPAUSE);
	SetButtonBitmap(2,IDB_UP);
	SetButtonBitmap(3,IDB_DOWN);

	m_updateTimer.Start();

	return true;
}

//************************************************************************
// Shutdown the scren
//************************************************************************
bool CListScreen::Shutdown() {
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CListScreen::Update() {
	if(!CScreen::Update())
		return false;
	
	if(m_updateTimer.GetSpan() > 1.0) {
		m_updateTimer.Start();
		int pos = m_Controller->getListPosition() + 1;
		int size = m_Controller->getListSize();
		if(size >= 0) {
			m_ListPositionLabel.SetText(tstringprintf(_T("%i/%i"),pos,size));
		} else {
			m_ListPositionLabel.SetText(_T("-/-"));
		}
	}
	
	return true;
}

//************************************************************************
// Resets the credits screens state
//************************************************************************
void CListScreen::Reset() {
	
}


//************************************************************************
// Draws the screen
//************************************************************************
bool CListScreen::Draw(CLCDGfx *pGfx) {
	if(!CScreen::Draw(pGfx))
		return false;
	
	return true;
}


//************************************************************************
// Called when the screen size has changed
//************************************************************************
void CListScreen::OnSizeChanged() {
	CScreen::OnSizeChanged();
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CListScreen::OnLCDButtonDown(int iButton){
	if(iButton == 0) {
		CAppletManager::GetInstance()->activatePlayingScreen();
	} else if(iButton == 2 || iButton == 3) {
		if(iButton == 2) {
			m_Playlist.ScrollUp();
		} else {
			m_Playlist.ScrollDown();
		}
		
	}
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CListScreen::OnLCDButtonRepeated(int iButton) {
	if(iButton == 1 && !m_bFocused) {
		m_bFocused = true;
		m_Playlist.setPositionByNumber(m_Controller->getListPosition());
	}
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CListScreen::OnLCDButtonUp(int iButton) {
	if(iButton == 1) {
		if(m_bFocused) {
			m_bFocused = false;
		} else {
			CHeliumController* pController = CHeliumController::getInstance();
			SLISTITEM* pItem = m_Playlist.getSelectedItem();
			
			int track = pController->getListPosition();
			int pos = pItem->iPos;
			if(pos < 0) {
				return;
			}
			if(pItem) {
				if(pos == track) {
					pController->playPause();
				} else {
					pController->setListPosition(pos);
					m_Playlist.focusCurrentTrack();
				}
			}
		}
	}
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CListScreen::OnActivation() {
	m_Playlist.setPositionByNumber(m_Controller->getListPosition());
}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CListScreen::OnDeactivation() {
	
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CListScreen::OnExpiration() {
}

// Called when the playback status has changed
void CListScreen::OnPlaybackStateChanged(EPlaybackState eStatus) {
}

// Called when the current track has changed
void CListScreen::OnTrackChanged(SPLAYINGITEM currentTrack) {
}

// Called when the connection to helium has changed
void CListScreen::OnConnectionChanged(bool bConnected) {
}

void CListScreen::OnListItemReceived(SLISTITEM item) {
	
}

void CListScreen::OnRatingChanged(int iRating) {
}

void CListScreen::OnPlaylistChanged(int size) {
}