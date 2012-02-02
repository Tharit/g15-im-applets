#include "stdafx.h"
#include "CHeliumController.h"
#include "CAppletManager.h"

// returns the AppletManager's instance
CAppletManager *CAppletManager::GetInstance() {
	return (CAppletManager*)CLCDOutputManager::GetInstance();
}

// Constructor
CAppletManager::CAppletManager() {
}

// Destructor
CAppletManager::~CAppletManager() {
}

// Initializes the AppletManager
bool CAppletManager::Initialize(tstring strAppletName,bool bAutostart,bool bConfigDialog) {
	if(!CLCDOutputManager::Initialize(strAppletName,bAutostart,bConfigDialog))
		return false;

	CHeliumController::getInstance()->registerObserver(this);

	m_pController = CHeliumController::getInstance();

	// initialize screens
	m_PlayingScreen.Initialize();
	m_ListScreen.Initialize();
	m_NotificationScreen.Initialize();

	AddScreen(&m_PlayingScreen);
	AddScreen(&m_ListScreen);
	AddScreen(&m_NotificationScreen);

	// active the now playing screen
	ActivateScreen(&m_PlayingScreen);
}


// Deinitializes the AppletManager
bool CAppletManager::Shutdown() {
	if(!CLCDOutputManager::Shutdown())
		return false;

	return true;
}

// Updates the AppletManager
bool CAppletManager::Update() {
	if(!CLCDOutputManager::Update())
		return false;

	return true;
}
//************************************************************************
// Called when the active screen has expired
//************************************************************************
void CAppletManager::OnScreenExpired(CLCDScreen *pScreen)
{
	// If the notification screen has expired, activate the last active screen
	if(pScreen == (CLCDScreen*)&m_NotificationScreen)
	{
		ActivateScreen((CLCDScreen*)m_pLastScreen);
	}
}


void CAppletManager::activateListScreen() {
	ActivateScreen(&m_ListScreen);
}

void CAppletManager::activatePlayingScreen() {
	ActivateScreen(&m_PlayingScreen);
}

void CAppletManager::activateNotificationScreen() {
	m_pLastScreen = GetActiveScreen();
	m_NotificationScreen.SetAlert(true);
	m_NotificationScreen.SetExpiration(2500);
	ActivateScreen(&m_NotificationScreen);
}

LRESULT CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) 
	{
	case WM_COMMAND:
	case WM_CLOSE:
		DestroyWindow(hWnd);
		TRACE(_T("window closed!\n"));
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// Called when a config dialog is requested
DWORD CAppletManager::OnConfigDialogRequest(int connection, const PVOID pContext) {
	TRACE(_T("Configuration dialog requested!\n"));
	
	HWND dlg = CreateDialog(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_CONFIGDLG),NULL,(DLGPROC)DlgProc);
	ShowWindow(dlg,true);

	
	MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
	while( msg.message!=WM_QUIT )
    {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
		}
	}
	
	return 0;
}

void CAppletManager::OnTrackChanged(SPLAYINGITEM currentTrack) {
	if(!this->GetLCDConnection()->IsForeground()) {
		if(!currentTrack.bEmpty) {
			activateNotificationScreen();
		}
	}
}

void CAppletManager::OnConnectionChanged(bool bConnected) {
}

void CAppletManager::OnPlaybackStateChanged(EPlaybackState eStatus) {
}

void CAppletManager::OnListItemReceived(SLISTITEM item) {
}

void CAppletManager::OnPlaylistChanged(int size) {
}

void CAppletManager::OnRatingChanged(int iRating) {
}