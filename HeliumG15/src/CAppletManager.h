#ifndef _COUTPUTMANAGER_H_
#define _COUTPUTMANAGER_H_

#include "CLCDOutputManager.h"

#include "CListScreen.h"
#include "CPlayingScreen.h"
#include "CNotificationScreen.h"
#include "CLCDConnectionLogitech.h"

#include "CHeliumController.h"

class CAppletManager : public CLCDOutputManager, CHeliumObserver
{
private:
	CPlayingScreen m_PlayingScreen;
	CListScreen m_ListScreen;
	CNotificationScreen m_NotificationScreen;
	CHeliumController* m_pController;
	
	void OnScreenExpired(CLCDScreen *pScreen);
	CLCDScreen* m_pLastScreen;
public:
	// returns the AppletManager's instance
	static CAppletManager *GetInstance();

	// Constructor
	CAppletManager();
	// Destructor
	~CAppletManager();

	// Initializes the AppletManager
	bool Initialize(tstring strAppletName,bool bAutostart=false,bool bConfigDialog=false);
	// Deinitializes the AppletManager
	bool Shutdown();
	// Updates the AppletManager
	bool Update();

	void activateListScreen();
	void activatePlayingScreen();
	void activateNotificationScreen();

	// Called when a config dialog is requested
	DWORD OnConfigDialogRequest(int connection, const PVOID pContext);

	void OnTrackChanged(SPLAYINGITEM currentTrack);
	void OnConnectionChanged(bool bConnected);
	void OnPlaybackStateChanged(EPlaybackState eStatus);
	void OnListItemReceived(SLISTITEM item);
	void OnPlaylistChanged(int size);
	void OnRatingChanged(int iRating);
};

#endif