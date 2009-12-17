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
*  Description : Storage for viewer fields
*  Version     : %version: tr1sido#6 %
*
*/

#include "emailtrace.h"
#include "cesmrviewerfieldstorage.h"

//<cmail>
#include "cesmrpolicy.h"
//</cmail>

#include "cesmrfield.h"

// <cmail> Removed profiling. </cmail>

/// Unnamed namespace for local definitions
namespace { // codescanner::namespace

const TInt KRequiredLabel = 0;
const TInt KStatusField = 1;
const TInt KOptionalLabel = 2;

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::CESMRViewerFieldStorage
// ---------------------------------------------------------------------------
//
CESMRViewerFieldStorage::CESMRViewerFieldStorage(
        MESMRFieldEventObserver& aEventObserver )
    : CESMRFieldStorage( aEventObserver )
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::~CESMRViewerFieldStorage
// ---------------------------------------------------------------------------
//
CESMRViewerFieldStorage::~CESMRViewerFieldStorage()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::NewL
// ---------------------------------------------------------------------------
//
CESMRViewerFieldStorage* CESMRViewerFieldStorage::NewL(
        CESMRPolicy* aPolicy,
        MESMRResponseObserver* aResponseObserver,
        TBool aResponseReady,
        MESMRFieldEventObserver& aEventObserver,
        TInt aReqAttendee,
        TInt aOptAttendee )
    {
    FUNC_LOG;
    CESMRViewerFieldStorage* self =
            new (ELeave) CESMRViewerFieldStorage( aEventObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aPolicy, aResponseObserver, 
					  aResponseReady , aReqAttendee, 
					  aOptAttendee);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRViewerFieldStorage::ConstructL(
        CESMRPolicy* aPolicy,
        MESMRResponseObserver* aResponseObserver,
        TBool aResponseReady,
        const TInt &aReqAttendee,
        const TInt &aOptAttendee
        )
    {
    FUNC_LOG;
    CESMRFieldStorage::BaseConstructL();
    
    if ( (aReqAttendee == 0) && (aOptAttendee ==0) )
        {
        CreatePolicyFieldsL (aPolicy,aResponseObserver, aResponseReady);
        }
    else// Total field count is given Tracking dialog
        {
        CreateTrackingFieldsL( aPolicy,
							   aResponseObserver, 
							   aResponseReady,
							   aReqAttendee,
							   aOptAttendee);
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::ExternalizeL
// ---------------------------------------------------------------------------
//
void CESMRViewerFieldStorage::ExternalizeL(
        MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    CESMRFieldStorage::ExternalizeL( aEntry );
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::CreatePolicyFieldsL
// ---------------------------------------------------------------------------
//
void CESMRViewerFieldStorage::CreatePolicyFieldsL(CESMRPolicy* aPolicy,
        MESMRResponseObserver* aResponseObserver,
        TBool aResponseReady)
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    
    RArray<TESMREntryField> fields = aPolicy->Fields();
    const TInt count = fields.Count();
    for ( TInt i(0); i < count ; i++ )
        {
        field = CreateViewerFieldL( 
        		aResponseObserver, fields[i], aResponseReady );

        TBool visible( ETrue );
            
        switch ( fields[i].iFieldId )
            {
            case EESMRFieldResponseArea:
                {
                if ( aResponseReady )
                    {
                    visible = EFalse;
                    }
                break;
                }
            case EESMRFieldResponseReadyArea:
                {
                if ( !aResponseReady )
                    {
                    visible = EFalse;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        
        CleanupStack::PushL( field );
        AddFieldL( field, visible );
        CleanupStack::Pop( field );
        }
    }

// ---------------------------------------------------------------------------
// CESMRViewerFieldStorage::CreateTrackingFieldsL
// ---------------------------------------------------------------------------
//
void CESMRViewerFieldStorage::CreateTrackingFieldsL(CESMRPolicy* aPolicy,
        MESMRResponseObserver* aResponseObserver,
        TBool aResponseReady,
        const TInt &aReqAttendee,
        const TInt &aOptAttendee)
    {
    FUNC_LOG;
    CESMRField* field = NULL;
    RArray<TESMREntryField> fields = aPolicy->Fields();

    if (aReqAttendee > 0)
        {
        //create "required attendee" label
        field = CreateViewerFieldL( 
					aResponseObserver, 
					fields[KRequiredLabel], 
					aResponseReady);
        CleanupStack::PushL( field );
        AddFieldL( field );
        CleanupStack::Pop( field );

        //create attendee status
        for ( TInt j(0); j < aReqAttendee ; j++ )
             {
             field = CreateViewerFieldL( 
						 aResponseObserver, 
						 fields[KStatusField], 
						 aResponseReady);
             CleanupStack::PushL( field );
             AddFieldL( field );
             CleanupStack::Pop( field );
             }
        }

    if (aOptAttendee > 0)
        {
        //create "optional attendee" label
        field = CreateViewerFieldL( 
					aResponseObserver, 
					fields[KOptionalLabel], 
					aResponseReady);
        CleanupStack::PushL( field );
        AddFieldL( field );
        CleanupStack::Pop( field );

        //create attendee status
        for ( TInt j(0); j < aOptAttendee ; j++ )
             {
             field = CreateViewerFieldL( 
						 aResponseObserver, 
						 fields[KStatusField], 
						 aResponseReady);
             CleanupStack::PushL( field );
             AddFieldL( field );
             CleanupStack::Pop( field );
             }
        }

    //this dummy field is created as a fallback because 
    //empty screen with no fields will crash with any key input.
    if(!aOptAttendee && !aReqAttendee )
        {
        field = CreateViewerFieldL( 
					aResponseObserver, 
					fields[KRequiredLabel], 
					aResponseReady);
        CleanupStack::PushL( field );
        AddFieldL( field );
        CleanupStack::Pop( field );
        }
    }


// EOF

