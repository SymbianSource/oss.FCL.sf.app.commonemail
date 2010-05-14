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

#ifndef NMATTACHMENTMANAGER_H_
#define NMATTACHMENTMANAGER_H_

#include <QObject>
#include <QPointer>

#include <nmcommon.h>

class NmUiEngine;
class NmOperation;
class NmAttachmentFetchObserver;

class NmAttachmentManager : public QObject
{
    Q_OBJECT
public:
    NmAttachmentManager(NmUiEngine &uiEngine);
    virtual ~NmAttachmentManager();
    void fetchAttachment(
            const NmId &mailboxId, 
            const NmId &folderId, 
            const NmId &messageId, 
            const NmId &messagePartId);
    bool isFetching() const;
    NmId partIdUnderFetch() const;
    int progressValue() const;
    void setObserver(NmAttachmentFetchObserver *observer);
    void clearObserver();
    void cancelFetch();
    
private slots:
    void changeProgress(int value);
    void attachmentFetchCompleted(int result);
    
private:
    NmUiEngine &mUiEngine;
    QPointer<NmOperation> mFetchOperation;      // Not owned
    NmAttachmentFetchObserver *mFetchObserver;  // Not owned
    NmId mAttaId;
    int mProgressValue;
    bool mIsFetching;
};


#endif /* NMATTACHMENTMANAGER_H_ */
