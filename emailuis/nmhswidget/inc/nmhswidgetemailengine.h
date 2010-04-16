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

#ifndef NMHSWIDGETEMAILENGINE_H_
#define NMHSWIDGETEMAILENGINE_H_

#include <QObject>
class NmMessageEnvelope;
class NmFrameworkAdapter_stub;
//Maximum amount of envelopes that can be provided to client in getData function
//This is also the amount of envelopes that is kept in mData all the time
const int KMaxNumberOfEnvelopesProvided = 2;

//Maximum value for unread count. Counting will stop when this limit is reached
const int KMaxUnreadCount = 999; 

class NmHsWidgetEmailEngine : public QObject
    {
    Q_OBJECT
public:
    NmHsWidgetEmailEngine();
    ~NmHsWidgetEmailEngine();

    int getEnvelopes(QList<NmMessageEnvelope> &list, int maxEnvelopeAmount);
    int unreadCount();
    QString accountName();
        
public slots:
    void updateData(); 
    void updateAccount();

signals:
    void mailDataChanged();    
    void accountNameChanged(const QString& accountName);
    void unreadCountChanged(const int& unreadCount);
    
private:
    int calculateUnreadCount(QList<NmMessageEnvelope*> envelopeList);
    void resetEnvelopeList();
    void shrinkEnvelopeList();
    
private:
    QString mAccountName;
    int mUnreadCount;
    QList<NmMessageEnvelope*> mEnvelopeList;
    
    NmFrameworkAdapter_stub* m_stub_adapter; 

    };

#endif /* NMHSWIDGETEMAILENGINE_H_ */
