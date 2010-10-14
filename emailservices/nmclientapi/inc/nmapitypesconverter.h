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

#include <QList>

class NmMailbox;
class NmFolder;
class NmMessageEnvelope;
class NmAddress;
class NmMessage;
class NmMessagePart;


namespace EmailClientApi
{
class NmApiMailbox;
class NmApiFolder;
class NmApiMessageEnvelope;
class NmApiEmailAddress;
class NmApiMessage;
class NmApiAttachment;

class NmToApiConverter
{
public:
    static NmApiMailbox NmMailbox2NmApiMailbox(const NmMailbox &mailbox);
    static NmApiFolder NmFolder2NmApiFolder(const NmFolder &folder);
    static NmApiMessageEnvelope NmMessageEnvelope2NmApiMessageEnvelope(
        const NmMessageEnvelope &envelope);
    static QList<NmApiEmailAddress> NmAddress2QString(
        const QList<NmAddress> &addresses);
    static NmApiMessage NmMessage2NmApiMessage(NmMessage &message);

};
}
#endif /* NMTOAPICONVERTER_H_ */
