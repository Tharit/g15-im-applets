#ifndef _CPLAYINGSCREEN_H_
#define _CPLAYINGSCREEN_H_

#include "CScreen.h"
#include "CLCDLabel.h"
#include "CLCDBar.h"
#include "CHeliumController.h"
#include "CHeliumObserver.h"
#include "CLCDBitmap.h"

class CPlayingScreen : public CScreen,public CHeliumObserver
{
private:
	int m_iNewRating;
	int	m_iNewPosition;
	enum {MODE_VIEW,MODE_RATE} m_eMode;
	CHeliumController* m_Controller;

	CTimeSys m_updateTimer;
	CTimeSys m_modeTimer;

	CLCDLabel m_Label,m_Label2;
	CLCDLabel m_PositionLabel,m_DurationLabel,m_StreamDurationLabel;
	CLCDLabel m_ListPositionLabel;
	CLCDBar m_DurationBar;
	CLCDBitmap m_StatusBitmap;
	CLCDBitmap m_StarBitmap[5];

	HBITMAP m_hStatusBitmap;
	HBITMAP m_hStarBitmap;
	HBITMAP m_hStarBitmap2;
	HBITMAP m_hStarBitmap3;

	SPLAYINGITEM m_currentTrack;
	EPlaybackState m_ePlaybackState;

	void updateCurrentTrack();
	void configureScreen();
	void updatePosition();
	void updateRating();
public:
	// Constructor
	CPlayingScreen();
	// Destructor
	~CPlayingScreen();

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