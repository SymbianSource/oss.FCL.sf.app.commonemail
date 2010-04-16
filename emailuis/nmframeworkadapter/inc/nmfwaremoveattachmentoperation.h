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

#ifndef NMFWAREMOVEATTACHMENTOPERATION_H_
#define NMFWAREMOVEATTACHMENTOPERATION_H_

#include <QObject>
#include <nmoperation.h>
#include <nmcommon.h>

class NmMessage;
class CFSMailClient;

class NmFwaRemoveAttachmentOperation : public NmOperation
{
    Q_OBJECT
public:
    NmFwaRemoveAttachmentOperation(const NmMessage &message,
                                   const NmId &attachmentPartId,
                                   CFSMailClient &mailClient);
    
    virtual ~NmFwaRemoveAttachmentOperation();

protected slots:
    virtual void runAsyncOperation();

private:
    const NmMessage &mMessage;
    NmId mAttachmentPartId;
    CFSMailClient &mMailClient;
};

#endif /* NMFWAREMOVEATTACHMENTOPERATION_H_ */
