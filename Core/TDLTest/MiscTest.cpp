// FileMiscTest.cpp: implementation of the CMiscTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TDLTest.h"
#include "MiscTest.h"

#include "..\shared\Misc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const CString DEFSEP = (Misc::GetListSeparator() + ' ');
const int DEFSEPLEN = DEFSEP.GetLength();

const LPCTSTR NULLSTRING = NULL;

//////////////////////////////////////////////////////////////////////

CMiscTest::CMiscTest(const CTestUtils& utils) : CTDLTestBase(utils)
{

}

CMiscTest::~CMiscTest()
{
}

TESTRESULT CMiscTest::Run()
{
	ClearTotals();

	TestGetFormattedLength();
	TestFormatArray();

	return GetTotals();
}

void CMiscTest::TestGetFormattedLength()
{
	BeginTest(_T("Misc::GetFormattedLength"));

	{
		CStringArray aValues;
		aValues.Add(_T("a"));
		aValues.Add(_T("ab"));
		aValues.Add(_T("abc"));
		aValues.Add(_T("abcd"));

		// First 3 are equivalent and use default Windows separator
		ExpectEQ(Misc::GetFormattedLength(aValues), 10 + (3 * DEFSEPLEN));
		ExpectEQ(Misc::GetFormattedLength(aValues, NULL), 10 + (3 * DEFSEPLEN));
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("")), 10 + (3 * DEFSEPLEN));

		ExpectEQ(Misc::GetFormattedLength(aValues, _T(",")), 13);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("...")), 19);
	}
	
	{
		CStringArray aValues;
		aValues.Add(_T("a"));
		aValues.Add(_T(""));
		aValues.Add(_T("ab"));
		aValues.Add(_T(""));
		aValues.Add(_T("abc"));
		aValues.Add(_T(""));
		aValues.Add(_T("abcd"));

		// Excluding empty items
		ExpectEQ(Misc::GetFormattedLength(aValues), 10 + (3 * DEFSEPLEN));
		ExpectEQ(Misc::GetFormattedLength(aValues, NULL), 10 + (3 * DEFSEPLEN));
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("")), 10 + (3 * DEFSEPLEN));

		ExpectEQ(Misc::GetFormattedLength(aValues, _T(",")), 13);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("...")), 19);

		// Including empty items
		ExpectEQ(Misc::GetFormattedLength(aValues, NULL, TRUE), 10 + (6 * DEFSEPLEN));
		ExpectEQ(Misc::GetFormattedLength(aValues, _T(""), TRUE), 10 + (6 * DEFSEPLEN));

		ExpectEQ(Misc::GetFormattedLength(aValues, _T(","), TRUE), 16);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("..."), TRUE), 28);
	}
	
	{
		// Only empty items
		CStringArray aValues;
		aValues.Add(_T(""));
		aValues.Add(_T(""));
		aValues.Add(_T(""));

		// Excluding empty items
		ExpectEQ(Misc::GetFormattedLength(aValues), 0);
		ExpectEQ(Misc::GetFormattedLength(aValues, NULL), 0);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("")), 0);

		ExpectEQ(Misc::GetFormattedLength(aValues, _T(",")), 0);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("...")), 0);

		// Including empty items
		ExpectEQ(Misc::GetFormattedLength(aValues, NULL, TRUE), 2 * DEFSEPLEN);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T(""), TRUE), 2 * DEFSEPLEN);

		ExpectEQ(Misc::GetFormattedLength(aValues, _T(","), TRUE), 2);
		ExpectEQ(Misc::GetFormattedLength(aValues, _T("..."), TRUE), 6);
	}

	EndTest();
}

