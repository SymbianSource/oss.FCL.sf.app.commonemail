/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: mail message implementation definition
*
*/

#ifndef EMAILMESSAGE_H
#define EMAILMESSAGE_H

#include <memailmessage.h>
#include "cfsmailcommon.h"
#include "emailapiutils.h"

using namespace EmailInterface;

class CFSMailMessagePart;
class CFSMailMessage;
class CFSMailAddress;
class CFSMailPlugin;
class CPluginData;
class CEmailAddress;
class CEmailAttachment;
class CEmailTextContent;
class CEmailMultipart;

NONSHARABLE_CLASS( CEmailMessage ) : public CBase, public MEmailMessage
    {
public:
    /**
    * Creates email message from plugin message
    */
    static CEmailMessage* NewL( CPluginData& aPluginData,
                                CFSMailMessage* aFsMessage,
                                const TDataOwner aOwner );

    ~CEmailMessage();

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();
    
public: // from MEmailAddress
    virtual const TMessageId& MessageId() const;
    
    /**@see MEmailMessage */
    virtual MEmailAddress* SenderAddressL() const;
    
    /**@see MEmailMessage */
    virtual MEmailAddress* ReplyToAddressL() const;
    
    /**@see MEmailMessage */
    virtual void SetReplyToAddressL( const MEmailAddress& aSender );
    
    /**@see MEmailMessage */
    virtual TInt GetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients ) const;            
    
    /**@see MEmailMessage */
    virtual void SetRecipientsL( const MEmailAddress::TRole aRole, REmailAddressArray& aRecipients );
    
    /**@see MEmailMessage */
    virtual void RemoveRecipientL( const MEmailAddress& aRecipient );
    
    /**@see MEmailMessage */
    virtual TPtrC Subject() const;

    /**@see MEmailMessage */
    virtual void  SetSubjectL( const TPtrC& aSubject);    
    
    /**@see MEmailMessage */
    virtual TTime Date() const;

    /**@see MEmailMessage */
    virtual TInt Flags() const;
    
    /**@see MEmailMessage */
    virtual void SetFlag( const TUint aFlag );
    
    /**@see MEmailMessage */
    virtual void ResetFlag( const TUint aFlag );

    /**@see MEmailMessage */
    virtual MEmailMessageContent* ContentL() const;

    /**@see MEmailMessage */
    virtual void SetContentL( const MEmailMessageContent* aContent );
    
    /**@see MEmailMessage */
    virtual void SetPlainTextBodyL( const TDesC& aPlainText );

    /**@see MEmailMessage */
    virtual MEmailAttachment* AddAttachmentL( const TDesC& aFullPath );
    
    /**@see MEmailMessage */
    virtual MEmailAttachment* AddAttachmentL( RFile& aFile );
    
    /**@see MEmailMessage */
    virtual TInt GetAttachmentsL( REmailAttachmentArray& aAttachments );
    
    /**@see MEmailMessage */
    virtual void RemoveAttachmentL( const MEmailAttachment& aAttachment );

    /**@see MEmailMessage */
    virtual const TFolderId& ParentFolderId() const;

    /**@see MEmailMessage */
    virtual void SaveChangesL();

    /**@see MEmailMessage */
    virtual void SendL();

    /**@see MEmailMessage */
    virtual void ShowMessageViewerL();
    
    /**@see MEmailMessage */
    virtual void ReplyToMessageL( const TBool aReplyToAll = ETrue );
    
    /**@see MEmailMessage */
    virtual void ForwardMessageL();
    
private:        

    // Copies/moves flag values from iPluginMessage's flags to local flag member variable
    void InitializeFlagValues();
    
    CEmailMessage( CPluginData& aPluginData, CFSMailMessage* aMessage, const TDataOwner aOwner );
    
    void ConstructL();
							       
    void ConvertAddressArrayL( const MEmailAddress::TRole aRole, 
                               RPointerArray<CFSMailAddress>& aSrc, 
                               REmailAddressArray& aDst ) const; 
    CEmailAddress* CreateAddressLC( const MEmailAddress::TRole aRole, CFSMailAddress& aFsAddress ) const;
    TUint MapFlags( const TUint& aFlag );
        
private:
    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMessageId      iMessageId;
    
    TMessageContentId   iMsgContentId;
                                                                                  
    mutable CEmailAddress*  iSender;
    
    mutable CEmailAddress*  iReplyTo;
    
    REmailAddressArray iRecipients;
    
    CFSMailMessage* iPluginMessage;
    
    TUint        iFlags;
    
    mutable CEmailTextContent *iTextContent;
    mutable CEmailMultipart* iContent;
    RPointerArray<CEmailAttachment> iAttachments;
    TDataOwner iOwner;

    };

#endif // EMAILMESSAGE_H

// End of file
