/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all test cases class member functions
*
*/


//#include <BAUTILS.H>
#include "ShutdownServerTest.h"
//#include <ismsgstorepropertykeys.h>
//#include "messagestoreclientserver.h"

// ============================ MEMBER FUNCTIONS ===============================

CShutdownServerTest* CShutdownServerTest::NewL( CStifLogger* aLog )
    {
    CShutdownServerTest* self = new(ELeave) CShutdownServerTest( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CShutdownServerTest::CShutdownServerTest( CStifLogger* aLog ) : CAsyncTestCaseBase( aLog ) 
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CShutdownServerTest::~CShutdownServerTest()
    {
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
void CShutdownServerTest::ConstructL()
    {
    CAsyncTestCaseBase::ConstructL();
    }

// -----------------------------------------------------------------------------
// ExecuteL : start executing the test case
// -----------------------------------------------------------------------------
TBool CShutdownServerTest::ExecuteL()
    {
    LogHeader( _L("Shutdown Server") );
    
    ShutdownServerL();
    WaitForServerToTerminate();
    
    //we are done 
    return ETrue;
    }


//  End of File
