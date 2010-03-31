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
* Description:  ESMR UI Factory implementation
*
*/

#include "cesmrfieldstorage.h"
#include "cesmrfield.h"
#include "mesmrcalentry.h"
#include "cesmrfieldeventqueue.h"
#include "cmrsystemnotifier.h"

#include <touchfeedback.h>

#include "emailtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFieldStorage::CESMRFieldStorage()
// ---------------------------------------------------------------------------
//
CESMRFieldStorage::CESMRFieldStorage(
		MESMRFieldEventObserver& aEventObserver ):
	iEventObserver( aEventObserver )
    {
    FUNC_LOG;
    // Do nothing
    }

void CESMRFieldStorage::BaseConstructL()
    {
    FUNC_LOG;
    // Add event observer to event queue
    EventQueueL().AddObserverL( &iEventObserver );    
    iSystemNotifier = CMRSystemNotifier::NewL( EventQueueL() );
    iSystemNotifier->StartL();
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::~CESMRFieldStorage()
// ---------------------------------------------------------------------------
//
CESMRFieldStorage::~CESMRFieldStorage()
    {
    FUNC_LOG;
    iArray.ResetAndDestroy();

    delete iPlugin;
    delete iSystemNotifier;
    delete iEventQueue;    
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::AddFieldL()
// ---------------------------------------------------------------------------
//
void CESMRFieldStorage::AddFieldL( CESMRField* aField )
    {
    FUNC_LOG;
    if ( aField )
        {
        AddFieldL( aField, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::AddFieldL()
// ---------------------------------------------------------------------------
//
void CESMRFieldStorage::AddFieldL( CESMRField* aField, TBool aVisible )
    {
    FUNC_LOG;
    if ( aField )
        {
        iArray.ReserveL( iArray.Count() + 1 );
        aField->SetEventQueueL( iEventQueue );
        aField->MakeVisible( aVisible );
        iArray.AppendL( aField );
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::Count()
// ---------------------------------------------------------------------------
//
TInt CESMRFieldStorage::Count() const
    {
    FUNC_LOG;
    return iArray.Count();
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::Field()
// ---------------------------------------------------------------------------
//
CESMRField* CESMRFieldStorage::Field( TInt aInd ) const
    {
    FUNC_LOG;
    return iArray[ aInd ];
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::FieldById
// ---------------------------------------------------------------------------
//
CESMRField* CESMRFieldStorage::FieldById( TESMREntryFieldId aId ) const
    {
    CESMRField* field = NULL;

    for ( TInt i = 0; i < iArray.Count(); ++i )
        {
        if ( iArray[ i ]->FieldId() == aId )
            {
            field = iArray[ i ];
            break;
            }
        }

    return field;
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::Validate()
// ---------------------------------------------------------------------------
//
TInt CESMRFieldStorage::Validate(
		TESMREntryFieldId& /*aId */,
		TBool /*aForceValidation*/ )
    {
    FUNC_LOG;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::InternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRFieldStorage::InternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;

    TInt index(0);
    while( index < iArray.Count() )
        {
        TInt fieldCount( iArray.Count() );
        iArray[ index ]->InternalizeL( aEntry );

        if ( fieldCount == iArray.Count() )
            {
            // Field that was internalized was not removed from list
            // We can update field index. If field is removed the next
            // field index is already the current index ...
            index++;
            }
        }
    }

// ---------------------------------------------------------------------------
// CESMRFieldStorage::ExternalizeL()
// ---------------------------------------------------------------------------
//
void CESMRFieldStorage::ExternalizeL( MESMRCalEntry& aEntry )
    {
    FUNC_LOG;
    for ( TInt i(0); i < iArray.Count(); i++ )
        {
        if(iArray[ i ]->IsVisible())
        	{
        	iArray[ i ]->ExternalizeL( aEntry );
        	}
        }
    }


// -----------------------------------------------------------------------------
// CESMRFieldStorage::FieldBuilderL()
// -----------------------------------------------------------------------------
//
MESMRFieldBuilder* CESMRFieldStorage::FieldBuilderL()
    {
    FUNC_LOG;
    if(!iFieldBuilder)
        {
        LoadPluginL();
        }
    return iFieldBuilder;
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::LoadPluginL()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::LoadPluginL()
    {
    FUNC_LOG;
    iFieldBuilder = NULL;
    CESMRFieldBuilderInterface* plugin = NULL;

	TRAPD( error, plugin =
		CESMRFieldBuilderInterface::CreatePluginL(
				TUid::Uid(KESMRUIFieldBuilderPluginImplUId) ) );

	if ( error == KErrNone && plugin)
		{
		delete iPlugin;
		iPlugin = plugin;

		iFieldBuilder = iPlugin->FieldBuilderL();
		}
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::CreateEditorFieldL()
// -----------------------------------------------------------------------------
//
CESMRField* CESMRFieldStorage::CreateEditorFieldL(
		MESMRFieldValidator* aValidator,
		TESMREntryField aField )
    {
    FUNC_LOG;
    CESMRField* field = NULL;

    if (FieldBuilderL())
        {
        field = FieldBuilderL()->CreateEditorFieldL(aValidator, aField);
        }

    return field;
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::CreateViewerFieldL()
// -----------------------------------------------------------------------------
//
CESMRField* CESMRFieldStorage::CreateViewerFieldL(
		MESMRResponseObserver* aResponseObserver,
		TESMREntryField aField,
		TBool aResponseReady )
    {
    FUNC_LOG;
    CESMRField* field = NULL;

    if (FieldBuilderL())
        {
        field = FieldBuilderL()->CreateViewerFieldL(
					aResponseObserver, aField, aResponseReady );
        }
    return field;
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::ChangePolicyL()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::ChangePolicyL(
        const CESMRPolicy& /*aNewPolicy*/,
        MESMRCalEntry& /*aEntry*/ )
    {
    ASSERT( EFalse ); // Assert for debug builds
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::EventQueueL()
// -----------------------------------------------------------------------------
//
CESMRFieldEventQueue& CESMRFieldStorage::EventQueueL()
    {
    FUNC_LOG;
    if ( !iEventQueue )
        {
        iEventQueue = CESMRFieldEventQueue::NewL();
        }
    return *iEventQueue;
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::RemoveField()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::RemoveField( TInt aInd )
    {
    CESMRField* field = iArray[ aInd ];
    iArray.Remove( aInd );
    delete field;
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::InsertFieldL()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::InsertFieldL( CESMRField* aField, TInt aIndex )
    {
    iArray.InsertL( aField, aIndex );
    aField->SetEventQueueL( iEventQueue );
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::ReserveL()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::ReserveL( TInt aCount )
    {
    iArray.ReserveL( aCount );
    }

// -----------------------------------------------------------------------------
// CESMRFieldStorage::Reset()
// -----------------------------------------------------------------------------
//
void CESMRFieldStorage::Reset()
    {
    iArray.ResetAndDestroy();
    }
// EOF

