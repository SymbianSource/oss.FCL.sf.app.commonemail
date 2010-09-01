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
* Description:  Interface definition for location history item factory
*
*/


#ifndef MESMRLOCATIONHISTORYITEMFACTORY_H
#define MESMRLOCATIONHISTORYITEMFACTORY_H

class MESMRLocationHistoryItem;

/**
 *  MESMRLocationHistoryItemFactory defines interface for creating location history items.
 */
class MESMRLocationHistoryItemFactory
    {
public:
    /**
     * Virtual C++ destructor.
     */
    virtual ~MESMRLocationHistoryItemFactory() { }

    /**
     * Creates a location history item. Ownership is transferred to
     * caller.
     * 
     * @param aAddress Address for the location history item
     * @param aUrl Url for the location history item
     * 
     * @return Pointer to created location history item object.
     */
    virtual MESMRLocationHistoryItem* CreateLocationHistoryItemL( 
            const TDesC& aAddress, 
            const TDesC& aUrl ) = 0;
    };

#endif // MESMRLOCATIONHISTORYITEMFACTORY_H
