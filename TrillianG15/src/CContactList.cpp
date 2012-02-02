#include "stdafx.h"
#include "CConfig.h"
#include "CContactList.h"
#include "CAppletManager.h"

#define USE_GROUPS 1

const int aiStatusPriority[] = {	0,	// ID_STATUS_OFFLINE               40071
							9,	// ID_STATUS_ONLINE                40072
							8,	// ID_STATUS_AWAY                  40073
							1,	// ID_STATUS_DND                   40074
							7,	// ID_STATUS_NA                    40075
							6,	// ID_STATUS_OCCUPIED              40076
							10,	// ID_STATUS_FREECHAT              40077
							9,	// ID_STATUS_INVISIBLE             40078
							8,	// ID_STATUS_ONTHEPHONE            40079
							8 	// ID_STATUS_OUTTOLUNCH            40080
							};

CContactList *CContactList::m_pInstance = NULL;

CContactList *CContactList::GetInstance()
{
	return m_pInstance;
}

//************************************************************************
// constructor
//************************************************************************
CContactList::CContactList()
{
	CContactList::m_pInstance = this;
}

//************************************************************************
// destructor
//************************************************************************
CContactList::~CContactList()
{
}

//************************************************************************
// initializes the list
//************************************************************************
bool CContactList::Initialize()
{
	if(!CLCDList<CContactData*,CContactListGroup*>::Initialize())
		return false;

	InitializeGroupObjects();

	//RefreshList();

	return true;
}

//************************************************************************
// deinitializes the list
//************************************************************************
bool CContactList::Shutdown()
{
	if(!CLCDList<CContactData*,CContactListGroup*>::Shutdown())
		return false;

	UninitializeGroupObjects();

	return false;
}

//************************************************************************
// regenerates the list
//************************************************************************
void CContactList::RegenerateList()
{
	Clear();

	InitializeGroupObjects();
	//RefreshList();
}

//************************************************************************
// Called to draw the specified entry
//************************************************************************
void CContactList::DrawEntry(CLCDGfx *pGfx,CContactData *pEntry,bool bSelected)
{
	int iOffset = 0;
	
	if(CConfig::GetBoolSetting(CLIST_SHOWPROTO) && !CConfig::GetBoolSetting(CLIST_COLUMNS))
	{
		pGfx->DrawText(pGfx->GetClipWidth()-25,0,25,pEntry->strMedium);
		pGfx->DrawText(8,0,pGfx->GetClipWidth()-38,pEntry->strDisplayName);
	}
	else
		pGfx->DrawText(8,0,pGfx->GetClipWidth()-8,pEntry->strDisplayName);

	
	pGfx->DrawBitmap(1,ceil((pGfx->GetClipHeight()-5)/2.0f),5,5,CAppletManager::GetInstance()->GetStatusBitmap(pEntry->strStatus));

	if(bSelected && GetTickCount() - m_dwLastScroll < 1000)
	{
		RECT invert = { 0,0,GetWidth(),m_iFontHeight};
		InvertRect(pGfx->GetHDC(), &invert);
	}
}

//************************************************************************
// Called to draw the specified group
//************************************************************************
void CContactList::DrawGroup(CLCDGfx *pGfx,CContactListGroup *pGroup,bool bOpen,bool bSelected)
{
	char num[10],num2[10];
	itoa(pGroup->iMembers,num,10);
	itoa(pGroup->iOnline,num2,10);

	tstring strText = pGroup->strName;
	int iTextOffset = (pGfx->GetClipHeight() - m_iFontHeight) / 2;
	if(iTextOffset < 0)
		iTextOffset = 0;

	// online/member counter
	if(!pGroup->pMetaContact && CConfig::GetBoolSetting(CLIST_COUNTERS))
		strText = strText + _T(" (") + toTstring(num2).c_str()+ _T("/") + toTstring(num).c_str() + _T(")");

	// status for meta contacts
	if(pGroup->pMetaContact)
		pGfx->DrawBitmap(8,ceil((pGfx->GetClipHeight()-5)/2.0f),5,5,CAppletManager::GetInstance()->GetStatusBitmap(pGroup->pMetaContact->strStatus));
		
	
	// label for contactlists
	if(pGroup->bIsContactlist)
	{	
		pGfx->DrawLine(0,1,pGfx->GetClipWidth(),1);
		pGfx->DrawLine(0,pGfx->GetClipHeight()-2,pGfx->GetClipWidth(),pGfx->GetClipHeight()-2);

		pGfx->DrawText(pGfx->GetClipWidth()-50,iTextOffset,_T("Contactlist"));
	}
	else
	{
		// group name
		int iOffset = !pGroup->pMetaContact?8:15;
		pGfx->DrawText(iOffset,iTextOffset,pGfx->GetClipWidth()-iOffset,strText.c_str());
	}
	// expander icon
	if(bOpen)
		pGfx->DrawText(1,iTextOffset,_T("-"));
	else
		pGfx->DrawText(1,iTextOffset,_T("+"));
	
	// selection rectangle
	if(bSelected && GetTickCount() - m_dwLastScroll < 1000)
	{
		RECT invert2 = { 0,0,GetWidth(),pGfx->GetClipHeight()};
		InvertRect(pGfx->GetHDC(),&invert2);
	}

}

