/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cesmrsyncfield.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "cesmrtextitem.h"
#include "cesmrlistquery.h"
#include "nmrlayoutmanager.h"

#include <esmrgui.rsg>
#include <StringLoader.h>
#include <barsread.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <calentry.h>
// DEBUG
#include "emailtrace.h"

// Unnamed namespace for local definitions and functions
namespace // codescanner::namespace
    { 
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

    // Field's component count, icon and label
    const TInt KComponentCount( 2 );
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
    
    delete iIcon;
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
        CESMRTextItem* sync = new (ELeave) CESMRTextItem();
        CleanupStack::PushL( sync );
        sync->ConstructFromResourceL ( reader );
        iArray.AppendL ( sync );
        CleanupStack::Pop( sync );
        if ( synchValue == MapToReplicationStatus ( static_cast<TESMRSyncValue>( sync->Id ( ) ) ) )
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
            repStatus = MapToReplicationStatus ( static_cast<TESMRSyncValue>( iArray[iIndex]->Id ( ) ) );
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
TBool CESMRSyncField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if(aCommand == EESMRCmdOpenSyncQuery ||
       aCommand == EAknCmdOpen )
        {
    	HandleTactileFeedbackL();
    	
        ExecuteSyncQueryL();
        isUsed = ETrue;
        }
    return isUsed;
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
    SetFieldId( EESMRFieldSync );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::ConstructL( )
    {
    FUNC_LOG;
    iSync = CMRLabel::NewL();
    CESMRField::ConstructL( iSync );
    iSync->SetTextL( KNullDesC() );

    TGulAlignment align;
    align.SetHAlignment( EHLeft );
    align.SetVAlignment( EVCenter );
    iSync->SetAlignment( align );

    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapSynchronization );
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::UpdateLabelL
// ---------------------------------------------------------------------------
//
void CESMRSyncField::UpdateLabelL( TInt aIndex )
    {
    FUNC_LOG;
    CESMRTextItem* sync = iArray[ aIndex ];
    iSync->SetTextL( sync->TextL() );
    iSync->DrawDeferred();
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRSyncField::CountComponentControls() const
    {
    return KComponentCount;
    }
    
// ---------------------------------------------------------------------------
// CESMRSyncField::ComponentControl
// ---------------------------------------------------------------------------
//    
CCoeControl* CESMRSyncField::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = NULL;
    switch( aIndex )
        {
        case 0:
            {
            control = iSync;            
            break;
            }
        case 1:
            {
            control = iIcon;
            break;
            }
        default:
            ASSERT( EFalse );
        }
    
    return control;
    }

// ---------------------------------------------------------------------------
// CESMRSyncField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRSyncField::SizeChanged()
    {
    TRect rect( Rect() );
    TAknLayoutRect iconLayout = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorIcon );
    TRect iconRect( iconLayout.Rect() );
    iIcon->SetRect( iconRect );
    
    TAknLayoutRect bgLayoutRect = 
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknTextComponentLayout editorLayout =
        NMRLayoutManager::GetTextComponentLayout( 
                NMRLayoutManager::EMRTextLayoutTextEditor );    
    
    AknLayoutUtils::LayoutLabel( iSync, rect, editorLayout );         
    }

// EOF

