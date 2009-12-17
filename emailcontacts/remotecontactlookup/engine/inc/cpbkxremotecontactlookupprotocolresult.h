/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRemoteContactLookupProtocolResult.
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPPROTOCOLRESULT_H
#define CPBKXREMOTECONTACTLOOKUPPROTOCOLRESULT_H

#include <cntitem.h>

/**
 *  Results are returned through this interface. Results ownership is always 
 *  transfered to the client so it must take care of deleting them. 
 *
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupProtocolResult : public CBase
    {

public:

    /**
     * Is the search result complete or do we need a second phase?
     * 
     * @return ETrue if the search result is complete.
     */
    virtual TBool IsComplete() const = 0; 
    
    /**
    * Sets result as complete.
    *
    * This means that result now contains all possible fields and no more
    * fields can be retrieved.
    */
    virtual void Complete() = 0;

    /**
     * Returns extra protocol data need for mapping contact item to protocol 
     * level entries.
     *
     * @return extra protocol data
     */
    virtual const TDesC& ExtraProtocolData() const = 0;        

    /** 
     * Returns contact item.
     *
     * @return contact item
     */
    virtual CContactItem& ContactItem() = 0;         
    };


#endif // CPBKXREMOTECONTACTLOOKUPPROTOCOLRESULT_H
