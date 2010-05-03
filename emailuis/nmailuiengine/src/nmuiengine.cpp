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


#include "nmuiengineheaders.h"

/*!
    \class NmUiEngine
    \brief NmUiEngine provides a controller-type services for the email UI.

*/

NmUiEngine *NmUiEngine::mInstance;
int NmUiEngine::mReferenceCount;

/*!
    Constructor
*/
NmUiEngine::NmUiEngine() 
:mMailboxListModel(NULL),
mMessageListModel(NULL),
mSendOperation(NULL)
{
    mPluginFactory = NmDataPluginFactory::instance();
    mDataManager = new NmDataManager();
}


/*!
    Destructor
*/
NmUiEngine::~NmUiEngine()
{
    if (mMessageListModel) {
        delete mMessageListModel;
        mMessageListModel = NULL;
    }
    if (mMailboxListModel) {
        delete mMailboxListModel;
        mMailboxListModel = NULL;
    }
    NmDataPluginFactory::releaseInstance(mPluginFactory);
    delete mDataManager;
    
    while (!mOperations.isEmpty()) {
        delete mOperations.takeLast();
    }
    
    delete mSendOperation;
}

/*!

*/
NmUiEngine *NmUiEngine::instance()
{
    if (!mInstance) {
    	mInstance = new NmUiEngine();
    }
    mReferenceCount++;
    return mInstance;
}

/*!

*/
void NmUiEngine::releaseInstance(NmUiEngine *&instance)
{
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if(instance == mInstance) {
            instance = NULL;
            mReferenceCount--;
        }
        if (0 >= mReferenceCount) {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

/*!
    Returns a list model populated with the mailbox and top level folder objects. The model is
    updated dynamically. The ownership of the model object is not moved to the caller.
*/
NmMailboxListModel &NmUiEngine::mailboxListModel()
{
    if (!mMailboxListModel) {
        refreshMailboxListModel(); // creates the model too
    }
    
    return *mMailboxListModel;
}
        
/*!
    Populate the list model with the mailbox and top level folder objects. The model is
    updated dynamically. Creates the model if needed.
*/
void NmUiEngine::refreshMailboxListModel()
{
    if (!mMailboxListModel) {
        mMailboxListModel = new NmMailboxListModel(*mDataManager);
        
        // Connect the model to the plugins to receive change notifications
        QList<QObject*> *dataPlugins = mPluginFactory->pluginInstances();
        for (int i = 0; i < dataPlugins->count(); i++) {
            QObject *plugin = (*dataPlugins)[i];
            if (plugin) {
                connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                    mMailboxListModel, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)));
            }
        }
    } else {
        mMailboxListModel->clear();
    }
    
    QList<NmMailbox*> mailboxList;
    mDataManager->listMailboxes(mailboxList);
    mMailboxListModel->refresh(mailboxList);
    while (!mailboxList.isEmpty()) {
        delete mailboxList.takeFirst();
    }
}

