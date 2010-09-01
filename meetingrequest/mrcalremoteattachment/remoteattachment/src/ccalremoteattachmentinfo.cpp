/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ccalremoteattachmentinfo.h"
#include "ccalremoteattachment.h"

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::CCalRemoteAttachmentInfo
// ---------------------------------------------------------------------------
//
CCalRemoteAttachmentInfo::CCalRemoteAttachmentInfo()
    {
    
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::~CCalRemoteAttachmentInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CCalRemoteAttachmentInfo::~CCalRemoteAttachmentInfo()
    {
    iAttachments.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCalRemoteAttachmentInfo* CCalRemoteAttachmentInfo::NewL()
    {
    CCalRemoteAttachmentInfo* self = new (ELeave) CCalRemoteAttachmentInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachmentInfo::ConstructL()
    {
    
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::AddAttachmentInfoL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachmentInfo::AddAttachmentInfoL(
        const TDesC& aAttachmentIdentifier,
        const TDesC& aAttachmentLabel,
        TInt aAttachmentSizeInBytes,
        MCalRemoteAttachment::TCalRemoteAttachmentState aState )
    {
    CCalRemoteAttachment* attachment = 
            CCalRemoteAttachment::NewL(
                aAttachmentIdentifier,
                aAttachmentLabel,
                aAttachmentSizeInBytes,
                aState );
    
    CleanupStack::PushL( attachment );
    
    // Ownership is transferred
    iAttachments.AppendL( attachment );
    
    CleanupStack::Pop( attachment );
    }
 
// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::AddAttachmentInfoL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachmentInfo::AddAttachmentInfoL(
         MCalRemoteAttachment* aAttachment )
    {
    // Ownership is transferred
    iAttachments.AppendL( aAttachment );    
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::AttachmentCount
// ---------------------------------------------------------------------------
//
TInt CCalRemoteAttachmentInfo::AttachmentCount() const
    {
    return iAttachments.Count();
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::AttachmentL
// ---------------------------------------------------------------------------
//
const MCalRemoteAttachment& CCalRemoteAttachmentInfo::AttachmentL(
         TInt aIndex ) const
     {
     return *iAttachments[ aIndex ];
     }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::RemoveAttachmentAtL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachmentInfo::RemoveAttachmentAtL(
         TInt aIndex )
    {
    if ( aIndex < iAttachments.Count() && aIndex >= 0 )
        {
        MCalRemoteAttachment* attachment = iAttachments[ aIndex ];
        iAttachments.Remove( aIndex );
        delete attachment;
        }
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::RemoveAttachmentL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachmentInfo::RemoveAttachmentL(
         MCalRemoteAttachment& aAttachment )
    {
    TInt pos( iAttachments.Find( &aAttachment ) );
    if ( KErrNotFound == pos )
        {
        User::Leave( KErrNotFound );
        }
    
    RemoveAttachmentAtL( pos );
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachmentInfo::Attachments
// ---------------------------------------------------------------------------
//
const RPointerArray<MCalRemoteAttachment>& 
        CCalRemoteAttachmentInfo::Attachments() const
    {
    return iAttachments;
    }

// EOF
