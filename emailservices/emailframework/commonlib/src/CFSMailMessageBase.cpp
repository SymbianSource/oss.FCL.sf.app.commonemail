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
* Description:  common email base object
*
*/


#include "cfsmailrequesthandler.h"
#include "emailtrace.h"
#include "cfsmailmessagebase.h"
#include "cmailmessageext.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewLC( TFSMailMsgId aMessageId )
{
  FUNC_LOG;
  CFSMailMessageBase* message = new (ELeave) CFSMailMessageBase();
  CleanupStack:: PushL(message);
  message->ConstructL(aMessageId);
  return message;
} 

// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewL( TFSMailMsgId aMessageId )
{
  FUNC_LOG;
  CFSMailMessageBase* message =  CFSMailMessageBase::NewLC(aMessageId);
  CleanupStack:: Pop(message);
  return message;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::CFSMailMessageBase
// -----------------------------------------------------------------------------
CFSMailMessageBase::CFSMailMessageBase()
{
    FUNC_LOG;

	iSubject = HBufC::New(1);
	iSubject->Des().Copy(KNullDesC());

  	iSender = NULL;
  	iFlags = 0;
  	// set request handler pointer
	iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessageBase::ConstructL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
	iMessageId = aMessageId;    
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::~CFSMailMessageBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase::~CFSMailMessageBase()
{
    FUNC_LOG;
	if(iSender)
		{
		delete iSender;
		iSender = NULL;
		}

	if(iSubject)
		{
		delete iSubject;
		iSubject = NULL;
		}

	// clear recipients lists
	iToRecipients.ResetAndDestroy();		
	iCcRecipients.ResetAndDestroy();
	iBccRecipients.ResetAndDestroy();

}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMessageId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMessageId(  ) const
{
    FUNC_LOG;
	return iMessageId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetFolderId( ) const
{
    FUNC_LOG;
	return iFolderId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFolderId( const TFSMailMsgId aFolderId )
{
    FUNC_LOG;
	iFolderId = aFolderId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMailBoxId( ) const
{
    FUNC_LOG;
	return iMailBoxId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetMailBoxId( const TFSMailMsgId aMailBoxId )
{
    FUNC_LOG;
	iMailBoxId = aMailBoxId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSender
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSender(CFSMailAddress* aSender)
{
    FUNC_LOG;
	// store sender
	if (iSender)
		{
		delete iSender;
		}
	iSender = aSender;

}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSender
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailMessageBase::GetSender() const
{
    FUNC_LOG;
	return iSender;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetToRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress>&  CFSMailMessageBase::GetToRecipients()
{
	return iToRecipients;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress>&  CFSMailMessageBase::GetCCRecipients( )
{
	return iCcRecipients;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetBCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress>&  CFSMailMessageBase::GetBCCRecipients( )
{
	return iBccRecipients;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendToRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendToRecipient(CFSMailAddress* aRecipient)
{
    FUNC_LOG;
	iToRecipients.Append(aRecipient);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendCCRecipient(CFSMailAddress* aRecipient )
{
    FUNC_LOG;
	iCcRecipients.Append(aRecipient);	
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendBCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendBCCRecipient( CFSMailAddress* aRecipient )
{
    FUNC_LOG;
	iBccRecipients.Append(aRecipient);			
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearToRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearToRecipients( )
	{
    FUNC_LOG;
		iToRecipients.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearCcRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearCcRecipients( )
	{
    FUNC_LOG;
		iCcRecipients.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearBccRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearBccRecipients( )
	{
    FUNC_LOG;
		iBccRecipients.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSubject
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailMessageBase::GetSubject() const
{
    FUNC_LOG;
	return *iSubject;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetDate
// -----------------------------------------------------------------------------
EXPORT_C TTime CFSMailMessageBase::GetDate() const
{
    FUNC_LOG;
  return iDate;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetDate
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetDate( const TTime aDate )
{
    FUNC_LOG;
	iDate = aDate;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSubject
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSubject(const TDesC& aSubject)
{
    FUNC_LOG;
	// init mailbox name
	HBufC* subject = HBufC::New(aSubject.Length());
	
	// store new mailbox name
	if(subject)
	{
		delete iSubject;
		iSubject = subject;
		iSubject->Des().Copy(aSubject);
	}

}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFlags
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessageBase::GetFlags( ) const
{
    FUNC_LOG;
	return iFlags;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFlag(const TInt aFlag)
{
    FUNC_LOG;
	iFlags |= aFlag;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ResetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ResetFlag(const TInt aFlag)
{
    FUNC_LOG;
	iFlags &= ~aFlag;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::IsFlagSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessageBase::IsFlagSet(const TInt aFlag) const
{
    FUNC_LOG;
	if(iFlags & aFlag)
		{
		return ETrue;
		}
	else
	{
		return EFalse;
	}
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::IsRelatedTo
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::IsRelatedTo() const
	{
    FUNC_LOG;
	return iRelatedTo;
	}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetRelatedTo
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetRelatedTo( const TFSMailMsgId aMessageId )
	{
    FUNC_LOG;
	iRelatedTo = aMessageId;
	}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetReplyToAddress(CFSMailAddress* aReplyToAddress)
{
    FUNC_LOG;
	// store sender
	if (iReplyTo)
		{
		delete iReplyTo;
		}
	iReplyTo = aReplyToAddress;

}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C const CFSMailAddress& CFSMailMessageBase::GetReplyToAddress()
{
    FUNC_LOG;
	return *iReplyTo;
}

EXPORT_C CFSMailRequestHandler& CFSMailMessageBase::RequestHandler( )
{
	return *iRequestHandler;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ReleaseExtension( CEmailExtension* aExtension )
    {
    FUNC_LOG;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(iMessageId) )
        {
        // If plugin has created the extension, let it handle destruction.
        plugin->ReleaseExtension( aExtension );
        }
    else
        {
        CExtendableEmail::ReleaseExtension( aExtension );
        }
    }
    
// -----------------------------------------------------------------------------
// CFSMailMessageBase::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailMessageBase::ExtensionL( 
    const TUid& aInterfaceUid )
    {
    FUNC_LOG;
    CEmailExtension* ext = CExtendableEmail::ExtensionL( aInterfaceUid );
    // didn't find already created instance, try now
    if ( !ext )
        {
        // check that plugin supports requested extension.
        if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(iMessageId) )
            {
            // request extension from plugin, leaves if not supported
            ext = plugin->ExtensionL( aInterfaceUid );            
            }

        }
    return ext;
    }
