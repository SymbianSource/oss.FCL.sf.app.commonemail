/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Source file for FS Email authentication dialog
*
*/


// INCLUDE FILES

#include "emailtrace.h"
#include "FsEmailAuthenticationDialog.h"

// ---------------------------------------------------------
// CFsEmailAuthenticationDialog::CFsEmailAuthenticationDialog
// ---------------------------------------------------------
//
CFsEmailAuthenticationDialog::CFsEmailAuthenticationDialog(
        TRequestStatus& aStatus,
        TDesC& aMailboxName,
        TDes& aPassword )
: CAknTextQueryDialog( aPassword, CAknQueryDialog::ENoTone ),
  iRequest( &aStatus ),
  iMailboxName( aMailboxName )
    {
    FUNC_LOG;
    *iRequest = KRequestPending;
    }


// ---------------------------------------------------------
// CFsEmailAuthenticationDialog::~CFsEmailAuthenticationDialog
// ---------------------------------------------------------
//
CFsEmailAuthenticationDialog::~CFsEmailAuthenticationDialog()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CFsEmailAuthenticationDialog::NewL
// ---------------------------------------------------------
//
CFsEmailAuthenticationDialog* CFsEmailAuthenticationDialog::NewL( 
                                        TRequestStatus& aStatus, 
                                        TDesC& aMailboxName,
                                        TDes& aPassword )
    {
    FUNC_LOG;
    CFsEmailAuthenticationDialog* dialog = new( ELeave ) 
        CFsEmailAuthenticationDialog( aStatus, aMailboxName, aPassword );

    return dialog;
    }


// ---------------------------------------------------------
// CFsEmailAuthenticationDialog::PreLayoutDynInitL()
// ---------------------------------------------------------
//
void CFsEmailAuthenticationDialog::PreLayoutDynInitL()
    {
    FUNC_LOG;
    CAknTextQueryDialog::PreLayoutDynInitL();

	CAknPopupHeadingPane* heading = QueryHeading();
	if( heading )
		{
		heading->SetTextL( iMailboxName );
		}
    }

// ---------------------------------------------------------
// CFsEmailAuthenticationDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CFsEmailAuthenticationDialog::OkToExitL( TInt aButtonId )
    {
    FUNC_LOG;
    if ( CAknTextQueryDialog::OkToExitL( aButtonId ) )
        {
        if ( aButtonId == EAknSoftkeySelect || 
             aButtonId == EAknSoftkeyOk || 
             aButtonId == EAknSoftkeyDone )
            {
			// Ignore any errors to avoid the situation that the notifier keeps
			// hanging in screen if the calling party is crashed etc.
            User::RequestComplete( iRequest, KErrNone );
            }
        else //if ( aButtonId == EAknSoftkeyCancel )
            {
			// Ignore any errors to avoid the situation that the notifier keeps
			// hanging in screen if the calling party is crashed etc.
            User::RequestComplete( iRequest, KErrCancel );
            }
        	
        return ETrue;
        }

    return EFalse;
    }

// End of File

