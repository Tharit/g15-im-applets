#include "stdafx.h"
#include "CHeliumController.h"
#include "CPlayingScreen.h"
#include "CAppletManager.h"


//************************************************************************
// Constructor
//************************************************************************
CPlayingScreen::CPlayingScreen() {
	m_eMode	= MODE_VIEW;
	m_iNewRating = -1;
	m_iNewPosition = -1;
}

//************************************************************************
// Destructor
//************************************************************************
CPlayingScreen::~CPlayingScreen() {
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CPlayingScreen::Initialize() {
	if(!CScreen::Initialize())
		return false;
	
	m_currentTrack.bEmpty = true;

	m_Controller = CHeliumController::getInstance();
	m_ePlaybackState = m_Controller->getPlaybackState();
	m_currentTrack = m_Controller->getCurrentTrack();

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// register observer
	CHeliumController::getInstance()->registerObserver(this);
	
	// load status bitmaps
	m_hStatusBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_PAUSED),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_hStarBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STAR),
												IMAGE_BITMAP,5,5, LR_MONOCHROME);
	m_hStarBitmap2 = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STAR2),
												IMAGE_BITMAP,5,5, LR_MONOCHROME);
	m_hStarBitmap3 = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_STAR3),
												IMAGE_BITMAP,5,5, LR_MONOCHROME);

	m_updateTimer.Start();

	m_StreamDurationLabel.Initialize();
	m_ListPositionLabel.Initialize();
	m_StatusBitmap.Initialize();
	m_PositionLabel.Initialize();
	m_DurationBar.Initialize();
	m_Label.Initialize();
	m_Label2.Initialize();
	m_DurationLabel.Initialize();

	m_Label.SetAlignment(DT_CENTER);
	m_Label.SetFontFaceName(_T("Microsoft Sans Serif"));
	m_Label.SetFontPointSize(8);
	m_Label.SetFontWeight(FW_BOLD);
	m_Label.SetOrigin(0,0);
	m_Label.SetSize(GetWidth(),13);

	m_Label2.SetAlignment(DT_CENTER);
	m_Label2.SetText(_T(""));
	m_Label2.SetFontFaceName(_T("Microsoft Sans Serif"));
	m_Label2.SetFontPointSize(8);
	m_Label2.SetOrigin(0,13);
	m_Label2.SetSize(GetWidth(),13);
	
	m_PositionLabel.SetOrigin(0,28);
	m_PositionLabel.SetSize(22,9);
	m_PositionLabel.SetFontFaceName(_T("Small Fonts"));
	m_PositionLabel.SetFontPointSize(5);

	m_DurationLabel.SetAlignment(DT_RIGHT);
	m_DurationLabel.SetOrigin(GetWidth()-22,28);
	m_DurationLabel.SetSize(22,9);
	m_DurationLabel.SetFontFaceName(_T("Small Fonts"));
	m_DurationLabel.SetFontPointSize(5);

	m_StreamDurationLabel.SetAlignment(DT_CENTER);
	m_StreamDurationLabel.SetOrigin(GetWidth()/2-13,28);
	m_StreamDurationLabel.SetSize(26,9);
	m_StreamDurationLabel.SetFontFaceName(_T("Small Fonts"));
	m_StreamDurationLabel.SetFontPointSize(5);
	m_StreamDurationLabel.Show(false);

	m_ListPositionLabel.SetAlignment(DT_CENTER);
	m_ListPositionLabel.SetWordWrap(TRUE);
	m_ListPositionLabel.SetOrigin(GetWidth()/2-20,36);
	m_ListPositionLabel.SetSize(40,8);
	m_ListPositionLabel.SetFontFaceName(_T("Small Fonts"));
	m_ListPositionLabel.SetFontPointSize(5);
	m_ListPositionLabel.SetText(_T("-/-"));

	m_DurationBar.Show(false);
	m_DurationBar.SetOrigin(22,31);
	m_DurationBar.SetSize(GetWidth()-(44),3);
	m_DurationBar.SetMode(MODE_PROGRESSBAR);
	m_DurationBar.SetRange(0,0);
	m_DurationBar.SetOrientation(DIRECTION_HORIZONTAL);
	
	for(int i=0;i<5;i++) {
		m_StarBitmap[i].Initialize();
		m_StarBitmap[i].SetOrigin(i*10+(GetWidth()/2-(5*10)/2),29);
		m_StarBitmap[i].SetSize(5,5);
		m_StarBitmap[i].SetBitmap(m_hStarBitmap2);
		m_StarBitmap[i].Show(false);
		AddObject(&m_StarBitmap[i]);
	}
	
	m_StatusBitmap.SetOrigin(GetWidth()/2-5/2,32-5/2);
	m_StatusBitmap.SetSize(5,5);
	m_StatusBitmap.SetBitmap(m_hStatusBitmap);
	m_StatusBitmap.Show(false);

	AddObject(&m_ListPositionLabel);
	AddObject(&m_StreamDurationLabel);
	AddObject(&m_DurationLabel);
	AddObject(&m_PositionLabel);
	AddObject(&m_Label);
	AddObject(&m_Label2);
	AddObject(&m_DurationBar);
	AddObject(&m_StatusBitmap);

	SetButtonBitmap(0,IDB_LIST);
	SetButtonBitmap(1,IDB_MODE);
	SetButtonBitmap(2,IDB_REW);
	SetButtonBitmap(3,IDB_FFWD);

	updateCurrentTrack();
	configureScreen();

	return true;
}

