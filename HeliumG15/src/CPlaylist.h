#ifndef _CPLAYLIST_H_
#define _CPLAYLIST_H_

#include "CHeliumController.h"
#include "CHeliumObserver.h"
#include "CLCDList.h"

class CPlaylist : public CLCDList<SLISTITEM*>, public CHeliumObserver
{
private:
	int m_iStart,m_iPos,m_iEnd;
	int m_iListSize;
	int m_iCurrentTrack;
	bool m_bFocusCurrentTrack;

	HBITMAP m_hStatusBitmaps[2];
	EPlaybackState m_ePlaybackState;

	void updateList(int size = -1);
protected:
	void DeleteEntry(SLISTITEM* pItem);

	void DrawEntry(CLCDGfx *pGfx,SLISTITEM* pEntry,bool bSelected);
	int	GetEntryHeight(SLISTITEM* pEntry,bool bSelected);
	void DrawGroup(CLCDGfx *pGfx,tstring strValue,bool bOpen,bool bSelected);
	int	GetGroupHeight(tstring strValue,bool bOpen,bool bSelected);

public:	
	CPlaylist();
	~CPlaylist();
	
	bool Initialize();

	bool ScrollUp();
	bool ScrollDown();

	void OnTrackChanged(SPLAYINGITEM currentTrack);
	void OnConnectionChanged(bool bConnected);
	void OnPlaybackStateChanged(EPlaybackState eStatus);
	void OnListItemReceived(SLISTITEM item);
	void OnPlaylistChanged(int size);
	void OnRatingChanged(int iRating);

	SLISTITEM* getSelectedItem();
	void setPositionByNumber(int number);
	void focusCurrentTrack();
};
#endif