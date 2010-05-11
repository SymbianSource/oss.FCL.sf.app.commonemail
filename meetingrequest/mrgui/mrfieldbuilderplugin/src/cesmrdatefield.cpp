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
* Description:  ESMR date field impl.
 *
*/

#include "cesmrdatefield.h"
#include "cesmrmeetingtimevalidator.h"
#include "cesmrglobalnote.h"
#include "esmrfieldbuilderdef.h"
#include "mesmrlistobserver.h"
#include "cesmrgenericfieldevent.h"
#include "cmrimage.h"
#include "nmrlayoutmanager.h"
#include "nmrcolormanager.h"
#include "nmrbitmapmanager.h"

#include <AknsBasicBackgroundControlContext.h>

#include <eikmfne.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRDateField::CESMRDateField
// ---------------------------------------------------------------------------
//
CESMRDateField::CESMRDateField( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    
    iValidator = aValidator;
        
    SetFocusType( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRDateField::~CESMRDateField
// ---------------------------------------------------------------------------
//
CESMRDateField::~CESMRDateField()
    {
    FUNC_LOG;
    delete iFieldIcon;
    delete iBgCtrlContext;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::NewL
// ---------------------------------------------------------------------------
//
CESMRDateField* CESMRDateField::NewL(
        MESMRFieldValidator* aValidator,
        TESMREntryFieldId aId )
    {
    FUNC_LOG;
    CESMRDateField* self = new(ELeave)CESMRDateField( aValidator );
    CleanupStack::PushL( self );
    self->ConstructL( aId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRDateField::ConstructL( TESMREntryFieldId aId )
    {
    FUNC_LOG;
    SetFieldId ( aId );
    SetComponentsToInheritVisibility( ETrue );
    
    TTime startTime;
    startTime.UniversalTime();

    iDate = new( ELeave )CEikDateEditor;
    iDate->ConstructL( 
            TTIME_MINIMUMDATE, TTIME_MAXIMUMDATE, startTime, EFalse );
    iDate->SetUpAndDownKeysConsumed( EFalse );
    
    CESMRField::ConstructL( iDate ); //ownership transferred
    
    NMRBitmapManager::TMRBitmapId iconId = 
        ( iFieldId == EESMRFieldStartDate ) ? 
            NMRBitmapManager::EMRBitmapDateStart : 
            NMRBitmapManager::EMRBitmapDateEnd;

    iFieldIcon = CMRImage::NewL( iconId );
    iFieldIcon->SetParent( this );

    // Initialize validator
    InitializeValidatorL();
    
    // Setting background instead of theme skin 
    TRect tempRect(0, 0, 0, 0);
    NMRBitmapManager::TMRBitmapStruct bitmapStruct;
    bitmapStruct = NMRBitmapManager::GetBitmapStruct( NMRBitmapManager::EMRBitmapInputCenter );

    iBgCtrlContext = CAknsBasicBackgroundControlContext::NewL( 
                bitmapStruct.iItemId, 
                tempRect, 
                EFalse );
        
    iDate->SetSkinBackgroundControlContextL( iBgCtrlContext );    
    }

// ---------------------------------------------------------------------------
// CESMRDateField::OkToLoseFocusL
// ---------------------------------------------------------------------------
//
TBool CESMRDateField::OkToLoseFocusL(
        TESMREntryFieldId /*aNextItem*/ )
    {
    FUNC_LOG;
    return TriggerValidatorL();
    }

// ---------------------------------------------------------------------------
// CESMRDateField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRDateField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);

    TInt fieldIndex( iDate->CurrentField() );

    if ( aType == EEventKey )
        {

        // flowthrough, these events shouldn't be consumed
        if ( aEvent.iScanCode != EStdKeyUpArrow &&
             aEvent.iScanCode != EStdKeyDownArrow )
            {
            response = iDate->OfferKeyEventL( aEvent, aType );
            iDate->DrawDeferred();
            }
        }
    
    TInt endIndex( iDate->CurrentField());

    if ( ( response == EKeyWasConsumed ) &&
    	 ( fieldIndex != endIndex) )
    	{
    	CheckIfValidatingNeededL ( fieldIndex );
    	}
    
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRDateField::CheckIfValidatingNeededL
// ---------------------------------------------------------------------------
//
void CESMRDateField::CheckIfValidatingNeededL(
        TInt aStartFieldIndex )
    {
    FUNC_LOG;
    TInt fieldIndex( iDate->CurrentField() );

    if ( fieldIndex != aStartFieldIndex )
        {
        TriggerValidatorL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::TriggerValidatorL
// ---------------------------------------------------------------------------
//
TBool CESMRDateField::TriggerValidatorL()
    {
    FUNC_LOG;
    TInt err( KErrNone );

    if ( iValidator )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                TRAP( err, iValidator->StartDateChandedL() );
                break;
            case EESMRFieldStopDate:
                TRAP(err, iValidator->EndDateChangedL() );
                break;
            case EESMRFieldAlarmDate:
                TRAP(err, iValidator->AlarmDateChangedL() );
                break;
            default:
                break;
            }
        }

    if ( KErrNone != err )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                {
                if ( err == KErrOverflow )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatReSchedule );
                    }
                else if ( err == KErrUnderflow )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatInstanceTooEarly );                    
                    }
                }
                break;                
            case EESMRFieldStopDate:
                {
                if ( err == KErrArgument )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMREndsBeforeStarts );
                    }
                else if ( err == KErrOverflow )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatReSchedule );                    
                    }
                else if ( err == KErrUnderflow )
                    {
                    CESMRGlobalNote::ExecuteL(
                            CESMRGlobalNote::EESMRRepeatInstanceTooEarly );                    
                    }
                }
                break;

            case EESMRFieldAlarmDate:
                {
                CESMRGlobalNote::ExecuteL(
                        CESMRGlobalNote::EESMRCalenLaterDate );
                }
                break;
            
            default:
                break;
            }
        }
    
    return ( KErrNone == err );
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRDateField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );
    if ( aFocus )
        {
        ChangeMiddleSoftKeyL( EESMRCmdSaveMR,R_QTN_MSK_SAVE );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SetValidatorL
// ---------------------------------------------------------------------------
//
void CESMRDateField::SetValidatorL( MESMRFieldValidator* aValidator )
    {
    CESMRField::SetValidatorL( aValidator );
    InitializeValidatorL();
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SizeChanged()
// ---------------------------------------------------------------------------
//
TBool CESMRDateField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    
    TBool retValue( EFalse );
    
    if ( EMRCmdDoEnvironmentChange == aCommand )
        {
        DoEnvChangeL();
        retValue = ETrue;
        }
    
    return retValue;
    }


// ---------------------------------------------------------------------------
// CESMRDateField::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRDateField::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if( iFieldIcon )
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
// CESMRDateField::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRDateField::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    
    switch ( aInd )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iDate;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRDateField::SizeChanged()
    {
    FUNC_LOG;
    TRect rect( Rect() );

    // Layouting field icon
    if( iFieldIcon )
        {
        TAknWindowComponentLayout iconLayout = 
            NMRLayoutManager::GetWindowComponentLayout( 
                    NMRLayoutManager::EMRLayoutDateEditorIcon );
        AknLayoutUtils::LayoutImage( iFieldIcon, rect, iconLayout );
        }
    
    // Layouting date editor
    if( iDate )
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
        AknLayoutUtils::LayoutMfne( iDate, rect, editorLayout );

        NMRColorManager::SetColor( *iDate, 
                                   NMRColorManager::EMRMainAreaTextColor );
        }
    }

