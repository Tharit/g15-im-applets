#ifndef _CCONTACTLIST_H_
#define _CCONTACTLIST_H_

#include "CLCDList.h"
#include "CContactData.h"

class CContactListGroup
{
public:
	tstring strName;
	tstring strPath;
	int iMembers;
	int iGroups;
	int iOnline;
	bool bIsMetacontact;
	bool bIsContactlist;
	CContactData *pMetaContact;
};

class CContactList : public CLCDList<CContactData*,CContactListGroup*>
{
public:
	// returns the instance
	static CContactList *GetInstance();

	// constructor
	CContactList();
	// destructor
	~CContactList();
	
	// initializes the list
	bool Initialize();
	// deinitializes the list
	bool Shutdown();

	// regenerates the list
	void RegenerateList();

	// called when a contact has been added
	void OnContactAdded(CContactData *pContactData);
	// called when a contact has been deleted
	void OnContactDeleted(CContactData *pContactData);
	
	// called when a contacts status has changed
	void OnStatusChange(CContactData *pContactData,tstring strOldStatus);

	// called when the configuration has changed
	void OnConfigChanged();
	
	// Called to compare two entrys
	static bool CompareEntries(CListEntry<CContactData*,CContactListGroup*> *pLeft,CListEntry<CContactData*,CContactListGroup*> *pRight);

	void SetPosition(CListEntry<CContactData*,CContactListGroup*> *pEntry);
	bool ScrollUp();
	bool ScrollDown();
protected:	
	// returns the group string for a contact
	static tstring GetContactGroupPath(CContactData *pContactData);

	// converts a status string to a priority number
	int GetStatusPriority(tstring strStatus);

	// checks if a contact is visible
	bool IsVisible(CContactData *pContactData);

	// changes the groupobjects member/online counters
	void ChangeGroupObjectCounters(tstring strGroup,int iMembers,int iOnline=0);
	
	// Adds a contact
	void AddContact(CContactData *pContactData);
	// Removes a contact
	void RemoveContact(CContactData *pContactData,tstring strOldStatus);

	// Adds a group
	CListContainer<CContactData*,CContactListGroup*> *AddGroupByString(tstring strGroup);
	// get group by string
	CListContainer<CContactData*,CContactListGroup*> *GetGroupByString(tstring strGroup);
	// tries to find a group in the specified group
	CListContainer<CContactData*,CContactListGroup*> *FindGroupInGroup(tstring strGroup,CListContainer<CContactData*,CContactListGroup*> *pGroup);
	// returns the entry for the specified group name
	CListContainer<CContactData*,CContactListGroup*> *FindGroup(tstring strGroup);

	// tries to find a contact in the list
	CListItem<CContactData*,CContactListGroup*> *CContactList::FindContact(CContactData *pContactData);
	// tries to find a contact in the specified group
	CListItem<CContactData*,CContactListGroup*> *FindContactInGroup(CContactData *pContactData,CListContainer<CContactData*,CContactListGroup*> *pGroup);

	// Group object functions
	CContactListGroup *GetGroupObjectByPath(tstring strPath);
	CContactListGroup *CreateGroupObjectByPath(tstring strPath);
	void DeleteGroupObjectByPath(tstring strPath);
	void InitializeGroupObjects();
	void UninitializeGroupObjects();

	// Called to delete the specified item
	void DeleteItem(CContactData *pEntry);
	// Called to delete the specified group
	void DeleteGroup(CContactListGroup* pGroup);
	// Called to draw the specified group
	void DrawGroup(CLCDGfx *pGfx,CContactListGroup* pGroup,bool bOpen,bool bSelected);
	// Called to draw the specified entry
	void DrawEntry(CLCDGfx *pGfx,CContactData *pEntry,bool bSelected);
	
	DWORD m_dwLastScroll;
	static CContactList *m_pInstance;
	vector<CContactListGroup*> m_Groups;
	bool m_bGroupedMode;
};

#endif