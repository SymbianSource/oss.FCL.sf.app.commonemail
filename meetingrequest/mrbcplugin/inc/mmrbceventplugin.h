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
* Description: MBUtils ECOM definition
*
*/
#ifndef M_MRBCEVENTPLUGIN_H
#define M_MRBCEVENTPLUGIN_H

#include <e32base.h>

class MCalenCommandHandler;

/**
 * MMRBCEventPlugin defines interface for BC event plug-in
 */
class MMRBCEventPlugin
    {
public: // Interface
    
    /**
     * Fetches event plugin command handler
     */
    virtual MCalenCommandHandler* CommandHandler() = 0;
    
    /**
     * Returns extension interface.
     * @param aExtensionUid Extension identifier 
     */
    virtual TAny* Extension( TUid /* aExtensionUid */ ) { return NULL; }
    };

#endif // M_MRBCPLUGINCOMMAND_H

// EOF
