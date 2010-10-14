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

#include <QObject>


namespace EmailClientApi
{
class NmApiEngine;
class NmApiFolder;

class NmApiFolderListingPrivate : public QObject
{
    Q_OBJECT
    
public:

    NmApiFolderListingPrivate(quint64 mailboxId, QObject *parent = NULL);
    virtual ~NmApiFolderListingPrivate();
    qint32 listFolders();
    bool folders(QList<NmApiFolder> &folders);
    void cancel();
    bool isRunning() const;
private:
	  quint64 mMailboxId;
    NmApiEngine *mEngine;
    bool mIsRunning;
    QList<NmApiFolder> mFolders;
};
}
#endif /* NMAPIFOLDERLISTINGPRIVATE_H */