//************************************************************************
// Shutdown the scren
//************************************************************************
bool CPlayingScreen::Shutdown() {
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CPlayingScreen::Update() {
	if(!CScreen::Update())
		return false;
	
	if(m_updateTimer.GetSpan() > 1.0 && m_ePlaybackState == STATE_PLAYING) {
		m_updateTimer.Start();
		updatePosition();
	}
	if(m_modeTimer.GetSpan() >= 4.0 && m_eMode == MODE_RATE) {
		m_modeTimer.Stop();
		m_eMode = MODE_VIEW;
		SetButtonBitmap(2,IDB_REW);
		SetButtonBitmap(3,IDB_FFWD);
		configureScreen();
	}

	return true;
}

//************************************************************************
// Resets the credits screens state
//************************************************************************
void CPlayingScreen::Reset() {
	
}


//************************************************************************
// Draws the screen
//************************************************************************
bool CPlayingScreen::Draw(CLCDGfx *pGfx) {
	if(!CScreen::Draw(pGfx))
		return false;
	
	return true;
}


//************************************************************************
// Called when the screen size has changed
//************************************************************************
void CPlayingScreen::OnSizeChanged() {
	CScreen::OnSizeChanged();
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CPlayingScreen::OnLCDButtonDown(int iButton){
	if(iButton == 0) {
		CAppletManager::GetInstance()->activateListScreen();
	} else if(iButton == 1) {
		if(m_currentTrack.bStream) {
			return;
		}
		if(m_eMode == MODE_VIEW) {
			m_modeTimer.Start();
			m_eMode = MODE_RATE;
			SetButtonBitmap(2,IDB_MINUS);
			SetButtonBitmap(3,IDB_PLUS);
		} else {
			m_eMode = MODE_VIEW;
			SetButtonBitmap(2,IDB_REW);
			SetButtonBitmap(3,IDB_FFWD);
		}
		configureScreen();
	} else {
		if(m_eMode == MODE_RATE) {
			if(m_iNewRating == -1) {
				m_iNewRating = m_currentTrack.iRating;
			}
			if(iButton == 2) {
				m_iNewRating--;
			} else if(iButton == 3) {
				m_iNewRating++;
			}
			if(m_iNewRating < 0) {
				m_iNewRating = 0;
			} else if(m_iNewRating > 10) {
				m_iNewRating = 10;
			}
			updateRating();
		} else {
			if(m_ePlaybackState != STATE_PLAYING || m_currentTrack.bStream) {
				return;
			}
			if(m_iNewPosition == -1) {
				m_iNewPosition = m_Controller->getPosition();
			}
			if(iButton == 2) {
				m_iNewPosition--;
			} else if(iButton == 3) {
				m_iNewPosition++;
			}
			if(m_iNewPosition < 0) {
				m_iNewPosition = 0;
			} else if(m_iNewPosition > m_currentTrack.iLength) {
				m_iNewPosition =  m_currentTrack.iLength;
			}
			updatePosition();
		}
	}
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CPlayingScreen::OnLCDButtonRepeated(int iButton) {
	if(m_eMode == MODE_RATE) {
		if(iButton == 2) {
			m_iNewRating--;
		} else if(iButton == 3) {
			m_iNewRating++;
		}
		if(m_iNewRating < 0) {
			m_iNewRating = 0;
		} else if(m_iNewRating > 10) {
			m_iNewRating = 10;
		}
		updateRating();
	} else {
		if(m_ePlaybackState != STATE_PLAYING || m_currentTrack.bStream) {
			return;
		}

		if(iButton == 2) {
			m_iNewPosition-=2;
		} else if(iButton == 3) {
			m_iNewPosition+=2;
		}
		if(m_iNewPosition < 0) {
			m_iNewPosition = 0;
		} else if(m_iNewPosition > m_currentTrack.iLength) {
			m_iNewPosition = m_currentTrack.iLength;
		}
		updatePosition();
	}
}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CPlayingScreen::OnLCDButtonUp(int iButton) {
	if(m_eMode == MODE_RATE) {
		m_Controller->setRating(m_iNewRating);
		m_iNewRating = -1;
	} else {
		if(m_ePlaybackState != STATE_PLAYING || m_currentTrack.bStream) {
			return;
		}
		m_Controller->setPosition(m_iNewPosition);
		m_iNewPosition = -1;
	}
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CPlayingScreen::OnActivation() {
}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CPlayingScreen::OnDeactivation() {
	
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CPlayingScreen::OnExpiration() {
}

// Called when the playback status has changed
void CPlayingScreen::OnPlaybackStateChanged(EPlaybackState eStatus) {
	m_ePlaybackState = eStatus;
	configureScreen();
}

// Called when the connection to helium has changed
void CPlayingScreen::OnConnectionChanged(bool bConnected) {
	configureScreen();
}

// Called when the current track has changed
void CPlayingScreen::OnTrackChanged(SPLAYINGITEM currentTrack) {
	m_currentTrack = currentTrack;
	
	bool bDurationBar = m_DurationBar.IsVisible();

	if(m_currentTrack.bStream && m_eMode == MODE_RATE) {
		m_eMode = MODE_VIEW;
		SetButtonBitmap(2,IDB_REW);
		SetButtonBitmap(3,IDB_FFWD);
	}

	updateCurrentTrack();
	configureScreen();

	if(CAppletManager::GetInstance()->GetActiveScreen() == this) {
		RECT transRect;
		SetRect(&transRect,0,0,GetWidth(),m_DurationBar.IsVisible() == bDurationBar?28:36);

		CAppletManager::GetInstance()->StartTransition(TRANSITION_MORPH,&transRect);
	}
}

// Called when a list item has been received
void CPlayingScreen::OnListItemReceived(SLISTITEM item) {

}

void CPlayingScreen::OnPlaylistChanged(int size) {
}

void CPlayingScreen::OnRatingChanged(int iRating) {
	m_currentTrack.iRating = iRating;
	updateRating();
}

void CPlayingScreen::updateCurrentTrack() {
	//if(m_currentTrack.bStream) {
	//	m_Label.SetText(m_currentTrack.strAlbum);
	//	m_Label2.SetText(m_currentTrack.strArtist);
	//} else {
		m_Label.SetText(m_currentTrack.strArtist);
		m_Label2.SetText(m_currentTrack.strTitle);
	//}
}

void CPlayingScreen::updatePosition() {
	if(m_ePlaybackState != STATE_PLAYING) {
		return;
	}

	int iDuration = m_iNewPosition != -1? m_iNewPosition : m_Controller->getPosition();
	int min = iDuration / 60, min2 = m_currentTrack.iLength / 60;
	int sec = iDuration % 60, sec2 = m_currentTrack.iLength % 60;
	
	if(iDuration >= 0) {
		m_DurationBar.ScrollTo(iDuration);

		m_PositionLabel.SetText(tstringprintf(_T("%02i:%02i"),min,sec));
	} else {
		m_PositionLabel.SetText(_T("00:00"));
	}
	
	if(m_StreamDurationLabel.IsVisible()) {
		m_StreamDurationLabel.SetText(tstringprintf(_T("%02i:%02i"),min,sec));
	}

	if(m_currentTrack.iLength >= 0) {
		m_DurationLabel.SetText(tstringprintf(_T("%02i:%02i"),min2,sec2));
	} else {
		m_DurationLabel.SetText(_T("00:00"));
	}
	
	int pos = m_Controller->getListPosition() + 1;
	int size = m_Controller->getListSize();
	if(size >= 0) {
		m_ListPositionLabel.SetText(tstringprintf(_T("%i/%i"),pos,size));
	} else {
		m_ListPositionLabel.SetText(_T("-/-"));
	}
}

void CPlayingScreen::configureScreen() {
	// Duration controls
	bool bDefaultDuration = m_eMode != MODE_RATE && (m_ePlaybackState != STATE_STOPPED) && !m_currentTrack.bEmpty && !m_currentTrack.bStream;
	bool bStreamDuration = m_eMode != MODE_RATE && (m_ePlaybackState != STATE_STOPPED) && !m_currentTrack.bEmpty && m_currentTrack.bStream;

	m_DurationBar.SetRange(0,m_currentTrack.iLength);
	m_StreamDurationLabel.Show(bStreamDuration);

	m_DurationLabel.Show(bDefaultDuration);
	m_PositionLabel.Show(bDefaultDuration);
	m_DurationBar.Show(bDefaultDuration);

	if(m_eMode == MODE_VIEW) {
		if(m_ePlaybackState == STATE_PAUSED){
			m_StatusBitmap.Show(true);
			m_StreamDurationLabel.Show(false);
		} else {
			m_StatusBitmap.Show(false);
			if(m_ePlaybackState == STATE_PLAYING && !m_currentTrack.bEmpty && m_currentTrack.bStream) {
				m_StreamDurationLabel.Show(true);
			}
		}
	}
	
	// Rating controls
	for(int i=0;i<5;i++) {
		m_StarBitmap[i].Show(m_eMode == MODE_RATE);
	}
	updateRating();

	// track controls
	if(!m_Controller->isConnected()) {
		m_Label.SetText(_T("Helium is not running"));
		m_Label2.SetText(_T("No Data available"));
		m_ListPositionLabel.SetText(_T("-/-"));
	} else if(m_ePlaybackState == STATE_STOPPED) {
		m_Label.SetText(_T(""));
		m_Label2.SetText(_T(""));
	}

	updatePosition();
	m_updateTimer.Start();
}

void CPlayingScreen::updateRating() {
	int rating = m_iNewRating != -1?m_iNewRating:m_currentTrack.iRating;
	for(int i=0;i<5;i++) {
		if(rating >= (i+1)*2) {
			m_StarBitmap[i].SetBitmap(m_hStarBitmap);
		} else if(rating > i*2) {
			m_StarBitmap[i].SetBitmap(m_hStarBitmap3);
		} else {
			m_StarBitmap[i].SetBitmap(m_hStarBitmap2);
		}
	}
}