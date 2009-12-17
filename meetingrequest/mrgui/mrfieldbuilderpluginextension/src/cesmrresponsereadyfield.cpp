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
* Description: This file implements class CESMRResponseReadyField.
*
*/


#include "emailtrace.h"
#include "cesmrresponsereadyfield.h"
#include "mesmrmeetingrequestentry.h"
#include "mesmrresponseobserver.h"
#include <esmrgui.rsg>

#include <eiklabel.h>
#include <caluser.h>
#include <calalarm.h>
#include <avkon.rsg>
#include <calentry.h>
#include <eikenv.h>
#include <eikedwin.h>
#include <StringLoader.h>
#include <AknUtils.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::CESMRResponseReadyField
// ---------------------------------------------------------------------------
//
CESMRResponseReadyField::CESMRResponseReadyField(
        MESMRResponseObserver* aResponseObserver )
:   iResponseObserver(aResponseObserver)
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::~CESMRResponseReadyField
// ---------------------------------------------------------------------------
//
CESMRResponseReadyField::~CESMRResponseReadyField()
    {
    FUNC_LOG;
    //do nothing
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::CESMRResponseReadyField
// ---------------------------------------------------------------------------
//
CESMRResponseReadyField* CESMRResponseReadyField::NewL(
        MESMRResponseObserver* aResponseObserver )
    {
    FUNC_LOG;
    CESMRResponseReadyField* self =
            new (ELeave) CESMRResponseReadyField( aResponseObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRResponseReadyField::ConstructL()
    {
    FUNC_LOG;
    iLabel = new (ELeave) CEikLabel;
    iLabel->SetTextL( KNullDesC );
    SetFieldId( EESMRFieldResponseReadyArea );
    CESMRField::ConstructL( iLabel, EESMRHighlightFocus );
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::InitializeL
// ---------------------------------------------------------------------------
//
void CESMRResponseReadyField::InitializeL()
    {
    FUNC_LOG;
    iLabel->SetFont( iLayout->Font( iCoeEnv, iFieldId ));
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::InternalizeL
// ---------------------------------------------------------------------------
//
void CESMRResponseReadyField::InternalizeL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    MESMRMeetingRequestEntry* entry = NULL;
    if ( MESMRCalEntry::EESMRCalEntryMeetingRequest == aEntry.Type() )
        {
        entry = static_cast<MESMRMeetingRequestEntry*>(&aEntry);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    TInt attendeeInfo = entry->EntryAttendeeInfoL();
    HBufC* stringholder = NULL;
    switch( attendeeInfo )
        {
        case MESMRMeetingRequestEntry::EESMREntryInfoOutOfDate:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPONSE_OUT_OF_DATE,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoCancelled:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPONSE_CANCELLED,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoAccepted:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPOND_ACCEPTED,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoTentativelyAccepted:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPOND_TENTATIVE,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoDeclined:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPOND_DECLINED,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoOccursInPast:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPONSE_OCCURS_PAST,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoConflicts:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPONSE_CONFLICT,
                            iEikonEnv );
            }
            break;
        case MESMRMeetingRequestEntry::EESMREntryInfoNormal:
            {
            stringholder = StringLoader::LoadLC(
                            R_QTN_MEET_REQ_RESPONSE_PLEASE_RESPOND,
                            iEikonEnv );
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }

    iLabel->SetTextL( *stringholder );
    CleanupStack::PopAndDestroy( stringholder );
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CESMRResponseReadyField::OfferKeyEventL(
        const TKeyEvent& aEvent,
        TEventCode aType)
    {
    FUNC_LOG;
    TKeyResponse response = EKeyWasNotConsumed;
    
    if( aType == EEventKey )
        {
        if( aEvent.iScanCode == EStdKeyDevice3 )
            {
            iResponseObserver->ChangeReadyResponseL();
            response = EKeyWasConsumed;
            }
        }
    return response;
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRResponseReadyField::SetOutlineFocusL( TBool aFocus )
    {
    FUNC_LOG;
    CESMRField::SetOutlineFocusL( aFocus );

    TRgb fontColor( 0, 0, 0 );
    if ( aFocus )
        {
        SetMiddleSoftKeyVisible( ETrue );
        fontColor = iLayout->ViewerListAreaHighlightedTextColor();
        }
    else
        {
        fontColor = iLayout->ViewerListAreaTextColor();
        }

    AknLayoutUtils::OverrideControlColorL( *iLabel,
                                      EColorLabelText,
                                           fontColor );
    }

// ---------------------------------------------------------------------------
// CESMRResponseReadyField::SetOutlineFocusL
// ---------------------------------------------------------------------------
//
void CESMRResponseReadyField::ExecuteGenericCommandL( TInt aCommand )
    {
    FUNC_LOG;
    if ( aCommand == EAknSoftkeySelect )
        {
        iResponseObserver->ChangeReadyResponseL();
        }
    }

