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
* Description:  Class implementing new entry commands
*
*/

#ifndef CMRBCPLUGINNEWCALEVENTCOMMAND_H
#define CMRBCPLUGINNEWCALEVENTCOMMAND_H

#include "mmrbcplugincommand.h"
#include "esmrdef.h"

#include <e32base.h>
#include <calentry.h>
#include <ConeResLoader.h>
#include <MAgnEntryUi.h>

class MCalenServices;
class CESMRViewerController;


/**
 * CBCCalNewEventCommand is responsible for handling 
 * create new event commands.
 */
NONSHARABLE_CLASS( CMRPluginNewCalEntryCommand ) : 
        public CBase,
        public MMRBCPluginCommand,
        public MAgnEntryUiCallback
    {
public: // Construction and destruction
    
    static CMRPluginNewCalEntryCommand* NewL( 
            MCalenServices& aServices,
            CCalEntry::TType aType );
    
    static CMRPluginNewCalEntryCommand* NewLC(
            MCalenServices& aServices,
            CCalEntry::TType aType );
    
    /**
     * C++ destructor
     */
    ~CMRPluginNewCalEntryCommand();

protected: // from base class MBCCalEventCommand
    void ExecuteCommandL( 
            const TCalenCommand& aContext );

private: // From base class MAgnEntryUiCallback
    void ProcessCommandL(TInt aCommandId);
    TInt ProcessCommandWithResultL( TInt aCommandId );    
    
private: // Implementation
    CMRPluginNewCalEntryCommand( 
            MCalenServices& aServices,
            CCalEntry::TType aEntryType);
    void ConstructL();    
    void CreateEntryL( 
            const TCalenCommand& aCommand );    
    void CreateAndExecuteViewL(
            const TCalenCommand& aCommand );       
    void IssueCommandL( 
                MAgnEntryUi::TAgnEntryUiOutParams aOutParams );
            
private: // data
    /// Ref: Reference to calendar services
    MCalenServices& iServices;
    /// Own: Calendar Entry 
    CCalEntry* iCalEntry;
    /// Own: Entry type
    CCalEntry::TType iEntryType;
    /// Own: Viewer component
    CESMRViewerController* iViewer;
    };

#endif // CBCCALEVENTCOMMAND_H

// EOF
