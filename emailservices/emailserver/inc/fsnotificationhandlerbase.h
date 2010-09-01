/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header for notification handling base class
*
*/


#ifndef __FSNOTIFICATIONHANDLERBASE_H__
#define __FSNOTIFICATIONHANDLERBASE_H__

#include <e32base.h>
//<cmail>
#include "mfsmaileventobserver.h"
//</cmail>

class CFSMailClient;
class CFSMailFolder;
class CFSMailMessage;
class MFSNotificationHandlerMgr;
class CFSNotificationHandlerHSConnection;
class TFSMailMsgId;


class TNewMailInfo
    {
public:
    inline TNewMailInfo( TFSMailMsgId aMsgId, 
                         TFSMailMsgId aMailBoxId,
                         TFSMailMsgId aParentFolderId) 
    : iMsgId( aMsgId ), iMailBox( aMailBoxId ), iParentFolderId( aParentFolderId)
    { }
    
    TFSMailMsgId iMsgId;
    TFSMailMsgId iMailBox;
    TFSMailMsgId iParentFolderId;
    };

class MFSTimerObserver
    {
    public:

    /**
     * TODO document me
     */
     virtual void TimerExpiredL() = 0;

     };

class CNewMailNotificationTimer : public CTimer
    {
public:

    static CNewMailNotificationTimer* NewL(
            MFSTimerObserver& aObserver );
    static CNewMailNotificationTimer* NewLC(
            MFSTimerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CNewMailNotificationTimer();


protected:
  
    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aObserver Observer of this timer.
     */ 
    CNewMailNotificationTimer(
            MFSTimerObserver& aObserver );
    void ConstructL();

// from base class CActive
    virtual void DoCancel();
    virtual void RunL();

private:


            
private: // data

    /**
     * Observer and user of this timer.
     */    
    MFSTimerObserver& iObserver;

    };

/**
 *  A base class for notification handlers that need timer services.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSNotificationHandlerBase : public CBase, public MFSMailEventObserver, public MFSTimerObserver
    {
public:
    
    /**
     * Two-phased constructor.
     * @param aOwner Owner and manager of this handler.
     */ 
    static CFSNotificationHandlerBase* NewL( TInt aHandlerUid, MFSNotificationHandlerMgr& aOwner );
    
public:
    
    /**
    * Function that returns a reference to the mail client which
    * is used to access email framework.
    *
    * @since S60 ?S60_version
    * @return Reference to framework object.
    */
    CFSMailClient& MailClient() const;

// from base class MFSMailEventObserver

    /**
    * Function that is used to inform the handler of a new event.
    * Default implementation checks whether the startup is complete
    * and therefore the object of this class is observing
    * events.
    * Also checks whether for example the mailbox has capabilities
    * so that the event handling is in this handler is possible. This
    * is checked by calling CapabilitiesToContinueL function which
    * should return ETrue whether the capabilities indicate that
    * the notification handling should be continued.
    * If notification handling is continued then call for HandleEventL
    * is made.
    *
    * This function is not necessarily called by the framework and
    * thus an instance of this class does not necessarily act as a
    * direct observer for the framework but for example for a
    * manager class.
    *
    * @see MFSMailEventObserver::EventL
    */
    virtual void EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox,
                                  TAny* aParam1, TAny* aParam2, TAny* aParam3 );

    /**
    * Destructor.
    */
    virtual ~CFSNotificationHandlerBase();


