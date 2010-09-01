/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  3rd party api testplugin email data
*
*/

#ifndef EMAILDATAIMPL_H
#define EMAILDATAIMPL_H

#include <e32base.h>
#include <memaildata.h>

#include "mailboxdataimpl.h"

class MMailboxData;

NONSHARABLE_CLASS( CEmailDataImpl ) : public EmailInterface::MEmailData
    {
    public:
        static CEmailDataImpl* NewL();
        virtual ~CEmailDataImpl();
        virtual RPointerArray<EmailInterface::MMailboxData>& MailboxesL();
        
    private:
        CEmailDataImpl();
        void ConstructL();

    public:    // Data
        RPointerArray<EmailInterface::MMailboxData> iMailboxes;
    };

#endif // EMAILDATAIMPL_H
