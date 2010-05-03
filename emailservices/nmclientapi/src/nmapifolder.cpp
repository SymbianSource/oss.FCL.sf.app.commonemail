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

#include "nmapifolder.h"

namespace EmailClientApi
{
NmFolderPrivate::NmFolderPrivate()
{

}

NmFolderPrivate::~NmFolderPrivate()
{

}

/*
 * constructor for NmFolder class
 */
NmFolder::NmFolder()
{
    d = new NmFolderPrivate();
}

/*
 * destructor for NmFolder class
 */
NmFolder::~NmFolder()
{

}

/*
 * getter for id
 */
quint64 NmFolder::id() const
{
    return d->id;
}

/*
 * getter for name 
 */
QString NmFolder::name() const
{
    return d->name;
}

/*
 * getter for foldertype
 */
EmailFolderType NmFolder::folderType() const
{
    return d->folderType;
}

/*
 * setter for name
 */
void NmFolder::setName(const QString& name)
{
    d->name = name;
}

/*
 * setter for id
 */
void NmFolder::setId(quint64 id)
{
    d->id = id;
}

/*
 * setter for foldertype
 */
void NmFolder::setFolderType(EmailFolderType folderType)
{
    d->folderType = folderType;
}

/*
 * setter for parent folder id
 */
void NmFolder::setParentFolderId(quint64 parentId)
{
    d->parentId = parentId;
}

/*
 * seter for childfolder ids
 */
void NmFolder::setChildFolderIds(QList<quint64> &childFolderIds)
{
    d->childFolderIds = childFolderIds;
}

/*
 * Returns count of child folder ids.
 * to be implemented later when nmail functionality is available
 */
void NmFolder::getChildFolderIds(QList<quint64> &childFolderIds)
{
    childFolderIds = d->childFolderIds;
}
;

quint64 NmFolder::parentFolderId() const
{
    return d->parentId;
}
}
