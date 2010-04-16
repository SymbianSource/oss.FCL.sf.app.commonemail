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

#include "nmapifolder.h"
#include <QObject>

class NmEngine;

namespace EmailClientApi
{

class NmFolderListingPrivate : public QObject
{
public:

    NmFolderListingPrivate(QObject *parent = 0);
    virtual ~NmFolderListingPrivate();

    bool initializeEngine();
    void releaseEngine();
    qint32 grabFolders();

    QList<EmailClientApi::NmFolder> mFolders;//!<List of folders \sa getFolder \sa QSharedData \sa EmailClientApi::NmFolder
    NmEngine* mEngine;//!<Pointer to engine instance \sa NmUiEngine
    bool mIsRunning;//!<Keep info if folder listing is running \sa isRunning
    quint64 mMailboxId;//!<Keep info about mailbox id for foler
};
}
#endif /* NMFOLDERLISTINGPRIVATE_H_ */
