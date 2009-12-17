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
* Description:  ESMR Synchronization field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrsyncfield.h"

#include <eiklabel.h>
#include <esmrgui.rsg>
#include <StringLoader.h>
#include <barsread.h>
#include <avkon.hrh>
#include <AknsConstants.h>
#include <AknUtils.h>

#include "cesmrsync.h"
#include "cesmrlistquery.h"
#include <calentry.h>

// Unnamed namespace for local definitions and functions
namespace{ // codescanner::namespace

CCalEntry::TReplicationStatus MapToReplicationStatus(
        TESMRSyncValue aSyncValue )
    {
    CCalEntry::TReplicationStatus ret;
    switch ( aSyncValue )
        {
        case ESyncNone:
            {
            ret = CCalEntry::ERestricted;
            break;
            }
        case ESyncPrivate:
            {
            ret = CCalEntry::EPrivate;
            break;
            }
        case ESyncPublic:
            {
            ret = CCalEntry::EOpen;
            break;
            }
        default:
            {
            ret = CCalEntry::ERestricted;
            }
        }

    return ret;
    }

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSyncField::NewL
// ---------------------------------------------------------------------------
//
CESMRSyncField* CESMRSyncField::NewL( )
    {
    FUNC_LOG;
    CESMRSyncField* self = new (ELeave) CESMRSyncField;
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::~CESMRSyncField
// ---------------------------------------------------------------------------
//
CESMRSyncField::~CESMRSyncField( )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy ( );
    iArray.Close ( );
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::InitializeL()
    {
    FUNC_LOG;
    iSync->SetFont( iLayout->Font( iCoeEnv, iFieldId ) );
    iSync->SetLabelAlignment( CESMRLayoutManager::IsMirrored()
                              ? ELayoutAlignRight : ELayoutAlignLeft );
    AknLayoutUtils::OverrideControlColorL(*iSync,
                                           EColorLabelText,
                                           iLayout->GeneralListAreaTextColor() );
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TInt synchValue = aEntry.Entry().ReplicationStatusL ( );

    // esmrgui.rss
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC ( reader, R_ESMREDITOR_SYNC );

    // Read sync items to array
    iArray.ResetAndDestroy( );
    TInt count = reader.ReadInt16 ( );
    for (TInt i(0); i < count; i++ )
        {
        CESMRSync* sync = new (ELeave) CESMRSync();
        CleanupStack::PushL( sync );
        sync->ConstructFromResourceL ( reader );
        iArray.AppendL ( sync );
        CleanupStack::Pop( sync );
        if ( synchValue == MapToReplicationStatus ( sync->Id ( ) ) )
            {
            iIndex = i;
            iSync->SetTextL ( sync->TextL ( ) );
            }
        }
    // resource reader
    CleanupStack::PopAndDestroy(); // codescanner::cleanup
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( iIndex < iArray.Count() )
        {
        CCalEntry::TReplicationStatus
            repStatus = MapToReplicationStatus ( iArray[iIndex]->Id ( ) );
        aEntry.Entry().SetReplicationStatusL (repStatus );
        }
    else
        {
        User::Leave( KErrOverflow );
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenSyncQuery, R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if(aCommand == EESMRCmdOpenSyncQuery ||
       aCommand == EAknCmdOpen )
        {
        ExecuteSyncQueryL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRPriorityField::ExecuteSyncQuery
// ---------------------------------------------------------------------------
//
void CESMRSyncField::ExecuteSyncQueryL()
    {
    FUNC_LOG;

    TInt ret =
        CESMRListQuery::ExecuteL( CESMRListQuery::EESMRSynchronizationPopup );
    if ( ret != KErrCancel )
        {
        iIndex = ret;
        UpdateLabelL (ret );
        }
    }


// ---------------------------------------------------------------------------
// CESMRSyncField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRSyncField::OfferKeyEventL( const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        switch ( aEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                {
                if ( iIndex > 0 )
                    {
                    UpdateLabelL ( --iIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;

            case EStdKeyRightArrow:
                {
                if ( iIndex < (iArray.Count() - 1) )
                    {
                    UpdateLabelL ( ++iIndex );
                    response = EKeyWasConsumed;
                    }
                }
                break;

            default:
                break;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::CESMRSyncField
// ---------------------------------------------------------------------------
//
CESMRSyncField::CESMRSyncField( ) :
    iIndex(0)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::ConstructL( )
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldSync );

    iSync = new (ELeave) CEikLabel;
    iSync->SetTextL( KNullDesC );

    TGulAlignment align;
    align.SetHAlignment( EHLeft );
    align.SetVAlignment( EVCenter );
    iSync->SetAlignment( align );

    CESMRIconField::ConstructL(
            KAknsIIDQgnFscalIndiSynchronisation,
            iSync );
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::UpdateLabelL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::UpdateLabelL( TInt aIndex )
    {
    FUNC_LOG;
    CESMRSync* sync = iArray[ aIndex ];
    iSync->SetTextL( sync->TextL() );
    iSync->DrawDeferred();
    }

// EOF

