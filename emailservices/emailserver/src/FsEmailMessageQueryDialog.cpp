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
* Description:  Source file for FS Email message query dialog
*
*/


// INCLUDE FILES
//<cmail>
#include "emailtrace.h"
#include <uikon/eiksrvui.h>   // CEikServAppUi
//</cmail>
#include <StringLoader.h>

#include <fsmailserver.rsg>
//<cmail>
#include "fsmailserverconst.h"
//</cmail>

#include "FsEmailMessageQueryDialog.h"

// ---------------------------------------------------------
// CFsEmailMessageQueryDialog::CFsEmailMessageQueryDialog
// ---------------------------------------------------------
//
CFsEmailMessageQueryDialog::CFsEmailMessageQueryDialog( 
        TRequestStatus& aStatus,
        TDesC& aMailboxName,
        TFsEmailNotifierSystemMessageType aMessageType )
: CAknMessageQueryDialog( CAknQueryDialog::ENoTone ),
  iRequest( &aStatus ),
  iMessageType( aMessageType ),
  iMailboxName( aMailboxName )
    {
    FUNC_LOG;
    *iRequest = KRequestPending;
    }


// ---------------------------------------------------------
// CFsEmailMessageQueryDialog::~CFsEmailMessageQueryDialog
// ---------------------------------------------------------
//
CFsEmailMessageQueryDialog::~CFsEmailMessageQueryDialog()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------
// CFsEmailMessageQueryDialog::NewL
// ---------------------------------------------------------
//
CFsEmailMessageQueryDialog* CFsEmailMessageQueryDialog::NewLC( 
        TRequestStatus& aStatus,
        TDesC& aMailboxName,
        TFsEmailNotifierSystemMessageType aMessageType,
        const TDesC& aCustomMessageText )
    {
    FUNC_LOG;
    CFsEmailMessageQueryDialog* self = new( ELeave ) 
        CFsEmailMessageQueryDialog( aStatus, aMailboxName, aMessageType );

    self->PrepareDialogLC( aCustomMessageText );
    
    return self;
    }


