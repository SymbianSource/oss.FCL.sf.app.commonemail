/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Always online plugin for freestyle smtp handling
*
*/

#ifndef IPSSOSAOPLUGINECOMINTERFACE_H_
#define IPSSOSAOPLUGINECOMINTERFACE_H_

#include <e32base.h>    // CBase
#include <AlwaysOnlineEComInterface.h>
#include <AlwaysOnlineManagerClient.h>
#include <msvapi.h>


class CIpsSosAOSmtpSendWatcher;
class CIpsSosAOImapPopLogic;

//constants
/**
* Always online plugin interface, creates imappop logic and smtp logic
* objects, sends always online server command to both logic objects. 
* Sends msv events to logics
*
* @lib ipssosaoplugin.lib
*/
NONSHARABLE_CLASS (CIpsSosAOPluginEComInterface) : 
    public CAlwaysOnlineEComInterface,
    public MMsvSessionObserver
    {
    public:

        /**
        * Destructor
        */
        virtual ~CIpsSosAOPluginEComInterface();

    public: // from CAlwaysOnlineEComInterface
        
        /**
        * NewL
        * Two phased constructor
        * @return CEComEmailPlugin*, self pointer
        */
        IMPORT_C static CIpsSosAOPluginEComInterface* NewL();

        /**
        * Handles always online server commands
        * @param TInt, command Id
        * @param TDesC8*, parameter pack
        * @return TAny*, return value
        */
        IMPORT_C virtual TAny* HandleServerCommandL( 
            TInt    aCommand, 
            TDesC8* aParameters );

    public: // from MMscServerSession
        
        /*
         * Handles message server session events
         */
        virtual void HandleSessionEventL(
                TMsvSessionEvent aEvent, 
                TAny* aArg1, TAny* aArg2, TAny* aArg3);
        
    private:
        /**
        * Constructor
        */
        CIpsSosAOPluginEComInterface();

        /**
        * ConstructL
        * Second phase constructor
        */
        void ConstructL();
        
        /**
        * Called when CMsvSession is ready
        */
        void CompleteConstructL();
        
    private: // data
        
        CIpsSosAOSmtpSendWatcher*   iSendWatcher;
        CIpsSosAOImapPopLogic*  iImapPopLogic;
        CMsvSession*            iSession;
        RArray<TInt>            iBufferedAOCommands;


    };

#endif /*IPSSOSAOPLUGINECOMINTERFACE_H_*/
