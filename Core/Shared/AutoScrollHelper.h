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
	CAutoScrollHelper(BOOL bVertical, int nZoneWidth)
		:
		m_bVertical(bVertical),
		m_nZoneWidth(max(nZoneWidth, 0))
	{

	}

	BOOL HitTest(HWND hwnd, SCROLLZONE* pZone = NULL) const
	{
		SCROLLZONE nZone = HitTestZone(hwnd);

		if (pZone)
			*pZone = nZone;

		if (m_bVertical)
			return (nZone == ASHZ_TOP) || (nZone == ASHZ_BOTTOM);

		// else
		return (nZone == ASHZ_LEFT) || (nZone == ASHZ_RIGHT);
	}

	SCROLLZONE HitTestZone(HWND hwnd) const
	{
		CPoint point(::GetMessagePos());
		::ScreenToClient(hwnd, &point);

		CRect rect;
		::GetClientRect(hwnd, rect);

		if (!rect.PtInRect(point))
			return ASHZ_OUTSIDE;

		if (m_nZoneWidth == 0)
			return ASHZ_INSIDE;

		if (m_bVertical)
		{
			rect.DeflateRect(0, m_nZoneWidth);

			if (rect.PtInRect(point))
				return ASHZ_INSIDE;

			if (point.y <= rect.top)
				return ASHZ_TOP;

			// else
			return ASHZ_BOTTOM;
		}
		else
		{
			rect.DeflateRect(m_nZoneWidth, 0);
				
			if (rect.PtInRect(point))
				return ASHZ_INSIDE;

			if (point.x <= rect.left)
				return ASHZ_LEFT;

			// else
			return ASHZ_TOP;
		}
	}

protected:
	BOOL m_bVertical;
	int m_nZoneWidth;
};

#endif // !defined(AFX_AUTOSCROLLHELPER_H__06381648_F0F3_4791_8204_6A0A8798F29A__INCLUDED_)
