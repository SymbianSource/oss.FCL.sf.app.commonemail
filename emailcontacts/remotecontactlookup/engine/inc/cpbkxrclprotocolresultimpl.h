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
* Description:  Definition of the class CPbkxRclProtocolResultImpl.
*
*/


#ifndef CPBKXRCLPROTOCOLRESULTIMPL_H
#define CPBKXRCLPROTOCOLRESULTIMPL_H

#include "cpbkxremotecontactlookupprotocolresult.h"

/**
* Class that holds remote contact search results.
*
* @lib pbkxengine.lib
* @since S60 3.1
*/
class CPbkxRclProtocolResultImpl : public CPbkxRemoteContactLookupProtocolResult
    {
public: // constructors and destructor

    /**
    * Two-phased constructor.
    *
    * @param aIsComplete Flag indicating whether result is complete or not.
    * @param aContactItem Result contact item.
    * @param aExtraProtocolData Extra protocol specific data.
    * @return Create object.
    */
    static CPbkxRclProtocolResultImpl* NewL(
        TBool aIsComplete,
        CContactItem& aContactItem,
        const TDesC& aExtraProtocolData );

    /**
    * Two-phased constructor.
    *
    * Leaves pointer to the created object on cleanupstack.
    *
    * @param aIsComplete Flag indicating whether result is complete or not.
    * @param aContactItem Result contact item.
    * @param aExtraProtocolData Extra protocol specific data.
    * @return Create object.
    */
    static CPbkxRclProtocolResultImpl* NewLC(
        TBool aIsComplete,
        CContactItem& aContactItem,
        const TDesC& aExtraProtocolData );

    /**
    * Destructor.
    */
    virtual ~CPbkxRclProtocolResultImpl();

public: // methods from CPbkxRemoteContactLookupProtocolResult

    /**
     * Is the search result complete or do we need a second phase?
     * 
     * @return ETrue if the search result is complete.
     */
    virtual TBool IsComplete() const; 
    
    /**
    * Sets result as complete.
    *
    * This means that result now contains all possible fields and no more
    * fields can be retrieved.
    */
    virtual void Complete();

    /**
     * Returns extra protocol data need for mapping contact item to protocol 
     * level entries.
     *
     * @return extra protocol data
     */
    virtual const TDesC& ExtraProtocolData() const;        

    /** 
     * Returns contact item.
     *
     * @return contact item
     */
    virtual CContactItem& ContactItem();
    
private: // constructor

    /**
    * Constructor.
    *
    * @param aIsComplete Flag indicating whether result is complete or not.
    * @param aContactItem Result contact item.
    */
    CPbkxRclProtocolResultImpl( TBool aIsComplete, CContactItem& aContactItem );

    /**
    * Second-phase constructor.
    *
    * @param aExtraProtocolData Extra protocol data.
    */
    void ConstructL( const TDesC& aExtraProtocolData );

private: // data

    // Flag indicating whether result is complete or not.
    // If result is not complete, second-phase search is needed.
    TBool iIsComplete;

    // Result contact item. Not owned.
    CContactItem* iContactItem;

    // Extra protocol data.
    RBuf iExtraProtocolData;

    };

#endif
