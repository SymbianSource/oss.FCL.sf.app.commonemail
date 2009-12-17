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
* Description:  FreestyleEmailUi mail viewer rich text definition
*
*/

 
    
#ifndef __FREESTYLEEMAILUI_MAILVIEWERRICHTEXT_H__
#define __FREESTYLEEMAILUI_MAILVIEWERRICHTEXT_H__

// SYSTEM INCLUDE FILES    
#include <e32base.h>
#include <finditemengine.h> 
//<cmail>
#include <alf/alfanchorlayout.h>
//</cmail>

// Forward declarations
class CAknLayoutFont;
class CFSMailMessage;
class CFSMailBox;
class CFsTextViewer;
class CFsRichText;
class CFreestyleEmailUiAppUi;
class CAlfTextVisual;

// Header data
struct SMailMsgHeaderInfo
	{
	TInt iHeaderLineCount;
	TInt iToLineCount;
	TInt iCcLineCount;
	TInt iSubjectLineCount;
	TBool iFlagIconOnHtmlLine;
	};	

// Different hotspot types
enum TViewerHeadingHotspotType
	{
	ETypeFromAddressDisplayName = 0,
	ETypeToAddressDisplayName,
	ETypeCcAddressDisplayName,
	ETypeBccAddressDisplayName,
	ETypeEmailAddress,
	ETypeToNMoreRecipients,
	ETypeCcNMoreRecipients,
	ETypeBccNMoreRecipients,
	ETypeAttachment,
	ETypeAttachments,
	ETypeHtml
	};

// Different recipient types
enum TViewerRecipientType
	{
	EViewerRecipientTypeTo = 0,
	EViewerRecipientTypeCc,
	EViewerRecipientTypeBcc
	};

	
// Struct for one header hotspot data
struct SViewerHeadingHotSpotData
	{
	TViewerHeadingHotspotType iType;
	TInt iStart;
	TInt iEnd;
	TInt iDisplayNameArrayIndex;
	TInt iHotspotLine;
	};

// Body and header hotspots are different
enum THotspotType
	{
	ENoHotspot,
	EHeaderHotspot,
	EBodyHotspot
	};

	
