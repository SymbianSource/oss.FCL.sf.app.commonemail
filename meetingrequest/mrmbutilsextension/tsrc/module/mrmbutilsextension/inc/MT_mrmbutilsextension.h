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

#ifndef __MRMBUTILSEXTENSION_H__
#define __MRMBUTILSEXTENSION_H__

// INCLUDES
#include <ceunittestsuiteclass.h>
#include <eunitdecorators.h>
#include <cmrmailboxutils.h>

// FORWARD DECLARATIONS
class CMRMBUtilsMBInfoHandler;
class CFSMailClient;
class CFSMailBox;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( MT_MRMbUtilsExtension ) : 
		public CEUnitTestSuiteClass,
		public MMRMailboxUtilsObserver
    {
public:  // Constructors and destructor

	static MT_MRMbUtilsExtension* NewL();
	static MT_MRMbUtilsExtension* NewLC();
	~MT_MRMbUtilsExtension();

private:
    void HandleMRMailboxEventL( 
            MMRMailboxUtilsObserver::TEventType aType );
	
private: // Constructors

	MT_MRMbUtilsExtension();
	void ConstructL();
	
private: // New methods

	 void SetupL();
	 void SetupWithAllMailboxesL();
	 void SetupWithMRMailboxesL();
	 
	 void Teardown();
	
	 void T_MRECOMExtensionFoundL();
	 
	 void T_MRCreateCMailExtensionL();
	
	 void T_CheckECOMDataL();
	 
	 void T_FindMRMailboxesL();
	 
	 void T_FilterNonMRMailboxesL();
	 
	 void T_FetchSpecificMailboxInformationL();
	 
	 void T_FetchInvalidMailboxInformationL();
	 
private: // Data

	CFSMailClient* iMailClient;
	
	RPointerArray<CFSMailBox> iMailboxes;
	
	CMRMBUtilsMBInfoHandler* iCmailHandler;
	
	EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __MRMBUTILSEXTENSION_H__

// EOF
