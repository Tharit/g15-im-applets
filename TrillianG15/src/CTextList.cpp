#include "Stdafx.h"
#include "CTextList.h"

CTextList::CTextList()
{

}

CTextList::~CTextList()
{

}
	
void CTextList::DrawEntry(CLCDGfx *pGfx,tstring strValue,bool bSelected)
{
	SelectObject(pGfx->GetHDC(),m_hFont);
	pGfx->DrawText(0,0,pGfx->GetClipWidth(),strValue);
	if(bSelected)
	{
		RECT invert = { 0,0,GetWidth(),m_iFontHeight};
		InvertRect(pGfx->GetHDC(),&invert);
	}
}

int	CTextList::GetEntryHeight(tstring strValue,bool bSelected)
{
	return m_iFontHeight;
}


void CTextList::DrawGroup(CLCDGfx *pGfx,tstring strValue,bool bOpen,bool bSelected)
{
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

int	CTextList::GetGroupHeight(tstring strValue,bool bOpen,bool bSelected)
{
	return m_iFontHeight;
}
