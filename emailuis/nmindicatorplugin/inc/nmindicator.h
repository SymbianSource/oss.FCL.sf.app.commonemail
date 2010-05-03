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
 * Description: Mail Indicator class
 *
 */

#ifndef NMINDICATOR_H
#define NMINDICATOR_H

#include <QObject>
#include <hbindicatorinterface.h>
#include <nmcommon.h>

class NmMailboxInfo
{
public:
    NmMailboxInfo();

public:
    NmId mId;
    QString mName;
    NmSyncState mSyncState;
    NmConnectState mConnectState;
    int mUnreadMails;
    int mOutboxMails;
};

class NmIndicator : public HbIndicatorInterface
{
    Q_OBJECT

public:

    NmIndicator(const QString &indicatorType);
    ~NmIndicator();

public: // From HbIndicatorInterface

    bool handleInteraction(InteractionType type);
    QVariant indicatorData(int role) const;
    HbIndicatorInterface::Category category() const;
    bool acceptIcon(bool sending);
    void updateGlobalStatus(bool sending);

protected: // From HbIndicatorInterface

    bool handleClientRequest(RequestType type, const QVariant &parameter);

signals:

    void indicatorIconLost();
    void globalStatusChanged(bool sending);
    void mailboxLaunched(quint64 mailboxId);

private slots:

    void hideSendIndicator();
    
private:

    void storeMailboxData(QVariant mailboxData);
    void showSendProgress();
    bool isSending() const;

private:

	NmMailboxInfo mMailbox;
    bool mShowIndicator;
    bool mSendingState;
    bool mShowSendProgress;
	bool mActive;
};

#endif // NMINDICATOR_H

