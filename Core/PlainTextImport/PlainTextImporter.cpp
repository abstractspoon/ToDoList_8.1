// PlainTextImporter.cpp: implementation of the CPlainTextImporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlainTextImporter.h"
#include "optionsdlg.h"

#include "..\shared\misc.h"

#include "..\Interfaces\ITasklist.h"
#include "..\Interfaces\IPreferences.h"

#include "..\3rdParty\stdiofileex.h"

#include <time.h>
#include <unknwn.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlainTextImporter::CPlainTextImporter()
{

}

CPlainTextImporter::~CPlainTextImporter()
{

}

void CPlainTextImporter::SetLocalizer(ITransText* /*pTT*/)
{
	//CLocalizer::Initialize(pTT);
}

bool CPlainTextImporter::InitConsts(DWORD dwFlags, IPreferences* pPrefs, LPCTSTR szKey)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString sKey(szKey);
	sKey += _T("\\PlainText");

	WANTPROJECT = pPrefs->GetProfileInt(szKey, _T("IncludeProject"), FALSE);
	INDENT = pPrefs->GetProfileString(szKey, _T("Indent"), _T("  "));

	BOOL bSilent = ((dwFlags & IIEF_SILENT) != 0);

	if (!bSilent)
	{
		COptionsDlg dlg(TRUE, WANTPROJECT, INDENT);

		if (dlg.DoModal() != IDOK)
			return false;
		
		INDENT = dlg.GetIndent();
		WANTPROJECT = dlg.GetWantProject();

		pPrefs->WriteProfileInt(szKey, _T("IncludeProject"), WANTPROJECT);
		pPrefs->WriteProfileString(szKey, _T("Indent"), INDENT);
	}

	return true;
}

IIMPORTEXPORT_RESULT CPlainTextImporter::Import(LPCTSTR szSrcFilePath, ITaskList* pDestTaskFile, DWORD dwFlags, IPreferences* pPrefs, LPCTSTR szKey)
{
	ITaskList8* pTasks = GetITLInterface<ITaskList8>(pDestTaskFile, IID_TASKLIST8);
	
	if (pTasks == NULL)
	{
		ASSERT(0);
		return IIER_BADINTERFACE;
	}
	
	if (!InitConsts(dwFlags, pPrefs, szKey))
		return IIER_CANCELLED;

	CStdioFileEx file;

	if (!file.Open(szSrcFilePath, CFile::modeRead))
		return IIER_BADFILE;

	// the first line can be the project name
	if (WANTPROJECT)
	{
		CString sProjectName;
		file.ReadString(sProjectName);
		Misc::Trim(sProjectName);

		pTasks->SetProjectName(sProjectName);
	}

	// what follows are the tasks, indented to express subtasks
	int nLastDepth = 0;
	HTASKITEM hLastTask = NULL;

	ROOTDEPTH = -1; // gets set to the first task's depth

	CString sLine;
	
	while (file.ReadString(sLine)) 
	{
		CString sTitle, sComments;

		if (!GetTitleComments(sLine, sTitle, sComments))
			continue;

		// find the appropriate parent fro this task
		HTASKITEM hParent = NULL;
		int nDepth = GetDepth(sLine);

		if (nDepth == nLastDepth) // sibling
		{
			hParent = hLastTask ? pTasks->GetTaskParent(hLastTask) : NULL;
		}
		else if (nDepth > nLastDepth) // child
		{
			hParent = hLastTask;
		}
		else if (hLastTask) // we need to work up the tree
		{
			hParent = pTasks->GetTaskParent(hLastTask);

			while (hParent && nDepth < nLastDepth)
			{
				hParent = pTasks->GetTaskParent(hParent);
				nLastDepth--;
			}
		}
		
		HTASKITEM hTask = pTasks->NewTask(sTitle, hParent, 0);

		if (!sComments.IsEmpty())
			pTasks->SetTaskComments(hTask, sComments);

		// update state
		hLastTask = hTask;
		nLastDepth = nDepth;
	}

	return IIER_SUCCESS;
}

int CPlainTextImporter::GetDepth(const CString& sLine)
{
	if (INDENT.IsEmpty() || sLine.IsEmpty())
		return 0;

	// else
	int nDepth = 0;
	
	if (INDENT == "\t")
	{
		while (nDepth < sLine.GetLength())
		{
			if (sLine[nDepth] == '\t')
				nDepth++;
			else
				break;
		}
	}
	else // one or more spaces
	{
		int nPos = 0;

		while (nPos < sLine.GetLength())
		{
			if (sLine.Find(INDENT, nPos) == nPos)
				nDepth++;
			else
				break;

			// next
			nPos = nDepth * INDENT.GetLength();
		}
	}

	// set root depth if not set 
	if (ROOTDEPTH == -1)
		ROOTDEPTH = nDepth;

	// and take allowance for it
	nDepth -= ROOTDEPTH;

	return nDepth;
}

BOOL CPlainTextImporter::GetTitleComments(const CString& sLine, 
										  CString& sTitle, CString& sComments)
{
	sTitle = sLine;
	Misc::Split(Misc::Trim(sTitle), sComments, '|');
	
	if (!sComments.IsEmpty())
	{
		// comments replace [\][n] with [\n]
		sComments.Replace(_T("\\n"), _T("\n"));
	}

	return !sTitle.IsEmpty();
}
