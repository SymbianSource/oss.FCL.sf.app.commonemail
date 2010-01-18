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

