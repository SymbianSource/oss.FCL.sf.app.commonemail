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


namespace EmailClientApi
{
class NmApiEngine;
class NmApiMailbox;

class NmApiMailboxListingPrivate : public QObject
{
public:

    NmApiMailboxListingPrivate(QObject *parent = NULL);
    virtual ~NmApiMailboxListingPrivate();
    qint32 listMailboxes();
    bool mailboxes(QList<NmApiMailbox> &mailboxes);
    bool isRunning() const;
    void cancel();
private:
    QList<NmApiMailbox> mMailboxes;
    NmApiEngine *mEngine;
    bool mIsRunning;
};

}

#endif /* NMAPIMAILBOXLISTINGPRIVATE_H */