//************************************************************************
// converts a status string to a priority number
//************************************************************************
int CContactList::GetStatusPriority(tstring strStatus)
{
	strStatus = toLower(strStatus);

	if(strStatus == _T("online"))
		return 10;
	if(strStatus == _T("free for chat"))
		return 9;
	if(strStatus == _T("invisible"))
		return 8;
	if(strStatus == _T("away"))
		return 7;
	if(strStatus == _T("occupied"))
		return 6;
	if(strStatus == _T("not available"))
		return 5;
	if(strStatus == _T("dnd"))
		return 4;
	if(strStatus == _T("offline"))
		return 0;
	return 1;
}

//************************************************************************
// Called to compare two entrys
//************************************************************************
bool CContactList::CompareEntries(CListEntry<CContactData*,CContactListGroup*> *pLeft,CListEntry<CContactData*,CContactListGroup*> *pRight)
{
	
	if(pLeft->GetType() == ITEM && pRight->GetType() == ITEM)
	{
		CContactData *pLeftEntry = ((CListItem<CContactData*,CContactListGroup*>*)pLeft)->GetItemData();
		CContactData *pRightEntry = ((CListItem<CContactData*,CContactListGroup*>*)pRight)->GetItemData();

		int iLeft = CContactList::GetInstance()->GetStatusPriority(pLeftEntry->strStatus);
		int iRight = CContactList::GetInstance()->GetStatusPriority(pRightEntry->strStatus);
		
		if(iLeft != iRight)
			return iLeft > iRight;
		else
			return wcsicmp(pLeftEntry->strDisplayName.c_str(),pRightEntry->strDisplayName.c_str())<0;
	}
	else if(pLeft->GetType() == ITEM && pRight->GetType() == CONTAINER)
		return false;
	else if(pLeft->GetType() == CONTAINER && pRight->GetType() == ITEM)
		return true;
	else if(pLeft->GetType() == CONTAINER && pRight->GetType() == CONTAINER)
	{	
		CContactListGroup *pGroup1 = ((CListContainer<CContactData*,CContactListGroup*>*)pLeft)->GetGroupData();
		CContactListGroup *pGroup2 = ((CListContainer<CContactData*,CContactListGroup*>*)pRight)->GetGroupData();

		return wcscmp(pGroup1->strName.c_str(),pGroup2->strName.c_str())<0;
	}
	
		return true;
}

//************************************************************************
// returns the entry for the specified group name
//************************************************************************
CListContainer<CContactData*,CContactListGroup*> *CContactList::FindGroup(tstring strGroup)
{
	return FindGroupInGroup(strGroup,(CListContainer<CContactData*,CContactListGroup*>*)this);
}

//************************************************************************
// returns the entry for the specified group name
//************************************************************************
CListContainer<CContactData*,CContactListGroup*> *CContactList::FindGroupInGroup(tstring strGroup,CListContainer<CContactData*,CContactListGroup*> *pGroup)
{
	CListContainer<CContactData*,CContactListGroup*>::iterator iter = pGroup->begin();
	CListContainer<CContactData*,CContactListGroup*> *pItem = NULL;
	while(iter != pGroup->end())
	{
		if((*iter)->GetType() == CONTAINER)
		{
			pItem = (CListContainer<CContactData*,CContactListGroup*>*)*iter;
			if(pItem->GetGroupData()->strName == strGroup)
				return pItem;

			pItem = FindGroupInGroup(strGroup,(CListContainer<CContactData*,CContactListGroup*> *)*iter);
			if(pItem)
				return pItem;
		}
		iter++;
	}
	return NULL;
}

