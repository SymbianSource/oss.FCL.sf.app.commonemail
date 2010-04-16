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
};

class NmIndicator : public HbIndicatorInterface
{
public:

    NmIndicator(const QString &indicatorType);
    ~NmIndicator();

public: // From HbIndicatorInterface

    bool handleInteraction(InteractionType type);
    QVariant indicatorData(int role) const;

protected: // From HbIndicatorInterface

    bool handleClientRequest(RequestType type, const QVariant &parameter);

private:

    bool showMailbox(quint64 mailboxId);
    void storeMailboxData(QVariant mailboxData);

private:

	NmMailboxInfo mMailbox;
};

#endif // NMINDICATOR_H

