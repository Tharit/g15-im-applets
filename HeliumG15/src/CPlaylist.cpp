#include "Stdafx.h"
#include "CPlaylist.h"

CPlaylist::CPlaylist() {
	m_iStart = -1;
	m_iEnd = -1;
	m_iPos = 0;
	m_iListSize = 0;
}

CPlaylist::~CPlaylist() {

}

bool CPlaylist::Initialize() {
	if(!CLCDList::Initialize()) {
		return false;
	}
	CHeliumController::getInstance()->registerObserver(this);
	
	HINSTANCE hInstance = GetModuleHandle(NULL);

	m_hStatusBitmaps[0] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_PLAYING),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	m_hStatusBitmaps[1] = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_PAUSED),
												IMAGE_BITMAP,5, 5, LR_MONOCHROME);
	
	m_bFocusCurrentTrack = false;

	m_iCurrentTrack = CHeliumController::getInstance()->getListPosition();
	m_ePlaybackState =  CHeliumController::getInstance()->getPlaybackState();
	return true;
}

void CPlaylist::DrawEntry(CLCDGfx *pGfx,SLISTITEM* pEntry,bool bSelected) {
	SelectObject(pGfx->GetHDC(),m_hFont);
	
	tstring item = tstringprintf(_T("%i "),pEntry->iPos+1);
	
	if(pEntry->iLength == 0) {
		item += _T("(Webradio) ") + pEntry->strTitle;
	} else {
		item += pEntry->strArtist + _T(" - ") + pEntry->strTitle;
	}

	if(pEntry->iPos == m_iCurrentTrack) {
		pGfx->DrawBitmap(1,2,5,5,m_hStatusBitmaps[m_ePlaybackState == STATE_PLAYING?0:1]);
	}

	pGfx->DrawText(6,0,pGfx->GetClipWidth(),item);
	if(bSelected) {
		RECT invert = { 0,0,GetWidth(),m_iFontHeight};
		InvertRect(pGfx->GetHDC(),&invert);
	}
}

int	CPlaylist::GetEntryHeight(SLISTITEM* pEntry,bool bSelected) {
	return m_iFontHeight;
}


void CPlaylist::DrawGroup(CLCDGfx *pGfx,tstring strValue,bool bOpen,bool bSelected) {
	SelectObject(pGfx->GetHDC(),m_hFont);
	pGfx->DrawText(10,0,strValue.c_str());
	
	if(bOpen)
		pGfx->DrawText(1,0,_T("-"));
	else
		pGfx->DrawText(1,0,_T("+"));
	
	RECT invert = { 1,4,8,m_iFontHeight-2};
	InvertRect(pGfx->GetHDC(),&invert);

	if(bSelected)
	{
		RECT invert2 = { 0,0,GetWidth(),m_iFontHeight};
		InvertRect(pGfx->GetHDC(),&invert2);
	}
}

int	CPlaylist::GetGroupHeight(tstring strValue,bool bOpen,bool bSelected) {
	return m_iFontHeight;
}

void CPlaylist::DeleteEntry(SLISTITEM* pItem) {
	delete pItem;
}

void CPlaylist::updateList(int size) {
	if(size >= 0) {
		m_iListSize = size;
	}
	
	m_iCurrentTrack = CHeliumController::getInstance()->getListPosition();

	if(m_bFocusCurrentTrack) {
		m_iPos = m_iCurrentTrack;
	}

	if(!m_Position || m_Position->GetIndex()+m_iStart >= m_iListSize) {
		m_iStart = 0;
	} else {
		m_iStart = m_iPos-3;
	}
	if(m_iStart < 0) {
		m_iStart = 0;
	}
	if(m_iPos < 0) {
		m_iPos = 0;
	}
	m_iEnd = m_iStart-1;
	
	Clear();
	TRACE(_T("request list from %i to %i!\n"),m_iStart,m_iStart+5);
	for(int i=m_iStart;i<m_iStart+6 && i < m_iListSize;i++) {
		CHeliumController::getInstance()->requestListItem(this,i);
	}
}

void CPlaylist::OnTrackChanged(SPLAYINGITEM currentTrack) {
	updateList();
}

void CPlaylist::OnConnectionChanged(bool bConnected) {
}

void CPlaylist::OnRatingChanged(int iRating) {
}

void CPlaylist::OnPlaybackStateChanged(EPlaybackState eStatus) {
	m_ePlaybackState = eStatus;
}

void CPlaylist::OnPlaylistChanged(int size) {
	TRACE(_T("playlist changed: %i\n"),size);
	updateList(size);
}

void CPlaylist::OnListItemReceived(SLISTITEM item) {
	SLISTITEM* pItem = new SLISTITEM();
	pItem->iLength = item.iLength;
	pItem->iRating = item.iRating;
	pItem->iPos = item.iPos;
	pItem->strTitle = item.strTitle;
	pItem->strArtist = item.strArtist;
	
	CListEntry<SLISTITEM*> *pListItem = NULL;

	if(item.iPos == m_iStart - 1) {
		pListItem = InsertItem(begin(),pItem);
		m_iStart--;
	} else if(item.iPos == m_iEnd + 1) {
		pListItem = AddItem(pItem);
		m_iEnd++;
		if(m_iStart < 0) {
			m_iStart = 0;
		}
	} else {
		delete pItem;
		return;
	}
	if((m_bFocusCurrentTrack && item.iPos == m_iCurrentTrack) || (!m_bFocusCurrentTrack && item.iPos == m_iPos)) {
		SetPosition(pListItem);
	}
}

bool CPlaylist::ScrollDown() {
	if(m_Position && m_Position->GetIndex() == this->GetEntryCount() - 1 && m_iEnd + 1 < m_iListSize) {
		CListItem<SLISTITEM*>* pItem = (CListItem<SLISTITEM*>*)(*begin());
		this->RemoveItem(pItem->GetItemData());
		m_iStart++;
		CHeliumController::getInstance()->requestListItem(this,m_iEnd+1);
		
	}
	bool bRes = CLCDList::ScrollDown();
	if(bRes) {
		m_bFocusCurrentTrack = false;
		m_iPos = m_Position->GetIndex()+m_iStart;
	}
	return bRes;
}

bool CPlaylist::ScrollUp() {
	if(m_Position && m_Position->GetIndex() == 1 && m_iStart - 1 >= 0) {
		CListItem<SLISTITEM*>* pItem = (CListItem<SLISTITEM*>*)(*(--end()));
		this->RemoveItem(pItem->GetItemData());
		m_iEnd--;
		CHeliumController::getInstance()->requestListItem(this,m_iStart-1);
	}

	bool bRes = CLCDList::ScrollUp();
	if(bRes) {
		m_bFocusCurrentTrack = false;
		m_iPos = m_Position->GetIndex()+m_iStart;
	}
	return bRes;
}

SLISTITEM* CPlaylist::getSelectedItem() {
	CListItem<SLISTITEM*>* pListItem = (CListItem<SLISTITEM*>*)GetSelectedEntry();
	if(pListItem != NULL) {
		SLISTITEM *pItem = pListItem->GetItemData();
		return pItem;
	}
	return NULL;
}

void CPlaylist::setPositionByNumber(int number) {
	m_iPos = number;
	updateList();
}

void CPlaylist::focusCurrentTrack() {
	m_bFocusCurrentTrack = true;
}