#if !defined(AFX_TDLSETREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_)
#define AFX_TDLSETREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDLReminderDlg.h : header file
//

#include "tdcstruct.h"
#include "tdcenum.h"
#include "TDLReminderPeriodComboBox.h"
#include "TDLDialog.h"

#include "..\shared\timecombobox.h"
#include "..\shared\soundedit.h"
#include "..\shared\enstatic.h"

/////////////////////////////////////////////////////////////////////////////

struct TDCREMINDER;

/////////////////////////////////////////////////////////////////////////////

#define IDDISMISS 0xff

/////////////////////////////////////////////////////////////////////////////

enum // DoModal flags
{
	TDCREM_NEWREMINDER		= 0x1,
	TDCREM_MULTIPLETASKS	= 0x2,
};

/////////////////////////////////////////////////////////////////////////////
// CTDLSetReminderDlg dialog

class CTDLSetReminderDlg : public CTDLDialog
{
// Construction
public:
	CTDLSetReminderDlg(CWnd* pParent = NULL);   // standard constructor

	int DoModal(TDCREMINDER& rem, DWORD dwFlags = 0);

protected:
// Dialog Data
	//{{AFX_DATA(CTDLSetReminderDlg)
	enum { IDD = IDD_SETREMINDER_DIALOG };
	//}}AFX_DATA
	CTimeComboBox	m_cbAbsoluteTime;
	CSoundEdit	m_ePlaySound;
	CTDLReminderPeriodComboBox m_cbLeadIn;
	CEnStatic m_stTaskTitle;

	int		m_bRelativeFromDueDate;
	double  m_dRelativeLeadInHours;
	CString	m_sSoundFile;
	int		m_bRelative;
	COleDateTime	m_dtAbsoluteDate;
	CString	m_sTaskTitle;
	double	m_dAbsoluteTime;
	CString m_sModifyDlgTitle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLSetReminderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLSetReminderDlg)
	afx_msg void OnSelchangeLeadin();
	afx_msg void OnChangeRelative();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	static COleDateTime GetNextNearestHour();
public:
	afx_msg void OnDismissReminder();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_)
