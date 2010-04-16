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
* Description: This file defines class CIpsPlgSmtpService.
 *
*/


#ifndef IPSPLGSMTPSERVICE_H
#define IPSPLGSMTPSERVICE_H

class CIpsPlgSosBasePlugin;
class CIpsPlgMsgMapper;

/**
 *  Freestyle message send and creation implementation
 *  
 *  Encapsulates message creation and send operations using
 *  Symbian os smtp services.
 *
 *
 *  @lib ipssosplugin.lib
 *  @since FS 1.0
 */
NONSHARABLE_CLASS( CIpsPlgSmtpService ) : 
    public CBase,
// <qmail>
    public MIpsPlgSingleOpWatcher
// </qmail>
    {

public:

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpService* NewL(
        CMsvSession& aSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Symbian 2nd phase construcror
     *
     * @since FS 1.0
     * @return None
     */
    IMPORT_C static CIpsPlgSmtpService* NewLC( 
        CMsvSession& aSession,
        CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Class destructor
     *
     * @since FS 1.0
     * @return None
     */
    virtual ~CIpsPlgSmtpService();

    /**
     * Stores message to symbian message store
     *
     * @param aMailboxId id for corresponding mailbox
     * @param aMessage FS Email message object
     * @param aSession MsvSession
     * @since FS 1.0
     */
    void StoreMessageL(
        TFSMailMsgId aMailBoxId,
        CFSMailMessage& aMessage );

    /**
     * Creates new email message to message store
     *
     * @param aMailBoxId msv entry id to mailbox which setting are used
     * @param aSession Msv Session
     * @since FS 1.0
     * @return pointer created fs message object
     */
    CFSMailMessage* CreateNewSmtpMessageL(
        const TFSMailMsgId& aMailBoxId );

// <qmail>
    /**
     * Creates new email message to message store asynchronously
     *
     * @param aMailBoxId msv entry id to mailbox which setting are used
     * @param aOperationObserver Observer for the operation
     * @param aRequestId Id of the operation
     */
    void CreateNewSmtpMessageL(
        const TFSMailMsgId& aMailBoxId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
// </qmail>

    CFSMailMessage* CreateForwardSmtpMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId );

// <qmail>
    void CreateForwardSmtpMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
// </qmail>
    
    CFSMailMessage* CreateReplySmtpMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        TBool aReplyToAll );

// <qmail>
    void CreateReplySmtpMessageL(
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aOriginalMessageId,
        TBool aReplyToAll,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId );
// </qmail>

// <qmail>
    /**
     * Creates proper fs message object and set flags
     * to correspond orginal message flags
     *
     * @since FS 1.0
     * @param aMessageId id of created message
     * @param aOrginalMsgId id of orginal message
     * @param aMailboxId mailbox id
     * @param aCopyOriginalMsgProperties Copy properties from original message
     * @return TMscId
     */
    CFSMailMessage* CreateFSMessageAndSetFlagsL(
       TMsvId aMessageId, 
       TMsvId aOriginalMsgId, 
       TMsvId aMailboxId,
       TBool aCopyOriginalMsgProperties = EFalse );
// </qmail>

// <qmail>
public: //from MIpsPlgSingleOpWatcher

    void OpCompleted(
        CIpsPlgSingleOpWatcher& aOpWatcher,
        TInt aCompletionCode );
// </qmail>
protected:

    /**
     * Class constructor
     *
     * @since FS 1.0
     * @return None
     */
    CIpsPlgSmtpService( CMsvSession& aSession,
                    CIpsPlgSosBasePlugin& aPlugin );

    /**
     * Constructor for leaving methods
     *
     * @since FS 1.0
     * @return None
     */
    void ConstructL();

private:

    /**
     * Combines alias and email address to one string
     * format : Alias<email_address>
     *
     * @since FS 1.0
     * @param TDesC& aAliasName alias
     * @param TDesC& aEmailName email address
     * @return result of combination
     */
    HBufC* CreateDisplayStringLC( const TDesC& aDisplayName, 
                              const TDesC& aEmailName );

    /**
     * Changes messages service id
     *
     * @since FS 1.0
     * @param aEntry Original message entry 
     * @return None
     */
    void ChangeServiceIdL( TMsvEntry& aEntry );
    
    /**
     * Gets MsvId from msv operations final progress 
     * descriptor, leaves if msvId is null entry
     *
     * @since FS 1.0
     * @param aProg progress descriptor
     * @return TMscId
     */
    inline static TMsvId GetIdFromProgressL( const TDesC8& aProg );

    /**
     * Gets character set from original message
     * @param aOriginalMsgId id of original message
     * */
    TUid GetOriginalCharsetL( TMsvId aOriginalMsgId );
    
    /**
     * Sets the charset for outgoing messages 
     * @param aEntry message where the charset is to be set
     * @param aCharset the charset to be set
     */
    void SetCharactersetL( CMsvEntry& aEntry, TUid aCharset );

// <qmail>
    /**
    * Cancel, delete and remove operation from iOperations array.
    * Send Sync Completed event to plugin if operation is sync op
    */
    void DeleteAndRemoveOperation( const TInt aOpArrayIndex );
// </qmail>

private: // data

    CIpsPlgSosBasePlugin& iPlugin;

    CMsvSession& iSession;
    
    CIpsPlgMsgMapper* iMsgMapper;

// <qmail> array of operation watchers
    RPointerArray<CIpsPlgSingleOpWatcher>   iOperations;
// </qmail>
    };

#endif /* IPSPLGSMTPSERVICE_H*/

// End of File
