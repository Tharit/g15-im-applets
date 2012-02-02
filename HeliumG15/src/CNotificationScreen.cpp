#include "stdafx.h"
#include "CNotificationScreen.h"
#include "CAppletManager.h"


//************************************************************************
// Constructor
//************************************************************************
CNotificationScreen::CNotificationScreen() {
}

//************************************************************************
// Destructor
//************************************************************************
CNotificationScreen::~CNotificationScreen() {
}

//************************************************************************
// Initializes the screen 
//************************************************************************
bool CNotificationScreen::Initialize() {
	if(!CScreen::Initialize())
		return false;
	
	
	m_Controller = CHeliumController::getInstance();
	m_Controller->registerObserver(this);

	m_Label.Initialize();
	m_Label2.Initialize();

	m_Label.SetAlignment(DT_CENTER);
	m_Label.SetFontFaceName(_T("Microsoft Sans Serif"));
	m_Label.SetFontPointSize(8);
	m_Label.SetFontWeight(FW_BOLD);
	m_Label.SetOrigin(0,7);
	m_Label.SetSize(GetWidth(),13);

	m_Label2.SetAlignment(DT_CENTER);
	m_Label2.SetText(_T(""));
	m_Label2.SetFontFaceName(_T("Microsoft Sans Serif"));
	m_Label2.SetFontPointSize(8);
	m_Label2.SetOrigin(0,20);
	m_Label2.SetSize(GetWidth(),13);

	AddObject(&m_Label);
	AddObject(&m_Label2);
	return true;
}

//************************************************************************
// Shutdown the scren
//************************************************************************
bool CNotificationScreen::Shutdown() {
	if(!CScreen::Shutdown())
		return false;

	return true;
}

//************************************************************************
// Updates the screen
//************************************************************************
bool CNotificationScreen::Update() {
	if(!CScreen::Update())
		return false;
	
	
	
	return true;
}

//************************************************************************
// Resets the credits screens state
//************************************************************************
void CNotificationScreen::Reset() {
	
}


//************************************************************************
// Draws the screen
//************************************************************************
bool CNotificationScreen::Draw(CLCDGfx *pGfx) {
	if(!CScreen::Draw(pGfx))
		return false;
	
	return true;
}


//************************************************************************
// Called when the screen size has changed
//************************************************************************
void CNotificationScreen::OnSizeChanged() {
	CScreen::OnSizeChanged();
}

//************************************************************************
// Called when an LCD-button is pressed
//************************************************************************
void CNotificationScreen::OnLCDButtonDown(int iButton){
	
}

//************************************************************************
// Called when an LCD-button event is repeated
//************************************************************************
void CNotificationScreen::OnLCDButtonRepeated(int iButton) {

}

//************************************************************************
// Called when an LCD-button is released
//************************************************************************
void CNotificationScreen::OnLCDButtonUp(int iButton) {
	
}

//************************************************************************
// Called when the screen is activated
//************************************************************************
void CNotificationScreen::OnActivation() {

}

//************************************************************************
// Called when the screen is deactivated
//************************************************************************
void CNotificationScreen::OnDeactivation() {
	
}

//************************************************************************
// Called when the screen has expired
//************************************************************************
void CNotificationScreen::OnExpiration() {
}

// Called when the playback status has changed
void CNotificationScreen::OnPlaybackStateChanged(EPlaybackState eStatus) {
}

// Called when the current track has changed
void CNotificationScreen::OnTrackChanged(SPLAYINGITEM currentTrack) {
	//if(currentTrack.bStream) {
	//	m_Label.SetText(currentTrack.strAlbum);
	//	m_Label2.SetText(currentTrack.strArtist);
	//} else {
		m_Label.SetText(currentTrack.strArtist);
		m_Label2.SetText(currentTrack.strTitle);
	//}
}

// Called when the connection to helium has changed
void CNotificationScreen::OnConnectionChanged(bool bConnected) {
}

void CNotificationScreen::OnListItemReceived(SLISTITEM item) {
	
}

void CNotificationScreen::OnRatingChanged(int iRating) {
}

void CNotificationScreen::OnPlaylistChanged(int size) {
}