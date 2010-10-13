/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: utility class definition
*
*/



// INCLUDE FILES
#include "emailtrace.h"
#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknsSkinInstance.h>
#include <aknbutton.h>
#include <eikedwin.h>
#include <FreestyleEmailUi.rsg>
#include <RPbkViewResourceFile.h>
#include <CPbkContactEngine.h>
#include <CPbkMultipleEntryFetchDlg.h>
#include <CPbkContactItem.h>
#include <AknIconUtils.h>
#include <eikimage.h>
#include <fbs.h>
#include <flogger.h>
#include <aknenv.h> // CAknEnv

#include "cfsmailbox.h"
#include "cfsmailclient.h"
#include "cfsmailaddress.h"

#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <AknLayoutFont.h>

#include "ncsutility.h"
#include "ncsemailaddressobject.h"
#include "ncsconstants.h"
#include "FSDelayedLoader.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLayoutData.h"

// CONSTANTS
const TInt KFSUtilityMinColorValue = 0;
const TInt KFSUtilityMaxColorValue = 255;
const TInt KFSUtilityThresholdColorValue = 200;
const TInt KFSUtilityThresholdColorMiddleValue = 127;

// ========================= MEMBER FUNCTIONS ==================================


// -----------------------------------------------------------------------------
// NcsUtility::CompleteRelativePathL()
// -----------------------------------------------------------------------------
//
void NcsUtility::CompleteRelativePathL( 
        const TDesC& aRelativePath, TPtr& aAbsolutePath )
    {
    FUNC_LOG;

    // + 2 is for drive letter and ":"
    if( aAbsolutePath.MaxLength() < aRelativePath.Length() + 2 )
        {
        User::Leave( KErrArgument );
        }

    TFileName path;
    path.Copy( aRelativePath );
    TInt err = CompleteWithAppPath( path );
    User::LeaveIfError( err );

    aAbsolutePath.Copy( path );

    }

// -----------------------------------------------------------------------------
// NcsUtility::GenerateFromLineToMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::GenerateFromLineToMessageBodyL( 
        const RPointerArray<CNcsEmailAddressObject>& aFromArray )
    {
    FUNC_LOG;

    HBufC* buf = DoGenerateAddressLineToMessageBodyL( 
            R_NCS_ENGINE_EMAIL_FROM, aFromArray );

    return buf;
    }

// -----------------------------------------------------------------------------
// NcsUtility::GenerateSentLineToMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::GenerateSentLineToMessageBodyL( CFSMailMessage& aMessage )
    {
    FUNC_LOG;

    HBufC* dateText = TFsEmailUiUtility::DateTextFromMsgLC( &aMessage );
    HBufC* timeText = TFsEmailUiUtility::TimeTextFromMsgLC( &aMessage );
    HBufC* sent = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_SENT );

    HBufC* buf = HBufC::NewL( sent->Length() +
                              KSpace().Length() +
                              dateText->Length() +
                              KSentLineDateAndTimeSeparatorText().Length() +
                              timeText->Length() );
    TPtr ptr = buf->Des();
    ptr.Append( *sent );
    ptr.Append( KSpace );
    ptr.Append( *dateText );
    ptr.Append( KSentLineDateAndTimeSeparatorText );
    ptr.Append( *timeText );

    CleanupStack::PopAndDestroy( sent );
    CleanupStack::PopAndDestroy( timeText );
    CleanupStack::PopAndDestroy( dateText );

    return buf;
    }

// -----------------------------------------------------------------------------
// NcsUtility::GenerateAddressLineToMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::GenerateAddressLineToMessageBodyL(
    RPointerArray<CNcsEmailAddressObject>& aToArray, TRecipientType aType )
    {
    FUNC_LOG;

    // figure out the recipient type dependent text
    TInt labelResource = R_NCS_ENGINE_EMAIL_TO;
    if ( aType == ERecipientTypeCc )
        {
        labelResource = R_NCS_ENGINE_EMAIL_CC;
        }

    HBufC* buf = DoGenerateAddressLineToMessageBodyL( labelResource, aToArray );

    return buf;
    }

// -----------------------------------------------------------------------------
// NcsUtility::DoGenerateAddressLineToMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::DoGenerateAddressLineToMessageBodyL( TInt aLabelResourceId,
        const RPointerArray<CNcsEmailAddressObject>& aAddressArray )
    {
    FUNC_LOG;

    // load label string (e.g. "From:", "To:", "Cc:")
    HBufC* label = StringLoader::LoadLC( aLabelResourceId );

    // calculate how large buffer do we need for the whole line
    TInt bufLength = label->Length();
    TInt count = aAddressArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CNcsEmailAddressObject* obj = aAddressArray[i];
        const TDesC& dispName = obj->DisplayName();
        // use display name if it is available, otherwise email address
        if ( dispName.Length() > 0 )
            {
            bufLength += dispName.Length();
            }
        else
            {
            bufLength += obj->EmailAddress().Length();
            }
        if ( i < ( count - 1 ) )
            {
            bufLength += KEmailAddressSeparator().Length();
            bufLength += KSpace().Length();
            }
        }

    // allocate buffer to contain the whole text
    HBufC* buf = HBufC::NewL( bufLength );
    TPtr ptr = buf->Des();

    // add label
    ptr.Append( *label );
    CleanupStack::PopAndDestroy( label );

    // append each address into our text buffer
    for ( TInt i = 0; i < count; i++ )
        {
        CNcsEmailAddressObject* obj = aAddressArray[i];
        const TDesC& dispName = obj->DisplayName();
        // use display name if it is available, otherwise email address
        if ( dispName.Length() > 0 )
            {
            ptr.Append( dispName );
            }
        else
            {
            ptr.Append( obj->EmailAddress() );
            }
        if ( i < ( count - 1 ) )
            {
            ptr.Append( KEmailAddressSeparator );
            ptr.Append( KSpace );
            }
        }

    return buf;
    }

