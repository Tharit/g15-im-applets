#ifndef _CEVENTLOG_H_
#define _CEVENTLOG_H_

#include "CLCDList.h"
#include "CContactData.h"

class CEventLogEntry
{
public:
	CContactData *pContact;
	tstring strValue;
	tstring strTimestamp;
	tm Time;
	int iConnectionID;
	EventType eType;
};

class CEventLog : public CLCDList<CEventLogEntry*>
{
public:
	// constructor
	CEventLog();
	// destructor
	~CEventLog();
	
	// deinitializes the list
	bool Shutdown();
	// adds an entry to the list
	CListItem<CEventLogEntry*> *AddItem(CEventLogEntry *);
	
	void SetPosition(CListEntry<CEventLogEntry*> *pEntry);
	bool ScrollUp();
	bool ScrollDown();

	bool SetFont(LOGFONT &lf);
protected:
	// Called to delete the specified entry
	void DeleteEntry(CEventLogEntry *pEntry);
	// Called to draw the specified entry
	void DrawEntry(CLCDGfx *pGfx,CEventLogEntry *pEntry,bool bSelected);

	DWORD m_dwLastScroll;
};

#endif