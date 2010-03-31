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

#include "cmrcmailremoteattachmentplugin.h"
#include "cmrcmailremoteattachmentpluginattinfofetcher.h"
#include "cfsmailclient.h"
#include "mcalremoteattachmentinfo.h"
#include "cmrcmailremoteattachmentplugindownloader.h"
#include "mcalremoteattachment.h"

#include "emailtrace.h"

#include <f32file.h>
#include <bamdesca.h> 

// Unnamed namespace for local definitions
namespace { // namespace::codescanner

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::CMRCmailRemoteAttachmentPlugin
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPlugin::CMRCmailRemoteAttachmentPlugin()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::~CMRCmailRemoteAttachmentPlugin
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPlugin::~CMRCmailRemoteAttachmentPlugin()
    {
    FUNC_LOG;
    
    delete iAttinfoFetcher;
    
    if ( iMailClient )
        {
        // DEcrement mail client reference count
        iMailClient->Close();
        iMailClient = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::NewL
// ---------------------------------------------------------------------------
//
CMRCmailRemoteAttachmentPlugin* CMRCmailRemoteAttachmentPlugin::NewL()
    {
    FUNC_LOG;
    
    CMRCmailRemoteAttachmentPlugin* self = 
            new (ELeave) CMRCmailRemoteAttachmentPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPlugin::ConstructL()
    {
    FUNC_LOG;
    
    iMailClient = CFSMailClient::NewL();
    
    iAttinfoFetcher = 
            CMRCmailRemoteAttachmentPluginAttinfoFetcher::NewL( 
                    *iMailClient );
    }

// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::AttachmentInfo
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPlugin::AttachmentInfo(
            MCalRemoteAttachmentInfo& aAttachmentInfo,
            MDesCArray& aAttachmentUrlArray ) const
    {    
    FUNC_LOG;
        
    TRAP_IGNORE( iAttinfoFetcher->FetchAttachmentInformationL(
            aAttachmentInfo, 
            aAttachmentUrlArray ) )
    }
    
// ---------------------------------------------------------------------------
// CMRCmailRemoteAttachmentPlugin::DownloadAttachmentsL
// ---------------------------------------------------------------------------
//
void CMRCmailRemoteAttachmentPlugin::DownloadAttachmentsL(
            const RPointerArray<MCalRemoteAttachment>& aAttachments,
            RPointerArray<MCalRemoteAttachmentOperation>& aOperations,
            MCalRemoteAttachmentOperationObserver& aObserver )
    {
    FUNC_LOG;
    
    TInt attachmentCount( aAttachments.Count() );
    aOperations.ReserveL( attachmentCount );
    for ( TInt i(0); i < attachmentCount; ++i )
        {
        CMRCmailRemoteAttachmentDownloader* downloadCommand = 
                CMRCmailRemoteAttachmentDownloader::NewL(
                            *iMailClient,
                            aObserver,
                            aAttachments[i]->AttachmentIdentifier() );
        
        // Ownership transferred
        aOperations.AppendL( downloadCommand );
        }
    }

// EOF

