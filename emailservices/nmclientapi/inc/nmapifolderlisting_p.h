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
#ifndef NMAPIFOLDERLISTINGPRIVATE_H
#define NMAPIFOLDERLISTINGPRIVATE_H

class NmApiEngine;

namespace EmailClientApi
{

class NmApiFolder;

class NmApiFolderListingPrivate : public QObject
{
public:

    NmApiFolderListingPrivate(QObject *parent = 0);
    virtual ~NmApiFolderListingPrivate();

    bool initializeEngine();
    void releaseEngine();
    qint32 grabFolders();

    QList<EmailClientApi::NmApiFolder> mFolders;//!<List of folders \sa getFolder \sa QSharedData \sa EmailClientApi::NmApiFolder
    NmApiEngine *mEngine;//!<Pointer to engine instance \sa NmUiEngine
    bool mIsRunning;//!<Keep info if folder listing is running \sa isRunning
    quint64 mMailboxId;//!<Keep info about mailbox id for foler
};
}
#endif /* NMAPIFOLDERLISTINGPRIVATE_H */
