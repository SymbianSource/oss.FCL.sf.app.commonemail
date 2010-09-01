/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing the RECAL customisation interface
*
*/

#ifndef CMRBCPLUGINCMDHANDLER_H
#define CMRBCPLUGINCMDHANDLER_H

#include <e32base.h>
#include <calencommandhandler.h>

class MCalenServices;
class CMRBCPluginResourceReader;
class CMRBCPluginEntryLoader;
class CMRBCPluginCreateNewMRCmd;
class CMRBCPluginOpenMRViewerCmd;

/**
 * CMRBCPluginCmdHandler is responsible for handling 
 * BC Plugin commands.
 */
NONSHARABLE_CLASS( CMRBCPluginCmdHandler ) : 
        public CBase,
        public MCalenCommandHandler
    {
public: // Construction and destruction
    
    /**
     * Creates and initializes new CMRBCPluginCmdHandler object.
     * @param aServices Reference to calendar services
     * @param aEntryLoader Reference to entry loader
     * @param aPluginResourceReader Reference to plug-in resource reader
     */
    static CMRBCPluginCmdHandler* NewL(
            MCalenServices& aServices );
    
    /**
     * C++ destructor
     */
    ~CMRBCPluginCmdHandler();
    
private: // From base class MCalenCommandHandler
    TBool HandleCommandL( const TCalenCommand& aCommand );
    TAny* CalenCommandHandlerExtensionL( TUid aExtensionUid );
    
private: // Implementation
    CMRBCPluginCmdHandler( 
            MCalenServices& aServices );
    void ConstructL();
    
private: // Data
    /// Ref: Reference to calen services object
    MCalenServices& iServices;
    };

#endif // CMRBCPLUGINCMDHANDLER_H

// EOF
