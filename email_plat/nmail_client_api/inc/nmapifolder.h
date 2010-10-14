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

#ifndef NMAPIFOLDERINFO_H
#define NMAPIFOLDERINFO_H

#include <QExplicitlySharedDataPointer>
#include <QList>
#include <nmapidef.h>
#include <nmapicommon.h>

namespace EmailClientApi
{

class NmApiFolderPrivate;

class NMAPI_EXPORT NmApiFolder
{
public:

    NmApiFolder();
    NmApiFolder(const NmApiFolder &nmApiFolder);
    virtual ~NmApiFolder();
    NmApiFolder &operator=(const NmApiFolder &folder);
    bool operator==(const NmApiFolder &folder);
    quint64 id() const;
    QString name() const;
    EmailClientApi::NmApiEmailFolderType folderType() const;
    void setId(quint64 id);
    void setName(const QString &name);
    void setFolderType(EmailClientApi::NmApiEmailFolderType folderType);
    void setChildFolderIds(QList<quint64> &childFolderIds);
    void setParentFolderId(quint64 parentId);
    void getChildFolderIds(QList<quint64> &childFolderIds);
    quint64 parentFolderId() const;

private:
    QExplicitlySharedDataPointer<NmApiFolderPrivate> d;
};

}
#endif
