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

 NmApiMessageManager::NmApiMessageManager(quint64 mailboxId,QObject *parent)
	:QObject(parent)
{
	d = new NmApiMessageManagerPrivate(mailboxId,this);	
	connect(d, SIGNAL(messagesCopied(int)),this,SIGNAL(messagesCopied(int)));
	connect(d, SIGNAL(messagesCreated(int)),this,SIGNAL(messagesCreated(int)));
	connect(d, SIGNAL(messagesMoved(int)),this,SIGNAL(messagesMoved(int)));
	connect(d, SIGNAL(messagesDeleted(int)),this,SIGNAL(messagesDeleted(int)));	
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
 bool NmApiMessageManager::createForwardMessage(const QVariant *initData)
 {
    NM_FUNCTION;
    Q_UNUSED(initData);
	return false;
 }
    
    // creates reply message
    // signaled with replyMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
bool NmApiMessageManager::createReplyMessage(const QVariant *initData,
													bool replyAll)
{
    NM_FUNCTION;
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
	return d->moveMessages(messageIds,sourceFolderId,targetFolderId);
}

/*!
 \fn copyMessages 
 \param messageIds Id list of source messages.
 \param sourceFolder Id of the source folder.
 \param targetFolder Id of the target folder.
 \return true if operation was successfully started.
 
 Starts async copy operation for given messages.  
 Completion signalled with messagesCopied(int result).
 */
bool NmApiMessageManager::copyMessages(const QList<quint64> messageIds,								
									quint64 sourceFolder,
									quint64 targetFolder)
{
	return d->copyMessages(messageIds, sourceFolder, targetFolder);
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
bool NmApiMessageManager::fetch(const NmApiMessage &message)
{
    Q_UNUSED(message);
	return false;
}
    
    // moves message to outbox. Actual sending time may be immediate or scheduled
    // signaled with messageSent(quint64 messageId, int result)
bool NmApiMessageManager::send(const NmApiMessage &message)
{
    Q_UNUSED(message);
	return false;
}

// creates new attachment for a message. Currently attachment can be specified as file name (attachmentSpec is QString)
// signaled with attachmentCreated(quint64 attachemntId)
//  * 
bool NmApiMessageManager::createAttachment(NmApiEmailMessage &message,
										const QVariant &attachmenSpec)
{
    Q_UNUSED(message);
    Q_UNUSED(attachmenSpec);
    return false;
}
                                                      
// removes attachment from a message
// signaled with attachmentRemoved(int result)
bool NmApiMessageManager::removeAttachment(NmApiEmailMessage &message,
quint64 /*attachmentId*/)
{    
    Q_UNUSED(message);
    return false;
}
 
} // namespace EmailClientApi


