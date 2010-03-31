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
 * Description:  
 *
 */

#include "cmrcmailremoteattachmentpluginattinfofetcher.h"
#include "tmrcmailremoteattachmentpluginurlparser.h"
#include "mcalremoteattachmentinfo.h"
#include "mcalremoteattachment.h"
#include "cfsmailclient.h"
#include "emailtrace.h"

#include <ct/rcpointerarray.h>

// Unnamed namespace for local definitions
namespace { // namespace::codescanner

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::CMRCmailRemoteAttachmentPluginAttinfoFetcher
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPluginAttinfoFetcher::CMRCmailRemoteAttachmentPluginAttinfoFetcher(
            CFSMailClient& aMailClient )
:   iMailClient( aMailClient )
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::~CMRCmailRemoteAttachmentPluginAttinfoFetcher
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPluginAttinfoFetcher::~CMRCmailRemoteAttachmentPluginAttinfoFetcher()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::NewL
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPluginAttinfoFetcher* 
        CMRCmailRemoteAttachmentPluginAttinfoFetcher::NewL(
            CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    
    CMRCmailRemoteAttachmentPluginAttinfoFetcher* self = 
            new (ELeave) CMRCmailRemoteAttachmentPluginAttinfoFetcher( 
                            aMailClient );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::ConstructL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPluginAttinfoFetcher::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::FetchAttachmentInformationL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPluginAttinfoFetcher::FetchAttachmentInformationL(
        MCalRemoteAttachmentInfo& aAttachmentInfo,
        MDesCArray& aAttachmentUrlArray )
    {
    FUNC_LOG;
    
    TInt attachmentInfoCount( aAttachmentUrlArray.MdcaCount() );
            
    for ( TInt i(0); i <  attachmentInfoCount; ++i )
        {
        TMRCMailRemoteAttachmentPluginURLParser mailIdParser;
        
        TPtrC uri( aAttachmentUrlArray.MdcaPoint(i) );
        
        TInt err = mailIdParser.Parse( uri );        
        User::LeaveIfError( err );
        
        AddAttachmentInformationL( mailIdParser, uri, aAttachmentInfo );        
        }
    
    
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPluginAttinfoFetcher::FetchAttachmentInformationL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPluginAttinfoFetcher::AddAttachmentInformationL(
        TMRCMailRemoteAttachmentPluginURLParser& aParser,
        const TDesC& aUri,
        MCalRemoteAttachmentInfo& aAttachmentInfo )
    {
    FUNC_LOG;
    
    // Fetch message
    CFSMailMessage* message = iMailClient.GetMessageByUidL(
            aParser.iMailboxUid, 
            aParser.iFolderUid,
            aParser.iMessageUid, 
            EFSMsgDataStructure );
    
    CleanupStack::PushL( message );
    
    CFSMailMessagePart* attachment = 
            message->ChildPartL( aParser.iMessagepartUid );
    CleanupStack::PushL( attachment );
    
    TPtrC attachmentName( attachment->AttachmentNameL() );
    TInt attachmentSize( attachment->ContentSize() );
    
    MCalRemoteAttachment::TCalRemoteAttachmentState attachmentState(
            MCalRemoteAttachment::EAttachmentStateNotDownloaded );
    
    if ( EFSFull == attachment->FetchLoadState() )
        {
        attachmentState = MCalRemoteAttachment::EAttachmentStateDownloaded;
        }
    
    aAttachmentInfo.AddAttachmentInfoL(
            aUri,
            attachmentName,
            attachmentSize,
            attachmentState );
    
    CleanupStack::PopAndDestroy( attachment );
    CleanupStack::PopAndDestroy( message );
    }

// EOF
