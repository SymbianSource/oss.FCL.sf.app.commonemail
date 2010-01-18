/*
* ============================================================================
*  Name     : CTestAppApp from TestAppApp.h
*  Part of  : TestApp
* ============================================================================
*/

#ifndef TestAppAPP_H
#define TestAppAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
// UID of the application
// <cmail> S60 UID update
const TUid KUidTestApp = { 0x2001FE0F };
// </cmail> S60 UID update

// CLASS DECLARATION

/**
* CTestAppApp application class.
* Provides factory to create concrete document object.
*
*/
class CTestAppApp : public CAknApplication
    {

    public: // Functions from base classes
    private:

        /**
        * From CApaApplication, creates CTestAppDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns application's UID (KUidTestApp).
        * @return The value of KUidTestApp.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

