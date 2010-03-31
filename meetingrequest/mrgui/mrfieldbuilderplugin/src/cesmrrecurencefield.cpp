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
* Description:  ESMR Recurrence field implementation
 *
*/

#include "emailtrace.h"
#include "cesmrrecurencefield.h"
#include "cesmrrecurrence.h"
#include "mesmrlistobserver.h"
#include "cesmrlistquery.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
#include "nmrlayoutmanager.h"
#include "cmrimage.h"
#include "cmrlabel.h"

#include <barsread.h>
#include <esmrgui.rsg>
//<cmail>
#include "esmrdef.h"
//</cmail>


// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG

/** Panic code literal */
_LIT( KESMRRecurenceFieldPanicTxt, "ESMRRecurenceField" );

/** Panic code enumeration */
enum TESMRRecurenceFieldPanic
    {
    /** Observer is not set */
    EESMRRecurenceFieldNoObserver = 0
    };

/**
 * Raises system panic.
 * @param aPanic CESMRRecurenceField panic code.
 */
void Panic( TESMRRecurenceFieldPanic aPanic )
    {
    User::Panic( KESMRRecurenceFieldPanicTxt, aPanic );
    }

#endif // DEBUG

 }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::CESMRRecurrenceField
// ---------------------------------------------------------------------------
//
CESMRRecurenceField::CESMRRecurenceField( MESMRFieldValidator* aValidator )
    : iIndex( 0 )
    {
    FUNC_LOG;
    
    iValidator = aValidator;
    
    SetFieldId ( EESMRFieldRecurrence );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::~CESMRRecurrenceField
// ---------------------------------------------------------------------------
//
CESMRRecurenceField::~CESMRRecurenceField( )
    {
    FUNC_LOG;
    iArray.ResetAndDestroy();
    iArray.Close();

    delete iFieldIcon;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::NewL
// ---------------------------------------------------------------------------
//
CESMRRecurenceField* CESMRRecurenceField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRRecurenceField* self = 
        new( ELeave )CESMRRecurenceField( aValidator );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::ConstructL( )
    {
    FUNC_LOG;
    iRecurrence = CMRLabel::NewL();
    iRecurrence->SetParent( this );
    CESMRField::ConstructL( iRecurrence );
    
    iRecurrence->SetTextL( KNullDesC() );
    
    iFieldIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapRecurrence );
    iFieldIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.CanSetRecurrenceL() )
        {
        CCoeEnv* env = CCoeEnv::Static ( );
        TResourceReader reader;
        env->CreateResourceReaderLC( reader, R_ESMREDITOR_RECURRENCE );

        TESMRRecurrenceValue recurrenceVal;
        TTime until;
        aEntry.GetRecurrenceL(recurrenceVal, until );

        iArray.ResetAndDestroy( );
        TInt count = reader.ReadInt16 ( );
        for (TInt i(0); i < count; ++i )
            {
            CESMRRecurrence* recurrence = new (ELeave) CESMRRecurrence;
            CleanupStack::PushL( recurrence );
            recurrence->ConstructFromResourceL( env, reader );
            iArray.AppendL( recurrence );
            CleanupStack::Pop( recurrence );
            // set default recurrence
            if ( recurrence->RecurrenceValue() == recurrenceVal )
                {
                iIndex = i;
                SetRecurrenceL ( iIndex );
                }
            }
        // resource reader
        CleanupStack::PopAndDestroy(); // codescanner::cleanup 
        }
    else
        {
        __ASSERT_DEBUG( iObserver, Panic( EESMRRecurenceFieldNoObserver ) );
        iObserver->HideControl ( iFieldId );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdOpenRecurrenceQuery, R_QTN_MSK_OPEN);
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRRecurenceField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    if (aCommand == EESMRCmdOpenRecurrenceQuery ||
    	aCommand == EAknCmdOpen )
        {
        ExecuteRecurrenceQueryL();
        isUsed = ETrue;
        
        HandleTactileFeedbackL();
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::ExecuteRecurrenceQueryL
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::ExecuteRecurrenceQueryL()
    {
    FUNC_LOG;
    TInt ret = CESMRListQuery::ExecuteL( 
            CESMRListQuery::EESMRRecurrenceQuery );

    if ( ret != KErrCancel )
        {
        SetRecurrenceL( ret );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurrenceField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRRecurenceField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( EEventKey == aType )
        {
        switch ( aEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                {
                if ( iIndex > 0 )
                    {
                    SetRecurrenceL ( iIndex - 1 );
                    response = EKeyWasConsumed;
                    }
                }
                break;

            case EStdKeyRightArrow:
                {
                if ( iIndex < (iArray.Count() - 1) )
                    {
                    SetRecurrenceL ( iIndex + 1 );
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
// CESMRRecurrenceField::SetRecurrence
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::SetRecurrenceL(
        TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex < 0 || aIndex >= iArray.Count() )
        {
        User::Leave( KErrArgument );
        }

    CESMRRecurrence* rec = iArray[ aIndex ];
    TRAPD( err, iValidator->RecurrenceChangedL( rec->RecurrenceValue() ) );
    if ( err == KErrNone )
        {
        iRecurrence->SetTextL( rec->RecurrenceText() );
        iRecurrence->DrawDeferred();

        iIndex = aIndex;

        if ( rec->RecurrenceValue() != ERecurrenceNot )
            {
            // if recurrence is switched "on", the end date should be removed
            iObserver->HideControl ( EESMRFieldStopDate );
            iObserver->ShowControl ( EESMRFieldRecurrenceDate );
            }
        else
            {
            // if recurrence is switched off, end date should be visible
            iObserver->ShowControl ( EESMRFieldStopDate );
            iObserver->HideControl ( EESMRFieldRecurrenceDate );
            }
        }
    else
        {
        // Error occured
        CESMRGlobalNote::ExecuteL(
                CESMRGlobalNote::EESMRRepeatDifferentStartAndEndDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect = Rect();
    
    TAknLayoutRect rowLayoutRect =
     NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    rect = rowLayoutRect.Rect();
    
    TAknWindowComponentLayout iconLayout =
     NMRLayoutManager::GetWindowComponentLayout( 
             NMRLayoutManager::EMRLayoutTextEditorIcon );
    AknLayoutUtils::LayoutImage( iFieldIcon, rect, iconLayout );
    
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
    iRecurrence->SetRect( labelLayout.TextRect() );
    
    // Setting font also for the field
    iRecurrence->SetFont( labelLayout.Font() );
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRRecurenceField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iFieldIcon )
        {
        ++count;
        }
    if ( iRecurrence )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRRecurenceField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iRecurrence;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRRecurenceField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRRecurenceField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iRecurrence->SetContainerWindowL( aContainer );
    
    iRecurrence->SetParent( this );
    }

// EOF
