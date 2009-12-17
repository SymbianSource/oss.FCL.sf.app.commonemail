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
* Description:  Synchronisation field for viewers
*
*/

#include "cesmrviewersyncfield.h"
#include "nmrlayoutmanager.h"
#include "nmrbitmapmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "mesmrlistobserver.h"

#include <calentry.h>
#include <StringLoader.h>
#include <AknLayout2ScalableDef.h>

#include <esmrgui.rsg>
// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::NewL()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField* CESMRViewerSyncField::NewL()
    {
    FUNC_LOG;
    CESMRViewerSyncField* self = new (ELeave) CESMRViewerSyncField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::~CESMRViewerSyncField()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField::~CESMRViewerSyncField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLabel;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::CESMRViewerSyncField()
// ---------------------------------------------------------------------------
//
CESMRViewerSyncField::CESMRViewerSyncField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldSync );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapSynchronization );
    iIcon->SetParent( this );
    
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CCalEntry& entry = aEntry.Entry();
    CCalEntry::TReplicationStatus rStatus = entry.ReplicationStatusL();

    HBufC* statusHolder = NULL;
    switch( rStatus )
        {
        case CCalEntry::ERestricted:
            {
            statusHolder = StringLoader::LoadLC( R_QTN_CALENDAR_TASK_SYNC_NONE,
                                                 iEikonEnv );
            break;
            }
        case CCalEntry::EPrivate:
            {
            statusHolder = StringLoader::LoadLC( R_QTN_CALENDAR_TASK_SYNC_PRIVATE,
                                                 iEikonEnv );
            break;
            }
        case CCalEntry::EOpen:
            {
            statusHolder = StringLoader::LoadLC( R_QTN_CALENDAR_TASK_SYNC_PUBLIC,
                                                 iEikonEnv );
            break;
            }
        default: // There is no replication status set - hide field- this should never happen
            {
            iObserver->RemoveControl( iFieldId );
            }
        }
        
    if ( statusHolder )
        {
        iLabel->SetTextL( *statusHolder );
        CleanupStack::PopAndDestroy( statusHolder );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::InitializeL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::InitializeL()
    {
    FUNC_LOG;
    // Setting Font for the rich text viewer
    TAknLayoutText text = NMRLayoutManager::GetLayoutText( 
            Rect(), 
            NMRLayoutManager::EMRTextLayoutTextEditor );
    
    iLabel->SetFont( text.Font() );
        
    // This is called so theme changes will apply when changing theme "on the fly"
    if ( IsFocused() )
        {
        iLabel->FocusChanged( EDrawNow );
        }

    AknLayoutUtils::OverrideControlColorL( *iLabel, EColorLabelText,
                                       KRgbBlack );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::SizeChanged()
    {
    TRect rect = Rect();
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
        NMRLayoutManager::GetWindowComponentLayout( 
                NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iIcon, rect, iconLayout );
    
    TAknLayoutRect bgLayoutRect =
        NMRLayoutManager::GetLayoutRect( 
                rect, NMRLayoutManager::EMRLayoutTextEditorBg );
    TRect bgRect( bgLayoutRect.Rect() );
    // Move focus rect so that it's relative to field's position.
    bgRect.Move( -Position() );
    SetFocusRect( bgRect );
    
    TAknLayoutText labelLayout = 
        NMRLayoutManager::GetLayoutText( 
                rect, NMRLayoutManager::EMRTextLayoutTextEditor );
    iLabel->SetRect( labelLayout.TextRect() );
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRViewerSyncField::CountComponentControls() const
    {
    TInt count( 0 );
    if ( iIcon )
        {
        ++count;
        }

    if ( iLabel )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRViewerSyncField::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            return iIcon;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerSyncField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerSyncField::SetOutlineFocusL( TBool aFocus )
    {
    CESMRField::SetOutlineFocusL ( aFocus );
    
    iLabel->SetFocus( aFocus );    

    if ( !aFocus )
        {
        AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                                 KRgbBlack );
        }
    }
// EOF

