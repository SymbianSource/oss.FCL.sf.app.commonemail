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
* Description:  Location history item factory class definition
*
*/


#ifndef CESMRLOCATIONHISTORYITEMFACTORY_H
#define CESMRLOCATIONHISTORYITEMFACTORY_H

#include <e32base.h>

class MESMRLocationHistoryItem;

/**
 *  CESMRLocationHistoryItemFactory implements interface for creating location history items.
 */
NONSHARABLE_CLASS(CESMRLocationHistoryItemFactory) : public CBase
    {
public: // Construction and destruction
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     * 
     * @return CESMRLocationHistoryItemFactory instance
     */
    static CESMRLocationHistoryItemFactory* NewL();

    /**
     * C++ destructor.
     */
    ~CESMRLocationHistoryItemFactory();

public: // new methods
    
    /**
     * Creates a location history item. Ownership is transferred to
     * caller.
     * 
     * @param aAddress Address for the location history item
     * @param aUrl Url for the location history item
     * 
     * @return Pointer to created location history item object.
     */
    MESMRLocationHistoryItem* CreateLocationHistoryItemL( const TDesC& aAddress, 
                                                          const TDesC& aUrl );

private: // Implementation
    
    /**
     * Default constructor
     */ 
    CESMRLocationHistoryItemFactory();
 
private: // Data
    
    TUint iIndex; // global running index of location history items 
    };

#endif // CESMRLOCATIONHISTORYITEMFACTORY_H