//************************************************************************
// get group by string
//************************************************************************
CListContainer<CContactData*,CContactListGroup*> *CContactList::GetGroupByString(tstring strGroup)
{
	tstring strParse = strGroup;
	CListContainer<CContactData*,CContactListGroup*> *pGroup = (CListContainer<CContactData*,CContactListGroup*>*)this;
	tstring::size_type pos;
	while((pos = strParse.find('\\')) !=  tstring::npos )
	{
		strGroup = strParse.substr(0,pos);
		strParse = strParse.substr(pos+1);
		
		pGroup = FindGroupInGroup(strGroup,pGroup);
		if(pGroup == NULL)
			return NULL;
	}
	pGroup = FindGroupInGroup(strParse,pGroup);
	return pGroup;
}

//************************************************************************
// Adds a group
//************************************************************************
CListContainer<CContactData*,CContactListGroup*> *CContactList::AddGroupByString(tstring strGroup)
{
	tstring strParse = strGroup;
	tstring strPath = _T("");

	CListContainer<CContactData*,CContactListGroup*> *pGroup = (CListContainer<CContactData*,CContactListGroup*>*)this;
	CListContainer<CContactData*,CContactListGroup*> *pGroup2 = NULL;
	tstring::size_type pos;
	while((pos = strParse.find('\\')) !=  tstring::npos )
	{
		strGroup = strParse.substr(0,pos);
		strParse = strParse.substr(pos+1);
		strPath += strGroup;

		if(pGroup2 = FindGroupInGroup(strGroup,pGroup))
			pGroup = pGroup2;
		else
		{
			pGroup2 = pGroup->InsertGroup(pGroup->begin(),GetGroupObjectByPath(strPath));
			pGroup->sort(CContactList::CompareEntries);
			pGroup = pGroup2;
		}
		ASSERT(pGroup != NULL);
		strPath += _T("\\");
	}
	strPath += strParse;
	if(pGroup2 = FindGroupInGroup(strParse,pGroup))
		return pGroup2;
	else
	{
		pGroup2 = pGroup->InsertGroup(pGroup->begin(),GetGroupObjectByPath(strPath));
		pGroup->sort(CContactList::CompareEntries);
		return pGroup2;
	}
}

//************************************************************************
// called when the configuration has changed
//************************************************************************
void CContactList::OnConfigChanged()
{
	RegenerateList();
}


void CContactList::SetPosition(CListEntry<CContactData*,CContactListGroup*> *pEntry)
{
	CLCDList<CContactData*,CContactListGroup*>::SetPosition(pEntry);


}

bool CContactList::ScrollUp()
{
	m_dwLastScroll = GetTickCount();
	return CLCDList<CContactData*,CContactListGroup*>::ScrollUp();
}

bool CContactList::ScrollDown()
{
	m_dwLastScroll = GetTickCount();
	return CLCDList<CContactData*,CContactListGroup*>::ScrollDown();
}

//************************************************************************
// checks if a contact is visible
//************************************************************************
bool CContactList::IsVisible(CContactData *pContactData)
{
	if(!CConfig::GetProtocolContactlistFilter(pContactData->strMedium))
		return false;

	if(toLower(pContactData->strMedium) == _T("metacontact") ||
		(toLower(pContactData->strStatus) == _T("offline") && CConfig::GetBoolSetting(CLIST_HIDEOFFLINE)))
		return false;
	return true;
}

