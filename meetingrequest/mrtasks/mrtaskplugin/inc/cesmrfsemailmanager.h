/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class encapsulating FS email sending
*
*/


#ifndef C_ESMRFSEMAILMANAGER_H
#define C_ESMRFSEMAILMANAGER_H

#include <e32base.h>
//<cmail>
#include "cfsmailcommon.h"
#include "mmrinfoprocessor.h"
#include "esmrdef.h"
//</cmail>

class CFSMailBox;
class CFSMailMessage;
class CFSMailMessagePart;
class CFSMailClient;
class CMRMailboxUtils;
class TESMRInputParams;
class RFile;

/**
 *  Class definition for FS Email manager class. This class encapsulates
 *  all mail sending acitivities with FS EMail framework.
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CESMRFSEMailManager ) : public CBase
    {
public:
    /** Enumeration for text/calendar part */
    enum TESMRMethod
        {
        EESMRMethodRequest,     // Request
        EESMRMethodResponse,    // response
        EESMRMethodCancel       // cancellation
        };

public: // Construction and destruction
    /**
     * Creates and initializes new CESMRSendMRRespFSEmailTask object.
     * Ownership is transferred to caller.
     * @param aMRMailboxUtils Reference to mailbox utilities
     */
    static CESMRFSEMailManager* NewL(
            CMRMailboxUtils& aMRMailboxUtils );

    /**
     * C++ destructor.
     */
    ~CESMRFSEMailManager();

    /**
     * Prepares email manager to be used for sending meeting
     * request.
     *
     * @param aMailboxOwnerAddr mailbox owner's email address
     */
    void PrepareForSendingL(
            const TDesC& aMailboxOwnerAddr );
    
    /**
     * Prepares email manager to be used for sending reply
     * message.
     *
     * @param aMailboxOwnerAddr mailbox owner's email address
     * @param aMsgId original message's id
     * @param aReplyAll is this reply for all participants
     */
    void PrepareForSendingReplyL(
            const TDesC& aMailboxOwnerAddr,
            const TFSMailMsgId& aMsgId,
            TBool aReplyAll );

    /**
     * Sets sender to email message
     * @param aAddress Sender's email address
     * @param aCommonName Sender's common name
     */
    void SetSenderL(
            const TDesC& aAddress,
            const TDesC& aCommonName );

    /**
     * Sets reply to address to email message
     * @param aAddress Email address
     * @param aCommonName Common name
     */
    void SetReplyToAddressL(
            const TDesC& aAddress,
            const TDesC& aCommonName );
    
    /**
     * Sets recipient to <to> field
     * @param aAddress Recipient's email address
     * @param aCommonName Recipient's common name
     */
    void AppendToRecipientL(
            const TDesC& aAddress,
            const TDesC& aCommonName );

    /**
     * Sets recipient to <cc> field
     * @param aAddress Recipient's email address
     * @param aCommonName Recipient's common name
     */
    void AppendCCRecipientL(
            const TDesC& aAddress,
            const TDesC& aCommonName );

    /**
     * Sets subject to email message
     * @param aSubject Email message's subject.
     */
    void SetSubjectL(
            const TDesC& aSubject);

    /**
     * Sets plain text part to email message.
     * @param aPlainText Plain text context
     */
    void CreateTextPlainPartL(
            const TDesC& aPlainText);

    /**
     * Sets calendar part to email message.
     * @param aCalendarMethod Calendar entry method.
     */
    void CreateTextCalendarPartL(
            TESMRMethod aCalendarMethod );

    /**
     * Sets calendar part to email message.
     * @param aCalendarMethod Calendar entry method.
     * @param aFilename File containing calendar part
     */
    void CreateTextCalendarPartL(
            TESMRMethod aCalendarMethod,
            const TDesC& aFilename );

    /**
     * Adds attachment to email message.
     * @param aAttachmentFile attachment filename.
     */
    void SetAttachmentL(
            const TDesC& aAttachmentFile );
    
    /**
     * Adds attachment to email message.
     * @param aFile Reference to attachment file
     * @param aMimeType attachment mimetype
     */
    void SetAttachmentL(
            RFile& aFile,
            const TDesC8& aMimeType );

    /**
     * Stores message to drafts folder.
     */
    TInt StoreMessageToDraftsFolderL();

    /**
     * Sends message.
     */
    TInt SendMessageL();

    /**
     *  Fetches mailbox id.
     *  @return Mailbox id
     */
    TFSMailMsgId MailboxId() const;

    /**
     * Send MRINFO object via mail framework
     * @param aParam Reference to ESMR input parameters
     * @param aInfoObject Reference to MRINFO object
     * @param aResponseMode Response mode definition.
     * @param aFreeResponseText Free response text
     */
    void SendMailViaSyncL(
            TESMRInputParams& aParams,
            MMRInfoObject& aInfoObject,
            MMRInfoProcessor::TMRInfoResponseMode aResponseMode,
            const TDesC& aFreeResponseText );

    /**
     *  Fetches message id.
     *  @return Message id
     */
    TFSMailMsgId MessageId() const;

    /**
     *  Fetches message folder id.
     *  @return Message folder id
     */
    TFSMailMsgId MessageFolderId() const;

private:
    CESMRFSEMailManager(
            CMRMailboxUtils& aMRMailboxUtils );

    void ConstructL();

    CFSMailBox* SelectMailBoxL(
            const TDesC& aMailboxOwnerAddr );

    CFSMailMessage* CreateMessageL();
    
    

    CFSMailMessagePart* CreateParentPartL();

    void AddPlainTextPartL(
            const TDesC& aContent );

    void AddMessagePartFromFileL(
            const TDesC& aContentType,
            const TDesC& aContentClass,
            const TDesC& aContentDescription,
            const TDesC& aContentDisposition,
            const TDesC& aFileAndPath );

private: // Data
    /**
    * Reference to mailbox utilites
    * Not own.
    */
    CMRMailboxUtils& iMRMailboxUtils;

    /**
    * FS EMail client
    * Own.
    */
    CFSMailClient* iMailClient;

    /**
    * FS EMailBox
    * Own.
    */
    CFSMailBox*    iMailBox;

    /**
    * FS Mail message
    * Own.
    */
    CFSMailMessage* iMessage;

    /**
    * Parent message part
    * Own.
    */
    CFSMailMessagePart* iParentPart;
    };

#endif  // C_ESMRFSEMAILMANAGER_H
