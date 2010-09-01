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


#ifndef T_FSCCONTACTACTIONPLUGINPARAMS_H
#define T_FSCCONTACTACTIONPLUGINPARAMS_H

#include <e32base.h>

// FORWARD DECLARATIONS
class MFscActionUtils;

/**
 *  Construction parameters for action plugins
 *
 *  @since S60 3.1
 */
class TFscContactActionPluginParams
    {

public: // Public methods

    /**
     * Constructor
     *
     * @param aUtils utils interface
     * @param aMode Menu mode
     */
    TFscContactActionPluginParams( 
        MFscActionUtils* aUtils ) 
        : iUtils( aUtils ) {}
   
public: // Public members

    /**
     * Pointer to utils interface
     */
    MFscActionUtils* iUtils;

    };
#endif // T_FSCCONTACTACTIONPLUGINPARAMS_H