//************************************************************************
// called when a contacts status has changed
//************************************************************************
void CContactList::OnStatusChange(CContactData *pContactData,tstring strOldStatus)
{
	CListContainer<CContactData*,CContactListGroup*>* pGroup = NULL;

	tstring strGroup = CContactList::GetContactGroupPath(pContactData);
	if(toLower(pContactData->strMedium) == _T("metacontact"))
	{
		tstring strMetacontactGroup = _T("");
		if(strGroup.empty())
			strMetacontactGroup	= pContactData->strDisplayName;
		else
			strMetacontactGroup = strGroup + _T("\\") + pContactData->strDisplayName;

		pGroup = GetGroupByString(strMetacontactGroup);
		if(pGroup == NULL)
			pGroup = AddGroupByString(strMetacontactGroup);
	}
	else
	{
		CListItem<CContactData *,CContactListGroup*> *pItem = FindContact(pContactData);
		if(!pItem)
		{
			AddContact(pContactData);
			return;
		}

		if(!IsVisible(pItem->GetItemData()))
		{
			RemoveContact(pContactData,strOldStatus);
			return;
		}
		
		pGroup = ((CListContainer<CContactData*,CContactListGroup*>*)pItem->GetParent());
	}
	// Update the contacts group if it has one
	if(pGroup && pGroup->GetType() != ROOT && !pContactData->bIsSubContact)
	{
		if(toLower(pContactData->strStatus)  == _T("offline") && toLower(strOldStatus) != _T("offline"))
			ChangeGroupObjectCounters(pGroup->GetGroupData()->strPath,0,-1);

		else if(pContactData->strStatus  != _T("offline") &&  toLower(strOldStatus) == _T("offline"))
			ChangeGroupObjectCounters(pGroup->GetGroupData()->strPath,0,1);
	}

	if(pGroup)
		pGroup->sort(CContactList::CompareEntries);
}


//************************************************************************
// called when a contact has been added
//************************************************************************
void CContactList::OnContactAdded(CContactData *pContactData)
{
	tstring strGroup = CContactList::GetContactGroupPath(pContactData);
	// increase the membercount of the new group, and check if it needs to be created
	if(!strGroup.empty())
	{
		CContactListGroup *pGroup = GetGroupObjectByPath(strGroup);
		if(!pGroup)
			pGroup = CreateGroupObjectByPath(strGroup);
		ChangeGroupObjectCounters(strGroup,1);
	}

	// Update the list
	AddContact(pContactData);
}

//************************************************************************
// called when a contact has been deleted
//************************************************************************
void CContactList::OnContactDeleted(CContactData *pContactData)
{
	// Update the list
	RemoveContact(pContactData,pContactData->strStatus);
	
	tstring strGroup = CContactList::GetContactGroupPath(pContactData);
	// Decrease the membercount of the old group, and check if it needs to be deleted
	if(!strGroup.empty())
	{
		CContactListGroup *pGroup = GetGroupObjectByPath(strGroup);
		ChangeGroupObjectCounters(strGroup,-1);
		if(pGroup->iMembers <= 0)
			DeleteGroupObjectByPath(strGroup);
	}
}

//************************************************************************
// changes the groups membercount
//************************************************************************
void CContactList::ChangeGroupObjectCounters(tstring strGroup,int iMembers,int iOnline)
{
	CContactListGroup* pGroup = GetGroupObjectByPath(strGroup);
	if(!pGroup)
		return;

	pGroup->iMembers += iMembers;
	pGroup->iOnline += iOnline;

	tstring strParse = pGroup->strPath;
	tstring::size_type pos;

	while((pos = strParse.rfind('\\')) !=  tstring::npos )
	{
		strParse = strParse.substr(0,pos);

		pGroup = GetGroupObjectByPath(strParse);
		if(!pGroup)
			break;
		pGroup->iMembers += iMembers;
		pGroup->iOnline += iOnline;
	}	
}

//************************************************************************
// Called to delete the specified item
//************************************************************************
void CContactList::DeleteItem(CContactData *pEntry)
{
}

//************************************************************************
// Called to delete the specified group
//************************************************************************
void CContactList::DeleteGroup(CContactListGroup* pGroup)
{
}

//************************************************************************
// Adds a contact
//************************************************************************
void CContactList::AddContact(CContactData *pContactData)
{
	tstring strGroup = CContactList::GetContactGroupPath(pContactData);

	if(!IsVisible(pContactData))
	{
		return;
	}

	CListContainer<CContactData*,CContactListGroup*> *pGroup = NULL;

	CListItem<CContactData*,CContactListGroup*> *pItem = NULL;
	if(strGroup.empty())
	{
		pItem = AddItem(pContactData);
		((CListContainer<CContactData*,CContactListGroup*>*)this)->sort(CContactList::CompareEntries);
	}
	else
	{
		pGroup = GetGroupByString(strGroup);
		if(pGroup == NULL)
		{
			pGroup = AddGroupByString(strGroup);
			if(pGroup->GetGroupData()->bIsMetacontact && pGroup->GetGroupData()->pMetaContact)
			{
				if(toLower(pGroup->GetGroupData()->pMetaContact->strStatus) != _T("offline"))
				{
					CListContainer<CContactData*,CContactListGroup*>* pParent = static_cast<CListContainer<CContactData*,CContactListGroup*>*>(pGroup->GetParent());
					if(pParent->GetType() != ROOT)
						ChangeGroupObjectCounters(pParent->GetGroupData()->strPath,0,1);
				}
			}
		}
		if(pGroup->GetGroupData() == NULL)
			return;

		pItem = pGroup->AddItem(pContactData);
	
		if(!pContactData->bIsSubContact && toLower(pContactData->strStatus) != _T("offline"))
			ChangeGroupObjectCounters(pGroup->GetGroupData()->strPath,0,1);

		pGroup->sort(CContactList::CompareEntries);
	}
}

