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
* Description:  Definition of class TFscContactActionQueryResult.
*
*/


#ifndef T_FSCCONTACTACTIONQUERYRESULT_H
#define T_FSCCONTACTACTIONQUERYRESULT_H

#include <e32base.h>

//<cmail>
#include "tfsccontactactionvisibility.h"
//</cmail>

// FORWARD DECLARATIONS
class MFscContactAction;

/**
 *  Contact action list item.
 *  Contact action query result item.
 *
 *  @since S60 3.1
 */
class TFscContactActionQueryResult
    {

public: // Public constructors
 
    /**
     * Default constructor
     */
    TFscContactActionQueryResult() {}

    /**
     * Constructor
     *
     * @param aAction Pointer to action object. 
     *   Action object can be used to show action in UI. Ownership not transferred
     * @param aPriority Action priority
     * @param aActionMenuVisibility Action's visibility in action menu
     * @param aOptionsMenuVisibility Action's visibility in options menu  
     */
    TFscContactActionQueryResult( 
        const MFscContactAction* aAction, 
        TUint aPriority,
        TFscContactActionVisibility aActionMenuVisibility,
        TFscContactActionVisibility aOptionsMenuVisibility ) 
    : iAction( aAction ), 
      iPriority( aPriority ),
      iActionMenuVisibility( aActionMenuVisibility ),
      iOptionsMenuVisibility( aOptionsMenuVisibility ) {}

public: // Public variables

    const MFscContactAction* iAction; // Reference to action object
    TInt iPriority; // Actions priority
    
    TFscContactActionVisibility iActionMenuVisibility; // Action menu visibility
    TFscContactActionVisibility iOptionsMenuVisibility; // Options menu visibility
    
    };

#endif // T_FSCCONTACTACTIONQUERYRESULT_H
