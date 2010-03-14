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
* Description:  common email object
*
*/

#include "emailtrace.h"
#include <apgcli.h>
#include <apmrec.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy

#include "cfsmailmessage.h"
#include "cfsmailplugin.h"
#include "cfsmailrequesthandler.h"

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFSMailMessage::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage * CFSMailMessage::NewLC(TFSMailMsgId aMessageId)
{
    FUNC_LOG;
  	CFSMailMessage* message = new (ELeave) CFSMailMessage();
  	CleanupStack:: PushL(message);
  	message->ConstructL( aMessageId );
  	return message;
} 

// -----------------------------------------------------------------------------
// CFSMailMessage::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage * CFSMailMessage::NewL(TFSMailMsgId aMessageId)
{
    FUNC_LOG;
  	CFSMailMessage* message =  CFSMailMessage::NewLC(aMessageId);
  	CleanupStack:: Pop(message);
  	return message;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessage::ConstructL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
	iMessageId = aMessageId;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::CFSMailMessage
// -----------------------------------------------------------------------------
CFSMailMessage::CFSMailMessage() : CFSMailMessagePart()
{
    FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::~CFSMailMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage::~CFSMailMessage()
{
    FUNC_LOG;

}

// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessage::SaveMessageL()
	{
    FUNC_LOG;
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
		{
		plugin->StoreMessageL(GetMailBoxId(),*this);
		}
	}

// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::AddNewAttachmentL( RFile& aFile,
                                                                const TDesC8& aMimeType )
    {
    FUNC_LOG;
    
    CFSMailMessagePart* newPart = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        TBuf<KMaxDataTypeLength> contentType;
        if(aMimeType.Length() > 0)
            {
            // use user given content type
            contentType.Copy(aMimeType);
            }
        else
            {
            // try to find out content type using recognizers
            RApaLsSession apaSession;
            TDataRecognitionResult dataType;
            User::LeaveIfError(apaSession.Connect());
            User::LeaveIfError(apaSession.RecognizeData(aFile, dataType));
            apaSession.Close();
            contentType.Copy(dataType.iDataType.Des());
            }
        
        // get file name
        TFileName fileName;
        aFile.FullName( fileName );

        // get new part from plugin
        newPart = plugin->NewChildPartFromFileL(    GetMailBoxId(),
                                                    GetFolderId(),
                                                    GetMessageId(),
                                                    GetPartId(),
                                                    contentType,
                                                    aFile );
        // set attachment name
        newPart->SetAttachmentNameL(fileName);

        // store new message part
        newPart->SaveL();
        
        // set flag
        SetFlag(EFSMsgFlag_Attachments);
        }
    return newPart;
    }
// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::AddNewAttachmentL(	const TDesC& aFilePath,
																const TFSMailMsgId /*aInsertBefore*/ )
    {
    FUNC_LOG;

	CFSMailMessagePart* newPart = NULL;
	
	if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
		{
	    // use recognizer to find out ContentType 
	    RApaLsSession apaSession;
		TDataRecognitionResult dataType;
		TBufC8<KMaxDataTypeLength> buf;
	    User::LeaveIfError(apaSession.Connect());
	    User::LeaveIfError(apaSession.RecognizeData(aFilePath, buf, dataType));
	    apaSession.Close();

	    // Create new message part with correct Content-Type
		TBuf<KMaxDataTypeLength> contentType;
		contentType.Copy(dataType.iDataType.Des());
	    newPart = plugin->NewChildPartFromFileL( 	GetMailBoxId(),
	    											GetFolderId(),
	    											GetMessageId(),
	    											GetPartId(),
	    											contentType,
	    											aFilePath );

		
	    // Set attachment name
	    newPart->SetAttachmentNameL(aFilePath);

		// store message part
		newPart->SaveL();

		// set flag
        SetFlag(EFSMsgFlag_Attachments);
		}
		
    return newPart;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::AttachmentListL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessage::AttachmentListL(
    RPointerArray<CFSMailMessagePart>& aParts )
    {
    FUNC_LOG;
    // First list all message parts
    AppendAttachmentsL( aParts ); 

    // special case, if single part content type is not given,
    // default type is text/plain
    if ( aParts.Count() == 1 && aParts[0]->GetContentType().Length() == 0)
        {
        aParts.ResetAndDestroy();
        }
    else
        {
        // find plain text body part from the list
        CFSMailMessagePart* txtPart = FindBodyPartL(KFSMailContentTypeTextPlain);
        CleanupStack::PushL( txtPart );
        if ( txtPart
          && txtPart->AttachmentNameL().Length() == 0 ) // real attachments have names
            {
            // remove plain text body part from attachment list
            for ( TInt ii = aParts.Count() - 1; ii >= 0; --ii )
                {
                if ( aParts[ii]->GetPartId() == txtPart->GetPartId() )
                    {
                    delete aParts[ii];
                    aParts.Remove(ii);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( txtPart );

        // find  html body part from the list
        CFSMailMessagePart* htmlPart = FindBodyPartL(KFSMailContentTypeTextHtml);
        CleanupStack::PushL( htmlPart );
        if ( htmlPart
          && htmlPart->AttachmentNameL().Length() == 0 ) // real attachments have names
            {
            // remove html body part from attachment list
            for ( TInt ii = aParts.Count() - 1; ii >= 0; --ii )
                {
                if ( aParts[ii]->GetPartId() == htmlPart->GetPartId() )
                    {
                    delete aParts[ii];
                    aParts.Remove(ii);
                    break;
                    }
                }
            }
        CleanupStack::PopAndDestroy( htmlPart );
        }

    iMessageParts.ResetAndDestroy();
    iReadMessageParts = ETrue;
    }

// -----------------------------------------------------------------------------
// CFSMailMessage::PlainTextBodyPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::PlainTextBodyPartL()
{
    FUNC_LOG;
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextPlain);
    
    // special case, single part content type is not given
    if(part == NULL && iMessageParts.Count() == 1 && iMessageParts[0]->GetContentType().Length() == 0)
    	{
    	part = iMessageParts[0];
    	iMessageParts.Remove(0);
    	}
    iMessageParts.ResetAndDestroy();
	iReadMessageParts = ETrue;
    return part;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::HtmlBodyPartL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessagePart* CFSMailMessage::HtmlBodyPartL()
{
    FUNC_LOG;
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextHtml);
    iMessageParts.ResetAndDestroy();
	iReadMessageParts = ETrue;
    return part;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::IsMessageL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessage::IsMessageL() const
	{
    FUNC_LOG;

	TBuf<KMaxDataTypeLength> ptr;
	if ( iContentType )
    {
		ptr.Copy(iContentType->Des());
		TInt length = ptr.Locate(';');
		if(length >= 0)
			{
			ptr.SetLength(length);
			}
		
		if( !ptr.Compare(KFSMailContentTypeMessage) )
		{
			return ETrue;
		}
    }
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CFSMailMessage::ContentTypeParameters()
// -----------------------------------------------------------------------------
CDesCArray& CFSMailMessage::ContentTypeParameters()
{
    FUNC_LOG;
   CDesCArray* faked = NULL;
   return *faked;
}

// -----------------------------------------------------------------------------
// CFSMailMessage::DoAttachmentListL
// -----------------------------------------------------------------------------
void CFSMailMessage::DoAttachmentListL( RPointerArray<CFSMailMessagePart>& aParts )
    {
    FUNC_LOG;
    AttachmentListL(aParts);
    }

