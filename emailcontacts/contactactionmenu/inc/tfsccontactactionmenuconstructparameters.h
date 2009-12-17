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
* Description:  Declaration of construct parameters class for Action Menu
*
*/


#ifndef C_FSCCONTACTACTIONMENUCONSTRUCTPARAMETERS_H
#define C_FSCCONTACTACTIONMENUCONSTRUCTPARAMETERS_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionmenudefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CFscContactActionService;

/**
 *  Construction parameters for Action Menu
 *
 *  @since S60 3.1
 */
class TFscContactActionMenuConstructParameters
    {

public: // Public methods

    /**
     * Constructor
     *
     * @param aService Pointer to Contact Action Service
     * @param aMode Menu mode
     */
    TFscContactActionMenuConstructParameters( 
        CFscContactActionService& aService,
        TFscContactActionMenuMode aMode ) 
        : iService( aService ), iMode( aMode ) {}
   
public: // Public members

    /**
     * Pointer to CAS
     */
    CFscContactActionService& iService;
        
    /**
     * Mode of the menu
     */     
    TFscContactActionMenuMode iMode;

    };
#endif // C_FSCCONTACTACTIONMENUCONSTRUCTPARAMETERS_H
