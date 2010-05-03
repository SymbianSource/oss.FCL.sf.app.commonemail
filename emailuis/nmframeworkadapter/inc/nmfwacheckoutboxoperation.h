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

#ifndef NMFWACHECKOUTBOXOPERATION_H_
#define NMFWACHECKOUTBOXOPERATION_H_

#include <nmcheckoutboxoperation.h>
#include <nmcommon.h>

class CFSMailClient;

class NmFwaCheckOutboxOperation : public NmCheckOutboxOperation
{
    Q_OBJECT
public:
    NmFwaCheckOutboxOperation(
        const NmId &mailboxId,
        CFSMailClient &mailClient);
    
    virtual ~NmFwaCheckOutboxOperation();
    
    virtual bool getMessageId(NmId &messageId) const;

private:
    virtual void doRunAsyncOperation();
    
private:
    void doRunAsyncOperationL();

private:
    NmId mMailboxId;

    CFSMailClient &mMailClient;

    bool mFound;

    NmId mMessageId;
};

#endif /* NMFWACHECKOUTBOXOPERATION_H_ */
