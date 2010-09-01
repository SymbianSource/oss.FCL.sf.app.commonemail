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
#ifndef C_MRBCPLUGINOPENMRVIEWERCMD_H
#define C_MRBCPLUGINOPENMRVIEWERCMD_H

#include "mmrbcplugincommand.h"
#include <e32base.h>
#include <MAgnEntryUi.h>

class MCalenServices;
class CESMRViewerController;
class CCalEntry;

/**
 * 
 */
NONSHARABLE_CLASS( CMRBCPluginOpenMRViewerCmd ) : 
        public CBase,
        public MMRBCPluginCommand,
        public MAgnEntryUiCallback
    {
public: // Construction and destruction
    
    /**
     * Creates new CMRBCPluginOpenMRViewerCmd command
     * @param aServices Reference to calednar services
     */
    static CMRBCPluginOpenMRViewerCmd* NewL(
            MCalenServices& aServices );
    
    /**
     * C++ destructor
     */
    ~CMRBCPluginOpenMRViewerCmd();

public: // From base class

    /**
     * Executes the command.
     * @param aCommand Command to be executed
     */
    void ExecuteCommandL(
            const TCalenCommand& aCommand );
    
private: // From base class MAgnEntryUiCallback
    void ProcessCommandL(TInt aCommandId);
    TInt ProcessCommandWithResultL( TInt aCommandId );
    
private: // Implementation
    CMRBCPluginOpenMRViewerCmd(
            MCalenServices& aServices );
    void ConstructL();
    void LaunchMRViewerL(
            const TCalenCommand& aCommand );
    void GetEntryL( 
            const TCalenCommand& aCommand );
    
    void IssueNotifyL(
            const TCalenCommand& aCommand,
            MAgnEntryUi::TAgnEntryUiOutParams aOutParams );
    
    void IssueCommandL( 
            MAgnEntryUi::TAgnEntryUiOutParams aOutParams );
    
private: // Data
    /// Ref: Reference to calendar services
    MCalenServices& iServices;
    /// Own: Editor component
    CESMRViewerController* iEditor;
    /// Own: Entry to be created
    CCalEntry* iEntry;
    };

#endif // C_MRBCPLUGINOPENMRVIEWERCMD_H

// EOF
