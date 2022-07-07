// CAutoScrollHelper.h: interface for the CTreeDragDropHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOSCROLLHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_)
#define AFX_AUTOSCROLLHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////

enum SCROLLZONE
{
	ASHZ_OUTSIDE,
	ASHZ_INSIDE,
	ASHZ_LEFT,
	ASHZ_TOP,
	ASHZ_RIGHT,
	ASHZ_BOTTOM
};

class CAutoScrollHelper
{
public:
	CAutoScrollHelper(BOOL bVertical, int nZoneWidth);

	BOOL HitTest(HWND hwnd, SCROLLZONE* pZone = NULL) const;
	BOOL HitTest(const CRect& rScreen, SCROLLZONE* pZone = NULL) const;

	SCROLLZONE HitTestZone(HWND hwnd) const;
	SCROLLZONE HitTestZone(const CRect& rScreen) const;

	static CRect GetClientScreenRect(HWND hwnd);

protected:
	BOOL m_bVertical;
	int m_nZoneWidth;
};

#endif // !defined(AFX_AUTOSCROLLHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_)
