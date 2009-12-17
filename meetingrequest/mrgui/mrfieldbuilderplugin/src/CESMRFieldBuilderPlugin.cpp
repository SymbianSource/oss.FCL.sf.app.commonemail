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
* Description:  MRUI field build plugin implementation
*
*/


#include "emailtrace.h"
#include "CESMRFieldBuilderPlugin.h"

//<cmail>
#include "esmrdef.h"
//</cmail>
#include "mesmrresponseobserver.h"

// Editor fields
#include "cesmrsubjectfield.h"
#include "cesmrtimefield.h"
#include "cesmrdatefield.h"
#include "cesmrlocationfield.h"
#include "cesmralarmfield.h"
#include "cesmrrecurencefield.h"
#include "cesmrdescriptionfield.h"
#include "cesmrcheckbox.h"
#include "cesmrsingletimefield.h"
#include "cesmrbooleanfield.h"
#include "cesmrsyncfield.h"
#include "cesmrpriorityfield.h"

// Viewer fields
#include "cesmrviewerlabelfield.h"
#include "cesmrviewerlocationfield.h"
#include "cesmrviewerstartdatefield.h"
#include "cesmrviewertimefield.h"
#include "cesmrvieweralarmfield.h"
#include "cesmrviewerdescriptionfield.h"
#include "cesmrviewerstopdatefield.h"
#include "cesmrviewerrecurrencefield.h"
#include "cesmrviewerrecurrencedatefield.h"
#include "cesmrviewersyncfield.h"
#include "cesmrviewerpriorityfield.h"
#include "cesmrvieweralldayeventfield.h"
#include "cesmrvieweralarmdatefield.h"
#include "cesmrvieweralarmtimefield.h"
#include "cesmrviewerdetailedsubjectfield.h"
#include "cesmrrepeatuntil.h"

// <cmail> Removed profiling.</cmail>

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

#ifdef _DEBUG

// Panic literal
_LIT( KESMRFieldBuilderPlugin, "ESMRFieldBuilderPlugin" );

/** Panic codes */
enum TESMRFieldBuilderPluginPanic
    {
    EInvalidFieldId = 0 // Invalid field id
    };

void Panic( TESMRFieldBuilderPluginPanic aPanic )
    {
    User::Panic( KESMRFieldBuilderPlugin, aPanic );
    }

#endif // _DEBUG

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::CESMRFieldBuilderPlugin
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPlugin::CESMRFieldBuilderPlugin()
    :iExtension(NULL),
    iFieldExtension(NULL)
    {
    FUNC_LOG;
    //do nothing
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::~CESMRFieldBuilderPlugin
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPlugin::~CESMRFieldBuilderPlugin()
    {
    FUNC_LOG;
    delete iExtension;
    }

// -----------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::NewL
// -----------------------------------------------------------------------------
//
CESMRFieldBuilderPlugin* CESMRFieldBuilderPlugin::NewL()
    {
    FUNC_LOG;
    CESMRFieldBuilderPlugin* self = new (ELeave) CESMRFieldBuilderPlugin;
    return self;
    }

// ----------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::ExtensionL
// ----------------------------------------------------------------------------
//
TAny* CESMRFieldBuilderPlugin::ExtensionL( TUid aExtensionUid )
    {
    FUNC_LOG;
    if(!iExtension)
        {
        LoadExtensionL();
        }

    TAny* extension = NULL;
    if ( iExtension )
        {
        extension = iExtension->ExtensionL( aExtensionUid );
        }
    return extension;
    }

// ---------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::FieldBuilderL()
// ---------------------------------------------------------------------------
//
MESMRFieldBuilder* CESMRFieldBuilderPlugin::FieldBuilderL()
    {
    FUNC_LOG;
    return this;
    }

// ---------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::CreateEditorFieldL()
// ---------------------------------------------------------------------------
//
CESMRField* CESMRFieldBuilderPlugin::CreateEditorFieldL(
        MESMRFieldValidator* aValidator,
        TESMREntryField aField )
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    switch ( aField.iFieldId )
        {
        case EESMRFieldMeetingTime:
            field = CESMRTimeField::NewL( aValidator );
            break;
        case EESMRFieldStartDate:
            field = CESMRDateField::NewL( aValidator, aField.iFieldId );
            break;
        case EESMRFieldStopDate:
            field = CESMRDateField::NewL( aValidator, aField.iFieldId );
            break;
        case EESMRFieldLocation:
            field = CESMRLocationField::NewL();
            break;
        case EESMRFieldAlarm:
            field = CESMRAlarmField::NewL( aValidator );
            break;
        case EESMRFieldAllDayEvent:
            field = CESMRCheckBox::NewL( aValidator );
            break;
        case EESMRFieldAlarmOnOff:
            field = CESMRBooleanField::NewL( aValidator );
            break;
        case EESMRFieldAlarmTime:
            field = CESMRSingleTimeField::NewL( aValidator );
            break;
        case EESMRFieldAlarmDate:
            field = CESMRDateField::NewL( aValidator, aField.iFieldId );
            break;
        case EESMRFieldSubject:
            field = CESMRSubjectField::NewL();
            break;
        case EESMRFieldDescription:
            field = CESMRDescriptionField::NewL();
            break;
        case EESMRFieldRecurrence:
            field = CESMRRecurenceField::NewL( aValidator );
            break;
        case EESMRFieldRecurrenceDate:
            field = CESMRRepeatUntilField::NewL( aValidator );
            break;
        case EESMRFieldSync:
            field = CESMRSyncField::NewL();
            break;
        case EESMRFieldOccasion:
            field = CESMRSubjectField::NewL( CESMRSubjectField::ETypeOccasion );
            break;
        case EESMRFieldPriority:
            field = CESMRPriorityField::NewL();
            break;
        default:
            {
            field = FieldBuilderExtensionL()->CreateEditorFieldL(
                    aValidator,
                    aField );
            }
            break;
        }

    __ASSERT_DEBUG( field, Panic(EInvalidFieldId) );
    return field;
    }

