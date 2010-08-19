/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email application internal Central Repository keys
*
*/

#ifndef FSEMAILINTERNALCRKEYS_H
#define FSEMAILINTERNALCRKEYS_H

#include <e32std.h>

//
// Fs Email repository in the central repository
//

// <cmail> S60 UID update
const TUid KFreestyleEmailCenRep                    = {0x2001E277};
// </cmail> S60 UID update

// Global settings values

const TUint32 KFreestyleEmailMessageListLayout      = {0x00000001};
const TInt KFreestyleEmailMessageListLayoutDefault  = 2;
    
const TUint32 KFreestyleEmailBodyPreview            = {0x00000002};
const TInt KFreestyleEmailBodyPreviewDefault = 0;
    
const TUint32 KFreestyleEmailTitleDividers          = {0x00000003};
const TInt KFreestyleEmailTitleDividersDefault = 1;
    
const TUint32 KFreestyleEmailDownloadNotification   = {0x00000004};
const TInt KFreestyleEmailDownloadNotificationDefault = 1;
    
const TUint32 KFreestyleEmailActiveIdle             = {0x00000005};
const TInt KFreestyleEmailActiveIdleDefault = 1;

const TUint32 KFreestyleEmailWarnBeforeDelete       = {0x00000006};
const TInt KFreestyleEmailWarnBeforeDeleteDefault = 1;

const TUint32 KFreestyleEmailDownloadHTMLImages     = {0x00000007};
const TInt KFreestyleEmailDownloadHTMLImagesDefault = 1;

// Other CR values for FS Email application internal use

const TUint32 KFreestyleEmailEditorCCVisible        = {0x00000080};
const TInt KFreestyleEmailEditorCCVisibleDefault = 1;

const TUint32 KFreestyleEmailEditorBCVisible        = {0x00000081};
const TInt KFreestyleEmailEditorBCVisibleDefault = 0;

// Used by Integration MTM
const TUint32 KFreestyleMtmMessageId                = {0x00000090};

// Used by White Label Branding
const TUint32 KFreestyleWLBBrandIdMatchString            = {0x00000120};
const TUint32 KFreestyleWLBMailboxName                   = {0x00000121};
const TUint32 KFreestyleWLBMIFFilePathWithTargetFilename = {0x00000122};
const TUint32 KFreestyleWLBMailboxIconID                 = {0x00000123};

// used for email indicator popup-note handling
// First key tells how many mailboxes have new emails
const TUint32 KNumberOfMailboxesWithNewEmails                = {0x00001000};


// These keys define limits on how much data is allowed to load by plugins 
/* The setting allows to limit max body size, KB */
const TUint32 KFreestyleMaxBodySize                      = 0x00002000;
/* The setting allows to limit max body size in reply/forward, KB. */
const TUint32 KFreestyleReplyMaxBodySize                 = 0x00002001;
/* The setting allows to limit max total size of mail attachments shown in mail viewer, KB */
const TUint32 KFreestyleViewerMaxAttachmentsSize         = 0x00002002;


//
// Email local variation flags. Flags are defined in emailvariant.hrh
const TUint32 KEmailLocalVariationFlags         = {0x0000FFFE};

// FS 2.05 variation.
// Values: 1 = hide, 0 = show.
const TUint32 KEmailHideFS205UIFeature          = {0x0000FFFF};
const TUint32 KEmailHideFSHiLightUIFeature          = {0x0000FFFF};
// PS2 variation
// one centrep key for every feature, 0 means disabled
const TUint32 KEmailFeatureSplitScreen                  = {0x00001001};

//
// TP Wizard repository in the central repository
// - used for parameter passing (for creating a new mailbox)
//
const TUid KSetupWizardCenRep                       = {0x2000B112};

// New mailbox parameter values

const TUint32 KSetupWizardAccountType               = {0x00000001};
    
const TUint32 KSetupWizardCentrepStartKey           = {0x00000002};
                                                                   
#endif
