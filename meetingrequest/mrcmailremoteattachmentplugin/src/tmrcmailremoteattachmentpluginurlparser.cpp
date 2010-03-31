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

#include "tmrcmailremoteattachmentpluginurlparser.h"
#include "cfsmailcommon.h"
#include "emailtrace.h"

#include <uri16.h>
#include <delimitedpath16.h>

// Unnamed namespace for local definitions
namespace { // namespace::codescanner

// Length for path buffer
const TInt KPathBufferLen( 64 );

// Mail ID separator
_LIT( KMailIDSepararor, "." );

} // namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TMRCMailRemoteAttachmentPluginURLParser::TMRCMailRemoteAttachmentPluginURLParser
// ---------------------------------------------------------------------------
//
TMRCMailRemoteAttachmentPluginURLParser::TMRCMailRemoteAttachmentPluginURLParser()
    {
    FUNC_LOG;    
    }

// ---------------------------------------------------------------------------
// TMRCMailRemoteAttachmentPluginURLParser::~TMRCMailRemoteAttachmentPluginURLParser
// ---------------------------------------------------------------------------
//
TMRCMailRemoteAttachmentPluginURLParser::~TMRCMailRemoteAttachmentPluginURLParser()
    {
    FUNC_LOG;    
    }

// ---------------------------------------------------------------------------
// TMRCMailRemoteAttachmentPluginURLParser::Parse
// ---------------------------------------------------------------------------
//
TInt TMRCMailRemoteAttachmentPluginURLParser::Parse( 
        const TDesC& aUri )
    {
    FUNC_LOG;
    
    TInt err( KErrNone );
    
    iMailboxUid.SetNullId(); 
    iFolderUid.SetNullId();
    iMessageUid.SetNullId();
    iMessagepartUid.SetNullId();
    
    // Parse URI cmail://mailboxuid/folder/message/messagepart/
    // mailboxuid will be the host and rest is the folder structure
    //
    TUriParser16 uriparser;
    TInt error = uriparser.Parse( aUri );
    
    // First we will get mailbox UID
    TPtrC mailboxuid( uriparser.Extract( EUriHost ) );
    ParseMailId( mailboxuid );
    
    TDelimitedPathParser16 pathParser;    
    TPtrC uripath( uriparser.Extract( EUriPath ) );
    pathParser.Parse( uripath );
    
    TBuf<KPathBufferLen> pathBuffer;
    TPtrC pathBufferPtr( pathBuffer );
    err = pathParser.Peek( pathBufferPtr );
    while ( pathBufferPtr.Length() && err == KErrNone )
        {
        err = ParseMailId( pathBufferPtr );
        pathParser.Inc();
        err = pathParser.Peek( pathBufferPtr );        
        }
    
    if ( iMailboxUid.IsNullId() || 
         iFolderUid.IsNullId() ||
         iMessageUid.IsNullId() ||
         iMessagepartUid.IsNullId() )
        {
        // Not all UIDs were found
        err = KErrNotFound;
        }
     
    return err;
    }

// ---------------------------------------------------------------------------
// TMRCMailRemoteAttachmentPluginURLParser::Parse
// ---------------------------------------------------------------------------
//
void TMRCMailRemoteAttachmentPluginURLParser::SetMailId(
        TFSMailMsgId& aMailId )
    {
    FUNC_LOG;
    
    if ( iMailboxUid.IsNullId() )
        {
        iMailboxUid = aMailId;
        }
    else if ( iFolderUid.IsNullId() )
        {
        iFolderUid = aMailId;
        }
    else if ( iMessageUid.IsNullId() )
        {
        iMessageUid = aMailId;
        }
    else if ( iMessagepartUid.IsNullId() )
        {
        iMessagepartUid = aMailId;
        }    
    }

// ---------------------------------------------------------------------------
// TMRCMailRemoteAttachmentPluginURLParser::ParseMailId
// ---------------------------------------------------------------------------
//
TInt TMRCMailRemoteAttachmentPluginURLParser::ParseMailId( 
        TPtrC& aMailId )
    {
    FUNC_LOG;
    
    TInt err( KErrNone );
    TInt pos( KErrNotFound );
    
    // Now we have pathpart in buffer. Each pathpart contains
    // TFSMailMsgId if following format: pluginuid.pluginid
    // We need to parse that further to plugin UID and plugin ID
    pos = aMailId.Find( KMailIDSepararor );
    if ( pos != KErrNotFound )
        {
        TPtrC mailUidStr( aMailId.Left( pos ) );
        TPtrC mailIdStr( aMailId.Mid( pos + 1 ) );
        
        TUint pluginUid(0);
        TLex numberParser( mailUidStr );
        err = numberParser.Val(pluginUid);
        
        if ( KErrNone == err )
            {
            // Then parse plugin ID
            TUint pluginId(0);
            numberParser = mailIdStr;
            err = numberParser.Val(pluginId);
            
            TFSMailMsgId mailId( pluginUid, pluginId );
            SetMailId( mailId );
            }
        }
    
    return err;
    }

// EOF
