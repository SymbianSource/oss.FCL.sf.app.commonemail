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
* Description:  ESMR imap calendar entry retriever definition
*
*/


#ifndef CESMRICALVIEWERCOMMANDBASE_H
#define CESMRICALVIEWERCOMMANDBASE_H

#include <e32base.h>
//<cmail>
#include "mesmricalviewerobserver.h"
#include "esmricalvieweropcodes.hrh"
//</cmail>
#include "mesmricalviewercommand.h"

class CCalSession;
class CFSMailMessage;

/**
 * CESMRIcalViewerCommandBase is concrete command base class for
 * esmricalviewer command.
 */
NONSHARABLE_CLASS( CESMRIcalViewerCommandBase ) :
        public CBase,
        public MESMRIcalViewerCommand
    {
public:
    /**
     * C++ destructor
     */
    virtual ~CESMRIcalViewerCommandBase();

public: // From MESMRIcalViewerCommand
    TESMRIcalViewerOperationType OperationType() const;

protected: // Implementation
    /**
     * Base class constructor.
     * @param aType Operation type
     * @param aCalSession reference to calendar session
     */
    CESMRIcalViewerCommandBase(
            TESMRIcalViewerOperationType aType,
            CCalSession& aCalSession );

    /**
     *  Base class 2nd phase constructor.
     */
    void BaseConstructL();

    /**
     * Fetches reference to calendar session
     */
    CCalSession& CalSession();

    /**
     * Sets command observer.
     * @param aObserver Pointer to command observer.
     */
    void SetObserver(
            MESMRIcalViewerObserver* aObserver );

    /**
     * Fetches command observer.
     * @return Command observer.
     */
    MESMRIcalViewerObserver* Observer();

    /**
     * Sets mail message.
     * @param aMessage Pointer to message.
     */
    void SetMessage(
            CFSMailMessage* aMessage );

    /**
     * Fetches message pointer.
     * @return message pointer.
     */
    CFSMailMessage* Message();

private: // data
    /// Own. Operation type
    TESMRIcalViewerOperationType iOperationType;
    /// Ref. Calendar session
    CCalSession& iCalSession;
    /// Ref: Command observer.
    MESMRIcalViewerObserver* iObserver;
    /// Ref: Message to be handled
    CFSMailMessage* iMessage;
    };

#endif // CESMRICALVIEWERCOMMANDBASE_H

// EOF
