/*
 *  
 *     The contents of this file are subject to the Initial 
 *     Developer's Public License Version 1.0 (the "License"); 
 *     you may not use this file except in compliance with the 
 *     License. You may obtain a copy of the License at 
 *     http://www.ibphoenix.com/main.nfs?a=ibphoenix&page=ibp_idpl.
 *
 *     Software distributed under the License is distributed on 
 *     an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either 
 *     express or implied.  See the License for the specific 
 *     language governing rights and limitations under the License.
 *
 *
 *  The Original Code was created by Vladimir Tsvigun for IBPhoenix.
 *
 *  Copyright (c) 2005 Vladimir Tsvigun
 *  All Rights Reserved.
 */

// ServiceTabRestore.cpp: Service Restore Manager class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "OdbcJdbcSetup.h"
#include "../IscDbc/Connection.h"
#include "CommonUtil.h"
#include "DsnDialog.h"
#include "../SetupAttributes.h"
#include "ServiceTabChild.h"
#include "ServiceClient.h"
#include "ServiceTabRestore.h"

#undef _TR
#define _TR( id, msg ) msg

namespace OdbcJdbcSetupLibrary {

extern HINSTANCE m_hInstance;
extern int currentCP;

CServiceTabRestore::CServiceTabRestore() : CServiceTabChild()
{
}

CServiceTabRestore::~CServiceTabRestore()
{
}

void CServiceTabRestore::UpdateData( HWND hDlg, BOOL bSaveAndValidate )
{
}

BOOL CALLBACK wndproCServiceTabRestoreChild( HWND hWndChildTab, UINT message, UINT wParam, LONG lParam )
{
	switch ( message )
	{
    case WM_INITDIALOG:
		{
			HWND hWndParent = GetParent( hWndChildTab );
			PTAG_DIALOG_HEADER tabData = (PTAG_DIALOG_HEADER)GetWindowLong( hWndParent, GWL_USERDATA );
			RECT rcTabHdr;

			SetRectEmpty( &rcTabHdr ); 
			TabCtrl_AdjustRect( hWndParent, TRUE, &rcTabHdr );
			tabData->hWndChildTab = hWndChildTab;
			SetWindowPos( tabData->hWndChildTab, NULL, -rcTabHdr.left, -rcTabHdr.top, 0, 0, 
						  SWP_NOSIZE | SWP_NOZORDER );
		}
		return TRUE;

	case WM_DESTROY:
		{
			HWND hWndParent = GetParent( hWndChildTab );
			PTAG_DIALOG_HEADER tabData = (PTAG_DIALOG_HEADER)GetWindowLong( hWndParent, GWL_USERDATA );
			//ASSERT( tabData->hWndChildTab == hWndChildTab );
		}
		return TRUE;

	case WM_COMMAND:
        switch ( LOWORD( wParam ) ) 
		{
        case IDCANCEL:
			return TRUE;

        case IDOK:
            return TRUE;
        }
        break;
	}

    return FALSE;
}

bool CServiceTabRestore::createDialogIndirect()
{
	CreateDialogIndirect( m_hInstance,
						  resource,
						  parent,
						  wndproCServiceTabRestoreChild );
	return true;
}

bool CServiceTabRestore::buildDlgChild( HWND hWndParent )
{
	WORD *p;
	int nchar;
	DWORD lStyle;

	parent = hWndParent;
	resource = (LPDLGTEMPLATE)LocalAlloc( LPTR, 2048 );
	p = (PWORD)resource;

	lStyle = DS_SETFONT | WS_CHILD | WS_VISIBLE;
	
	*p++ = LOWORD (lStyle);
	*p++ = HIWORD (lStyle);
	*p++ = 0;          // LOWORD (lExtendedStyle)
	*p++ = 0;          // HIWORD (lExtendedStyle)

	*p++ = 28;         // NumberOfItems

	*p++ = 0;          // x
	*p++ = 0;          // y
	*p++ = 320;        // cx
	*p++ = 190;        // cy
	*p++ = 0;          // Menu
	*p++ = 0;          // Class

	/* copy the title of the dialog */
	nchar = nCopyAnsiToWideChar( p, TEXT( _TR( IDS_DLG_TITLE_SETUP, "FireBird ODBC Service" ) ) );
	p += nchar;

	*p++ = 8;          // FontSize
	nchar = nCopyAnsiToWideChar( p, TEXT( "MS Sans Serif" ) );
	p += nchar;

    TMP_EDITTEXT      ( IDC_BACKUP_FILE,7,10,246,12,ES_AUTOHSCROLL )
    TMP_PUSHBUTTON    ( "Browse",IDC_FIND_FILE_BACKUP,259,9,60,14 )
    TMP_EDITTEXT      ( IDC_DATABASE,7,35,246,12,ES_AUTOHSCROLL )
    TMP_PUSHBUTTON    ( "Browse",IDC_FIND_FILE,259,34,60,14 )
    TMP_EDITTEXT      ( IDC_USER,7,60,107,12,ES_UPPERCASE | ES_AUTOHSCROLL )
    TMP_EDITTEXT      ( IDC_PASSWORD,125,60,74,12,ES_PASSWORD | ES_AUTOHSCROLL )
    TMP_EDITTEXT      ( IDC_ROLE,212,60,107,12,ES_AUTOHSCROLL )
    TMP_COMBOBOX      ( IDC_COMBOBOX_PAGE_SIZE,111,77,100,120,CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP )
    TMP_BUTTONCONTROL ( "Deactivate indexes",IDC_CHECK_NO_INDEX,"Button", BS_AUTOCHECKBOX | WS_TABSTOP,16,97,74,10 )
    TMP_BUTTONCONTROL ( "Don't create shadows",IDC_CHECK_NO_SHADOW,"Button", BS_AUTOCHECKBOX | WS_TABSTOP,16,110,81,10 )
    TMP_BUTTONCONTROL ( "Don't validate constraints",IDC_CHECK_NO_VALIDITY, "Button",BS_AUTOCHECKBOX | WS_TABSTOP,16,123,91,10 )
    TMP_BUTTONCONTROL ( "Replace existing database",IDC_CHECK_REPLACE,"Button", BS_AUTOCHECKBOX | WS_TABSTOP,16,136,97,10 )
    TMP_BUTTONCONTROL ( "Use all space",IDC_CHECK_FULL_SPACE,"Button", BS_AUTOCHECKBOX | WS_TABSTOP,170,97,58,10 )
    TMP_BUTTONCONTROL ( "Creates a read only database",IDC_CHECK_NO_READONLY, "Button",BS_AUTOCHECKBOX | WS_TABSTOP,170,110,105,10 )
    TMP_BUTTONCONTROL ( "Commit each table",IDC_CHECK_COMMIT,"Button", BS_AUTOCHECKBOX | WS_TABSTOP,170,123,72,10 )
    TMP_EDITTEXT      ( IDC_SIZE_BUFFERS,272,134,39,12,ES_AUTOHSCROLL )
    TMP_DEFPUSHBUTTON ( "Start restore",IDOK,12,153,83,14 )
    TMP_PUSHBUTTON    ( "View log",IDC_BUTTON_VIEW_LOG,72,172,88,14 )
    TMP_PUSHBUTTON    ( "Save log",IDC_BUTTON_SAVE_LOG,169,172,85,14 )
    TMP_LTEXT         ( "Backup file",IDC_STATIC,7,0,218,8 )
    TMP_LTEXT         ( "Database",IDC_STATIC,7,25,218,8 )
    TMP_LTEXT         ( "Database Account",IDC_STATIC,7,50,107,8 )
    TMP_LTEXT         ( "Password",IDC_STATIC,126,50,72,8 )
    TMP_LTEXT         ( "Role",IDC_STATIC,214,50,105,8 )
    TMP_LTEXT         ( "Page size",IDC_STATIC,7,78,95,8 )
    TMP_GROUPBOX      ( "Restore options",IDC_STATIC,7,89,312,61 )
    TMP_EDITTEXT      ( IDC_EDIT_VIEW_LOG_LINE,100,154,219,13,ES_AUTOHSCROLL )
    TMP_LTEXT         ( "Buffers",IDC_STATIC,169,136,95,8 )

	return true;
}

}; // end namespace OdbcJdbcSetupLibrary