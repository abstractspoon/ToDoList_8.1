// TaskCalendarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TaskCalendarCtrl.h"

#include "..\Shared\GraphicsMisc.h"
#include "..\Shared\DateHelper.h"
#include "..\Shared\TimeHelper.h"
#include "..\Shared\dialoghelper.h"

#include "..\3rdParty\ColorDef.h"

#include <math.h>

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

TASKCALITEM::TASKCALITEM()
	: 
	color(CLR_NONE), 
	bGoodAsDone(FALSE),
	dwTaskID(0),
	bTopLevel(FALSE),
	bLocked(FALSE),
	bIsParent(FALSE),
	bRecurring(FALSE),
	bTreatOverdueAsDueToday(FALSE)
{

}
	
TASKCALITEM::TASKCALITEM(const ITASKLISTBASE* pTasks, HTASKITEM hTask, DWORD dwCalcDates) 
	: 
	color(CLR_NONE), 
	bGoodAsDone(FALSE),
	dwTaskID(0),
	bTopLevel(FALSE),
	bLocked(FALSE),
	bIsParent(FALSE),
	bRecurring(FALSE),
	bTreatOverdueAsDueToday(FALSE)
{
	UpdateTask(pTasks, hTask, dwCalcDates);

	// Handle TopLevel only on creation
	bTopLevel = (pTasks->GetTaskParentID(hTask) == 0);
}

	
TASKCALITEM::TASKCALITEM(const TASKCALITEM& tci)
{
	*this = tci;
}

TASKCALITEM& TASKCALITEM::operator=(const TASKCALITEM& tci)
{
	sName = tci.sName;
	sFormattedName = tci.sFormattedName;
	color = tci.color;
	dwTaskID = tci.dwTaskID;
	bGoodAsDone = tci.bGoodAsDone;
	bLocked = tci.bLocked;
	dtCreation = tci.dtCreation;
	dtStart = tci.dtStart;
	dtDue = tci.dtDue;
	dtDone = tci.dtDone;
	dtStartCalc = tci.dtStartCalc;
	dtEndCalc = tci.dtEndCalc;
	bHasIcon = tci.bHasIcon;
	bIsParent = tci.bIsParent;
	bRecurring = tci.bRecurring;
	bTreatOverdueAsDueToday = tci.bTreatOverdueAsDueToday;

	aTags.Copy(tci.aTags);
	
	return (*this);
}

BOOL TASKCALITEM::operator==(const TASKCALITEM& tci)
{
	return ((sName == tci.sName) &&
			(color == tci.color) &&
			(dwTaskID == tci.dwTaskID) &&
			(bGoodAsDone == tci.bGoodAsDone) &&
			(bLocked == tci.bLocked) &&
			(dtCreation == tci.dtCreation) &&
			(dtStart == tci.dtStart) &&
			(dtDue == tci.dtDue) &&
			(dtDone == tci.dtDone) &&
			(dtStartCalc == tci.dtStartCalc) &&
			(dtEndCalc == tci.dtEndCalc) &&
			(bHasIcon == tci.bHasIcon) &&
			(bIsParent == tci.bIsParent) &&
			(bRecurring == tci.bRecurring) &&
			Misc::MatchAllT(aTags, tci.aTags, FALSE));
}

void TASKCALITEM::UpdateTaskDates(const ITASKLISTBASE* pTasks, HTASKITEM hTask, DWORD dwCalcDates)
{
	// check for quick exit
	if (!pTasks->IsAttributeAvailable(TDCA_STARTDATE) &&
		!pTasks->IsAttributeAvailable(TDCA_DUEDATE) &&
		!pTasks->IsAttributeAvailable(TDCA_DONEDATE))
	{
		return;
	}

	// get creation date once only
	time64_t tDate = 0;

	if (!CDateHelper::IsDateSet(dtCreation) && pTasks->GetTaskCreationDate64(hTask, tDate))
		dtCreation = GetDate(tDate);

	// retrieve new dates
	if (pTasks->IsAttributeAvailable(TDCA_STARTDATE))
	{
		if (pTasks->GetTaskStartDate64(hTask, FALSE, tDate))
			dtStart = GetDate(tDate);
		else
			CDateHelper::ClearDate(dtStart);
	}
	
	if (pTasks->IsAttributeAvailable(TDCA_DUEDATE))
	{
		if (pTasks->GetTaskDueDate64(hTask, FALSE, tDate))
			dtDue = GetDate(tDate);
		else
			CDateHelper::ClearDate(dtDue);
	}

	if (pTasks->IsAttributeAvailable(TDCA_DONEDATE))
	{
		if (pTasks->GetTaskDoneDate64(hTask, tDate))
			dtDone = GetDate(tDate);
		else
			CDateHelper::ClearDate(dtDone);
	}

	RecalcDates(dwCalcDates);
}

