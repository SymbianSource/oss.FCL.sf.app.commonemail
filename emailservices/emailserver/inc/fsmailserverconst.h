/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constant definitions for FSMailServer
*
*/


#ifndef FSMAILSERVERCONST_H_
#define FSMAILSERVERCONST_H_

//<cmail>
#include "fsmailserverconst.hrh"
//</cmail>

///////////////////////////////////////////////////////////////////////////
// Different error notes that can be shown via this notifier
enum TFsEmailNotifierSystemMessageType
    {
    // Used to indicate that system message type is not set
    // -> meant only for internal use
    EFsEmailNotifErrUndefined = -1,

    EFsEmailNotifErrCustom = 0,

    // Error messages that doesn't require response
    EFsEmailNotifErrAccountDisabled,
    EFsEmailNotifErrPasswordExpired,
    EFsEmailNotifErrDataDeleted,
    EFsEmailNotifErrLoginUnsuccesfull,
    EFsEmailNotifErrPasswordChanged,
    EFsEmailNotifErrLoginCanceled,
    EFsEmailNotifErrServerNotAvailable,
    EFsEmailNotifErrNetworkNotAvailable,
    EFsEmailNotifErrServerBusy,
    EFsEmailNotifErrUnableToConnectToServerTryAgain,
    EFsEmailNotifErrEmailAddrAndPwdDontMatch,
    EFsEmailNotifErrInvalidEmailAddress,
    EFsEmailNotifErrServerOffline,
    EFsEmailNotifErrRoamingTurnedOn,
    EFsEmailNotifErrRoamingCosts,
    EFsEmailNotifErrUnableToComplete,
    EFsEmailNotifErrConnectionError,
    EFsEmailNotifErrUnableToConnect,
    EFsEmailNotifErrMultipleSyncErrors,
    EFsEmailNotifErrOutOfMemory,
    EFsEmailNotifErrLoginFailed,
    EFsEmailNotifErrServerNotFoundCheckSettings,
    EFsEmailNotifErrUnableToConnectToServer,
    EFsEmailNotifErrDisconnected,
    EFsEmailNotifErrServerErroTryAgain,
    
    EFsEmailNotifErrLastNoResponse = 1000,

    // Error messages that do require response (which soft key pressed)
    EFsEmailNotifErrCustomResponse,
    EFsEmailNotifErrSynchronizationFailed,

    // Note! Not yet implemented as not currently needed.
    // Value kept here as a placeholder for future needs.
    EFsEmailNotifErrAuthenticatingWaitNote = 2000
    };

#endif /*FSMAILSERVERCONST_H_*/
