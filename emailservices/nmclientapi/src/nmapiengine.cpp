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


#include "nmapitypesconverter.h"
#include "nmapidataplugininterface.h"
#include "nmapidatapluginfactory.h"
#include "nmapiengine.h"


NmEngine *NmEngine::mInstance = NULL;
quint32 NmEngine::mReferenceCount = 0;

/*!
 \class NmEngine
 \brief The NmEngine class provides access to data for
 mailboxes, folders and messages. If the dataplugin can't be loaded, fatal assert is called.

 */

/*!
 Private constructor
 */
NmEngine::NmEngine() :
    mFactory(NULL)
{
    mFactory = NmDataPluginFactory::instance();
    Q_ASSERT(mFactory->pluginInstances()->count() > 0);
}

/*!
 Destructor
 */
NmEngine::~NmEngine()
{
    NmDataPluginFactory::releaseInstance(mFactory);
}

/*!
 * Creates instance of NmEngine and prvide pointer to it.
 * 
 * If engine exist, it increase references count and provide pointer to it.
 * 
 * \return Instance of engine
 */
NmEngine* NmEngine::instance()
{
    if (!mInstance) {
        mInstance = new NmEngine();
    }
    mReferenceCount++;
    return mInstance;
}

/*!
 * Creates connections for events from email store.
 */
void NmEngine::startCollectingEvents()
{
    QList<QObject*>* pluginList = mFactory->pluginInstances();
    for (int i = 0; i < pluginList->count(); i++) {
        QObject* plugin = pluginList->at(i);

        QObject::connect(plugin, SIGNAL(messageEvent(NmMessageEvent, NmId, QList<NmId> , NmId)),
            this, SLOT(messageChangedArrived(NmMessageEvent, NmId, QList<NmId> , NmId)),
            Qt::UniqueConnection);

        connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, QList<NmId> )), this, SLOT(
            mailboxChangedArrived(NmMailboxEvent, QList<NmId> )), Qt::UniqueConnection);

    }

}

/*!
 * It process mailbox ids from email store for given event.
 * On end it emit \sa emailStoreEvent
 * 
 * \arg Event of mailbox change
 * \arg List of mailbox ids for that event 
 */
void NmEngine::mailboxChangedArrived(NmMailboxEvent mailboxEvent, const QList<NmId> &mailboxIds)
{
    NmApiMessage message;
    message.objectType = EMailbox;
    switch (mailboxEvent) {
        case NmMailboxCreated: {
            message.action = ENew;
        }
            break;
        case NmMailboxDeleted: {
            message.action = EDeleted;
        }
            break;
        case NmMailboxChanged: {
            message.action = EChange;
        }
            break;
        default:
            break;
    }

    message.folderId = 0;
    message.mailboxId = 0;

    for (int i = 0; i < mailboxIds.count(); i++) {
        message.objectIds.append(mailboxIds.at(i).id());
    }

    emit emailStoreEvent(message);
}

/*!
 * It process message ids from email store for given event.
 * On end it emit \sa emailStoreEvent
 * 
 * \arg Event of message change
 * \arg List of message ids for that event 
 */
void NmEngine::messageChangedArrived(
    NmMessageEvent messageEvent,
    const NmId &folderId,
    const QList<NmId> &messageIds,
    const NmId &mailboxId)
{
    NmApiMessage message;
    message.objectType = EMessage;
    switch (messageEvent) {
        case NmMessageCreated: {
            message.action = ENew;
        }
            break;
        case NmMessageDeleted: {
            message.action = EDeleted;
        }
            break;
        case NmMessageChanged: {
            message.action = EChange;
        }
            break;
        default:
            break;
    }

    message.folderId = folderId.id();
    message.mailboxId = mailboxId.id();

    for (int i = 0; i < messageIds.count(); i++) {
        message.objectIds.append(messageIds.at(i).id());
    }

    emit emailStoreEvent(message);
}

/*!
 * Release instance of engine if there are not referenced objects. 
 * If there are refenced objects it only decrease refenced count.
 * 
 * \arg Instance of engine
 */
