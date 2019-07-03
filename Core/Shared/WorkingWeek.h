// DateHelper.h: interface for the CDateHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORKINGWEEK_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
#define AFX_WORKINGWEEK_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

enum WD_DAYOFWEEK 
{
	WD_NONE			= 0X00,

	WD_SUNDAY		= 0X01,
	WD_MONDAY		= 0X02,
	WD_TUESDAY		= 0X04,
	WD_WEDNESDAY	= 0X08,
	WD_THURSDAY		= 0X10,
	WD_FRIDAY		= 0X20,
	WD_SATURDAY		= 0X40,

	WD_EVERYDAY		= 0x7F
};

//////////////////////////////////////////////////////////////////////

class CWorkingDay
{
public:
	CWorkingDay(); // uses static initialisation set up by CWorkingWeek
	CWorkingDay(double dStartOfDayInHours,		// eg.9
				double dWorkingLengthInHours,	// eg.8
				double dStartOfLunchInHours,	// eg.12
				double dEndOfLunchInHours);		// eg.13

	static void Initialise(const CWorkingDay& workday);

	BOOL Initialise(double dStartOfDayInHours,		
					double dWorkingLengthInHours,	
					double dStartOfLunchInHours,	
					double dEndOfLunchInHours);		

	static BOOL IsValid(double dStartOfDayInHours,
						double dWorkingLengthInHours,
						double dStartOfLunchInHours,
						double dEndOfLunchInHours);

	double GetStartOfDayInHours() const;
	double GetEndOfDayInHours() const;
	double GetStartOfLunchInHours() const;
	double GetEndOfLunchInHours() const;

	COleDateTime GetStartOfDay(const COleDateTime& date) const;
	COleDateTime GetEndOfDay(const COleDateTime& date) const;
	COleDateTime GetStartOfLunch(const COleDateTime& date) const;
	COleDateTime GetEndOfLunch(const COleDateTime& date) const;

	double CalculateDurationInHours(double fromHour, double toHour) const;
	double GetDayLengthInHours(bool bIncludingLunch = false) const;
	double GetLunchLengthInHours() const;

	static double GetTimeOfDayInHours(const COleDateTime& date);
	
protected:
	double m_dStartOfDayInHours;
	double m_dWorkingLengthInHours;
	double m_dStartOfLunchInHours;
	double m_dEndOfLunchInHours;

protected:
};


//////////////////////////////////////////////////////////////////////

class CWeekend
{
public:
	CWeekend();	// uses static initialisation set up by CWorkingWeek
	CWeekend(DWORD dwDays); // eg. WD_SATURDAY | WD_SUNDAY

	static void Initialise(const CWeekend& weekend);

	BOOL Initialise(DWORD dwDays);

	static BOOL IsValid(DWORD dwDays);

	DWORD GetWeekend() const { return m_dwDays; }
	DWORD GetWeekdays() const { return (WD_EVERYDAY & ~m_dwDays); }

	BOOL IsWeekend(WD_DAYOFWEEK nDOW) const;
	BOOL IsWeekend(OLE_DAYOFWEEK nDOW) const;
	BOOL IsWeekend(const COleDateTime& date) const;
	BOOL IsWeekend(double dDate) const;

	int GetDuration() const;

protected:
	DWORD m_dwDays;
	int m_nDuration;
};

//////////////////////////////////////////////////////////////////////

class CWorkingWeek
{
public:
	CWorkingWeek(); // uses static initialisation
	CWorkingWeek(DWORD dwWeekendDays,			// eg. (DHW_SATURDAY | DHW_SUNDAY),
				 double dStartOfDayInHours,		// eg. 9
				 double dWorkingLengthInHours,	// eg. 8
				 double dStartOfLunchInHours,	// eg. 12
				 double dEndOfLunchInHours);	// eg. 13

	static void Initialise(const CWorkingWeek& week);
	
	BOOL Initialise(DWORD dwWeekendDays,			// eg. (DHW_SATURDAY | DHW_SUNDAY),
					double dStartOfDayInHours,		// eg. 9
					double dWorkingLengthInHours,	// eg. 8
					double dStartOfLunchInHours,	// eg. 12
					double dEndOfLunchInHours);		// eg. 13

	double CalculateDurationInHours(const COleDateTime& dtFrom, const COleDateTime& dtTo);
	double CalculateDurationInDays(const COleDateTime& dtFrom, const COleDateTime& dtTo);
	double CalculateDurationInWeeks(const COleDateTime& dtFrom, const COleDateTime& dtTo);

	BOOL MakeWeekday(COleDateTime& date, BOOL bForwards = TRUE, BOOL bTruncateTime = TRUE) const;
	COleDateTime ToWeekday(const COleDateTime& date, BOOL bForwards = TRUE) const;

	BOOL HasWeekend() const;

	const CWorkingDay& WorkDay() const { return m_WorkDay;	}
	const CWeekend& Weekend() const { return m_Weekend; }

protected:
	CWorkingDay m_WorkDay;
	CWeekend m_Weekend;
};

//////////////////////////////////////////////////////////////////////

class CTwentyFourSevenWeek : public CWorkingWeek
{
public:
	CTwentyFourSevenWeek() 
		: 
		CWorkingWeek(0,			// no weekend  
					 0,			// 12 am
					 24,		// 24 hours
					 12, 12)	// no lunch
	{
	}

};



//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_WORKINGWEEK_H__2A4E63F6_A106_4295_BCBA_06D03CD67AE7__INCLUDED_)
