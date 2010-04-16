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

#ifndef NMFWAMESSAGESENDINGOPERATION_H_
#define NMFWAMESSAGESENDINGOPERATION_H_

#include <QObject>
#include <nmmessagesendingoperation.h>
#include <nmcommon.h>
#include <CFSMailCommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaMessageSendingOperation : public NmMessageSendingOperation,
                                     public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessageSendingOperation(NmMessage *message,
                                 CFSMailClient &mailClient);
    
    ~NmFwaMessageSendingOperation();

    const NmMessage *getMessage();

    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected slots:
    void runAsyncOperation();
    
protected:
    void doCompleteOperation();

    void doCancelOperation();

private:
    void runAsyncOperationL();

private:
    NmMessage *mMessage;
    CFSMailClient &mMailClient;
    TInt mRequestId;
};

#endif /* NMFWAMESSAGESENDINGOPERATION_H_ */
