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
* Description:  MRUI attachmentinfo definition
*
*/

#include "emailtrace.h"
#include "cesmrattachmentinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::CESMRAttachmentInfo
// ---------------------------------------------------------------------------
//
inline CESMRAttachmentInfo::CESMRAttachmentInfo()
    {
    FUNC_LOG;
    }
// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::~CESMRAttachmentInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAttachmentInfo::~CESMRAttachmentInfo()
    {
    FUNC_LOG;
    iAttachments.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRAttachmentInfo* CESMRAttachmentInfo::NewL()
    {
    FUNC_LOG;
    CESMRAttachmentInfo* self = new (ELeave) CESMRAttachmentInfo;
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRAttachmentInfo::ConstructL()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::AddAttachmentInfoL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAttachmentInfo::AddAttachmentInfoL(
    const TDesC& aAttachmentName,
    TInt aAttachntSizeInBytes,
    CESMRAttachment::TESMRAttachmentState aState )
    {
    FUNC_LOG;
    CESMRAttachment* attachment =
            CESMRAttachment::NewL(
                    aAttachmentName,
                    aAttachntSizeInBytes,
                    aState );

    // Ownership of 'attachment' is transferred
    AddAttachmentInfoL( attachment );
    attachment = NULL;
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::AddAttachmentInfoL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAttachmentInfo::AddAttachmentInfoL(
     CESMRAttachment* aAttachment )
    {
    FUNC_LOG;
    // Ownership of 'aAttachment' is transferred to pointer array on the stack
    User::LeaveIfError( iAttachments.Append(aAttachment) );
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::AttachmentCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRAttachmentInfo::AttachmentCount() const
    {
    FUNC_LOG;
    return iAttachments.Count();
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::Attachment
// ---------------------------------------------------------------------------
//
EXPORT_C const CESMRAttachment& CESMRAttachmentInfo::AttachmentL(
     TInt aIndex ) const
    {
    FUNC_LOG;
    if ( aIndex < 0 || aIndex >= iAttachments.Count() )
        {
        User::Leave( KErrArgument );
        }

    return *(iAttachments[ aIndex ]);
    }

// ---------------------------------------------------------------------------
// CESMRAttachmentInfo::RemoveAttachmentAtL
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRAttachmentInfo::RemoveAttachmentAtL(
        TInt aIndex )
    {
    FUNC_LOG;
    CESMRAttachment* attachment = iAttachments[ aIndex ];
    iAttachments.Remove( aIndex );
    delete attachment;
    }

// EOF

