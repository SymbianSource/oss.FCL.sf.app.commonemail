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
* Description: 3rd party api testplugin
*
*/

#ifndef TESTPLUGINIMPL_H
#define TESTPLUGINIMPL_H

#include <e32base.h>
#include <emailobserverplugin.h>
#include <memailmailboxdata.h>

class MEmailObserverListener;
class CEmailDataImpl;

NONSHARABLE_CLASS( CEmailObserverPluginImpl ) : public EmailInterface::CEmailObserverPlugin
    {
    public:
        static EmailInterface::CEmailObserverPlugin* NewL( EmailInterface::MEmailObserverListener* aListener );
        virtual ~CEmailObserverPluginImpl();
        virtual EmailInterface::MEmailData& EmailDataL();

    private:
        CEmailObserverPluginImpl( EmailInterface::MEmailObserverListener* aListener );
        void ConstructL();
        void ReadInitialDatafromPubSubL();

    private:    // Data
        EmailInterface::MEmailObserverListener* iListener; // not owned
        CEmailDataImpl* iData;
    };

#endif // TESTPLUGINIMPL_H
