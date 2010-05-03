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
 *
 */

#include <QDebug>
#include <QTimer>
#include <QDir>
#include <qpluginloader.h>
#include <XQServiceRequest.h>
#include "email_services_api.h"
#include "nmcommon.h"
#include "nmmessageenvelope.h"
#include "nmhswidgetemailengine.h"
#include "nmdataplugininterface.h"
#include "nmmailbox.h"
#include "nmfolder.h"
#include "nmdatapluginfactory.h"

/*!
 Constructor
 */
NmHsWidgetEmailEngine::NmHsWidgetEmailEngine(const NmId& monitoredMailboxId) :
    mMailboxId(monitoredMailboxId), mAccountName(0), mUnreadCount(0),
            mEmailInterface(0), mFactory(0), mAccountEventReceivedWhenSuspended(false),
            mMessageEventReceivedWhenSuspended(false), mSuspended(false) 
    {
    qDebug()<<"NmHsWidgetEmailEngine() -- START";
    constructNmPlugin();
    updateData();
    updateAccount();
    qDebug()<<"NmHsWidgetEmailEngine() -- END";
    }


/*!
 constructNmPlugin
 */
void NmHsWidgetEmailEngine::constructNmPlugin()
    {
    qDebug()<<"NmHsWidgetEmailEngine::constructNmPlugin() -- START";
    
    QObject* pluginInstance(0); 
    //Get data plugin factory instance
    mFactory = NmDataPluginFactory::instance();

    if(mFactory)        
        {
        pluginInstance = mFactory->pluginInstance(mMailboxId);
        }

    if(pluginInstance)
        { 
        mEmailInterface = mFactory->interfaceInstance( pluginInstance );
        mFolderId = mEmailInterface->getStandardFolderId(mMailboxId, NmFolderInbox);
        //Subscription is needed - otherwise the signals will not be received
        mEmailInterface->subscribeMailboxEvents(mMailboxId);
        
        //Connect to events
        connect(pluginInstance,
                SIGNAL( messageEvent(NmMessageEvent, const NmId&, 
                        const QList<NmId>&, const NmId&) ),
                this,
                SLOT( handleMessageEvent(NmMessageEvent, const NmId&, 
                        const QList<NmId>&, const NmId&) ));

        connect(pluginInstance,
                SIGNAL( mailboxEvent(NmMailboxEvent, const QList<NmId>& ) ),
                this,
                SLOT( handleMailboxEvent(NmMailboxEvent, const QList<NmId>&) ));
        
        qDebug()<<"NmHsWidgetEmailEngine::constructNmPlugin() -- OK";
        }
    else
        {
        emit errorOccured(NmEngineErrFailure);
        qDebug()<<"NmHsWidgetEmailEngine::constructNmPlugin() -- FAILED";
        }

    qDebug()<<"NmHsWidgetEmailEngine::constructNmPlugin() -- END";
    }

/*!
 Reset envelope list
 \post mEnvelopeList.isEmpty() == true && all contained objects are deleted
 */
void NmHsWidgetEmailEngine::resetEnvelopeList()
    {
    qDebug()<<"NmHsWidgetEmailEngine::resetEnvelopeList() -- START";
    
    while (!mEnvelopeList.isEmpty())
        {
        delete mEnvelopeList.takeFirst();
        }
    
    qDebug()<<"NmHsWidgetEmailEngine::resetEnvelopeList() -- END";
    }

/*!
 Destructor
 */
NmHsWidgetEmailEngine::~NmHsWidgetEmailEngine()
    {
    qDebug()<<"~NmHsWidgetEmailEngine -- START";
 
    resetEnvelopeList();
    if  ( mFactory )
        {        
        NmDataPluginFactory::releaseInstance(mFactory);
        }
    qDebug()<<"~NmHsWidgetEmailEngine -- END";
    }


