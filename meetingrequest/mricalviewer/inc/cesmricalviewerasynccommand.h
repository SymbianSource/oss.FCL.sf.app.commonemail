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
* Description:  ESMRIcalViewer async command base class
*
*/


#ifndef CESMRICALVIEWERASYNCCOMMAND_H
#define CESMRICALVIEWERASYNCCOMMAND_H

#include "cesmricalviewercommandbase.h"
#include "mesmricalviewerasynccommand.h"

class CAsyncCallBack;

/**
 * CESMRIcalViewerAsyncCommand is base class for concrete
 * asynchronous esmricalviewer commands.
 */
NONSHARABLE_CLASS( CESMRIcalViewerAsyncCommand ) :
        public CESMRIcalViewerCommandBase,
        public MESMRIcalViewerAsyncCommand
    {
public:
    /**
     * C++ destructor
     */
    virtual ~CESMRIcalViewerAsyncCommand();

public: // From MESMRIcalViewerCommand
    void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver );
    void CancelCommand();

protected: // Implementation
    CESMRIcalViewerAsyncCommand(
            TESMRIcalViewerOperationType aType,
            CCalSession& aCalSession );
    void BaseConstructL();

private: // data
    /// Own. Asynchronous invoker.
    CAsyncCallBack* iAsyncInvoker;
    };

#endif

// EOF