void NmEngine::releaseInstance(NmEngine *&instance)
{
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if (instance == mInstance) {
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
 *    It get all mailboxes from email store
 *    
 *    \sa EmailClientApi::NmMailbox
 *    \arg List of mailboxes to be filled.
 */
void NmEngine::listMailboxes(QList<EmailClientApi::NmMailbox> &mailboxList)
{
    QList<NmMailbox*> mailboxFromPlugin;
    int count = mFactory->pluginInstances()->count();
    for (int i = 0; i < count; i++) {
        NmDataPluginInterface *instance = mFactory->interfaceInstance(
            mFactory->pluginInstances()->at(i));
        if (instance) {
            instance->listMailboxes(mailboxFromPlugin);
        }
    }
    while (mailboxFromPlugin.isEmpty() == false) {
        NmMailbox* tempNmMailbox = mailboxFromPlugin.takeFirst();
        mailboxList << NmToApiConverter::NmMailbox2ApiNmMailbox(*tempNmMailbox);
        delete tempNmMailbox;
    }
}

/*!
 *    It get all folders from email store for given mailbox
 *    
 *    \sa EmailClientApi::NmFolder
 *    \arg Mailbox id from where folders should be returned
 *    \arg List of folders to be filled.
 */
void NmEngine::listFolders(const quint64 mailboxId, QList<EmailClientApi::NmFolder> &folderList)
{
    QList<NmFolder*> folderFromPlugin;
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);
    if (instance) {
        instance->listFolders(mailboxId, folderFromPlugin);
    }

    while (folderFromPlugin.isEmpty() == false) {
        NmFolder* tempNmFolder = folderFromPlugin.takeFirst();
        folderList << NmToApiConverter::NmFolder2ApiNmFolder(*tempNmFolder);
        delete tempNmFolder;
    }
}

/*!
 *    It get all envelopes from email store for given mailbox and folder
 *    
 *    \sa EmailClientApi::NmMessageEnvelope
 *    \arg Mailbox id from where envelope should be returned
 *    \arg Folder id from where envelope should be returned
 *    \arg List of envelopes to be filled.
 */
void NmEngine::listEnvelopes(const quint64 mailboxId, const quint64 folderId, QList<
    EmailClientApi::NmMessageEnvelope> &messageEnvelopeList)
{
    QList<NmMessageEnvelope*> envelopes;
    NmDataPluginInterface *instance = mFactory->interfaceInstance(folderId);
    if (instance) {
        instance->listMessages(mailboxId, folderId, envelopes);
    }

    while (!envelopes.isEmpty()) {
        NmMessageEnvelope* env = envelopes.takeFirst();

        EmailClientApi::NmMessageEnvelope nmEnvelope =
            NmToApiConverter::NmMessageEnvelope2ApiEnvelope(*env);
        NmMessage* message = new NmMessage();
        instance->getMessageById(mailboxId, folderId, nmEnvelope.id(), message);

        QString plainText = QString();
        if (message->plainTextBodyPart()) {
            plainText = message->plainTextBodyPart()->textContent();
        }

        nmEnvelope.setPlainText(plainText);
        nmEnvelope.setFetchedSize(message->fetchedSize());
        nmEnvelope.setTotalSize(message->size());

        messageEnvelopeList << nmEnvelope;
        delete env;
    }
}

/*!
 * Return envelope given by mailbox, folder and envelope id.
 * 
 * \arg Mailbox id from where envlope should come
 * \arg Folder id from where envlope should come
 * \arg Id of envelope which should be returned
 * \arg Envelope to fill.
 * 
 * \return Return true if it will find any envelope
 */
bool NmEngine::envelopeById(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    EmailClientApi::NmMessageEnvelope& envelope)
{
    QList<EmailClientApi::NmMessageEnvelope> envelopes;
    listEnvelopes(mailboxId, folderId, envelopes);

    //flag indicating that envelope with given id was found
    bool found = false;

    for (int i = 0; i < envelopes.count(); i++) {
        if (envelopes.at(i).id() == envelopeId) {
            envelope = envelopes.at(i);
            found = true;
            break;
        }
    }

    return found ? true : false;
}

/*!
 * Return mailbox given by mailbox id.
 * 
 * \arg Id of Mailbox which should be returned
 * \arg Mailbox to fill.
 * 
 * \return Return true if it will find any envelope
 */
bool NmEngine::mailboxById(const quint64 mailboxId, EmailClientApi::NmMailbox &mailbox)
{
    NmDataPluginInterface *instance = mFactory->interfaceInstance(mailboxId);

    if (!instance) {
        return false;
    }

    NmMailbox *nmmailbox;
    instance->getMailboxById(NmId(mailboxId), nmmailbox);

    if (!nmmailbox) {
        return false;
    }

    mailbox = NmToApiConverter::NmMailbox2ApiNmMailbox(*nmmailbox);

    return true;

}

