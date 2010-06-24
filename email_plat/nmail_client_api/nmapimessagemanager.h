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

#ifndef NMAPIMESSAGEMANAGER_H_
#define NMAPIMESSAGEMANAGER_H_

#include <QObject>
#include <QList>

#include <nmapidef.h>

struct NmApiMessage;
class NmApiEmailMessage;

namespace EmailClientApi {

class NmApiFolder;
class NmApiMessageManagerPrivate;

class NMAPI_EXPORT NmApiMessageManager : public QObject
{
    Q_OBJECT
	
public:
    NmApiMessageManager(quint64 mailboxId,QObject *parent = 0);

    virtual ~NmApiMessageManager();

public slots:
    bool createDraftMessage(const QVariant *initData);
    
    bool createForwardMessage(const QVariant *initData);
    
    bool createReplyMessage(const QVariant *initData,bool replyAll);
    
    bool moveMessages(const QList<quint64> messageIds,
		quint64 sourceFolderId,quint64 targetFolderId);
    
    bool copyMessages(const QList<quint64> messageIds,					
					quint64 sourceFolder,
					quint64 targetFolder);
    
    bool saveMessage(const ::NmApiMessage &message);
    
    bool deleteMessages(const QList<quint64> messageIds);
    
    bool fetch(const NmApiMessage &message);
    
    bool send(const NmApiMessage &message);
    
    bool createAttachment(NmApiEmailMessage &message,const QVariant &attachmenSpec);

    bool removeAttachment(NmApiEmailMessage &message,quint64 attachmentId);
    
signals:
	void messagesCopied(int result);

	void messagesCreated(int result);

	void messagesMoved(int result);

	void messagesDeleted(int result);

private:
	NmApiMessageManagerPrivate *d;
};

}

#endif
