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
* Description:  MR Async command handler
*
*/
#ifndef CMRASYNCHCMD_H
#define CMRASYNCHCMD_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MAknToolbarObserver;

// CLASS DECLARATIONS
NONSHARABLE_CLASS( CMRAsynchCmd )
    : public CAsyncOneShot
    {
    public:
        static CMRAsynchCmd* NewL();
        ~CMRAsynchCmd();
        
    public: // Interface
        /**
         * Asynchronously calls the aObserver with aCmd as a parameter
         * @param aCmd Command the will be sent as param to aObserver
         * @param aObserver Pointer to observer that will be notified
         */
        void NotifyObserver( TInt aCmd, MAknToolbarObserver* aObserver );
        
    protected: // From base classes
        void RunL();
        
    private: // Implementation
        CMRAsynchCmd();
        
    private: // Data
        /// Own: command that is sent as param to observer
        TInt iCommand;
        /// Ref: Observer that will be notified
        MAknToolbarObserver* iObserver;
    };

#endif // CMRASYNCHCMD_H

// End of file