//************************************************************************
// Removes a contact
//************************************************************************
void CContactList::RemoveContact(CContactData *pContactData,tstring strOldStatus)
{
	CListContainer<CContactData*,CContactListGroup*> *pGroup = NULL;


	CListItem<CContactData*,CContactListGroup*> *pItem = FindContact(pContactData);
	if(!pItem)
		return;

	
	pGroup = (CListContainer<CContactData*,CContactListGroup*>*)pItem->GetParent();
	ASSERT(pGroup != NULL);
	
	// Update the contacts group if it has one
	if(pGroup->GetType() != ROOT && !pContactData->bIsSubContact)
	{
		if(toLower(strOldStatus) != _T("offline"))
			ChangeGroupObjectCounters(pGroup->GetGroupData()->strPath,0,-1);
	}

	pGroup->RemoveItem(pItem->GetItemData());

	CListContainer<CContactData*,CContactListGroup*> *pParent = (CListContainer<CContactData*,CContactListGroup*>*)pGroup->GetParent();
	while(pParent != NULL && pGroup->IsEmpty())
	{
		pParent->RemoveGroup(pGroup->GetGroupData());
		pGroup = pParent;
		pParent = (CListContainer<CContactData*,CContactListGroup*>*)pGroup->GetParent();
	}
}

//************************************************************************
// uninitializes the group objects
//************************************************************************
void CContactList::UninitializeGroupObjects()
{
	vector<CContactListGroup*>::iterator iter = m_Groups.begin();
	while(iter != m_Groups.end())
	{
		delete (*iter);
		iter++;
	}
	m_Groups.clear();
}

void CContactList::InitializeGroupObjects()
{
	UninitializeGroupObjects();

	vector<CContactData*> *pvContactData = CAppletManager::GetInstance()->GetContactDataVector();
	vector<CContactData*>::iterator iter = pvContactData->begin();
	while(iter != pvContactData->end())
	{
		CContactData *pConData = *iter;
		if(!pConData)
			continue;

		CContactList *pList = CContactList::GetInstance();

		tstring strGroup = GetContactGroupPath(pConData);

		if(!strGroup.empty())
		{
			CContactListGroup *pGroupObject = pList->GetGroupObjectByPath(strGroup);
			if(!pGroupObject)
			{	
				pGroupObject = pList->CreateGroupObjectByPath(strGroup);
				if(pConData->bIsSubContact)
				{
					pGroupObject->pMetaContact = CAppletManager::GetInstance()->GetContactDataByID(pGroupObject->strName,_T("METACONTACT"),pConData->strSection);
					pGroupObject->bIsMetacontact = true;
				}
			}
			// update it's counters
			//if(toLower(pConData->strMedium) != _T("metacontact"))
			if(!pConData->bIsSubContact)
				pList->ChangeGroupObjectCounters(strGroup,1);
		}

		pList->AddContact(pConData);

		iter++;
	}

	/*
	struct contactlist_enum_t	cet2;
	trillianInitialize(cet2);
	cet2.callback = CContactList::EnumSectionCallback;

	g_plugin_send(APP_GUID, "contactlistSectionEnumerate", &cet2);
	*/
}

tstring CContactList::GetContactGroupPath(CContactData *pConData)
{
	tstring strGroup = _T("");
	if(CAppletManager::GetInstance()->GetSectionCount() > 1)
	{
		strGroup = pConData->strSection.empty()?_T("My Contacts"):pConData->strSection;
		if(!pConData->strGroup.empty())
			strGroup += _T("\\");
	}
	if(CConfig::GetBoolSetting(CLIST_USEGROUPS))
		strGroup += pConData->strGroup;
	else if(pConData->bIsSubContact)
	{
		tstring::size_type pos = pConData->strGroup.rfind('\\');
		if(pos ==  tstring::npos )
			strGroup += pConData->strGroup;
		else
			strGroup += pConData->strGroup.substr(pos+1);
	}
	return strGroup;
}

