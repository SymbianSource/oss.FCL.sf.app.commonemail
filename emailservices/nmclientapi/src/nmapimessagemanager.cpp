/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     Email message related operations
 */
 
#include "nmapiheaders.h"


/*!
    \class NmApiMessageManager
    \brief A class for performing various operations on messages.
 */
 
 
namespace EmailClientApi
{

NmApiMessageManager::NmApiMessageManager(QObject *parent, quint64 mailboxId)
	:QObject(parent)
{
	d = new NmApiMessageManagerPrivate(this, mailboxId);
}

NmApiMessageManager::~NmApiMessageManager()
{
	delete d;
}
   

    // creates a new email message
    // signaled with draftMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
bool NmApiMessageManager::createDraftMessage(const QVariant *initData)
{
    NM_FUNCTION;
    Q_UNUSED(initData);
	return false;
}
    
    // creates fw message
    // signaled with forwardMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
 bool NmApiMessageManager::createForwardMessage(NmApiMessage *orig,const QVariant *initData)
 {
    NM_FUNCTION;
    Q_UNUSED(initData);
    Q_UNUSED(orig);
	return false;
 }
    
    // creates reply message
    // signaled with replyMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
bool NmApiMessageManager::createReplyMessage(const NmApiMessage *orig,
        const QVariant *initData,bool replyAll)
{
    NM_FUNCTION;
    Q_UNUSED(orig);
    Q_UNUSED(initData);
    Q_UNUSED(replyAll);
	return false;
}
 
/*!
 \fn moveMessages 
 \param messageIds Id list of source messages.
 \param sourceFolderId Id of the source folder.
 \param targetFolderId Id of the target folder.
 \return true if operation was successfully started.
 
 Starts async move operation for given messages.  
 Completion signalled with messagesMoved(int result).
 */
bool NmApiMessageManager::moveMessages(const QList<quint64> messageIds,
									quint64 sourceFolderId,
									quint64 targetFolderId)
{    
    NM_FUNCTION;
    Q_UNUSED(messageIds);
    Q_UNUSED(sourceFolderId);
    Q_UNUSED(targetFolderId);
    return false;
	//return d->moveMessages(messageIds,sourceFolderId,targetFolderId);
}
    
// signaled with messageSaved(quint64 messageId, int result)
bool NmApiMessageManager::saveMessage(const NmApiMessage &message)
{
    Q_UNUSED(message);
	return false;
}
    
// deletes message
// signaled with messagesDeleted(int result)
bool NmApiMessageManager::deleteMessages(const QList<quint64> messageIds)
{
    Q_UNUSED(messageIds);
	return false;
}
    
    // starts fetching rest of message body from server
    // signaled with messageFetched(quint64 messageId, int result)
bool NmApiMessageManager::fetchMessage(quint64 messageId)
{
    Q_UNUSED(messageId);
	return false;
}
    
    // moves message to outbox. Actual sending time may be immediate or scheduled
    // signaled with messageSent(quint64 messageId, int result)
bool NmApiMessageManager::sendMessage(const NmApiMessage &message)
{
    Q_UNUSED(message);
	return false;
}

// creates new attachment for a message. Currently attachment can be specified as file name (attachmentSpec is QString)
// signaled with attachmentCreated(quint64 attachemntId)
//  * 
bool NmApiMessageManager::createAttachment(NmApiMessage &message,
										const QVariant &attachmenSpec)
{
    Q_UNUSED(message);
    Q_UNUSED(attachmenSpec);
    return false;
}
                                                      
// removes attachment from a message
// signaled with attachmentRemoved(int result)
bool NmApiMessageManager::removeAttachment(NmApiMessage &message,
quint64 attachmentId)
{    
    Q_UNUSED(message);
    Q_UNUSED(attachmentId);
    return false;
}

bool NmApiMessageManager::fetchAttachment(const NmApiMessage &relatedMessage,
        quint64 attachmentId)
{
    Q_UNUSED(relatedMessage);
    Q_UNUSED(attachmentId);
    return false;
}
 
} // namespace EmailClientApi


