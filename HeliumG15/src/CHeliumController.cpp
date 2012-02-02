#include "stdafx.h"
#include "CHeliumController.h"

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CHeliumController::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= _T("HeliumG15");
	wcex.lpszClassName	= _T("HeliumG15");
	wcex.hIconSm		= 0;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}
CHeliumController* CHeliumController::m_pInstance = NULL;

CHeliumController* CHeliumController::getInstance() {
	if(m_pInstance == NULL) {
		m_pInstance = new CHeliumController();
	}
	return m_pInstance;
}

CHeliumController::CHeliumController() {
	m_initTimer.Start();
	m_playbackState = STATE_STOPPED;
	m_pInstance = this;
	m_currentTrack.bStream = false;
	m_hwndHelium = NULL;
	
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MyRegisterClass(hInstance);
	m_hwndWindow = CreateWindow(_T("HeliumG15"), _T("HeliumG15"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	// start the update timer
	m_uiTimer = SetTimer(0,0,1000/10,CHeliumController::updateTimer);
	m_updateTimer.Start();
	
	updateConnection();
	updatePlaybackState();
	updateCurrentTrack();
}

CHeliumController::~CHeliumController() {
	KillTimer(m_hwndWindow,m_uiTimer);
}

void CHeliumController::update() {
	if(!isConnected()) {
		updateConnection();
	} else {
		// update the current track
		if(m_updateTimer.GetSpan() > 1.0) {
			m_updateTimer.Start();
			updateList();
			if(m_currentTrack.bStream) {
				updateCurrentTrack();
			}
		}
	}
}

// update callback
VOID CALLBACK CHeliumController::updateTimer(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime) {
	CHeliumController* controller = CHeliumController::getInstance();
	if(controller) {
		controller->update();
	}
}

// update current track
void CHeliumController::updateCurrentTrack() {
	m_iRequestState = REQUEST_CURRENT;
	sendIPCMessage(IPC_GET_CURRENT_TRACK);
}

// update playlist information
void CHeliumController::updateList() {
	int size = getListSize();
	if(size == -1) {
		return;
	}
	if(m_iPlaylistSize != size) {
		m_iPlaylistSize = size;
		list<CHeliumObserver*>::iterator iter = m_ObserverList.begin();
		for(;iter != m_ObserverList.end();iter++) {
			(*iter)->OnPlaylistChanged(m_iPlaylistSize);
		}
	}
}

// update playback state
void CHeliumController::updatePlaybackState() {
	int iPlaybackState = sendIPCMessage(IPC_GET_PLAYER_STATE);

	switch(iPlaybackState) {
		case 0:
			m_playbackState = STATE_STOPPED;
			break;
		case 1:
			m_playbackState = STATE_PLAYING;
			break;
		case 2:
			m_playbackState = STATE_PAUSED;
			break;
	}
	list<CHeliumObserver*>::iterator iter = m_ObserverList.begin();
	for(;iter != m_ObserverList.end();iter++) {
		(*iter)->OnPlaybackStateChanged(m_playbackState);
	}
}

// searches for the helium window
void CHeliumController::updateConnection() {
	HWND window = FindWindow(_T("THeliumMainForm"), NULL);
	bool bChanged = false;
	if((window && !m_hwndHelium) ||(!window && m_hwndHelium)) {
		bChanged = true;
	}
	m_hwndHelium = window;
	// if a new helium window was found, check if it initialized
	if(m_hwndHelium && bChanged) {
		if(!m_startupTimer.isActive() || m_startupTimer.GetSpan() >= 1) {
			m_startupTimer.Start();
			int res = sendIPCMessage(IPC_SET_CALLBACK_HWND,(WPARAM)m_hwndWindow);
		
			if(res != 1) {
				m_hwndHelium = NULL;
				return;
			}
		} else {
			m_hwndHelium = NULL;
			return;
		}
	}
	
	// notify observers of the change
	if(bChanged) {
		list<CHeliumObserver*>::iterator iter = m_ObserverList.begin();
		for(;iter != m_ObserverList.end();iter++) {
			(*iter)->OnConnectionChanged(true);
		}
	}
}

// updates information for the currently playing track
void CHeliumController::parseCurrentTrack(char* szMessage,int len) {
	SPLAYINGITEM info;

	int i=0;
	int field = 0;
	int start = 0;
	char *szField = NULL;
	while(i < len) {
		if(szMessage[i] == 9 || i == len -1) {
			
			if(i-start != 0) {
				szField = (char*)malloc((i-start)+1);
				strncpy(szField,szMessage+start,i-start);
				szField[i-start] = 0;
			} else {
				szField = (char*)malloc(2);
				strncpy(szField,"",1);
				szField[1] = 0;
			}
			switch(field) {
			case 0: info.strTitle		= toWideString(szField);	break;
			case 1: info.strArtist		= toWideString(szField);	break;
			case 2: info.strAlbum		= toWideString(szField);	break;
			case 3: info.strGenre		= toWideString(szField);	break;
			case 4: info.iYear			= atoi(szField);			break;
			case 5: info.strComment		= toWideString(szField);	break;
			case 6: info.iTrack			= atoi(szField);			break;				
			case 7: info.iLength		= atoi(szField);			break;
			case 8: info.strPath		= toWideString(szField);	break;
			case 9: info.iRating		= atoi(szField);			break;
			case 10: info.strComposer	= toWideString(szField);	break;
			case 11: info.strLyricist	= toWideString(szField);	break;
			case 12: info.strConductor	= toWideString(szField);	break;
			case 13: info.strProducer	= toWideString(szField);	break;
			case 14: info.strCopyright	= toWideString(szField);	break;
			}
				
			free(szField);
			start = i+1;
			field++;
		}
		i++;
	}
	
	// adjust info for streams
	if(!info.strPath.empty() && info.iLength == 0) {
		info.strStreamTitle = info.strArtist;
		int pos = info.strTitle.find(L" - ");
		info.strArtist = info.strTitle.substr(0,pos);
		info.strTitle = info.strTitle.substr(pos+3);
		info.bStream = true;
	} else {
		info.bStream = false;
	}
	if(info.strPath.empty() && !info.bStream) {
		info.bEmpty = true;
	} else {
		info.bEmpty = false;
	}

	if(m_currentTrack != info) {
		m_currentTrack = info;
		
		TRACE(_T("Current Track changed!\n"));
		if(m_currentTrack.bStream) {
			TRACE(_T("Now Playing: (STREAM)%s\n"),m_currentTrack.strStreamTitle.c_str());
		} else {
			TRACE(_T("Now Playing: %s - %s\n"),m_currentTrack.strArtist.c_str(),m_currentTrack.strTitle.c_str());
		}
		list<CHeliumObserver*>::iterator iter = m_ObserverList.begin();
		for(;iter != m_ObserverList.end();iter++) {
			(*iter)->OnTrackChanged(m_currentTrack);
		}
	}
	m_iRequestState &= (~REQUEST_CURRENT);
}

// updates information for the currently playing track
void CHeliumController::parseListItem(char* szMessage,int len) {
	ASSERT(!m_RequestQueue.empty());

	SLISTITEMREQUEST request = m_RequestQueue.front();
	m_RequestQueue.pop();

	SLISTITEM info;
	info.iPos = request.pos;

	int i=0;
	int field = 0;
	int start = 0;
	while(i < len) {
		if(szMessage[i] == 9 || i == len -1) {
			if(i-start != 0) {
				char* szField = (char*)malloc((i-start)+1);
				strncpy(szField,szMessage+start,i-start);
				szField[i-start] = 0;

				switch(field) {
				case 0: info.strArtist	= toWideString(szField);	break;
				case 1: info.strTitle	= toWideString(szField);	break;
				case 2: info.iLength	= atoi(szField);			break;
				case 3: info.strPath	= toWideString(szField);	break;
				case 4: info.iRating	= atoi(szField);			break;
				}
				
				free(szField);
			}
			start = i+1;
			field++;
		}
		i++;
	}
	
	if(request.observer != NULL) {
		request.observer->OnListItemReceived(info);
	}
}

LRESULT CALLBACK CHeliumController::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	CHeliumController* manager = CHeliumController::getInstance();

	switch (message) 
	{
	case WM_USER: {
		switch(lParam) {
		case IPC_TRACK_CHANGED_NOTIFICATION: {
			manager->updateCurrentTrack();
			break;
		}
		case IPC_PLAYER_STATE_CHANGED_NOTIFICATION: {
			manager->updatePlaybackState();
			break;
		}
		case IPC_RATING_CHANGED_NOTIFICATION : {
			list<CHeliumObserver*>::iterator iter = manager->m_ObserverList.begin();
			for(;iter != manager->m_ObserverList.end();iter++) {
				(*iter)->OnRatingChanged(wParam);
			}
			break;
		}
		case IPC_SHUTDOWN_NOTIFICATION: {
			// currently not used
			break;
		}
		}
		break;
	}
	case WM_COPYDATA: {
		PCOPYDATASTRUCT copyData = (PCOPYDATASTRUCT)lParam;
		switch(copyData->dwData) {
		case IPC_GET_CURRENT_TRACK: {
			manager->parseCurrentTrack((char*)copyData->lpData,copyData->cbData);
			break;
		}
		case IPC_GET_LIST_ITEM: {
			manager->parseListItem((char*)copyData->lpData,copyData->cbData);
			break;
		}
		case IPC_NEW_COVER_NOTIFICATION: {
			// currently not used
			break;
		}
		}
		break;
	}
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// sends a message to helium
int CHeliumController::sendIPCMessage(LPARAM message,WPARAM param) {
	if(m_hwndHelium == NULL) {
		return -1;
	}
	int result = SendMessageW(m_hwndHelium, WM_HMM_IPC, param, message);
	if(GetLastError() == ERROR_INVALID_WINDOW_HANDLE) {
		updateConnection();
	}
	return result;
}

void CHeliumController::registerObserver(CHeliumObserver* pListener) {
	m_ObserverList.push_back(pListener);
}

void CHeliumController::unregisterObserver(CHeliumObserver* pListener) {
	m_ObserverList.remove(pListener);
}

bool CHeliumController::isConnected() {
	return m_hwndHelium != NULL;
}

SPLAYINGITEM CHeliumController::getCurrentTrack() {
	return m_currentTrack;
}

bool CHeliumController::playPause() {
	if(!isConnected()) {
		return false;
	}
	if(sendIPCMessage(IPC_IS_PLAYING) || sendIPCMessage(IPC_IS_PAUSED)) {
		return sendIPCMessage(IPC_PLAYPAUSE) < 0 ? false:true;
	} else {
		return sendIPCMessage(IPC_PLAY) < 0 ? false:true;
	}
}

bool CHeliumController::stop() {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_STOP) < 0 ? false:true;
}

bool CHeliumController::next() {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_NEXT) < 0 ? false:true;
}

bool CHeliumController::prev() {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_PREVIOUS) < 0 ? false:true;
}

