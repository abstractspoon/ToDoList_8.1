// AutoScrollHelper.cpp: implementation of the CAutoScrollHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoScrollHelper.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoScrollHelper::CAutoScrollHelper(BOOL bVertical, int nZoneWidth)
	:
	m_bVertical(bVertical),
	m_nZoneWidth(max(nZoneWidth, 0))
{

}

BOOL CAutoScrollHelper::HitTest(HWND hwnd, SCROLLZONE* pZone) const
{
	return HitTest(GetClientScreenRect(hwnd), pZone);
}

BOOL CAutoScrollHelper::HitTest(const CRect& rScreen, SCROLLZONE* pZone) const
{
	SCROLLZONE nZone = HitTestZone(rScreen);

	if (pZone)
		*pZone = nZone;

	if (m_bVertical)
		return (nZone == ASHZ_TOP) || (nZone == ASHZ_BOTTOM);

	// else
	return (nZone == ASHZ_LEFT) || (nZone == ASHZ_RIGHT);
}

SCROLLZONE CAutoScrollHelper::HitTestZone(HWND hwnd) const
{
	return HitTestZone(GetClientScreenRect(hwnd));
}

SCROLLZONE CAutoScrollHelper::HitTestZone(const CRect& rScreen) const
{
	CPoint point(::GetMessagePos());

	if (!rScreen.PtInRect(point))
		return ASHZ_OUTSIDE;

	if (m_nZoneWidth == 0)
		return ASHZ_INSIDE;

	if (m_bVertical)
	{
		CRect rInner(rScreen);
		rInner.DeflateRect(0, m_nZoneWidth);

		if (rInner.PtInRect(point))
			return ASHZ_INSIDE;

		if (point.y <= rInner.top)
			return ASHZ_TOP;

		// else
		return ASHZ_BOTTOM;
	}
	else
	{
		CRect rInner(rScreen);
		rInner.DeflateRect(m_nZoneWidth, 0);

		if (rInner.PtInRect(point))
			return ASHZ_INSIDE;

		if (point.x <= rInner.left)
			return ASHZ_LEFT;

		// else
		return ASHZ_TOP;
	}
}

CRect CAutoScrollHelper::GetClientScreenRect(HWND hwnd)
{
	CRect rect;
	::GetClientRect(hwnd, rect);

	::ClientToScreen(hwnd, &rect.TopLeft());
	::ClientToScreen(hwnd, &rect.BottomRight());

	return rect;
}
