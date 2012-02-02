#ifndef _CTextList_H_
#define _CTextList_H_

#include "CLCDList.h"

class CTextList : public CLCDList<tstring,tstring>
{
public:
	CTextList();
	~CTextList();
	
protected:
	void DrawEntry(CLCDGfx *pGfx,tstring strValue,bool bSelected);
	int	GetEntryHeight(tstring strValue,bool bSelected);
	void DrawGroup(CLCDGfx *pGfx,tstring strValue,bool bOpen,bool bSelected);
	int	GetGroupHeight(tstring strValue,bool bOpen,bool bSelected);
};
#endif