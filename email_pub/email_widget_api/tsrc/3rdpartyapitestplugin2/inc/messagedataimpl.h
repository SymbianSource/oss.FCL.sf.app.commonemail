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
* Description:  3rd party api testplugin message data
*
*/

#ifndef MESSAGEDATAIMPL_H
#define MESSAGEDATAIMPL_H

#include <e32base.h>

class MMessageData;

NONSHARABLE_CLASS( CMessageDataImpl ) : public EmailInterface::MMessageData
    {
    public:
        static CMessageDataImpl* NewL();
        virtual ~CMessageDataImpl();

        virtual const TDesC& Sender() const;
        virtual const TDesC& Subject() const;
        virtual TTime TimeStamp() const;
        virtual TBool Unread() const;
        virtual EmailInterface::TEmailPriority Priority() const;
        virtual TBool Attachments() const;
        virtual TBool CalendarMsg() const;
        virtual TBool Replied() const;
        virtual TBool Forwarded() const;

        void SetFlagsL( const TInt aFlags );

    private:
        CMessageDataImpl();
        void ConstructL();
    public:    // Data
        HBufC* iSender;
        HBufC* iSubject;
        TTime iTimeStamp;
        TBool iUnread;
        EmailInterface::TEmailPriority iPriority;
        TBool iAttachments;
        TBool iCalendarMsg;
        TBool iReplied;
        TBool iForwarded;
    };

#endif // MESSAGEDATAIMPL_H
