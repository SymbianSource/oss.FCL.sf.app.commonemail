/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgDeleteRemote.
*
*/



// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::CIpsPlgDeleteRemote
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteRemote::CIpsPlgDeleteRemote(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus )
    :
    CMsvOperation( 
        aMsvSession, 
        CActive::EPriorityStandard, 
        aObserverRequestStatus),
        iBlank( KNullDesC8 )
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    }


// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::ConstructL(
    CMsvEntrySelection& aDeletedEntries )
    {
    FUNC_LOG;
    iEntrySelection = aDeletedEntries.CopyL();
    iEntryCount = iEntrySelection->Count();
    if ( iEntryCount == 0 )
        {
        User::Leave( KErrNotSupported );
        }

    TMsvId serviceId;
    TMsvEntry entry;
    User::LeaveIfError(
        iMsvSession.GetEntry(
            (*iEntrySelection)[0], serviceId, entry ) );
    iEntry = CMsvEntry::NewL(
        iMsvSession, entry.Parent(), TMsvSelectionOrdering() );
    iMtm = iEntry->Entry().iMtm;

    iStatus=KRequestPending;

	// <qmail>    
    // It is known that there is at least one entry -> no return value check
    SetNextLocallyDeletedFlagL();

    SetActive();
	// </qmail>    
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteRemote* CIpsPlgDeleteRemote::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CMsvEntrySelection& aDeletedEntries )
    {
    FUNC_LOG;
    CIpsPlgDeleteRemote* self=new (ELeave) CIpsPlgDeleteRemote(
        aMsvSession, aObserverRequestStatus );
    CleanupStack::PushL(self);
    self->ConstructL( aDeletedEntries );
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::~CIpsPlgDeleteRemote
// ----------------------------------------------------------------------------
//
CIpsPlgDeleteRemote::~CIpsPlgDeleteRemote()
    {
    FUNC_LOG;
    Cancel();

    delete iOperation;
    delete iEntry;
    delete iEntrySelection;
	// <qmail>    
    delete iSetFlagEntry;
	// </qmail>    
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::DoCancel
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::DoCancel()
    {
    FUNC_LOG;
    if (iOperation)
        {
        iOperation->Cancel();
        }
    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        User::RequestComplete(status, iStatus.Int());
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::RunL()
    {
    FUNC_LOG;
    // Make first a check, that nothing has failed
    if ( iStatus.Int() != KErrNone )
        {
        // something failed, just complete
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        return;
        }

	// <qmail>    
    if ( iState == EDeletingMessagesStateSetFlags )
        {
        // cleanup is handled by SetNextLocallyDeletedFlagL
        TBool ret = EFalse;
        
        TRAPD( err, ret = SetNextLocallyDeletedFlagL() );
        
        if ( err != KErrNone )
            {
            TRequestStatus* status = &iObserverRequestStatus;
            User::RequestComplete(status, iStatus.Int());
            }
        else if ( ret )
            {
            SetActive();
            }
        else
            { // setting the flags is ready, start local deletion
            StartNextDeleteLocally();
            }
        }
    else if ( iState == EDeletingMessagesStateLocally )
        {
        // local delete completed, start deleting from server
        StartDeleteFromServer();
        }
    else
        {
        // nothing left to process, so complete the observer
        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete(status, iStatus.Int());
        }
    // </qmail>    
	}

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::ProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgDeleteRemote::ProgressL()
    {
    FUNC_LOG;
    // Make sure that operation is active
    if ( IsActive() )
        {
        // Get progress
        if ( iOperation )
            {
            return iOperation->ProgressL();
            }
        }

    return iBlank;
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::StartNextDeleteLocally
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::StartNextDeleteLocally()
    {
    FUNC_LOG;
    iState = EDeletingMessagesStateLocally;
    iStatus = KRequestPending;
    
    TRAPD( err, MakeNextDeleteLocallyL() );
    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, err);
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::MakeNextDeleteLocallyL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::MakeNextDeleteLocallyL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    iOperation = CIpsPlgDeleteLocal::NewL(
        *iEntrySelection, iMsvSession, iStatus );
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::StartDeleteFromServer
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::StartDeleteFromServer()
    {
    FUNC_LOG;
    iState = EDeletingMessagesStateFromServer;
    iStatus = KRequestPending;
    TRAPD( err, MakeDeleteFromServerL() );

    SetActive();
    // if error then complete this pass with the error code
    if ( err )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::MakeDeleteFromServerL
// ----------------------------------------------------------------------------
//
void CIpsPlgDeleteRemote::MakeDeleteFromServerL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    iOperation = iEntry->DeleteL( *iEntrySelection, iStatus );
    }


// <qmail>    
// ----------------------------------------------------------------------------
// CIpsPlgDeleteRemote::SetNextLocallyDeletedFlagL
// ----------------------------------------------------------------------------
//
TBool CIpsPlgDeleteRemote::SetNextLocallyDeletedFlagL()
    {
    FUNC_LOG;
    
    delete iOperation;
    iOperation = NULL;
    
    delete iSetFlagEntry;
    iSetFlagEntry = NULL;

    TBool ret = EFalse;
    
    if ( iSetFlagIndex < iEntryCount )
        {
        TMsvId entryId = ( *iEntrySelection )[ iSetFlagIndex++ ];
        
        iSetFlagEntry = CMsvEntry::NewL( 
            iMsvSession, entryId, TMsvSelectionOrdering() );
        
        TMsvEntry tEntry = iSetFlagEntry->Entry();

        // Sets bit 32 of iMtmData1, used when msg deleted in Offline
        // and status hasn't updated to server (client entry still exists)
        tEntry.SetLocallyDeleted( ETrue );

        iOperation = iSetFlagEntry->ChangeL( tEntry, iStatus );
        
        ret = ETrue;
        }
    
    return ret;
    }
// </qmail>    

//  End of File


