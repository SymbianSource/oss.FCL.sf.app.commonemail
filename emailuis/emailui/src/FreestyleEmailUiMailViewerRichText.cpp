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
* Description: FreestyleEmailUi mail viewer rich text implementation
* Version    : %version: e003sa37#49 %
*
*/


// SYSTEM INCLUDES
#include "emailtrace.h"
#include <AknUtils.h>
#include <freestyleemailui.mbg>
#include <FreestyleEmailUi.rsg>
#include <StringLoader.h>
#include <AknBidiTextUtils.h>
//<cmail>
#include "fstextviewer.h"
#include "fsrichtext.h"
#include "cfsmailmessagepart.h"
#include "cfsmailclient.h"
#include "cfsmailcommon.h"
//</cmail>
#include <txtfrmat.h>
// <cmail> SF
#include <alf/alftextvisual.h>
// </cmail>
//<cmail> Platform layout changes
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
//</cmail> Platform layout changes

// for fonts
#include <AknFontAccess.h>
#include <AknLayoutFont.h>

// <cmail> separator line   
#include <w32std.h>
#include <coecntrl.h>
// </cmail> separator line  

// INTERNAL INCLUDES
#include "FreestyleEmailUiMailViewerRichText.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiLiterals.h"
#include "FreestyleEmailUiFileSystemInfo.h"
#include "FreestyleEmailUi.hrh"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "FreestyleEmailUiMailViewerConstants.h"
#include "ncsconstants.h" // for KSentLineDateAndTimeSeparatorText


