/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FreestyleEmailUi exit guardian.
*
*/

#ifndef C_EUIEXITGUARDIAN_H
#define C_EUIEXITGUARDIAN_H

#include <e32base.h>

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
/**
 * Application exit guardian for email UI. This is needed because some routines
 * in email framework create nested scheduler loops to hide asynchronity in
 * them. When application is shutdown these nested levels may cause crashes
 * as resources are freed while they are still being used on other recursion
 * levels.
 *
 *
 * @since S60 5.1
 */
class CEUiExitGuardian : public CBase
    {

public:

    /**
     * Leave safe construction for CEuiExitGuardian.
     * @return Newly created instance of CEuiExitGuardian
     */
    static CEUiExitGuardian* NewL(CFreestyleEmailUiAppUi& aAppUi);

    /**
     * Destructor.
     */
    ~CEUiExitGuardian();


    /**
     * Called by the application to inform that application is ready to
     * exit. If there is any nested active scheduler levels (i.e. recursion),
     * exit execution will be delayed until every nested level has returned.
     *
     * @return KRequestPending if the exit execution is delayed because of
     * nesting, KErrNone otherwise.
     */
    TInt TryExitApplication();

private:
    /**
     * Constructor.
     */
    CEUiExitGuardian( CFreestyleEmailUiAppUi& aAppUi );

    /**
     * Pass 2 constructor, may leave.
     */
    void ConstructL();

    /**
     * Static callback method for CPeriodic.
     */
    static TInt PeriodicCallBack( TAny* aPtr );

private:
    // Reference to application UI
    CFreestyleEmailUiAppUi& iAppUi;
    TBool iPeriodicTimerStarted;
    CPeriodic* iPeriodicTimer;
    };

#endif // C_EUIEXITGUARDIAN_H
