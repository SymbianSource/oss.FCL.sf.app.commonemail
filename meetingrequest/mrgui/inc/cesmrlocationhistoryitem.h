/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location history item class definition
*
*/


#ifndef CESMRLOCATIONHISTORYITEM_H
#define CESMRLOCATIONHISTORYITEM_H

//  INCLUDES
#include <e32base.h>
#include "mesmrlocationhistoryitem.h"

// CLASS DECLARATION

/**
 * List item wrapper for location history
 */
NONSHARABLE_CLASS( CESMRLocationHistoryItem ) : public CBase,
                                                public MESMRLocationHistoryItem
    {
public:
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * 
     * @return CESMRLocationHistoryItem instance
     */
    static CESMRLocationHistoryItem* NewL( const TUint aId,
                                           const TDesC& aAddress, 
                                           const TDesC& aUrl );

    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * and leaves it on the cleanupstack
     * 
     * @return CESMRLocationHistoryItem instance
     */
    static CESMRLocationHistoryItem* NewLC( const TUint aId,
                                            const TDesC& aAddress, 
                                            const TDesC& aUrl );

    /**
     * C++ Destructor.
     */
    virtual ~CESMRLocationHistoryItem();

public: // from MESMRLocationHistoryItem
    
    /**
     * Sets aAddress as new address 
     * 
     * @param aAddress New address to be set 
     */
    void SetAddressL( const TDesC& aAddress );
    
    /**
     * Sets aUrl as new url 
     * 
     * @param aUrl New url to be set 
     */
    void SetUrlL( const TDesC& aUrl );
    
    /**
     * Returns address 
     * 
     * @return the address of this item
     */
    const TDesC& Address() const;
    
    /**
     * Returns url 
     * 
     * @return the url of this item
     */
    const TDesC& Url() const;
    
    /**
     * Returns id
     * 
     * @return the id of this item
     */ 
    TUint Id() const;
    
protected:
    /**
     * C++ default constructor.
     * 
     * @param aId id for this item
     */
    CESMRLocationHistoryItem(  const TUint aId );

private:
    /**
     * ConstructL
     * 
     * @param aAddress address for this item
     * @param aUrl url for this item
     */
    void ConstructL( const TDesC& aAddress, 
                     const TDesC& aUrl );

private: // data
    HBufC*  iAddress; /// Owned: address

    HBufC*  iUrl; /// Owned: url

    TUint iId; // unique id
    };

#endif  // CESMRLOCATIONHISTORYITEM_H

// End of File
