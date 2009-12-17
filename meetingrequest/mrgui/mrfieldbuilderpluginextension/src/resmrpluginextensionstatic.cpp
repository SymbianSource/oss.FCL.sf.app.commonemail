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
#include "resmrpluginextensionstatic.h"
#include "cmrmailboxutils.h"

/**
 * Storage class for RESMRStatic.
 */
class TESMRPluginExtensionData
    {
public:
    TInt iInstanceCount;
    TInt iFSMailBoxCount;
    CFSMailClient* iFSMailClient;
    CFSMailBox* iDefaultFSMailBox;
    };

// Unnamed namespace for local definitions, structs and functions
namespace { // codescanner::namespace

const TInt KMailBox = 0x01;

// Definition for equal string
const TInt KEqualString(0);

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

}//namespace

// ---------------------------------------------------------------------------
// RESMRPluginExtensionStatic::RESMRPluginExtensionStatic
// ---------------------------------------------------------------------------
//
RESMRPluginExtensionStatic::RESMRPluginExtensionStatic( )
    {
    FUNC_LOG;
    iStaticData = NULL;
    }

// ---------------------------------------------------------------------------
// RESMRStatic::~RESMRStatic
// ---------------------------------------------------------------------------
//
RESMRPluginExtensionStatic::~RESMRPluginExtensionStatic( )
    {
    FUNC_LOG;
    Close();
    }

// ---------------------------------------------------------------------------
// RESMRPluginExtensionStatic::ConnectL
// ---------------------------------------------------------------------------
//
void RESMRPluginExtensionStatic::ConnectL( )
    {
    FUNC_LOG;
    if ( iStaticData )
        {
        return;
        }

    // Retrieve Tls pointer
    iStaticData = static_cast<TESMRPluginExtensionData*>( Dll::Tls());

    // If Tls pointer was not set, create new static stuct
    // with NULL values
    if ( !iStaticData )
        {
        iStaticData = new (ELeave) TESMRPluginExtensionData();
        memset ( iStaticData, 0, sizeof( TESMRPluginExtensionData) );
        Dll::SetTls ( iStaticData );
        }
    ++iStaticData->iInstanceCount;
    }

// ---------------------------------------------------------------------------
// RESMRPluginExtensionStatic::Close
// ---------------------------------------------------------------------------
//
void RESMRPluginExtensionStatic::Close( )
    {
    FUNC_LOG;
    if ( iStaticData )
        {

        // If FSMailClientL or DefaultFSMailBox was used
        if ( iUsedTypes & KMailBox )
            {
            // Decrease counter
            --iStaticData->iFSMailBoxCount;

            // If this was last instance using pointer
            if ( iStaticData->iFSMailBoxCount == 0 )
                {
                // Owned by CFSMailClient
                delete iStaticData->iDefaultFSMailBox;
                iStaticData->iDefaultFSMailBox = NULL;

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
            if ( iStaticData->iDefaultFSMailBox )
                {
                delete iStaticData->iDefaultFSMailBox;
                iStaticData->iDefaultFSMailBox = NULL;
                }

            if (iStaticData->iFSMailClient)
                {
                iStaticData->iFSMailClient->Close();
                iStaticData->iFSMailClient = NULL;
                }

            delete iStaticData;
            Dll::FreeTls ( );
            }
        iStaticData = NULL;
        }
    }

// ---------------------------------------------------------------------------
// RESMRPluginExtensionStatic::DefaultFSMailBoxL
// ---------------------------------------------------------------------------
//
CFSMailBox& RESMRPluginExtensionStatic::DefaultFSMailBoxL( TBool aForceReset )
    {
    FUNC_LOG;
    
    if ( aForceReset && iStaticData->iDefaultFSMailBox )
        {
        delete iStaticData->iDefaultFSMailBox;
        iStaticData->iDefaultFSMailBox = NULL;
        }
    
    if ( !iStaticData->iDefaultFSMailBox )
        {
        CFSMailClient& fsMailClient = FSMailClientL();

        CMRMailboxUtils* mbUtils =
                CMRMailboxUtils::NewL();
        CleanupStack::PushL( mbUtils );

        CMRMailboxUtils::TMailboxInfo mbInfo;
        User::LeaveIfError(
                mbUtils->GetDefaultMRMailBoxL( mbInfo ) );

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
              j < mailboxCount && !iStaticData->iDefaultFSMailBox;
              ++j )
            {
            TPtrC mailboxOwnerAddName(
                    mailboxes[j]->OwnMailAddress().GetEmailAddress() );
            if ( KEqualString == mailboxOwnerAddName.Compare(mbInfo.iEmailAddress) )
                {
                // Correct mailbox is found
                iStaticData->iDefaultFSMailBox = mailboxes[j];
                mailboxes.Remove(j);
                }
            }

        CleanupStack::PopAndDestroy(); // mailboxes
        CleanupStack::PopAndDestroy( mbUtils );
        mbUtils = NULL;

        if ( !iStaticData->iDefaultFSMailBox )
            {
            User::Leave( KErrAbort );
            }
        }

    iUsedTypes |= KMailBox;
    ++iStaticData->iFSMailBoxCount;
    return *iStaticData->iDefaultFSMailBox;
    }

// ---------------------------------------------------------------------------
// RESMRPluginExtensionStatic::FSMailClientL
// ---------------------------------------------------------------------------
//
CFSMailClient& RESMRPluginExtensionStatic::FSMailClientL( )
    {
    FUNC_LOG;
    if (!iStaticData->iFSMailClient)
        {
        iStaticData->iFSMailClient = CFSMailClient::NewL ( );
        }

    return *iStaticData->iFSMailClient;
    }

// EOF

