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


#include <nmapicommonheader.h>

#include "nmapitypesconverter.h"
#include "nmdataplugininterface.h"
#include "nmapidatapluginfactory.h"
#include "nmapiengine.h"

/*!
    from nmailbase
*/
#include "nmapiprivateheaders.h"

NmApiEngine *NmApiEngine::mInstance = NULL;
quint32 NmApiEngine::mReferenceCount = 0;

/*!
   \class NmApiEngine
   \brief The NmEngine class provides access to data for
   mailboxes, folders and messages.
 */

/*!
   Private constructor
 */
NmApiEngine::NmApiEngine() :
    mFactory(NULL)
{
    mFactory = NmApiDataPluginFactory::instance();
}

/*!
  Destructor
 */
NmApiEngine::~NmApiEngine()
{
    NmApiDataPluginFactory::releaseInstance(mFactory);
}

/*!
   Creates instance of NmApiEngine and provide pointer to it.
   
   If engine exist, it increase references count and provide pointer to it.
   
   \return Instance of engine
 */
NmApiEngine *NmApiEngine::instance()
{
    if (!mInstance) {
        mInstance = new NmApiEngine();
    }
    mReferenceCount++;
    return mInstance;
}

/*!
   Creates connections for events from email store.
 */
void NmApiEngine::startCollectingEvents()
{
    QObject *plugin = mFactory->plugin();
    if(plugin){
        connect(plugin, SIGNAL(messageEvent(NmMessageEvent, NmId, QList<NmId> , NmId)), this,
            SLOT(messageChangedArrived(NmMessageEvent, NmId, QList<NmId> , NmId)), Qt::UniqueConnection);

        connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, QList<NmId> )), this, SLOT(
            mailboxChangedArrived(NmMailboxEvent, QList<NmId> )), Qt::UniqueConnection);
    }
}

/*!
   It process mailbox ids from email store for given event.
   On end it emit \sa emailStoreEvent
   
   \arg Event of mailbox change
   \arg List of mailbox ids for that event 
 */
void NmApiEngine::mailboxChangedArrived(NmMailboxEvent mailboxEvent, const QList<NmId> &mailboxIds)
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
   It process message ids from email store for given event.
   On end it emit \sa emailStoreEvent
   
   \param messageEvent Event of message change
   \param folderId Id of folder from where are messages
   \param messageIds List of message ids for that event 
   \param mailboxId Id of mailbox from where are messages
 */
void NmApiEngine::messageChangedArrived(
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
   Release instance of engine if there are not referenced objects. 
   If there are refenced objects it only decrease refenced count.
   
   \param instance Instance of engine
 */
void NmApiEngine::releaseInstance(NmApiEngine *&instance)
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
      It get all mailboxes from email store
      
      \sa EmailClientApi::NmMailbox
      \param mailboxList List of mailboxes to be filled.
 */
void NmApiEngine::listMailboxes(QList<EmailClientApi::NmApiMailbox> &mailboxList)
{
    QList<NmMailbox*> mailboxFromPlugin;

    NmDataPluginInterface *instance =  mFactory->interfaceInstance();
    if (instance) {
        instance->listMailboxes(mailboxFromPlugin);
    }

    while (mailboxFromPlugin.isEmpty() == false) {
        NmMailbox* tempNmMailbox = mailboxFromPlugin.takeLast();
        mailboxList << NmToApiConverter::NmMailbox2NmApiMailbox(*tempNmMailbox);
        delete tempNmMailbox;
    }
}

/*!
      It get all folders from email store for given mailbox
      
      \sa EmailClientApi::NmApiFolder
      \param mailboxId Mailbox id from where folders should be returned
      \param folderList  of folders to be filled.
 */
void NmApiEngine::listFolders(const quint64 mailboxId, QList<EmailClientApi::NmApiFolder> &folderList)
{
    QList<NmFolder*> folderFromPlugin;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        instance->listFolders(mailboxId, folderFromPlugin);
    }

    while (folderFromPlugin.isEmpty() == false) {
        NmFolder* tempNmFolder = folderFromPlugin.takeLast();
        folderList << NmToApiConverter::NmFolder2NmApiFolder(*tempNmFolder);
        delete tempNmFolder;
    }
}

/*!
      It get all envelopes from email store for given mailbox and folder
      
      \sa EmailClientApi::NmApiMessageEnvelope
      \param mailboxId Mailbox id from where envelope should be returned
      \param folderId Folder id from where envelope should be returned
      \param messageEnvelopeList List of envelopes to be filled.
 */
void NmApiEngine::listEnvelopes(const quint64 mailboxId, const quint64 folderId, 
                    QList<EmailClientApi::NmApiMessageEnvelope> &messageEnvelopeList)
{
    QList<NmMessage*> messages;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        instance->listMessages(mailboxId, folderId, messages);
    }

    while (!messages.isEmpty()) {
        NmMessage* message = messages.takeFirst();
            
        EmailClientApi::NmApiMessageEnvelope nmEnvelope =
            NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(message->envelope());
        
        NmMessagePart *plainTextPart = message->plainTextBodyPart();
        
        QString plainText = QString();
        if (plainTextPart) {
            plainText = plainTextPart->textContent();
        }

        nmEnvelope.setPlainText(plainText);
        nmEnvelope.setFetchedSize(message->fetchedSize());
        nmEnvelope.setTotalSize(message->size());

        messageEnvelopeList << nmEnvelope;
        delete message;
    }
}

/*!
   Return envelope given by mailbox, folder and envelope id.
   
   \param mailboxId Mailbox id from where envlope should come
   \param folderId Folder id from where envlope should come
   \param folderId Id of envelope which should be returned
   \param envelope Envelope to fill.
   
   \return Return true if it will find any envelope
 */
bool NmApiEngine::getEnvelopeById(
    const quint64 mailboxId,
    const quint64 folderId,
    const quint64 envelopeId,
    EmailClientApi::NmApiMessageEnvelope &envelope)
{
    //flag indicating that envelope with given id was found
    bool found = false;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();
    if (instance) {
        NmMessage *message(NULL);
        instance->getMessageById(mailboxId, folderId, envelopeId, message);
        if(message){
            NmMessageEnvelope env = message->envelope();

            envelope = NmToApiConverter::NmMessageEnvelope2NmApiMessageEnvelope(env);
            QString plainText = QString();
            
            NmMessagePart *plainTextPart = message->plainTextBodyPart();
            if (plainTextPart) {
                instance->contentToMessagePart(mailboxId, folderId, envelopeId, *plainTextPart);
                plainText = plainTextPart->textContent();
            }

            envelope.setPlainText(plainText);
            envelope.setFetchedSize(message->fetchedSize());
            envelope.setTotalSize(message->size());

            found = true;
            delete plainTextPart;
        }
        delete message;
    }
    return found;
}

/*!
   Return mailbox given by mailbox id.
   
   \param mailboxId Id of Mailbox which should be returned
   \param mailbox Mailbox to fill.
   
   \return Return true if it will find any envelope
 */
bool NmApiEngine::getMailboxById(const quint64 mailboxId, EmailClientApi::NmApiMailbox &mailbox)
{
    //flag indicating that mailbox with given id was found
    bool found = false;
    NmDataPluginInterface *instance = mFactory->interfaceInstance();

    if (instance) {    
        NmMailbox *nmmailbox = NULL;
        instance->getMailboxById(NmId(mailboxId), nmmailbox);
    
        if (nmmailbox) {
            mailbox = NmToApiConverter::NmMailbox2NmApiMailbox(*nmmailbox);
            found = true;
            delete nmmailbox;
        }
    }
    
    return found;
}
