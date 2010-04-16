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

#include <QTimer>

#include "nmhswidgetemailengine.h"
#include "nmmessageenvelope.h"

#include "nmframeworkadapter_stub.h" //Stub implementation for Sprint 2


/*!
    Constructor
*/
NmHsWidgetEmailEngine::NmHsWidgetEmailEngine() :
    mAccountName(0), 
    mUnreadCount(0), 
    m_stub_adapter(0)
    {
    m_stub_adapter = new NmFrameworkAdapter_stub();

    updateData();
    updateAccount();
    
    //TODO: To be removed in Sprint 3

    connect( m_stub_adapter, SIGNAL( messageEvent() ) , this, SLOT( updateData() ) );
    connect( m_stub_adapter, SIGNAL( mailboxEvent() ) , this, SLOT( updateAccount() ) );

    //END
    
    }



/*!
    Reset envelope list
    \post mEnvelopeList.isEmpty() == true && all contained objects are deleted
*/
void NmHsWidgetEmailEngine::resetEnvelopeList()
    {
    while (!mEnvelopeList.isEmpty())
        {
        delete mEnvelopeList.takeFirst();    
        }
    
    }

/*!
    Destructor
*/
NmHsWidgetEmailEngine::~NmHsWidgetEmailEngine()
    {
    resetEnvelopeList();
    delete m_stub_adapter;
    }


/*!
    Shrink envelope list
    \post mEnvelopeList.count() <= KMaxNumberOfEnvelopesProvided, 
    other objects are removed and freed.
*/
void NmHsWidgetEmailEngine::shrinkEnvelopeList()
    {
    //remove & delete the last object from the list
    //until we have shrinked the list to be small enough
    while ( mEnvelopeList.count() > KMaxNumberOfEnvelopesProvided )
        {
        delete mEnvelopeList.takeLast();    
        }
    }


/*!
    getEnvelopes() provides message envelopes. 
    Amount of message envelopes returned is the smallest of the following factors:
    'KMaxNumberOfEnvelopesProvided', 'maxEnvelopeAmount', 'amount of available envelopes'. 
    
    \param list list to be filled with message envelopes
    \param maxEnvelopeAmount Client side limit for amount of message envelope count. 
    \return count of envelopes added to list
*/
int NmHsWidgetEmailEngine::getEnvelopes(QList<NmMessageEnvelope> &list, int maxEnvelopeAmount)
    {
    int i = 0;
    for (; i<mEnvelopeList.count() && i<maxEnvelopeAmount ; i++ )
        {
        NmMessageEnvelope env( *mEnvelopeList[i] );
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
    return mUnreadCount;
    }

/*!
    accountName

    \return name of the monitored account
*/
QString NmHsWidgetEmailEngine::accountName()
    {
    return mAccountName;
    }

/*!
    Refresh email data.
    \post mData is refreshed with valid content so that it has
    valid data with maximum of KMaxNumberOfEnvelopesProvided envelopes
    
    NOTE: THIS IS STUB IMPLEMENTATION UNTIL SPRINT 3
*/
void NmHsWidgetEmailEngine::updateData()
    {
    resetEnvelopeList();
    //TODO Sprint 3: Get the list from the server
    /* TEST DATA */

    resetEnvelopeList();
    m_stub_adapter->listMessages(mEnvelopeList);
    
    emit mailDataChanged();
    mUnreadCount = calculateUnreadCount( mEnvelopeList );
    emit unreadCountChanged( mUnreadCount );
    /* TEST DATA END*/
    
    //TODO: Sprint3 at this point shrink the list to KMaxNumberOfEnvelopesProvided
    shrinkEnvelopeList();
    }

/*!
    Update Account data
    \post mAccountName is valid
    
    NOTE: THIS IS STUB IMPLEMENTATION UNTIL SPRINT 3
*/
void NmHsWidgetEmailEngine::updateAccount()
    {
    mAccountName = m_stub_adapter->mailboxName();
    emit accountNameChanged (mAccountName);
    }

/*!
    Calculate unread count from envelope list.
    
    \param envelopeList List of envelopes from which unread envelopes are to be count.
    \return count of unreads in list 
    if unread count is < KMaxUnreadCount, otherwise KMaxUnreadCount. 
*/
int NmHsWidgetEmailEngine::calculateUnreadCount(QList<NmMessageEnvelope*> envelopeList)
    {
    int unreadCount = 0; 
    
    QList<NmMessageEnvelope*>::const_iterator itEnd(envelopeList.constEnd());
    QList<NmMessageEnvelope*>::const_iterator it = envelopeList.constBegin();
    
    for ( ; (it != itEnd)&&(unreadCount < KMaxUnreadCount); ++it)
    {
      if ( !(*it)->isRead() )
          {
          unreadCount++;
          }
    }
    return unreadCount;
    }