// ---------------------------------------------------------------------------
// CESMRDateField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRDateField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {    
    iContainerWindow = &aContainer;
    
    CCoeControl::SetContainerWindowL( *iContainerWindow );
    iDate->SetContainerWindowL( *iContainerWindow );
    iFieldIcon->SetContainerWindowL( *iContainerWindow );
    
    iDate->SetParent( this );
    iFieldIcon->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CESMRDateField::InitializeValidatorL
// ---------------------------------------------------------------------------
//
void CESMRDateField::InitializeValidatorL()
    {
    FUNC_LOG;
    
    // Initialize validator
    if ( iValidator )
        {
        switch ( iFieldId )
            {
            case EESMRFieldStartDate:
                iValidator->SetStartDateFieldL( *iDate );
                break;
            case EESMRFieldStopDate:
                iValidator->SetEndDateFieldL( *iDate );
                break;
            case EESMRFieldAlarmDate:
                iValidator->SetAlarmDateFieldL( *iDate );
                break;
            default:
                break;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CESMRDateField::DoEnvChangeL
// ---------------------------------------------------------------------------
//
void CESMRDateField::DoEnvChangeL()
    {
    FUNC_LOG;
    
    CEikDateEditor* date = new( ELeave )CEikDateEditor;
    CleanupStack::PushL( date );
    
    TTime startTime;
    startTime.UniversalTime();
    
    date->ConstructL( 
            TTIME_MINIMUMDATE, 
            TTIME_MAXIMUMDATE, 
            startTime, 
            EFalse );
    
    date->SetUpAndDownKeysConsumed( EFalse );        
    
    UpdateExtControlL( date );
    
    CleanupStack::Pop( date );
    iDate = date;
    InitializeValidatorL();
    
    iDate->SetSkinBackgroundControlContextL( iBgCtrlContext );        
    SetContainerWindowL( *iContainerWindow );
    
    iDate->ActivateL();        
    SizeChanged();
    DrawDeferred();    
    }

// EOF