EPlaybackState CHeliumController::getPlaybackState() {
	return m_playbackState;
}

int CHeliumController::getPosition() {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_GET_POSITION);
}

bool CHeliumController::setPosition(int position) {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_SET_POSITION,(WPARAM)position) < 0 ? false:true;
}

bool CHeliumController::setVolume(int volume) {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_SET_VOLUME,(WPARAM)volume) < 0 ? false:true;
}

int CHeliumController::getVolume() {
	return sendIPCMessage(IPC_GET_VOLUME);
}

bool CHeliumController::requestListItem(CHeliumObserver* pObserver,int pos) {
	SLISTITEMREQUEST request;
	request.observer = pObserver;
	request.pos = pos;

	m_RequestQueue.push(request);

	return sendIPCMessage(IPC_GET_LIST_ITEM,(WPARAM)pos);
}

bool CHeliumController::setListPosition(int pos) {
	if(!isConnected()) {
		return false;
	}
	return sendIPCMessage(IPC_SET_LIST_POS,(WPARAM)pos) < 0 ? false:true;
}

int CHeliumController::getListPosition() {
	return sendIPCMessage(IPC_GET_LIST_POS);
}

int CHeliumController::getListSize() {
	return sendIPCMessage(IPC_GET_LIST_LENGTH);
}

bool CHeliumController::showHeliumWindow() {
	if(!isConnected()) {
		return false;
	}
	sendIPCMessage(IPC_SHOW_HELIUM_WINDOW);
	return true;
}

bool CHeliumController::showAutoEnqueueOptions() {
	if(!isConnected()) {
		return false;
	}
	sendIPCMessage(IPC_AUTOENQUEUE_OPTIONS);
	return true;
}

bool CHeliumController::closeHelium() {
	if(!isConnected()) {
		return false;
	}
	sendIPCMessage(IPC_CLOSE_HELIUM);
	return true;
}

bool CHeliumController::setRepeat(int mode) {
	if(!isConnected()) {
		return false;
	}
	sendIPCMessage(IPC_GET_REPEAT,mode);
	return true;
}

int CHeliumController::getRepeat() {
	return sendIPCMessage(IPC_GET_REPEAT);
}

bool CHeliumController::setRating(int iRating) {
	if(!isConnected()) {
		return false;
	}
	if(iRating > 10 || iRating < 0) {
		return false;
	}
	sendIPCMessage(IPC_SET_RATING,iRating);
	return true;
}