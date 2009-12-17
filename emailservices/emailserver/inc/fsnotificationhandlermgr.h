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
* Description: This file defines class MFSNotificationHandlerMgr.
*
*/



#ifndef M_FSNOTIFICATIONHANDLERMGR_H
#define M_FSNOTIFICATIONHANDLERMGR_H

#include "fsmailserverconst.h"


class CFSMailClient;
class CFSNotificationHandlerHSConnection;
class CFsEmailGlobalDialogsAppUi;

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class MFSNotificationHandlerMgr
    {

public:

    /**
     * Function that returns a reference to the mail client which
     * is used to access email framework.
     *
     * @since S60 ?S60_version
     * @return Pointer to framework object.
     */
    virtual CFSMailClient& MailClient() const = 0;

    /**
     * Gets pointer to object that provides information
     * about the current Home Screen status. NULL is returned
     * if the software is not a preinstalled version.
     *
     * Ownership IS NOT transferred.
     *
     * Notice that UpdateStatusL() is called on the HS
     * connection object in case notification handler manager
     * has just received an event of type TFSEventNewMail from
     * FSEmailFramework. In that case it is not necessary
     * to update the status again.
     *
     *
     * @since S60 ?S60_version
     * @return Pointer to HS connection object. NULL if this
     *         software is a postinstalled version. Ownership is
     *         not transferred.
     */     
    virtual CFSNotificationHandlerHSConnection* HSConnection() const = 0;

    //<cmail>
    virtual void MessageQueryL( TDesC& aMailboxName,
                                TRequestStatus& aStatus,
                                const TDesC& aCustomMessageText,
                                TFsEmailNotifierSystemMessageType aMessageType ) = 0;
    
    virtual TInt AuthenticateL( TDes& aPassword,
                                TDesC& aMailboxName, 
                                TRequestStatus& aStatus ) = 0;
    //</cmail>

    /**
     * Increases the active dialog count.
     */
    virtual void IncreaseDialogCount() = 0;

    /**
     * Decreases the active dialog count.
     */
    virtual void DecreaseDialogCount() = 0;

    /**
     * Gets the active dialog count.
     *
     * @return Number of active dialogs.
     */
    virtual TInt GetDialogCount() = 0;

    //<cmail>
    /**
     * If AppUi pointer is available, send app ui to background
     * 
     * @since S60 5.0
     */
    virtual void SendAppUiToBackground() = 0;
    
    /**
     * If AppUi pointer is available, bring app ui to foreground
     *
     * @since S60 5.0
     */
    virtual void BringAppUiToForeground() = 0;
    //</cmail>

protected:

    };


#endif // M_FSNOTIFICATIONHANDLERMGR_H
