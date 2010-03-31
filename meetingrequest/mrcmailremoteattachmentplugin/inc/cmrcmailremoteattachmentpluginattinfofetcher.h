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

#ifndef CMRCMAILREMOTEATTACHMENTPLUGINATTINFOFETCHER_H
#define CMRCMAILREMOTEATTACHMENTPLUGINATTINFOFETCHER_H

#include <e32base.h>
#include <bamdesca.h> 

// FORWARD DECLARATIONS
class MCalRemoteAttachmentInfo;
class CFSMailClient;
class CFSMailBox;
class CFSMailMessage;
class TMRCMailRemoteAttachmentPluginURLParser;

/**
 * CMRCmailRemoteAttachmentPluginAttinfoFetcher is responsible for
 * fetching attachment information from the email message.
 */
NONSHARABLE_CLASS( CMRCmailRemoteAttachmentPluginAttinfoFetcher ) :
        public CBase
    {
public: // Construction and destruction
    
    /**
     * Creates and initializes new CMRCmailRemoteAttachmentPluginAttinfoFetcher
     * object. Ownership is transferred to caller.
     * 
     * @param aMailClient Reference to CMAIL mail client object.
     * @return Pointer to CMRCmailRemoteAttachmentPluginAttinfoFetcher object.
     */
    static CMRCmailRemoteAttachmentPluginAttinfoFetcher* NewL(
            CFSMailClient& aMailClient );
    
    /**
     * C++ destructor.
     */
    ~CMRCmailRemoteAttachmentPluginAttinfoFetcher();
    
public: // Interface
    
    /**
     * Fetches attachment information from the CMail email message
     */
    void FetchAttachmentInformationL(
            MCalRemoteAttachmentInfo& aAttachmentInfo,
            MDesCArray& aAttachmentUrlArray );
    
private: // Implementation
    CMRCmailRemoteAttachmentPluginAttinfoFetcher(
                CFSMailClient& aMailClient );
    void ConstructL();
    void AddAttachmentInformationL(
            TMRCMailRemoteAttachmentPluginURLParser& aParser,
            const TDesC& aUri,
            MCalRemoteAttachmentInfo& aAttachmentInfo );
    
private: // Data
    /// Ref: Reference to mail client
    CFSMailClient& iMailClient;
    };

#endif // CMRCMAILREMOTEATTACHMENTPLUGINATTINFOFETCHER_H

// EOF
