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
* Description:  Email setup plugin parameters header file
*
*/


#ifndef __EMAILSETUPPLUGINPARAMS_H
#define __EMAILSETUPPLUGINPARAMS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MEmailSetupFramework;

// CLASS DECLARATION

/**
 * TPluginParam
 * Parameter transmitted upon constructing Ecom plugin
 */
class TEmailSetupPluginParam
    {
    public: // Constructor
    
        /**
         * C++ Constructor
         *
         * @param aObserver reference to plugin observer
         */
        inline TEmailSetupPluginParam( MEmailSetupFramework& aFramework ):iFramework(aFramework){};
    
    public: // Data
    
        // Framework
        MEmailSetupFramework& iFramework;
        
    };


#endif // __EMAILSETUPPLUGINPARAMS_H

// End of File