// -----------------------------------------------------------------------------
// NcsUtility::GenerateSubjectLineToMessageBodyL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::GenerateSubjectLineToMessageBodyL( const TDesC& aSubject )
    {
    FUNC_LOG;

    // load "Subject:" string
    HBufC* subjectLabel = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_SUBJECT );

    // allocate buffer for the text
    HBufC* buf = HBufC::NewL( subjectLabel->Length() + aSubject.Length() );
    TPtr ptr = buf->Des();
    ptr.Append( *subjectLabel );
    ptr.Append( aSubject );

    CleanupStack::PopAndDestroy( subjectLabel );

    return buf;
    }

// -----------------------------------------------------------------------------
// NcsUtility::CreateNcsAddressL()
//
// -----------------------------------------------------------------------------
//
CNcsEmailAddressObject* NcsUtility::CreateNcsAddressL( 
        const CFSMailAddress& aFsAddress )
    {
    FUNC_LOG;

    CNcsEmailAddressObject* address = CNcsEmailAddressObject::NewL( ETrue );
    CleanupStack::PushL( address );
    // temporary ugly code because email API can return references to null
    TDesC& a = aFsAddress.GetEmailAddress();
    if ( &a )
        {
        address->SetEmailAddressL( a );
        }
    TDesC& name = aFsAddress.GetDisplayName();
    if ( &name )
        {
        address->SetDisplayNameL( name );
        }

    CleanupStack::Pop( address );

    return address;
    }

// -----------------------------------------------------------------------------
// NcsUtility::CreateFsAddressL()
// Caller takes the ownership of the returned address object
// -----------------------------------------------------------------------------
//
CFSMailAddress* NcsUtility::CreateFsAddressL( 
        const CNcsEmailAddressObject& aNcsAddress )
    {
    FUNC_LOG;

    CFSMailAddress* address = CFSMailAddress::NewL();
    address->SetEmailAddress( aNcsAddress.EmailAddress() );
    address->SetDisplayName( aNcsAddress.DisplayName() );

    return address;
    }


// -----------------------------------------------------------------------------
// NcsUtility::FormatSubjectLineL()
//
// -----------------------------------------------------------------------------
//
HBufC* NcsUtility::FormatSubjectLineL( 
        const TDesC& aNewSubjectLine, const TDesC& aPrefix )
    {
    FUNC_LOG;
       
        HBufC* formattedSubjectLine = aNewSubjectLine.AllocLC();
        HBufC* prefix = aPrefix.AllocLC();
        TPtr formattedSubjectLinePtr = formattedSubjectLine->Des();
        TPtr prefixPtr = prefix->Des();
        prefixPtr.Trim();
        
        HBufC* fwdPrefix = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_FORWARD_PREFIX );
        HBufC* rePrefix = StringLoader::LoadLC( R_NCS_ENGINE_EMAIL_REPLY_PREFIX );
        TPtr fwdPrt = fwdPrefix->Des();
        TPtr rePtr = rePrefix->Des();
        fwdPrt.Trim();
        rePtr.Trim();
        
        TInt indexRe;
        TInt indexFw;
        
        
        // Start to analyse the original subject string and remove
        // all prefixes at the beggining. When no prefix found as
        // first word, than loop ends
        do
            {
            indexRe = formattedSubjectLinePtr.FindC( rePtr );
			      if( indexRe == 0 )
				        {
                formattedSubjectLinePtr.Delete( indexRe, rePtr.Length() );
                formattedSubjectLinePtr.Trim();
                }

            indexFw = formattedSubjectLinePtr.FindC( fwdPrt );
            if( indexFw == 0 )
                {
                formattedSubjectLinePtr.Delete( indexFw, rePtr.Length() );
                formattedSubjectLinePtr.Trim();
                }
            }
        while( !( indexRe != 0 && indexFw != 0)  );
        
        HBufC* finalSubject = HBufC::NewL( formattedSubjectLinePtr.Length() + prefixPtr.Length() + KSpace().Length()  );
        TPtr ptr = finalSubject->Des();
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            ptr.Append( formattedSubjectLinePtr );
            ptr.Append( KSpace );
            ptr.Append( prefixPtr );        
            
            }
        else
            {
            ptr.Append( prefixPtr );
            ptr.Append( KSpace );
            ptr.Append( formattedSubjectLinePtr );
        
            }
        CleanupStack::PopAndDestroy( rePrefix );
        CleanupStack::PopAndDestroy( fwdPrefix );
        CleanupStack::PopAndDestroy( prefix );
        CleanupStack::PopAndDestroy( formattedSubjectLine );

        return finalSubject;
    }

