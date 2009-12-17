/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MRUI attachmen definition
*
*/


#include "emailtrace.h"
#include "cesmrattachment.h"

// unnamed namespace for local definitions
namespace {

// Unnamed attachment
_LIT( KESMRUnnamedAttachment, "" );

// Zero length attachment
const TInt KZeroLengthAttachment( 0 );

}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRAttachment::CESMRAttachment
// ---------------------------------------------------------------------------
// 
CESMRAttachment::CESMRAttachment(             
        TInt aSizeInBytes,
        TESMRAttachmentState aAttachmentState )
:   iSizeInBytes( aSizeInBytes ),
    iState( aAttachmentState ) 
     {
    FUNC_LOG;
     // No implementation
     }

// ---------------------------------------------------------------------------
// CESMRAttachment::~CESMRAttachment
// ---------------------------------------------------------------------------
// 
EXPORT_C CESMRAttachment::~CESMRAttachment()
     {
    FUNC_LOG;
     delete iAttachmentName;
     }

// ---------------------------------------------------------------------------
// CESMRAttachment::NewL
// ---------------------------------------------------------------------------
// 
EXPORT_C CESMRAttachment* CESMRAttachment::NewL()
    {
    FUNC_LOG;
    CESMRAttachment* self =  NewL( 
				 KESMRUnnamedAttachment, 
				 KZeroLengthAttachment, 
				 EAttachmentStateUnknown );
    return self;    
    }

// ---------------------------------------------------------------------------
// CESMRAttachment::NewL
// ---------------------------------------------------------------------------
// 
EXPORT_C CESMRAttachment* CESMRAttachment::NewL(
             const TDesC& aAttachmentName,
             TInt aSizeInBytes,
             TESMRAttachmentState aAttachmentState )
    {
    FUNC_LOG;
    CESMRAttachment* self = 
            new (ELeave) CESMRAttachment( aSizeInBytes, aAttachmentState );
    CleanupStack::PushL( self );
    self->ConstructL( aAttachmentName );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRAttachment::ConstructL
// ---------------------------------------------------------------------------
// 
void CESMRAttachment::ConstructL(
    const TDesC& aAttachmentName )
     {
    FUNC_LOG;
     iAttachmentName = aAttachmentName.AllocL();
     }

// ---------------------------------------------------------------------------
// CESMRAttachment::SetAttachmentNameL
// ---------------------------------------------------------------------------
// 
EXPORT_C void CESMRAttachment::SetAttachmentNameL(
    const TDes& aAttachmentName )
     {
    FUNC_LOG;
     delete iAttachmentName; 
     iAttachmentName = NULL;     
     iAttachmentName = aAttachmentName.AllocL();     
     }

// ---------------------------------------------------------------------------
// CESMRAttachment::AttachmentName
// ---------------------------------------------------------------------------
// 
EXPORT_C const TDesC& CESMRAttachment::AttachmentName() const
    {
    FUNC_LOG;
    return *iAttachmentName;
    }

// ---------------------------------------------------------------------------
// CESMRAttachment::SetAttachmentSize
// ---------------------------------------------------------------------------
// 
EXPORT_C void CESMRAttachment::SetAttachmentSize(
    TInt aAttachmentSizeInBytes )
    {
    FUNC_LOG;
    iSizeInBytes = aAttachmentSizeInBytes;
    }
    
// ---------------------------------------------------------------------------
// CESMRAttachment::AttachmentSizeInBytes
// ---------------------------------------------------------------------------
// 
EXPORT_C TInt CESMRAttachment::AttachmentSizeInBytes() const
     {
    FUNC_LOG;
     return iSizeInBytes;
     }
    
// ---------------------------------------------------------------------------
// CESMRAttachment::SetAttachmentState
// ---------------------------------------------------------------------------
// 
EXPORT_C void CESMRAttachment::SetAttachmentState(
    TESMRAttachmentState aAttachmentState )
     {
    FUNC_LOG;
     iState = aAttachmentState;
     }

// ---------------------------------------------------------------------------
// CESMRAttachment::AttachmenState
// ---------------------------------------------------------------------------
// 
EXPORT_C CESMRAttachment::TESMRAttachmentState 
		CESMRAttachment::AttachmenState() const
     {
    FUNC_LOG;
     return iState;
     }

// EOF

