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
* Description:  ESMR alarm on off field implementation
*
*/

#include "emailtrace.h"
#include "cmralarmonofffield.h"
#include "cmrlabel.h"
#include "cmrimage.h"
#include "mesmrcalentry.h"
#include "mesmrlistobserver.h"
#include "mesmrfieldvalidator.h"
#include "nmrlayoutmanager.h"

#include <StringLoader.h>
#include <avkon.hrh>
#include <esmrgui.rsg>
#include <AknUtils.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::NewL
// ---------------------------------------------------------------------------
//
CMRAlarmOnOffField* CMRAlarmOnOffField::NewL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CMRAlarmOnOffField* self = new (ELeave) CMRAlarmOnOffField();
    CleanupStack::PushL ( self );
    self->ConstructL ( aValidator );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::CMRAlarmOnOffField
// ---------------------------------------------------------------------------
//
CMRAlarmOnOffField::CMRAlarmOnOffField() 
    {
    FUNC_LOG;
    iStatus = ETrue;
    
    SetFieldId ( EESMRFieldAlarmOnOff );
    SetFocusType ( EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::ConstructL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::ConstructL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    iValidator = aValidator;
    iValidator->SetAbsoluteAlarmOnOffFieldL( *this );
        
    iIcon = CMRImage::NewL( NMRBitmapManager::EMRBitmapAlarm );
    iIcon->SetParent( this );
    
    iLabel = CMRLabel::NewL();
    CESMRField::ConstructL( iLabel ); // ownership transfered
    iLabel->SetTextL( KNullDesC() );
    iLabel->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::~CMRAlarmOnOffField
// ---------------------------------------------------------------------------
//
CMRAlarmOnOffField::~CMRAlarmOnOffField( )
    {
    FUNC_LOG;
    delete iIcon;
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::InternalizeL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if ( aEntry.IsStoredL() ) // entry is already stored in db
        {
        MESMRCalEntry::TESMRAlarmType alarmType;
        TTime alarmTime;
        aEntry.GetAlarmL( alarmType, alarmTime );

        switch( alarmType )
            {
            //flow through
            case MESMRCalEntry::EESMRAlarmAbsolute:
            case MESMRCalEntry::EESMRAlarmRelative: // Show alarm as - ON
                {
                iStatus = ETrue;
                }
                break;
                //flow through
            case MESMRCalEntry::EESMRAlarmNotFound:
            default: // Show alarm as - OFF
                {
                iStatus = EFalse;
                }
                break;
            }
        }
    else // This is a new entry and default implementation is required
        {
        switch( aEntry.Type() )
            {
            case MESMRCalEntry::EESMRCalEntryTodo: // To-do
                {
                TTime alarmTime;
                MESMRCalEntry::TESMRAlarmType alarmType;
                aEntry.GetAlarmL (alarmType, alarmTime );

                // Change status to "ON" if alarm has been set by default
                iStatus = alarmType != MESMRCalEntry::EESMRAlarmNotFound;
                }
                break;
            // Flow through
            case MESMRCalEntry::EESMRCalEntryMeetingRequest: // Meeting request
            case MESMRCalEntry::EESMRCalEntryMeeting: // Meeting
                {
                iStatus = ETrue;// Change the status to "ON" for these dialog default
                }
                break;
                
            case MESMRCalEntry::EESMRCalEntryAnniversary: // Anniversary    
                {
                iStatus = ETrue;// Change the status to "ON" for these dialog default
                }
                break;
                // Flow through
            case MESMRCalEntry::EESMRCalEntryMemo: // Memo
            case MESMRCalEntry::EESMRCalEntryReminder: // Reminder
            
            default:
                iStatus = EFalse; // Change the status to "OFF" for these dialog default
                break;
            }
        }
    ResetFieldL();
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CMRAlarmOnOffField::OfferKeyEventL( const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed );
    if ( aType == EEventKey )
        {
        switch( aEvent.iScanCode )
            {
            // Flowthrough, doesn't matter which key was pressed since
            // the value is always changes from "off" to "on" or vice versus.
            case EStdKeyDevice3: // selection key
            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
                iStatus = !iStatus; // change status
                ResetFieldL();
                response = EKeyWasConsumed;
                break;

            default:
                break;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    iLabel->SetFocus( aFocus );
    
    if( aFocus ) //Focus is gained on the field
        {
        if ( iStatus )
            {
            ChangeMiddleSoftKeyL( EESMRCmdAlarmOff,R_QTN_MEET_REQ_ALARM_OFF ); //R_TEXT_SOFTKEY_OFF
            }
        else
            {
            ChangeMiddleSoftKeyL( EESMRCmdAlarmOn,R_QTN_MEET_REQ_ALARM_ON ); //R_TEXT_SOFTKEY_ON
            }
        }
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::MakeVisible
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::MakeVisible( TBool aVisible )
    {
    FUNC_LOG;
    CCoeControl::MakeVisible( aVisible );
    if ( iObserver )
        {
        TRAP_IGNORE( ResetFieldL() );
        }
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
TBool CMRAlarmOnOffField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    TBool isUsed( EFalse );
    // EAknCmdOpen is added for the Pointer events, see ListPane
    if( aCommand == EESMRCmdAlarmOn || aCommand == EESMRCmdAlarmOff 
            || aCommand == EAknCmdOpen )
        {
		HandleTactileFeedbackL();
		
        SwitchMiddleSoftKeyL();
        isUsed = ETrue;
        }
    return isUsed;
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::SwitchMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SwitchMiddleSoftKeyL()
    {
    FUNC_LOG;
    iStatus = !iStatus; // change status
    ResetFieldL();
    if( iStatus )
        {
        ChangeMiddleSoftKeyL( EESMRCmdAlarmOff,R_QTN_MEET_REQ_ALARM_OFF ); 
        }
    else
        {
        ChangeMiddleSoftKeyL( EESMRCmdAlarmOn,R_QTN_MEET_REQ_ALARM_ON ); 
        }
    
    if( iLabel->IsVisible() )
        {
        iLabel->DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::ResetFieldL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::ResetFieldL()
    {
    FUNC_LOG;
    iValidator->SetAlarmOnOffL( iStatus );

    HBufC* buf = StringLoader::LoadLC( iStatus ?
    R_QTN_MEET_REQ_ALARM_ON :
    R_QTN_MEET_REQ_ALARM_OFF );

    iLabel->SetTextL( *buf );
    CleanupStack::PopAndDestroy( buf );

    // if status on 'on', add time and date fields for
    // all day event alert, otherwise remove them.
    // if this field is not visible -> alldayevent not set
    // let's not show the date and time fields either.
    if ( iStatus && IsVisible() )
        {
        iObserver->ShowControl ( EESMRFieldAlarmTime );
        iObserver->ShowControl ( EESMRFieldAlarmDate );
        }
    else
        {
        iObserver->HideControl ( EESMRFieldAlarmTime );
        iObserver->HideControl ( EESMRFieldAlarmDate );
        }
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::SizeChanged()
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SizeChanged()
    {
    FUNC_LOG;
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
    
    // Setting font for the label also
    iLabel->SetFont( labelLayout.Font() );
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::CountComponentControls()
// ---------------------------------------------------------------------------
//
TInt CMRAlarmOnOffField::CountComponentControls() const
    {
    FUNC_LOG;
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
// CMRAlarmOnOffField::ComponentControl()
// ---------------------------------------------------------------------------
//
CCoeControl* CMRAlarmOnOffField::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
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
// CMRAlarmOnOffField::SetContainerWindowL
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SetContainerWindowL( 
        const CCoeControl& aContainer )
    {
    FUNC_LOG;
    CCoeControl::SetContainerWindowL( aContainer );
    iLabel->SetContainerWindowL( aContainer );
    iLabel->SetParent( this );
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::SetAbsoluteAlarmOn
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SetAbsoluteAlarmOn()
    {
    FUNC_LOG;
    iStatus = ETrue;
    }

// ---------------------------------------------------------------------------
// CMRAlarmOnOffField::SetAbsoluteAlarmOn
// ---------------------------------------------------------------------------
//
void CMRAlarmOnOffField::SetAbsoluteAlarmOff()
    {
    FUNC_LOG;
    iStatus = EFalse;
    }

// EOF