void TASKCALITEM::ClearCalculatedDates()
{
	CDateHelper::ClearDate(dtStartCalc);
	CDateHelper::ClearDate(dtEndCalc);

	bTreatOverdueAsDueToday = FALSE;
}

void TASKCALITEM::RecalcDates(DWORD dwCalcDates, TCC_HITTEST nDragging)
{
	ClearCalculatedDates();

	BOOL bHasStartDate = CDateHelper::IsDateSet(dtStart);
	BOOL bHasDueDate = CDateHelper::IsDateSet(dtDue);
	BOOL bHasDoneDate = CDateHelper::IsDateSet(dtDone);
	BOOL bHasEndDate = (bHasDueDate || bHasDoneDate);

	// Don't fixup tasks with no dates at all
	if (!bHasStartDate && !bHasEndDate)
		return;

	// Else calculate missing dates
	const COleDateTime dtEnd = (bHasDoneDate ? dtDone : dtDue);

	if (nDragging == TCCHT_MIDDLE)
	{
		if (!bHasStartDate)
		{
			ASSERT(bHasEndDate);

			// Set duration to one day
			if (CDateHelper::IsEndOfDay(dtEnd, TRUE))
				dtStartCalc = CDateHelper::GetDateOnly(dtEnd);
			else
				dtStartCalc = (dtEnd.m_dt - 1.0);
		}
		else if (!bHasEndDate)
		{
			ASSERT(bHasStartDate);

			// Set duration to one day
			if (CDateHelper::DateHasTime(dtStart))
				dtEndCalc = (dtStart.m_dt - 1.0);
			else
				dtEndCalc = CDateHelper::GetEndOfDay(dtStart);
		}
		else
		{
			ASSERT(bHasStartDate && bHasEndDate);

			// Leave dates 'as-is'
		}
	}
	else 
	{
		const COleDateTime dtToday = CDateHelper::GetDate(DHD_TODAY);

		if (!bHasStartDate) // -----------------------------------------------------
		{
			if (Misc::HasFlag(dwCalcDates, TCCO_CALCMISSINGSTARTASCREATION))
			{
				if (bHasEndDate)
				{
					dtStartCalc = min(dtEnd, dtCreation);
				}
				else
				{
					dtStartCalc = dtCreation;
				}
			}
			else if (Misc::HasFlag(dwCalcDates, TCCO_CALCMISSINGSTARTASDUE))
			{
				if (bHasEndDate)
				{
					dtStartCalc = dtEnd;
				}
			}
			else // TCCO_CALCMISSINGSTARTASEARLIESTDUEANDTODAY
			{
				if (bHasEndDate)
				{
					dtStartCalc = min(dtEnd, dtToday);
				}
				else
				{
					dtStartCalc = dtToday;
				}
			}

			dtStartCalc = CDateHelper::GetDateOnly(dtStartCalc);
		}

		if (!bHasEndDate) // -------------------------------------------------------
		{
			if (Misc::HasFlag(dwCalcDates, TCCO_CALCMISSINGDUEASSTART))
			{
				if (bHasStartDate)
				{
					dtEndCalc = dtStart;
				}
			}
			else // TCCO_CALCMISSINGDUEASLATESTSTARTANDTODAY
			{
				if (bHasStartDate)
				{
					dtEndCalc = max(dtStart, dtToday);
				}
				else
				{
					dtEndCalc = dtToday;
				}
			}

			dtEndCalc = CDateHelper::GetEndOfDay(dtEndCalc);
		}
		else if (bHasDoneDate) // ---------------------------------------------------
		{
			// adjust done date to point to end of day if it has no time component
			if (!CDateHelper::DateHasTime(dtDone))
			{
				dtDone = CDateHelper::GetEndOfDay(dtDone);
			}
		}
		else if (bHasDueDate) // ---------------------------------------------------
		{
			// Special case: treat overdue tasks as due today
			if ((dtDue < dtToday) && Misc::HasFlag(dwCalcDates, TCCO_TREATOVERDUEASDUETODAY))
			{
				dtEndCalc = CDateHelper::GetEndOfDay(dtToday);
				bTreatOverdueAsDueToday = TRUE;
			}

			// adjust due date to point to end of day if it has no time component
			if (!CDateHelper::DateHasTime(dtDue))
			{
				dtDue = CDateHelper::GetEndOfDay(dtDue);
			}
		}

		// Finally ensure start date precedes end date
		if (bHasStartDate && bHasEndDate) // ----------------------------------------
		{
			if (bHasDoneDate && (dtStart > dtDone))
			{
				dtStartCalc = CDateHelper::GetDateOnly(dtDone);
			}
			else if (bHasDueDate && (dtStart > dtDue))
			{
				dtStartCalc = CDateHelper::GetDateOnly(dtDue);
			}
		}
	}
}

