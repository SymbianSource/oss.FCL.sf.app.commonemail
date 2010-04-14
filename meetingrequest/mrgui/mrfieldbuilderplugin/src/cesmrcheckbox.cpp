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
* Description:  ESMR All day event checkbox field implementation
 *
*/

#include "cesmrcheckbox.h"
#include "mesmrlistobserver.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrfieldvalidator.h"
#include "esmrconfig.hrh"
#include "esmrhelper.h"
#include "nmrbitmapmanager.h"
#include "cmrimage.h"
#include "cmrlabel.h"
#include "nmrlayoutmanager.h"
#include "mesmrfieldeventqueue.h"
#include "cesmrgenericfieldevent.h"

#include <esmrgui.rsg>
#include <stringloader.h>
#include <data_caging_path_literals.hrh>

// DEBUG
#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRCheckbox::NewL
// ---------------------------------------------------------------------------
//
CESMRCheckBox* CESMRCheckBox::NewL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRCheckBox* self = new( ELeave )CESMRCheckBox( aValidator );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CESMRCheckbox
// ---------------------------------------------------------------------------
//
CESMRCheckBox::CESMRCheckBox( MESMRFieldValidator* aValidator ) 
: iChecked( EFalse )
    {
    FUNC_LOG;
    
    iValidator = aValidator;
    
    SetFieldId( EESMRFieldAllDayEvent );
    SetFocusType ( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::ConstructL()
    {
    FUNC_LOG;
    iLabel = CMRLabel::NewL();
    iLabel->SetParent( this );
    
    CESMRField::ConstructL( iLabel ); //ownership transfered
    
    HBufC* txt = StringLoader::LoadLC ( R_QTN_MEET_REQ_ALL_DAY_EVENT );
    iLabel->SetTextL( *txt );
    CleanupStack::PopAndDestroy( txt );

    // Creating field icon
    SetIconL ( iChecked );    
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::~CESMRCheckbox
// ---------------------------------------------------------------------------
//
CESMRCheckBox::~CESMRCheckBox( )
    {
    FUNC_LOG;
    delete iFieldIcon;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL ( aFocus );

    //Focus gained
    if ( aFocus )
        {
        SwitchMSKLabelL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ExecuteGenericCommandL
// ---------------------------------------------------------------------------
//
TBool CESMRCheckBox::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    // EAknCmdOpen is added for the Pointer events, see ListPane
    if( aCommand == EESMRCmdCheckEvent || aCommand == EAknCmdOpen )
        {
    	HandleTactileFeedbackL();
    
        HandleCheckEventL();
        SwitchMSKLabelL();
        SendFieldChangeEventL( EESMRFieldAllDayEvent );
        isUsed = ETrue;
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRCheckBox::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        if ( aEvent.iScanCode == EStdKeyDevice3 )
            {
            HandleCheckEventL();
            SwitchMSKLabelL();
            SendFieldChangeEventL( EESMRFieldAllDayEvent );
            response = EKeyWasConsumed;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CESMRCheckBox::CountComponentControls( ) const
    {
    FUNC_LOG;
    TInt count( 0 );
    if ( iFieldIcon )
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
// CESMRCheckbox::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CESMRCheckBox::ComponentControl( TInt aInd ) const
    {
    FUNC_LOG;
    switch ( aInd )
        {
        case 0:
            return iFieldIcon;
        case 1:
            return iLabel;
        default:
            return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SizeChanged
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SizeChanged()
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
    iLabel->SetRect( labelLayout.TextRect() );
        
    // Setting font also for the label
    iLabel->SetFont( labelLayout.Font() );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    TBool alldayEvent( aEntry.IsAllDayEventL() );

    // Update the validator
    iValidator->SetAllDayEventL( alldayEvent );

    // if this is all day event and not checked
    if ( alldayEvent && !iChecked )
        {
        HandleCheckEventL();
        }
    SetIconL( iChecked );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::ExternalizeL( MESMRCalEntry& /*aEntry*/)
    {
    FUNC_LOG;
    iValidator->SetAllDayEventL( iChecked );
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetIconL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SetIconL( TBool aChecked )
    {
    FUNC_LOG;
    delete iFieldIcon;
    iFieldIcon = NULL;
    
    NMRBitmapManager::TMRBitmapId iconID;
    if( aChecked )
        {
        iconID = NMRBitmapManager::EMRBitmapCheckBoxOn;
        }
    else
        {
        iconID = NMRBitmapManager::EMRBitmapCheckBoxOff;
        }
    
    iFieldIcon = CMRImage::NewL( iconID );
    iFieldIcon->SetParent( this );

    SizeChanged();
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::HandleCheckEventL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::HandleCheckEventL( )
    {
    FUNC_LOG;
    /*
     * Change check status
     */
    iChecked = !iChecked; // change status
    SetIconL ( iChecked ); // set icon for the status

    // update validator status:
    iValidator->SetAllDayEventL ( iChecked );

    /*
     * Add / remove new fields to list
     */
    if ( iChecked )
        {
        // start - end time should be removed
        // relateive alarm should be removed
        iObserver->HideControl ( EESMRFieldMeetingTime );
        iObserver->HideControl ( EESMRFieldAlarm );
        // absolute alarm should be inserted
        iObserver->ShowControl ( EESMRFieldAlarmOnOff );
        }
    else
        {
        iObserver->ShowControl ( EESMRFieldMeetingTime );
        iObserver->ShowControl ( EESMRFieldAlarm );
        iObserver->HideControl ( EESMRFieldAlarmOnOff );
        iObserver->HideControl ( EESMRFieldAlarmTime );
        iObserver->HideControl ( EESMRFieldAlarmDate );
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SwitchMSKLabelL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SwitchMSKLabelL()
    {
    FUNC_LOG;
    if( iChecked )
        {
        ChangeMiddleSoftKeyL(EESMRCmdCheckEvent, R_QTN_MSK_UNMARK);
        }
    else
        {
        ChangeMiddleSoftKeyL(EESMRCmdCheckEvent, R_QTN_MSK_MARK);
        }
    }

// ---------------------------------------------------------------------------
// CESMRCheckbox::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    iLabel->SetContainerWindowL( aContainer );
    iLabel->SetParent( this );
    }
// ---------------------------------------------------------------------------
// CESMRCheckBox::SendFieldChangeEventL
// ---------------------------------------------------------------------------
//
void CESMRCheckBox::SendFieldChangeEventL(
        TESMREntryFieldId aFieldId )
    {
    FUNC_LOG;
    if ( iEventQueue )
        {
        CESMRGenericFieldEvent* event =
            CESMRGenericFieldEvent::NewLC( NULL,
                                           MESMRFieldEvent::EESMRFieldChangeEvent );
        TInt fieldId = aFieldId;
        CESMRFieldEventValue* field = CESMRFieldEventValue::NewLC(
                CESMRFieldEventValue::EESMRInteger, &fieldId );
        event->AddParamL( field );
        CleanupStack::Pop( field );
        CESMRFieldEventValue* checked = CESMRFieldEventValue::NewLC(
                CESMRFieldEventValue::EESMRInteger, &iChecked );
        event->AddParamL( checked );
        CleanupStack::Pop( checked );     
        iEventQueue->NotifyEventL( *event );
        CleanupStack::PopAndDestroy( event );
        }
    }
// EOF
