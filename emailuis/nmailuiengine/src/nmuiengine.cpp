/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
: mMailboxListModel(NULL),
  mMessageListModel(NULL),
  mMessageSearchListModel(NULL),
  mSendOperation(NULL)
{
    mPluginFactory = NmDataPluginFactory::instance();
    mDataManager = new NmDataManager();
    // Connect to the plugins to receive change notifications
    QList<QObject*> *dataPlugins = mPluginFactory->pluginInstances();
    for (int i(0); i < dataPlugins->count(); i++) {
        QObject *plugin = (*dataPlugins)[i];
        if (plugin) {
            // connet mailbox events
            QObject::connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                 this, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                 Qt::UniqueConnection);
            // connect message events
            QObject::connect(plugin, 
                SIGNAL(messageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
                this, SLOT(handleMessageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
                Qt::UniqueConnection);
            // connect connection events
            QObject::connect(plugin,
                SIGNAL(connectionEvent(NmConnectState, const NmId &, const int)),
                this,
                SLOT(handleConnectEvent(NmConnectState, const NmId &, const int)),
                Qt::UniqueConnection);       
            // do the subscriptions also
            NmDataPluginInterface *pluginInterface = mPluginFactory->interfaceInstance(plugin);
            if (pluginInterface) {
                QList<NmId> mailboxIdList;
                pluginInterface->listMailboxIds(mailboxIdList);
                for (int j(0); j < mailboxIdList.count(); j++) {
                    pluginInterface->subscribeMailboxEvents(mailboxIdList[j]);
                }
                mailboxIdList.clear();
            }
        }
    }
}


/*!
    Destructor
*/
NmUiEngine::~NmUiEngine()
{
    if (mMessageSearchListModel) {
        delete mMessageSearchListModel;
        mMessageSearchListModel = NULL;
    }

    if (mMessageListModel) {
        delete mMessageListModel;
        mMessageListModel = NULL;
    }
    if (mMailboxListModel) {
        delete mMailboxListModel;
        mMailboxListModel = NULL;
    }
    // do the unsubscriptions
    QList<NmId> mailboxIdList;
    mDataManager->listMailboxIds(mailboxIdList);
    for (int i(0); i < mailboxIdList.count(); i++) {
        NmId id = mailboxIdList[i];
        NmDataPluginInterface *pluginInterface = mPluginFactory->interfaceInstance(id);
        if (pluginInterface) {
            pluginInterface->unsubscribeMailboxEvents(id);
        }
    }
    mailboxIdList.clear();
    NmDataPluginFactory::releaseInstance(mPluginFactory);
    delete mDataManager;
    if (mSendOperation && mSendOperation->isRunning()) {
        mSendOperation->cancelOperation();
    }
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
        // no need for mailbox event subscription here, already done in constructor
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
    Returns a reference of the message search list model. If the model does not
    exist yet, one is constructed.

    \param sourceModel The source model for the search list model.

    \return The message search list model.
*/
NmMessageSearchListModel &NmUiEngine::messageSearchListModel(
    QAbstractItemModel *sourceModel)
{
    if (!mMessageSearchListModel) {
        mMessageSearchListModel = new NmMessageSearchListModel();
    }

    mMessageSearchListModel->setSourceModel(sourceModel);
    return *mMessageSearchListModel;
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
QPointer<NmOperation> NmUiEngine::fetchMessage( const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId )
{
    NMLOG("NmUiEngine::fetchMessage() <---");
    QPointer<NmOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessage(mailboxId, folderId, messageId);
    }
    return value;
}

/*!

*/
QPointer<NmOperation> NmUiEngine::fetchMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const NmId &messagePartId)
{
    NMLOG("NmUiEngine::fetchMessagePart() <---");
    QPointer<NmOperation> value(NULL);
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
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmStoreEnvelopesOperation> NmUiEngine::setEnvelopes(
        const NmId &mailboxId,
        const NmId &folderId,
        NmEnvelopeProperties property,
        const QList<const NmMessageEnvelope*> &envelopeList)
{
    NMLOG("NmUiEngine::setEnvelopes() <---");
    QPointer<NmStoreEnvelopesOperation> operation(NULL);
    if (mMessageListModel && envelopeList.count()) {
        QList<NmId> messageIdList;
        
        for (int i(0); i < envelopeList.count(); i++){
            messageIdList.append(envelopeList[i]->messageId());
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
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createNewMessage(const NmId &mailboxId)
{
    NMLOG("NmUiEngine::createNewMessage() <---");
    QPointer<NmMessageCreationOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createNewMessage(mailboxId);
    }
    return value;
}

/*!
    Creates a new forward message (into Drafts-folder).
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId)
{
    NMLOG("NmUiEngine::createForwardMessage() <---");
    QPointer<NmMessageCreationOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createForwardMessage(mailboxId, originalMessageId);
    }
    return value;
}

/*!
    Creates a new reply message (into Drafts-folder).
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        bool replyAll)
{
    NMLOG("NmUiEngine::createReplyMessage() <---");
    QPointer<NmMessageCreationOperation> value(NULL);
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
    const NmId &mailboxId = message.envelope().mailboxId();
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
    Sends the given message.
 */
void NmUiEngine::sendMessage(NmMessage *message, const QList<NmOperation *> &preliminaryOperations)
{
    //First trigger message storing
    if (message) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(message->envelope().mailboxId());
        
        if (plugin) {
            // to be on the safer side:
            // we shouldn't even be here if mSendOperation != NULL
            if (mSendOperation && mSendOperation->isRunning()) {
                mSendOperation->cancelOperation();
            }
            // ownership of message changes
            mSendOperation = plugin->sendMessage(message);
            // don't put this to mOperations as we need to handle this
            // operation separately
            if (mSendOperation) {
                for (int i(0); i < preliminaryOperations.count(); i++ ) {
                    QPointer<NmOperation> op = preliminaryOperations[i];
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
    if (mSendOperation && mSendOperation->isRunning()) {
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
    observing/cancelling. Operation is automatically deleted after completion or cancelling.
 */
QPointer<NmAddAttachmentsOperation> NmUiEngine::addAttachments(
    const NmMessage &message,
    const QList<QString> &fileList)
{
    NMLOG("NmUiEngine::addAttachments() <---");
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.envelope().mailboxId());
    
    QPointer<NmAddAttachmentsOperation> ret(NULL);    
    if (plugin) {
        ret = plugin->addAttachments(message, fileList);
    }
    return ret;
}

/*!
    Remove attached file from given message. Return the operation object for
    observing/cancelling. Operation is automatically deleted after completion or cancelling.
 */
QPointer<NmOperation> NmUiEngine::removeAttachment(
    const NmMessage &message,
    const NmId &attachmentPartId)
{
    NMLOG("NmUiEngine::removeAttachments() <---");
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.envelope().mailboxId());
    
    QPointer<NmOperation> ret(NULL);   
    if (plugin) {
        ret = plugin->removeAttachment(message, attachmentPartId);
    }
    return ret;
}

/*!
    Operation is automatically deleted after completion or cancelling.
 */
QPointer<NmCheckOutboxOperation> NmUiEngine::checkOutbox(const NmId &mailboxId)
{
    NMLOG("NmUiEngine::checkOutbox() <---");
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);

    QPointer<NmCheckOutboxOperation> ret(NULL); 
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
    Starts the search.

    \param mailboxId The ID of the mailbox to search from.
    \param searchStrings The strings to search with.

    \return A possible error code.
*/
int NmUiEngine::search(const NmId &mailboxId,
                       const QStringList &searchStrings)
{
    // Get the plugin instance.
    QObject *pluginInstance = mPluginFactory->pluginInstance(mailboxId);

    if (pluginInstance) {
        // Make sure the required signals are connected.
        connect(pluginInstance, SIGNAL(matchFound(const NmId &)),
                this, SIGNAL(matchFound(const NmId &)), Qt::UniqueConnection);    
        connect(pluginInstance, SIGNAL(matchFound(const NmId &)),
                mMessageSearchListModel, SLOT(addSearchResult(const NmId &)),
                Qt::UniqueConnection);    
        connect(pluginInstance, SIGNAL(searchComplete()),
                this, SIGNAL(searchComplete()), Qt::UniqueConnection);    
    }

    int retVal(NmNoError);

    // Get the plugin interface.
    NmDataPluginInterface *pluginInterface =
        mPluginFactory->interfaceInstance(mailboxId);

    if (pluginInterface) {
        // Start the search.
        retVal = pluginInterface->search(mailboxId, searchStrings);
    }
    
    return retVal;
}


/*!
    Cancels the search operation if one is ongoing.

    \param mailboxId The ID of the mailbox running the search.

    \return A possible error code.
*/
int NmUiEngine::cancelSearch(const NmId &mailboxId)
{
    int retVal(NmNoError);

    // Get the plugin interface.
    NmDataPluginInterface *pluginInterface =
        mPluginFactory->interfaceInstance(mailboxId);

    if (pluginInterface) {
        // Cancel the search.
        retVal = pluginInterface->cancelSearch(mailboxId);
    }  
    return retVal;    
}

/*!
    Cancels the search operation if one is ongoing.

    \param mailboxId The ID of the mailbox containing the folder

    \param folderId The ID of the folder 

    \return Folder type
*/
NmFolderType NmUiEngine::folderTypeById(NmId mailboxId, NmId folderId)
{
    NmFolderType folderType(NmFolderOther);
    if (standardFolderId(mailboxId,NmFolderInbox)==folderId){
        folderType=NmFolderInbox;
    }
    else if (standardFolderId(mailboxId,NmFolderOutbox)==folderId){
        folderType=NmFolderOutbox; 
    }
    else if (standardFolderId(mailboxId,NmFolderDrafts)==folderId){
        folderType=NmFolderDrafts;
    }
    else if (standardFolderId(mailboxId,NmFolderSent)==folderId){
        folderType=NmFolderSent; 
    }    
    else if (standardFolderId(mailboxId,NmFolderDeleted)==folderId){
        folderType=NmFolderDeleted;  
    }    
    return folderType;
}
/*!
    Handle completed send operation.
*/
void NmUiEngine::handleCompletedSendOperation()
{
    NMLOG("NmUiEngine::handleCompletedSendOperation()");
    emit sendOperationCompleted();
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

/*!
    Emits signals based on message events coming from plugins. 
    Currently only NmMessageDeleted is handled.
*/
void NmUiEngine::handleMessageEvent(NmMessageEvent event,
                                    const NmId &folderId,
                                    const QList<NmId> &messageIds, 
                                    const NmId& mailboxId)
{
    switch (event) {
        case NmMessageDeleted:
        {
            for (int i(0); i < messageIds.count(); i++) {
                emit messageDeleted(mailboxId, folderId, messageIds[i]);
            }
           break; 
        }
        default:
        break;
    }
}

/*!
    Emits signals based on mailbox events coming from plugins. 
    Currently only NmMailboxDeleted is handled.
*/
void NmUiEngine::handleMailboxEvent(NmMailboxEvent event,
                                    const QList<NmId> &mailboxIds)
{
    switch (event) {
        case NmMailboxDeleted:
        {
            for (int i(0); i < mailboxIds.count(); i++) {
                emit mailboxDeleted(mailboxIds[i]);
            }
           break; 
        }
        default:
        break;
    }
}

/*!
    receives events when going online, and offline.
*/
void NmUiEngine::handleConnectEvent(NmConnectState connectState, const NmId &mailboxId, const int errorCode)
{
    // signal for connection state icon handling
    emit connectionEvent(connectState, mailboxId);

    // in case going offline w/ error, emit signal to UI
    if ( connectState == Disconnected && errorCode!= NmNoError ) {
        NmOperationCompletionEvent event={NoOp, errorCode, mailboxId, 0, 0}; 
        emit operationCompleted(event);
    }
}