/*!
    Returns a message list model for a folder identified by \a mailboxId and \a folderId.
    The model is updated dynamically. The ownership of the model object is not moved to the caller.
*/
NmMessageListModel &NmUiEngine::messageListModel(const NmId &mailboxId, const NmId &folderId)
{
    if (!mMessageListModel){
        mMessageListModel = new NmMessageListModel(*mDataManager);
    }
    else{
        mMessageListModel->clear();
    }
    QObject *plugin =
        mPluginFactory->pluginInstance(mailboxId);
    if (plugin) {
        QObject::connect(plugin,
            SIGNAL(messageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
        mMessageListModel,
            SLOT(handleMessageEvent(NmMessageEvent, const NmId &, const QList<NmId> &)),
            Qt::UniqueConnection );

        QObject::connect(
            plugin, SIGNAL(syncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
            this, SLOT(handleSyncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
            Qt::UniqueConnection);
        
        QObject::connect(plugin,
            SIGNAL(connectionEvent(NmConnectState, const NmId &)),
            this,
            SIGNAL(connectionEvent(NmConnectState, const NmId &)),
            Qt::UniqueConnection);
        
        // Start to listen mailbox events
        NmDataPluginInterface *pluginInterface = mPluginFactory->interfaceInstance(plugin);
        if (pluginInterface) {
            pluginInterface->subscribeMailboxEvents(mailboxId);
        }
    }
    QList<NmMessageEnvelope*> messageEnvelopeList;
    mDataManager->listMessages(mailboxId, folderId, messageEnvelopeList);
    mMessageListModel->refresh(mailboxId, folderId, messageEnvelopeList);
    while (!messageEnvelopeList.isEmpty()) {
        delete messageEnvelopeList.takeFirst();
    }
    return *mMessageListModel;
}

/*!
    
*/
void NmUiEngine::releaseMessageListModel(const NmId &mailboxId)
{
    // Stop listening mailbox events
    NmDataPluginInterface *pluginInterface = mPluginFactory->interfaceInstance(mailboxId);
    if (pluginInterface) {
        pluginInterface->unsubscribeMailboxEvents(mailboxId);
    }
}

/*!
    Get the identifier of the standard folder of a type \a folderType 
    from the mailbox \a mailboxId.
*/
NmId NmUiEngine::standardFolderId(
    const NmId &mailboxId,
    NmFolderType folderType)
{
    NmId value;
    if (folderType != NmFolderOther) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
        if(plugin) {
            value = plugin->getStandardFolderId(mailboxId, folderType);
        }
    }
    return value;
}

/*!
    Returns a message identified by \a mailboxId,  \a folderId and \amessageId.
    The ownership of the  object is moved to the caller.
    Returns null pointer if the message is not found.
*/
NmMessage *NmUiEngine::message(const NmId &mailboxId,
                      const NmId &folderId,
                      const NmId &messageId)
{
    NmMessage *message = mDataManager->message(mailboxId, folderId, messageId);
    return message;
}

/*!

*/
NmOperation *NmUiEngine::fetchMessage( const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId )
{
    NmOperation *value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessage(mailboxId, folderId, messageId);
    }
    return value;
}

/*!

*/
NmOperation *NmUiEngine::fetchMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const NmId &messagePartId)
{
    NmOperation *value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessagePart(mailboxId, folderId, messageId, messagePartId);
    }
    return value;
}

/*!

*/
XQSharableFile NmUiEngine::messagePartFile(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->messagePartFile(mailboxId, folderId, messageId, messagePartId);
    }
    else {
        // empty file handle
        return XQSharableFile();
    }
}
/*!
    Get content to message part
*/
int NmUiEngine::contentToMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    return mDataManager->contentToMessagePart(mailboxId, folderId, messageId, messagePart);
}

/*!
    Deletes messages from model and routes call to plugin
*/
int NmUiEngine::deleteMessages(
	const NmId &mailboxId,
	const NmId &folderId,
	const QList<NmId> &messageIdList)
{
    int result(NmNotFoundError);
	if (mMessageListModel) {
	    mMessageListModel->handleMessageEvent(NmMessageDeleted, folderId, messageIdList);
	}
    NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
          result = plugin->deleteMessages(
	            mailboxId, folderId, messageIdList);
    }
	return result;
}

/*!
    Sets envelope property given in argument.
    Ownership of operation object is transferred to the caller.
*/

NmStoreEnvelopesOperation *NmUiEngine::setEnvelopes(
        const NmId &mailboxId,
        const NmId &folderId,
        NmEnvelopeProperties property,
        const QList<const NmMessageEnvelope*> &envelopeList)
{
    NMLOG("NmUiEngine::setEnvelopes() <---");
    NmStoreEnvelopesOperation *operation(NULL);
    if (mMessageListModel && envelopeList.count()) {
        QList<NmId> messageIdList;
        
        for (int i(0); i < envelopeList.count(); i++){
            messageIdList.append(envelopeList[i]->id());
        }
        mMessageListModel->setEnvelopeProperties(
                           property, messageIdList);
        // Store new envelopes to plugin
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
        if (plugin) {
            operation = plugin->storeEnvelopes(
                    mailboxId, folderId, envelopeList);
        }
        messageIdList.clear();
    }
    
    NMLOG("NmUiEngine::setEnvelopes() --->");
    return operation;
}


