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
* Description:  Converting CPbkContactItem to MVPbkStoreContact
*
*/


#ifndef CPBKXRCLCONTACTCONVERTER_H
#define CPBKXRCLCONTACTCONVERTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
//<cmail>
#include "fsccontactactionservicedefines.h"      // rfscstorecontactlist
//</cmail>

// FORWARD DECLARATIONS
class CBCardEngine;
class CPbkContactEngine;
class MVPbkContactStore;
class CVPbkVCardEng;
class MVPbkStoreContact;
class CVPbkContactManager;
class RWriteStream;

// CLASS DECLARATION

/**
 *  CPbkxRclContactConverter
 *  Convert contact to MVPbkStoreContact objects.
 * 
 */

NONSHARABLE_CLASS( CPbkxRclContactConverter ): public CBase, public MVPbkSingleContactOperationObserver 
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CPbkxRclContactConverter();

    /**
     * Two-phased constructor.
     */
    static CPbkxRclContactConverter* NewL(
            CPbkContactEngine&   aContactEngine, 
            CVPbkContactManager& aContactManager,
            MVPbkContactStore&   aContactStore );

    /**
     * Two-phased constructor.
     */
    static CPbkxRclContactConverter* NewLC(
            CPbkContactEngine&   aContactEngine, 
            CVPbkContactManager& aContactManager,
            MVPbkContactStore&   aContactStore );

    /**
     * Convert contact to MVPbkStoreContact
     * MVPbkStoreContact is stored to RFscStoreContactList.
     * 
     * @param aContactItem Contact item to convert
     * @param aConvertedContact Reference to list where result is stored.
     *        Calling client is responsible to delete MVPbkStoreContact object.
     */
    void ConvertContactL( 
            CPbkContactItem& aContactItem, 
            RFscStoreContactList& aConvertedContact );
       
    /**
     * Convert contact to MVPbkStoreContact
     * end export it to stream as VCard 
     * 
     * @param aContactItem Contact item to convert
     * @param stream to save VCard 
     * 
     */   
    void ConvertAndExportContactL( 
            CPbkContactItem& aContactItem, 
            RWriteStream& aStream );
    
    // from base class MVPbkSingleContactOperationObserver 
    void VPbkSingleContactOperationComplete( 
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    
    void VPbkSingleContactOperationFailed( 
            MVPbkContactOperationBase& aOperation, TInt aError );

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CPbkxRclContactConverter(CPbkContactEngine&   aContactEngine, 
                             CVPbkContactManager& aContactManager,
                             MVPbkContactStore&   aContactStore );

    /**
     *  Default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    // DATA
    /**
     * Reference to CPbkContactEngine
     */
    CPbkContactEngine&      iContactEngine;
    /**
     * Old vCard engine. 
     * Own.
     */    
    CBCardEngine*           iCardEngine;
    /**
     * Reference to CPbkContactEngine
     */    
    CVPbkContactManager&    iContactManager;
    /**
     * New Virtual Phonebook vCard engine. 
     * Own.
     */    
    CVPbkVCardEng*          iVPbkCardEngine;
    /**
     * Active scheduler waiter
     * Own.
     */    
    CActiveSchedulerWait*   iWait;
    /**
     * Reference to MVPbkContactStore
     */
    MVPbkContactStore&      iContactStore;
    /**
     * Last error from Operation execution
     */     
    TInt                    iLastError;
    };

#endif // CPBKXRCLCONTACTCONVERTER_H