BOOL TASKCALITEM::UpdateTask(const ITASKLISTBASE* pTasks, HTASKITEM hTask, DWORD dwCalcDates)
{
	ASSERT(dwTaskID == 0 || pTasks->GetTaskID(hTask) == dwTaskID);

	if (dwTaskID == 0)
		dwTaskID = pTasks->GetTaskID(hTask);

	// snapshot current state to check for changes
	TASKCALITEM tciOrg = *this;

	if (pTasks->IsAttributeAvailable(TDCA_TASKNAME))
		sName = pTasks->GetTaskTitle(hTask);

	if (pTasks->IsAttributeAvailable(TDCA_ICON))
		bHasIcon = !Misc::IsEmpty(pTasks->GetTaskIcon(hTask));

	if (pTasks->IsAttributeAvailable(TDCA_DEPENDENCY))
		bHasDepends = !Misc::IsEmpty(pTasks->GetTaskDependency(hTask, 0));

	if (pTasks->IsAttributeAvailable(TDCA_RECURRENCE))
		bRecurring = pTasks->IsTaskRecurring(hTask);

	if (pTasks->IsAttributeAvailable(TDCA_TAGS))
	{
		aTags.RemoveAll();
		int nTag = pTasks->GetTaskTagCount(hTask);

		while (nTag--)
			aTags.InsertAt(0, pTasks->GetTaskTag(hTask, nTag));
	}

	UpdateTaskDates(pTasks, hTask, dwCalcDates);

	// always update colour
	color = pTasks->GetTaskTextColor(hTask);

	// always update lock state
	bLocked = pTasks->IsTaskLocked(hTask, true);

	// and 'Good as Done'
	bGoodAsDone = pTasks->IsTaskGoodAsDone(hTask);

	// and Parent
	bIsParent = pTasks->IsTaskParent(hTask);

	if (pTasks->IsAttributeAvailable(TDCA_TASKNAME) || 
		pTasks->IsAttributeAvailable(TDCA_STARTDATE))
	{
		ReformatName();
	}
	
	return !(*this == tciOrg);
}

BOOL TASKCALITEM::IsValid() const
{
	return (IsStartDateSet() || IsEndDateSet());
}

BOOL TASKCALITEM::IsParent() const
{
	return bIsParent;
}

BOOL TASKCALITEM::HasIcon(BOOL bShowParentsAsFolder) const
{
	return (bHasIcon || (bIsParent && bShowParentsAsFolder));
}

BOOL TASKCALITEM::IsDone(BOOL bIncGoodAs) const
{
	if (CDateHelper::IsDateSet(dtDone))
		return TRUE;
	
	// else
	return (bIncGoodAs && bGoodAsDone);
}

