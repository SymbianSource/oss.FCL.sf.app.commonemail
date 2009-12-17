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
* Description : Header definition for EmailServer shutdown observer
* 
*
*/

#ifndef EMAILSERVERSHUTDOWNOBSERVER_H_
#define EMAILSERVERSHUTDOWNOBSERVER_H_

#include <e32base.h>        // CActive
#include <e32property.h>    // RProperty

// Forward declarations
class CFsEmailGlobalDialogsAppUi;

class CEmailServerShutdownObserver : public CActive
    {
public:
    // Two phase constructor and destructor
    static CEmailServerShutdownObserver* NewL( CFsEmailGlobalDialogsAppUi& aAppUi );
    ~CEmailServerShutdownObserver();

private:
    // Construction
    CEmailServerShutdownObserver( CFsEmailGlobalDialogsAppUi& aAppUi );
    void ConstructL();

    // From base class CActive
    void RunL();
    void DoCancel();

private:
    /**
     * AppUi class.
     * Not Owned.
     */
    CFsEmailGlobalDialogsAppUi& iAppUi;

    /**
     * Publish & Subscribe property to be observed.
     */
    RProperty iProperty;
    };


#endif /*EMAILSERVERSHUTDOWNOBSERVER_H_*/