// ---------------------------------------------------------
// CFsEmailMessageQueryDialog::OkToExitL
// ---------------------------------------------------------
//
TBool CFsEmailMessageQueryDialog::OkToExitL( TInt aButtonId )
    {
    FUNC_LOG;
    if ( CAknMessageQueryDialog::OkToExitL( aButtonId ) )
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




// ---------------------------------------------------------------------------
// Show asynchronous message query
// ---------------------------------------------------------------------------
//
void CFsEmailMessageQueryDialog::PrepareDialogLC( const TDesC& aCustomMessageText )
    {
    FUNC_LOG;
    PrepareLC( R_FS_MSERVER_DIALOG_MESSAGE_QUERY );

    if( GetResourceIdsL() )
        {
        SetMessageTextAndCbaToDialogL();
        }
    else
        {
        SetMessageTextAndCbaToDialogL( aCustomMessageText );
        }
    }

// ---------------------------------------------------------
// Get message text and CBA resource ids by input data
// ---------------------------------------------------------
//
TBool CFsEmailMessageQueryDialog::GetResourceIdsL()
    {
    FUNC_LOG;
    iAddMailboxNameToText = EFalse;
    switch( iMessageType )
        {
        // Custom error messages
        case EFsEmailNotifErrCustom:
            {
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            return EFalse;

        case EFsEmailNotifErrCustomResponse:
            {
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_CANCEL;
            }
            return EFalse;

        // No response
        case EFsEmailNotifErrAccountDisabled:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_ACCOUNT_DISABLED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;
        
        case EFsEmailNotifErrPasswordExpired:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_PASSWORD_EXPIRED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrDataDeleted:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_DATA_DELETED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrLoginUnsuccesfull:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_LOGIN_UNSUCCESSFUL;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrPasswordChanged:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_PASSWORD_CHANGED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrLoginCanceled:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_AUTH_CANCELED_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrServerNotAvailable:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SERVER_NOT_FOUND_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrNetworkNotAvailable:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_NETWORK_UNAVAILABLE_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrServerBusy:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SERVER_CURRENTLY_BUSY_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrUnableToConnectToServerTryAgain:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_UNABLE_TO_CONNECT_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrEmailAddrAndPwdDontMatch:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_INVALID_EMAIL_ADDRESS_AND_PASSWORD_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrInvalidEmailAddress:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_INVALID_EMAIL_ADDRESS;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrServerOffline:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SERVER_OFFLINE;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrRoamingTurnedOn:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_ROAMING_TURNED_ON;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrRoamingCosts:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_ROAMING_COSTS;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrUnableToComplete:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_UNABLE_TO_COMPLETE;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrConnectionError:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_CONNECTION_ERROR;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrUnableToConnect:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_UNABLE_TO_CONNECT;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrMultipleSyncErrors:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_MULTIPLE_SYNC_ERRORS;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrLoginFailed:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_LOGIN_FAILED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrServerNotFoundCheckSettings:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SERVER_CANNOT_FOUND_CHECK_SETTINGS;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrUnableToConnectToServer:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_UNABLE_TO_CONNECT_TO_SERVER;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrDisconnected:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_DISCONNECTED;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrServerErroTryAgain:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SERVER_ERROR_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            }
            break;

        case EFsEmailNotifErrOutOfMemory:
            {
            iMsgTxtResourceId = R_FS_MSERVER_SYNCH_NOT_ENOUGH_MEMORY;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_EMPTY;
            iAddMailboxNameToText = ETrue;
            }
            break;
            
        // Response
        case EFsEmailNotifErrSynchronizationFailed:
            {
            iMsgTxtResourceId = R_FS_MSERVER_TEXT_SYNC_FAILED_TRY_AGAIN;
            iCbaResourceId = R_AVKON_SOFTKEYS_OK_CANCEL;
            }
            break;

        // Wait note
        case EFsEmailNotifErrAuthenticatingWaitNote:
            {
            iMsgTxtResourceId = R_FS_MSERVER_AUTHENTICATING;
            iCbaResourceId = R_AVKON_SOFTKEYS_CANCEL;
            }
            break;

        // Error cases
        case EFsEmailNotifErrLastNoResponse:
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        }
    return ETrue;
    }

// -----------------------------------------------------------
// Set message text and CBA buttons
// -----------------------------------------------------------
//
void CFsEmailMessageQueryDialog::SetMessageTextAndCbaToDialogL()
    {
    FUNC_LOG;
    SetDialogHeadingL();
    
    HBufC* text( NULL );
    if( iAddMailboxNameToText )
        {
        text = StringLoader::LoadLC( iMsgTxtResourceId, iMailboxName );
        }
    else
        {
        text = StringLoader::LoadLC( iMsgTxtResourceId  );
        }
    SetMessageTextL( *text );
    CleanupStack::PopAndDestroy( text );
    
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandSetL( iCbaResourceId );
    }

// -----------------------------------------------------------
// Set message text and CBA buttons
// -----------------------------------------------------------
//
void CFsEmailMessageQueryDialog::SetMessageTextAndCbaToDialogL(
        const TDesC& aText )
    {
    FUNC_LOG;
    SetDialogHeadingL();
    
    SetMessageTextL( aText );

    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    cba.SetCommandSetL( iCbaResourceId );
    }

// -----------------------------------------------------------
// Set dialog heading text
// -----------------------------------------------------------
//
void CFsEmailMessageQueryDialog::SetDialogHeadingL()
    {
    FUNC_LOG;
    HBufC* text(NULL);
    if( iMailboxName != KNullDesC )
        {
        text = StringLoader::LoadLC( R_FS_MSERVER_MAILBOX_NAME, iMailboxName );
        }
    else
        {
        text = StringLoader::LoadLC( R_FS_MSERVER_APPLICATION_NAME );
        }

    QueryHeading()->SetTextL( *text );

    CleanupStack::PopAndDestroy( text );
    }


// End of File

