/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Local message deletion class
*
*/

#ifndef IPSPLGDELETELOCAL_H
#define IPSPLGDELETELOCAL_H

//  INCLUDES

#include <cacheman.h>


// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*
*/
class CIpsPlgDeleteLocal : public CImCacheManager
    {
    public:

        /**
        * Two-phased constructor.
        * @param aMessageSelection array of message ids to be purged
        * @param aMsvSession
        * @param aObserverRequestStatus
        * @return pointer to created CIpsPlgDeleteLocal object.
        */
        static CIpsPlgDeleteLocal* NewL(
            CMsvEntrySelection& aMessageSelection,
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );

        /**
        * destructor
        */
        virtual ~CIpsPlgDeleteLocal();

    
    protected:
    
        /**
        * Symbian OS constructor.
        * @param aMessageSelection array of message ids to be purged
        */
        void ConstructL( CMsvEntrySelection& aMessageSelection );

    private:
    
        /**
        * from CImCacheManager
        */
        TBool Filter() const;

    private:
    
        /**
        * C++ constructor
        */
        CIpsPlgDeleteLocal(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus );

    private: // data

        CMsvEntrySelection*                 iMessageSelection;
        
    };

#endif      // IPSPLGDELETELOCAL_H

// End of File
