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
* Description:  ESMR repeat until field implementation
 *
*/


#include "cesmrrepeatuntil.h"
#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "cmrlabel.h"
#include "nmrbitmapmanager.h"

#include <eikmfne.h>
#include <stringloader.h>
#include <esmrgui.rsg>
#include <aknsbasicbackgroundcontrolcontext.h>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CESMRRepeatUntilField
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField::CESMRRepeatUntilField( 
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    
    iValidator = aValidator;
    SetFieldId( EESMRFieldRecurrenceDate );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::~CESMRRepeatUntilField
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField::~CESMRRepeatUntilField( )
    {
    FUNC_LOG;
    delete iLabel;
    delete iBgCtrlContext;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::NewL
// ---------------------------------------------------------------------------
//
CESMRRepeatUntilField* CESMRRepeatUntilField::NewL(
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRRepeatUntilField* self =
            new( ELeave )CESMRRepeatUntilField( aValidator );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::ConstructL( )
    {
    FUNC_LOG;
    SetComponentsToInheritVisibility( ETrue );
    HBufC* label = StringLoader::LoadLC( R_QTN_MEET_REQ_REPEAT_UNTIL );

    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    iLabel->SetTextL( *label );
    CleanupStack::PopAndDestroy( label );

    TTime startTime;
    startTime.UniversalTime();

    iDate = new( ELeave )CEikDateEditor;
    CESMRField::ConstructL( iDate ); //ownership transferred
    
    iDate->ConstructL(
            KAknMinimumDate,
            TTIME_MAXIMUMDATE,
            startTime,
            EFalse );
    
    iDate->SetUpAndDownKeysConsumed ( EFalse );

    if ( iValidator )
        {
        iValidator->SetRecurrenceUntilDateFieldL( *iDate );
        }
    
    TRect tempRect( 0, 0, 0, 0 );
    
    // Setting background instead of theme skin  
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL( 
                bitmapStruct.iItemId, 
                tempRect, 
                EFalse );
            
    iDate->SetSkinBackgroundControlContextL( iBgCtrlContext );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::MinimumSize
// ---------------------------------------------------------------------------
//
TSize CESMRRepeatUntilField::MinimumSize()
    {
    TRect parentRect( Parent()->Rect() );
       
    TRect richTextRect = 
            NMRLayoutManager::GetFieldLayoutRect( parentRect, 1 ).Rect();
    
    // Add title area to the required size
    TSize titleSize( CESMRField::MinimumSize() );
    
    TSize completeFieldSize( titleSize );
    completeFieldSize.iHeight += richTextRect.Height();
    
    return completeFieldSize;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRRepeatUntilField::OkToLoseFocusL( TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    TRAPD(err, iValidator->RecurrenceEndDateChangedL() );

    if ( KErrNone != err )
        {
        CESMRGlobalNote::ExecuteL(
                CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
        }
    return( KErrNone == err );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRRepeatUntilField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if( iLabel )
        {
        ++count;
        }
    if( iDate )
        {
        ++count;
        }
        
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRRepeatUntilField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    
    switch ( aInd )
        {
        case 0:
            return iLabel;
        case 1:
            return iDate;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );
    
    // Layouting label
    TAknLayoutRect rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 1 );
    TRect rowRect = rowLayoutRect.Rect();
    
    // Layout label to first row's rect
    TAknTextComponentLayout titleLayout =
        NMRLayoutManager::GetTextComponentLayout( 
                NMRLayoutManager::EMRTextLayoutText );
    AknLayoutUtils::LayoutLabel( iLabel, rect, titleLayout );
        
    // Move upper left corner below first line and get second row's rect.
    rect.iTl.iY += rowRect.Height();
    rowLayoutRect =
        NMRLayoutManager::GetFieldRowLayoutRect( rect, 2 );
    rowRect = rowLayoutRect.Rect();
    
    // Layouting date editor
    if( iDate )
        {
        TAknLayoutRect bgLayoutRect = 
            NMRLayoutManager::GetLayoutRect( 
                    rowRect, NMRLayoutManager::EMRLayoutTextEditorBg );
        TRect bgRect( bgLayoutRect.Rect() );
        // Move focus rect so that it's relative to field's position.
        bgRect.Move( -Position() );
        SetFocusRect( bgRect );
                       
        TAknTextComponentLayout editorLayout =
            NMRLayoutManager::GetTextComponentLayout( 
                    NMRLayoutManager::EMRTextLayoutDateEditor );
        AknLayoutUtils::LayoutMfne( iDate, rect, editorLayout );
        
        NMRColorManager::SetColor( *iDate, 
                                   NMRColorManager::EMRMainAreaTextColor );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRRepeatUntilField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);

    if ( aType == EEventKey )
        {
        TInt fieldIndex( iDate->CurrentField() );
        response = CESMRField::OfferKeyEventL( aEvent, aType );
        
        if ( aEvent.iScanCode != EStdKeyUpArrow &&
             aEvent.iScanCode != EStdKeyDownArrow )
        	{
            iDate->DrawDeferred();
        	}
        }

    return response;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt fieldIndex( iDate->CurrentField() );

    if ( fieldIndex != aStartFieldIndex && iValidator )
        {
        TRAPD(err, iValidator->RecurrenceEndDateChangedL() );

        if ( KErrNone != err )
            {
            CESMRGlobalNote::ExecuteL(
                    CESMRGlobalNote::EESMRRepeatEndEarlierThanItStart );
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL(EESMRCmdSaveMR,R_QTN_MSK_SAVE);
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SetValidatorL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    
    CESMRField::SetValidatorL( aValidator );
    
    if ( iValidator )
        {
        iValidator->SetRecurrenceUntilDateFieldL( *iDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CESMRRepeatUntilField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    if ( EMRCmdDoEnvironmentChange == aCommand )
        {
        // Locale has been changed       
        DoEnvChangeL();
        retValue = ETrue;
        }
    
    return retValue;
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    iContainerWindow = &aContainer;
    
    CCoeControl::SetContainerWindowL( aContainer );
    iDate->SetContainerWindowL( aContainer );
    iLabel->SetContainerWindowL( aContainer );
    
    iDate->SetParent( this );
    iLabel->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRRepeatUntilField::DoEnvChangeL
// ---------------------------------------------------------------------------
//
void CESMRRepeatUntilField::DoEnvChangeL()
    {
    FUNC_LOG;
    
    CEikDateEditor* date = new( ELeave )CEikDateEditor;
    CleanupStack::PushL( date );
    
    date->ConstructL( 
            TTIME_MINIMUMDATE, 
            TTIME_MAXIMUMDATE, 
            iDate->Date(), 
            EFalse );
    date->SetUpAndDownKeysConsumed( EFalse );        
    
    UpdateExtControlL( date );
    
    CleanupStack::Pop( date );
    iDate = date;
    
    if ( iValidator )
        {
        iValidator->SetRecurrenceUntilDateFieldL( *iDate );
        }    
    
    iDate->SetSkinBackgroundControlContextL( iBgCtrlContext );        
    SetContainerWindowL( *iContainerWindow );
    
    iDate->ActivateL();        
    SizeChanged();
    DrawDeferred();      
    }

// EOF

