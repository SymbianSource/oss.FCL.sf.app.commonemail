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

#ifndef NMAPIMAILBOXLISTING_H_
#define NMAPIMAILBOXLISTING_H_


#include <nmapimessagetask.h>
#include <nmapidef.h>

class QObject;

namespace EmailClientApi
{
class NmApiMailboxListingPrivate;
class NmApiMailbox;

/*!
   \class Class for creating list of all mailboxes
 */
class NMAPI_EXPORT NmApiMailboxListing : public NmApiMessageTask
{
    Q_OBJECT
public:

    NmApiMailboxListing(QObject *parent);
    virtual ~NmApiMailboxListing();

    bool getMailboxes(QList<EmailClientApi::NmApiMailbox> &mailboxes);
    bool isRunning() const;
    
    enum {MailboxListingFailed = -1};

signals:

    void mailboxesListed(qint32 count);

public slots:
    bool start();
    void cancel();

private:
    NmApiMailboxListingPrivate *mNmApiMailboxListingPrivate;
};

}

#endif /* NMAPIMAILBOXLISTING_H_ */
