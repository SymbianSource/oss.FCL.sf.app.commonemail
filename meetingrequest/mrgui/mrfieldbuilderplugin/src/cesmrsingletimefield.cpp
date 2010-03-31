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
* Description:  ESMR single time field implementation
 *
*/

#include "cesmrsingletimefield.h"
#include "mesmrfieldvalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "nmrbitmapmanager.h"

#include <aknsbasicbackgroundcontrolcontext.h>
#include <eikmfne.h>

#include "emailtrace.h"

namespace{  // codescanner::namespace
#define KMinimumTime (TTime(0)) // codescanner::baddefines

// 24 hours in microseconds
const TInt64 KDayInMicroSeconds = 86400000000;
#define KMaximumTime (TTime(KDayInMicroSeconds)) // codescanner::baddefines 
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::CESMRSingleTimeField
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField::CESMRSingleTimeField( MESMRFieldValidator* aValidator ) 
    {
    FUNC_LOG;
    
    iValidator = aValidator;
    SetFieldId ( EESMRFieldAlarmTime );
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::~CESMRSingleTimeField
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField::~CESMRSingleTimeField()
    {
    FUNC_LOG;
    delete iFieldIcon;
    delete iBgCtrlContext;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::NewL
// ---------------------------------------------------------------------------
//
CESMRSingleTimeField* CESMRSingleTimeField::NewL( 
        MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRSingleTimeField* self =
            new( ELeave )CESMRSingleTimeField( aValidator );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::ConstructL( )
    {
    FUNC_LOG;
    
    SetComponentsToInheritVisibility( ETrue );
    
    TTime startTime;
    startTime.HomeTime();

    iTime = new( ELeave )CEikTimeEditor();
    
    CESMRField::ConstructL( iTime ); //ownership transferred
    
    iTime->ConstructL(
            KMinimumTime,
            KMaximumTime,
            startTime,
            EEikTimeWithoutSecondsField );
    iTime->SetUpAndDownKeysConsumed( EFalse );
  
    iFieldIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarmClock );
    iFieldIcon->SetParent( this );
    
    if( iValidator )
        {
        iValidator->SetAlarmTimeFieldL( *iTime );
        }
    
    TRect tempRect( 0, 0, 0, 0 );
    
    // Setting background instead of theme skin  
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL( 
                bitmapStruct.iItemId, 
                tempRect, 
                EFalse );
        
    iTime->SetSkinBackgroundControlContextL( iBgCtrlContext );
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRSingleTimeField::OkToLoseFocusL(
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    if( iValidator )
        {
        TRAP( err, iValidator->AlarmTimeChangedL() );
        }
    if( err != KErrNone )
        {
        CESMRGlobalNote::ExecuteL(
                CESMRGlobalNote::EESMRCalenLaterDate );
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRSingleTimeField::OfferKeyEventL( const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( aType == EEventKey )
        {
        TInt fieldIndex( iTime->CurrentField() );

        switch( aEvent.iScanCode )
            {
            // make sure these events are NOT consumed
            case EStdKeyUpArrow:
            case EStdKeyDownArrow:
                break;

            default:
                response = iTime->OfferKeyEventL ( aEvent, aType );
                iTime->DrawDeferred();
                break;
            }

        CheckIfValidatingNeededL( fieldIndex );
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt err( KErrNone );
    TInt fieldIndex( iTime->CurrentField() );

    if( fieldIndex != aStartFieldIndex )
        {
        TRAP( err, iValidator->AlarmTimeChangedL() );
        }

    if( err != KErrNone )
        {
        CESMRGlobalNote::ExecuteL (
                CESMRGlobalNote::EESMRCalenLaterDate );
        }
    }
// ---------------------------------------------------------------------------
// CESMRSingleTimeField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );
    
    if( aFocus )
        {
        ChangeMiddleSoftKeyL( EESMRCmdSaveMR, R_QTN_MSK_SAVE );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::SetValidatorL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    
    CESMRField::SetValidatorL( aValidator );
    
    if ( iValidator )
        {
        iValidator->SetAlarmTimeFieldL( *iTime );
        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
TBool CESMRSingleTimeField::ExecuteGenericCommandL( TInt aCommand )
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
// CESMRSingleTimeField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::SizeChanged()
    {
     TRect rect( Rect() );

    // Layouting field icon
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutDateEditorIcon );
        AknLayoutUtils::LayoutImage( iFieldIcon, rect, iconLayout );
        }
        
    // Layouting time editor
    if( iTime )
        {
        TAknLayoutRect bgLayoutRect = 
            NMRLayoutManager::GetLayoutRect( 
                    rect, NMRLayoutManager::EMRLayoutTextEditorBg );
        TRect bgRect( bgLayoutRect.Rect() );
        // Move focus rect so that it's relative to field's position.
        bgRect.Move( -Position() );
        SetFocusRect( bgRect );
        
        TAknTextComponentLayout editorLayout =
            NMRLayoutManager::GetTextComponentLayout( 
                    NMRLayoutManager::EMRTextLayoutDateEditor );
        AknLayoutUtils::LayoutMfne( iTime, rect, editorLayout );
        
        NMRColorManager::SetColor( *iTime, 
                                   NMRColorManager::EMRMainAreaTextColor );

        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CESMRSingleTimeField::CountComponentControls() const
    {
    TInt count( 0 );
    if( iFieldIcon )
        {
        ++count;
        }

    if( iTime )
        {
        ++count;
        }
    return count;
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRSingleTimeField::ComponentControl( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iTime;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    iContainerWindow = &aContainer;
    
    CCoeControl::SetContainerWindowL( *iContainerWindow );
    
    iFieldIcon->SetContainerWindowL( *iContainerWindow );
    iTime->SetContainerWindowL( *iContainerWindow );
    
    iFieldIcon->SetParent( this );
    iTime->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRSingleTimeField::DoEnvChangeL
// ---------------------------------------------------------------------------
//
void CESMRSingleTimeField::DoEnvChangeL()
    {
    FUNC_LOG;
    
    CEikTimeEditor* time = new( ELeave )CEikTimeEditor;
    CleanupStack::PushL( time );
    
    TTime startTime;
    startTime.HomeTime();    
    
    time->ConstructL(
            KMinimumTime,
            KMaximumTime,
            startTime,
            EEikTimeWithoutSecondsField );
    
    time->SetUpAndDownKeysConsumed( EFalse );       
    
    UpdateExtControlL( time );
    
    CleanupStack::Pop( time );
    iTime = time;

    iTime->SetSkinBackgroundControlContextL( iBgCtrlContext );        
    SetContainerWindowL( *iContainerWindow );    
    
    if ( iValidator )
        {
        iValidator->SetAlarmTimeFieldL( *iTime );
        }
    
    iTime->ActivateL();
    
    SizeChanged();
    DrawDeferred();    
    }

// EOF