/*!
 getEnvelopes() provides message envelopes as a list of stack objects
 Amount of message envelopes in the list parameter is the smallest of the following factors:
 'KMaxNumberOfEnvelopesProvided', 'maxEnvelopeAmount', 'amount of available envelopes'. 
  
 \param list list to be filled with message envelopes
 \param maxEnvelopeAmount Client side limit for amount of message envelope count. 
 \return count of envelopes added to list
 */
int NmHsWidgetEmailEngine::getEnvelopes(QList<NmMessageEnvelope> &list,
        int maxEnvelopeAmount)
    {
    qDebug()<<"NmHsWidgetEmailEngine::getEnvelopes()";
    list.clear(); //Reset the parameter list to avoid side effects
    int i = 0;
    for (; i < mEnvelopeList.count() && i < maxEnvelopeAmount; i++)
        {
        NmMessageEnvelope env(*mEnvelopeList[i]);
        list.append(env);
        }
    return i;
    }

/*!
 UnreadCount

 \return count of unread mails
 */
int NmHsWidgetEmailEngine::unreadCount()
    {
    qDebug()<<"NmHsWidgetEmailEngine::unreadCount()";
    return mUnreadCount;
    }

/*!
 accountName

 \return name of the monitored account
 */
QString NmHsWidgetEmailEngine::accountName()
    {
    qDebug()<<"NmHsWidgetEmailEngine::accountName()";
    return mAccountName;
    }

/*!
 Refresh email data.
 \post mEnvelopeList is refreshed with valid content so that it has
 valid data with maximum of KMaxNumberOfEnvelopesProvided envelopes
 
 */
void NmHsWidgetEmailEngine::updateData()
    {
    qDebug()<<"NmHsWidgetEmailEngine::updateData() -- START";
    if(!mEmailInterface)
        {
        qDebug()<<"NmHsWidgetEmailEngine::updateData() -- Interface missing";
        return; //if interface is missing there's nothing to do
        }
    //reset envelope list before retrieving new items
    resetEnvelopeList(); 
    //get messages from inbox
    mEmailInterface->listMessages(mMailboxId, mFolderId, mEnvelopeList, KMaxNumberOfEnvelopesProvided);
    Q_ASSERT_X(mEnvelopeList.count() <= KMaxNumberOfEnvelopesProvided, "nmhswidgetemailengine", "list max size exeeded");
    //emit signal about new message data right away
    emit mailDataChanged();
    //retrieve new unread count to mUnreadCount
    NmFolder* folder = NULL;
    int err = mEmailInterface->getFolderById(mMailboxId, mFolderId, folder);
 
    if(folder)
        {
        mUnreadCount = folder->unreadMessageCount();
        delete folder;
        folder = NULL;
        //limit the unread count to KMaxUnreadCount
        if( mUnreadCount > KMaxUnreadCount)
            {
            mUnreadCount = KMaxUnreadCount;
            }
        //emit signal about changed unread count
        emit unreadCountChanged(mUnreadCount);
        }



    qDebug()<<"NmHsWidgetEmailEngine::updateData() -- END";
    }

/*!
 handleMessageEvent slot.
 */
void NmHsWidgetEmailEngine::handleMessageEvent(NmMessageEvent event,
        const NmId &folderId, const QList<NmId> &messageIds, const NmId& mailboxId)
    {
    qDebug()<<"NmHsWidgetEmailEngine::handleMessageEvent() -- START";
    Q_UNUSED(event);
    Q_UNUSED(messageIds);
    if ( (folderId == mFolderId) && (mailboxId == mMailboxId) )
        {
        //Data is updated only if the engine is not suspended
        if( mSuspended )
            {
            mMessageEventReceivedWhenSuspended = true;
            }
        else
            {
            updateData();
            }   
        }
    qDebug()<<"NmHsWidgetEmailEngine::handleMessageEvent() -- END";
    }

/*!
 handleMailboxEvent slot.
 */
