// TaskFileTest.cpp: implementation of the CXmlFileTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TDLTest.h"
#include "XmlFileTest.h"

#include "..\shared\Misc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlFileTest::CXmlFileTest(const CTestUtils& utils) : CTDLTestBase(utils)
{

}

CXmlFileTest::~CXmlFileTest()
{

}

TESTRESULT CXmlFileTest::Run()
{
	ClearTotals();

	TestAddSiblingPerformance();

	return GetTotals();
}

void CXmlFileTest::TestAddSiblingPerformance()
{
	if (!m_utils.HasCommandlineFlag('p'))
	{
		_tprintf(_T("Add '-p' to run CXmlFileTest::AddSiblingPerformance\n"));
		return;
	}

	BeginTest(_T("CXmlFileTest::TestAddSiblingPerformance"));
	
	for (int nLevel = 2, nNumTasks = 10; nLevel <= 5; nLevel++)
	{
		nNumTasks *= 10;
		TestAddSiblingPerformance(nNumTasks);
	}

	EndTest();
}

void CXmlFileTest::TestAddSiblingPerformance(int nNumItems)
{
	DWORD dwTickStart = GetTickCount();
	CXmlFile xml;

	for (int i = 0; i < nNumItems; i++)
	{
		xml.AddItem(_T("ITEM"), Misc::Format(_T("Item_%d"), i), XIT_ELEMENT);
	}

	DWORD dwDuration = (GetTickCount() - dwTickStart);
	_tprintf(_T("Test took %ld ms to add %d siblings\n"), dwDuration, nNumItems);
}

