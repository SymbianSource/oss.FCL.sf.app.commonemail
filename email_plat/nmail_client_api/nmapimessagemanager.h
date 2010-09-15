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
#include <QVariant>

#include <nmapidef.h>
#include <nmapimessage.h>

namespace EmailClientApi {

class NmApiMessageManagerPrivate;

class NMAPI_EXPORT NmApiMessageManager : public QObject
{
    Q_OBJECT
public:
    NmApiMessageManager(QObject *parent,quint64 mailboxId);

    virtual ~NmApiMessageManager();

public slots:
    bool createDraftMessage(const QVariant *initData);
    
    bool createForwardMessage(NmApiMessage *orig,const QVariant *initData);
    
    bool createReplyMessage(const NmApiMessage *orig,const QVariant *initData,bool replyAll);
    
    bool moveMessages(const QList<quint64> messageIds,quint64 sourceFolderId,quint64 targetFolderId);
    
    bool saveMessage(const NmApiMessage &message);
    
    bool deleteMessages(const QList<quint64> messageIds);
    
    bool fetchMessage(quint64 messageId);
    
    bool sendMessage(const NmApiMessage &message);
    
    bool createAttachment(NmApiMessage &message,const QVariant &attachmenSpec);

    bool removeAttachment(NmApiMessage &message,quint64 attachmentId);
    
	bool fetchAttachment(const NmApiMessage &relatedMessage,quint64 attachmentId);
	
private:
    NmApiMessageManagerPrivate* d;
};

} // namespace EmailClientApi
#endif /*NMAPIMESSAGEMANAGER_H_ */
