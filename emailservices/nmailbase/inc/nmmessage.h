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

#ifndef NMMESSAGE_H_
#define NMMESSAGE_H_

#include <QString>
#include <QDateTime>
#include "nmbasedef.h"
#include "nmcommon.h"
#include "nmmessagepart.h"

class CFSMailMessage;

class NMBASE_EXPORT NmMessage : public NmMessagePart
{
public:
    NmMessage();
    NmMessage(const NmId &id);
    NmMessage(const NmId &id, const NmId &parentId);
    NmMessage(const NmId &id, const NmId &parentId, const NmId &mailboxId);
    NmMessage(const NmMessageEnvelope &envelope);
    NmMessage(const NmMessage &message);
    virtual ~NmMessage();
    NmMessage(const NmMessagePart& message);

    virtual NmId id() const;
    virtual void setId(const NmId &id);

    NmMessagePart *plainTextBodyPart();
    NmMessagePart *htmlBodyPart();

    void setEnvelope(const NmMessageEnvelope &envelope);
    NmMessageEnvelope &envelope();
    const NmMessageEnvelope &envelope() const;

    CFSMailMessage* getCFSMailMessage();

    void attachmentList(QList<NmMessagePart*> &parts);
    
private:
    NmMessage &operator=(const NmMessage &message);

};

#endif /* NMMESSAGE_H_ */




