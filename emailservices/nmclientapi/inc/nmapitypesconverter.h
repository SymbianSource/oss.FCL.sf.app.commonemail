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

#ifndef NMTOAPICONVERTER_H
#define NMTOAPICONVERTER_H

#include "nmenginedef.h"
#include "nmapiconverterheader.h"

/*!
 * 
 */
class NmToApiConverter
{
public:
    static EmailClientApi::NmMailbox NmMailbox2ApiNmMailbox(const NmMailbox &mailbox);
    static EmailClientApi::NmFolder NmFolder2ApiNmFolder(const NmFolder &folder);
    static EmailClientApi::NmMessageEnvelope NmMessageEnvelope2ApiEnvelope(
        const NmMessageEnvelope &envelope);

private:
    static QList<EmailClientApi::NmEmailAddress> NmAddress2QString(
        const QList<NmAddress> &addresses);
};

#endif /* NMTOAPICONVERTER_H_ */
