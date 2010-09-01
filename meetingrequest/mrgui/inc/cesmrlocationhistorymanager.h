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
* Description:  Location history manager class definition
*
*/


#ifndef CESMRLOCATIONHISTORYMANAGER_H
#define CESMRLOCATIONHISTORYMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h> 

class MESMRLocationHistoryItemFactory;
class CESMRLocationHistoryItemFactory;
class MESMRLocationHistoryItem;
class CRepository;

// CLASS DECLARATION
/**
 * Location history manager
 */
NONSHARABLE_CLASS( CESMRLocationHistoryManager ) : public CBase,
                                                   public MCenRepNotifyHandlerCallback
    {
public:
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * 
     * @return CESMRLocationHistoryManager instance
     */
    static CESMRLocationHistoryManager* NewL();

    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * and leaves it on the cleanupstack
     * 
     * @return CESMRLocationHistoryManager instance
     */
    static CESMRLocationHistoryManager* NewLC();

    /**
     * C++ Destructor.
     */
    virtual ~CESMRLocationHistoryManager();

public: // new methods
    
    /**
     * Updates the location history with the given location history item
     * 
     * @param aItem location history item to be used to update location history
     */
    void UpdateLocationHistoryL( const MESMRLocationHistoryItem* aItem );
      
    /**
     * Returns the location history item from index position
     *
     * @param aIndex position of the wanted item
     *  
     * @return the location history item at given position
     */
    const MESMRLocationHistoryItem& LocationHistoryItemL( TInt aIndex );
    
    /**
     * Returns the number of location history items available
     * 
     * @return the number of location history items available
     */ 
    TUint ItemCount();
    
    /**
     * Creates a location history item. Ownership is transferred to
     * caller.
     * 
     * @param aAddress Address for the location history item
     * @param aUrl Url for the location history item
     * 
     * @return Pointer to created location history item object.
     */
    MESMRLocationHistoryItem* CreateLocationHistoryItemL( 
                const TDesC& aAddress, 
                const TDesC& aUrl );
    
protected:
    /**
     * C++ default constructor.
     */
    CESMRLocationHistoryManager();

private: // From MCenRepNotifyHandlerCallback
    void HandleNotifyGeneric( TUint32 aId );
    
private:
    /**
     * ConstructL
     */
    void ConstructL();

    /**
     * Reads the location order from central repository
     */ 
    void ReadOrderDataL();
    
    /**
     * Reads the location history from central repository
     */ 
    void ReadHistoryDataL();
    
private: // data

    /// Own: Central Repository session
    CRepository* iCRSession;    
    
    /// Own: Central Repository observer
    CCenRepNotifyHandler* iNotifyHandler;
    
    /// own: list of location history items
    /**
     * Notice that this list represents the contents of location history cenrep keys
     * so that first key is always mapped to first list item. I.e. this list is never sorted
     */   
    RPointerArray<MESMRLocationHistoryItem> iHistoryList;
    
    // location history item sort order    
    RArray<TUint> iOrder;

    /// own: location history item factory instance
    CESMRLocationHistoryItemFactory* iFactory;
    
    // Maximum number of location history items available
    TInt iMaxCount;
    };

#endif  // CESMRLOCATIONHISTORYMANAGER_H

// End of File
