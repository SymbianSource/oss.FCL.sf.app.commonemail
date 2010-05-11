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
* Description:  MRUI field build plugin extension implementation
*
*/

#include "CESMRFieldBuilderPluginExtension.h"

//<cmail>
#include "esmrdef.h"
//</cmail>
#include "tesmrentryfield.h"
#include "mesmrresponseobserver.h"
#include "cesmrattendeefield.h"
#include "cesmrtrackingfieldlabel.h"
#include "cesmrtrackstatus.h"
#include "cesmrviewerattendeesfield.h"
#include "cesmrresponsefield.h"
#include "cesmrviewerfromfield.h"
#include "cesmrfieldbuilderinterface.h"
#include "cfsmailcommon.h"

#include <CalenInterimUtils2.h>

// DEBUG
#include "emailtrace.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::CESMRFieldBuilderPluginExtension
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPluginExtension::CESMRFieldBuilderPluginExtension() :
    iESMRStaticAccessed(EFalse)
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::~CESMRFieldBuilderPluginExtension
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPluginExtension::~CESMRFieldBuilderPluginExtension()
    {
    FUNC_LOG;
    if(iESMRStaticAccessed) 
        {
        iESMRStatic.Close();
        }
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::ExtensionL
// ----------------------------------------------------------------------------
//
TAny* CESMRFieldBuilderPluginExtension::ExtensionL( TUid /*aExtensionUid*/ )
    {
    FUNC_LOG;
    MESMRBuilderExtension* capa = static_cast<MESMRBuilderExtension*>( this );
    return capa;
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::CFSMailBoxCapabilityL
// ----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::CFSMailBoxCapabilityL(
        TMRCFSMailBoxCapability aCapa )
    {
    FUNC_LOG;
    
    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;

    return HasCapability( iESMRStatic.DefaultFSMailBoxL(), aCapa );
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::CFSMailBoxCapabilityL
// ----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::CFSMailBoxCapabilityL(
            const TDesC& aEmailAddress,
            TMRCFSMailBoxCapability aCapa )
    {
    FUNC_LOG;
        
    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;

    CFSMailBox* mailBox = iESMRStatic.MailBoxL( aEmailAddress );
    TBool result = HasCapability( *mailBox, aCapa );
    delete mailBox;
    
    return result;
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::MRCanBeOriginateedL
// ----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::MRCanBeOriginateedL()
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;

    CCalenInterimUtils2* calUtils2 = CCalenInterimUtils2::NewL();
    CleanupStack::PushL( calUtils2 );
    if ( calUtils2->MRViewersEnabledL() )
        {
        TRAPD( err, iESMRStatic.DefaultFSMailBoxL() );

        if ( KErrNone == err )
            {
            // If can fetch the default FS mailbox -->
            // We are able to originate MR as well.
            retValue = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( calUtils2 );
    calUtils2 = NULL;

    return retValue;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::NewL
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPluginExtension* CESMRFieldBuilderPluginExtension::NewL()
    {
    FUNC_LOG;
    CESMRFieldBuilderPluginExtension* self =
            new (ELeave) CESMRFieldBuilderPluginExtension;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CESMRFieldBuilderPluginExtension::ConstructL()
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::FieldBuilderL
// -----------------------------------------------------------------------------
//
MESMRFieldBuilder* CESMRFieldBuilderPluginExtension::FieldBuilderL()
    {
    FUNC_LOG;
    return this;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::CreateEditorFieldL
// -----------------------------------------------------------------------------
//
CESMRField* CESMRFieldBuilderPluginExtension::CreateEditorFieldL(
        MESMRFieldValidator* /*aValidator*/,
        TESMREntryField aField )
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    switch ( aField.iFieldId )
        {
        case EESMRFieldAttendee:
            {
            field = CESMRAttendeeField::NewL( CCalAttendee::EReqParticipant );
            break;
            }
        case EESMRFieldOptAttendee:
            {
            field = CESMRAttendeeField::NewL( CCalAttendee::EOptParticipant );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    // Set field mode
    field->SetFieldMode( EESMRFieldModeEdit );

    return field;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::CreateViewerFieldL
// -----------------------------------------------------------------------------
//
CESMRField* CESMRFieldBuilderPluginExtension::CreateViewerFieldL(
        MESMRResponseObserver* aResponseObserver,
        TESMREntryField aField,
        TBool /*aResponseReady*/ )
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    switch ( aField.iFieldId )
        {
        case EESMRFieldAttendee:
            {
            field = CESMRViewerAttendeesField::NewL( CCalAttendee::EReqParticipant );
            break;
            }
        case EESMRFieldOptAttendee:
            {
            field = CESMRViewerAttendeesField::NewL( CCalAttendee::EOptParticipant );
            break;
            }
        case EESMRFieldResponseArea:
            {
            field = CESMRResponseField::NewL( aResponseObserver );
            break;
            }
        case EESMRFieldOrganizer:
            {
            field = CESMRViewerFromField::NewL();
            break;
            }
        case EESMRFieldReqTrack:
            {
            field = CESMRTrackingFieldLabel::NewL(CCalAttendee::EReqParticipant );
            break;
            }
        case EESMRFieldOptTrack:
            {
            field = CESMRTrackingFieldLabel::NewL(CCalAttendee::EOptParticipant );
            break;
            }
        case EESMRTrackStatus:
            {
            field = CESMRTrackStatus::NewL();
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    // Set field mode
    field->SetFieldMode( EESMRFieldModeView );

    return field;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::HasCapability
// -----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::HasCapability(
        const CFSMailBox& aMailBox,
        MESMRBuilderExtension::TMRCFSMailBoxCapability aCapability ) const
    {
    TBool response(EFalse);

    switch ( aCapability )
        {
        case EMRCFSAttendeeStatus:
            {
            response = aMailBox.HasCapability(
                    EFSMBoxCapaMeetingRequestAttendeeStatus );
            break;
            }
            
        case EMRCFSRemoveFromCalendar:
            {
            response = aMailBox.HasCapability( EFSMBoxCapaRemoveFromCalendar );
            break;
            }
            
        case EMRCFSSupportsAttachmentsInMR:
            {
            response = aMailBox.HasCapability( EFSMboxCapaSupportsAttahmentsInMR );
            }
            break;
            
        default:
            {
            break;
            }
        }

    return response;
    }

//EOF

