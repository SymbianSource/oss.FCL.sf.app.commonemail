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

#include "nmfolder.h"



NmFolderPrivate::NmFolderPrivate()
:mFolderId(0),
mParentFolderId(0),
mMailboxId(0),
mName(""),
mFolderType(NmFolderOther),
mMessageCount(0),
mUnreadMessageCount(0),
mUnseenCount(0),
mSubFolderCount(0),
mUpdated(QDateTime())
{
}

NmFolderPrivate::~NmFolderPrivate()
{
}


/*!
    \class NmFolder
    \brief The NmFolder class represents data model for folder.
*/


/*!
    Constructor for folder identified by \a folderId.
*/
NmFolder::NmFolder(NmId folderId)
{
    d = new NmFolderPrivate();
    d->mFolderId = folderId;
}

/*!
    Copy constructor
*/
NmFolder::NmFolder(const NmFolder &folder)
{
    d = folder.d;
}

/*!
 * Creating NmFolder from privateData
 */
NmFolder::NmFolder(QExplicitlySharedDataPointer<NmFolderPrivate> folderPrivate)
{
  d = folderPrivate;
}

/*!
    operator =
*/
NmFolder &NmFolder::operator=(const NmFolder &folder)
{
  if (this != &folder) {
    d = folder.d;
  }
  return *this;
}

/*!
    Destructor
*/
NmFolder::~NmFolder()
{
}

/*!
    Folder id accessor
*/
NmId NmFolder::folderId() const
{
    return d->mFolderId;
}

/*!
    Set folder id
*/
void NmFolder::setFolderId(NmId folderId)
{
  d->mFolderId = folderId;
}

/*!
    Parent id accessor
*/
NmId NmFolder::parentId() const
{
    return d->mParentFolderId;
}

/*!
    Set parent id
*/
void NmFolder::setParentId(NmId parentFolderId)
{
    d->mParentFolderId = parentFolderId;
}

/*!
    Mailbox id accessor
*/
NmId NmFolder::mailboxId() const
{
    return d->mMailboxId;
}

/*!
    Set mailbox id
*/
void NmFolder::setMailboxId(NmId mailboxId)
{
    d->mMailboxId = mailboxId;
}

/*!
    Folder name accessor
*/
QString NmFolder::name() const
{
    return d->mName;
}

/*!
    Set folder name
*/
void NmFolder::setName(QString name)
{
    d->mName = name;
}

/*!
    Folder type accessor
*/
NmFolderType NmFolder::folderType() const
{
    return d->mFolderType;
}

/*!
    Set folder type
*/
void NmFolder::setFolderType(NmFolderType folderType)
{
    d->mFolderType = folderType;
}

/*!
    Last update time accessor
*/
QDateTime NmFolder::lastUpdated() const
{
    return d->mUpdated;
}

/*!
    Set last update time
*/
void NmFolder::setLastUpdated(QDateTime time)
{
    d->mUpdated = time;
}

/*!
    Get count of messages
 */
quint32 NmFolder::messageCount() const
{
  return d->mMessageCount;
}

/*!
    Set count of messages
 */
void NmFolder::setMessageCount(quint32 messageCount)
{
  d->mMessageCount = messageCount;
}

/*!
    Count of unread messages
 */
quint32 NmFolder::unreadMessageCount() const
{
  return d->mUnreadMessageCount;
}

/*!
    Set count of unread messages
 */
void NmFolder::setUnreadMessageCount(quint32 unreadMessageCount)
{
  d->mUnreadMessageCount = unreadMessageCount;
}

/*!
    Count of unseen messages
 */
quint32 NmFolder::unseenCount() const
{
  return d->mUnseenCount;
}

/*!
    Set count of unseen messages
 */
void NmFolder::setUnseenCount(quint32 unseenCount)
{
  d->mUnseenCount =  unseenCount;
}

/*!
    Count of subfolders of current folder
 */
quint32 NmFolder::subFolderCount() const
{
  return d->mSubFolderCount;
}

/*!
    Set count of subfolders of current folder
 */
void NmFolder::setSubFolderCount(quint32 subFolderCount)
{
  d->mSubFolderCount = subFolderCount;
}

