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
* Description:  3rd party api testplugin app launcher data
*
*/

#ifndef APPLAUNCHDATAIMPL_H
#define APPLAUNCHDATAIMPL_H

#include <e32base.h>
#include <e32des8.h>

#include "memailapplaunchdata.h"

NONSHARABLE_CLASS( CAppLaunchDataImpl ) : public EmailInterface::MEmailLaunchParameters
    {
    public:
        static CAppLaunchDataImpl* NewL( TUid aAppUid, TUid aViewUid );
        virtual ~CAppLaunchDataImpl();

    public:
        virtual TUid ApplicationUid() const;
        virtual TUid ViewId() const;
        virtual TUid CustomMessageId() const;
        virtual const TDesC8& CustomMessage() const;

    private:
        CAppLaunchDataImpl( TUid aAppUid, TUid aViewUid );
        void ConstructL();

    public:    // Data
        TUid iAppUid;
        TUid iViewUid;
        TUid iCustMsgUid;
        HBufC8* iCustomMessage;
    };

#endif // APPLAUNCHDATAIMPL_H