/*!
    Returns a mailbox meta data object from model with the ID \a mailboxId.
    Ownership of the return value is not moved to the caller.
    Returns NULL if the mailbox is not found.
*/
NmMailboxMetaData *NmUiEngine::mailboxById(const NmId &mailboxId)
{
    NmMailboxMetaData *meta(NULL);
    if (mMailboxListModel) {
	    for (int i(0); i < mMailboxListModel->rowCount(); i++) {
	        QModelIndex index = mMailboxListModel->index(i,0);
	        NmMailboxMetaData *mailbox =
	            mMailboxListModel->data(index, Qt::DisplayRole).value<NmMailboxMetaData*>();
	        if (mailbox && mailbox->id() == mailboxId) {
	            meta = mailbox;
	            break;
	        }
	    }
    }
    return meta;
}


/*!
    Creates a new message (into Drafts-folder).
    Ownership of operation object is transferred to the caller.
*/
NmMessageCreationOperation *NmUiEngine::createNewMessage(const NmId &mailboxId)
{
    NmMessageCreationOperation *value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createNewMessage(mailboxId);
    }
    return value;
}

/*!
    Creates a new forward message (into Drafts-folder).
    Ownership of operation object is transferred to the caller.
*/
NmMessageCreationOperation *NmUiEngine::createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId)
{
    NmMessageCreationOperation *value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createForwardMessage(mailboxId, originalMessageId);
    }
    return value;
}

/*!
    Creates a new reply message (into Drafts-folder).
    Ownership of operation object is transferred to the caller.
*/
NmMessageCreationOperation *NmUiEngine::createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        bool replyAll)
{
    NmMessageCreationOperation *value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createReplyMessage(mailboxId, originalMessageId, replyAll);
    }
    return value;
}

/*!
    Saves a message (into message store).
*/
int NmUiEngine::saveMessage(const NmMessage &message)
{
    const NmId &mailboxId = message.mailboxId();
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->saveMessage(message);
    }
    return ret;
}

/*!
    Refreshes mailbox.
*/
int NmUiEngine::refreshMailbox(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->refreshMailbox(mailboxId);
    }
    return ret;
}

/*!
    Online mailbox.
*/
int NmUiEngine::goOnline(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->goOnline(mailboxId);
    }
    return ret;
}

/*!
    Offline mailbox.
*/
int NmUiEngine::goOffline(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->goOffline(mailboxId);
    }
    return ret;
}

/*!
    Removes message from given mailbox and folder
    - routes call to plugin
*/
int NmUiEngine::removeMessage(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId)
{
    int result(NmNotFoundError);
    NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
          result = plugin->removeMessage(mailboxId, folderId, messageId);
    }
    return result;
}

/*!
    Takes ownership of an operation and connects to 
    it's completion signal
 */
void NmUiEngine::storeOperation(NmOperation *op)
{
    mOperations.append(op);
    connect(
            op, 
            SIGNAL(operationCompleted(int)), 
            this, 
            SLOT(handleCompletedOperation()));
}

/*!
    Sends the given message.
 */
void NmUiEngine::sendMessage(NmMessage *message, const QList<NmOperation *> &preliminaryOperations)
{
    //First trigger message storing
    if (message) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(message->mailboxId());
        
        if (plugin) {
            // to be on the safer side:
            // we shouldn't even be here if mSendOperation != NULL
            delete mSendOperation;
            mSendOperation = NULL;
            // ownership of message changes
            mSendOperation = plugin->sendMessage(message);
            // don't put this to mOperations as we need to handle this
            // operation separately
            if (mSendOperation) {
                foreach (NmOperation *op, preliminaryOperations) {
                    // ownership is transferred
                    mSendOperation->addPreliminaryOperation(op);
                }
                
                connect(mSendOperation, 
                        SIGNAL(operationCompleted(int)), 
                        this, 
                        SLOT(handleCompletedSendOperation()));
            }
        }
    }
}

