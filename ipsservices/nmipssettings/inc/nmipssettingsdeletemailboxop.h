/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef NMIPSSETTINGSDELETEMAILBOXOP_H
#define NMIPSSETTINGSDELETEMAILBOXOP_H

#include <QObject>
#include <nmcommon.h>

class CMailboxDisconnectWait;
class NmIpsSettingsManagerBase;
class HbMessageBox;
class HbProgressDialog;
class HbAction;

/// Operation results
enum NmDeleteMailboxOpResults {
    /// Operation successful. Errorcode is always 0
    DeleteMbResultSuccess,
    /// Operation has failed. Errorcode will identify the error
    DeleteMbResultFailure,
    /// Operation was canceled. Errorcode is always 0
    DeleteMbResultCanceled
};

class NmIpsSettingsDeleteMailboxOp : public QObject
{
    Q_OBJECT

public:
    NmIpsSettingsDeleteMailboxOp(NmIpsSettingsManagerBase &settingsManager);
    ~NmIpsSettingsDeleteMailboxOp();

    void startOperation();

public slots:
    void mailboxDisconnected();
    
signals:
    void operationComplete(int result, int error);
    void goOffline(NmId mailboxId);
    void stateChanged();

private:
    /// Operation states
    enum DeleteOpStates {
        StateConfirmation,
        StateDisconnect,
        StateDelete,
        StateUnregisterMailbox,
        StateDone
    };

private:
    void complete(int result, int error);
    
private slots:
    void handleConfimationDlg(HbAction *action);
    void handleDisconnectTimeout();
    void processState();
    void changeState(DeleteOpStates nextState);

    
private: // data
    // Owned. Messaging server observer (Symbian class)
    CMailboxDisconnectWait *mDisconnectWait;
    // Settings manager
    NmIpsSettingsManagerBase &mSettingsManager;
    // Owned. Confirmation query dialog
    HbMessageBox *mDeleteConfirmationDlg;
    // Owned. Wait dialog
    HbProgressDialog* mDeleteProgressDlg;
    // Current state
    DeleteOpStates mState;
    // How many retry attempts is left
    int mRetryCount;
    // Operation error code
    int mError;
    // Operation result (NmDeleteMailboxOpResults)
    int mResult;
};

#endif // NMIPSSETTINGSDELETEMAILBOXOP_H
