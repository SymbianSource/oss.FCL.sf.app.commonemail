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
* Description: This file implements class CFSNotificationHandlerNotifierInitiator.
*
*/


//<cmail>
//#include <eikenv.h>
//#include <aknappui.h>
//#include <aknmessagequerydialog.h>
//#include <eiksrvui.h>   // CEikServAppUi
//</cmail>

#include "emailtrace.h"
#include <fsmailserver.rsg>
//<cmail>
#include "fsmailserverconst.h"
//</cmail>

#include "fsnotificationhandlernotifierinitiator.h"
#include "fsnotificationhandlermgr.h"
#include "fsmailoutofmemoryhandler.h"
//<cmail>
//#include "FsEmailGlobalDialogsAppUi.h"
//</cmail>
#include "FsEmailAuthenticationDialog.h"
#include "FsEmailMessageQueryDialog.h"

// ======== MEMBER FUNCTIONS ========

CFSNotificationHandlerNotifierInitiator::CFSNotificationHandlerNotifierInitiator(
    MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
    MFSNotificationHandlerMgr& aNotificationHandlerMgr ) :
    CActive( EPriorityStandard ),
    iOwner( aOwner ),
    iNotificationHandlerMgr( aNotificationHandlerMgr ),
    iMailBoxId(),
    iCallback( NULL )
    {
    FUNC_LOG;
    iMessageType = static_cast<TFsEmailNotifierSystemMessageType>( -1 );
    }

void CFSNotificationHandlerNotifierInitiator::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    User::LeaveIfError( iNotifier.Connect() );
    }

CFSNotificationHandlerNotifierInitiator* CFSNotificationHandlerNotifierInitiator::NewL(
    MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
    MFSNotificationHandlerMgr& aNotificationHandlerMgr )
    {
    FUNC_LOG;
    CFSNotificationHandlerNotifierInitiator* self = CFSNotificationHandlerNotifierInitiator::NewLC(
        aOwner, aNotificationHandlerMgr );
    CleanupStack::Pop( self );
    return self;
    }

CFSNotificationHandlerNotifierInitiator* CFSNotificationHandlerNotifierInitiator::NewLC(
    MFSNotificationHandlerNotifierInitiatorObserver& aOwner,
    MFSNotificationHandlerMgr& aNotificationHandlerMgr )
    {
    FUNC_LOG;
    CFSNotificationHandlerNotifierInitiator* self = new( ELeave ) CFSNotificationHandlerNotifierInitiator(
            aOwner, aNotificationHandlerMgr );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CFSNotificationHandlerNotifierInitiator::SetMailBoxId(
    TFSMailMsgId aNewMailBoxId )
    {
    FUNC_LOG;
    iMailBoxId.SetPluginId( aNewMailBoxId.PluginId() );
    iMailBoxId.SetId( aNewMailBoxId.Id() );
    }

TFSMailMsgId CFSNotificationHandlerNotifierInitiator::MailBoxId() const
    {
    FUNC_LOG;
    return iMailBoxId;
    }

void CFSNotificationHandlerNotifierInitiator::SetMessageType(
        TFsEmailNotifierSystemMessageType aMessageType )
    {
    FUNC_LOG;
    iMessageType = aMessageType;
    }

TFsEmailNotifierSystemMessageType CFSNotificationHandlerNotifierInitiator::MessageType() const
    {
    FUNC_LOG;
    return iMessageType;
    }

void CFSNotificationHandlerNotifierInitiator::SetCallback(
        MFSMailExceptionEventCallback* aCallback )
    {
    FUNC_LOG;
    iCallback = aCallback;
    }

MFSMailExceptionEventCallback* CFSNotificationHandlerNotifierInitiator::Callback() const
    {
    FUNC_LOG;
    return iCallback;
    }


CFSNotificationHandlerNotifierInitiator::
    ~CFSNotificationHandlerNotifierInitiator()
    {
    Cancel();
    iNotifier.Close();
    }

void CFSNotificationHandlerNotifierInitiator::BeforeDialog()
    {
    FUNC_LOG;
    iLogin = EFalse;
    iNotificationHandlerMgr.IncreaseDialogCount();
    //<cmail>
    //iNotificationHandlerMgr.AppUi().BringToForeground();
    iNotificationHandlerMgr.BringAppUiToForeground();
    //</cmail>
    }

void CFSNotificationHandlerNotifierInitiator::AfterDialog()
    {
    FUNC_LOG;
    // Decrease the dialog count and send mail server to background
    // if this was last active dialog.
    iNotificationHandlerMgr.DecreaseDialogCount();
    if( iNotificationHandlerMgr.GetDialogCount() <= 0 )
        {
        //<cmail>
        //iNotificationHandlerMgr.AppUi().SendToBackground();
        iNotificationHandlerMgr.SendAppUiToBackground();
        //</cmail>
        }
    }

void CFSNotificationHandlerNotifierInitiator::RequestMessageQueryL(
    TDesC& aMailboxName,
    TFsEmailNotifierSystemMessageType aMessageType,
    const TDesC& aCustomMessageText )
    {
    FUNC_LOG;
    Cancel();

    // <cmail>
    BeforeDialog();
    TRAPD( err, iNotificationHandlerMgr.MessageQueryL( aMailboxName, iStatus, aCustomMessageText, aMessageType ) );
    // </cmail>

    if( KErrNone != err )
        {
        AfterDialog();
        User::Leave( err );
        }
    SetActive();
    }

void CFSNotificationHandlerNotifierInitiator::RequestAuthenticationL(
    TDesC& aMailboxName )
    {
    FUNC_LOG;

    Cancel();
    iPassword.Zero();
    //<cmail>
    BeforeDialog();

    TInt err( iNotificationHandlerMgr.AuthenticateL( iPassword, aMailboxName, iStatus ) );
    //</cmail>

    if( err != KErrNone )
        {
        AfterDialog();
        User::Leave( err );
        }

    SetActive();
    }

void CFSNotificationHandlerNotifierInitiator::GetLastAuthenticationOutput(
    TBuf<KFsEmailNotifierPasswordMaxLength>& aPassword ) const
    {
    FUNC_LOG;
    aPassword = iPassword;
    }

TBool CFSNotificationHandlerNotifierInitiator::GetLastResponse() const
    {
    FUNC_LOG;
    return iLogin;
    }

void CFSNotificationHandlerNotifierInitiator::DoCancel()
    {
    FUNC_LOG;
    // Let's cancel the notification if showing of it is still
    // pending.
    AfterDialog();
    }

void CFSNotificationHandlerNotifierInitiator::RunL()
    {
    FUNC_LOG;
    // Notification showing is complete. We don't check the outcome
    // because we have no use for it anymore. There is no way
    // to communicate a possible error to the object that has
    // requested of the notification originally.
    AfterDialog();
    if( iStatus == KErrNone )
        {
        iLogin = ETrue;
        }
    else
        {
        iLogin = EFalse;
        }

    // Delete this. No member variable usage after this!
    iOwner.NotifierInitiatorDoneL( this );
    }