// ---------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::CreateViewerFieldL()
// ---------------------------------------------------------------------------
//
CESMRField* CESMRFieldBuilderPlugin::CreateViewerFieldL( MESMRResponseObserver* aResponseObserver,
                                                   TESMREntryField aField,
                                                   TBool aResponseReady )
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    switch ( aField.iFieldId )
        {
        case EESMRFieldMeetingTime:
            {
            field = CESMRViewerTimeField::NewL();
            break;
            }
        case EESMRFieldStartDate:
            {
            field = CESMRViewerStartDateField::NewL();
            break;
            }
        case EESMRFieldStopDate:
            {
            field = CESMRViewerStopDateField::NewL();
            break;
            }
        case EESMRFieldLocation:
            {
            field = CESMRViewerLocationField::NewL();
            break;
            }
        case EESMRFieldAlarm:
            {
            field = CESMRViewerAlarmField::NewL();
            break;
            }
        case EESMRFieldDescription:
            {
            field = CESMRViewerDescriptionField::NewL();
            break;
            }
        case EESMRFieldRecurrence:
            {
            field = CESMRViewerRecurrenceField::NewL();
            break;
            }
        case EESMRFieldRecurrenceDate:
            {
            field = CESMRViewerRecurrenceDateField::NewL();
            break;
            }
        case EESMRFieldSync:
            {
            field = CESMRViewerSyncField::NewL();
            break;
            }
        case EESMRFieldPriority:
            {
            field = CESMRViewerPriorityField::NewL();
            break;
            }
        case EESMRFieldAllDayEvent:
            {
            field = CESMRViewerAllDayEventField::NewL();
            break;
            }
        case EESMRFieldAlarmDate:
            {
            field = CESMRViewerAlarmDateField::NewL();
            break;
            }
        case EESMRFieldAlarmTime:
            {
            field = CESMRViewerAlarmTimeField::NewL();
            break;
            }
        case EESMRFieldDetailedSubject:
            {
            field = CESMRViewerDetailedSubjectField::NewL();
            break;
            }
        default:
            {
            MESMRFieldBuilder* extension = FieldBuilderExtensionL();
            if ( extension )
                {
                field = extension->CreateViewerFieldL(
                        aResponseObserver,
                        aField,
                        aResponseReady );
                }
            }
            break;
        }

    __ASSERT_DEBUG( field, Panic(EInvalidFieldId) );
    // Set field mode
    field->SetFieldMode( EESMRFieldModeView );
    
    return field;
    }

// ---------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::FieldBuilderExtensionL()
// ---------------------------------------------------------------------------
//
MESMRFieldBuilder* CESMRFieldBuilderPlugin::FieldBuilderExtensionL()
    {
    FUNC_LOG;
    if(!iFieldExtension)
        {
        LoadExtensionL();
        }

    return iFieldExtension;
    }

// ---------------------------------------------------------------------------
// CESMRFieldBuilderPlugin::LoadExtensionL()
// ---------------------------------------------------------------------------
//
void CESMRFieldBuilderPlugin::LoadExtensionL()
    {
    FUNC_LOG;
    CESMRFieldBuilderInterface* extension = NULL;

    TRAPD(error,
            extension = CESMRFieldBuilderInterface::CreatePluginL(
                    TUid::Uid(KESMRUIFieldBuilderPluginExtensionImplUId) ) );

    if ( error == KErrNone && extension )
        {
        delete iExtension;
        iExtension = extension;
        iFieldExtension = iExtension->FieldBuilderL();
        }

    }
//EOF