// CONST VALUES
static const TInt KHotspotHighlightOn = 1;
// Max hotspot count is a maximum number of hotspots allowed to be placed into
// fs_generic text viewer component.
static const TInt KMaxHotspotCount = 300;

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::NewL
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerRichText* CFSEmailUiMailViewerRichText::NewL(
        CFreestyleEmailUiAppUi& aAppUi )
    {
    FUNC_LOG;
    CFSEmailUiMailViewerRichText* self =
        CFSEmailUiMailViewerRichText::NewLC( aAppUi );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::NewLC
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerRichText* CFSEmailUiMailViewerRichText::NewLC(
        CFreestyleEmailUiAppUi& aAppUi )
	{
    FUNC_LOG;
    CFSEmailUiMailViewerRichText* self =
        new (ELeave) CFSEmailUiMailViewerRichText( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::ConstructL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::ConstructL()
    {
    FUNC_LOG;

    CEikonEnv* env = CEikonEnv::Static();
    iViewerRichText = CFsRichText::NewL( env->SystemParaFormatLayerL(),
        env->SystemCharFormatLayerL() );

    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CFSEmailUiMailViewerRichText
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerRichText::CFSEmailUiMailViewerRichText(
        CFreestyleEmailUiAppUi& aAppUi )
	: iAppUi( aAppUi )
	{
    FUNC_LOG;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::~CFSEmailUiMailViewerRichText
// -----------------------------------------------------------------------------
CFSEmailUiMailViewerRichText::~CFSEmailUiMailViewerRichText()
	{
    FUNC_LOG;
	delete iViewerRichText;

	iFontsArray.ResetAndDestroy();

	iViewerHeadingHotSpotData.Reset();
	iBodyTextHotSpotData.Reset();
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SetHotspotByIndexL
// -----------------------------------------------------------------------------
// <cmail>
void CFSEmailUiMailViewerRichText::SetHotspotByIndexL( TInt aIndex )
    {
    FUNC_LOG;
    /*TInt headingHotspotCount = iViewerHeadingHotSpotData.Count();
    TInt bodyHotspotCount = iBodyTextHotSpotData.Count();

    if ( iTextViewer )
        {
        if ( aIndex < headingHotspotCount )
            {
            iTextViewer->FocusLineL( iViewerHeadingHotSpotData[aIndex].iHotspotLine , EFalse );
            iTextViewer->SetCurrentHotspotByOrderL( aIndex );
            }
        else if ( (aIndex - headingHotspotCount) < bodyHotspotCount )
            {
            iTextViewer->FocusLineL( iViewerHeadingHotSpotData[headingHotspotCount - 1].iHotspotLine , ETrue );
            iTextViewer->SetCurrentHotspotByOrderL( headingHotspotCount - 1 );
            }
        }*/
    // Remove the if/elseif contents above and replace them with the commented line below, once it becomes available in fs_generic
    iTextViewer->SetCurrentHotspotByOrderL( aIndex, ETrue );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentHotspotIndexL
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::CurrentHotspotIndexL()
    {
    FUNC_LOG;
    /*TInt hotspotIndex(0);

    if ( iTextViewer )
        {
        TInt start(0);
        TInt end(0);
        TInt headingHotspotCount = iViewerHeadingHotSpotData.Count();
        TInt bodyHotspotCount = iBodyTextHotSpotData.Count();
        TBool foundHotspot = EFalse;

        iTextViewer->GetCurrentHotspotL( start, end );

        if ( headingHotspotCount > 0 )
            {
            TInt headingHotspotEnd = iViewerHeadingHotSpotData[ headingHotspotCount-1 ].iEnd;
            if ( start < headingHotspotEnd )
                {
                for ( TInt i = 0; i < headingHotspotCount; ++i)
                    {
                    if ( iViewerHeadingHotSpotData[i].iStart == start )
                        {
                        foundHotspot = ETrue;
                        hotspotIndex = i;
                        break;
                        }
                    }
                }
            }

        if ( !foundHotspot )
            {
            for ( TInt i = 0; i < bodyHotspotCount; ++i)
                {
                if ( iBodyTextHotSpotData[i].iStartPos + iHeaderLength == start )
                    {
                    hotspotIndex = i + headingHotspotCount;
                    break;
                    }
                }
            }
        }

    return hotspotIndex;*/
    
    return iTextViewer->GetCurrentHotspotByOrder();    
    }
// </cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::EmailStatusIconLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::EmailStatusIconLine()
    {
    FUNC_LOG;
    return iEmailStatusIconLine;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FollowupIconLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::FollowupIconLine()
    {
    FUNC_LOG;
    return iFollowupIconLine;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::RefreshRichTextL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::RefreshRichTextL( CFsTextViewer& aTextViewer,
                                                     CFSMailMessage& aMessage,
                                                     CFSMailBox& aMailBox )
	{
    FUNC_LOG;

    Reset();
    
    // Save new pointers
	iTextViewer = &aTextViewer;
	iMessage = &aMessage;
	iMailBox = &aMailBox;
	
	// Reconstruct the whole rich text
	// line by line according to information found from current message
	AppendFromLineL( *iMessage, iHeaderLineInfo );
	AppendRecipientLinesL( *iMessage, iHeaderLineInfo, EViewerRecipientTypeTo );
	AppendRecipientLinesL( *iMessage, iHeaderLineInfo, EViewerRecipientTypeCc );
	
	// show bcc field only if opened from sent folder
	TFSMailMsgId currentMessageFolderId = iMessage->GetFolderId();
	CFSMailFolder* currentFolder = NULL;
	
    TRAP_IGNORE( currentFolder = iAppUi.GetMailClient()->GetFolderByUidL( 
            iMessage->GetMailBoxId(), currentMessageFolderId ) );
            
	CleanupStack::PushL( currentFolder );
	if ( currentFolder && currentFolder->GetFolderType() == EFSSentFolder )
		{ 
	    AppendRecipientLinesL( *iMessage, iHeaderLineInfo, EViewerRecipientTypeBcc );
		}
    CleanupStack::PopAndDestroy( currentFolder );
    
	AppendSentLineL( *iMessage, iHeaderLineInfo );

	AppendSubjectLinesL( *iMessage, iHeaderLineInfo );
    AppendHtmlTextLineL( *iMessage, iHeaderLineInfo );
	AppendAttachmentLinesL( *iMessage, iHeaderLineInfo );

	// Append little bit of vertical space before the header area ends.
    InsertTransparentSpacingIconL( EBottomMostSpace );

    // <cmail> separator line  		
	AppendSeparatorLineL();   
    // </cmail> separator line

    // Add the upmost space also just before the body text.
    InsertTransparentSpacingIconL( ETopMostSpace );

    // Save the total header lenght in characters
    iHeaderLength = iViewerRichText->DocumentLength();
    
    // Add body text
	AppendInitialBodytextLinesL( *iMessage );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::Reset
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::Reset()
    {
    FUNC_LOG;

    // reset all the data
    iTextViewer = NULL;
    iMessage = NULL;
    iMailBox = NULL;
    
    iViewerRichText->Reset();
    iViewerHeadingHotSpotData.Reset();
    iBodyTextHotSpotData.Reset();
    iStatusLayout = NULL;
    iStatusVisual = NULL;

    iHeaderLength = 0;
    iHeaderLineInfo.iHeaderLineCount = 0;
    iHeaderLineInfo.iToLineCount = 0;
    iHeaderLineInfo.iCcLineCount = 0;
    iHeaderLineInfo.iSubjectLineCount = 0;

    iAttachmentHotSpotIndex = KErrNotFound;
    iViewHtmlHotSpotIndex = KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::RichText
// -----------------------------------------------------------------------------
CFsRichText& CFSEmailUiMailViewerRichText::RichText()
	{
    FUNC_LOG;
	return *iViewerRichText;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SentTextLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::SentTextLine() const
    {
    FUNC_LOG;
    return iSentTextLine;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SubjectTextLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::SubjectTextLine() const
	{
    FUNC_LOG;
	return iSubjectTextLine;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AttachmentTextLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::AttachmentTextLine() const
	{
    FUNC_LOG;
	TInt line = KErrNotFound;
	if ( iAttachmentHotSpotIndex >= 0 &&
	     iAttachmentHotSpotIndex < iViewerHeadingHotSpotData.Count() )
	    {
	    line = iViewerHeadingHotSpotData[ iAttachmentHotSpotIndex ].iHotspotLine;
	    }
	return line;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::ViewHtmlTextLine
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::ViewHtmlTextLine() const
    {
    FUNC_LOG;
    TInt line = KErrNotFound;
    if ( iViewHtmlHotSpotIndex >= 0 &&
         iViewHtmlHotSpotIndex < iViewerHeadingHotSpotData.Count() )
        {
        line = iViewerHeadingHotSpotData[ iViewHtmlHotSpotIndex ].iHotspotLine;
        }
    return line;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::HeaderLengthInCharacters
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::HeaderLengthInCharacters() const
	{
    FUNC_LOG;
	return iHeaderLength;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::UpdateIconL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::UpdateIconL( TViewerIconType aViewerIconType )
	{
    FUNC_LOG;
	TInt place = 0;

// <cmail> icons changed

	CFbsBitmap* imageImage = NULL;
	CFbsBitmap* mask = NULL;
	TSize newIconSize = iAppUi.LayoutHandler()->ViewerIconSize();
	TFileName iconFile;
	TFsEmailUiUtility::GetFullIconFileNameL( iconFile );
	// Load right icon according to type
	// Email status and follow up icons have their fixed places.
	// Spacing and indentation icons are always appended to the end of the rich text and
	// they are always transparent.
	switch ( aViewerIconType )
		{
	    case EViewerEmailStatus:
	        {
	        TFSEmailUiTextures textureId = TFsEmailUiUtility::GetMsgIconTextureId( iMessage );
	        iAppUi.FsTextureManager()->ProvideBitmapL( textureId, imageImage, mask );
	        place = iEmailStatusIconPlace;
	        }
            break;
	    case EViewerIconFollowUp:
	        {
	        AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                imageImage,
                mask,
                iconFile,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_mask,
                KRgbBlack );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iFollowupIconPlace;
	        }
	        break;
	    case EViewerIconFollowUpComplete:
	        {
	        AknsUtils::CreateColorIconL(
                AknsUtils::SkinInstance(),
                KAknsIIDNone,
                KAknsIIDQsnIconColors,
                EAknsCIQsnIconColorsCG7,
                imageImage,
                mask,
                iconFile,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete,
                EMbmFreestyleemailuiQgn_indi_navi_follow_up_complete_mask,
                KRgbBlack );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iFollowupIconPlace;
	        }
	        break;
	    case EViewerIconFollowUpNone:
	        {
	        AknIconUtils::CreateIconL( imageImage, mask, iconFile,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank_mask );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iFollowupIconPlace;
	        }
	        break;
	    case EViewerAttachment:
	        {
	        AknIconUtils::CreateIconL( imageImage, mask, iconFile,
	                EMbmFreestyleemailuiQgn_indi_cmail_attachment,
	                EMbmFreestyleemailuiQgn_indi_cmail_attachment_mask );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iAttachmentIconPlace;
	        // Make the attachment icon a bit smaller than other icons. Shrink only the height:
	        // when aspect ratio is preserved, the shrinked image will be centered within the standard width icon.
	        // <cmail> Platform layout change
	        //newIconSize.iHeight = newIconSize.iHeight * 3 / 4;
            // </cmail> Platform layout change
	        }
	        break;
	    case EViewerIconIndentation:
	        {
	        AknIconUtils::CreateIconL( imageImage, mask, iconFile,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank_mask );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iViewerRichText->DocumentLength();
	        }
	        break;
	    case EViewerIconSpacing:
	        {
	        AknIconUtils::CreateIconL( imageImage, mask, iconFile,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank,
	                EMbmFreestyleemailuiQgn_graf_cmail_blank_mask );
	        AknIconUtils::DisableCompression( imageImage );
	        AknIconUtils::DisableCompression( mask );
	        place = iViewerRichText->DocumentLength();
	        // for spacing icon iCurrentSpacingIconSize need to be calculated beforehand
	        // and set properly
	        newIconSize = iCurrentSpacingIconSize;
	        }
	        break;
	    // <cmail> separator line   
	    case EViewerIconSeparatorLine:
	        {
            TRgb colorToFetch = iAppUi.LayoutHandler()->SeparatorLineColor();  	        
            TSize lineSize = iAppUi.LayoutHandler()->SeparatorGraphicSizeInThisResolution();           
            TInt KMyTempHigh = lineSize.iHeight;
            // Graphic elments width is too large for these rich text classes
            // and thus we have to calculate proper width by ourself.
            TInt KMyTempWidth =  SeparatorLineWidth();    
 	        
	        // create a bitmap to be used off-screen
	        CFbsBitmap* offScreenBitmap = new (ELeave) CFbsBitmap();
	        User::LeaveIfError(offScreenBitmap->Create(TSize(KMyTempWidth,KMyTempHigh), 
	                iAppUi.DisplayMode()));
	        CleanupStack::PushL(offScreenBitmap);
	        
	        mask = new (ELeave) CFbsBitmap(); 
            CleanupStack::PushL(mask);	        

	        // create an off-screen device and context
	        CGraphicsContext* bitmapContext=NULL;
	        CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(offScreenBitmap);
	        CleanupStack::PushL(bitmapDevice);
	        User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
	        CleanupStack::PushL(bitmapContext);

	        TRect rect(0,0,KMyTempWidth,KMyTempHigh);
	        bitmapContext->SetBrushColor(colorToFetch);
	        bitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	        bitmapContext->SetPenColor(colorToFetch);	        
	        bitmapContext->DrawRect(rect); 

	        imageImage = offScreenBitmap;
            AknIconUtils::DisableCompression( imageImage );
            place = iViewerRichText->DocumentLength();	        
            newIconSize = TSize( KMyTempWidth,KMyTempHigh);
            // cleanup
            CleanupStack::PopAndDestroy(2);        
            CleanupStack::Pop(2);            
	        }
	        break;
	    // </cmail> separator line  	        
	    default:
	        {
	        User::Leave( KErrNotSupported );
	        }
            break;
		}
// </cmail>

	AknIconUtils::SetSize( imageImage, newIconSize, EAspectRatioPreserved );
	AknIconUtils::SetSize( mask, newIconSize, EAspectRatioPreserved );

	// Create picture object, which takes ownership of bitmaps
    CleanupStack::PushL( imageImage );
    CleanupStack::PushL( mask );
	CMyPicture* picture = new ( ELeave ) CMyPicture( newIconSize, *imageImage, *mask );
    CleanupStack::Pop( mask );
	CleanupStack::Pop( imageImage );

	CleanupStack::PushL( picture );

	// remove old picture if there is one.
	if ( place < iViewerRichText->DocumentLength() )
		{
		iViewerRichText->DeleteL( place, 1 );
		}

    // Insert header object to the right spot
    TPictureHeader header;
    header.iPicture = TSwizzle<CPicture>( picture );
	CleanupStack::Pop( picture ); // The following call will always take the ownership of the picture pointed in the header. Not only when succesful.
	iViewerRichText->InsertL( place, header );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FindCurrentHeaderHotSpotL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerRichText::FindCurrentHeaderHotSpotL( SViewerHeadingHotSpotData& aCurrentHotSpot ) const
	{
    FUNC_LOG;
	TBool foundHotSpot = EFalse;
	TInt start(0);
	TInt end(0);
	if ( iTextViewer )
		{
		iTextViewer->GetCurrentHotspotL( start, end );
		TInt headingHotSpotCount = iViewerHeadingHotSpotData.Count();
		if ( headingHotSpotCount > 0 )
			{
			TInt headingHotspotEnd = iViewerHeadingHotSpotData[ headingHotSpotCount-1 ].iEnd;
			if ( start < headingHotspotEnd )
				{
				for ( TInt i = 0 ; i < headingHotSpotCount ; i++ )
					{
					if ( iViewerHeadingHotSpotData[ i ].iStart == start )
						{
						aCurrentHotSpot = iViewerHeadingHotSpotData[ i ];
						foundHotSpot = ETrue;
						break;
						}
					}
				}
			}
		}
	return foundHotSpot;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FindCurrentBodyHotSpotL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerRichText::FindCurrentBodyHotSpotL( CFindItemEngine::SFoundItem& aCurrentHotSpot  ) const
	{
    FUNC_LOG;
	TBool foundHotSpot = EFalse;
	TInt start(0);
	TInt end(0);
	if ( iTextViewer )
		{
		iTextViewer->GetCurrentHotspotL( start, end );
		for ( TInt a = 0 ; a < iBodyTextHotSpotData.Count() ; a++ )
			{
			if ( start == iBodyTextHotSpotData[a].iStartPos + iHeaderLength )
				{
				aCurrentHotSpot = iBodyTextHotSpotData[a];
				foundHotSpot = ETrue;
				break;
				}
			}
		}
	return foundHotSpot;
 	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FindCurrentHotSpotL
// -----------------------------------------------------------------------------
THotspotType CFSEmailUiMailViewerRichText::FindCurrentHotSpotL(
									SViewerHeadingHotSpotData& aHeaderHotspot,
									CFindItemEngine::SFoundItem& aBodyHotspot ) const
	{
    FUNC_LOG;
	THotspotType retVal = ENoHotspot;
	if ( FindCurrentHeaderHotSpotL( aHeaderHotspot ) )
		{
		retVal = EHeaderHotspot;
		}
	else if ( FindCurrentBodyHotSpotL( aBodyHotspot ) )
		{
		retVal = EBodyHotspot;
		}
	return retVal;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::GetHotspotTextLC, heading hotspots
// -----------------------------------------------------------------------------
HBufC* CFSEmailUiMailViewerRichText::GetHotspotTextLC(
    const SViewerHeadingHotSpotData& aHotSpot ) const
    {
    FUNC_LOG;
    TInt hotspotTextLength = aHotSpot.iEnd - aHotSpot.iStart;
    if ( aHotSpot.iEnd == aHotSpot.iStart ) // check for special case
        {
        hotspotTextLength = 1;
        }

    return GetHotspotTextLC( aHotSpot.iStart, hotspotTextLength );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::GetHotspotTextLC, body text hotspots
// -----------------------------------------------------------------------------
HBufC* CFSEmailUiMailViewerRichText::GetHotspotTextLC(
    const CFindItemEngine::SFoundItem& /*aHotSpot*/ ) const
    {
    FUNC_LOG;
    HBufC* hotspotText = iTextViewer->GetCurrentHotspotTextL();
	CleanupStack::PushL( hotspotText );
    return hotspotText;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::GetEmailAddressLC
// -----------------------------------------------------------------------------
HBufC* CFSEmailUiMailViewerRichText::GetEmailAddressLC(
						const SViewerHeadingHotSpotData& aCurrentHotSpot ) const
	{
    FUNC_LOG;
	HBufC* emailAddress = NULL;
	if( aCurrentHotSpot.iType == ETypeFromAddressDisplayName )
		{
		CFSMailAddress* mailAddressObject = iMessage->GetSender();
		emailAddress = mailAddressObject->GetEmailAddress().AllocLC();
		}
	else if( aCurrentHotSpot.iType == ETypeToAddressDisplayName )
		{
		RPointerArray<CFSMailAddress>& addressArray = iMessage->GetToRecipients();
		emailAddress = addressArray[aCurrentHotSpot.iDisplayNameArrayIndex]->GetEmailAddress().AllocLC();
		}
	else if( aCurrentHotSpot.iType == ETypeCcAddressDisplayName )
		{
		RPointerArray<CFSMailAddress>& addressArray =  iMessage->GetCCRecipients();
		emailAddress = addressArray[aCurrentHotSpot.iDisplayNameArrayIndex]->GetEmailAddress().AllocLC();
		}
	else if( aCurrentHotSpot.iType == ETypeBccAddressDisplayName )
		{
		RPointerArray<CFSMailAddress>& addressArray = iMessage->GetBCCRecipients();
		emailAddress = addressArray[aCurrentHotSpot.iDisplayNameArrayIndex]->GetEmailAddress().AllocLC();
		}
	else if( aCurrentHotSpot.iType == ETypeEmailAddress )
		{
		emailAddress = GetHotspotTextLC( aCurrentHotSpot );
		}
	else
		{
		User::Leave( KErrNotSupported );
		}
	return emailAddress;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::GetHeaderHotspotEmailAddressLC
// -----------------------------------------------------------------------------
HBufC* CFSEmailUiMailViewerRichText::GetHeaderHotspotEmailAddressLC(
			const SViewerHeadingHotSpotData& aCurrentHeaderHotSpotData ) const
	{
    FUNC_LOG;
	return GetEmailAddressLC( aCurrentHeaderHotSpotData );
	}



// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendRecipientIndetationL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendRecipientIndetationL()
	{
    FUNC_LOG;
	// add transparent indentation icon here, to get right indentation in pixels
	UpdateIconL( EViewerIconIndentation );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendNewLineL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendNewLineL()
    {
    FUNC_LOG;
    AppendFormattedTextL( KLineFeed, EViewerFontText );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendSpaceL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendSpaceL()
    {
    FUNC_LOG;
    AppendFormattedTextL( KSpace, EViewerFontText );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::ClipHeaderInfoToFitLC
// -----------------------------------------------------------------------------
//<cmail>
HBufC* CFSEmailUiMailViewerRichText::ClipHeaderInfoToFitLC( const TDesC& aText, TBool aIsDisplayName )
	{
	FUNC_LOG;
	// Clip to fit in available amouth of pixels
    const CAknLayoutFont* textNameFont = GetCorrectFontL( EViewerFontHotspotNormal );
    // <cmail> Platform layout change
    TInt textWidthInPixels = HeaderIndentedTextAreaWidth( EFalse );
    // </cmail> Platform layout change
	// create new descriptor for the clipped text.
	HBufC* textBuffer = aText.AllocLC();
	// Drop out unwanted characters from display name such as <> and ""
	if ( aIsDisplayName && textBuffer )
		{
		TFsEmailUiUtility::StripDisplayName( *textBuffer );
		textWidthInPixels -= iAppUi.LayoutHandler()->ViewerIconSize().iWidth;
		}
	TPtr textBufferModDes = textBuffer->Des();
	AknTextUtils::ClipToFit( textBufferModDes, *textNameFont, textWidthInPixels );
	return textBuffer;
	}
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::HeaderIndentedTextAreaWidth
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::HeaderIndentedTextAreaWidth( const TBool aAddExtra ) const
    {
    FUNC_LOG;
    // For some reason, the ratioanally calculated text width seems to be a bit
    // wider than the width where GenericTextViewer does line wrapping. Reserve
    // some extra pixels to overcome this. Maybe there is some difference in
    // the ways the text width is calculated?
    // <cmail> Platform layout change
    //const TInt KExtraPixels = 10;
    TInt extraPixels( 10 );
    if ( aAddExtra )
        {
        const CAknLayoutFont* newFont = NULL;
        TRAP_IGNORE( newFont = GetCorrectFontL( EViewerFontText ) );
        if ( newFont )
            {
            TOpenFontMetrics fontMetrics;
            newFont->GetFontMetrics( fontMetrics );
            extraPixels = fontMetrics.MaxWidth();
            }
        }
    // </cmail> Platform layout change

    TRect screenRect = iTextViewer->GetControl()->DisplayArea();

    TInt recipientIndenInpixels = iAppUi.LayoutHandler()->ViewerIndentationIconSize().iWidth;
    TInt scrollBarBreadth = CEikScrollBar::DefaultScrollBarBreadth();

    // Calculate space left for actual text
    TInt rightMargin = iAppUi.LayoutHandler()->ViewerRightMarginInPixels() + scrollBarBreadth;
    TInt leftMargin = iAppUi.LayoutHandler()->ViewerLeftMarginInPixels();
    TInt textWidthInPixels = screenRect.iBr.iX - recipientIndenInpixels - rightMargin - leftMargin - extraPixels;

    return textWidthInPixels;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendFromLineL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendFromLineL( CFSMailMessage& aMessagePtr,
                                                    SMailMsgHeaderInfo& aHeaderInfo )
	{
    FUNC_LOG;

	// Add the upmost space before the header information
	InsertTransparentSpacingIconL( ETopMostSpace );

	CFSMailAddress* fromAddress = aMessagePtr.GetSender();
	if ( fromAddress )
		{
	 	// Append "from:" text
	  	HBufC* fromHeadingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_FROM );
        AppendFormattedTextL( *fromHeadingText, EViewerFontTitle );
		CleanupStack::PopAndDestroy( fromHeadingText );

		// Append new line
		AppendNewLineL();
		aHeaderInfo.iHeaderLineCount++;

		AppendRecipientIndetationL();

		TDesC* displayName = &fromAddress->GetDisplayName();
		TDesC* emailAddrs = &fromAddress->GetEmailAddress();
		if ( displayName && displayName->Length() != 0 )
			{
			HBufC* clippedDisplayName = ClipHeaderInfoToFitLC( *displayName, ETrue );

			// Append display name and hotspot
			InsertHotspotTextAndFontL( *clippedDisplayName );

			// Add hotspot information to rich text and hotspot data to header hotspots array
			TInt hotSpotLength = clippedDisplayName->Length();
			iViewerRichText->SetHotSpotL( iRichTextDocumentLength, hotSpotLength, KHotspotHighlightOn );
			SViewerHeadingHotSpotData hsData;
			hsData.iType = ETypeFromAddressDisplayName;
			hsData.iStart = iRichTextDocumentLength;
			hsData.iEnd = iRichTextDocumentLength + hotSpotLength;
			hsData.iDisplayNameArrayIndex = 0;
			hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
			iViewerHeadingHotSpotData.Append( hsData );

			CleanupStack::PopAndDestroy( clippedDisplayName );

			// Append new line
			AppendNewLineL();
			aHeaderInfo.iHeaderLineCount++;
			}
		else if ( emailAddrs && emailAddrs->Length() != 0 )
			{
			// clip emailAddress to fit in one line.
			HBufC* clippedDisplayName = ClipHeaderInfoToFitLC( *displayName, ETrue );

			// Append email address	and hotspot
			InsertHotspotTextAndFontL( *clippedDisplayName );
		 	TInt hotSpotLength = clippedDisplayName->Length();
			iViewerRichText->SetHotSpotL(iRichTextDocumentLength, hotSpotLength, KHotspotHighlightOn );
			SViewerHeadingHotSpotData hsData;
			hsData.iType = ETypeEmailAddress;
			hsData.iStart = iRichTextDocumentLength;
			hsData.iEnd = iRichTextDocumentLength + hotSpotLength;
			hsData.iDisplayNameArrayIndex = 0;
			hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
			iViewerHeadingHotSpotData.Append( hsData );

			CleanupStack::PopAndDestroy( clippedDisplayName );

			// Append new line
			AppendNewLineL();
			aHeaderInfo.iHeaderLineCount++;
			}
		}
	}


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendRecipientsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendRecipientsL(
	const RPointerArray<CFSMailAddress>& aRecipientArray, SMailMsgHeaderInfo& aHeaderInfo,
	TViewerRecipientType aRecipientType )
	{
    FUNC_LOG;

 	TInt showAddresses(0);
	TInt recipientCount = aRecipientArray.Count();
	TInt maxRecipientCount = iAppUi.LayoutHandler()->ViewerMaxRecipientLineCount();

	if ( recipientCount <= maxRecipientCount )
		{
		// All recipients fit to screen
		showAddresses = recipientCount;
		}
	else
		{
		// Reserve space for additional recipients count
		showAddresses = maxRecipientCount - 1;
		}

	for ( TInt i = 0; i < showAddresses; i++ )
		{
		iRichTextDocumentLength = iViewerRichText->DocumentLength();
		TDesC* displayName = &( aRecipientArray[i]->GetDisplayName() );
		TDesC* emailAddrs = &( aRecipientArray[i]->GetEmailAddress() );
		if ( displayName && displayName->Length() != 0 )
			{
			AppendRecipientIndetationL();

			HBufC* clippedDisplayName = ClipHeaderInfoToFitLC( *displayName, ETrue );

			// Append display name and set hotspot data
			InsertHotspotTextAndFontL( *clippedDisplayName );

			iViewerRichText->SetHotSpotL( iRichTextDocumentLength, clippedDisplayName->Length(), KHotspotHighlightOn );
			SViewerHeadingHotSpotData hsData;
			switch ( aRecipientType )
				{
				case EViewerRecipientTypeCc:
					hsData.iType = ETypeCcAddressDisplayName;
					break;
				case EViewerRecipientTypeBcc:
					hsData.iType = ETypeBccAddressDisplayName;
					break;
				case EViewerRecipientTypeTo:
				default:
					hsData.iType = ETypeToAddressDisplayName;
					break;
				}
			hsData.iStart = iRichTextDocumentLength;
			hsData.iEnd = iRichTextDocumentLength + clippedDisplayName->Length();
			hsData.iDisplayNameArrayIndex = i;
			hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
			iViewerHeadingHotSpotData.Append( hsData );

			CleanupStack::PopAndDestroy( clippedDisplayName );

			// Append new line
			AppendNewLineL();
			aHeaderInfo.iHeaderLineCount++;
			}
		else if ( emailAddrs && emailAddrs->Length() != 0 )
			{
			AppendRecipientIndetationL();

			// clip emailAddress to fit in one line.
			HBufC* clippedDisplayName = ClipHeaderInfoToFitLC( *displayName, ETrue );

			// Append email address name and set hotspot data
			InsertHotspotTextAndFontL( *clippedDisplayName );
			iViewerRichText->SetHotSpotL( iRichTextDocumentLength, clippedDisplayName->Length(), KHotspotHighlightOn );
			SViewerHeadingHotSpotData hsData;
			hsData.iType = ETypeEmailAddress;
			hsData.iStart = iRichTextDocumentLength;
			hsData.iEnd = iRichTextDocumentLength + clippedDisplayName->Length();
			hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
			iViewerHeadingHotSpotData.Append( hsData );

			CleanupStack::PopAndDestroy( clippedDisplayName );

			// Append new line
			AppendNewLineL();
			aHeaderInfo.iHeaderLineCount++;
			}
		}
	if ( recipientCount > showAddresses )
		{
		InsertTransparentSpacingIconL( EHeaderInfoSpace );

		AppendRecipientIndetationL();

		HBufC* nMoreRecipientsText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_N_MORE_RECIPIENTS, recipientCount );
		// Append n more recipients address name and set hotspot data
		InsertHotspotTextAndFontL( *nMoreRecipientsText );
		iViewerRichText->SetHotSpotL(iRichTextDocumentLength, nMoreRecipientsText->Length(), KHotspotHighlightOn );
		SViewerHeadingHotSpotData hsData;
		switch ( aRecipientType )
			{
			case EViewerRecipientTypeCc:
				hsData.iType = ETypeCcNMoreRecipients;
				break;
			case EViewerRecipientTypeBcc:
				hsData.iType = ETypeBccNMoreRecipients;
				break;
			case EViewerRecipientTypeTo:
			default:
				hsData.iType = ETypeToNMoreRecipients;
				break;
			}
		hsData.iStart = iRichTextDocumentLength;
		hsData.iEnd = iRichTextDocumentLength+nMoreRecipientsText->Length();
		hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
		iViewerHeadingHotSpotData.Append( hsData );
		// Append new line
		AppendNewLineL();
		aHeaderInfo.iHeaderLineCount++;
		CleanupStack::PopAndDestroy( nMoreRecipientsText );
		}
	}


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendRecipientLinesL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendRecipientLinesL( CFSMailMessage& aMessagePtr,
        SMailMsgHeaderInfo& aHeaderInfo, TViewerRecipientType aRecipientType )
 	{
    FUNC_LOG;

 	RPointerArray<CFSMailAddress>* mailAddressArray = NULL;
 	HBufC* headingText = NULL;
 	switch ( aRecipientType )
 		{
 		case EViewerRecipientTypeTo:
 			{
 			mailAddressArray = &aMessagePtr.GetToRecipients(); // ownership is not transfered.
 			headingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_TO );
 			}
 		break;
 		case EViewerRecipientTypeCc:
 			{
 			mailAddressArray = &aMessagePtr.GetCCRecipients(); // ownership is not transfered.
 			headingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_CC );
 			}
 		break;
 		case EViewerRecipientTypeBcc:
 			{
            mailAddressArray = &aMessagePtr.GetBCCRecipients(); // ownership is not transfered.
 			headingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_BCC );
			}
 		break;
 		default:
 			{
 			// nothing here.
 			}
 		break;
 		}

	if ( mailAddressArray && mailAddressArray->Count() )
		{
		// Add vertical empty space between last recipient and new heading info text
		InsertTransparentSpacingIconL( EHeaderInfoSpace );

        AppendFormattedTextL( *headingText, EViewerFontTitle );

		AppendNewLineL();
		aHeaderInfo.iHeaderLineCount++;

		// Append the recipients
		AppendRecipientsL( *mailAddressArray, aHeaderInfo, aRecipientType );
		}

	// cleanup if heading text was created
	if ( headingText )
		{
		CleanupStack::PopAndDestroy( headingText );
		}

 	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendSentLineL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendSentLineL( CFSMailMessage& aMessagePtr,
														   SMailMsgHeaderInfo& aHeaderInfo )
	{
    FUNC_LOG;

	InsertTransparentSpacingIconL( EHeaderInfoSpace );

	// Save "Sent ..." text line number for initial focus setting
	iSentTextLine = aHeaderInfo.iHeaderLineCount;

	HBufC* dateText = TFsEmailUiUtility::DateTextFromMsgLC( &aMessagePtr );
	HBufC* timeText = TFsEmailUiUtility::TimeTextFromMsgLC( &aMessagePtr );
	HBufC* sentHeadingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_SENT );
	HBufC* combinedSentText = sentHeadingText->ReAllocL( sentHeadingText->Length() + KSpace().Length() +
														dateText->Length() +
														KSentLineDateAndTimeSeparatorText().Length() +
														timeText->Length() );
	CleanupStack::Pop( sentHeadingText );
	CleanupStack::PushL( combinedSentText );
	TPtr combinedSentTextPtr = combinedSentText->Des();
	combinedSentTextPtr.Append( KSpace );
	combinedSentTextPtr.Append( *dateText );
	combinedSentTextPtr.Append( KSentLineDateAndTimeSeparatorText );
	combinedSentTextPtr.Append( *timeText );

    AppendFormattedTextL( *combinedSentText, EViewerFontSent );

	CleanupStack::PopAndDestroy( combinedSentText );
	CleanupStack::PopAndDestroy( timeText );
	CleanupStack::PopAndDestroy( dateText );

	AppendNewLineL();
	aHeaderInfo.iHeaderLineCount++;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendSubjectLinesL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendSubjectLinesL( CFSMailMessage& aMessagePtr,
														  SMailMsgHeaderInfo& aHeaderInfo )
	{
    FUNC_LOG;
	InsertTransparentSpacingIconL( EHeaderInfoSpace );

	// "Subject:" text
	HBufC* subjectHeadingText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_SUBJECT );
    AppendFormattedTextL( *subjectHeadingText, EViewerFontTitle );
	CleanupStack::PopAndDestroy( subjectHeadingText );

	AppendNewLineL();
	aHeaderInfo.iHeaderLineCount++;

	// Save "subject:" text line number for initial focus setting
	iSubjectTextLine = aHeaderInfo.iHeaderLineCount;

    TInt subjectLineCount = 0;

    // Get the subject text to modifyable buffer because truncation may be needed.
    // Use "no subject" text if not available.
    HBufC* subjectBuf = TFsEmailUiUtility::CreateSubjectTextLC( &aMessagePtr );
    TPtr subjectPtr = subjectBuf->Des();

    // Wrap and clip to maximum number of lines
    TInt subjectWidthInPixels = HeaderIndentedTextAreaWidth();
    TInt maxSubjectLines = iAppUi.LayoutHandler()->ViewerMaxSubjectLineCount();
    CArrayFixFlat<TPtrC>* wrappedArray = new (ELeave) CArrayFixFlat<TPtrC>( maxSubjectLines );
    CleanupStack::PushL( wrappedArray );
    CArrayFixFlat<TInt>* lineWidthArray = new (ELeave) CArrayFixFlat<TInt>( maxSubjectLines );
    CleanupStack::PushL( lineWidthArray );
    for ( TInt i = 0 ; i < maxSubjectLines ; ++i )
        {
        lineWidthArray->AppendL( subjectWidthInPixels );
        }
    //<cmail>
    const CAknLayoutFont* subjectFont = GetCorrectFontL( EViewerFontText );

    AknTextUtils::WrapToArrayAndClipL( subjectPtr, *lineWidthArray, *subjectFont, *wrappedArray );
    //</cmail>
    subjectLineCount = wrappedArray->Count();

    TInt i = 1; // indicating current subject line, starting as 1 = first line, 2 = second line
    for(  ; i <= subjectLineCount ; i++ )
        {
        // Rest of the lines have no preceeding icon, just indentation
        AppendRecipientIndetationL();

        TPtrC temp = (*wrappedArray)[i-1];
        AppendFormattedTextL( temp, EViewerFontText );

        // add line break
        AppendNewLineL();
        aHeaderInfo.iHeaderLineCount++;
        aHeaderInfo.iSubjectLineCount++;
        }

    CleanupStack::PopAndDestroy( lineWidthArray );
    CleanupStack::PopAndDestroy( wrappedArray );
    CleanupStack::PopAndDestroy( subjectBuf );
	}


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendMessageIconL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendMessageIconL()
	{
    FUNC_LOG;
	// append embedded icon.
	iEmailStatusIconPlace = iViewerRichText->DocumentLength();
	iEmailStatusIconLine = iHeaderLineInfo.iHeaderLineCount;
	UpdateIconL( EViewerEmailStatus );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendFollowUpIconL
// -----------------------------------------------------------------------------
TBool CFSEmailUiMailViewerRichText::AppendFollowUpIconL( CFSMailMessage& aMessagePtr )
	{
    FUNC_LOG;
	TBool added = EFalse;

	if ( TFsEmailUiUtility::IsFollowUpSupported( *iMailBox ) )
	    {
	    iFollowupIconPlace = iViewerRichText->DocumentLength();
	    iFollowupIconLine = iHeaderLineInfo.iHeaderLineCount;
	    if ( aMessagePtr.IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
	        {
	        UpdateIconL( EViewerIconFollowUpComplete );
	        }
	    else if ( aMessagePtr.IsFlagSet( EFSMsgFlag_FollowUp ) )
	        {
	        UpdateIconL( EViewerIconFollowUp );
	        }
	    else
	        {
	        UpdateIconL( EViewerIconFollowUpNone );
	        }
	    added = ETrue;
	    }

	return added;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendAttachmentIconL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendAttachmentIconL()
    {
    FUNC_LOG;
    // append embedded icon.
    iAttachmentIconPlace = iViewerRichText->DocumentLength();
    UpdateIconL( EViewerAttachment );

    InsertSpaceAfterIconL();
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendAttachmentLinesL
// Append attachement(s) line to the rich text.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendAttachmentLinesL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo )
 	{
    FUNC_LOG;
	if ( aMessagePtr.IsFlagSet( EFSMsgFlag_Attachments ) )
		{
		// get attachments and count
		RPointerArray<CFSMailMessagePart> attachments;
		CleanupResetAndDestroyClosePushL( attachments );
		aMessagePtr.AttachmentListL( attachments );
		TInt attachmentsCount = attachments.Count();
		HBufC* attachmentText = NULL;
		if ( attachmentsCount == 1 )
			{
			TDesC& attachmentName = attachments[0]->AttachmentNameL();
			TUint attachmentSize = attachments[0]->ContentSize(); // bytes

			// create size text including the unit and preceeding space character
			HBufC* sizeWithUnit = TFsEmailUiUtility::CreateSizeDescLC( attachmentSize );
			HBufC* sizeText = HBufC::NewL( sizeWithUnit->Length()+1 );
			sizeText->Des().Append( ' ' );
			sizeText->Des().Append( *sizeWithUnit );
			CleanupStack::PopAndDestroy( sizeWithUnit );
			CleanupStack::PushL( sizeText );

			// clip attachment name to fit in one line so that size information has enough space too.
			 //<cmail>
			const CAknLayoutFont* attachmentFont = GetCorrectFontL( EViewerFontHotspotNormal );

			TInt attachmentTextWidthInPixels = HeaderIndentedTextAreaWidth();
			TInt sizeTextInPixels = attachmentFont->TextWidthInPixels( *sizeText );
			TInt pixelsLeftForAttachmentName = attachmentTextWidthInPixels - sizeTextInPixels;

			// safe check if the size information is too wide for some reason.
			if ( pixelsLeftForAttachmentName < 0 )
				{
				attachmentText = HBufC::NewL( attachmentName.Length() + sizeText->Length() );
				attachmentText->Des().Append( attachmentName );
				attachmentText->Des().Append( *sizeText );
				TPtr attachmentTextBufferModDes = attachmentText->Des();
				AknTextUtils::ClipToFit( attachmentTextBufferModDes, *attachmentFont, attachmentTextWidthInPixels );
				}
			else
				{ // normal case
				HBufC* attacmentNameBuffer = attachmentName.AllocLC();
				TPtr attachmentNameBufferModDes = attacmentNameBuffer->Des();
				AknTextUtils::ClipToFit( attachmentNameBufferModDes, *attachmentFont, pixelsLeftForAttachmentName );

				attachmentText = HBufC::NewL( attacmentNameBuffer->Length() + sizeText->Length() );
				attachmentText->Des().Append( *attacmentNameBuffer );
				attachmentText->Des().Append( *sizeText );
				CleanupStack::PopAndDestroy( attacmentNameBuffer );
				}
			CleanupStack::PopAndDestroy( sizeText );
			}
        //</cmail>
		else if ( attachmentsCount > 1 )
			{
			TInt totalSize = 0;
			for( TInt i = 0; i < attachmentsCount ; i++ )
				{
				totalSize += attachments[i]->ContentSize();
				}
			HBufC* sizeText = TFsEmailUiUtility::CreateSizeDescLC( totalSize );
			attachmentText = StringLoader::LoadL( R_FSE_VIEWER_ATTACHMENTS_TEXT_WITH_SIZE,
												  *sizeText, attachmentsCount );
			CleanupStack::PopAndDestroy( sizeText );
			}

		// Append attachment line to rich text if there was attachments
		if ( attachmentText )
			{
			CleanupStack::PushL( attachmentText );

            InsertTransparentSpacingIconL( EHeaderInfoSpace );

			AppendAttachmentIconL();

			InsertHotspotTextAndFontL( *attachmentText );
			iViewerRichText->SetHotSpotL( iRichTextDocumentLength, attachmentText->Length(), KHotspotHighlightOn );
			SViewerHeadingHotSpotData hsData;
			if ( attachmentsCount == 1 )
				{
				hsData.iType = ETypeAttachment;
				}
			else
				{
				hsData.iType = ETypeAttachments;
				}
			hsData.iStart = iRichTextDocumentLength;
			hsData.iEnd = iRichTextDocumentLength + attachmentText->Length();
			hsData.iHotspotLine = aHeaderInfo.iHeaderLineCount;
			iAttachmentHotSpotIndex = iViewerHeadingHotSpotData.Count();
			iViewerHeadingHotSpotData.Append( hsData );
			CleanupStack::PopAndDestroy( attachmentText );

			// Append new lines
			AppendNewLineL();
			aHeaderInfo.iHeaderLineCount++;
			}
		CleanupStack::PopAndDestroy( &attachments );
		}
 	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendHtmlTextLineL
// Appends "View as HTML" selection hotspot to the rich text if message has HTML text part
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendHtmlTextLineL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo )
	{
    FUNC_LOG;
    
    CFSMailMessagePart* htmlBodyPart = aMessagePtr.HtmlBodyPartL();
    if ( htmlBodyPart )
		{
	    CleanupStack::PushL( htmlBodyPart );

	    // Add just a little bit of vertical space before the next lines
	    InsertTransparentSpacingIconL( EHeaderInfoSpace );

	    // Indent the "View as HTML" link
	    AppendRecipientIndetationL();

		HBufC* htmlLineText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_HTML );
		HBufC* clippedHtmlLine = ClipHeaderInfoToFitLC( *htmlLineText, EFalse );

		InsertHotspotTextAndFontL( *clippedHtmlLine );
		iViewerRichText->SetHotSpotL( iRichTextDocumentLength, clippedHtmlLine->Length(), KHotspotHighlightOn );
		SViewerHeadingHotSpotData hsData;
		hsData.iType = ETypeHtml;
		hsData.iStart = iRichTextDocumentLength;
		hsData.iEnd = iRichTextDocumentLength + htmlLineText->Length();
		hsData.iHotspotLine = iHeaderLineInfo.iHeaderLineCount;
		iViewHtmlHotSpotIndex = iViewerHeadingHotSpotData.Count();
		iViewerHeadingHotSpotData.Append( hsData );

		CleanupStack::PopAndDestroy( clippedHtmlLine );
		CleanupStack::PopAndDestroy( htmlLineText );
	    CleanupStack::PopAndDestroy( htmlBodyPart );

	    // Append one line feed plus little extra vertical space
        AppendNewLineL();
        aHeaderInfo.iHeaderLineCount++;
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendInitialBodytextLinesL
// Append body text which is found from the message to the end of the rich text.
// If plain text body part is not found, tries to convert body from html part
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendInitialBodytextLinesL( CFSMailMessage& aMessagePtr )
	{
    FUNC_LOG;
	CFSMailMessagePart* plainTextBodyPart = aMessagePtr.PlainTextBodyPartL();

	if ( plainTextBodyPart )
		{
		CleanupStack::PushL( plainTextBodyPart );

		TInt fetchedSize = plainTextBodyPart->FetchedContentSize();
		if ( fetchedSize > 0 )
			{
			// Limit displayed plain text body size to KPlainTextLimitationInBytes
			// currently set to 150K characters of text that should be more than enough for mobile email
			TInt bufSize = Min( fetchedSize, KPlainTextLimitationInChars );
            HBufC* plainTextData16 = HBufC::NewLC( bufSize );

			TPtr pointer = plainTextData16->Des();
			plainTextBodyPart->GetContentToBufferL( pointer, 0 );

            // Store body length
			iBodyLength = plainTextData16->Length();

			// append everything we have now
            AppendFormattedTextL( *plainTextData16, EViewerFontText );

			// Append hotspot data for content
			iBodyTextHotSpotData.Reset();
			FindBodyTextHotSpotsL( *plainTextData16 );
			CleanupStack::PopAndDestroy( plainTextData16 );
            // Make sure that text viewer is not filled with more than max
			// number of hotspot count
			TInt hotspotCount = iBodyTextHotSpotData.Count();
            if ( hotspotCount > KMaxHotspotCount )
                {
                hotspotCount = KMaxHotspotCount;
                }
			for ( TInt i=0; i < hotspotCount; i++ )
				{
				AddBodyHotsSpotWithTextFormatingL( iBodyTextHotSpotData[i] );
				}
			}
		CleanupStack::PopAndDestroy( plainTextBodyPart );
		}
    else
        {
        CFSMailMessagePart* htmlBodyPart = aMessagePtr.HtmlBodyPartL();
        if ( htmlBodyPart )
            {
            CleanupStack::PushL( htmlBodyPart );

            TInt fetchedBodySize = htmlBodyPart->FetchedContentSize();
            TInt wholeContentSize = htmlBodyPart->ContentSize();
            if ( fetchedBodySize == wholeContentSize )
                {
                HBufC* htmlData = HBufC::NewLC( fetchedBodySize );
                TPtr pointer = htmlData->Des();
                htmlBodyPart->GetContentToBufferL( pointer, 0 );
                HBufC* txtData = TFsEmailUiUtility::ConvertHtmlToTxtL( *htmlData );
                CleanupStack::PopAndDestroy( htmlData );
                CleanupStack::PushL( txtData );

                // User large fixed maximum amount of characters to append.
                // In normal cases all text is included.
                TPtrC truncatedText = txtData->Left( KPlainTextLimitationInChars );
                iBodyLength = truncatedText.Length();
                AppendFormattedTextL( truncatedText, EViewerFontText );
                // Append hotspot data for content
                iBodyTextHotSpotData.Reset();
                FindBodyTextHotSpotsL( truncatedText );
                CleanupStack::PopAndDestroy( txtData );
                // Make sure that text viewer is not filled with more than max
                // number of hotspot count
                TInt hotspotCount = iBodyTextHotSpotData.Count();
                if ( hotspotCount > KMaxHotspotCount )
                    {
                    hotspotCount = KMaxHotspotCount;
                    }
                for ( TInt i=0; i < hotspotCount; i++ )
                    {
                    AddBodyHotsSpotWithTextFormatingL( iBodyTextHotSpotData[i] );
                    }
                }

            CleanupStack::PopAndDestroy( htmlBodyPart );
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendFetchedBodytextLinesL
// Append rest of the body text replacing "Fetching more text"
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendFetchedBodytextLinesL( CFSMailMessage& aMessagePtr )
	{
    FUNC_LOG;
	TInt startBodyPoint = iBodyLength;
	CFSMailMessagePart* plainTextBodyPart = aMessagePtr.PlainTextBodyPartL();
	if ( plainTextBodyPart )
		{
		CleanupStack::PushL( plainTextBodyPart );
        // Fetching More text is not required any more
		SetEmptyStatusLayoutTextL();

		// Limit the whole data reading to KPlainTextLimitationInChars, usually this is more than
		// enough for mobile email client
		TInt fetchedSize = plainTextBodyPart->FetchedContentSize();
		TInt bufSize = Min( fetchedSize, KPlainTextLimitationInChars );
        HBufC* plainTextData16 = HBufC::NewLC( bufSize );

		// Get more fetched content to buffer
		TPtr pointer = plainTextData16->Des();
		plainTextBodyPart->GetContentToBufferL( pointer, 0 );

        // Store body length
		iBodyLength = plainTextData16->Length();

		// Safety check required by POP protocol. Sometimes iBodyLength changes
		// when message has been fetched. Following is added to prevent crashes related to that.
		if ( startBodyPoint > iBodyLength )
		    {
		    startBodyPoint = iBodyLength;
		    }

        // Insert rest of the body text after previously added body text
        TPtrC restOfThePlainText = plainTextData16->Mid( startBodyPoint );
        InsertFormattedTextL( restOfThePlainText, EViewerFontText,
            iHeaderLength + startBodyPoint );

        // Find hotspots from the added body text
		TInt previousBodyHotspotCount = iBodyTextHotSpotData.Count();
		FindBodyTextHotSpotsL( restOfThePlainText );
        // Make sure that text viewer is not filled with more than max
        // number of hotspot count
        TInt hotspotCount = iBodyTextHotSpotData.Count();
        if ( hotspotCount > KMaxHotspotCount )
             {
             hotspotCount = KMaxHotspotCount;
             }
		for ( TInt i = previousBodyHotspotCount; i < hotspotCount; i++ )
			{
			iBodyTextHotSpotData[i].iStartPos += startBodyPoint;
			AddBodyHotsSpotWithTextFormatingL( iBodyTextHotSpotData[i] );
			}

	 	iTextViewer->ReloadTextL();
		CleanupStack::PopAndDestroy( plainTextData16 );
		CleanupStack::PopAndDestroy( plainTextBodyPart );
		}

	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CreateStatusLayoutL
// Creates status layout to display fetching more text.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::CreateStatusLayoutL( TBool aSetEmptyContents /*= ETrue*/ )
    {
    FUNC_LOG;
    // ensure not created twice
    if ( !iStatusVisual && iTextViewer )
        {
        iStatusLayout = static_cast<CAlfAnchorLayout*>( iTextViewer->GetStatusLayout() );
        CAlfControl* textViewerControl = iTextViewer->GetControl();
        iStatusVisual = CAlfTextVisual::AddNewL( *textViewerControl, iStatusLayout );
        iStatusVisual->EnableShadow( EFalse );
        iStatusVisual->SetColor( iAppUi.LayoutHandler()->ViewerBodyTextColor() );
        
        TAlfTimedValue statusVisualOpacity;
        statusVisualOpacity.SetValueNow ( 1 );
        iStatusVisual->SetOpacity( statusVisualOpacity );
    
        // <cmail> Platform layout change
        /*iStatusVisual->SetTextStyle( iAppUi.LayoutHandler()->FSTextStyleFromIdL( EFSFontTypeSmallBold )->Id() );*/
        iStatusVisual->SetTextStyle( iAppUi.LayoutHandler()->FSTextStyleFromLayoutL( AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1() ).Id() );
        // </cmail> Platform layout change
        iStatusVisual->SetAlign( EAlfAlignHCenter, EAlfAlignVTop );

        if ( aSetEmptyContents )
            {
            SetEmptyStatusLayoutTextL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendFetchingMoreTextL
// metdod for updating the animated text while rest of the message is been fetched
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendFetchingMoreTextL()
    {
    FUNC_LOG;
    // Create status layout if it doesn't exist yet
    CreateStatusLayoutL( EFalse );    

    // add fetching content text with current animation		
    HBufC* fetchingContentText = StringLoader::LoadLC( R_FREESTYLE_EMAIL_UI_VIEWER_FETCHING_CONTENT_TEXT ); 

    // create buffer for "Fetching more text" and dots animation
    HBufC* newDes = HBufC::NewLC( fetchingContentText->Length() + KTotalNumberOfLines + KTotalNumberOfLines);
    TPtr des = newDes->Des();

    // add KTotalNumberOfLines always at the beginning
    for( TInt i = 0 ; i < KTotalNumberOfLines / KNumberOfLines ; i++ )
        {
        des.Append( KThreeLines );
        }

    // append fething text
    des.Append( *fetchingContentText );

    // add dots according to current ellipsis count
    for( TInt i = 0 ; i < iCurrentEllipsisCount ; i++ )
        {
        des.Append( KThreeLines );
        }

    // ellipses count counter going through 0,1,2,3,0,1,2,3,0,... sequence
    if( iCurrentEllipsisCount >= KTotalNumberOfLines / KNumberOfLines )
        {
        iCurrentEllipsisCount = 0;
        }
    else
        {
        iCurrentEllipsisCount++;
        }

    // just in case, try clipping the text if it doesn't fit in one line for some reason.
    TRect screenRect = iTextViewer->GetControl()->DisplayArea();
    //<cmail>
    const CAknLayoutFont* bodyTextFont = GetCorrectFontL( EViewerFontBody );
    TInt rightMargin = iAppUi.LayoutHandler()->ViewerRightMarginInPixels();
    TInt leftMargin = iAppUi.LayoutHandler()->ViewerLeftMarginInPixels();
    TInt pixelsToFit = screenRect.iBr.iX - rightMargin - leftMargin;
    AknTextUtils::ClipToFit( des, *bodyTextFont, pixelsToFit );
    //</cmail>

    // Assign the text to status layout.
    iStatusVisual->SetTextL( *newDes );
    iStatusLayout->SetClipping( EFalse );
    TSize statusSize = iStatusVisual->TextExtents();
    iStatusLayout->SetSize( statusSize );
    iStatusVisual->SetRect( TRect( TPoint( 0, 0 ), statusSize ) );
    iTextViewer->UpdateStatusLayout();

    CleanupStack::PopAndDestroy( newDes );
    CleanupStack::PopAndDestroy( fetchingContentText );
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SetEmptyStatusLayoutTextL
// The text in status layout is updated to contain only a couple of line feeds.
// This ensures, that the whole body can be scrolled visible also when the
// attachment downloading popup is shown in the bottom.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::SetEmptyStatusLayoutTextL()
    {
    FUNC_LOG;
    _LIT( KEmptyStatusText, "\n " );
    if ( iStatusVisual && iStatusLayout && iTextViewer )
        {
        iStatusVisual->SetTextL( KEmptyStatusText );
        iStatusLayout->SetClipping( EFalse );
        TSize statusSize = iStatusVisual->TextExtents();
        iStatusLayout->SetSize( statusSize );
        iStatusVisual->SetRect( TRect( TPoint( 0, 0 ), statusSize ) );
        iTextViewer->UpdateStatusLayout();
        }
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SetHotspotHighlightedColorL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::SetHotspotHighlightedColorL( TInt aStartIndx, TInt aLenght, TBool aHighlight )
	{
    FUNC_LOG;
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    if ( aHighlight )
        {
        charFormat.iFontPresentation.iTextColor = SkinFontColorByType( EViewerFontHotspotHighLighted );
        }
    else
        {
        charFormat.iFontPresentation.iTextColor = SkinFontColorByType( EViewerFontHotspotNormal );
        }

	charFormatMask.SetAttrib( EAttColor );
	iViewerRichText->ApplyCharFormatL( charFormat, charFormatMask, aStartIndx, aLenght );
	iTextViewer->ReloadTextL();
	}

// ---------------------------------------------------------------------------
// Getter for the index of the attachment hotspot
// ---------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::AttachmentHotSpotIndex() const
    {
    FUNC_LOG;
    return iAttachmentHotSpotIndex;
    }

// ---------------------------------------------------------------------------
// Getter for the index of the View HTML hotspot
// ---------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::ViewHtmlHotSpotIndex() const
    {
    FUNC_LOG;
    return iViewHtmlHotSpotIndex;
    }

// ---------------------------------------------------------------------------
// Helper function for getting hotspot text from rich text data
// ---------------------------------------------------------------------------
HBufC* CFSEmailUiMailViewerRichText::GetHotspotTextLC( TInt aStartPos,
    TInt aLength ) const
    {
    FUNC_LOG;
    __ASSERT_DEBUG( aStartPos >= 0, User::Invariant() );
    __ASSERT_DEBUG( aLength > 0, User::Invariant() );

    HBufC* des = HBufC::NewLC( aLength );

    // Hotspot text is retrived in a loop, because the CPlainText::Read
    // method may return only part of the requested text
    TInt documentLength = iViewerRichText->DocumentLength();
    while ( aLength > 0 && aStartPos < documentLength )
        {
        TPtrC text = iViewerRichText->Read( aStartPos, aLength );
        des->Des().Append( text );
        aLength -= text.Length();
        aStartPos += text.Length();
        }

    return des;
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FindBodyTextHotSpotsL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::FindBodyTextHotSpotsL( const TDesC& aBodyContent )
	{
    FUNC_LOG;
	// Use s60 find item engine to find hotspots from the body text
	CFindItemEngine* hotspotSearch = CFindItemEngine::NewL
		(
		aBodyContent,
		// define wanted
		(CFindItemEngine::TFindItemSearchCase)
		(CFindItemEngine::EFindItemSearchMailAddressBin |
		CFindItemEngine::EFindItemSearchPhoneNumberBin |
		CFindItemEngine::EFindItemSearchURLBin |
		CFindItemEngine::EFindItemSearchScheme )
		);
	CleanupStack::PushL( hotspotSearch );

	// <cmail>
	// insert found data to member variable iHotSpotData
	const CArrayFixFlat<CFindItemEngine::SFoundItem>* itemArray = hotspotSearch->ItemArray();
	TInt itemCount = hotspotSearch->ItemCount();
	for ( TInt i = 0; i < itemCount; i++ )
		{
		CFindItemEngine::SFoundItem* foundItem = const_cast<CFindItemEngine::SFoundItem*> ( &itemArray->At( i ) );
		TBool foundSameHotspotTwice = EFalse;
		// Check if same hotspot is found twice. This may happen when both flags
		// EFindItemSearchScheme and EFindItemSearchURLBin are ON for search request.
		for ( TInt j = i + 1; j < itemCount; j++ )
			{
			if ( foundItem->iStartPos == itemArray->At( j ).iStartPos )
				{
				foundSameHotspotTwice = ETrue;
				break;
				}
			}
		if ( !foundSameHotspotTwice )
			{
			// At this phase hotspot is one character too early
			// Change it to right place. If FindItemEngine implementation
			// will change then following line must be removed.
			//foundItem->iStartPos++;
			iBodyTextHotSpotData.AppendL( itemArray->At( i ) );
			}
		}
	// </cmail>

	CleanupStack::PopAndDestroy( hotspotSearch );
	}


//<cmail>
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::GetCorrectFontL
// -----------------------------------------------------------------------------
const CAknLayoutFont* CFSEmailUiMailViewerRichText::GetCorrectFontL( TViewerFontType aFontType ) const
    {
	FUNC_LOG;
     /*
     EViewerFontTitle = 0,
     EViewerFontText,
     EViewerFontBody,
     EViewerFontHotspotNormal,
     EViewerFontHotspotHighLighted,
     EViewerFontSent,
     EViewerFontIndentation
    */
    const CAknLayoutFont* font = NULL;

    switch ( aFontType )
        {
        case EViewerFontTitle:
        case EViewerFontSent:
            {
            font = CurrentTitleFontL();
            }
            break;
        case EViewerFontText:
        case EViewerFontBody:
        case EViewerFontHotspotNormal:
        case EViewerFontHotspotHighLighted:
        case EViewerFontIndentation:
        default:
            {
            font = CurrentTextFontL();
            }
            break;
        }

    if (NULL == font)
        User::Leave(KErrGeneral);

    return font;

    }

/*
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::ReConstructFontArrayL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::ReConstructFontArrayL()
	{
    FUNC_LOG;
	iFontsArray.ResetAndDestroy();

	// append title font
	CAknLayoutFont* titleFont = CurrentTitleFontLC();
	iFontsArray.AppendL( titleFont );
	CleanupStack::Pop( titleFont );

	// append text font
	CAknLayoutFont* textFont = CurrentTextFontLC();
	iFontsArray.AppendL( textFont );
	CleanupStack::Pop( textFont );

	// append body font
	CAknLayoutFont* bodyFont = CurrentTextFontLC();
	iFontsArray.AppendL( bodyFont );
	CleanupStack::Pop( bodyFont );

	// append not highlighted hotspot font
	CAknLayoutFont* normalHotspotFont = CurrentTextFontLC();
	iFontsArray.AppendL( normalHotspotFont );
	CleanupStack::Pop( normalHotspotFont );

	// append highlighted hotspot font
	CAknLayoutFont* highLightedHotspotFont = CurrentTextFontLC();
	iFontsArray.AppendL( highLightedHotspotFont );
	CleanupStack::Pop( highLightedHotspotFont );

	// append sent line font
	CAknLayoutFont* sentLineFont = CurrentSentTextFontLC();
	iFontsArray.AppendL( sentLineFont );
	CleanupStack::Pop( sentLineFont );

	// append indentation font
	CAknLayoutFont* indentationFont = CurrentTextFontLC();
	iFontsArray.AppendL( indentationFont );
	CleanupStack::Pop( indentationFont );
	}
*/

/*
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::FontFromFontArray
// -----------------------------------------------------------------------------
CAknLayoutFont& CFSEmailUiMailViewerRichText::FontFromFontArray( TViewerFontType aFontType ) const
	{
    FUNC_LOG;
	return *iFontsArray[aFontType];
	}
*/
//</cmail>


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SkinFontColorByType
// -----------------------------------------------------------------------------
TRgb CFSEmailUiMailViewerRichText::SkinFontColorByType( TViewerFontType aFontType )
	{
    FUNC_LOG;
	TRgb returnedColor;
	switch ( aFontType )
		{
		case EViewerFontTitle:
			{
			returnedColor = iAppUi.LayoutHandler()->ViewerTitleTextColor();
			}
			break;
		case EViewerFontBody:
			{
			returnedColor = iAppUi.LayoutHandler()->ViewerBodyTextColor();
			}
			break;
		case EViewerFontHotspotNormal:
			{
			returnedColor = iAppUi.LayoutHandler()->ViewerNormalHotspotTextColor();
			}
			break;
		case EViewerFontHotspotHighLighted:
			{
			returnedColor = iAppUi.LayoutHandler()->ViewerHighlightedHotspotTextColor();
			}
			break;
		case EViewerFontSent:
			{
			// <cmail>
			//returnedColor = iAppUi.LayoutHandler()->ViewerTitleFontHeight();
			returnedColor = iAppUi.LayoutHandler()->ViewerTitleTextColor();
			// </cmail>
		    }
			break;
		case EViewerFontText:
		default:
			{
			returnedColor = iAppUi.LayoutHandler()->ViewerTextTextColor();
			}
			break;
		}
	return returnedColor;
	}

//<cmail> LAYOUT CHANGES

/*
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentTitleFontLC
// -----------------------------------------------------------------------------
CAknLayoutFont* CFSEmailUiMailViewerRichText::CurrentTitleFontLC()
	{
    FUNC_LOG;
	TAknLogicalFontId logicalFontId = iAppUi.LayoutHandler()->ViewerTitleFontAknLogicalFontId();
	TAknFontSpecification spec( logicalFontId );
	TInt heightInPixels = iAppUi.LayoutHandler()->ViewerTitleFontHeight();
	spec.SetTextPaneHeight( heightInPixels );
	CWsScreenDevice* dev = CCoeEnv::Static()->ScreenDevice();
	CAknLayoutFont* font = AknFontAccess::CreateLayoutFontFromSpecificationL( *dev, spec );
	CleanupStack::PushL( font );
	return font;
	}
// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentTextFontLC
// -----------------------------------------------------------------------------
CAknLayoutFont* CFSEmailUiMailViewerRichText::CurrentTextFontLC()
	{
    FUNC_LOG;
	TAknLogicalFontId logicalFontId = iAppUi.LayoutHandler()->ViewerTextFontAknLogicalFontId();
	TAknFontSpecification spec( logicalFontId );
	TInt heightInPixels = iAppUi.LayoutHandler()->ViewerTextFontHeight();
	spec.SetTextPaneHeight( heightInPixels );
	CWsScreenDevice* dev = CCoeEnv::Static()->ScreenDevice();
	CAknLayoutFont* font = AknFontAccess::CreateLayoutFontFromSpecificationL( *dev, spec );
	CleanupStack::PushL( font );
	return font;
	}
*/


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentTitleFontL
// -----------------------------------------------------------------------------
const CAknLayoutFont* CFSEmailUiMailViewerRichText::CurrentTitleFontL() const
    {
	FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TAknLayoutRect cmailPaneRect;
    cmailPaneRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::list_cmail_pane());

    TAknLayoutRect labelRect;
    labelRect.LayoutRect(cmailPaneRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane());

    TAknLayoutText layoutText;
    layoutText.LayoutText(labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_caption_pane_t1());

    const CAknLayoutFont* font = CAknLayoutFont::AsCAknLayoutFontOrNull( layoutText.Font() );

    //All fonts must be loaded properly? Leave if something is failing.
    if(NULL == font)
        User::Leave(KErrGeneral);

    return font;

    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentTextFontL
// -----------------------------------------------------------------------------
const CAknLayoutFont* CFSEmailUiMailViewerRichText::CurrentTextFontL() const
    {
	FUNC_LOG;
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TAknLayoutRect cmailPaneRect;
    cmailPaneRect.LayoutRect(mainPaneRect, AknLayoutScalable_Apps::list_cmail_pane());

    //Using variety 0 -> 1 row line by default
    TAknLayoutRect labelRect;
    labelRect.LayoutRect(cmailPaneRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane( 0 ));

    TAknLayoutText layoutText;
    layoutText.LayoutText(labelRect.Rect(), AknLayoutScalable_Apps::list_single_cmail_header_detail_pane_t1( 0 ));

    const CAknLayoutFont* font = CAknLayoutFont::AsCAknLayoutFontOrNull( layoutText.Font() );

    //All fonts must be loaded properly? Leave if something is failing.
    if(NULL == font)
        User::Leave(KErrGeneral);

    return font;

    }


// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentSentTextFontLC
// -----------------------------------------------------------------------------
const CAknLayoutFont* CFSEmailUiMailViewerRichText::CurrentSentTextFontL() const
	{
	return CurrentTitleFontL();
	}
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::CurrentSpacingHeight
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::CurrentSpacingHeight( TViewerSpacingIconType aSpacingIconType ) const
	{
    FUNC_LOG;
	TInt heightInPixels = 0;
	switch( aSpacingIconType )
		{
		case ETopMostSpace:
			{
			heightInPixels = iAppUi.LayoutHandler()->ViewerTopMostSpaceHeight();
			}
			break;
		case ERecipientSpace:
			{
			heightInPixels = iAppUi.LayoutHandler()->ViewerRecipientSpaceHeight();
			}
			break;
		case EHeaderInfoSpace:
			{
			heightInPixels = iAppUi.LayoutHandler()->ViewerHeaderInfoSpaceHeight();
			}
			break;
		case EBottomMostSpace:
			{
			heightInPixels = iAppUi.LayoutHandler()->ViewerBottomMostSpaceHeight();
			}
			break;
		}
	return heightInPixels;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::InsertTransparentSpacingIconL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::InsertTransparentSpacingIconL( TViewerSpacingIconType aSpacingIconType )
	{
    FUNC_LOG;
	TInt currentSpace = CurrentSpacingHeight( aSpacingIconType );
	TInt fontSide = 0;
	switch ( aSpacingIconType )
		{
		case ETopMostSpace:
        case EBottomMostSpace:
			{
			TInt currentTextHeight = iAppUi.LayoutHandler()->ViewerTitleFontHeight();
			fontSide = currentSpace - currentTextHeight;
			}
			break;
		case ERecipientSpace:
			{
			TInt currentTextHeight = iAppUi.LayoutHandler()->ViewerTextFontHeight();
			fontSide = currentSpace - currentTextHeight;
			}
			break;
		case EHeaderInfoSpace:
			{
			TInt currentTextHeight = iAppUi.LayoutHandler()->ViewerTitleFontHeight();
			fontSide = currentSpace - currentTextHeight;
			}
			break;
		}
	iCurrentSpacingIconSize.SetSize( fontSide, fontSide );

	UpdateIconL( EViewerIconSpacing );

	// Add extra space before line ending so that we can modify the line height
	AppendSpaceL();

	AppendNewLineL();
	iHeaderLineInfo.iHeaderLineCount++;

	// to make the icon need less space.
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
	charFormat.iFontSpec.iHeight = CEikonEnv::Static()->ScreenDevice()->VerticalPixelsToTwips( fontSide );
    charFormat.iFontPresentation.iUnderline = EUnderlineOff;
    charFormatMask.SetAttrib( EAttFontHeight );
    charFormatMask.SetAttrib( EAttFontUnderline );
    iViewerRichText->ApplyCharFormatL( charFormat, charFormatMask, iViewerRichText->DocumentLength() - 3, 3 );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::InsertSpaceAfterIconL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::InsertSpaceAfterIconL()
	{
    FUNC_LOG;
	TInt spaceInPixels = iAppUi.LayoutHandler()->ViewerPixelsBetweenMsgStatusIconAndSubject();
	iCurrentSpacingIconSize.SetSize( spaceInPixels, spaceInPixels );
	UpdateIconL( EViewerIconSpacing );
	}

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::InsertHotspotTextAndFontL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::InsertHotspotTextAndFontL( const TDesC& aText )
    {
    FUNC_LOG;
    AppendFormattedTextL( aText, EViewerFontHotspotNormal );
    }

// -----------------------------------------------------------------------------
// Appends formatted text to the rich text
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendFormattedTextL( const TDesC& aText,
    TViewerFontType aFontType )
    {
    FUNC_LOG;
    iRichTextDocumentLength = iViewerRichText->DocumentLength();
    InsertFormattedTextL( aText, aFontType, iRichTextDocumentLength );
    }

// -----------------------------------------------------------------------------
// Inserts formatted text to the rich text
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::InsertFormattedTextL( const TDesC& aText,
    TViewerFontType aFontType, TInt aPosition )
    {
    FUNC_LOG;
    ASSERT( aPosition >= 0 );
    ASSERT( aPosition <= iViewerRichText->DocumentLength() );

    // Insert text at given position
    iViewerRichText->InsertL( aPosition, aText );

    //<cmail>
    const CAknLayoutFont* newFont = GetCorrectFontL( aFontType );

    TRgb fontColor = SkinFontColorByType( aFontType );
    TFontUnderline underline = EUnderlineOff;
    if ( aFontType == EViewerFontHotspotNormal )
        {
        underline = EUnderlineOn;
        }

    ApplyFontToTextL( newFont, aPosition, aText.Length(), fontColor, underline );
    //</cmail>
    }

//<cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::ApplyFontToTextL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::ApplyFontToTextL( const CAknLayoutFont* aFont,
														 TInt aStartInx,
														 TInt aLength,
														 TRgb& aColor,
														 TFontUnderline aUnderline )
	{
    FUNC_LOG;
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontSpec = aFont->FontSpecInTwips();
    // Applies antialiasing for all text in mail viewer.
    charFormat.iFontSpec.iFontStyle.SetBitmapType( EAntiAliasedGlyphBitmap );
    charFormat.iFontPresentation.iTextColor = aColor;
    charFormat.iFontPresentation.iUnderline = aUnderline;
    charFormatMask.SetAll();
    iViewerRichText->ApplyCharFormatL( charFormat, charFormatMask, aStartInx, aLength );
	}
//</cmail>

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AddBodyHotsSpotWithTextFormatingL
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AddBodyHotsSpotWithTextFormatingL( const CFindItemEngine::SFoundItem& aBodyHotSpotData )
	{
    FUNC_LOG;
	iViewerRichText->SetHotSpotL( aBodyHotSpotData.iStartPos + iHeaderLength,
									  aBodyHotSpotData.iLength, KHotspotHighlightOn );

	// add underlining and right color the the hotspots
	//<cmail>
	const CAknLayoutFont* newFont = GetCorrectFontL( EViewerFontText );
	TRgb fontColor = SkinFontColorByType( EViewerFontHotspotNormal );

	TFontUnderline underline = EUnderlineOn;
 	ApplyFontToTextL( newFont, aBodyHotSpotData.iStartPos + iHeaderLength,
 					 aBodyHotSpotData.iLength, fontColor, underline );
    //</cmail>

	}

// <cmail> separator line  

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::AppendSeparatorLineL
// Append attachement(s) line to the rich text.
// -----------------------------------------------------------------------------
void CFSEmailUiMailViewerRichText::AppendSeparatorLineL()
    {
    FUNC_LOG;
    UpdateIconL( EViewerIconSeparatorLine );       
    }

// -----------------------------------------------------------------------------
// CFSEmailUiMailViewerRichText::SeparatorLineWidth
// -----------------------------------------------------------------------------
TInt CFSEmailUiMailViewerRichText::SeparatorLineWidth() const
    {
    FUNC_LOG;
    // For some reason, the ratioanally calculated text width seems to be a bit
    // wider than the width where GenericTextViewer does line wrapping. Reserve
    // some extra pixels to overcome this. Maybe there is some difference in 
    // the ways the text width is calculated?
    const TInt KExtraPixels( 10 );

    TRect screenRect = iTextViewer->GetControl()->DisplayArea();
    TInt scrollBarBreadth = CEikScrollBar::DefaultScrollBarBreadth();
    
    // Calculate space left for actual text
    TInt rightMargin = iAppUi.LayoutHandler()->ViewerRightMarginInPixels() + scrollBarBreadth;
    TInt leftMargin = iAppUi.LayoutHandler()->ViewerLeftMarginInPixels();
    TInt widthInPixels = screenRect.iBr.iX - rightMargin - leftMargin - KExtraPixels;
    
    return widthInPixels;
    }

// </cmail> separator line  

// End of the file

