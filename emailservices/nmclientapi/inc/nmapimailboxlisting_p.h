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
 * Description: 
 *
 */
#ifndef NMAPIMAILBOXLISTINGPRIVATE_H
#define NMAPIMAILBOXLISTINGPRIVATE_H

#include "nmapimailbox.h"
#include <QObject>
#include <QList>

class NmEngine;

namespace EmailClientApi
{
class NmMailboxListingPrivate : public QObject
{

public:

    NmMailboxListingPrivate(QObject *parent = 0);
    virtual ~NmMailboxListingPrivate();

    bool initializeEngine();

    void releaseEngine();

    qint32 grabMailboxes();

    QList<EmailClientApi::NmMailbox> mMailboxes;//!<List of mailboxes \sa getMailboxInfo \sa QSharedData \sa EmailClientApi::NmMailbox
    NmEngine* mNmEngine;//!<Pointer to engine instance \sa NmUiEngine
    bool mIsRunning;//!<Keep info if mailbox listing is running \sa isRunning
};
}
#endif /* NMMAILBOXLISTINGPRIVATE_H_ */