/**
* CFSEmailUiMailViewerRichText is a rich text for the mail viewer visualiser
*	
*/
class CFSEmailUiMailViewerRichText : public CBase
	{
public:
	static CFSEmailUiMailViewerRichText* NewL( CFreestyleEmailUiAppUi& aFsTextureManager );	
	static CFSEmailUiMailViewerRichText* NewLC( CFreestyleEmailUiAppUi& aFsTextureManager );				
				
	virtual ~CFSEmailUiMailViewerRichText();

    void RefreshRichTextL( CFsTextViewer& aTextViewer, CFSMailMessage& aMessage, CFSMailBox& aMailBox );
    void Reset();

	// Provides handle to richtext
	CFsRichText& RichText();

	// Getters for text lines
    TInt SentTextLine() const;
	TInt SubjectTextLine() const;
	TInt AttachmentTextLine() const;
	TInt ViewHtmlTextLine() const;
	TInt HeaderLengthInCharacters() const;

    // Getters for hotspot indicies
    TInt AttachmentHotSpotIndex() const;
    TInt ViewHtmlHotSpotIndex() const;
    
	// Icon updating
	
	// Different icon types presented in message viewer.
	enum TViewerIconType
		{
		// status icon
		EViewerEmailStatus,
		// follow up
		EViewerIconFollowUp,
		EViewerIconFollowUpComplete,
		EViewerIconFollowUpNone,
		// attachment icon
		EViewerAttachment,
		// used for layouting
		EViewerIconIndentation,
		EViewerIconSpacing,
		// <cmail>
		EViewerIconSeparatorLine
       // </cmail>
		};

	// Append given icon type to rich text		
	void UpdateIconL( TViewerIconType aViewerIconType );

	// Helpers to get possible current hotpots asked
	TBool FindCurrentHeaderHotSpotL( SViewerHeadingHotSpotData& aHotSpot ) const;
	TBool FindCurrentBodyHotSpotL( CFindItemEngine::SFoundItem& aHotSpot ) const;

	THotspotType FindCurrentHotSpotL( SViewerHeadingHotSpotData& aHeaderHotspot,
										CFindItemEngine::SFoundItem& aBodyHotspot ) const;

	// Getters for current hotspot texts
	HBufC* GetHotspotTextLC( const SViewerHeadingHotSpotData& aCurrentHotSpot ) const;
	HBufC* GetHotspotTextLC( const CFindItemEngine::SFoundItem&  aCurrentHotSpot ) const;
	HBufC* GetEmailAddressLC( const SViewerHeadingHotSpotData& aCurrentHotSpot  ) const;

	HBufC* GetHeaderHotspotEmailAddressLC( const SViewerHeadingHotSpotData& aCurrentHeaderHotSpotData ) const;

    // Appending newly fetched text to the end of the body
    void AppendFetchedBodytextLinesL( CFSMailMessage& aMessagePtr ); 

    // Status layout to show "Fetching More - - -"
    void CreateStatusLayoutL( TBool aSetEmptyContents = ETrue );
    
    // For fetching updating "Fetching more" -text
    void AppendFetchingMoreTextL();
    void SetEmptyStatusLayoutTextL();
    
	void SetHotspotHighlightedColorL( TInt aStartIndx, TInt aLenght, TBool aHighlight );
	
	TInt CurrentHotspotIndexL();
	
	void SetHotspotByIndexL( TInt aIndex );

	TInt EmailStatusIconLine();
	TInt FollowupIconLine();

private:

    // Helper function for getting hotspot text from rich text data
    HBufC* GetHotspotTextLC( TInt aStartPos, TInt aLength ) const;

private: // Methods for constructing the view's rich text
	
	// Helper functions for constructing the rich text from the message
	void AppendRecipientIndetationL();
	void AppendNewLineL();
	void AppendSpaceL();
	void AppendMessageIconL();
 	TBool AppendFollowUpIconL( CFSMailMessage& aMessagePtr );
    void AppendAttachmentIconL();

    //<cmail>
	HBufC* ClipHeaderInfoToFitLC( const TDesC& aText, TBool aIsDisplayName = EFalse );
	TInt HeaderIndentedTextAreaWidth( const TBool aAddExtra = ETrue ) const;
    //</cmail>

	// These functions read actual data from the message and appends it in the 
	// correct format to the rich text
   	void AppendFromLineL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo );

   	void AppendRecipientLinesL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo, 
   								TViewerRecipientType aRecipientType );

   	void AppendSentLineL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo ); 
    void AppendSubjectLinesL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo ); 
	void AppendAttachmentLinesL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo );
    void AppendInitialBodytextLinesL( CFSMailMessage& aMessagePtr ); 
    void AppendHtmlTextLineL( CFSMailMessage& aMessagePtr, SMailMsgHeaderInfo& aHeaderInfo ); 
    void AppendRecipientsL( const RPointerArray<CFSMailAddress>& aRecipientArray, SMailMsgHeaderInfo& aHeaderInfo, TViewerRecipientType aRecipientType );
    // <cmail> separator line      
    void AppendSeparatorLineL();
    TInt SeparatorLineWidth() const;  
    // </cmail> separator line      
    
    void FindBodyTextHotSpotsL( const TDesC& aBodyContent );

