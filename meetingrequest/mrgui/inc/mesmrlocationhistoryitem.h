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
* Description:  Interface definition for location history item
*
*/


#ifndef MESMRLOCATIONHISTORYITEM_H
#define MESMRLOCATIONHISTORYITEM_H

/**
 *  MESMRLocationHistoryItem defines interface for a location history item.
 */
class MESMRLocationHistoryItem
    {
public:
    /**
     * Virtual C++ destructor.
     */
    virtual ~MESMRLocationHistoryItem() { }

    /**
     * Sets aAddress as new address 
     * 
     * @param aAddress New address to be set 
     */
    virtual void SetAddressL( const TDesC& aAddress ) = 0;
     
    /**
     * Sets aUrl as new url 
     * 
     * @param aUrl New url to be set 
     */
    virtual void SetUrlL( const TDesC& aUrl ) = 0;
     
    /**
     * Returns address 
     * 
     * @return the address of this item
     */
    virtual const TDesC& Address() const = 0;
     
    /**
     * Returns url 
     * 
     * @return the url of this item
     */
    virtual const TDesC& Url() const = 0;
     
    /**
     * Returns id
     * 
     * @return the id of this item
     */ 
    virtual TUint Id() const = 0;
    };

#endif // MESMRLOCATIONHISTORYITEM_H
