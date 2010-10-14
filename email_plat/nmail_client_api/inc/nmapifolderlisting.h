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

#ifndef NMAPIFOLDERLISTING_H_
#define NMAPIFOLDERLISTING_H_

#include <nmapimessagetask.h>
#include <nmapidef.h>


namespace EmailClientApi
{
class NmApiEngine;
class NmApiFolderListingPrivate;
class NmApiMailbox;
class NmApiFolder;

class NMAPI_EXPORT NmApiFolderListing : public NmApiMessageTask
{
    Q_OBJECT
public:

    NmApiFolderListing(QObject *parent, const quint64 &mailboxId);
    virtual ~NmApiFolderListing();

    enum {FolderListingFailed = -1};

    bool getFolders(QList<EmailClientApi::NmApiFolder> &folders);
    bool isRunning() const;

signals:

    void foldersListed(qint32 count);

public slots:

    bool start();
    void cancel();

private:
    NmApiFolderListingPrivate *mFolderListing;
};
}

#endif /* NMAPIFOLDERLISTING_H_ */
