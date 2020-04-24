// ExporterBridge.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "SpreadsheetContentControlBridge.h"
#include "resource.h"

#include <unknwn.h>
#include <tchar.h>
#include <msclr\auto_gcroot.h>

#include <Interfaces\ITransText.h>
#include <Interfaces\IPreferences.h>
#include <Interfaces\UITheme.h>
#include <Interfaces\ISpellcheck.h>

////////////////////////////////////////////////////////////////////////////////////////////////

#using <PluginHelpers.dll> as_friend

////////////////////////////////////////////////////////////////////////////////////////////////

using namespace SpreadsheetContentControl;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

using namespace Abstractspoon::Tdl::PluginHelpers;

////////////////////////////////////////////////////////////////////////////////////////////////

// REPLACE THIS WITH NEW GUID!
const LPCWSTR SPREADSHEET_GUID = L"BBDCAEDF-B297-4E09-BBFB-B308358628B9";
const LPCWSTR SPREADSHEET_NAME = L"Spreadsheet";

////////////////////////////////////////////////////////////////////////////////////////////////

CSpreadsheetContentBridge::CSpreadsheetContentBridge() : m_hIcon(NULL)
{
   HMODULE hMod = LoadLibrary(L"SpreadsheetContentControlBridge.dll"); // us

   m_hIcon = ::LoadIcon(hMod, MAKEINTRESOURCE(IDI_SPREADSHEET));
}

CSpreadsheetContentBridge::~CSpreadsheetContentBridge()
{
   ::DestroyIcon(m_hIcon);
}

void CSpreadsheetContentBridge::Release()
{
	delete this;
}

void CSpreadsheetContentBridge::SetLocalizer(ITransText* /*pTT*/)
{
	// TODO
}

LPCWSTR CSpreadsheetContentBridge::GetTypeDescription() const
{
	return SPREADSHEET_NAME;
}

HICON CSpreadsheetContentBridge::GetTypeIcon() const
{
   return m_hIcon;
}

LPCWSTR CSpreadsheetContentBridge::GetTypeID() const
{
	return SPREADSHEET_GUID;
}

IContentControl* CSpreadsheetContentBridge::CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, 
	long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
{
	CSpreadsheetContentControlBridge* pCtrl = new CSpreadsheetContentControlBridge();

	if (!pCtrl->Create(nCtrlID, nStyle, nLeft, nTop, nWidth, nHeight, hwndParent))
	{
		pCtrl->Release();
		pCtrl = NULL;
	}

	return pCtrl;
}

void CSpreadsheetContentBridge::SavePreferences(IPreferences* pPrefs, LPCWSTR szKey) const
{
	// TODO

}

void CSpreadsheetContentBridge::LoadPreferences(const IPreferences* pPrefs, LPCWSTR szKey, bool bAppOnly)
{
	// TODO
}

// returns the length of the html or zero if not supported
int CSpreadsheetContentBridge::ConvertToHtml(const unsigned char* pContent, int nLength,
	LPCWSTR szCharSet, LPWSTR& szHtml, LPCWSTR szImageDir)
{
	szHtml = nullptr;
	return 0;
}

void CSpreadsheetContentBridge::FreeHtmlBuffer(LPWSTR& szHtml)
{
	delete [] szHtml;
	szHtml = nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////

// This is the constructor of a class that has been exported.
// see ExporterBridge.h for the class definition
CSpreadsheetContentControlBridge::CSpreadsheetContentControlBridge()
{
}

BOOL CSpreadsheetContentControlBridge::Create(UINT nCtrlID, DWORD nStyle, 
	long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
{
	m_wnd = gcnew SpreadsheetContentControl::SpreadsheetContentControlCore(static_cast<IntPtr>(hwndParent));

	HWND hWnd = GetHwnd();

	if (hWnd)
	{
		::SetParent(hWnd, hwndParent);
		::SetWindowLong(hWnd, GWL_ID, nCtrlID);
		::SetWindowLong(hWnd, GWL_STYLE, nStyle);
		::MoveWindow(hWnd, nLeft, nTop, nWidth, nHeight, FALSE);

		return true;
	}

	return false;
}

int CSpreadsheetContentControlBridge::GetContent(unsigned char* pContent) const
{
	cli::array<Byte>^ content = m_wnd->GetContent();
	int nLength = content->Length;

	if (pContent && nLength)
	{
		pin_ptr<Byte> ptrContent = &content[content->GetLowerBound(0)];
		CopyMemory(pContent, ptrContent, nLength);
	}

	return nLength;
}

bool CSpreadsheetContentControlBridge::SetContent(const unsigned char* pContent, int nLength, bool bResetSelection)
{
	cli::array<Byte>^ content = gcnew cli::array<Byte>(nLength);

	for (int i = 0; i < nLength; i++)
		content[i] = pContent[i];

	return m_wnd->SetContent(content, bResetSelection);
}

LPCWSTR CSpreadsheetContentControlBridge::GetTypeID() const
{
	return SPREADSHEET_GUID;
}

// text content if supported. return false if not supported
int CSpreadsheetContentControlBridge::GetTextContent(LPWSTR szContent, int nLength) const
{
	String^ content = m_wnd->GetTextContent();
	nLength = content->Length;

	if (szContent != nullptr)
	{
		MarshalledString msContent(content);
		CopyMemory(szContent, msContent, (nLength * sizeof(WCHAR)));
	}

	return nLength;
}

bool CSpreadsheetContentControlBridge::SetTextContent(LPCWSTR szContent, bool bResetSelection)
{
	msclr::auto_gcroot<String^> content = gcnew String(szContent);

	return m_wnd->SetTextContent(content.get(), bResetSelection);
}

bool CSpreadsheetContentControlBridge::InsertTextContent(LPCWSTR szContent, bool bAtEnd)
{
	// TODO
	return false;
}

bool CSpreadsheetContentControlBridge::FindReplaceAll(LPCWSTR szFind, LPCWSTR szReplace, bool bCaseSensitive, bool bWholeWord)
{
	// TODO
	return false;
}

void CSpreadsheetContentControlBridge::SetReadOnly(bool bReadOnly)
{

}

void CSpreadsheetContentControlBridge::Enable(bool bEnable)
{
	m_wnd->Enabled = bEnable;
}

HWND CSpreadsheetContentControlBridge::GetHwnd() const
{
	return static_cast<HWND>(m_wnd->Handle.ToPointer());
}

void CSpreadsheetContentControlBridge::Release()
{
	::DestroyWindow(GetHwnd());
	delete this;
}

bool CSpreadsheetContentControlBridge::ProcessMessage(MSG* pMsg)
{
	return false;
}

ISpellCheck* CSpreadsheetContentControlBridge::GetSpellCheckInterface()
{
	return nullptr;
}

bool CSpreadsheetContentControlBridge::Undo()
{
	return false;
}

bool CSpreadsheetContentControlBridge::Redo()
{
	return false;
}

void CSpreadsheetContentControlBridge::SetUITheme(const UITHEME* pTheme)
{

}

void CSpreadsheetContentControlBridge::SetContentFont(HFONT hFont)
{

}

void CSpreadsheetContentControlBridge::SavePreferences(IPreferences* pPrefs, LPCWSTR szKey) const
{

}

void CSpreadsheetContentControlBridge::LoadPreferences(const IPreferences* pPrefs, LPCWSTR szKey, bool bAppOnly)
{

}