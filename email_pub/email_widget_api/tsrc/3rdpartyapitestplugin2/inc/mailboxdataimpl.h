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
* Description:  3rd party api testplugin mailbox data
*
*/

#ifndef MAILBOXDATAIMPL_H
#define MAILBOXDATAIMPL_H

#include <e32base.h>
#include <memailmailboxdata.h>
#include <memailapplaunchdata.h>


class MMessageData;
class MEmailLaunchParameters;
class CAppLaunchDataImpl;

NONSHARABLE_CLASS( CMailboxDataImpl ) : public EmailInterface::MMailboxData
    {
    public:
        static CMailboxDataImpl* NewL();
        virtual ~CMailboxDataImpl();

        virtual TUint MailboxId() const;
        virtual const TDesC& Name() const;
        virtual const TDesC& BrandingIcon() const;
        virtual TBool IsOutboxEmpty() const;
        virtual TBool Unseen() const;
        virtual EmailInterface::MEmailLaunchParameters& LaunchParameters() const;
        virtual TInt MessageCount() const;
        virtual const RPointerArray<EmailInterface::MMessageData>& LatestMessagesL() const;

    private:
        CMailboxDataImpl();
        void ConstructL();

    public:    // Data
        TInt iMailboxId;
        HBufC* iName;
        HBufC* iBrandingIcon;
        TBool iIsOutboxEmpty;
        TBool iUnseen;
        CAppLaunchDataImpl* iAppLaunchParams;
        RPointerArray<EmailInterface::MMessageData> iMessages;
    };

#endif // MAILBOXDATAIMPL_H
