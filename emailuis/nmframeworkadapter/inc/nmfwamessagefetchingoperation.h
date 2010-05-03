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

#ifndef NMFAMESSAGEFETCHINGOPERATION_H_
#define NMFAMESSAGEFETCHINGOPERATION_H_

#include <nmoperation.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaMessageFetchingOperation : public NmOperation, public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessageFetchingOperation(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        CFSMailClient &mailClient,
        QObject *parent = NULL);
    
    ~NmFwaMessageFetchingOperation();
    
    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    virtual void doRunAsyncOperation();
    
protected:
    virtual void doCancelOperation();

protected:
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;

    CFSMailClient &mMailClient;
    
    TInt mRequestId;
};

#endif /* NMFAMESSAGEFETCHINGOPERATION_H_ */
