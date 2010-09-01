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

#ifndef TMRCMAILREMOTEATTACHMENTPLUGINURLPARSER_H
#define TMRCMAILREMOTEATTACHMENTPLUGINURLPARSER_H

#include <e32base.h>
#include "cfsmailcommon.h"

/**
 * TMRCMailRemoteAttachmentPluginURLParser is responsible for
 * parsing CMAIUL attachment URI to mail message UIDs.
 */
NONSHARABLE_CLASS( TMRCMailRemoteAttachmentPluginURLParser )
    {
public: // Construction and destruction
    
    /**
     * Default C++ constructor
     */
    TMRCMailRemoteAttachmentPluginURLParser();
    
    /**
     * C++ destructor
     */
    ~TMRCMailRemoteAttachmentPluginURLParser();
    
public: // Interface
    
    /**
     * Parses CMAIL attachment URI to CMAIL message IDs.
     * @param  aUri URI to be parsed
     * @return KErrNone if URI was parsed succesfully.
     *         Otherwise system wide error code.
     */
    TInt Parse( const TDesC& aUri );
    
private: // Implementation    
    void SetMailId( TFSMailMsgId& aMailId );    
    TInt ParseMailId( TPtrC& aMailId );
    
public: // Data
    /// Own: Mailbox UID    
    TFSMailMsgId iMailboxUid;
    /// Owm: Folder UID
    TFSMailMsgId iFolderUid;
    /// Own: Message UID
    TFSMailMsgId iMessageUid;
    /// Own: Messagepart UID
    TFSMailMsgId iMessagepartUid;
    };

#endif // TMRCMAILREMOTEATTACHMENTPLUGINURLPARSER_H

// EOF