COleDateTime TASKCALITEM::GetDate(time64_t tDate)
{
	COleDateTime date;

	if (tDate > 0)
		date = CDateHelper::GetDate(tDate);
	else
		CDateHelper::ClearDate(date);

	// else
	return date;
}

BOOL TASKCALITEM::IsStartDateSet() const
{
	if (CDateHelper::IsDateSet(dtStart))
	{
		// sanity check
		ASSERT(!CDateHelper::IsDateSet(dtStartCalc) || (IsEndDateSet() && (dtStart > GetAnyEndDate())));
		return TRUE;
	}

	return FALSE;
}

BOOL TASKCALITEM::IsEndDateSet() const
{
	if (CDateHelper::IsDateSet(dtDone))
	{
		ASSERT(!CDateHelper::IsDateSet(dtEndCalc));
		return TRUE;
	}

	if (CDateHelper::IsDateSet(dtDue))
	{
		ASSERT(!CDateHelper::IsDateSet(dtEndCalc) || bTreatOverdueAsDueToday);
		return TRUE;
	}

	return FALSE;
}

BOOL TASKCALITEM::HasAnyStartDate() const
{
	return CDateHelper::IsDateSet(GetAnyStartDate());
}

BOOL TASKCALITEM::HasAnyEndDate() const
{
	return CDateHelper::IsDateSet(GetAnyEndDate());
}

COleDateTime TASKCALITEM::GetAnyStartDate() const
{
	// take calculated value in preference
	if (CDateHelper::IsDateSet(dtStartCalc))
		return dtStartCalc;
	
	// else
	return dtStart;
}

COleDateTime TASKCALITEM::GetAnyEndDate() const
{
	// take calculated value in preference
	if (CDateHelper::IsDateSet(dtEndCalc))
	{
		return dtEndCalc;
	}
	else if (CDateHelper::IsDateSet(dtDone))
	{
		return dtDone;
	}
	
	// else
	return dtDue;
}

COleDateTime TASKCALITEM::GetDoneDate() const
{
	ASSERT(CDateHelper::IsDateSet(dtDone));

	return dtDone;
}

void TASKCALITEM::MinMax(COleDateTime& dtMin, COleDateTime& dtMax) const
{
	MinMax(GetAnyStartDate(), dtMin, dtMax);
	MinMax(GetAnyEndDate(), dtMin, dtMax);
}

void TASKCALITEM::MinMax(const COleDateTime& date, COleDateTime& dtMin, COleDateTime& dtMax)
{
	if (CDateHelper::IsDateSet(date))
	{
		if (CDateHelper::IsDateSet(dtMin))
			dtMin.m_dt = min(dtMin.m_dt, date.m_dt);
		else
			dtMin = date;

		if (CDateHelper::IsDateSet(dtMax))
			dtMax.m_dt = max(dtMax.m_dt, date.m_dt);
		else
			dtMax = date;
	}
}

void TASKCALITEM::SetStartDate(const COleDateTime& date)
{
	ASSERT(CDateHelper::IsDateSet(date));

	dtStart = date;
	CDateHelper::ClearDate(dtStartCalc);

	ReformatName();
}

void TASKCALITEM::SetDueDate(const COleDateTime& date)
{
	ASSERT(CDateHelper::IsDateSet(date));

	dtDue = date;
	CDateHelper::ClearDate(dtEndCalc);
}

COLORREF TASKCALITEM::GetTextColor(BOOL bSelected, BOOL bColorIsBkgnd) const
{
	if (HasColor())
	{
		if (bColorIsBkgnd && !bSelected && !IsDone(TRUE))
		{
			return GraphicsMisc::GetBestTextColor(color);
		}
		else if (!Misc::IsHighContrastActive())
		{
			return color;
		}
	}
	
	// else
	return GetSysColor(COLOR_WINDOWTEXT);
}