// -----------------------------------------------------------------------------
// NcsUtility::ConvertAddressArrayL()
//
// -----------------------------------------------------------------------------
//
void NcsUtility::ConvertAddressArrayL( 
        const RPointerArray<CFSMailAddress>& aSrc, 
        RPointerArray<CNcsEmailAddressObject>& aDst )
    {
    FUNC_LOG;

    for ( TInt i=0; i<aSrc.Count(); i++ )
        {
        CNcsEmailAddressObject* ncsRecipient = 
            NcsUtility::CreateNcsAddressL( *aSrc[i] );
        CleanupStack::PushL( ncsRecipient );
        aDst.AppendL( ncsRecipient );
        CleanupStack::Pop( ncsRecipient );
        }

    }

// -----------------------------------------------------------------------------
// NcsUtility::ConvertAddressArrayL()
// Caller takes the ownership of address objects pointoed in the array aDst.
// -----------------------------------------------------------------------------
//
void NcsUtility::ConvertAddressArrayL( 
        const RPointerArray<CNcsEmailAddressObject>& aSrc, 
        RPointerArray<CFSMailAddress>& aDst )
    {
    FUNC_LOG;

    for ( TInt i=0; i<aSrc.Count(); i++ )
        {
        CFSMailAddress* fsRecipient = 
            NcsUtility::CreateFsAddressL( *aSrc[i] );
        CleanupStack::PushL( fsRecipient );
        aDst.AppendL( fsRecipient );
        CleanupStack::Pop( fsRecipient );
        }

    }



// -----------------------------------------------------------------------------
// NcsUtility::LoadScalableIconL()
//
// -----------------------------------------------------------------------------
//
void NcsUtility::LoadScalableIconL( 
        const TDesC& aMifRelativeFileName, TInt aImgId, 
        TInt aMaskId, CEikImage*& aIcon, 
        CFbsBitmap*& aImage, CFbsBitmap*& aMask )
    {
    FUNC_LOG;

    HBufC* buf = HBufC::NewLC( KMaxFileName );
    TPtr ptr = buf->Des();
    NcsUtility::CompleteRelativePathL( aMifRelativeFileName, ptr );

    AknIconUtils::CreateIconL( aImage,
                               aMask,
                               ptr,
                               aImgId,
                               aMaskId );

    TSize size;
    AknIconUtils::GetContentDimensions( aImage, size );
    AknIconUtils::SetSize( aImage, size );

    aIcon = new ( ELeave ) CEikImage;
    aIcon->SetNewBitmaps( aImage, aMask );
    aIcon->SetPictureOwnedExternally( ETrue );

    CleanupStack::PopAndDestroy( buf );

    }


// -----------------------------------------------------------------------------
// NcsUtility::DebugLog
// -----------------------------------------------------------------------------
//
void NcsUtility::DebugLog( TRefByValue<const TDesC> aFmt, ... )
    {
    FUNC_LOG;
    _LIT( KDebugDir, "FreestyleEmailUi" );
    _LIT( KDebugLog, "log.txt" );

    VA_LIST args;
    VA_START( args, aFmt );
    RFileLogger::WriteFormat( KDebugDir,
                              KDebugLog,
                              EFileLoggingModeAppend,
                              aFmt,
                              args );
    VA_END( args );
    }

// -----------------------------------------------------------------------------
// NcsUtility::IsEqualAddressArray
// -----------------------------------------------------------------------------
//
TBool NcsUtility::IsEqualAddressArray( 
        const RPointerArray<CFSMailAddress>& aAddresses1,
        const RPointerArray<CFSMailAddress>& aAddresses2 )
    {
    FUNC_LOG;

    if ( aAddresses1.Count() != aAddresses2.Count() )
        {
        return EFalse;
        }
    for ( TInt i=0; i<aAddresses1.Count(); i++ )
        {
        if ( aAddresses1[i]->GetEmailAddress().Compare( 
                aAddresses2[i]->GetEmailAddress() ) != 0 )
            {
            return EFalse;
            }
        if ( aAddresses1[i]->GetDisplayName().Compare( 
                aAddresses2[i]->GetDisplayName() ) != 0 )
            {
            return EFalse;
            }
        }

    return ETrue;
    }


