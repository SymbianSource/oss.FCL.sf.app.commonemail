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

#ifndef NMMESSAGECREATIONOPERATION_H_
#define NMMESSAGECREATIONOPERATION_H_

#include "nmoperation.h"

class NmMessage;
class NmId;

class NmMessageCreationOperation: public NmOperation
{
public:
    virtual NmMessage *getMessage() = 0;
    virtual NmId getMessageId() = 0;
};

#endif /* NMMESSAGECREATIONOPERATION_H_ */
