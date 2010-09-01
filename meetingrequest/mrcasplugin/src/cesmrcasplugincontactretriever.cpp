/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 *  Description : Implementation of the class CESMRCasPluginImpl.
 *  Version     : %version: e002sa32#6.1.1 % << Don't touch! Updated by Synergy at check-out.
 *
 */


#include "emailtrace.h"
#include "cesmrcasplugincontactretriever.h"
#include "esmrinternaluid.h"
#include "esmrhelper.h"
#include "mesmrcasplugincontactretrieverobserver.h"

#include <esmrcasplugindata.rsg>
#include <esmrcasplugin.mbg>
#include <caluser.h>

#include <MVPbkStoreContact.h>
#include <data_caging_path_literals.hrh>
#include <badesca.h>

#include <utf.h>
#include <bautils.h>
//<cmail>
#include "mfscactionutils.h"
#include "mfsccontactset.h"
#include "mfsccontactsetobserver.h"
#include "tfsccontactactionpluginparams.h"
//</cmail>
#include <MVPbkStoreContactField.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <VPbkFieldType.hrh>

// Unnamed namespace for local definitions
namespace { // codescanner::namespace

const TInt KEmailTextLength( 255 );

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::CESMRCasPluginContactRetriever
// ---------------------------------------------------------------------------
//
CESMRCasPluginContactRetriever::CESMRCasPluginContactRetriever(
        const TFscContactActionPluginParams& aParams )
:   iParams( aParams )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginImpl::~CESMRCasPluginImpl
// ---------------------------------------------------------------------------
//
CESMRCasPluginContactRetriever::~CESMRCasPluginContactRetriever( )
    {
    FUNC_LOG;

    iAttendees.ResetAndDestroy();

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NewL
// ---------------------------------------------------------------------------
//
CESMRCasPluginContactRetriever* CESMRCasPluginContactRetriever::NewL(
        const TFscContactActionPluginParams& aParams )
    {
    FUNC_LOG;

    CESMRCasPluginContactRetriever* self =
            new (ELeave) CESMRCasPluginContactRetriever( aParams );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );


    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::ConstructL()
    {
    FUNC_LOG;
    
    // Nothing to do.
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::CanExecuteL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::CanExecuteL(
        MFscContactSet& aContactSet,
        MESMRCasPluginContactRetrieverObserver& aObserver )
    {
    FUNC_LOG;

    iOperation = EOperationCanExecute;
    iContactSet = &aContactSet;
    iObserver = &aObserver;

    StartIterationL();

    }


// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::GetAttendeesL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::GetAttendeesL(
        MFscContactSet& aContactSet,
        MESMRCasPluginContactRetrieverObserver& aObserver )
    {
    FUNC_LOG;

    iOperation = EOperationGetAttendees;
    iContactSet = &aContactSet;
    iObserver = &aObserver;

    StartIterationL();

    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::Cancel
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::Cancel()
    {
    FUNC_LOG;

    if ( iContactSet )
        {
        // We need to cancel both operations.
        TRAP_IGNORE( iContactSet->CancelNextContactL() );
        TRAP_IGNORE( iContactSet->CancelNextGroupL() );
        }
    
    iContactSet = NULL;
    iAttendees.ResetAndDestroy();
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::StartIterationL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::StartIterationL()
    {
    FUNC_LOG;
    
    // Initialize member variables for iteration
    iContactSet->SetToFirstContact();
    iContactSet->SetToFirstGroup();
    iContactSet->CancelNextContactL();
    iContactSet->CancelNextGroupL();

    iContactCount = 0;
    iContactCountWithEmail = 0;
    iAttendees.ResetAndDestroy();
    
    if ( iContactSet->HasNextContact() )
        {
        iLastEvent = EActionEventNextContact;
        iContactSet->NextContactL( this );
        }
    else if ( iContactSet->HasNextGroup() )
        {
        iLastEvent = EActionEventNextGroup;
        iContactSet->NextGroupL( this );
        }
    else
        {
        NotifyFinished();
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NextContactComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NextContactComplete(
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    
    TRAPD( err, HandleNextContactCompleteL( *aContact ) );
    if ( KErrNone != err )
        {
        NotifyError( err );
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NextContactFailed
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NextContactFailed(
        TInt aError )
    {
    FUNC_LOG;
    
    NotifyError( aError );
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NextGroupComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NextGroupComplete(
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    
    TRAPD( err, HandleNextGroupCompleteL( aContact ) );
    if ( KErrNone != err )
        {
        NotifyError( err );
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NextGroupFailed
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NextGroupFailed(
        TInt aError )
    {
    FUNC_LOG;
    
    NotifyError( aError );
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::GetGroupContactComplete
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::GetGroupContactComplete(
        MVPbkStoreContact* aContact )
    {
    FUNC_LOG;
    
    TRAPD( err, HandleNextContactCompleteL( *aContact ) );
    if ( KErrNone != err )
        {
        NotifyError( err );
        }
    delete aContact;
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::GetGroupContactFailed
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::GetGroupContactFailed(
        TInt aError )
    {
    FUNC_LOG;
    
    NotifyError( aError );
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::HandleNextContactCompleteL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::HandleNextContactCompleteL(
        MVPbkStoreContact& aContact )
    {
    FUNC_LOG;
    
    TBool stopIteration = EFalse;
    TInt error = KErrNone;
    iContactCount++;
    
    if ( EOperationCanExecute == iOperation )
        {
        if ( iParams.iUtils->IsContactNumberAvailableL( aContact, ETypeEmailAddress ) )
            {
            // Stop iteration as soon as first contact with email is found.
            iContactCountWithEmail++;
            stopIteration = ETrue;
            }
        }
    else if ( EOperationGetAttendees == iOperation )
        {
        TBuf<KEmailTextLength> eMailAddress;
        TInt retVal = iParams.iUtils->GetEmailAddressL( aContact, eMailAddress );
        
        // User selected an email address for contact or
        // contact had only 1 email address.
        if ( retVal == KErrNone )
            {
            // Add attendee with selected email to attendees list.
            CCalUser* attendee = CCalUser::NewL( eMailAddress );
            CleanupStack::PushL( attendee );
            
            HBufC* contactName = iParams.iUtils->GetContactNameL( aContact );
            CleanupStack::PushL( contactName );
            
            attendee->SetCommonNameL( *contactName );
            CleanupStack::PopAndDestroy( contactName );
            contactName = NULL;
            
            // Ownership transferred
            iAttendees.AppendL( attendee );
            CleanupStack::Pop( attendee );
            }
        // User selected cancel (KErrCancel) or some other error was returned.
        // If retVal = KErrNotFound contact had no email address,
        // in that case continue iteration.
        else if ( KErrNotFound != retVal )
            {
            // Stop iteration and notify error to observer.
            error = retVal;
            stopIteration = ETrue;
            }        
        }
    
    // If iteration should be stopped
    if ( stopIteration )
        {
        if ( error != KErrNone )
            {
            NotifyError( error );
            }
        else
            {
            NotifyFinished();
            }
        }
    // First iterate contacts
    else if ( iLastEvent == EActionEventNextContact &&
            iContactSet->HasNextContact() )
        {
        iContactSet->NextContactL( this );
        }
    // If there are more contacts in current group
    else if ( iLastEvent == EActionEventGetGroupContact &&
            iCurrentGroupContactIndex < iCurrentGroupContactCount )
        {
        iContactSet->GetGroupContactL(
                *iCurrentGroup,
                iCurrentGroupContactIndex,
                this);
        ++iCurrentGroupContactIndex;
        }
    // If there are more groups
    else if ( iContactSet->HasNextGroup() )
        {
        iLastEvent = EActionEventNextGroup;
        iContactSet->NextGroupL( this );
        }
    // Iteration complete
    else
        {
        NotifyFinished();
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::HandleNextGroupCompleteL
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::HandleNextGroupCompleteL(
        MVPbkStoreContact* aGroup )
    {
    FUNC_LOG;
    
    iCurrentGroupContactCount = iContactSet->GroupContactCountL( *aGroup );
    iCurrentGroup = NULL;

    // If group contains contacts
    if ( iCurrentGroupContactCount )
        {
        iCurrentGroup = aGroup;
        iCurrentGroupContactIndex = 0;
        iLastEvent = EActionEventGetGroupContact;
        iContactSet->GetGroupContactL(
                *iCurrentGroup,
                iCurrentGroupContactIndex,
                this );

        ++iCurrentGroupContactIndex;
        }
    // If there are more groups
    else if ( iContactSet->HasNextGroup() )
        {
        iLastEvent = EActionEventNextGroup;
        iContactSet->NextGroupL( this );
        }
    // Group iteration complete
    else
        {
        NotifyFinished();
        }
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NotifyFinished
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NotifyFinished()
    {
    FUNC_LOG;
    
    iContactSet = NULL;
    
    if ( iObserver )
        {
        if ( EOperationCanExecute == iOperation )
            {
            iObserver->MRCanExecuteComplete( iContactCountWithEmail > 0 );
            }
        else if ( EOperationGetAttendees == iOperation )
            {
            iObserver->MRGetAttendeesComplete( iContactCount, iAttendees );
            }
        }
    
    iAttendees.ResetAndDestroy();
    
    }

// ---------------------------------------------------------------------------
// CESMRCasPluginContactRetriever::NotifyError
// ---------------------------------------------------------------------------
//
void CESMRCasPluginContactRetriever::NotifyError( TInt aError )
    {
    FUNC_LOG;
    
    iContactSet = NULL;
    
    if ( iObserver )
        {
        if ( EOperationCanExecute == iOperation )
            {
            iObserver->MRCanExecuteError( aError );
            }
        else if ( EOperationGetAttendees == iOperation )
            {
            iObserver->MRGetAttendeesError( aError );
            }
        }
    
    iAttendees.ResetAndDestroy();
    
    }

// EOF

