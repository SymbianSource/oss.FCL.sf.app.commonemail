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
#ifndef C_MRBCPLUGINRESOURCEREADER_H
#define C_MRBCPLUGINRESOURCEREADER_H

#include <e32base.h>
#include <barsc.h>
#include <babitflags.h>
#include "tmrbcplugincommand.h"
#include "esmrdef.h"

class CMRBCEventPlugin;
class MCalenServices;

/**
 * CMRBCPluginResourceReader reads BC Plug-In resource file
 * information.
 * @see mrbcplugin.rh
 */
NONSHARABLE_CLASS( CMRBCPluginResourceReader ) : public CBase
    {
public: // Constrction and destruction
    
    /**
     * Creates and initializes new CMRBCPluginResourceReader object.
     * @param aCalenServices Reference to calendar services
     */
    static CMRBCPluginResourceReader* NewL(
            MCalenServices& aCalenServices );
    
    /**
     * C++ destructor
     */
    ~CMRBCPluginResourceReader();
    
public: // Interface 
    /**
     * Reads resource information from file.
     * @param aResourceFile
     */
    void ReadFromResourceFileL(
            TUid aUid,
            const TDesC& aResourceFile );
    
    /**
     * Version information
     * @return Version information
     */
    TVersion Version() const;
    
    /**
     * Tests if BC plugin resource supports specified type
     * @param aEventType Specifies event type
     * @return ETrue if event type is supported, EFalse otherwise
     */
    TBool SupportsType( TBCPluginEventType aEventType ) const;
    
    /**
     * Tests if specified command is supported
     * @param aCommand Specifies the command
     * @return ETrue if command is supported, EFalse otherwise
     */
    TBool SupportsCommand( TInt aCommand ) const;
    
    /**
     * Fetches command from resource.
     * @param aCommand Command to be executed
     * @param aPluginCommand On return contains the fecthed plug-in command
     * @return KErrNone if command is found, KErrNotFound otherwise 
     */
    TInt Command( 
            TInt aCommand, 
            TMRBCPluginCommand& aPluginCommand) const; 
    /**
     * Fetches plugin implemenation UID
     * @return plugin implemenation UID
     */
    TUid PluginImplUid() const;
    
    /**
     * Fetches the plugin implemenation.
     * @return Plugin implementation
     */
    CMRBCEventPlugin& PluginL();
    
private: // Implementation    
    CMRBCPluginResourceReader(
            MCalenServices& aCalenServices );
    void ConstructL();
    
private: // Data
    /// Ref: Calender services
    MCalenServices& iCalenServices;
    /// Own: Resource reader
    RResourceFile iBCPluginResourceFile;
    /// Own: Plugin implementation UID
    TUid iPluginImplementationUid;
    /// Own: Resource file offset
    TInt iOffset;    
    /// Own: Version information
    TVersion iVersion;
    /// Own: Supported event types
    TBitFlags32 iSupportedEventTypes;    
    /// Own: Command IDs
    RArray<TMRBCPluginCommand> iPluginCommands;
    /// Own: Plugin implementation
    CMRBCEventPlugin* iPlugin;
    };

#endif // C_MRBCPLUGINRESOURCEREADER_H

// EOF
