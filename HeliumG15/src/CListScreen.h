#ifndef _CLISTSCREEN_H_
#define _CLISTSCREEN_H_

#include "CHeliumController.h"
#include "CHeliumObserver.h"
#include "CScreen.h"
#include "CLCDLabel.h"
#include "CPlaylist.h"

class CListScreen : public CScreen,public CHeliumObserver
{
private:
	CHeliumController* m_Controller;
	CPlaylist m_Playlist;
	CLCDLabel m_ListPositionLabel;
	
	bool m_bFocused;

	CTimeSys m_updateTimer;
public:
	// Constructor
	CListScreen();
	// Destructor
	~CListScreen();

	// Initializes the screen 
	bool Initialize();
	// Shutdown the scren
	bool Shutdown();
	// Updates the screen
	bool Update();
	// Draws the screen
	bool Draw(CLCDGfx *pGfx);

	// Called when the configuration has changed
	void OnConfigChanged();
	// Called when the screen size has changed
	void OnSizeChanged();
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

	// Called when the playback status has changed
	void OnPlaybackStateChanged(EPlaybackState eStatus);
	// Called when the current track has changed
	void OnTrackChanged(SPLAYINGITEM currentTrack);
	// Called when the connection to helium has changed
	void OnConnectionChanged(bool bConnected);
	// Called when a list item has been received
	void OnListItemReceived(SLISTITEM item);
	void OnPlaylistChanged(int size);
	void OnRatingChanged(int iRating);
	// Resets the credits screens state
	void Reset();
};

#endif