// -----------------------------------------------------------------------------
// NcsUtility::Measures
// -----------------------------------------------------------------------------
//
TNcsMeasures NcsUtility::Measures()
    {
    FUNC_LOG;

    TNcsMeasures ms;

    TAknUiZoom zoomLevel = EAknUiZoomNormal;
    CAknEnv::Static()->GetCurrentGlobalUiZoom( zoomLevel );
    switch ( zoomLevel )
        {
        case EAknUiZoomNormal: case EAknUiZoomAutomatic:
            {
            ms.iAifHeight = KAifHeight;
            ms.iAifLabelMarginVertical = KAifLabelMarginVertical;
            ms.iAifEditorMarginVertical = KAifEditorMarginVertical;
            ms.iAifEditorHeight = KAifEditorHeight;
            ms.iSubjectExtraHeightBottom = KSubjectExtraHeightBottom;
            ms.iAttachmentExtraHeightBottom = KAttachmentExtraHeightBottom;
            ms.iIconMarginPriorityVertical = KIconMarginPriorityVertical;
            ms.iIconMarginFollowUpVertical = KIconMarginFollowUpVertical;
            ms.iIconMarginAttachmentVertical = KIconMarginAttachmentVertical;
            ms.iLabelFontHeightPx = KLabelFontHeightPx;
            ms.iEditorFontHeightPx = KEditorFontHeightPx;
            }
            break;
        case EAknUiZoomSmall: case EAknUiZoomVerySmall:
            {
            ms.iAifHeight = KAifHeightSmall;
            ms.iAifLabelMarginVertical = KAifLabelMarginVerticalSmall;
            ms.iAifEditorMarginVertical = KAifEditorMarginVerticalSmall;
            ms.iAifEditorHeight = KAifEditorHeightSmall;
            ms.iSubjectExtraHeightBottom = KSubjectExtraHeightBottomSmall;
            ms.iAttachmentExtraHeightBottom = 
                KAttachmentExtraHeightBottomSmall;
            ms.iIconMarginPriorityVertical = KIconMarginPriorityVerticalSmall;
            ms.iIconMarginFollowUpVertical = KIconMarginFollowUpVerticalSmall;
            ms.iIconMarginAttachmentVertical = 
                KIconMarginAttachmentVerticalSmall;
            ms.iLabelFontHeightPx = KLabelFontHeightPxSmall;
            ms.iEditorFontHeightPx = KEditorFontHeightPxSmall;
            }
            break;
        case EAknUiZoomLarge: case EAknUiZoomVeryLarge:
            {
            ms.iAifHeight = KAifHeightLarge;
            ms.iAifLabelMarginVertical = KAifLabelMarginVerticalLarge;
            ms.iAifEditorMarginVertical = KAifEditorMarginVerticalLarge;
            ms.iAifEditorHeight = KAifEditorHeightLarge;
            ms.iSubjectExtraHeightBottom = KSubjectExtraHeightBottomLarge;
            ms.iAttachmentExtraHeightBottom = 
                KAttachmentExtraHeightBottomLarge;
            ms.iIconMarginPriorityVertical = KIconMarginPriorityVerticalLarge;
            ms.iIconMarginFollowUpVertical = KIconMarginFollowUpVerticalLarge;
            ms.iIconMarginAttachmentVertical = 
                KIconMarginAttachmentVerticalLarge;
            ms.iLabelFontHeightPx = KLabelFontHeightPxLarge;
            ms.iEditorFontHeightPx = KEditorFontHeightPxLarge;
            }
            break;
        }

    return ms;
    }

// -----------------------------------------------------------------------------
// NcsUtility::GetNearestFontL
// -----------------------------------------------------------------------------
//
CFont* NcsUtility::GetNearestFontL( TInt aLogicalFontId, TInt aHeightPixels )
    {
    FUNC_LOG;

    // Base the font on logical font passed to us
    const CFont* logicalFont = AknLayoutUtils::FontFromId( aLogicalFontId );
    // Note: This font is owned by the application's system font array (where
    // it is likely already to have been created) and does not need to be
    // released. It can just go out of scope.

    // Extract font information
    TFontSpec fontSpec = logicalFont->FontSpecInTwips();

    // Desired height, weight, and posture
    CWsScreenDevice& screenDev = *( CEikonEnv::Static()->ScreenDevice() );
    fontSpec.iHeight = screenDev.VerticalPixelsToTwips( aHeightPixels );
    fontSpec.iFontStyle.SetStrokeWeight( EStrokeWeightNormal );
    fontSpec.iFontStyle.SetPosture( EPostureUpright );

    // Obtain new font
    CFont* font;

    TInt err = screenDev.GetNearestFontInTwips( font, fontSpec );
    User::LeaveIfError( err );

    return font;
    // Font is now usable. Must be released before application
    // exit using CWsScreenDevice::ReleaseFont()
    }

// <cmail> Platform layout change
// -----------------------------------------------------------------------------
// NcsUtility::GetLayoutFont
// -----------------------------------------------------------------------------
//
const CFont* NcsUtility::GetLayoutFont(
        const TRect& aParent, TNcsLayoutFont aUsage )
    {
    FUNC_LOG;
    TAknLayoutText layoutText;
    switch (aUsage)
        {
        case ENcsHeaderCaptionFont:
            {
            layoutText.LayoutText(aParent, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1());
            break;
            }
        case ENcsHeaderDetailFont:
            {
            layoutText.LayoutText(aParent, AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1(0));
            break;
            }
        case ENcsBodytextFont:
            {
            layoutText.LayoutText(aParent, AknLayoutScalable_Apps::list_cmail_body_pane_t1());
            break;
            }
        }
    return layoutText.Font();
    }
// </cmail>

// -----------------------------------------------------------------------------
// NcsUtility::IsEditReplyForwardQuoteSupported
// -----------------------------------------------------------------------------
//
TBool NcsUtility::IsReplyForwardQuoteReadOnly( const CFSMailBox& aMailBox )
    {
    FUNC_LOG;
    TBool r = aMailBox.HasCapability( EFSMBoxCapaReadOnlySmartQuote );
    return r;
    }

