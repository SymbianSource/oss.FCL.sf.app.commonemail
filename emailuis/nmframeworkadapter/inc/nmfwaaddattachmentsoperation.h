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

#ifndef NMFWAADDATTACHMENTSOPERATION_H_
#define NMFWAADDATTACHMENTSOPERATION_H_

#include <nmaddattachmentsoperation.h>
#include <QObject>
#include <nmcommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaAddAttachmentsOperation : public NmAddAttachmentsOperation,
                                     public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaAddAttachmentsOperation(const NmMessage &message,
                                 const QList<QString> &fileList,
                                 CFSMailClient &mailClient);
    
    virtual ~NmFwaAddAttachmentsOperation();

public: // from MFSMailRequestObserver
    virtual void RequestResponseL(TFSProgress aEvent, TInt aRequestId);
    
protected:
    virtual void doRunAsyncOperation();
    virtual void doCancelOperation();

private:
    void doRunAsyncOperationL();	

private:
    const NmMessage &mMessage;

    QList<QString> mFileList;
    
    CFSMailClient &mMailClient;
    
    QList<int> mRequestIds;
};

#endif /* NMFWAADDATTACHMENTSOPERATION_H_ */
