/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MESMRICALVIEWERASYNCCOMMAND_H
#define MESMRICALVIEWERASYNCCOMMAND_H

//<cmail>
#include "esmricalvieweropcodes.hrh"
//</cmail>

/**
 *  MESMRIcalViewerCommand defines the interface for smricalviewer commands.
 */
class MESMRIcalViewerAsyncCommand
    {
public: // Destruction
    /**
     * Virtual destructor
     */
    virtual ~MESMRIcalViewerAsyncCommand() {}

public: // Interface

    /**
     *  Executes the command. Leaves with system wide error code on error.
     */
    virtual void ExecuteAsyncCommandL() = 0;

    /**
     * Cancel command execution.
     */
    virtual void CancelAsyncCommand() = 0;
    };

#endif // MESMRICALVIEWERASYNCCOMMAND_H

// EOF
