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
* Description: This file defines class CFSNotificationHandlerHSConnection.
*
*/


#ifndef C_FSNOTIFICATIONHANDLERHSCONNECTION_H
#define C_FSNOTIFICATIONHANDLERHSCONNECTION_H

#include <e32base.h>
//<cmail>
#include "cfsmailcommon.h"
//</cmail>

class CRepository;


/**
 *  ?one_line_short_description
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSNotificationHandlerHSConnection : public CBase
    {

public:

    static CFSNotificationHandlerHSConnection* NewL();
    static CFSNotificationHandlerHSConnection* NewLC();
    

    /**
    * Destructor.
    */
    virtual ~CFSNotificationHandlerHSConnection();


    /**
     * Updates internal state from the status of the HS.
     * This function has to be called everytime before using
     * this class to get the latest info of the HS status.
     *
     * @since S60 ?S60_version
     */    
    void UpdateStatusL();

    /**
     * Boolean to represent whether all notifications have
     * been turned off or not. Returns true if any of Home Screen
     * email plug-ins related to fs email or notification
     * plug-in have been turned on. If HS is turned off or
     * none of the plugins are on while HS is on, EFalse is
     * returned.
     *
     * @since S60 ?S60_version
     * @return Boolean telling whether notifications
     *         are on or off. ETrue means on.
     */
    TBool NotificationsOn() const;

    /**
     * Boolean to represent whether the mailbox (account)
     * is visible on Home Screen or not.
     *
     * @since S60 ?S60_version
     * @param aMailboxId Mailbox identifier.
     * @return EFalse if the mailbox is not visible ETrue else.
     */
    TBool MailboxVisibleInHS( TFSMailMsgId aMailboxId ) const;
    
    
protected:

private:

    CFSNotificationHandlerHSConnection();

    void ConstructL();

    /**
     * Function to update internal HS state variables from
     * repositories.
     *
     * @since S60 ?S60_version
     */
    void UpdateFromCrL();
    
    /**
     * Boolean to represent whether notifications plugin is
     * marked to be on in repository.
     * Returns true if the plugin is on. Otherwise EFalse is
     * returned.
     *
     * @since S60 ?S60_version
     * @return Boolean telling whether notifications
     *         plugin is on or off. ETrue means on.
     */
    TBool NotificationsPluginOnInCrL();

    /**
     * Function to retrieve home screen email plugin info
     * from repositories.
     *
     * @since S60 ?S60_version
     * @param aPluginUid Uid identifying the mail plugin whose info
     *        is requested.
     * @param aCrIdForPluginFrameworkType Repository key for
     *        the framework type of the account attached to the plugin.
     * @param aCrIdForPluginEmailBoxId Repository key for the
     *        mailbox id of the account attached to the plugin.
     * @param aCrIdForPluginEmailPluginUid Repository key for the
     *        plugin uid of the account attached to the plugin.
     * @param aPluginOn Whether the identified plugin is on or off.
     *        ETrue is on.
     * @param aBoxIsFsBox Boolean telling whether the identified plugin
     *        is attached to a Freestyle mailbox. ETrue means that it
     *        is. This parameter is not updated if aPluginOn is EFalse.
     * @param aBoxId Id of the Frestyle mailbox if the aBoxIsFsBox is
     *        ETrue. Otherwise this parameter remains unaltered.
     */
    void PluginInfoFromCrL(
        const TUint32 aPluginUid,
        const TUint32 aCrIdForPluginFrameworkType,
        const TUint32 aCrIdForPluginEmailBoxId,
        const TUint32 aCrIdForPluginEmailPluginUid,
        TBool& aPluginOn,
        TBool& aBoxIsFsBox,
        TFSMailMsgId& aBoxId );
        
    /**
     * Function that updates internal status according to given
     * HS mail plugin info.
     *
     * @since S60 ?S60_version
     * @param aPluginOn Informs whether a HS mail plugin is on or
     *        not.
     * @param aMailboxId Mailbox identifier of the FS mailbox that is
     *        attached to the HS plugin. Should be NULL id if no FS
     *        mailbox is attached to HS plugin.
     */        
    void UpdateInternalStatusFromPluginInfoL(
        TBool aPluginOn,
        TFSMailMsgId aMailboxId );

private: // data

    /**
     * Boolean which tells whether the Home Screen is on or off.
     * ETrue when on.
     */   
    TBool iHSOn;
    
    /**
     * Boolean which tells whether any of the Home Screen plugins
     * that are relevant to FS email are on.
     * ETrue when on.
     */ 
    TBool iAnyFSRelevantHSPluginOn;
    
    /**
     * Access to personalization repository which is used to check
     * whether HS is on or not.
     * Own.
     */      
    CRepository* iPersonalizationRepo;
    
    /**
     * Access to Active Idle repository which is used to check
     * what plugins are active in HS.
     * Own.
     */  
    CRepository* iActiveIdleRepo;

    /**
     * Access to Home Screen repository which is used to check
     * Home Screen info.
     * Own.
     */  
    CRepository* iHomeScreenRepo;
    
    
    /**
     * Array identifying Freestyle mailboxes that are currently
     * shown in home screen.
     */ 
    RArray<TFSMailMsgId> iActiveFSMailboxesInHS;


    };


#endif // C_FSNOTIFICATIONHANDLERHSCONNECTION_H
