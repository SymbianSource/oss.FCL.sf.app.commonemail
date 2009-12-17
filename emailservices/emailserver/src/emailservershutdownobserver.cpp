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
* Description : Source file for EmailServer shutdown watcher
*
*
*/

#include <apgtask.h>

#include "emailtrace.h"
#include "emailshutdownconst.h"

#include "emailservershutdownobserver.h"
#include "FsEmailGlobalDialogsAppUi.h"

CEmailServerShutdownObserver* CEmailServerShutdownObserver::NewL(
        CFsEmailGlobalDialogsAppUi& aAppUi )
    {
    CEmailServerShutdownObserver* self=new(ELeave) CEmailServerShutdownObserver( aAppUi );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CEmailServerShutdownObserver::CEmailServerShutdownObserver(
        CFsEmailGlobalDialogsAppUi& aAppUi )
    : CActive( EPriorityStandard ), iAppUi(aAppUi)
    {}

void CEmailServerShutdownObserver::ConstructL()
    {
    FUNC_LOG;
    
    // Attach and subscribe to P&S to get the shutdown event from shutter app
    User::LeaveIfError( iProperty.Attach( KEmailShutdownPsCategory,
                                          EEmailPsKeyShutdownClients ));
    CActiveScheduler::Add(this);

    iProperty.Subscribe(iStatus);
    SetActive();
    }

CEmailServerShutdownObserver::~CEmailServerShutdownObserver()
    {
    Cancel();
    iProperty.Close();
    }

void CEmailServerShutdownObserver::DoCancel()
    {
    iProperty.Cancel();
    }

void CEmailServerShutdownObserver::RunL()
    {
    FUNC_LOG;

    // Property updated, which means that installation is starting and
    // FsMailServer needs to shutdown
    INFO( "Shutdown event received" );
    iAppUi.Exit();
    }
 
