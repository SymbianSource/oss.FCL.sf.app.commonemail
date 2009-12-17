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
#include "cesmrviewerattachmentsfield.h"
#include "cesmrviewerattendeesfield.h"
#include "cesmrresponsefield.h"
#include "cesmrresponsereadyfield.h"
#include "cesmrviewerfromfield.h"

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
// CESMRFieldBuilderPluginExtension::ExtensionL
// ----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::CFSMailBoxCapabilityL(
        EMRCFSMailBoxCapability aCapa )
    {
    FUNC_LOG;
    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;

    TBool response(EFalse);

    if (aCapa == EMRCFSAttendeeStatus)
        {
        response = iESMRStatic.DefaultFSMailBoxL().HasCapability(
                EFSMBoxCapaMeetingRequestAttendeeStatus );
        }

    return response;
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPluginExtension::MRCanBeOriginateedL
// ----------------------------------------------------------------------------
//
TBool CESMRFieldBuilderPluginExtension::MRCanBeOriginateedL(
        TBool aForceResetDefaultMRMailbox )
    {
    FUNC_LOG;
    TBool retValue( EFalse );

    iESMRStatic.ConnectL();
    iESMRStaticAccessed = ETrue;

    CCalenInterimUtils2* calUtils2 = CCalenInterimUtils2::NewL();
    CleanupStack::PushL( calUtils2 );
    if ( calUtils2->MRViewersEnabledL() )
        {
        TRAPD( err, 
               iESMRStatic.DefaultFSMailBoxL( aForceResetDefaultMRMailbox ) );

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
        case EESMRFieldAttachments:
            {
            field = CESMRViewerAttachmentsField::NewL();
            }
            break;
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
        case EESMRFieldResponseReadyArea:
            {
            field = CESMRResponseReadyField::NewL( aResponseObserver );
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
//EOF

