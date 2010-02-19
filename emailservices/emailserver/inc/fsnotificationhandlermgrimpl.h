/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of notification handler manager implementation
*
*/



#ifndef C_FSNOTIFICATIONHANDLERMGRIMPL_H
#define C_FSNOTIFICATIONHANDLERMGRIMPL_H

#include <e32base.h>

//<cmail>
#include "cfsmailcommon.h"
#include "mfsmaileventobserver.h"
//</cmail>

#include "fsnotificationhandlermgr.h"

class CFSMailClient;
class CFSNotificationHandlerBase;
class CFsEmailGlobalDialogsAppUi;

/**
 *  Class for managing notification showing.
 *
 *  Class that manages notification showing by managing the
 *  notification handlers which do the actual notification showing.
 *  Framework events are observed in this class and they are
 *  then directed to the notification handlers.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSNotificationHandlerMgr :
    public CActive,
    public MFSNotificationHandlerMgr,
    public MFSMailEventObserver
    {
public:

    //<cmail> aAppUi parameter no longer necessary
    static CFSNotificationHandlerMgr* NewL( CFsEmailGlobalDialogsAppUi* aAppUi = NULL );
    static CFSNotificationHandlerMgr* NewLC( CFsEmailGlobalDialogsAppUi* aAppUi = NULL );
    //</cmail>

    /**
    * Destructor.
    */
    virtual ~CFSNotificationHandlerMgr();


// from base class MFSMailEventObserver

    /**
     * @see MFSMailEventObserver::EventL
     */
    virtual void EventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );

// from base class MFSNotificationHandlerMgr

    /**
     * @see MFSNotificationHandlerMgr::MailClient
     */
    virtual CFSMailClient& MailClient() const;

    /**
     * @see MFSNotificationHandlerMgr::HSConnection
     */
    virtual CFSNotificationHandlerHSConnection* HSConnection() const;
//<cmail>
    virtual void MessageQueryL( TDesC& aMailboxName,
//</cmail>
                                TRequestStatus& aStatus,
                                const TDesC& aCustomMessageText,
                                TFsEmailNotifierSystemMessageType aMessageType );

    virtual TInt AuthenticateL( TDes& aPassword,
                                TDesC& aMailboxName,
                                TRequestStatus& aStatus  );
    //</cmail>

    /**
     * @see MFSNotificationHandlerMgr::IncreaseDialogCount
     */
    void IncreaseDialogCount();

    /**
     * @see MFSNotificationHandlerMgr::DecreaseDialogCount
     */
    void DecreaseDialogCount();

    /**
     * @see MFSNotificationHandlerMgr::GetDialogCount
     */
    TInt GetDialogCount();

    //<cmail>
    /**
     * @see MFSNotificationHandlerMgr::SendAppUiToBackground
     */
    void SendAppUiToBackground();

    /**
     * @see MFSNotificationHandlerMgr::BringAppUiToForeground
     */
    void BringAppUiToForeground();
    //</cmail>

protected:
    // From CActive
    /**
     * Handles an active object's request completion event
     */
    void RunL();

    /**
     * Implements cancellation of an outstanding request
     */
    void DoCancel();

private:

    //<cmail>
    CFSNotificationHandlerMgr(CFsEmailGlobalDialogsAppUi* aAppUi = NULL);
    //</cmail>

    void ConstructL();


    /**
     * Start to observe framework events.
     *
     * @since S60 ?S60_version
     */
    void StartObservingL();

    /**
     * Stop observing framework events.
     *
     * @since S60 ?S60_version
     */
    void StopObserving();

    /**
    * Function to create desired type of notification
    * handler and append it into handler array.
    *
    * @since S60 ?S60_version
    */
    void CreateAndStoreHandlerL( TInt aImplementationUid );

	void CleanTempFilesIfNeededL();

private: // data

    /**
     * Mail client to access email framework
     * Own.
     */
    CFSMailClient* iMailClient;


    /**
     * Handlers for different types of notifications.
     * Own.
     */
    RPointerArray<CFSNotificationHandlerBase> iHandlers;

    /**
     * Object representing connection to Home Screen specific data.
     * It is used to retrieve current status of Home Screen.
     * Own.
     */
    CFSNotificationHandlerHSConnection* iHSConnection;

    /**
     * AppUi class.
     * Not Owned.
     */
    //<cmail>
    //CFsEmailGlobalDialogsAppUi& iAppUi;
    CFsEmailGlobalDialogsAppUi* iAppUi;
    //</cmail>

    /**
     * Number of active dialogs.
     */
    TInt iDialogCount;

    };

#endif // C_FSNOTIFICATIONHANDLERMGRIMPL_H