/*!
    Is sending operation in progress.
 */
bool NmUiEngine::isSendingMessage() const
{
    int ret(false);
    if (mSendOperation) {
        ret = true;
    }
    return ret;
}

/*!
   Returns a pointer to the message that is being sent. Returns NULL if not sending.
 */
const NmMessage *NmUiEngine::messageBeingSent() const
{
    const NmMessage *message = NULL;
    
    if (mSendOperation != NULL) {
        message = mSendOperation->getMessage();
    }
    
    return message;
}

/*!
    Add file attachment into given message. Return the operation object for
    observing/cancelling. Ownership of operation object is transferred to the caller.
 */
NmAddAttachmentsOperation *NmUiEngine::addAttachments(
    const NmMessage &message,
    const QList<QString> &fileList)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.mailboxId());
    
    NmAddAttachmentsOperation *ret(NULL);
    
    if (plugin) {
        ret = plugin->addAttachments(message, fileList);
    }

    return ret;
}

/*!
    Remove attached file from given message. Return the operation object for
    observing/cancelling. Ownership of operation object is transferred to the caller.
 */
NmOperation *NmUiEngine::removeAttachment(
    const NmMessage &message,
    const NmId &attachmentPartId)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.mailboxId());
    
    NmOperation *ret(NULL);
    
    if (plugin) {
        ret = plugin->removeAttachment(message, attachmentPartId);
    }

    return ret;
}

/*!
    Ownership of operation object is transferred to the caller.
 */
NmCheckOutboxOperation *NmUiEngine::checkOutbox(const NmId &mailboxId)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);

    NmCheckOutboxOperation *ret(NULL);
    
    if (plugin) {
        ret = plugin->checkOutbox(mailboxId);
    }
    
    return ret;
}

/*!
    Returns the current sync state of the mailbox
 */
NmSyncState NmUiEngine::syncState(const NmId& mailboxId)
{
    NmDataPluginInterface *plugin =
                mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->syncState(mailboxId);
    }
    else {
        return SyncComplete;
    }
}

/*!
    Returns the current connection state of the mailbox
 */
NmConnectState NmUiEngine::connectionState(const NmId& mailboxId)
{
    NmDataPluginInterface *plugin =
                mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->connectionState(mailboxId);
    }
    else {
        return Disconnected;
    }
}

/*!
 * deletes completed operations
 * 
 */
void NmUiEngine::handleCompletedOperation()
{
    NMLOG("NmUiEngine::handleCompletedOperation() <---");
    int count = mOperations.count();
    
    QObject *sender = this->sender();
    
    for(int i(0); i < count; i++){
        if (mOperations[i] == sender){
            delete mOperations.takeAt(i);
            }
        }
    NMLOG("NmUiEngine::handleCompletedOperation() --->");
}
    
/*!
    Handle completed send operation.
*/
void NmUiEngine::handleCompletedSendOperation()
{
    // Let the callback method finish until cleaning the operation.
    QTimer::singleShot(1, this, SLOT(cleanupSendOperation()));
    emit sendOperationCompleted();
}

/*!
    Cleanup the send operation
*/
void NmUiEngine::cleanupSendOperation()
{
    delete mSendOperation;
    mSendOperation = NULL;
    // delete the sent messages from the store if necessary
    // ...
}

/*!
    Handles synch operation related events
 */
void NmUiEngine::handleSyncStateEvent(NmSyncState syncState, const NmOperationCompletionEvent &event)
{
    NMLOG("NmUiEngine::handleSyncStateEvent()");

    if ( syncState == SyncComplete ) {
        // signal for reporting about (sync) operation completion status
        emit operationCompleted(event);
    }

    // signal for handling sync state icons
    emit syncStateEvent(syncState, event.mMailboxId);
}


