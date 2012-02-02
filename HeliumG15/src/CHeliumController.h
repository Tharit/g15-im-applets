#ifndef _CHELIUMCONTROLLER_H_
#define _CHELIUMCONTROLLER_H_

#define REQUEST_CURRENT 1
#define REQUEST_LIST 2

enum EPlaybackState {STATE_PLAYING,STATE_PAUSED,STATE_STOPPED};

#include "CHeliumObserver.h"

struct SLISTITEMREQUEST {
	CHeliumObserver *observer;
	int pos;
};

class CHeliumController {
private:
	static CHeliumController* m_pInstance;

	HWND m_hwndHelium;
	HWND m_hwndWindow;
	
	// request queue
	std::queue<SLISTITEMREQUEST> m_RequestQueue;

	// update timer handle
	UINT	m_uiTimer;
	// update timer
	CTimeSys m_updateTimer;
	CTimeSys m_startupTimer;
	CTimeSys m_initTimer;

	// playback status
	EPlaybackState m_playbackState;

	// playlist information
	int m_iPlaylistSize;

	// current track
	SPLAYINGITEM m_currentTrack;

	// request status
	int m_iRequestState;

	// observer list
	list<CHeliumObserver*> m_ObserverList;

	int sendIPCMessage(LPARAM message,WPARAM param = NULL);
	void updateConnection();
	void updatePlaybackState();
	void updateCurrentTrack();
	void updateList();

	void parseCurrentTrack(char* szMessage,int len);
	void parseListItem(char* szMessage,int len);
	
	void update();

	CHeliumController();
	~CHeliumController();
	
public:
	// callback for the update timer
	static VOID CALLBACK updateTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime);
	// window proc callback
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	static CHeliumController* getInstance();
	
	void registerObserver(CHeliumObserver* pObserver);
	void unregisterObserver(CHeliumObserver* pListener);

	bool isConnected();

	// playback control
	SPLAYINGITEM getCurrentTrack();
	bool playPause();
	bool stop();
	bool next();
	bool prev();

	EPlaybackState getPlaybackState();

	int getPosition();
	bool setPosition(int position);

	// volume control
	bool setVolume(int volume);
	int getVolume();

	// active playlist control
	bool requestListItem(CHeliumObserver* pObserver,int pos);
	bool setListPosition(int pos);
	int getListPosition();
	int getListSize();
	
	bool showHeliumWindow();
	bool showAutoEnqueueOptions();
	bool closeHelium();
	bool setRepeat(int mode);
	int getRepeat();
	bool setRating(int iRating);
};

#endif