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

#ifndef NMAPIMESSAGEMANAGER_P_H_
#define NMAPIMESSAGEMANAGER_P_H_

#include <QObject>
#include <nmapimessagemanager.h>
#include <nmapifolder.h>
#include <nmapidatapluginfactory.h>
#include <nmcommon.h>
#include <nmapifolder.h>
#include <nmapiprivateclasses.h>


class NmApiEmailMessage;

namespace EmailClientApi
{

class NmApiMessageManagerPrivate : public QObject
{
 Q_OBJECT  
public:
    NmApiMessageManagerPrivate(QObject *parent,quint64 mailboxId);
    virtual ~NmApiMessageManagerPrivate();
    
private:
    enum EState {
        EIdle = 0,
        ECopyPending,
        EMovePending,
        EDeletePending        
    };
    
public slots: 
    bool moveMessages(const QList<quint64> messageIds,
                    quint64 sourceFolderId,
                    quint64 targetFolderId);

    bool copyMessages(const QList<quint64> messageIds, 
                   quint64 sourceFolder,
                   quint64 targetFolder);
    
	void messageEventHandler(NmMessageEvent event,
								const NmId &folder,
								const QList<NmId> &messages,
								const NmId &mailBox);
								
signals:
	void messagesCopied(int result);
	void messagesCreated(int result);
	void messagesMoved(int result);
	void messagesDeleted(int result);


private:    
    NmApiMessageManagerPrivate::EState mState;
    QList<quint64> mMessages;
    NmId mTarget;
    NmId mMailboxId; 
    NmApiDataPluginFactory *mFactory;
    QObject *mPlugin;
};

}

#endif /*NMAPIMESSAGEMANAGER_P_H_ */
