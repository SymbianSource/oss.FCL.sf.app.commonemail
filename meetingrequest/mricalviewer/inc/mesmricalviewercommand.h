/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for esmricalviewer command
*
*/

#ifndef MESMRICALVIEWERCOMMAND_H
#define MESMRICALVIEWERCOMMAND_H

//<cmail>
#include "esmricalvieweropcodes.hrh"
//</cmail>

class CFSMailMessage;
class MESMRIcalViewerObserver;

/**
 *  MESMRIcalViewerCommand defines the interface for smricalviewer commands.
 */
class MESMRIcalViewerCommand
    {
public: // Destruction
    /**
     * Virtual destructor
     */
    virtual ~MESMRIcalViewerCommand() {}

public: // Interface

    /**
     * Returns command operation type
     */
    virtual TESMRIcalViewerOperationType OperationType() const = 0;

    /**
     *  Executes the command. Leaves with system wide error code on error.
     */
    virtual void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver ) = 0;

    /**
     * Cancel command execution.
     */
    virtual void CancelCommand() = 0;
    };

#endif // MESMRICALVIEWERCOMMAND_H

// EOF

