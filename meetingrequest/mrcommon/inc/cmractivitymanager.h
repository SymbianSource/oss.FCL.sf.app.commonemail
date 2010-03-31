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
* Description:  Meeting Request timer for the focus
*
*/
#ifndef CMRTIMER_H
#define CMRTIMER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATIONS
class MMRActivityManagerObserver
    {
    public :
        virtual void PeriodCompleted() = 0;
    };

NONSHARABLE_CLASS( CMRActivityManager ): public CBase
    {
    public:

        IMPORT_C static CMRActivityManager* NewL(
                MMRActivityManagerObserver& aObserver,
                TInt aTimeout = 6 );

        IMPORT_C ~CMRActivityManager();

        IMPORT_C void SetTimeout( TInt aTimeout );

        IMPORT_C void Start();

        IMPORT_C void Reset();

    private: // New functions

        /**
         * The call back function.
         * \param aAny A pointer to this class.
         */
        static TInt PeriodicTimerCallBack(TAny* aAny);

        /**
         * Notice that this is a sample fuction.
         *
         */
        void NotifyObserver();

    private: // Implementation
        CMRActivityManager(
                MMRActivityManagerObserver& aObserver,
                TInt aTimeout );
        void ConstructL();

    private:    // Member data

        /**
         * The periodic timer.
         * Owned by CMyClass
         */
        CPeriodic* iPeriodicTimer;

        /// Ref: Observer for listening the activity manager
        MMRActivityManagerObserver& iObserver;
        /// Timeout
        TInt iTimeoutInSecs;
    };

#endif // CMRTIMER_H
// End of file