COLORREF TASKCALITEM::GetFillColor(BOOL bColorIsBkgnd) const
{
	if (HasColor() && !IsDone(TRUE))
	{
		if (bColorIsBkgnd)
		{
			return color;
		}
		else if (!Misc::IsHighContrastActive())
		{
			return GraphicsMisc::Lighter(color, 0.9);
		}
	}
	
	// else
	return GetSysColor(COLOR_WINDOW);
}

COLORREF TASKCALITEM::GetBorderColor(BOOL bColorIsBkgnd) const
{
	if (HasColor())
	{
		if (bColorIsBkgnd && !IsDone(TRUE))
		{
			return GraphicsMisc::Darker(color, 0.4);
		}
		else if (!Misc::IsHighContrastActive())
		{
			return color;
		}
	}
	
	// else
	return GetSysColor(COLOR_WINDOWFRAME);
}

BOOL TASKCALITEM::HasColor() const
{
	return ((color != CLR_NONE) && (color != GetSysColor(COLOR_WINDOWTEXT)));
}

CString TASKCALITEM::GetName(BOOL bFormatted) const
{
	if (!bFormatted || sFormattedName.IsEmpty()) 
		return sName;
	
	// else
	return sFormattedName;
}

void TASKCALITEM::ReformatName()
{
	// Prefix time if specified
	if (CDateHelper::DateHasTime(dtStart))
	{
		sFormattedName.Format(_T("%s - %s"), CTimeHelper::FormatClockTime(dtStart), sName);
	}
	else
	{
		sFormattedName.Empty();
	}
}

BOOL TASKCALITEM::HasTag(LPCTSTR szTag) const
{
	return (Misc::Find(szTag, aTags, FALSE, TRUE) != -1);
}

/////////////////////////////////////////////////////////////////////////////

TASKCALFUTUREITEM::TASKCALFUTUREITEM(const TASKCALITEM& tciOrg, DWORD dwFutureID, const COleDateTimeRange& dtRange)
	: 
	TASKCALITEM(tciOrg), 
	dwRealTaskID(tciOrg.GetTaskID())
{
	ASSERT(!tciOrg.IsDone(FALSE));
	ASSERT(dwFutureID && dtRange.IsValid());

	dwTaskID = dwFutureID;
	dtStart = dtRange.GetStart();
	dtDue = dtRange.GetEndInclusive();
}

COLORREF TASKCALFUTUREITEM::GetFillColor(BOOL /*bTextIsBack*/) const
{
	return CLR_NONE;
}

COLORREF TASKCALFUTUREITEM::GetBorderColor(BOOL /*bTextIsBack*/) const
{
	return 0; // Black
}

COLORREF TASKCALFUTUREITEM::GetTextColor(BOOL /*bSelected*/, BOOL /*bTextIsBack*/) const
{
	return GetSysColor(COLOR_3DDKSHADOW);
}

/////////////////////////////////////////////////////////////////////////////

CTaskCalItemMap::~CTaskCalItemMap()
{
	RemoveAll();
}

void CTaskCalItemMap::RemoveAll()
{
	POSITION pos = GetStartPosition();
	DWORD dwTaskID = 0;
	TASKCALITEM* pTCI = NULL;

	while (pos)
	{
		GetNextAssoc(pos, dwTaskID, pTCI);
		delete pTCI;
	}

	CMap<DWORD, DWORD, TASKCALITEM*, TASKCALITEM*&>::RemoveAll();
}

void CTaskCalItemMap::RemoveKey(DWORD dwTaskID)
{
	delete GetTaskItem(dwTaskID);

	CMap<DWORD, DWORD, TASKCALITEM*, TASKCALITEM*&>::RemoveKey(dwTaskID);

}

TASKCALITEM* CTaskCalItemMap::GetTaskItem(DWORD dwTaskID) const
{
	TASKCALITEM* pTCI = NULL;
	VERIFY(!Lookup(dwTaskID, pTCI) || pTCI);

	ASSERT(!pTCI || (pTCI->GetTaskID() == dwTaskID));
	return pTCI;
}

TASKCALITEM* CTaskCalItemMap::GetNextTask(POSITION& pos) const
{
	if (!pos)
	{
		ASSERT(0);
		return NULL;
	}

	DWORD dwTaskID = 0;
	TASKCALITEM* pTCI = NULL;

	GetNextAssoc(pos, dwTaskID, pTCI);

	return pTCI;
}