protected:  
  
    /**
    * Constructor.
    *
    * @since S60 ?S60_version
    * @param aOwner Owner and manager of this handler.
    */ 
    CFSNotificationHandlerBase( MFSNotificationHandlerMgr& aOwner );
    
    void ConstructL();
    
    /**
    * Getter for information whether the handler is observing events
    *
    * @since S60 ?S60_version
    * @return Whether handler is observing or not
    */         
    TBool Observing() const;
    
    /**
    * Function that goes through messages identified by the
    * message ids given in the parameter array and checks
    * if any of them is such that it causes a notification.
    * In this default implementation it is checked if any of the
    * messages is unread and that it is the newest of unread messages
    * in the folder.
    *
    * @since S60 ?S60_version
    * @param aMailboxId Mailbox which contains the messages
    *                   checked here.
    * @param aParentFolderId Folder that contains the messages
    *                        checked here.
    * @param aMsgIdList List of message identifiers identifying
    *                   the messages that should be checked
    *                   for newest unread message.
    * @return Boolean value to indicate whether newest unread
    *         messages was found among the identified messages.
    */
    virtual TBool MessagesCauseNotificationL( TFSMailMsgId aMailboxId,
                                              TFSMailMsgId aParentFolderId,
                                              const RArray<TFSMailMsgId>& aMsgIdList );
    

    /**
    * Function that checks whether the given message is unread
    * or not.
    *
    * @since S60 ?S60_version
    * @param aMessage Message which is checked.
    * @return Boolean indicating whether the message was unread
    *         or not. ETrue means unread.
    */ 
    TBool MsgIsUnread( /*const*/ CFSMailMessage& aMessage ) const;

protected:
    
    /**
    * Function that returns a reference to the Home Screen connection
    * which is used to access Home Screen status.
    *
    * @since S60 ?S60_version
    * @return Pointer to Home Screen connection. Ownership is not
    *         transferred. NULL is returned if the connection
    *         cannot be provided.
    */    
    CFSNotificationHandlerHSConnection* HSConnection() const;
     
    /**
    * Setter for information whether the handler is observing events.
    *
    * @since S60 ?S60_version
    * @param aNewValue New value for observing info
    */     
    void SetObserving( TBool aNewValue );
     
    /**
     * Function that is used to check desired capabilities.
     * Default implementation of this function checks that the mailbox
     * does not have EFSMBoxCapaNewEmailNotifications capability. If it
     * has then EFalse is returned. 
     *
     * @see EventL for parameters.
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
     * Function that does the actual event handling. This class provides
     * a default implementation for the function. Sets notification on
     * if the messages cause MessagesCauseNotification() to return
     * ETrue.
     *
     * @see EventL for parameters.
     *
     * @since S60 ?S60_version
     */     
    virtual void HandleEventL(
        TFSMailEvent aEvent,
        TFSMailMsgId aMailbox,
        TAny* aParam1,
        TAny* aParam2,
        TAny* aParam3 );


    /**
     * Function that gets the newest message from the given
     * folder.
     *
     * @since S60 ?S60_version
     * @param aFolder Folder where from the newest message
     *                is searched.
     * @return Pointer to newest message in the folder. If
     *         no messages are found, leaves with KErrNotFound.
     */      
    CFSMailMessage* NewestMsgInFolderL(
        /*const*/ CFSMailFolder& aFolder ) const;

    
private:

    /**
     * Function that sets the notification of the derived class on.
     *
     * @since S60 ?S60_version
     */  
    virtual void TurnNotificationOn() = 0;

    /**
     * Function that sets the notification of the derived class off.
     *
     * @since S60 ?S60_version
     */  
    virtual void TurnNotificationOff() = 0;
    
// from TODO
    virtual void TimerExpiredL();
    
protected: // data

    /**
     * Owner and manager of the instance of this handler class.
     */    
    MFSNotificationHandlerMgr& iOwner;

private: // data

    /**
     * Boolean to inform whether we are currently observing
     * the mail framework
     * Used for example in the startup to prevent callbacks before
     * the object is fully initialized.
     */    
    TBool iObserving;
    
    // Destructor key for the ECOM plugin
    TUid                               iDestructorKey;
    
    RArray<TNewMailInfo> iNewInboxEntries;
    
    CNewMailNotificationTimer* iTimer;
    };

#include "fsnotificationhandlerbase.inl"

#endif  //__FSNOTIFICATIONHANDLERBASE_H__