private: // Methods for handling font and layout spesific things.

	// Different fonts types
	enum TViewerFontType
		{
		EViewerFontTitle = 0,
		EViewerFontText,
		EViewerFontBody,
		EViewerFontHotspotNormal,
		EViewerFontHotspotHighLighted,
		EViewerFontSent,
		EViewerFontIndentation
		};
		
    //<cmail>
    // Constructs all used fonts in on array for current text size.
    //void ReConstructFontArrayL();
    
    // For getting spesific font
    //CAknLayoutFont& FontFromFontArray( TViewerFontType aFontType );
    const CAknLayoutFont* GetCorrectFontL( TViewerFontType aFontType ) const;
    //</cmail>

	// For getting different font colors
	TRgb SkinFontColorByType( TViewerFontType aFontType );
	
    //<cmail>
	// For constructing different fonts.
	const CAknLayoutFont* CurrentTitleFontL() const;
	const CAknLayoutFont* CurrentTextFontL() const;
	const CAknLayoutFont* CurrentSentTextFontL() const;
    //</cmail>
	
	// Transparent icons are used for layouting purpose. 
	// Only way for setting spesific size of indentations or spaces between
	// lines in pixels
	// See LAF documents for more information and for exact sizes.

	// Different transparent icon types used for layout.
	enum TViewerSpacingIconType
		{
		ETopMostSpace = 0,	// top most space in the view before any text
		ERecipientSpace,	// space that is after header info text and actual recipient text
		EHeaderInfoSpace,	// space that is always after recipient text and before next header info text. 
		EBottomMostSpace,	// bottom most space in the header after all text
		ESpaceBetweenIconAndSubject
		};
	
	// Returns the needed height for the given spacing icon type
	TInt CurrentSpacingHeight( TViewerSpacingIconType aSpacingIconType ) const;

	// Appends the given spacig icon type to the rich text
	void InsertTransparentSpacingIconL( TViewerSpacingIconType aSpacingIconType );
	
	// Append certain size spacing icon after the email status or follow up icon.
	void InsertSpaceAfterIconL();
	
	// Appends given text with the hotspot font type formating to the rich text as a hotspot
	void InsertHotspotTextAndFontL( const TDesC& aText );
	
    // Appends given text with the given font type formatting to the rich text	
    void AppendFormattedTextL( const TDesC& aText, TViewerFontType aFontType );
	
    // Inserts given text with the given font type formatting to the given
    // position in the rich text
    void InsertFormattedTextL( const TDesC& aText, TViewerFontType aFontType,
        TInt aPosition );

    //<cmail>
    // Applies formating information to the rich text.
	void ApplyFontToTextL( const CAknLayoutFont* aFont, TInt aStartInx,
							 TInt aLength, TRgb& aColor, TFontUnderline aUnderline );
    //</cmail>

	// Appends hotspot formating to given body hotspot
	void AddBodyHotsSpotWithTextFormatingL( const CFindItemEngine::SFoundItem& aBodyHotSpotData );

private: // Construction
	
	CFSEmailUiMailViewerRichText( CFreestyleEmailUiAppUi& aFsTextureManager );
	
	void ConstructL();

private:

	// Text Viewer component's rich text    
    CFsRichText* iViewerRichText;

	// App Ui handle for getting the texture manager
	CFreestyleEmailUiAppUi& iAppUi;

	// Mail message pointer, changes always in do activate
    CFsTextViewer* iTextViewer; // not owned
    // Used to display Fetching more Text
    CAlfTextVisual* iStatusVisual;   // not owned

    // Used to display Fetching more Text    
    CAlfAnchorLayout* iStatusLayout; // not owned
    
    CFSMailMessage* iMessage; // not owned
    CFSMailBox*     iMailBox; // not owned 

	// Header data storage
	SMailMsgHeaderInfo iHeaderLineInfo;

	// Header and body hotspot arrays
	RArray<SViewerHeadingHotSpotData> iViewerHeadingHotSpotData;
	RArray<CFindItemEngine::SFoundItem> iBodyTextHotSpotData;
	
	// Total length of the header text after the text has been constructed
	TInt iHeaderLength;
	
	// Helper variable to store current document lenght while constructing rich text
	TInt iRichTextDocumentLength;

	// Amount of characters currently shown from the body part
    TInt iBodyLength;

	// Autofetch animation ellipsis count
	TInt iCurrentEllipsisCount;

	// Array fonts
	RPointerArray<CAknLayoutFont> iFontsArray;

    // Size of the next spacing icon to be drawn.
    TSize iCurrentSpacingIconSize;
    
	// Icon places
	TInt iEmailStatusIconPlace;
	TInt iEmailStatusIconLine;
	TInt iFollowupIconPlace;
    TInt iFollowupIconLine;
	TInt iAttachmentIconPlace;

	// Lines and hotsopt indicies for initial scrolling position handling
    TInt iSentTextLine;
	TInt iSubjectTextLine;
	TInt iAttachmentHotSpotIndex;
	TInt iViewHtmlHotSpotIndex;
	};

#endif //__FREESTYLEEMAILUI_MAILVIEWERRICHTEXT_H__
