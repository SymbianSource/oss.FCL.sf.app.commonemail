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
* Description: 
*
*/


#include "MT_mrmbutilsextension.h"
#include "cfsmailcommon.h"
#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "esmrinternaluid.h"

#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <meetingrequestuids.hrh>
#include <ct/rcpointerarray.h>
#include <ecom/ecom.h>
#include <implementationinformation.h>
#include <cmrmbutilsmbinfohandler.h>

namespace {  // codescanner::namespace

// CMail literal
_LIT8( KCMail, "CMAIL" );

// CMAIL extension ECOM version
const TInt KCMailExtensionVersion( 1 );

// Constant for equal string
const TInt KEqualString( 0 );

TInt FindMailboxByName(
		const TDesC& aName, 
		RPointerArray<CFSMailBox>& aMailboxes )
	{
	TInt index( KErrNotFound );
	
	TInt mailboxCount( aMailboxes.Count() );
	for ( TInt i(0);
	      i < mailboxCount && KErrNotFound == index;
	      ++i )
		{
		TPtrC name( aMailboxes[i]->GetName() );
		if ( KEqualString == name.CompareF( aName ) )
			{
			index = i;
			}
		}	
	
	return index;
	}

}

// - Construction -----------------------------------------------------------

MT_MRMbUtilsExtension* MT_MRMbUtilsExtension::NewL()
    {
    MT_MRMbUtilsExtension* self = MT_MRMbUtilsExtension::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_MRMbUtilsExtension* MT_MRMbUtilsExtension::NewLC()
    {
    MT_MRMbUtilsExtension* self = new( ELeave ) MT_MRMbUtilsExtension();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

MT_MRMbUtilsExtension::~MT_MRMbUtilsExtension()
    {
    }

MT_MRMbUtilsExtension::MT_MRMbUtilsExtension()
    {
    }

void MT_MRMbUtilsExtension::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

void MT_MRMbUtilsExtension::HandleMRMailboxEventL( 
        MMRMailboxUtilsObserver::TEventType /*aType*/ )
    {
    
    }

// - Test methods -----------------------------------------------------------



void MT_MRMbUtilsExtension::SetupL(  )
    {
    iMailClient = CFSMailClient::NewL();    
    iCmailHandler = CMRMBUtilsMBInfoHandler::NewL( KCMail(), *this );
    }
    
void MT_MRMbUtilsExtension::SetupWithAllMailboxesL()
	{
	SetupL();
	
	TFSMailMsgId id;
	iMailClient->ListMailBoxes( id, iMailboxes );
	}

void MT_MRMbUtilsExtension::SetupWithMRMailboxesL()
	{
	SetupWithAllMailboxesL();
	
	TInt index( 0 );
	while ( index < iMailboxes.Count() )
		{
		CFSMailBox* mailbox( iMailboxes[index] );
		
		TBool supportsMeetingRequest( 
				mailbox->HasCapability(EFSMBoxCapaMeetingRequestRespond) &&
				mailbox->HasCapability(EFSMBoxCapaMeetingRequestCreate) &&
				mailbox->HasCapability(EFSMBoxCapaCanUpdateMeetingRequest) );

		if ( !supportsMeetingRequest )
			{
			delete mailbox;
			iMailboxes.Remove( index );
			}
		else
			{
			++index;
			}
		}
	}

void MT_MRMbUtilsExtension::Teardown()
    {
    if ( iCmailHandler )
        {
        delete iCmailHandler;
        iCmailHandler = NULL;
        }
    
    iMailboxes.ResetAndDestroy();
    iMailboxes.Close();
    iMailClient->Close();    
    }

void MT_MRMbUtilsExtension::T_MRECOMExtensionFoundL(  )
    {
    RCPointerArray<CImplementationInformation> implementations;
    CleanupClosePushL( implementations );
    
    const TUid mrmailboxUtilsExt = 
			{ KMRMailboxUtilsExtensionIFUID };
    
    REComSession::ListImplementationsL( 
    		mrmailboxUtilsExt, 
    		implementations );
    
    TBool cmailExtFound( EFalse );
    TInt implCount( implementations.Count() );    
    
    EUNIT_ASSERT_DESC( 
    		implCount,
    		"No implementations found for KMRMailboxUtilsExtensionIFUID" );    
    
    for (TInt i(0); i < implCount && !cmailExtFound; ++i )
    	{
    	CImplementationInformation* implInfo = implementations[i];
    	TUid implUid( implInfo->ImplementationUid() );
    	
    	if ( KESMRMButilsExtensionImplementationUid == implUid.iUid )
    		{
    		cmailExtFound = ETrue;
    		}
    	}
    
    EUNIT_ASSERT_DESC( 
    		cmailExtFound,
    		"No CMail extension found" );    
    
    CleanupStack::PopAndDestroy(); // implementations
    }
    
void MT_MRMbUtilsExtension::T_MRCreateCMailExtensionL()
	{
	CMRMBUtilsMBInfoHandler* cmailHandler = NULL;
	
	cmailHandler = CMRMBUtilsMBInfoHandler::NewL( KCMail(), *this );
	
	EUNIT_ASSERT_DESC(
			cmailHandler,
			"CMAIL extension cannot be created ");
	
	delete cmailHandler;
	cmailHandler = NULL;
	}

void MT_MRMbUtilsExtension::T_CheckECOMDataL()
	{
    RCPointerArray<CImplementationInformation> implementations;
    CleanupClosePushL( implementations );
    
    const TUid mrmailboxUtilsExt = 
			{ KMRMailboxUtilsExtensionIFUID };
    
    REComSession::ListImplementationsL( 
    		mrmailboxUtilsExt, 
    		implementations );
    
    TBool cmailExtFound( EFalse );    
    TInt implCount( implementations.Count() );    
    
    EUNIT_ASSERT_DESC( 
    		implCount,
    		"No implementations found for KMRMailboxUtilsExtensionIFUID" );    
    
    for (TInt i(0); i < implCount && !cmailExtFound; ++i )
    	{
    	CImplementationInformation* implInfo = implementations[i];
    	TUid implUid( implInfo->ImplementationUid() );
    	
    	if ( KESMRMButilsExtensionImplementationUid == implUid.iUid )
    		{
    		cmailExtFound = ETrue;
    		
    		EUNIT_ASSERT_EQUALS_DESC(
    				KCMailExtensionVersion,
    				implInfo->Version(),
    				"CMail extension ECOM VERSION mismatch" );
			
    		EUNIT_ASSERT_EQUALS_DESC(
    				KEqualString,
    				implInfo->DataType().CompareF( KCMail),
    				"CMail extension datatype mismatch" );
    		}
    	}
    
    EUNIT_ASSERT_DESC( 
    		cmailExtFound,
    		"No CMail extension found" );    
    
    CleanupStack::PopAndDestroy(); // implementations	
	}

void MT_MRMbUtilsExtension::T_FindMRMailboxesL()
	{
	const RPointerArray<CMRMailBoxInfo>& mailboxes = 
			iCmailHandler->MRMailBoxes();
	
	EUNIT_ASSERT_EQUALS_DESC(
			iMailboxes.Count(),
			mailboxes.Count(),
			"Mailbox count mismatch" );
	}

void MT_MRMbUtilsExtension::T_FilterNonMRMailboxesL()
	{
	const RPointerArray<CMRMailBoxInfo>& mailboxes = 
			iCmailHandler->MRMailBoxes();
	
	TInt mailBoxCount( iMailboxes.Count() );
	TInt mrMailboxCount( mailboxes.Count() );
	
	EUNIT_ASSERT_DESC( 
			mailBoxCount >= mrMailboxCount,
			"Too many MR mailboxes" );
	
	// Remove MR mailboxes from all mailboxes
	for ( TInt i(0); i < mrMailboxCount; ++i )
		{
		TPtrC mrMailboxName( mailboxes[i]->Name() );
		TInt mailboxIndex( 
				FindMailboxByName(mrMailboxName, iMailboxes) );
			
		if( KErrNotFound != mailboxIndex )
			{
			// MR mailbox was found --> Remove it from list
			CFSMailBox* mailbox = iMailboxes[ mailboxIndex ];
			delete mailbox;
			iMailboxes.Remove( mailboxIndex );			
			}
		}
	
	mailBoxCount = iMailboxes.Count();	
	for ( TInt i(0); i < mailBoxCount; i++ )
		{
		CFSMailBox* mailbox = iMailboxes[i];
		
		TBool supportsMeetingRequest( 
				mailbox->HasCapability(EFSMBoxCapaMeetingRequestRespond) &&
				mailbox->HasCapability(EFSMBoxCapaMeetingRequestCreate) &&
				mailbox->HasCapability(EFSMBoxCapaCanUpdateMeetingRequest) );
				
		EUNIT_ASSERT_DESC(
				!supportsMeetingRequest,
				"MR capable mailbox was not included in MR mailbox list" );		
		}
	}

void MT_MRMbUtilsExtension::T_FetchSpecificMailboxInformationL()
	{		
	const RPointerArray<CMRMailBoxInfo>& mailboxes = 
			iCmailHandler->MRMailBoxes();
	
	TInt mailboxCount( mailboxes.Count() );
	
	EUNIT_ASSERT_DESC(
			mailboxCount,
			"No any mailboxes found" );	
	
	for ( TInt i(0); i < mailboxCount; ++i )
		{
		TInt mailboxId( mailboxes[i]->EntryId() );
		
		const CMRMailBoxInfo* mbInfo = 
				iCmailHandler->MRMailBoxInfoL( mailboxId );
		
		EUNIT_ASSERT_DESC(
				mbInfo,
				"Mailbox not found with known ID" );
		}
	}

void MT_MRMbUtilsExtension::T_FetchInvalidMailboxInformationL()
	{
	const RPointerArray<CMRMailBoxInfo>& mailboxes = 
			iCmailHandler->MRMailBoxes();
	
	TInt mailboxCount( mailboxes.Count() );
	
	EUNIT_ASSERT_DESC(
			mailboxCount,
			"No any mailboxes found" );	
	
	TInt invalidId( 0x43219862 );
	
	const CMRMailBoxInfo* mbInfo = 
			iCmailHandler->MRMailBoxInfoL( invalidId );
	
	EUNIT_ASSERT_DESC(
			!mbInfo,
			"Mailbox not found with known ID" );
	}

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
		MT_MRMbUtilsExtension,
    "Module test: mrmbutilsextension",
    "MODULE" )

EUNIT_TEST(
    "Test ECOM implementation existence",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_MRECOMExtensionFoundL, Teardown)
    
EUNIT_TEST(
    "Test CMail extension creation",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_MRCreateCMailExtensionL, Teardown)

EUNIT_TEST(
    "Test ECOM resource data",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupL, T_CheckECOMDataL, Teardown)    
    
EUNIT_TEST(
    "Fetching MR mailboxes",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithMRMailboxesL, T_FindMRMailboxesL, Teardown)    
    
EUNIT_TEST(
    "Testing mailbox filtering",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithAllMailboxesL, T_FilterNonMRMailboxesL, Teardown)

EUNIT_TEST(
    "Testing mailbox information fetching",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithMRMailboxesL, T_FetchSpecificMailboxInformationL, Teardown)    

EUNIT_TEST(
    "Testing invalid mailbox information fetching",
    "CMRMbUtilsExtensionImpl",
    "NewL",
    "FUNCTIONALITY",
    SetupWithAllMailboxesL, T_FetchInvalidMailboxInformationL, Teardown)      
    
EUNIT_END_TEST_TABLE