void CMiscTest::TestFormatArray()
{
	BeginTest(_T("Misc::FormatArray"));

	{
		CStringArray aValues;
		aValues.Add(_T("a"));
		aValues.Add(_T("ab"));
		aValues.Add(_T("abc"));
		aValues.Add(_T("abcd"));

		CString sDefCompare = _T("a") + DEFSEP + 
								_T("ab") + DEFSEP + 
								_T("abc") + DEFSEP + 
								_T("abcd");

		ExpectEQ(Misc::FormatArray(aValues), sDefCompare);
		ExpectEQ(Misc::FormatArray(aValues, NULLSTRING), sDefCompare);
		ExpectEQ(Misc::FormatArray(aValues, _T("")), sDefCompare);

		ExpectEQ(Misc::FormatArray(aValues, _T(",")), _T("a,ab,abc,abcd"));
		ExpectEQ(Misc::FormatArray(aValues, _T("...")), _T("a...ab...abc...abcd"));

		ExpectTrue(ActualLengthMatchesCalculation(aValues, NULL));
		ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("")));
		ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(",")));
		ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("...")));
	}

	{
		CStringArray aValues;
		aValues.Add(_T("a"));
		aValues.Add(_T(""));
		aValues.Add(_T("ab"));
		aValues.Add(_T(""));
		aValues.Add(_T("abc"));
		aValues.Add(_T(""));
		aValues.Add(_T("abcd"));

		// Excluding empty items
		{
			CString sDefCompare = _T("a") + DEFSEP + 
									_T("ab") + DEFSEP + 
									_T("abc") + DEFSEP + 
									_T("abcd");

			ExpectEQ(Misc::FormatArray(aValues), sDefCompare);
			ExpectEQ(Misc::FormatArray(aValues, NULLSTRING), sDefCompare);
			ExpectEQ(Misc::FormatArray(aValues, _T("")), sDefCompare);

			ExpectEQ(Misc::FormatArray(aValues, _T(",")), _T("a,ab,abc,abcd"));
			ExpectEQ(Misc::FormatArray(aValues, _T("...")), _T("a...ab...abc...abcd"));

			ExpectTrue(ActualLengthMatchesCalculation(aValues, NULL));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("")));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(",")));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("...")));
		}

		// Including empty items
		{
			CString sDefCompare = _T("a") + DEFSEP + 
									_T("") + DEFSEP + 
									_T("ab") + DEFSEP + 
									_T("") + DEFSEP + 
									_T("abc") + DEFSEP + 
									_T("") + DEFSEP +
									_T("abcd");

			ExpectEQ(Misc::FormatArray(aValues, NULLSTRING, TRUE), sDefCompare);
			ExpectEQ(Misc::FormatArray(aValues, _T(""), TRUE), sDefCompare);

			ExpectEQ(Misc::FormatArray(aValues, _T(","), TRUE), _T("a,,ab,,abc,,abcd"));
			ExpectEQ(Misc::FormatArray(aValues, _T("..."), TRUE), _T("a......ab......abc......abcd"));

			ExpectTrue(ActualLengthMatchesCalculation(aValues, NULL, TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(""), TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(","), TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(", "), TRUE));
		}
	}

	{
		// Only empty items
		CStringArray aValues;
		aValues.Add(_T(""));
		aValues.Add(_T(""));
		aValues.Add(_T(""));

		// Excluding empty items
		{
			ExpectEQ(Misc::FormatArray(aValues), _T(""));
			ExpectEQ(Misc::FormatArray(aValues, NULLSTRING), _T(""));
			ExpectEQ(Misc::FormatArray(aValues, _T("")), _T(""));

			ExpectEQ(Misc::FormatArray(aValues, _T(",")), _T(""));
			ExpectEQ(Misc::FormatArray(aValues, _T("...")), _T(""));

			ExpectTrue(ActualLengthMatchesCalculation(aValues, NULL));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("")));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(",")));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("...")));
		}

		// Including empty items
		{
			CString sDefCompare = _T("") + DEFSEP + _T("") + DEFSEP + _T("");

			ExpectEQ(Misc::FormatArray(aValues, NULLSTRING, TRUE), sDefCompare);
			ExpectEQ(Misc::FormatArray(aValues, _T(""), TRUE), sDefCompare);

			ExpectEQ(Misc::FormatArray(aValues, _T(","), TRUE), _T(",,"));
			ExpectEQ(Misc::FormatArray(aValues, _T("..."), TRUE), _T("......"));

			ExpectTrue(ActualLengthMatchesCalculation(aValues, NULL, TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(""), TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T(","), TRUE));
			ExpectTrue(ActualLengthMatchesCalculation(aValues, _T("..."), TRUE));
		}
	}

	EndTest();
}

BOOL CMiscTest::ActualLengthMatchesCalculation(const CStringArray& aValues, LPCTSTR szSep, BOOL bIncEmpty)
{
	return (Misc::FormatArray(aValues, szSep, bIncEmpty).GetLength() == 
			Misc::GetFormattedLength(aValues, szSep, bIncEmpty));
}