// -----------------------------------------------------------------------------
// NcsUtility::FindFirstPrintableCharIndex
// -----------------------------------------------------------------------------
//
TInt NcsUtility::FindFirstPrintableCharIndex( const TDesC& aText )
    {
    FUNC_LOG;
    for ( TInt i=0; i<aText.Length(); i++ )
        {
        TChar c = aText[i];
        if ( c.IsGraph() )
            {
            return i;
            }
        else
            {
            continue;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// NcsUtility::ConvertCharacters
// -----------------------------------------------------------------------------
//
void NcsUtility::ConvertCharacters( TDes& aBuffer, TUint16 aFrom, TUint16 aTo )
    {
    FUNC_LOG;
    for ( TInt i=0; i<aBuffer.Length(); i++ )
        {
        if ( aBuffer[i] == aFrom )
            {
            aBuffer[i] = aTo;
            }
        }
    }

// -----------------------------------------------------------------------------
// NcsUtility::ConvertLineEndingsL
// -----------------------------------------------------------------------------
//
void NcsUtility::ConvertLineEndingsL( HBufC*& aBuffer )
    {
    FUNC_LOG;
    // find line ending positions
    RArray<TInt> positions;
    CleanupClosePushL( positions );
    for ( TInt i=0; i<aBuffer->Length(); i++ )
        {
        if ( (*aBuffer)[i] == (TInt32)CEditableText::EParagraphDelimiter )
            {
            positions.Append( i );
            }
        }

    // case when there are no line endings can be early exited
    if ( positions.Count() <= 0 )
        {
        CleanupStack::PopAndDestroy( &positions );
        return;
        }

    // we need to do the conversion
    // initialize
    const TChar KCR = 0x0D;
    const TChar KLF = 0x0A;
    TInt startPos = 0;
    TInt length = 0;

    // create destination buffer
    HBufC* newBuffer = HBufC::NewL( aBuffer->Length() + positions.Count() * 2 );
    TPtr dstPtr = newBuffer->Des();

    // do the conversion
    for ( TInt i=0; i<positions.Count(); i++ )
        {
        // copy text, no line ending
        length = positions[i] - startPos;
        TPtrC16 srcPtr = aBuffer->Mid( startPos, length );
        dstPtr.Append( srcPtr );
        startPos = positions[i] + 1;
        // append line ending
        dstPtr.Append( KCR );
        dstPtr.Append( KLF );
        }
    // handle the text piece after last line ending if there is such
    const TInt lastPos = positions[ positions.Count() - 1 ];
    const TInt high = aBuffer->Length() - 1;
    if ( lastPos < high )
        {
        const TInt start = lastPos + 1;
        TPtrC16 srcPtr = aBuffer->Mid( start, aBuffer->Length() - start );
        dstPtr.Append( srcPtr );
        }
    // delete old buffer and give pointer to new one
    delete aBuffer;
    aBuffer = newBuffer;
    CleanupStack::PopAndDestroy( &positions );
    }

 // -----------------------------------------------------------------------------
// NcsUtility::DeleteMessage
// -----------------------------------------------------------------------------
//
TInt NcsUtility::DeleteMessage( CFSMailClient& aMailClient,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aFolderId,
        const TFSMailMsgId& aMsgId )
    {
    FUNC_LOG;
    RArray<TFSMailMsgId> msgIds;
    TInt error = msgIds.Append( aMsgId );
    if ( error == KErrNone )
        {
        TRAP( error, aMailClient.DeleteMessagesByUidL( aMailBoxId, aFolderId,
            msgIds ) );
        }
    msgIds.Close();
    return error;
    }

// -----------------------------------------------------------------------------
// NcsUtility::HeaderCaptionPaneRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::HeaderCaptionPaneRect( const TRect& aParent )
    {
    FUNC_LOG;
    TAknLayoutRect headerCaptionPaneRect;
    headerCaptionPaneRect.LayoutRect( aParent, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    return headerCaptionPaneRect.Rect();
    }

// -----------------------------------------------------------------------------
// NcsUtility::HeaderDetailPaneRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::HeaderDetailPaneRect( 
        const TRect& aParent, const TInt aRows, TBool aSameRow )
    {
    FUNC_LOG;
    const TInt variety( aRows == 1 ? 0 : 1 );
    TAknLayoutRect headerDetailPaneRect;
    headerDetailPaneRect.LayoutRect( aParent, AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( variety ) );
    TRect result( headerDetailPaneRect.Rect() );
    if (!aSameRow)
        {
        result.Move( 0, HeaderCaptionPaneRect( aParent ).Height() );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// NcsUtility::HeaderDetailIconRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::HeaderDetailIconRect( 
        const TRect& aParent, const TInt aRows, const THeaderDetailIcon aIcon )
    {
    FUNC_LOG;
    const TInt variety( aRows == 1 ? 0 : 1 );
    TAknLayoutRect headerIconRect;
    headerIconRect.LayoutRect( aParent,
            aIcon == EIconFront ?
                AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g1( variety ) :
                AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_g2( variety ) );
    // Offset detail rect by header caption pane height
    TRect result( headerIconRect.Rect() );
    //result.Move( 0, HeaderCaptionPaneRect( aParent ).Height() );
    return result;
    }

// -----------------------------------------------------------------------------
// NcsUtility::ListscrollPaneRect
// -----------------------------------------------------------------------------
TRect NcsUtility::ListscrollPaneRect()
    {
    FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    TAknLayoutRect listRect;
    listRect.LayoutRect( mainPaneRect, AknLayoutScalable_Apps::main_sp_fs_listscroll_pane_te() );
    return listRect.Rect();
    }

// -----------------------------------------------------------------------------
// NcsUtility::ListCmailPaneRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::ListCmailPaneRect( const TRect& aParent )
    {
    FUNC_LOG;
    TAknLayoutRect listCmailPaneRect;
    listCmailPaneRect.LayoutRect( aParent, AknLayoutScalable_Apps::list_cmail_pane() );
    return listCmailPaneRect.Rect();
    }

// -----------------------------------------------------------------------------
// NcsUtility::ListCmailScrollbarRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::ListCmailScrollbarRect( const TRect& aParent )
    {
    FUNC_LOG;
    TAknLayoutRect listCmailPaneRect;
    listCmailPaneRect.LayoutRect( aParent, AknLayoutScalable_Apps::sp_fs_scroll_pane_cp03() );
    return listCmailPaneRect.Rect();
    }

// -----------------------------------------------------------------------------
// NcsUtility::SeparatorSizeInThisResolution
//
// Returns size of empty space between header and message body
// -----------------------------------------------------------------------------
//
TSize NcsUtility::SeparatorSizeInThisResolution()
    {
    FUNC_LOG;

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TAknLayoutRect listRect;
    listRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::list_cmail_pane());

    TAknLayoutRect bodyRect;
    bodyRect.LayoutRect(listRect.Rect(), AknLayoutScalable_Apps::list_cmail_body_pane());

    TRect rect( bodyRect.Rect() );
    rect.Shrink( 0, bodyRect.Rect().Height() / 4 );
    return rect.Size();
    }



// -----------------------------------------------------------------------------
// NcsUtility::SeparatorLineColor
// -----------------------------------------------------------------------------
//
TRgb NcsUtility::SeparatorLineColor()
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb rgb;
    if (AknsUtils::GetCachedColor( skin, rgb,
            KAknsIIDQsnLineColors, EAknsCIQsnLineColorsCG6) != KErrNone )
        {
        rgb = KRgbBlack;
        }

    return rgb;
    }

// -----------------------------------------------------------------------------
// NcsUtility::ComposerLeftMarginInPixels
//
// Left margin value for additional graphics in composer view
// -----------------------------------------------------------------------------
//
TInt NcsUtility::ComposerLeftMarginInPixels( const TRect& aParent )
    {
    FUNC_LOG;
    const TRect cmailPaneRect( ListCmailPaneRect( aParent ) );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iTl.iX - captionTextRect.TextRect().iTl.iX );
    }

// -----------------------------------------------------------------------------
// NcsUtility::ComposerRightMarginInPixels
//
// Right margin value for additional graphics in composer view
// -----------------------------------------------------------------------------
//
TInt NcsUtility::ComposerRightMarginInPixels( const TRect& aParent )
    {
    FUNC_LOG;
    const TRect cmailPaneRect( ListCmailPaneRect( aParent ) );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( cmailPaneRect, AknLayoutScalable_Apps::list_single_cmail_header_caption_pane() );
    TAknLayoutText captionTextRect;
    captionTextRect.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() );
    return Abs( cmailPaneRect.iBr.iX - captionTextRect.TextRect().iBr.iX );
    }

