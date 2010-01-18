/*
* ============================================================================
*  Name     : CTestAppDocument from TestAppDocument.h
*  Part of  : TestApp
* ============================================================================
*/

#ifndef TestAppDOCUMENT_H
#define TestAppDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CTestAppDocument application class.
*/
class CTestAppDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CTestAppDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CTestAppDocument();

    public: // New functions

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * EPOC default constructor.
        */
        CTestAppDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CTestAppAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

