/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class CFSMailMtmHandler.
*
*/


#ifndef __FSMAIL_MTM_HANDLER_H__
#define __FSMAIL_MTM_HANDLER_H__

#include <msvapi.h>

#include "fsnotificationhandlerbase.h"

class CMsvSession;
class CClientMtmRegistry;
class CFsMtmClient;

/**
 *  Class to handle informing of mtm about new/removed accounts
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSMailMtmHandler : public CFSNotificationHandlerBase,
                          public MMsvSessionObserver
    {
public:

    /**
     * Two-phased constructor.
     * @param aMailClient Object to access email framework
     * @param aOwner Owner and manager of this handler. NULL if not
     *               specified.
     */	
    static CFSMailMtmHandler* NewL( MFSNotificationHandlerMgr& aOwner );
    /**
    * Destructor.
    */
    virtual ~CFSMailMtmHandler();

// from base class MMsvSessionObserver
    void HandleSessionEventL(
        TMsvSessionEvent aEvent,
        TAny* aArg1, 
        TAny* aArg2,
        TAny* aArg3 );

protected:


// from base class CFSNotificationHandlerBase  

    /**
     * From CFSNotificationHandlerBase.
     * Implementation of CapabilitiesToContinueL. Checks whether
     * the mailbox handles the mtm communication itself.
     *
     * @see CFSNotificationHandlerBase::CapabilitiesToContinueL 
     *
     * @since S60 ?S60_version
     */
    virtual TBool CapabilitiesToContinueL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 ) const;

    /**
     * Function that does the actual event handling.
     *
     * @see CFSNotificationHandlerBase::EventL
     *
     * @since S60 ?S60_version
     */ 
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );

    
private:

    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aOwner Owner and manager of this handler.
     */ 
    CFSMailMtmHandler( MFSNotificationHandlerMgr& aOwner );
    void ConstructL();

    /**
     * Function to setup the mtm client before usage.
     *
     * @since S60 ?S60_version
     */
    void InstallMtmL();
    
    /**
     * Function to be called when mtm client is not needed anymore.
     *
     * @since S60 ?S60_version
     */
    void UninstallMtm();

    /**
     * Function used to retrieve file name for resource file used
     * in mtm installation/uninstallation.
     *
     * @since S60 ?S60_version
     * @return File name.
     */
    TFileName InstallationFilenameL();

    /**
     * Function to kill desired process.
     *
     * @since S60 ?S60_version
     * @param aName Name of the process. Kills all processes with given name
     */    
    void KillProcess( const TDesC& aName );

    /**
     * Kills applications with uids defined in KAppsToKill
     *
     * @since S60 ?S60_version
     */       
    void KillAppsL();

// from base class CFSNotificationHandlerBase         
    virtual void TurnNotificationOn();
    virtual void TurnNotificationOff();

private: // data
	
	/**
     * Boolean to inform whether we are in the start up or not.
     * ETrue means that we are starting up.
     */ 	
	TBool iStartingUp;
	
	/**
     * Session to message server.
     * Own.
     */ 
	CMsvSession* iMsvSession;
	
	/**
     * Access to client side MTM registry.
     * Own.
     */ 	
	CClientMtmRegistry* iMtmRegistry;
	
	/**
     * MTM client used to notify of changes in accounts.
     * Own.
     */  
	CFsMtmClient* iMtmClient;
	
    };


#endif  //__FSMAIL_MTM_HANDLER_H__
