/*
* ============================================================================
*  Name     : CTestAppDocument from TestAppDocument.h
*  Part of  : TestApp
* ============================================================================
*/

// INCLUDE FILES
#include "TestAppDocument.h"
#include "TestAppAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CTestAppDocument::CTestAppDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CTestAppDocument::~CTestAppDocument()
    {
    }

// EPOC default constructor can leave.
void CTestAppDocument::ConstructL()
    {
    }

// Two-phased constructor.
CTestAppDocument* CTestAppDocument::NewL(
        CEikApplication& aApp)     // CTestAppApp reference
    {
    CTestAppDocument* self = new (ELeave) CTestAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CTestAppDocument::CreateAppUiL()
// constructs CTestAppAppUi
// ----------------------------------------------------
//
CEikAppUi* CTestAppDocument::CreateAppUiL()
    {
    return new (ELeave) CTestAppAppUi;
    }

// End of File  
