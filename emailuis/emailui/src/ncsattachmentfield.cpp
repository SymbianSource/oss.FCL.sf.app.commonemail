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
* Description:  message editor
*
*/


#include "emailtrace.h"
#include <StringLoader.h>
#include <AknUtils.h>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <FreestyleEmailUi.rsg>

#include "FreestyleEmailUiLayoutData.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "ncsattachmentfield.h"
#include "ncsutility.h"
#include "ncsheadercontainer.h"


CNcsAttachmentField* CNcsAttachmentField::NewL(
    TInt aLabelTextId,
    MNcsFieldSizeObserver* aSizeObserver,
    CNcsHeaderContainer* aParentControl )
    {
    FUNC_LOG;
    CNcsAttachmentField* self =
        new ( ELeave ) CNcsAttachmentField( aSizeObserver, aParentControl );
    CleanupStack::PushL( self );
    self->ConstructL( aLabelTextId );
    CleanupStack::Pop( self );
    return self;
    }

CNcsAttachmentField::~CNcsAttachmentField()
    {
    FUNC_LOG;
    delete iAttachmentLabel;

    delete iAttachmentName;
    delete iAttachmentSizeDesc;

    delete iAttachmentBitmap;
    delete iAttachmentMask;

    delete iActionMenuBitmap;
    delete iActionMenuMask;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::CNcsAttachmentField()
// -----------------------------------------------------------------------------
//
CNcsAttachmentField::CNcsAttachmentField( MNcsFieldSizeObserver* aSizeObserver,
        CNcsHeaderContainer* aParentControl ):
    MNcsControl( aSizeObserver ),
    iParentControl( aParentControl )
    {
    FUNC_LOG;
    }

void CNcsAttachmentField::ConstructL( TInt aLabelTextId )
    {
    FUNC_LOG;
    // create title label
	HBufC* textBuf = StringLoader::LoadLC( aLabelTextId );

	// create attachment label
    iAttachmentLabel = new ( ELeave ) CNcsLabel( *this, NULL );
    iAttachmentLabel->SetTextL( textBuf->Des() );

	CleanupStack::PopAndDestroy( textBuf );

    // <cmail> Platform layout change
	CreateIconsL();
	// </cmail> Platform layout change
	
	UpdateFontSize();
    UpdateColors();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::LineCount()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::LineCount() const
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::ScrollableLines()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::ScrollableLines() const
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::GetNumChars()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::GetNumChars() const
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::CursorLineNumber()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::CursorLineNumber() const
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::CursorPosition()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::CursorPosition() const
    {
    FUNC_LOG;
    return 0;
    }

// <cmail> Platform layout change
// -----------------------------------------------------------------------------
// CNcsAttachmentField::Reposition()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::Reposition(TPoint& aPt, TInt /*aWidth*/ )
    {
    FUNC_LOG;
    /*
	TSize sz( aWidth, MinimumHeight() );
	if ( Rect() != TRect( aPt, sz ) )
	    {
		SetExtent( aPt, sz );
	    }
    aPt.iY += Size().iHeight;
    */
    SetPosition( aPt );
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CNcsAttachmentField::GetLabelText()
// -----------------------------------------------------------------------------
//
const TDesC& CNcsAttachmentField::GetLabelText() const
    {
    FUNC_LOG;
	return KNullDesC;
    }

// <cmail> Platform layout change
// ---------------------------------------------------------------------------
// CNcsAttachmentField::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsAttachmentField::LayoutLineCount() const
    {
    FUNC_LOG;
    return ( IsVisible() ? 1 : 0 );
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CNcsAttachmentField::Draw()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::Draw( const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();

    // Draw text box
    // <cmail> Drawing removed </cmail>

    if ( IsFocused() )
    	{
    	// highlight for label when focused
	    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
	    TRgb imageColor;
	    if( AknsUtils::GetCachedColor( skin, imageColor,
	    		KAknsIIDFsHighlightColors, EAknsCIFsHighlightColorsCG4 ) != KErrNone )
	        {
		    if( AknsUtils::GetCachedColor( skin, imageColor,
		    		KAknsIIDQsnHighlightColors, EAknsCIQsnHighlightColorsCG2 ) != KErrNone )
		    	{
		        imageColor = KRgbBlue;
		    	}
	        }
	    
	    gc.SetPenStyle( CGraphicsContext::ENullPen );
	    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	    gc.SetBrushColor( imageColor );
	    
	    TRect highlightRect( iAttachmentLabel->Rect() );

	    TInt sizeTextInPixels = iAttachmentLabel->Font()->TextWidthInPixels( 
									*iAttachmentLabel->Text() );
	    
        highlightRect.SetWidth( sizeTextInPixels );
        highlightRect.Grow( 0, 1 );
	    
	    if( AknLayoutUtils::LayoutMirrored() )
	    	{
	    	highlightRect.Move( Rect().Size().iWidth - 
								highlightRect.Size().iWidth - 
								highlightRect.iTl.iX - 2, 0 );
	    	}
	    
	    gc.DrawRect( highlightRect );
    	}
    
    // Draw icons
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    // <cmail> Platform layout change

    gc.BitBltMasked( iAttachmentIconPos, iAttachmentBitmap, 
    		iAttachmentBitmap->SizeInPixels(), iAttachmentMask, ETrue );

    if( IsFocused() )
    	{
        gc.BitBltMasked( iActionMenuIconPos, iActionMenuBitmap, 
        		iActionMenuBitmap->SizeInPixels(), iActionMenuMask, ETrue );
    	}
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SizeChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SizeChanged()
    {
    FUNC_LOG;
 
    LayoutControls();
    // </cmail> Platform laytout change
	UpdateFontSize();
	UpdateColors();
    // The attachment text needs to be updated when label size changes because it
	// may now get truncated differently.
	TRAP_IGNORE( UpdateAttachmentTextL() );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::PositionChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::PositionChanged()
    {
    FUNC_LOG;
    // <cmail> Platform layout change
    LayoutControls();
    UpdateColors();
   
    // </cmail> Platform layout change
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::MinimumHeight()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::MinimumHeight() const
    {
    FUNC_LOG;
    // <cmail> Platform laytout change
    /*
    TNcsMeasures m = NcsUtility::Measures();
    return m.iAifHeight + m.iAttachmentExtraHeightBottom;*/
    //return NcsUtility::MinimumHeaderSize( Rect(), 1 ).iHeight;
    // </cmail> Platform laytout change
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SetMaxLabelLength()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetMaxLabelLength( TInt aMaxLength )
    {
    FUNC_LOG;
	iMaximumLabelLength = aMaxLength;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::GetMinLabelLength()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::GetMinLabelLength() const
    {
    FUNC_LOG;
    return 0;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SetContainerWindowL() const
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetContainerWindowL( const CCoeControl& aContainer )
	{
    FUNC_LOG;
	CCoeControl::SetContainerWindowL( aContainer );

	// Create the component array
    InitComponentArrayL();
	CCoeControlArray& controls = Components();
	controls.SetControlsOwnedExternally( ETrue );
	controls.AppendLC( iAttachmentLabel );
    CleanupStack::Pop( iAttachmentLabel );

    // set label properties
	UpdateFontSize();

    // Setup text alignment according the mirrored/normal layout.
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iAttachmentLabel->SetAlignment( EHRightVCenter );
        }
    else
        {
        iAttachmentLabel->SetAlignment( EHLeftVCenter );
        }
	}

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SetTextL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetTextL( const TDesC& aText )
    {
    FUNC_LOG;
    delete iAttachmentName;
    iAttachmentName = NULL;
    delete iAttachmentSizeDesc;
    iAttachmentSizeDesc = NULL;

    iAttachmentLabel->SetTextL( aText );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SetTextL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetTextL( const TDesC& aAttachmentName, const TDesC& aAttachmentSizeDesc )
    {
    FUNC_LOG;
    delete iAttachmentName;
    iAttachmentName = NULL;
    delete iAttachmentSizeDesc;
    iAttachmentSizeDesc = NULL;

    iAttachmentName = aAttachmentName.AllocL();
    iAttachmentSizeDesc = aAttachmentSizeDesc.AllocL();
    UpdateAttachmentTextL();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateAttachmentTextL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateAttachmentTextL()
    {
    FUNC_LOG;
    if ( iAttachmentName && iAttachmentSizeDesc )
        {
        // create decorated size text including the preceeding space character
        HBufC* decoratedSize = StringLoader::LoadLC( R_FSE_VIEWER_ATTACHMENTS_LIST_SIZE, 
                                                     *iAttachmentSizeDesc );
        HBufC* decoratedSizeWithSpace = HBufC::NewL( decoratedSize->Length() + 
                                                     KSpace().Length() +
                                                     KBidiRleStartChar().Length() +
                                                     KBidiPdfChar().Length() );
        decoratedSizeWithSpace->Des().Append( KSpace );
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            // Put size part into embedded right-to-left block to ensure correct rendering
            // even when name part is written from left-to-right.
            decoratedSizeWithSpace->Des().Append( KBidiRleStartChar );
            decoratedSizeWithSpace->Des().Append( *decoratedSize );
            decoratedSizeWithSpace->Des().Append( KBidiPdfChar );
            }
        else
            {
            decoratedSizeWithSpace->Des().Append( *decoratedSize );
            }
        CleanupStack::PopAndDestroy( decoratedSize );
        CleanupStack::PushL( decoratedSizeWithSpace );

        // clip attachment name to fit the field so that size information has enough space too.
        const CFont& font = *iAttachmentLabel->Font();

        TInt labelWidth = iAttachmentLabel->Size().iWidth;
        TInt sizeTextInPixels = font.TextWidthInPixels( *decoratedSizeWithSpace );
        TInt pixelsLeftForAttachmentName = labelWidth - sizeTextInPixels;

        HBufC* attachmentText = NULL; // will contain the complete text
        // safe check if the size information is too wide for some reason.
        if ( pixelsLeftForAttachmentName < 0 )
            {
            attachmentText = HBufC::NewLC( iAttachmentName->Length() + decoratedSizeWithSpace->Length() );
            attachmentText->Des().Append( *iAttachmentName );
            attachmentText->Des().Append( *decoratedSizeWithSpace );
            TPtr attachmentTextBufferModDes = attachmentText->Des();
            AknTextUtils::ClipToFit( attachmentTextBufferModDes, font, labelWidth );
            }
        else
            { // normal case
            HBufC* attacmentNameBuffer = iAttachmentName->AllocLC();
            TPtr attachmentNameBufferModDes = attacmentNameBuffer->Des();
            AknTextUtils::ClipToFit( attachmentNameBufferModDes, font, pixelsLeftForAttachmentName );

            attachmentText = HBufC::NewL( attacmentNameBuffer->Length() + decoratedSizeWithSpace->Length() );
            attachmentText->Des().Append( *attacmentNameBuffer );
            attachmentText->Des().Append( *decoratedSizeWithSpace );
            CleanupStack::PopAndDestroy( attacmentNameBuffer );
            CleanupStack::PushL( attachmentText );
            }

        iAttachmentLabel->SetTextL( *attachmentText );

        CleanupStack::PopAndDestroy( attachmentText );
        CleanupStack::PopAndDestroy( decoratedSizeWithSpace );
        }
    }

// <cmail> Platform layout change
// -----------------------------------------------------------------------------
// CNcsAttachmentField::ResizeIcons()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::ResizeIcons()
    {
    FUNC_LOG;
    const TSize frontIconSize( NcsUtility::HeaderDetailIconRect( Rect(), 1, NcsUtility::EIconFront ).Size() );
    if ( frontIconSize != iAttachmentBitmap->SizeInPixels() )
        {
        AknIconUtils::SetSize( iAttachmentBitmap, frontIconSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
        AknIconUtils::SetSize( iAttachmentMask, frontIconSize, EAspectRatioPreservedAndUnusedSpaceRemoved );
        }
    const TSize backIconSize( NcsUtility::HeaderDetailIconRect( Rect(), 1, NcsUtility::EIconBack ).Size() );
    if ( backIconSize != iActionMenuBitmap->SizeInPixels() )
        {
        AknIconUtils::SetSize( iActionMenuBitmap, backIconSize, EAspectRatioNotPreserved  );
        AknIconUtils::SetSize( iActionMenuMask, backIconSize, EAspectRatioNotPreserved  );
        }
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CNcsAttachmentField::FocusChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::FocusChanged( TDrawNow aDrawNow )
	{
    FUNC_LOG;
	if ( IsFocused() )
		{
		iAttachmentLabel->SetFocus( ETrue );

        // make sure that control is visible on screen
		if ( Rect().iTl.iY < 0 )
			{
			TPoint pt = TPoint( 0, 0 );
			Reposition( pt,Rect().Width() );
            iSizeObserver->UpdateFieldPosition( this );
			}
		else
		    {
		    TPoint pos = PositionRelativeToScreen();
		    pos.iY += Size().iHeight;
    	    CWsScreenDevice* screenDev = ControlEnv()->ScreenDevice();
    	    TPixelsAndRotation pix;
    		screenDev->GetDefaultScreenSizeAndRotation( pix );
    		const TInt h = pix.iPixelSize.iHeight;
    		if ( pos.iY >= h - h / 3 )
    		    {
    			TPoint pt = TPoint( 0, h / 3 );
    			Reposition( pt,Rect().Width() );
                iSizeObserver->UpdateFieldPosition( this );
    		    }
		    }

        if ( iParentControl )
            {
            TRAP_IGNORE( iParentControl->SetMskL() );
            }
		}
	else
		{
		iAttachmentLabel->SetFocus( EFalse );
		}

    iAttachmentLabel->SetUnderlining( IsFocused() );
    UpdateColors();
    
	if ( aDrawNow )
		{
		DrawNow();
		}
	else
	    {
	    DrawDeferred();
	    }
	}

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateColors()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateColors()
    {
    // <cmail> Platform layout change
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    TRgb textColor = KRgbBlack;

    if( IsFocused() )
    	{
        AknsUtils::GetCachedColor(
            skin,
            textColor,
            KAknsIIDQsnTextColors,
            EAknsCIQsnTextColorsCG24 );
    	}
    else
    	{
		AknsUtils::GetCachedColor(
			skin,
			textColor,
			KAknsIIDQsnTextColors,
			EAknsCIQsnTextColorsCG6 );
    	}
    TRAP_IGNORE( iAttachmentLabel->OverrideColorL( 
            EColorLabelText, textColor ) );

    textColor = KRgbBlack;

    TInt err = AknsUtils::GetCachedColor(
        skin,
        textColor,
        KAknsIIDQsnHighlightColors,
        EAknsCIQsnHighlightColorsCG3 );

    TRAP_IGNORE( iAttachmentLabel->OverrideColorL(
        EColorLabelTextEmphasis, textColor ) );

    textColor = KRgbBlack;

    err = AknsUtils::GetCachedColor(
        skin,
        textColor,
        KAknsIIDQsnHighlightColors,
        EAknsCIQsnHighlightColorsCG2 );

    TRAP_IGNORE( iAttachmentLabel->OverrideColorL(
        EColorLabelHighlightFullEmphasis, textColor ) );
    // </cmail> Platform layout change
    CFreestyleEmailUiAppUi* appUi =
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    iBorderColor = appUi->LayoutHandler()->ComposerFieldBorderColor();
    iBgColor = appUi->LayoutHandler()->ComposerFieldBackgroundColor();
    }

// <cmail> Platform layout change
// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateFontSize()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateFontSize()
	{
    FUNC_LOG;
    /*if ( iLabelFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iLabelFont );
        iLabelFont = NULL;
        }

    if ( iEditorFont )
        {
        ControlEnv()->ScreenDevice()->ReleaseFont( iEditorFont );
        iEditorFont = NULL;
        }

    // set label properties
	TNcsMeasures m = NcsUtility::Measures();
	if ( !iLabelFont )
	    {
	    TRAP_IGNORE( iLabelFont = NcsUtility::GetNearestFontL(EAknLogicalFontPrimarySmallFont,
	    	m.iLabelFontHeightPx) );
	    }

	iTitleLabel->SetFont( iLabelFont );

	if ( !iEditorFont )
		{
		TRAP_IGNORE( iEditorFont = NcsUtility::GetNearestFontL(EAknLogicalFontPrimarySmallFont,
			m.iEditorFontHeightPx) );
		}

	iAttachmentLabel->SetFont( iEditorFont );*/
    iEditorFont = NcsUtility::GetLayoutFont( iAttachmentLabel->Rect(), NcsUtility::ENcsHeaderDetailFont );
	iAttachmentLabel->SetFont( iEditorFont );
	}

// -----------------------------------------------------------------------------
// CNcsAttachmentField::LayoutControls()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::LayoutControls()
    {
    FUNC_LOG;
    const TRect rect( Rect() );
    NcsUtility::LayoutDetailLabel( iAttachmentLabel, rect );
    UpdateIconPositions( rect );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateIconPositions()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateIconPositions( const TRect& aRect )
    {
    FUNC_LOG;
    iAttachmentIconPos = NcsUtility::HeaderDetailIconRect( aRect, 1, NcsUtility::EIconFront ).iTl;
    iActionMenuIconPos = NcsUtility::HeaderDetailIconRect( aRect, 1, NcsUtility::EIconBack ).iTl;
    }
// </cmail> Platform layout change

// -----------------------------------------------------------------------------
// CNcsAttachmentField::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikMessageColorSchemeChange )
        {
        UpdateFontSize();
        UpdateColors();
        // <cmail> Platform layout change
        CreateIcons();
        // </cmail> Platform layout change
        DrawDeferred();
        }
    }

// <cmail> Platform layout change
// -----------------------------------------------------------------------------
// CNcsAttachmentField::CreateIcons()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::CreateIcons()
    {
    FUNC_LOG;
    TRAPD( error, CreateIconsL() );
    return error;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::CreateIconsL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::CreateIconsL()
    {
    FUNC_LOG;
    CFreestyleEmailUiAppUi* fsAppUi = static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    delete iAttachmentBitmap; 
    iAttachmentBitmap = NULL;
    delete iAttachmentMask; 
    iAttachmentMask = NULL;
    fsAppUi->FsTextureManager()->ProvideBitmapL( EAttachmentIcon, iAttachmentBitmap, iAttachmentMask );
    delete iActionMenuBitmap; 
    iActionMenuBitmap = NULL;
    delete iActionMenuMask; 
    iActionMenuMask = NULL;
    fsAppUi->FsTextureManager()->ProvideBitmapL( EListControlMenuIcon, iActionMenuBitmap, iActionMenuMask );
    ResizeIcons();
    }
// </cmail> Platform layout change
