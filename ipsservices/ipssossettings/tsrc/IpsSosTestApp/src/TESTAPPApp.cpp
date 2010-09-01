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
* Description: 
*
*/
/*
* ============================================================================
*  Name     : CTestAppApp from TestAppApp.cpp
*  Part of  : TestApp
* ============================================================================
*/

// INCLUDE FILES
#include    "TestAppApp.h"
#include    "TestAppDocument.h"

#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CTestAppApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }


// ---------------------------------------------------------
// CTestAppApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CTestAppApp::AppDllUid() const
    {
    return KUidTestApp;
    }

   
// ---------------------------------------------------------
// CTestAppApp::CreateDocumentL()
// Creates CTestAppDocument object
// ---------------------------------------------------------
//
CApaDocument* CTestAppApp::CreateDocumentL()
    {
    return CTestAppDocument::NewL( *this );
    }
// End of File  

