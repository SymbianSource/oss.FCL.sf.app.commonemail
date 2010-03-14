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
* Description: This file defines class NcsUtility.
*
*/



#ifndef NCSUTILITY_H
#define NCSUTILITY_H

#include <cntitem.h>
#include "cfsmailcommon.h"
#include "ncsaddressinputfield.h"

class CNcsEmailAddressObject;
class CFSMailAddress;
class CEikImage;
class CFbsBitmap;
class CFSMailMessage;
class CFSMailBox;
class CFSMailClient;
class CEikLabel;
class CEikEdwin;
class CAknButton;
class CCoeControl;

struct TNcsMeasures
	{
	TInt iAifHeight;
	TInt iAifLabelMarginVertical;
	TInt iAifEditorMarginVertical;
	TInt iAifEditorHeight;
	TInt iSubjectExtraHeightBottom;
	TInt iAttachmentExtraHeightBottom;
	TInt iIconMarginPriorityVertical;
	TInt iIconMarginFollowUpVertical;
	TInt iIconMarginAttachmentVertical;
	TInt iLabelFontHeightPx;
	TInt iEditorFontHeightPx;
	};


/**
*  NcsUtility class.
*/
class NcsUtility
    {
public:
    enum THeaderDetailIcon
        {
        EIconFront,
        EIconBack
        };

public:
    static void CompleteRelativePathL( const TDesC& aRelativePath, TPtr& aAbsolutePath );

    static HBufC* GenerateFromLineToMessageBodyL( const RPointerArray<CNcsEmailAddressObject>& aFromArray );

    static HBufC* GenerateSentLineToMessageBodyL( CFSMailMessage& aMessage );

    enum TRecipientType { ERecipientTypeTo, ERecipientTypeCc /*, ERecipientTypeBcc*/ };

    static HBufC* GenerateAddressLineToMessageBodyL( RPointerArray<CNcsEmailAddressObject>& aToArray, TRecipientType aType );

    static HBufC* GenerateSubjectLineToMessageBodyL( const TDesC& aSubject );

    static CNcsEmailAddressObject* CreateNcsAddressL( const CFSMailAddress& aFsAddress );

    static CFSMailAddress* CreateFsAddressL( const CNcsEmailAddressObject& aNcsAddress );

    static HBufC* FormatSubjectLineL( const TDesC& aNewSubjectLine, const TDesC& aPrefix );

    static void ConvertAddressArrayL( const RPointerArray<CFSMailAddress>& aSrc, RPointerArray<CNcsEmailAddressObject>& aDst );

    static void ConvertAddressArrayL( const RPointerArray<CNcsEmailAddressObject>& aSrc, RPointerArray<CFSMailAddress>& aDst );

    static TBool GetAddressesFromPhonebookL( RPointerArray<CNcsEmailAddressObject>& aAddresses );

    static void LoadScalableIconL( const TDesC& aMifRelativeFileName, TInt aImgId, TInt aMaskId, CEikImage*& aIcon, CFbsBitmap*& aImage, CFbsBitmap*& aMask );


    static void DebugLog( TRefByValue<const TDesC> aFmt, ... );

    static TBool IsEqualAddressArray( const RPointerArray<CFSMailAddress>& aAddresses1,
                                      const RPointerArray<CFSMailAddress>& aAddresses2 );

    static TNcsMeasures Measures();

    static CFont* GetNearestFontL( TInt aLogicalFontId, TInt aHeightPixels );

    enum TNcsLayoutFont
        {
        ENcsHeaderCaptionFont,
        ENcsHeaderDetailFont,
        ENcsBodytextFont
        };

    static const CFont* GetLayoutFont( const TRect& aParent, TNcsLayoutFont aUsage );

    static TBool IsReplyForwardQuoteReadOnly( const CFSMailBox& aMailBox );

    static TInt FindFirstPrintableCharIndex( const TDesC& aText );

    static void ConvertCharacters( TDes& aBuffer, TUint16 aFrom, TUint16 aTo );

    static void ConvertLineEndingsL( HBufC*& aBuffer );

    static TInt DeleteMessage( CFSMailClient& aMailClient,
    		const TFSMailMsgId& aMailBoxId,
    		const TFSMailMsgId& aFolderId,
    		const TFSMailMsgId& aMsgId );

    static TRect HeaderCaptionPaneRect( const TRect& aParent );

    static TRect HeaderDetailPaneRect( const TRect& aParent, const TInt aRows, TBool aSameRow = EFalse );

    static TRect HeaderDetailIconRect( const TRect& aParent, const TInt aRows , const THeaderDetailIcon aIcon );

    static TRect ListscrollPaneRect();
    
    static TRect ListCmailPaneRect( const TRect& aParent );

    static TRect ListCmailScrollbarRect( const TRect& aParent );

    static TSize SeparatorSizeInThisResolution();
    static TInt ComposerLeftMarginInPixels( const TRect& aParent );
    static TInt ComposerRightMarginInPixels( const TRect& aParent );
    static TRect ListCmailBodyPaneRect( const TRect& aParent );

    static void LayoutCaptionLabel( CEikLabel* aLabel, const TRect& aParentRect );
    
    static void LayoutCaptionButton( CAknButton* aButton, const TRect& aParentRect);

    static void LayoutDetailLabel( CEikLabel* aLabel, const TRect& aParentRect, TInt aRow = 0 );

    static void LayoutDetailEdwin( CEikEdwin* aEdwin, const TRect& aParentRect, const TInt aLineCount, const TBool aBackIcon );

    static void LayoutDetailEdwinTouch( CEikEdwin* aEdwin, const TRect& aParentRect,
                                        const TInt aLineCount, const TBool aBackIcon  );

    static TPoint HeaderControlPosition( const TRect& aParentRect, const TInt aCurrentLine );

    static void LayoutHeaderControl( CCoeControl* aControl, const TRect& aParentRect, const TInt aCurrentLine, const TInt aNumberOfLines );

    static void LayoutBodyEdwin( CEikEdwin* aEdwin, const TRect& aParentRect, const TInt aCurrentLine, const TInt aNumberOfLines,
            TInt& aSeparatorYPosition );
    static TRgb CalculateSeparatorLineSecondaryColor( TInt aDegree, TRgb aStartColor );
    static TRgb SeparatorLineColor();
    static TRgb CalculateMsgBodyLineColor( TInt aDegree, TRgb aBaseColor );

private:
    static HBufC* DoGenerateAddressLineToMessageBodyL( TInt aLabelResourceId,
            const RPointerArray<CNcsEmailAddressObject>& aAddressArray );
    };

#endif // TNCSUTILITY_H


// End of File
