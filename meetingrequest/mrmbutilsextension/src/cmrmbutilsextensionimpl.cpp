/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MBUtils ECOM implementation
*
*/

//INCLUDE FILES
#include "cmrmbutilsextensionimpl.h"
#include "cmrmbutilsmailboxlistener.h"
#include "cmrmbutilssettings.h"
#include "CFSMailClient.h"

#include "emailtrace.h"

#include <ct/rcpointerarray.h>
#include <cmrmailboxutils.h>

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::CMRMbUtilsExtensionImpl
//
// Constructor.
// ----------------------------------------------------------------------------
//
CMRMbUtilsExtensionImpl::CMRMbUtilsExtensionImpl(
        MMRMailboxUtilsObserver& aObserver )
:   iMRMailboxObserver( aObserver )
	{
	// No implementation
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::~CMRMbUtilsExtensionImpl
//
// Destructor.
// ----------------------------------------------------------------------------
// 
CMRMbUtilsExtensionImpl::~CMRMbUtilsExtensionImpl()
	{
	FUNC_LOG;
	
	iMailboxes.ResetAndDestroy();
	
	if ( iMailClient )
	    {
        iMailClient->RemoveObserver( *iMailboxListener );
        iMailClient->Close();
	    }
	
	delete iSettings;
	delete iMailboxListener;
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::NewL
// ----------------------------------------------------------------------------
//
CMRMbUtilsExtensionImpl* CMRMbUtilsExtensionImpl::NewL(
        TAny* aMRMailboxObserver )
	{
	FUNC_LOG;
	
	MMRMailboxUtilsObserver* mrMailboxObserver = 
            static_cast<MMRMailboxUtilsObserver*>( aMRMailboxObserver );
	
	CMRMbUtilsExtensionImpl* self = 
            new (ELeave) CMRMbUtilsExtensionImpl( *mrMailboxObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::ConstructL
// ----------------------------------------------------------------------------
//
void CMRMbUtilsExtensionImpl::ConstructL()
	{
	FUNC_LOG;
	
	iMailClient = CFSMailClient::NewL();

	iMailboxListener = 
            CMRMBUtilsMailboxListener::NewL( 
                    iMailboxes,
                    *iMailClient,
                    iMRMailboxObserver );	
	
	iSettings = CMRMBUtilsSettings::NewL( *iMailClient );
	
	iMailClient->AddObserverL( *iMailboxListener );	
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::SettingsMgr
// ----------------------------------------------------------------------------
//
MMRMBUtilsSettings* CMRMbUtilsExtensionImpl::SettingsMgr()
    {
    return iSettings;
    }

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::MRMailBoxes
// ----------------------------------------------------------------------------
//
const RPointerArray<CMRMailBoxInfo>& 
		CMRMbUtilsExtensionImpl::MRMailBoxes() const
	{
	FUNC_LOG;
	
	return iMailboxes;
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::NewL
// ----------------------------------------------------------------------------
//
const CMRMailBoxInfo* CMRMbUtilsExtensionImpl::MRMailBoxInfoL( 
		TInt aEntryId ) const
	{
	FUNC_LOG;
	
	const CMRMailBoxInfo* KInfo = NULL;

    TInt mailboxCount( iMailboxes.Count() );    
    for ( TInt i(0); i < mailboxCount && !KInfo; ++i )
        {
        if ( aEntryId == iMailboxes[i]->EntryId() )
            {
            KInfo = iMailboxes[i];
            }
        }
    return KInfo;
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::ListMRMailBoxesL
// ----------------------------------------------------------------------------
//
void CMRMbUtilsExtensionImpl::ListMRMailBoxesL(
		RArray<CMRMailboxUtils::TMailboxInfo>& aMailBoxes )
	{
	FUNC_LOG;
	
	TInt mailboxCount( iMailboxes.Count() );
	for ( TInt i(0); i < mailboxCount; ++i )
		{
		CMRMailboxUtils::TMailboxInfo info;
		info.iName.Set( iMailboxes[i]->Name() );
		info.iEmailAddress.Set( iMailboxes[i]->Address() );
		info.iMtmUid = iMailboxes[i]->MtmUid();
		info.iEntryId = iMailboxes[i]->EntryId();
		
		aMailBoxes.Append( info );
		}
	}

// ----------------------------------------------------------------------------
// CMRMbUtilsExtensionImpl::ExtensionL
// ----------------------------------------------------------------------------
//
TAny* CMRMbUtilsExtensionImpl::ExtensionL( TUid /*aExtensionUid*/ )
    {
    return NULL;
    }

// EOF