DWORD CTaskCalItemMap::GetNextTaskID(POSITION& pos) const
{
	if (!pos)
	{
		ASSERT(0);
		return NULL;
	}

	DWORD dwTaskID = 0;
	TASKCALITEM* pTCI = NULL;

	GetNextAssoc(pos, dwTaskID, pTCI);

	return dwTaskID;
}

BOOL CTaskCalItemMap::HasTask(DWORD dwTaskID) const
{
	return (GetTaskItem(dwTaskID) != NULL);
}

/////////////////////////////////////////////////////////////////////////////

DWORD CTaskCalFutureItemMap::GetRealTaskID(DWORD dwTaskID) const
{
	const TASKCALITEM* pTCI = GetTaskItem(dwTaskID);

	if (!pTCI)
	{
		ASSERT(0);
		return 0;
	}

	const TASKCALFUTUREITEM* pTCIFuture = dynamic_cast<const TASKCALFUTUREITEM*>(pTCI);

	if (pTCIFuture)
		return pTCIFuture->dwRealTaskID;

	// else
	return dwTaskID;
}

/////////////////////////////////////////////////////////////////////////////

// Used temporarily by CompareItems
static TDC_ATTRIBUTE s_nSortBy = TDCA_NONE;
static BOOL s_bSortAscending = TRUE;

void CTaskCalItemArray::SortItems(TDC_ATTRIBUTE nSortBy, BOOL bSortAscending)
{
	if (GetSize() > 1)
	{
		s_nSortBy = nSortBy;
		s_bSortAscending = bSortAscending;

		Misc::SortArrayT<TASKCALITEM*>(*this, CompareItems);

		s_nSortBy = TDCA_NONE;
		s_bSortAscending = -1;
	}
}

int CTaskCalItemArray::CompareItems(const void* pV1, const void* pV2)
{
	typedef TASKCALITEM* PTASKCALITEM;

	const TASKCALITEM* pTCI1 = *(static_cast<const PTASKCALITEM*>(pV1));
	const TASKCALITEM* pTCI2 = *(static_cast<const PTASKCALITEM*>(pV2));

	// special case: Not drawing tasks continuous means that
	// the same task can appear twice
	if (pTCI1->GetTaskID() == pTCI2->GetTaskID())
		return 0;

	// earlier start date
	if (pTCI1->GetAnyStartDate() < pTCI2->GetAnyStartDate())
		return -1;

	if (pTCI1->GetAnyStartDate() > pTCI2->GetAnyStartDate())
		return 1;

	// equal so test for later end date
	if (pTCI1->GetAnyEndDate() > pTCI2->GetAnyEndDate())
		return -1;

	if (pTCI1->GetAnyEndDate() < pTCI2->GetAnyEndDate())
		return 1;

	// equal so test for sort attribute
	int nCompare = 0;

	switch (s_nSortBy)
	{
	case TDCA_TASKNAME:
		ASSERT(s_bSortAscending != -1);
		nCompare = pTCI1->GetName(FALSE).CompareNoCase(pTCI2->GetName(FALSE));
		break;

	case TDCA_ID:
		ASSERT(s_bSortAscending != -1);
		// fall thru

	case TDCA_NONE:
		nCompare = ((pTCI1->GetTaskID() < pTCI2->GetTaskID()) ? -1 : 1);
		break;

	default:
		ASSERT(0);
	}

	if (!s_bSortAscending && (nCompare != 0) && (s_nSortBy != TDCA_NONE))
		nCompare = -nCompare;

	return nCompare;
}

