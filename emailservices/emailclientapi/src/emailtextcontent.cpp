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
* Description: This file implements class CEmailAttachment.
*
*/

#include "emailcontent.h"
#include "emailtextcontent.h"
#include "emailclientapi.hrh"

// CEmailTextContent

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CEmailTextContent* CEmailTextContent::NewL( 
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart,
        const TDataOwner aOwner ) 
    {
    CEmailTextContent* self = new ( ELeave ) CEmailTextContent( aOwner );
    CleanupStack::PushL( self );
    self->ConstructL( aPluginData, aMsgContentId, aPart );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::ConstructL(
        CPluginData& aPluginData,
        const TMessageContentId& aMsgContentId,
        CFSMailMessagePart* aPart )
    {
    iEmailMsgContent = CEmailMessageContent::NewL( aPluginData, aMsgContentId, aPart );

    if (!CheckParameterFormatL( aPart->GetContentType() ).Compare(KFSMailContentTypeTextHtml))
        {
        iTextType = EHtmlText;        
        }
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CEmailTextContent::~CEmailTextContent()
    {    
    delete iEmailMsgContent;    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CEmailTextContent::CEmailTextContent( TDataOwner aOwner ) : iTextType(EPlainText), iOwner( aOwner )        
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TEmailTypeId CEmailTextContent::InterfaceId() const
    {
    return KEmailIFUidTextContent;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::Release()
    {
    if ( iOwner == EClientOwns )
        {    
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
MEmailTextContent::TTextType CEmailTextContent::TextType() const
    {    
    return iTextType;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetTextL(
            const TTextType aPlainOrHtml,
            const TDesC& aText )
    {    
    iTextType = aPlainOrHtml;
    
    if( aPlainOrHtml == EPlainText )
        {
        SetContentType( KContentTypeTextPlain );
        }
    else if( aPlainOrHtml == EHtmlText )
        {
        SetContentType( KContentTypeTextHtml );
        }
    SetContentL( aText );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TMessageContentId CEmailTextContent::Id() const
    {
    return iEmailMsgContent->Id(); 
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentType() const
    {
    return iEmailMsgContent->ContentType();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentType( const TDesC& aContentType )
    {
    iEmailMsgContent->SetContentType( aContentType );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentId() const
    {
    return iEmailMsgContent->ContentId();    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentId( const TDesC& aContentId )
    {
    iEmailMsgContent->SetContentId(aContentId);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentDescription() const
    {
    return iEmailMsgContent->ContentDescription();    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentDescription( const TDesC& aContentDescription )
    {
    iEmailMsgContent->SetContentDescription(aContentDescription);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentDisposition() const
    {
    return iEmailMsgContent->ContentDisposition();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentDisposition( const TDesC& aContentDisposition )
    {
    iEmailMsgContent->SetContentDisposition(aContentDisposition);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentClass() const
    {
    return iEmailMsgContent->ContentClass();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentClass( const TDesC& aContentClass )
    {
    iEmailMsgContent->SetContentClass(aContentClass);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CEmailTextContent::AvailableSize() const
    {
    return iEmailMsgContent->AvailableSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CEmailTextContent::TotalSize() const
    {
    return iEmailMsgContent->TotalSize();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TPtrC CEmailTextContent::ContentL() const
    {
    return iEmailMsgContent->ContentL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetContentL( const TDesC& aContent )
    {
    iEmailMsgContent->SetContentL( aContent );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::FetchL( MEmailFetchObserver& aObserver )
    {
    iEmailMsgContent->FetchL(aObserver);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::CancelFetch()
    {
    iEmailMsgContent->CancelFetch();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SaveToFileL( const TDesC& aPath )
    {
    iEmailMsgContent->SaveToFileL(aPath);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
MEmailMultipart* CEmailTextContent::AsMultipartOrNull() const  
    {    
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
MEmailTextContent* CEmailTextContent::AsTextContentOrNull() const 
    {
    const MEmailTextContent* ptr = this;
    return const_cast<MEmailTextContent *>(ptr);
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
MEmailAttachment* CEmailTextContent::AsAttachmentOrNull() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TDesC& CEmailTextContent::CheckParameterFormatL( const TDesC& aParam )
    {    
    _LIT( KSeparator, ";" );
    _LIT( KSlash, "/" );
    // Check for a type separator in the string
    TInt endPos = aParam.Find( KSeparator );
    // Construct the compare string
    TPtrC extractString( aParam.Left( endPos ) );
    HBufC* compareBuf = extractString.AllocLC();
    TPtr compareString = compareBuf->Des();
    TInt slashPos = aParam.Find( KSlash );
    TInt stringLen = compareString.Length();
         
    const TDesC& leftPart = extractString.Left( slashPos );
    HBufC* leftPartBuf = leftPart.AllocLC();
    TPtr leftPartString = leftPartBuf->Des();
    leftPartString.Trim();
         
    const TDesC& rightPart = extractString.Right( stringLen - ( slashPos + 1 ) );
    HBufC* rightPartBuf = rightPart.AllocLC();
    TPtr rightPartString = rightPartBuf->Des();
    rightPartString.Trim();
         
    compareString.Delete( 0, endPos );
    compareString.Append( leftPartString );
    compareString.Append( KSlash );
    compareString.Append( rightPartString );
    compareString.LowerCase();   
    CleanupStack::PopAndDestroy( rightPartBuf );
    CleanupStack::PopAndDestroy( leftPartBuf );
    
    const TDesC& retParam = compareString;
    CleanupStack::PopAndDestroy( compareBuf );
    
    return retParam;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CEmailTextContent::SetOwner( const TDataOwner aOwner )
    {
    iOwner = aOwner;
    }
