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
* Description:  Recurrence field for viewers
 *
*/

#include "cesmrviewerrecurrencefield.h"
#include "nmrlayoutmanager.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "mesmrlistobserver.h"

#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <AknsConstants.h>
#include <esmrgui.rsg>

// DEBUG
#include "emailtrace.h"

// ======== LOCAL FUNCTIONS ========
namespace // codescanner::namespace
    {
    const TInt KComponentCount( 2 );
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::NewL()
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceField* CESMRViewerRecurrenceField::NewL()
    {
    FUNC_LOG;
    CESMRViewerRecurrenceField* self = new (ELeave) CESMRViewerRecurrenceField();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::~CESMRViewerRecurrenceField()
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceField::~CESMRViewerRecurrenceField()
    {
    FUNC_LOG;
    delete iIcon;
    delete iLabel;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::CESMRViewerRecurrenceField()
// -----------------------------------------------------------------------------
//
CESMRViewerRecurrenceField::CESMRViewerRecurrenceField()
    {
    FUNC_LOG;
    SetFieldId( EESMRFieldRecurrence );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::ConstructL()
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceField::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iIcon = CMRImage::NewL( KAknsIIDQgnFscalIndiRecurrence );
    iIcon->SetParent( this );
    
    SetFocusType( EESMRHighlightFocus );    
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::InitializeL()
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceField::InitializeL()
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

    AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                             KRgbBlack );
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::InternalizeL()
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TESMRRecurrenceValue recValue;
    TTime recTime;
    // Get recurrence
    aEntry.GetRecurrenceL( recValue, recTime );

    HBufC* recHolder = NULL;
    switch( recValue )
        {
        case ERecurrenceDaily:
            {
            recHolder = StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRING_DAILY,
                                              iEikonEnv );
            break;
            }
        case ERecurrenceWeekly:
            {
            recHolder = StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRING_WEEKLY,
                                              iEikonEnv );
            break;
            }
        case ERecurrenceEverySecondWeek:
            {
            recHolder =
                StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRING_SEC_WEEK,
                                      iEikonEnv );
            break;
            }
        case ERecurrenceMonthly:
            {
            recHolder = StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRING_MONTHLY,
                                              iEikonEnv );
            break;
            }
        case ERecurrenceYearly:
            {
            recHolder = StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRING_YEARLY,
                                              iEikonEnv );
            break;
            }
        case ERecurrenceUnknown:
            {
            recHolder = StringLoader::LoadLC( R_QTN_MEET_REQ_RECURRENCE_EVENT,
                                              iEikonEnv );
            break;
            }
        case ERecurrenceNot:
        default:
            {
            // Hide the field if there is no recurrence
            iObserver->RemoveControl( iFieldId );
            break;
            }
        }
    if ( recHolder )
        {
        iLabel->SetTextL( *recHolder );
        CleanupStack::PopAndDestroy( recHolder );
        }
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::SizeChanged()
// -----------------------------------------------------------------------------
//
void CESMRViewerRecurrenceField::SizeChanged()
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

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::CountComponentControls()
// -----------------------------------------------------------------------------
//
TInt CESMRViewerRecurrenceField::CountComponentControls() const
    {
    return KComponentCount;
    }

// -----------------------------------------------------------------------------
// CESMRViewerRecurrenceField::ComponentControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CESMRViewerRecurrenceField::ComponentControl( TInt aIndex ) const
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
// CESMRViewerRecurrenceField::SetOutlineFocusL()
// ---------------------------------------------------------------------------
//
void CESMRViewerRecurrenceField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    
    iLabel->SetFocus( aFocus );    

    if ( !aFocus )
        {
        AknLayoutUtils::OverrideControlColorL ( *iLabel, EColorLabelText,
                                                 KRgbBlack );
        }
    }
// EOF

