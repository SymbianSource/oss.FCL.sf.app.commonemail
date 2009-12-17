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
* Description:  Definition of class CFscContactActionMenuCasItemImpl.
*
*/


#ifndef C_FSCCONTACTACTIONMENUCASITEMIMPL_H
#define C_FSCCONTACTACTIONMENUCASITEMIMPL_H

#include <e32base.h>
//<cmail>
#include "tfsccontactactionqueryresult.h"
//</cmail>
#include "cfsccontactactionmenuitemimpl.h"

// FORWARD DECLARATIONS

/**
 *  Contact Action Menu CAS Item implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuCasItemImpl 
    : public CFscContactActionMenuItemImpl
    {

public: // Public constructor and destructor

    /**
     * Two-phased constructor.
     *
     * @param aItem Contact Action query result item
     * @return New instance of the component  
     */
    static CFscContactActionMenuCasItemImpl* NewL(
        const TFscContactActionQueryResult& aItem );

    /**
     * Two-phased constructor.
     *
     * @param aItem Contact Action query result item     
     * @return New instance of the component  
     */
    static CFscContactActionMenuCasItemImpl* NewLC(
        const TFscContactActionQueryResult& aItem);

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuCasItemImpl();

private:

    /**
     * Constructor.
     *
     * @param aItem Contact Action query result item     
     */
    CFscContactActionMenuCasItemImpl(
        const TFscContactActionQueryResult& aItem );

    /**
     * Second phase constructor.
     *
     * @param aItem Contact Action query result item
     */
    void ConstructL( const TFscContactActionQueryResult& aItem );
                 
private: // data

    };

#endif // C_FSCCONTACTACTIONMENUCASITEMIMPL_H

