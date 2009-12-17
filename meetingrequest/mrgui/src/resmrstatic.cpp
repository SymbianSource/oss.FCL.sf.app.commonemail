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
* Description:  Static Tls data storage
*
*/


#include "emailtrace.h"
#include "resmrstatic.h"

#include "cesmrcontactmenuhandler.h"
#include "cesmrcontactmanagerhandler.h"
#include "CFSMailClient.h"
#include "CFSMailBox.h"
// <cmail> Removed profiling </cmail>

/**
 * Storage stuct for RESMRStatic.
 */
struct TESMRStaticData
    {
    TInt iInstanceCount;
    TInt iContactManagerHandlerCount;
    CESMRContactManagerHandler* iContactManagerHandler;
    TInt iContactMenuHandlerCount;
    CESMRContactMenuHandler* iContactMenuHandler;
    CFSMailClient* iFSMailClient;
    CFSMailBox* iFSMailBox;

    TInt iPbkxContactListingServiceCount;
    TInt iCurrentFieldIndex;
    TInt iFSMailBoxCount;
    };

// ---------------------------------------------------------------------------
// Cleanup operation for RPointerArray.
// @param aArray Pointer to RPointerArray.
// ---------------------------------------------------------------------------
//
void MailboxPointerArrayCleanup( TAny* aArray )
    {
    // just to avoid warnings when MR not supported
    RPointerArray<CFSMailBox>* entryArray =
        static_cast<RPointerArray<CFSMailBox>*>( aArray );

    entryArray->ResetAndDestroy();
    entryArray->Close();
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// RESMRStatic::RESMRStatic
// ---------------------------------------------------------------------------
//
EXPORT_C RESMRStatic::RESMRStatic( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// RESMRStatic::~RESMRStatic
// ---------------------------------------------------------------------------
//
EXPORT_C RESMRStatic::~RESMRStatic( )
    {
    FUNC_LOG;
    if ( iStaticData )
        {
        delete iStaticData->iContactMenuHandler;
        iStaticData->iContactMenuHandler = NULL;

        delete iStaticData->iContactManagerHandler;
        iStaticData->iContactManagerHandler = NULL;
        }
    }

// ---------------------------------------------------------------------------
// RESMRStatic::ConnectL
// ---------------------------------------------------------------------------
//
EXPORT_C void RESMRStatic::ConnectL( )
    {
    FUNC_LOG;
    if (iStaticData )
        {
        // already connected
        return;
        }

    // Retrieve Tls pointer
    iStaticData = (TESMRStaticData*) Dll::Tls();

    // If Tls pointer was not set, create new static stuct
    // with NULL values
    if ( !iStaticData )
        {
        iStaticData = new (ELeave) TESMRStaticData();
        memset ( iStaticData, 0, sizeof( TESMRStaticData) );
        Dll::SetTls ( iStaticData );
        }
    ++iStaticData->iInstanceCount;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::Close
// ---------------------------------------------------------------------------
//
EXPORT_C void RESMRStatic::Close( )
    {
    FUNC_LOG;
    if (iStaticData )
        {
        if (iUsedTypes & EContactMenuHandler )
            {
            --iStaticData->iContactMenuHandlerCount;
            if (iStaticData->iContactMenuHandlerCount == 0 )
                {
                if ( iStaticData->iContactManagerHandlerCount == 0 )
                    {
                    // Delete both Handlers when both are ready to delete
                    // and Contact Manager Handler should be deleted last
                    // because the Menu Handler has dependency to it.
                    delete iStaticData->iContactMenuHandler;
                    iStaticData->iContactMenuHandler = NULL;

                    delete iStaticData->iContactManagerHandler;
                    iStaticData->iContactManagerHandler = NULL;
                    }
                }
            }

        // If ContactManagerHandlerL was used
        if (iUsedTypes & EContactManagerHandler )
            {
            // Decrease counter
            --iStaticData->iContactManagerHandlerCount;

            // If this was last instance using pointer
            if ( iStaticData->iContactManagerHandlerCount == 0 )
                {
                if ( iStaticData->iContactMenuHandlerCount == 0 )
                    {
                    delete iStaticData->iContactMenuHandler;
                    iStaticData->iContactMenuHandler = NULL;
                    // Delete instance
                    delete iStaticData->iContactManagerHandler;
                    iStaticData->iContactManagerHandler = NULL;
                    }
                }
            }
        
        // If FSMailClientL or FSMailBox was used
        if (iUsedTypes & EMailBoxHandler )
            {
            // Decrease counter
            --iStaticData->iFSMailBoxCount;

            // If this was last instance using pointer
            if (iStaticData->iFSMailBoxCount == 0 )
                {
                // Owned by CFSMailClient
                delete iStaticData->iFSMailBox;
                iStaticData->iFSMailBox = NULL;

                // Delete instance
                if ( iStaticData->iFSMailClient )
                    {
                    iStaticData->iFSMailClient->Close();
                    iStaticData->iFSMailClient = NULL;
                    }
                }
            }

        // Decrease instance counter
        --iStaticData->iInstanceCount;

        // If this was last instance using pointer
        if (iStaticData->iInstanceCount == 0 )
            {
            delete iStaticData;
            Dll::FreeTls ( );
            }
        iStaticData = NULL;
        }
    }

// ---------------------------------------------------------------------------
// RESMRStatic::ContactManagerHandlerL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRContactManagerHandler& RESMRStatic::ContactManagerHandlerL( )
    {
    FUNC_LOG;
    if ( !iStaticData->iContactManagerHandler )
        {
        iStaticData->iContactManagerHandler = 
			CESMRContactManagerHandler::NewL();
        }

    if (!(iUsedTypes & EContactManagerHandler) )
        {
        ++iStaticData->iContactManagerHandlerCount;
        iUsedTypes |= EContactManagerHandler;
        }
    return *iStaticData->iContactManagerHandler;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::ContactMenuHandlerL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRContactMenuHandler& RESMRStatic::ContactMenuHandlerL( )
    {
    FUNC_LOG;
    if ( !iStaticData->iContactMenuHandler )
        {
        iStaticData->iContactMenuHandler =
            CESMRContactMenuHandler::NewL( ContactManagerHandlerL() );
        }

    if (!(iUsedTypes & EContactMenuHandler) )
        {
        ++iStaticData->iContactMenuHandlerCount;
        iUsedTypes |= EContactMenuHandler;
        }

    return *iStaticData->iContactMenuHandler;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::CurrentFieldIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RESMRStatic::CurrentFieldIndex()
    {
    FUNC_LOG;
    return iStaticData->iCurrentFieldIndex;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::SetCurrentFieldIndex
// ---------------------------------------------------------------------------
//
EXPORT_C void RESMRStatic::SetCurrentFieldIndex(TInt aFieldIndex)
    {
    FUNC_LOG;
    iStaticData->iCurrentFieldIndex = aFieldIndex;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::MailBoxL
// ---------------------------------------------------------------------------
//
EXPORT_C CFSMailBox& RESMRStatic::MailBoxL( const TDesC& aAddress )
    {
    FUNC_LOG;
    if ( !iStaticData->iFSMailBox )
        {
        CFSMailClient& fsMailClient = FSMailClientL();

        // Loop throug all mailboxes in this plug-in
        RPointerArray<CFSMailBox> mailboxes; // codescanner::resourcenotoncleanupstack
        CleanupStack::PushL(
                TCleanupItem(
                        MailboxPointerArrayCleanup,
                        &mailboxes    ) );

        // With null uid we get all mailboxes
        TFSMailMsgId msgId;
        fsMailClient.ListMailBoxes(
                msgId, mailboxes );

        TInt mailboxCount( mailboxes.Count() );
        for ( TInt j(0);
              j < mailboxCount && !iStaticData->iFSMailBox;
              ++j )
            {
            TPtrC mailboxOwnerAddName(
                    mailboxes[j]->OwnMailAddress().GetEmailAddress() );
            if ( mailboxOwnerAddName.Compare( aAddress ) == 0 )
                {
                // Correct mailbox is found
                iStaticData->iFSMailBox = mailboxes[j];
                mailboxes.Remove(j);
                }
            }
        CleanupStack::PopAndDestroy(); // mailboxes

        if ( !iStaticData->iFSMailBox )
            {
            User::Leave( KErrAbort );
            }
        }

    iUsedTypes |= EMailBoxHandler;
    ++iStaticData->iFSMailBoxCount;
    return *iStaticData->iFSMailBox;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::FSMailClientL
// ---------------------------------------------------------------------------
//
CFSMailClient& RESMRStatic::FSMailClientL( )
    {
    FUNC_LOG;
    if (!iStaticData->iFSMailClient)
        {
        iStaticData->iFSMailClient = CFSMailClient::NewL ( );
        }

    return *iStaticData->iFSMailClient;
    }

// EOF

