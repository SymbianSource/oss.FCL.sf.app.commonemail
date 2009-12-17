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
* Description:  ESMR boolean field implementation
*
*/

#include "emailtrace.h"
#include "cesmrbooleanfield.h"

#include <eiklabel.h>
#include <StringLoader.h>
#include <avkon.hrh>
#include <esmrgui.rsg>
#include <AknUtils.h>
#include "mesmrcalentry.h"
#include "mesmrlistobserver.h"
#include "mesmrfieldvalidator.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRBooleanField::NewL
// ---------------------------------------------------------------------------
//
CESMRBooleanField* CESMRBooleanField::NewL( MESMRFieldValidator* aValidator )
    {
    FUNC_LOG;
    CESMRBooleanField* self = new (ELeave) CESMRBooleanField( aValidator );
    CleanupStack::PushL ( self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::~CESMRBooleanField
// ---------------------------------------------------------------------------
//
CESMRBooleanField::~CESMRBooleanField( )
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::InitializeL()
    {
    FUNC_LOG;
    iLabel->SetFont ( iLayout->Font (iCoeEnv, iFieldId ) );
    iLabel->SetLabelAlignment (CESMRLayoutManager::IsMirrored ( ) ? ELayoutAlignRight
                                                                  : ELayoutAlignLeft );
    AknLayoutUtils::OverrideControlColorL ( *iLabel, 
                                            EColorLabelText,
                                            iLayout->GeneralListAreaTextColor()  );
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    if (aEntry.IsStoredL ( ) ) // entry is already stored in db  
        {
        MESMRCalEntry::TESMRAlarmType alarmType;
        TTime alarmTime;
        aEntry.GetAlarmL (alarmType, alarmTime );

        switch (alarmType )
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
        switch (aEntry.Type ( ) )
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
    ResetFieldL ( );
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRBooleanField::OfferKeyEventL(const TKeyEvent& aEvent,
        TEventCode aType )
    {
    FUNC_LOG;
    TKeyResponse response( EKeyWasNotConsumed);
    if ( aType == EEventKey )
        {
        switch ( aEvent.iScanCode )
            {
            // Flowthrough, doesn't matter which key was pressed since
            // the value is always changes from "off" to "on" or vice versus.
            case EStdKeyDevice3: // selection key
            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
                iStatus = !iStatus; // change status
                ResetFieldL ( );
                response = EKeyWasConsumed;
                break;

            default:
                break;
            }
        }
    return response;
    }


// ---------------------------------------------------------------------------
// CESMRBooleanField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    if (aFocus) //Focus is gained on the field
        {
        if (iStatus)
            {
            ChangeMiddleSoftKeyL(EESMRCmdAlarmOff,R_QTN_MEET_REQ_ALARM_OFF); //R_TEXT_SOFTKEY_OFF
            }
        else
            {
            ChangeMiddleSoftKeyL(EESMRCmdAlarmOn,R_QTN_MEET_REQ_ALARM_ON); //R_TEXT_SOFTKEY_ON
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::MakeVisible
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::MakeVisible(TBool aVisible )
    {
    FUNC_LOG;
    CCoeControl::MakeVisible (aVisible );
    if ( iObserver )
        {
        TRAP_IGNORE(ResetFieldL ( ) );
        }
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::ExecuteGenericCommandL()
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if( aCommand == EESMRCmdAlarmOn || aCommand == EESMRCmdAlarmOff )
        {    
        SwitchMiddleSoftKeyL();
        }
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::SwitchMiddleSoftKeyL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::SwitchMiddleSoftKeyL()
    {
    FUNC_LOG;
    iStatus = !iStatus; // change status
    ResetFieldL ( );
    if (iStatus)
        {
        ChangeMiddleSoftKeyL(EESMRCmdAlarmOff,R_QTN_MEET_REQ_ALARM_OFF); 
        }
    else
        {
        ChangeMiddleSoftKeyL(EESMRCmdAlarmOn,R_QTN_MEET_REQ_ALARM_ON); 
        }
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::CESMRBooleanField
// ---------------------------------------------------------------------------
//
CESMRBooleanField::CESMRBooleanField( MESMRFieldValidator* aValidator ) :
    iValidator( aValidator)
    {
    FUNC_LOG;
    iStatus = ETrue;
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::ConstructL( )
    {
    FUNC_LOG;
    _LIT(KEmptyText, "" );
    SetFieldId ( EESMRFieldAlarmOnOff );
    iLabel = new (ELeave) CEikLabel();
    iLabel->SetTextL (KEmptyText );

    CESMRIconField::ConstructL (KAknsIIDQgnMeetReqIndiAlarm, iLabel );
    }

// ---------------------------------------------------------------------------
// CESMRBooleanField::ResetFieldL
// ---------------------------------------------------------------------------
//
void CESMRBooleanField::ResetFieldL( )
    {
    FUNC_LOG;
    iValidator->SetAlarmOnOffL( iStatus );

    HBufC* buf = StringLoader::LoadLC ( iStatus ?
    R_QTN_MEET_REQ_ALARM_ON :
    R_QTN_MEET_REQ_ALARM_OFF );

    iLabel->SetTextL ( *buf );
    CleanupStack::PopAndDestroy ( buf );

    // if status on 'on', add time and date fields for
    // all day event alert, otherwise remove them.
    // if this field is not visible -> alldayevent not set
    // let's not show the date and time fields either.
    if ( iStatus && IsVisible ( ) )
        {
        iObserver->InsertControl ( EESMRFieldAlarmTime );
        iObserver->InsertControl ( EESMRFieldAlarmDate );
        }
    else
        {
        iObserver->RemoveControl ( EESMRFieldAlarmTime );
        iObserver->RemoveControl ( EESMRFieldAlarmDate );
        }

    iLabel->DrawDeferred ( );
    }

// EOF