void NmHsWidgetEmailEngine::handleMailboxEvent(NmMailboxEvent event,
        const QList<NmId> &mailboxIds)
    {
    qDebug()<<"NmHsWidgetEmailEngine::handleMailboxEvent() -- START";
    Q_UNUSED(mailboxIds);
    //react only to NmMailboxChanged event
    if (event == NmMailboxChanged)
        {
        if ( mSuspended )
            {
            mAccountEventReceivedWhenSuspended = true;
            }
        else
            {
            updateAccount();
            }

        }
    qDebug()<<"NmHsWidgetEmailEngine::handleMailboxEvent() -- END";
    }

/*!
 Update Account data
 \post if mEmailInterface exists, the mAccountName is refreshed from adapter 
       and accountNameChanged signal is emitted.
 */
void NmHsWidgetEmailEngine::updateAccount()
    {
    qDebug()<<"NmHsWidgetEmailEngine::updateAccount() -- START";
    
    NmMailbox* box = NULL;
    if(mEmailInterface)
        {
        mEmailInterface->getMailboxById(mMailboxId, box);
        }
    if (box)
        {
        mAccountName = box->name();
        emit accountNameChanged(mAccountName);
        delete box;
        box = NULL;
        }
    qDebug()<<"NmHsWidgetEmailEngine::updateAccount() -- END";
    }

/*!
  suspend slot.
  \post engine will not emit signals or refresh its data during suspension.
 */
void NmHsWidgetEmailEngine::suspend()
    {
    qDebug()<<"NmHsWidgetEmailEngine::suspend() -- START";
    mSuspended = true;
    qDebug()<<"NmHsWidgetEmailEngine::suspend() -- END";
    }

/*!
  activate slot.
  \post Engine will immediately refresh all the data that has been announced to
  have changed during the suspension. Events are enabled.
 */
void NmHsWidgetEmailEngine::activate()
    {
    qDebug()<<"NmHsWidgetEmailEngine::activate() -- START";
    mSuspended = false; 
    if ( mAccountEventReceivedWhenSuspended )
        {
        mAccountEventReceivedWhenSuspended = false;
        updateAccount();
        }
    if (mMessageEventReceivedWhenSuspended)
        {
        mMessageEventReceivedWhenSuspended = false;
        updateData();
        }
    qDebug()<<"NmHsWidgetEmailEngine::activate() -- END";
    }

/*!
  launchMailAppInboxView slot.
  \post Mail application is launched to inbox view corresponding widget's mailbox id
 */
void NmHsWidgetEmailEngine::launchMailAppInboxView()
    {
    qDebug()<<"NmHsWidgetEmailEngine::launchMailAppInboxView() -- START";

    XQServiceRequest request(
        emailInterfaceNameMailbox,
        emailOperationViewInbox,
        false);
    
    QList<QVariant> list;
    list.append(QVariant(mMailboxId.id()));

    request.setArguments(list);
    QVariant returnValue;
    bool rval = request.send(returnValue);
    
    qDebug()<<"NmHsWidgetEmailEngine::launchMailAppInboxView() -- END";
    }

/*!
  launchMailAppMailViewer slot.
  \param messageId Defines the message opened to viewer
  \post Mail application is launched and viewing mail specified by
 */
void NmHsWidgetEmailEngine::launchMailAppMailViewer(const NmId &messageId)
    {
    qDebug()<<"NmHsWidgetEmailEngine::launchMailAppMailViewer() -- START";

    XQServiceRequest request(
       emailInterfaceNameMessage,
       emailOperationViewMessage,
       false);
    
    QList<QVariant> list;
    list.append(QVariant(mMailboxId.id()));
    list.append(QVariant(mFolderId.id()));
    list.append(QVariant(messageId.id()));
    
    request.setArguments(list);
    QVariant returnValue;
    bool rval = request.send(returnValue);
    
    qDebug()<<"NmHsWidgetEmailEngine::launchMailAppMailViewer() -- END";
    }
