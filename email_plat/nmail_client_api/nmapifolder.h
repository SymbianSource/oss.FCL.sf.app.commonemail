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

#include <QString>
#include <QExplicitlySharedDataPointer>
#include <QList>
#include "nmenginedef.h"

/**
 * Mailbox metadata class
 */
namespace EmailClientApi
{

enum EmailFolderType
{
    Inbox, Drafts, Outbox, Sent, Deleted, EOther
};

enum
{
    GeneralError = -1, NotSupportedError
};

class NmFolderPrivate : public QSharedData
{
public:
    NmFolderPrivate();
    virtual ~NmFolderPrivate();

    quint64 id;
    QString name;
    EmailFolderType folderType;
    quint64 parentId;
    QList<quint64> childFolderIds;
};

class NMENGINE_EXPORT NmFolder
{
public:
    /*
     * constructor for NmFolder class
     */
    NmFolder();
    /*
     * destructor for NmFolder class
     */
    ~NmFolder();

    /*
     * getter for id
     */
    quint64 id() const;

    /*
     * getter for name 
     */
    QString name() const;

    /*
     * getter for foldertype
     */
    EmailFolderType folderType() const;

    /*
     * setter for id
     */
    void setId(quint64 id);

    /*
     * setter for name
     */
    void setName(const QString& name);

    /*
     * setter for foldertype
     */
    void setFolderType(EmailFolderType folderType);

    /*
     * seter for childfolder ids
     */
    void setChildFolderIds(QList<quint64> &childFolderIds);

    /*
     * seter for childfolder ids
     */
    void setParentFolderId(quint64 parentId);

    /*
     * returns child folder ids.
     * to be implemented later when nmail functionality is available
     */
    void childFolderIds(QList<quint64> &childFolderIds);

    /*
     * returns parent folder id.
     * 
     */
    quint64 parentFolderId() const;

private:
    QExplicitlySharedDataPointer<NmFolderPrivate> d;
};

}
#endif
