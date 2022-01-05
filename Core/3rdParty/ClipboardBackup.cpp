// ClipboardBackup.cpp: implementation of the CClipboardBackup class.
//
// Copyright 2006 (c) RegExLab.com
//
// Author: ʷ��ΰ (sswater shi)
//
// 2006/05/20 02:03:04
//

#include "stdafx.h"
#include "ClipboardBackup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClipboardBackup::CClipboardBackup(HWND hWnd) : m_hWnd(hWnd)
{
	ASSERT(hWnd && IsWindow(hWnd));
}

CClipboardBackup::~CClipboardBackup()
{
	Restore();
	Cleanup();
}

BOOL CClipboardBackup::AddData(LPCTSTR szData, UINT nFormat)
{
	if (!szData || !*szData || !nFormat)
		return FALSE;

	ClipboardData data = { nFormat, 0, NULL };
	BOOL bResult = FALSE;
	
	try
	{
		// Allocate a global memory object for the text. 
		size_t nBytes = (lstrlen(szData) * sizeof(TCHAR));
		data.m_hData = ::GlobalAlloc(GMEM_MOVEABLE, nBytes); 
		
		if (data.m_hData) 
		{ 
			// Lock the handle and copy the text to the buffer.
			LPVOID pCopy = ::GlobalLock(data.m_hData);
			CopyMemory(pCopy, szData, nBytes); 
			::GlobalUnlock(data.m_hData); 
			
			// Place the handle on the clipboard.
			m_lstData.AddTail(data);
			bResult = TRUE;
		}
	}
	catch(...)
	{
	}
	
	// cleanup
	if (!bResult && data.m_hData)
		::GlobalFree(data.m_hData);

	return bResult;
}

BOOL CClipboardBackup::Backup()
{
	if (m_lstData.GetCount())
		return FALSE;

	if (!::OpenClipboard(NULL))
		return FALSE;

	try
	{
		UINT format = 0;
		while((format = ::EnumClipboardFormats(format)) != 0)
		{
			ClipboardData data;
			data.m_nFormat = format;

			// skip some formats
			if (format == CF_BITMAP || format == CF_METAFILEPICT || format == CF_PALETTE || format == CF_OWNERDISPLAY ||
				format == CF_DSPMETAFILEPICT || format == CF_DSPBITMAP ||
				(format >= CF_PRIVATEFIRST && format <= CF_PRIVATELAST))
			{
				continue;
			}

			// get format name
			if (format <= 14)
			{
				data.m_szFormatName[0] = 0;
			}
			else if (GetClipboardFormatName(format, data.m_szFormatName, 100) == 0)
			{
				data.m_szFormatName[0] = 0;
			}

			// get handle
			HANDLE hMem = ::GetClipboardData(format);
			if (hMem == NULL)
				continue;

			// copy handle
			switch(format)
			{
			case CF_ENHMETAFILE:
			case CF_DSPENHMETAFILE:
				data.m_hData = ::CopyMetaFile((HMETAFILE)hMem, NULL);
				break;

			default:
				{
					int    size = ::GlobalSize(hMem);
					LPVOID pMem = ::GlobalLock(hMem);

					data.m_hData   = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, size);
					LPVOID pNewMem = ::GlobalLock(data.m_hData);

					memcpy(pNewMem, pMem, size);

					::GlobalUnlock(hMem);
					::GlobalUnlock(data.m_hData);
				}
			}

			m_lstData.AddTail(data);
		}
	}
	catch (...)
	{
		// do nothing
	}

	::CloseClipboard(); // always

	return TRUE;
}

BOOL CClipboardBackup::Restore()
{
	if (!m_lstData.GetCount())
		return FALSE;

	if (::OpenClipboard(m_hWnd))
	{
		try
		{
			VERIFY(::EmptyClipboard());

			POSITION pos = m_lstData.GetHeadPosition();

			while (pos != NULL)
			{
				ClipboardData & data = m_lstData.GetNext(pos);
				UINT format = data.m_nFormat;

				if (data.m_szFormatName[0] != 0)
				{
					UINT u = RegisterClipboardFormat(data.m_szFormatName);
					if (u > 0) 
						format = u;
				}

				if (::SetClipboardData(format, data.m_hData))
				{
					data.m_hData = NULL; // clipboard owns it now
				}
			}

			m_lstData.RemoveAll();
		}
		catch (...)
		{
		}

		VERIFY(::CloseClipboard()); // always
	}
	
	return !m_lstData.GetCount();
}

void CClipboardBackup::Cleanup()
{
	POSITION pos = m_lstData.GetHeadPosition();

	while (pos != NULL)
	{
		ClipboardData & data = m_lstData.GetNext(pos);

		if (data.m_hData)
			::GlobalFree(data.m_hData);
	}

	m_lstData.RemoveAll();
}