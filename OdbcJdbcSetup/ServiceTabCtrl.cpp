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

// ServiceTabCtrl.cpp: Service TabControl Manager class.
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
#include "ServiceClient.h"
#include "ServiceTabCtrl.h"

#undef _TR
#define _TR( id, msg ) msg

namespace OdbcJdbcSetupLibrary {

extern HINSTANCE m_hInstance;
extern int currentCP;

CServiceTabCtrl::CServiceTabCtrl()
{
}

CServiceTabCtrl::~CServiceTabCtrl()
{
}

void CServiceTabCtrl::SetDisabledDlgItem( HWND hDlg, int ID, BOOL bDisabled )
{
	HWND hWnd = GetDlgItem( hDlg, ID );
	int style = GetWindowLong( hWnd, GWL_STYLE );
	if ( bDisabled )style |= WS_DISABLED;
	else			style &= ~WS_DISABLED;
	SetWindowLong( hWnd, GWL_STYLE, style );
	InvalidateRect( hWnd, NULL, TRUE );
}

void CServiceTabCtrl::UpdateData( HWND hDlg, BOOL bSaveAndValidate )
{
}

BOOL CServiceTabCtrl::OnInitDialog( HWND hDlg )
{
	HWND hWndTab = GetDlgItem( hDlg, IDC_SERVICE_TABCTRL );
    TCITEM tie;

    tie.mask = TCIF_TEXT | TCIF_IMAGE; 
    tie.iImage = -1; 
	tabData.hDlg = hDlg;
	tabData.hWndTab = hWndTab;
    tie.lParam = (ULONG)&tabData;

	tabData.childTab[0] = backup.getObject();
	backup.buildDlgChild( hWndTab );
    tie.pszText = " Backup "; 
    TabCtrl_InsertItem( hWndTab, 0, &tie );

	tabData.childTab[1] = restore.getObject();
	restore.buildDlgChild( hWndTab );
    tie.pszText = " Restore "; 
    TabCtrl_InsertItem( hWndTab, 1, &tie );

	tabData.childTab[2] = repair.getObject();
	repair.buildDlgChild( hWndTab );
    tie.pszText = " Validate "; 
    TabCtrl_InsertItem( hWndTab, 2, &tie );

    SetWindowLong( hWndTab, GWL_USERDATA, (ULONG)&tabData ); 
	backup.createDialogIndirect(); 

	return TRUE;
}

BOOL CALLBACK wndproCServiceTabCtrl( HWND hDlg, UINT message, WORD wParam, LONG lParam )
{
	switch ( message )
	{
    case WM_INITDIALOG:

	    SetWindowLong( hDlg, GWL_USERDATA, (ULONG)lParam ); 

		if ( !((CServiceTabCtrl*)lParam)->OnInitDialog( hDlg ) )
			return FALSE;

		((CServiceTabCtrl*)lParam)->UpdateData( hDlg, FALSE );
		return TRUE;

	case WM_COMMAND:
        switch ( LOWORD( wParam ) ) 
		{
        case IDCANCEL:
			EndDialog( hDlg, FALSE );
			return TRUE;

        case IDOK:
			{
				CServiceTabCtrl *serviceTabCtrl = (CServiceTabCtrl*)GetWindowLong( hDlg, GWL_USERDATA );
				serviceTabCtrl->UpdateData( hDlg );
				EndDialog( hDlg, TRUE );
			}
            return TRUE;
        }
        break;

    case WM_NOTIFY:
		{
			if ( wParam == IDC_SERVICE_TABCTRL )
			{
				NMHDR * hdr = (NMHDR*)lParam;
				HWND hWndTab = GetDlgItem( hDlg, IDC_SERVICE_TABCTRL );
				PTAG_DIALOG_HEADER tabData = (PTAG_DIALOG_HEADER)GetWindowLong( hWndTab, GWL_USERDATA );
				int iPage = TabCtrl_GetCurSel( hWndTab );
				int focus = TabCtrl_GetCurFocus( hWndTab );

				switch ( hdr->code )
				{
				case TCN_SELCHANGE:
					if ( !tabData->hWndChildTab )
						tabData->childTab[iPage]->createDialogIndirect();
					break;

				case TCN_SELCHANGING:
					if ( tabData->hWndChildTab )
					{
						DestroyWindow( tabData->hWndChildTab ); 
						tabData->hWndChildTab = NULL;
					}
					break;
				}
			}
		}
        break; 
	}
    return FALSE ;
}

int CServiceTabCtrl::DoModal()
{
  	HWND hwnd = NULL;
	WORD *p, *pdlgtemplate;
	int nchar;
	DWORD lStyle;

	pdlgtemplate = p = (PWORD) LocalAlloc( LPTR, 512 );
	lStyle = DS_SETFONT | DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE;

	*p++ = LOWORD (lStyle);
	*p++ = HIWORD (lStyle);
	*p++ = 0;          // LOWORD (lExtendedStyle)
	*p++ = 0;          // HIWORD (lExtendedStyle)

	*p++ = 3;          // NumberOfItems

	*p++ = 0;          // x
	*p++ = 0;          // y
	*p++ = 342;        // cx
	*p++ = 234;        // cy
	*p++ = 0;          // Menu
	*p++ = 0;          // Class

	/* copy the title of the dialog */
	nchar = nCopyAnsiToWideChar( p, TEXT( _TR( IDS_DLG_TITLE_SETUP, "FireBird ODBC Service" ) ) );
	p += nchar;

	*p++ = 8;          // FontSize
	nchar = nCopyAnsiToWideChar( p, TEXT("MS Sans Serif" ) );
	p += nchar;

    TMP_DEFPUSHBUTTON ( _TR( IDS_BUTTON_OK, "OK" ), IDOK,86,216,60,14 )
    TMP_PUSHBUTTON    ( _TR( IDS_BUTTON_CANCEL, "Cancel" ), IDCANCEL,154,216,60,14 )
    TMP_TABCONTROL    ( _TR( IDS_BUTTON_OK, "Tab1" ), IDC_SERVICE_TABCTRL, "SysTabControl32",0x0,7,7,328,204 )

	int nRet = DialogBoxIndirectParam( m_hInstance, (LPDLGTEMPLATE) pdlgtemplate, hwnd, (DLGPROC)wndproCServiceTabCtrl, (ULONG)this );
	LocalFree( LocalHandle( pdlgtemplate ) );

	return nRet;
}

}; // end namespace OdbcJdbcSetupLibrary