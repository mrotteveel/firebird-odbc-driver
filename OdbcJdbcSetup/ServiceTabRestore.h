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

// ServiceTabRestore.h interface for the Service Restore class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(_ServiceTabRestore_h_)
#define _ServiceTabRestore_h_

namespace OdbcJdbcSetupLibrary {

/////////////////////////////////////////////////////////////////////////////
// CServiceTabRestore dialog

class CServiceTabRestore : public CServiceTabChild
{
public:
	CServiceTabRestore();
	~CServiceTabRestore();

public:
	void UpdateData( HWND hDlg, BOOL bSaveAndValidate = TRUE );
	bool createDialogIndirect( void );
	bool buildDlgChild( HWND hWndParent );
};

}; // end namespace OdbcJdbcSetupLibrary

#endif // !defined(_ServiceTabRestore_h_)