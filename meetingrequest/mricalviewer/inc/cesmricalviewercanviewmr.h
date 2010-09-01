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
* Description:  Definition for can view message command
*
*/


#ifndef CESMRICALVIEWERCANVIEWMR_H
#define CESMRICALVIEWERCANVIEWMR_H

#include <e32base.h>
#include "cesmricalviewercommandbase.h"

/**
 * CESMRImapIcalRetriever is responsible for parsing iCal information
 * from email message and importing it to calendar entry.
 */
NONSHARABLE_CLASS( CESMRIcalViewerCanViewMsgCmd ) :
        public CESMRIcalViewerCommandBase
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRIcalViewerCanViewMsgCmd command.
     * After execution the flag contains information whether message can be
     * viewed or not.
     *
     * @param aCalSession Reference to calendar database session.
     * @param aFlag Reference to boolean flag
     * @return Pointer to CESMRInfoIcalRetriever object.
     */
    static CESMRIcalViewerCanViewMsgCmd* NewL(
            CCalSession& aCalSession,
            TBool& aFlag );

    /**
     * C++ destructor.
     */
    ~CESMRIcalViewerCanViewMsgCmd();

public: // From MESMRIcalViewerCommand through CESMRIcalViewerCommandBase
    void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver );
    void CancelCommand();

private: // Implementation
    CESMRIcalViewerCanViewMsgCmd(
            CCalSession& aCalSession,
            TBool& aFlag );
    void ConstructL();

private: // Data
    /// Own: Command result
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    /// Ref: Reference to flag.
    TBool& iFlag;
    };

#endif // CESMRIcalViewerCanViewMr_h

// EOF
