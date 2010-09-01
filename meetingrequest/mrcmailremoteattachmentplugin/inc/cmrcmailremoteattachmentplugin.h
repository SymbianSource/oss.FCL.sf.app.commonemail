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

#ifndef CMRCMAILREMOTEATTPLUGIN_H
#define CMRCMAILREMOTEATTPLUGIN_H

#include <e32base.h>
#include "ccalremoteattachmentapi.h"

class CFSMailClient;
class CMRCmailRemoteAttachmentPluginAttinfoFetcher;

/**
 * CMRCmailRemoteAttachmentPlugin is responsible for handling
 * MR attachment downloading from MR email messages.
 */
NONSHARABLE_CLASS( CMRCmailRemoteAttachmentPlugin ) : 
        public CCalRemoteAttachmentApi
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CMRCmailRemoteAttachmentPlugin object.
     * @return Pointer to CMRCmailRemoteAttachmentPlugin
     */
    static CMRCmailRemoteAttachmentPlugin* NewL();
    
    /**
     * C++ destructor
     */
    ~CMRCmailRemoteAttachmentPlugin();
    
protected: // From CCalRemoteAttachmentApi
    void AttachmentInfo(
            MCalRemoteAttachmentInfo& aAttachmentInfo,
            MDesCArray& aAttachmentUrlArray ) const;
    
    void DownloadAttachmentsL(
                const RPointerArray<MCalRemoteAttachment>& aAttachments,
                RPointerArray<MCalRemoteAttachmentOperation>& aOperations,
                MCalRemoteAttachmentOperationObserver& aObserver );    

private: // Implementation
    CMRCmailRemoteAttachmentPlugin();
    void ConstructL();
    
private:
    /// Own: Pointer to CMAIL client object.
    CFSMailClient* iMailClient;
    /// Own: Attachment information fetcher
    CMRCmailRemoteAttachmentPluginAttinfoFetcher* iAttinfoFetcher;
    };

#endif // CMRCMAILREMOTEATTPLUGIN_H

// EOF

