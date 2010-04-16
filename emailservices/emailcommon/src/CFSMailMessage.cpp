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

#include <nmcommonheaders.h>
#include "emailtrace.h"
#include <apgcli.h>
#include <apmrec.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy

//<cmail>
#include "CFSMailMessage.h"
#include "CFSMailPlugin.h"
//</cmail>
#include "CFSMailRequestHandler.h"
#include "CFSMailRequestObserver.h"

//<qmail>
#include "nmmessage.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFSMailMessage::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage * CFSMailMessage::NewLC( TFSMailMsgId aMessageId )
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
EXPORT_C CFSMailMessage * CFSMailMessage::NewL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
    CFSMailMessage* message =  CFSMailMessage::NewLC(aMessageId);
    CleanupStack:: Pop(message);
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailMessage::NewL( const NmMessage &aMessage )
{
    FUNC_LOG;
    CFSMailMessage* self = new (ELeave) CFSMailMessage();
    CleanupStack::PushL(self);
    self->ConstructL(aMessage);
    CleanupStack::Pop(self);
    return self;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessage::ConstructL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
    // Base class initialization
    CFSMailMessagePart::ConstructL( aMessageId, TFSMailMsgId() );
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessage::ConstructL( const NmMessage &aMessage )
{
    FUNC_LOG;
    // Base class initialization
    
    CFSMailMessagePart::ConstructL( aMessage, aMessage.envelope() );
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailMessage::CFSMailMessage
// -----------------------------------------------------------------------------
CFSMailMessage::CFSMailMessage()
{
    FUNC_LOG;
    // set request handler pointer
    iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailMessage::~CFSMailMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage::~CFSMailMessage()
{
    FUNC_LOG;
    iMessageBufferForAsyncOp.Reset();
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

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::SaveMessageL( MFSMailRequestObserver& aOperationObserver )
{
    FUNC_LOG;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId());
    TFSPendingRequest request = iRequestHandler->InitAsyncRequestL(
            GetMessageId().PluginId(), aOperationObserver);
    MFSMailRequestObserver* observer = request.iObserver;
    if (plugin)
        {
        iMessageBufferForAsyncOp.Reset();
        iMessageBufferForAsyncOp.AppendL(this);
        // It saves only message header - notice that iMessageBufferForAsyncOp ownership is not transferred
        // to plugin - array contains 'self' pointer so it should not be destroy by plugin
        plugin->StoreMessagesL(GetMailBoxId(), iMessageBufferForAsyncOp,
                *observer, request.iRequestId);
        }

    return request.iRequestId;
}
//</qmail>

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::SaveMessagePartsL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::SaveMessagePartsL( MFSMailRequestObserver& aOperationObserver )
{
    FUNC_LOG;

    RPointerArray<CFSMailMessagePart> messageParts;
    CleanupResetAndDestroyPushL(messageParts);

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId());
    TFSPendingRequest request = iRequestHandler->InitAsyncRequestL(
            GetMessageId().PluginId(), aOperationObserver);
    MFSMailRequestObserver* observer = request.iObserver;

    //Message part storer can be used to store attachements as well
    //AttachmentListL(messageParts);

    CFSMailMessagePart *plainTextPart = PlainTextBodyPartL(EDataSourceLocal);
    if (plainTextPart)
        {
        messageParts.AppendL(plainTextPart);
        }
    CFSMailMessagePart *htmlPart = HtmlBodyPartL(EDataSourceLocal);
    if (htmlPart)
        {
        messageParts.AppendL(htmlPart);
        }

    if (plugin)
        {
        // Ownership of parts array is transferred to plugin - it will take care about reseting them
        plugin->StoreMessagePartsL(messageParts, *observer,
                request.iRequestId);
        // message parts array removed from CleanupStack (not destroyed)
        CleanupStack::Pop(1, &messageParts);
        }
    else
        {
        // message parts array removed from CleanupStack and destroyed
        CleanupStack::PopAndDestroy(1, &messageParts);
        }

    return request.iRequestId;
}
//<//qmail>

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
EXPORT_C CFSMailMessagePart* CFSMailMessage::AddNewAttachmentL( const TDesC& aFilePath,
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
        newPart = plugin->NewChildPartFromFileL( GetMailBoxId(),
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

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::AddNewAttachmentL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessage::AddNewAttachmentL( const TDesC& aFilePath,
                                                 MFSMailRequestObserver& aOperationObserver)
{
    FUNC_LOG;
    TFSPendingRequest request;
  
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetMessageId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetMessageId().PluginId(),
                                                      aOperationObserver );
        
        MFSMailRequestObserver* observer = request.iObserver;
        
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

        TRAPD( err, plugin->NewChildPartFromFileL( 
            GetMailBoxId(),
            GetFolderId(),
            GetMessageId(),
            GetPartId(),
            contentType,
            aFilePath,
            *observer,
            request.iRequestId) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }    
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
}
// </qmail>

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
        if ( txtPart )
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
        if ( htmlPart )
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
EXPORT_C CFSMailMessagePart* CFSMailMessage::PlainTextBodyPartL(
        TFSMailMessagePartDataSource aDataSource)
{
    FUNC_LOG;
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextPlain, aDataSource);
    
    // special case, single part content type is not given
    // <qmail>
    if(!part && iMessageParts.Count() == 1 && iMessageParts[0]->GetContentType().Length() == 0)
    // </qmail>
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
EXPORT_C CFSMailMessagePart* CFSMailMessage::HtmlBodyPartL(
        TFSMailMessagePartDataSource aDataSource)
{
    FUNC_LOG;
    CFSMailMessagePart* part = FindBodyPartL(KFSMailContentTypeTextHtml, aDataSource);
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
    return CFSMailMessagePart::IsMessageL();
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

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessage::GetNmMessage
// It does not fill the list of child parts
// -----------------------------------------------------------------------------
EXPORT_C NmMessage* CFSMailMessage::GetNmMessage()
{
	NmMessage* message = new NmMessage(iNmPrivateMessagePart);
	message->setEnvelope(iNmPrivateMessageEnvelope);
	return message;
}
//</qmail>
