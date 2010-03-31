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

#include "ccalremoteattachment.h"


// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CCalRemoteAttachment::CCalRemoteAttachment
// ---------------------------------------------------------------------------
//
CCalRemoteAttachment::CCalRemoteAttachment(
        TInt aAttachmentSizeInBytes,
        MCalRemoteAttachment::TCalRemoteAttachmentState aState ) :
    iSizeInBytes( aAttachmentSizeInBytes ),
    iState( aState )
    {
    
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::~CCalRemoteAttachment
// ---------------------------------------------------------------------------
//
EXPORT_C CCalRemoteAttachment::~CCalRemoteAttachment()
    {
    delete iLabel;
    delete iIdentifier;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CCalRemoteAttachment* CCalRemoteAttachment::NewL(
            const TDesC& aAttachmentIdentifier,
            const TDesC& aAttachmentLabel,
            TInt aAttachmentSizeInBytes,
            MCalRemoteAttachment::TCalRemoteAttachmentState aState )
    {
    CCalRemoteAttachment* self = 
            new (ELeave) CCalRemoteAttachment( 
                    aAttachmentSizeInBytes, aState );
    CleanupStack::PushL( self );
    self->ConstructL( aAttachmentIdentifier, aAttachmentLabel );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::ConstructL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachment::ConstructL(
        const TDesC& aAttachmentIdentifier,
        const TDesC& aAttachmentLabel )
    {    
    iIdentifier = aAttachmentIdentifier.AllocL();
    iLabel = aAttachmentLabel.AllocL();
    }
    
// ---------------------------------------------------------------------------
// CCalRemoteAttachment::SetAttachmentLabelL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachment::SetAttachmentLabelL(
        const TDesC& aAttachmentLabel )
    {
    HBufC* newLabel = aAttachmentLabel.AllocL();
    delete iLabel;
    iLabel = newLabel;  
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::AttachmentLabel
// ---------------------------------------------------------------------------
//
const TDesC& CCalRemoteAttachment::AttachmentLabel() const
    {
    return *iLabel;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::SetAttachmentIdentifierL
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachment::SetAttachmentIdentifierL(
        const TDesC& aAttachmentIdentifier )
    {
    HBufC* newLabel = aAttachmentIdentifier.AllocL();
    delete iIdentifier;
    iIdentifier = newLabel;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::AttachmentIdentifier
// ---------------------------------------------------------------------------
//
const TDesC& CCalRemoteAttachment::AttachmentIdentifier() const
    {
    return *iIdentifier;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::SetAttachmentSize
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachment::SetAttachmentSize(
        TInt aAttachmentSizeInBytes )
    {
    iSizeInBytes = aAttachmentSizeInBytes;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::AttachmentSizeInBytes
// ---------------------------------------------------------------------------
//
TInt CCalRemoteAttachment::AttachmentSizeInBytes() const
    {
    return iSizeInBytes;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::SetAttachmentState
// ---------------------------------------------------------------------------
//
void CCalRemoteAttachment::SetAttachmentState(
        MCalRemoteAttachment::TCalRemoteAttachmentState aAttachmentState )
    {
    iState = aAttachmentState;
    }

// ---------------------------------------------------------------------------
// CCalRemoteAttachment::AttachmenState
// ---------------------------------------------------------------------------
//
MCalRemoteAttachment::TCalRemoteAttachmentState CCalRemoteAttachment::AttachmenState() const
    {
    return iState;
    }

// EOF