//************************************************************************
// returns the entry for the specified handle
//************************************************************************
CListItem<CContactData*,CContactListGroup*> *CContactList::FindContact(CContactData *pContactData)
{
	return FindContactInGroup(pContactData,(CListContainer<CContactData*,CContactListGroup*>*)this);
}

//************************************************************************
// returns the entry for the specified handle
//************************************************************************
CListItem<CContactData*,CContactListGroup*> *CContactList::FindContactInGroup(CContactData *pContactData,CListContainer<CContactData*,CContactListGroup*> *pGroup)
{
	CListContainer<CContactData*,CContactListGroup*>::iterator iter = pGroup->begin();
	CListItem<CContactData*,CContactListGroup*> *pItem = NULL;
	while(iter != pGroup->end())
	{
		if((*iter)->GetType() == ITEM)
		{
			pItem = (CListItem<CContactData*,CContactListGroup*>*)*iter;
			if(pItem->GetItemData() == pContactData)
				return pItem;
		}
		else
		{
			pItem = FindContactInGroup(pContactData,(CListContainer<CContactData*,CContactListGroup*> *)*iter);
			if(pItem)
				return pItem;
		}
		iter++;
	}
	return NULL;
}

//************************************************************************
// get group object by string
//************************************************************************
CContactListGroup *CContactList::GetGroupObjectByPath(tstring strPath)
{
	ASSERT(!strPath.empty());

	CContactListGroup *pGroup = NULL;
	vector<CContactListGroup*>::iterator iter = m_Groups.begin();
	for(;iter != m_Groups.end();iter++)
	{
		if((*iter)->strPath == strPath)
		{
			pGroup = *iter;
			break;
		}
	}
	return pGroup;
}

//************************************************************************
// creates a group object by string
//************************************************************************
CContactListGroup *CContactList::CreateGroupObjectByPath(tstring strPath)
{
	ASSERT(!strPath.empty());

	CContactListGroup *pNewGroup = new CContactListGroup();
	CContactListGroup *pParentGroup = NULL;

	tstring strParsePath = _T("");
	tstring strName = strPath;
	tstring::size_type pos;

	while((pos = strName.find('\\')) !=  tstring::npos )
	{
		strParsePath += strName.substr(0,pos);
		strName = strName.substr(pos+1);
		
		pParentGroup = GetGroupObjectByPath(strParsePath);
		if(!pParentGroup)
			pParentGroup = CreateGroupObjectByPath(strParsePath);
		strParsePath += _T("\\");
	}
	
	pNewGroup->bIsContactlist = false;
	if(pParentGroup)
		pParentGroup->iGroups++;
	else if(CAppletManager::GetInstance()->GetSectionCount() > 1)
		pNewGroup->bIsContactlist = true;
	
	pNewGroup->strName = strName;
	pNewGroup->strPath = strPath;
	pNewGroup->iMembers = 0;
	pNewGroup->iOnline = 0;
	pNewGroup->iGroups = 0;
	pNewGroup->bIsMetacontact = false;
	pNewGroup->pMetaContact = NULL;

	m_Groups.push_back(pNewGroup);

	return pNewGroup;
}

//************************************************************************
// deletes a group object by string
//************************************************************************
void CContactList::DeleteGroupObjectByPath(tstring strPath)
{
	ASSERT(!strPath.empty());

	CContactListGroup *pParentGroup = NULL;
	vector<CContactListGroup*>::iterator iter = m_Groups.begin();
	for(iter = m_Groups.begin();iter != m_Groups.end();iter++)
	{
		if((*iter)->strPath == strPath)
		{
			CContactListGroup *pGroup = *iter;
			m_Groups.erase(iter);
			delete pGroup;

			tstring strParse = strPath;
			tstring::size_type pos = strParse.rfind('\\');
			if(pos !=  tstring::npos )
			{
				strParse = strParse.substr(0,pos);
				pParentGroup = GetGroupObjectByPath(strParse);
				pParentGroup->iGroups--;
				if(pParentGroup->iMembers <= 0 && pParentGroup->iGroups <= 0)
					DeleteGroupObjectByPath(strParse);
			}
			return;
		}
	}
}