// -----------------------------------------------------------------------------
// NcsUtility::ListCmailBodyPaneRect
// -----------------------------------------------------------------------------
//
TRect NcsUtility::ListCmailBodyPaneRect( const TRect& aParent )
    {
    FUNC_LOG;
    TAknLayoutRect listCmailBodyRect;
    listCmailBodyRect.LayoutRect( aParent, AknLayoutScalable_Apps::list_cmail_body_pane() );
    return listCmailBodyRect.Rect();
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutCaptionLabel
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutCaptionLabel( CEikLabel* aLabel, const TRect& aParentRect )
    {
    FUNC_LOG;
    AknLayoutUtils::LayoutLabel( 
            aLabel, NcsUtility::HeaderCaptionPaneRect( aParentRect ), 
            AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1().LayoutLine() );
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutCaptionLabel
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutCaptionButton( 
        CAknButton* aButton, const TRect& aParentRect)
    {
    FUNC_LOG;
    TAknLayoutText layoutText;
    
    const TInt labelVariety = 2;
    TAknTextComponentLayout textComponentLayout;
    TAknLayoutRect layoutRect;
    TRect rect;
    
    TInt fontId;
    TInt topMargin;
    TInt bottomMargin;
    
    // Getting editor's top and bottom margin from its layout
    textComponentLayout = 
        AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1(
            KHeaderDetailTextPaneVariety );
    fontId = textComponentLayout.LayoutLine().FontId();
    
    TAknTextDecorationMetrics metrics( fontId );
    metrics.GetTopAndBottomMargins( topMargin, bottomMargin );
    
    layoutRect.LayoutRect(
        HeaderDetailPaneRect( aParentRect, 1, ETrue ),
        AknLayoutScalable_Apps::bg_button_pane_cp12() );
    
    textComponentLayout = 
        AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t2( 
                labelVariety );
    layoutText.LayoutText(
        HeaderDetailPaneRect( aParentRect, 1, ETrue ),
        textComponentLayout.LayoutLine() );
    
    rect = layoutRect.Rect();
    rect.iTl.iY -= topMargin + bottomMargin;
    
    aButton->SetRect( rect );
    aButton->SetTextFont( layoutText.Font() );
    aButton->SetTextHorizontalAlignment( layoutText.Align() );
    TMargins8 margs( 0, 0, 5, 0 );
    aButton->SetMargins( margs );
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutDetailLabel
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutDetailLabel( 
        CEikLabel* aLabel, const TRect& aParentRect, TInt aRow)
    {
    FUNC_LOG;
    TRect rect( NcsUtility::HeaderDetailPaneRect( aParentRect, 1, ETrue ) );
    rect.Move( 0, aRow*rect.Height() );
    AknLayoutUtils::LayoutLabel( aLabel, rect, AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( 0 ).LayoutLine() );
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutDetailEdwin
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutDetailEdwin( 
        CEikEdwin* aEdwin, const TRect& aParentRect, 
        const TInt aLineCount, const TBool aBackIcon )
    {
    FUNC_LOG;
    TAknMultiLineTextLayout edwinLayout;
    const TInt textPaneVariety( aBackIcon ? 1 : 3 );
    TAknTextComponentLayout textLayout( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( textPaneVariety ) );
    RArray< TAknTextComponentLayout > edwinLayoutLines;
    TAknTextComponentLayout textRowLayout;
    TAknWindowComponentLayout headerDetailPane( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0, 0, 0 ) );
    for ( TInt line = 0; line < aLineCount; line++ )
        {
        textRowLayout = TAknWindowComponentLayout::ComposeText( TAknWindowComponentLayout::MultiLineY( headerDetailPane, line, 0 ), textLayout );
        edwinLayoutLines.Append( textRowLayout );
        }
    AknLayoutUtils::LayoutEdwin( aEdwin, NcsUtility::HeaderDetailPaneRect( aParentRect, aLineCount ), TAknTextComponentLayout::Multiline( edwinLayoutLines ) );
    edwinLayoutLines.Reset();
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutDetailEdwin
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutDetailEdwinTouch( 
        CEikEdwin* aEdwin, const TRect& aParentRect,
        const TInt aLineCount, const TBool /*aBackIcon*/  )
    {
    FUNC_LOG;
    TAknMultiLineTextLayout edwinLayout;
    TAknTextComponentLayout textLayout( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( KHeaderDetailTextPaneVariety ) );
    RArray< TAknTextComponentLayout > edwinLayoutLines;
    TAknTextComponentLayout textRowLayout;
    TAknWindowComponentLayout headerDetailPane( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0, 0, 0 ) );
    for ( TInt line = 0; line < aLineCount; line++ )
        {
        textRowLayout = TAknWindowComponentLayout::ComposeText( TAknWindowComponentLayout::MultiLineY( headerDetailPane, line, 0 ), textLayout );
        edwinLayoutLines.Append( textRowLayout );
        }
    AknLayoutUtils::LayoutEdwin( aEdwin, NcsUtility::HeaderDetailPaneRect( aParentRect, aLineCount, ETrue ), TAknTextComponentLayout::Multiline( edwinLayoutLines ) );
    edwinLayoutLines.Reset();
    }

// -----------------------------------------------------------------------------
// NcsUtility::HeaderControlPosition
// -----------------------------------------------------------------------------
//
TPoint NcsUtility::HeaderControlPosition( 
        const TRect& aParentRect, const TInt aCurrentLine )
    {
    FUNC_LOG;
    TAknLayoutRect detailPane;
    TAknWindowComponentLayout headerDetailPane( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0, 0, 0 ) );
    detailPane.LayoutRect( aParentRect, TAknWindowComponentLayout::MultiLineY( headerDetailPane, aCurrentLine, 0 ) );
    return detailPane.Rect().iTl;
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutHeaderControl
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutHeaderControl( 
        CCoeControl* aControl, const TRect& aParentRect, 
        const TInt aCurrentLine, const TInt aNumberOfLines )
    {
    FUNC_LOG;
    TAknLayoutRect detailPane;
    TAknWindowComponentLayout headerDetailPane( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0, 0, 0 ) );
    detailPane.LayoutRect( aParentRect, TAknWindowComponentLayout::MultiLineY( headerDetailPane, aCurrentLine, 0 ) );
    TRect rect( detailPane.Rect() );
    rect.SetHeight( rect.Height() * aNumberOfLines );
    aControl->SetRect( rect );
    }

// -----------------------------------------------------------------------------
// NcsUtility::LayoutBodyEdwin
// -----------------------------------------------------------------------------
//
void NcsUtility::LayoutBodyEdwin( 
        CEikEdwin* aEdwin, const TRect& aParentRect, const TInt aCurrentLine, 
        const TInt aNumberOfLines, TInt& aSeparatorYPosition )
    {
    FUNC_LOG;
    TAknMultiLineTextLayout edwinLayout;
    TAknTextComponentLayout textLayout( AknLayoutScalable_Apps::list_cmail_body_pane_t1() );
    RArray< TAknTextComponentLayout > edwinLayoutLines;
    TAknTextComponentLayout textRowLayout;
    TAknWindowComponentLayout headerCaptionPane( AknLayoutScalable_Apps::list_single_cmail_header_caption_pane( 0, 0, 0 ) );
    TInt lines = Max( aNumberOfLines, 2 );
    for ( TInt line = 0; line < lines; line++ )
        {
        textRowLayout = TAknWindowComponentLayout::ComposeText( TAknWindowComponentLayout::MultiLineY( headerCaptionPane, line, 0 ), textLayout );
        edwinLayoutLines.Append( textRowLayout );
        }
    TAknLayoutRect bodyPane;
    bodyPane.LayoutRect( aParentRect, AknLayoutScalable_Apps::list_cmail_body_pane() );
    TRect bodyRect( bodyPane.Rect() );
    TAknLayoutRect detailPane;
    TAknWindowComponentLayout headerDetailPane( AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0, 0, 0 ) );
    detailPane.LayoutRect( aParentRect, TAknWindowComponentLayout::MultiLineY( headerDetailPane, aCurrentLine, 0 ) );
    bodyRect = TRect( detailPane.Rect().iTl, bodyRect.Size() );
    aSeparatorYPosition = bodyRect.iTl.iY;
    // Since layout doesn't specify enough space between separator line
    // and editor, we add some space manually
    TInt deltaMove = NcsUtility::SeparatorSizeInThisResolution().iHeight;
    bodyRect.Move( 0, deltaMove );
    AknLayoutUtils::LayoutEdwin( aEdwin, bodyRect, TAknTextComponentLayout::Multiline( edwinLayoutLines ), EAknsCIQsnTextColorsCG6 );
    edwinLayoutLines.Reset();
    }

// -----------------------------------------------------------------------------
// NcsUtility::CalculateSeparatorLineSecondaryColor
//
// Calculates secondary color value based on the start value. Useful for
// gradient drawing.
// @param aDegree = color change coefficient
// -----------------------------------------------------------------------------
//
TRgb NcsUtility::CalculateSeparatorLineSecondaryColor( TInt aDegree,
                                                       TRgb /*aStartColor*/ )
    {
    TRgb secondaryColor;
    TInt redValue;
    TInt greenValue;
    TInt blueValue;
    TRgb startColor = SeparatorLineColor();
    if( startColor.Red() > KFSUtilityThresholdColorValue &&
            startColor.Green() > KFSUtilityThresholdColorValue &&
            startColor.Blue() > KFSUtilityThresholdColorValue )
        {
        //Original color is bright, so let's make secondary color darker
        redValue = startColor.Red() - ( startColor.Red() / aDegree );
        greenValue = startColor.Green() - ( startColor.Green() / aDegree );
        blueValue = startColor.Blue() - ( startColor.Blue() / aDegree );
        }
    else
        {
        //Original color is dark, so let's make secondary color brighter
        //Checking how bright original color is
        if( startColor.Red() < KFSUtilityThresholdColorMiddleValue &&
                startColor.Green() < KFSUtilityThresholdColorMiddleValue &&
                startColor.Blue() < KFSUtilityThresholdColorMiddleValue )
            {
            // Calculating new values from the unoccupied color space
            redValue = startColor.Red() + ( (
                    KFSUtilityMaxColorValue - startColor.Red() ) / aDegree );
            greenValue = startColor.Green() + ( (
                    KFSUtilityMaxColorValue - startColor.Green() ) / aDegree );
            blueValue = startColor.Blue() + ( (
                    KFSUtilityMaxColorValue - startColor.Blue() ) / aDegree );
            }
        else
            {
            // Calculating new values from the occupied color space
            redValue = startColor.Red() + ( startColor.Red() / aDegree );
            greenValue = startColor.Green() + ( startColor.Green() / aDegree );
            blueValue = startColor.Blue() + ( startColor.Blue() / aDegree );
            }
        }

    if ( redValue > KFSUtilityMaxColorValue )
        {
        redValue = KFSUtilityMaxColorValue;
        }
    if ( greenValue > KFSUtilityMaxColorValue )
        {
        greenValue = KFSUtilityMaxColorValue;
        }
    if ( blueValue > KFSUtilityMaxColorValue )
        {
        blueValue = KFSUtilityMaxColorValue;
        }
    return TRgb( redValue , greenValue, blueValue );
    }

// -----------------------------------------------------------------------------
// NcsUtility::CalculateMsgBodyLineColor
//
// Calculates line color to be used in message body. Calculated color is darker than
// given base color.
// @param aDegree = color darkening coefficient
// @param aBaseColor = base color to be used in calculation
// -----------------------------------------------------------------------------
//
TRgb NcsUtility::CalculateMsgBodyLineColor( TInt aDegree,
                                            TRgb aBaseColor )
    {
    TInt redValue = aBaseColor.Red() - ( aBaseColor.Red() / aDegree );
    TInt greenValue = aBaseColor.Green() - ( aBaseColor.Green() / aDegree );
    TInt blueValue = aBaseColor.Blue() - ( aBaseColor.Blue() / aDegree );

    if ( redValue < KFSUtilityMinColorValue )
        {
        redValue = KFSUtilityMinColorValue;
        }
    if ( greenValue < KFSUtilityMinColorValue )
        {
        greenValue = KFSUtilityMinColorValue;
        }
    if ( blueValue < KFSUtilityMinColorValue )
        {
        blueValue = KFSUtilityMinColorValue;
        }

    return TRgb( redValue , greenValue, blueValue );
    }

//End of file