int CTaskCalItemArray::FindItem(DWORD dwTaskID) const
{
	int nItem = GetSize();

	while (nItem--)
	{
		if (GetAt(nItem)->GetTaskID() == dwTaskID)
			return nItem;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////

CHeatMap::CHeatMap(int nMinHeatCutoff) 
	: 
	m_nMinHeatCutoff(nMinHeatCutoff),
	m_nMaxHeatCutoff(nMinHeatCutoff) 
{
}

void CHeatMap::ClearHeat()
{
	m_mapHeat.RemoveAll();
	m_nMaxHeatCutoff = m_nMinHeatCutoff;
}

BOOL CHeatMap::SetColorPalette(const CDWordArray& aColors)
{
	if (aColors.GetSize() < 5)
	{
		ASSERT(0);
		return FALSE;
	}

	if (Misc::MatchAll(aColors, m_aColorPalette, TRUE))
		return FALSE;
	
	m_aColorPalette.Copy(aColors);
	return TRUE;
}

BOOL CHeatMap::Recalculate(const CTaskCalItemMap& mapData, TDC_ATTRIBUTE nAttrib, DWORD dwOptions)
{
	m_mapHeat.RemoveAll();

	if ((nAttrib == TDCA_NONE) || (mapData.GetCount() == 0))
		return FALSE;

	POSITION pos = mapData.GetStartPosition();
	DWORD dwTaskID = 0;
	TASKCALITEM* pTCI = NULL;

	while (pos)
	{
		mapData.GetNextAssoc(pos, dwTaskID, pTCI);

		if (pTCI->IsParent() && Misc::HasFlag(dwOptions, TCCO_HIDEPARENTTASKS))
			continue;

		switch (nAttrib)
		{
		case TDCA_DONEDATE:
			if (pTCI->IsDone(FALSE))
			{
				COleDateTime dtDone = CDateHelper::GetDateOnly(pTCI->GetDoneDate());
				Misc::IncrementItemT<double, int>(m_mapHeat, dtDone.m_dt);
			}
			break;

		case TDCA_DUEDATE:
			if (!pTCI->IsDone(FALSE) && pTCI->HasAnyEndDate())
			{
				COleDateTime dtDue = CDateHelper::GetDateOnly(pTCI->GetAnyEndDate());
				Misc::IncrementItemT<double, int>(m_mapHeat, dtDue.m_dt);
			}
			break;

		case TDCA_STARTDATE:
			if (!pTCI->IsDone(FALSE) && pTCI->HasAnyStartDate())
			{
				COleDateTime dtStart = CDateHelper::GetDateOnly(pTCI->GetAnyStartDate());
				Misc::IncrementItemT<double, int>(m_mapHeat, dtStart.m_dt);
			}
			break;

		default:
			ASSERT(0);
			return FALSE;
		}
	}

	// Calculate the mean and maximum
	if (m_mapHeat.GetCount() == 0)
		return FALSE;

	int nTotalHeat = 0, nHeat = 0;
	double dUnused;

	pos = m_mapHeat.GetStartPosition();

	while (pos)
	{
		m_mapHeat.GetNextAssoc(pos, dUnused, nHeat);
		ASSERT(nHeat);

		nTotalHeat += nHeat;
	}

	double dMeanHeat = ((double)nTotalHeat / m_mapHeat.GetCount());

	m_nMaxHeatCutoff = Misc::Round(dMeanHeat * 2);
	m_nMaxHeatCutoff = max(m_nMinHeatCutoff, m_nMaxHeatCutoff);
	
	return TRUE;
}

int CHeatMap::GetHeat(const COleDateTime& date) const
{
	ASSERT(CDateHelper::IsDateSet(date));

	int nHeat = 0;
	m_mapHeat.Lookup(CDateHelper::GetDateOnly(date).m_dt, nHeat);

	return nHeat;
}

COLORREF CHeatMap::GetColor(const COleDateTime& date) const
{
	ASSERT(m_aColorPalette.GetSize() >= 5);
	ASSERT(CDateHelper::IsDateSet(date));

	int nHeat = GetHeat(date);

	if (nHeat > 0)
	{
		nHeat = min(nHeat, m_nMaxHeatCutoff);

		int nColor = ((m_aColorPalette.GetSize() * nHeat) / m_nMaxHeatCutoff);
		nColor = min(nColor, m_aColorPalette.GetSize() - 1);

		if (nColor >= 0)
			return m_aColorPalette[nColor];
	}

	// else
	return CLR_NONE;
}

/////////////////////////////////////////////////////////////////////////////
