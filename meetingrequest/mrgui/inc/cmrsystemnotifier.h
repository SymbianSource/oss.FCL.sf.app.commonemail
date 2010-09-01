/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Response dialog's custom control implementation
*
*/


#ifndef CMRSYSTEMNOTIFIER_H
#define CMRSYSTEMNOTIFIER_H

#include <e32base.h>

class CEnvironmentChangeNotifier;
class CESMRFieldEventQueue;

/**
 * CMRSystemNotifier listen system events and sends MRUI events.
 */
NONSHARABLE_CLASS( CMRSystemNotifier ) : public CBase
    {
public: // Contstruction and destruction
    /**
     * Creates and initialzes new CMRSystemNotifier object.
     * Ownership is transferred to caller.
     * @param aEventQueue Reference to event queue
     */
    static CMRSystemNotifier* NewL( 
            CESMRFieldEventQueue& aEventQueue );
    
    /**
     * C++ destructor
     */
    ~CMRSystemNotifier();

    /**
     * Starts system notifier
     */
    void StartL();
    
    /**
     * Stops system notifier
     */
    void Stop();
    
private: // Implementation
    CMRSystemNotifier( CESMRFieldEventQueue& aEventQueue );
    void ConstructL();
    static TInt EnvChangeCallbackL( TAny* aThisPtr );
    void DoEnvChangeL();
    
private: // Data
    /// Own: System notifier
    CEnvironmentChangeNotifier* iSystemNotifier;
    /// Ref: Reference to event queue
    CESMRFieldEventQueue& iEventQueue;
    };

#endif // CMRSYSTEMNOTIFIER_H
