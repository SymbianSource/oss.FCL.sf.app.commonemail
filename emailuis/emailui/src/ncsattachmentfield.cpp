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
#include <touchfeedback.h>

#include "FreestyleEmailUiLayoutData.h"
#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiLayoutHandler.h"
#include "ncsattachmentfield.h"
#include "ncsutility.h"
#include "ncsheadercontainer.h"
#include "FSEmail.pan"


CNcsAttachmentField* CNcsAttachmentField::NewL(
    TInt aLabelTextId,
    MNcsFieldSizeObserver* aSizeObserver,
    CNcsHeaderContainer* aParentControl )
    {
    FUNC_LOG;
    CNcsAttachmentField* self =
        new ( ELeave ) CNcsAttachmentField( aLabelTextId, aSizeObserver, aParentControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CNcsAttachmentField::~CNcsAttachmentField()
    {
    FUNC_LOG;
    iAttachmentLabels.ResetAndDestroy();

    delete iAttachmentNames;
    delete iAttachmentSizes;

    delete iAttachmentBitmap;
    delete iAttachmentMask;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::CNcsAttachmentField()
// -----------------------------------------------------------------------------
//
CNcsAttachmentField::CNcsAttachmentField( TInt aLabelTextId, 
        MNcsFieldSizeObserver* aSizeObserver,
        CNcsHeaderContainer* aParentControl ):
    MNcsControl( aSizeObserver ),
    iParentControl( aParentControl ),
    iLabelTextId ( aLabelTextId ),
    iFocusedLabelIndex( KNoAttachmentLabelFocused )
    {
    FUNC_LOG;
    }

void CNcsAttachmentField::ConstructL()
    {
    FUNC_LOG;	
	CreateIconsL();
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

// -----------------------------------------------------------------------------
// CNcsAttachmentField::Reposition()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::Reposition(TPoint& aPt, TInt /*aWidth*/ )
    {
    FUNC_LOG;
    SetPosition( aPt );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::GetLabelText()
// -----------------------------------------------------------------------------
//
const TDesC& CNcsAttachmentField::GetLabelText() const
    {
    FUNC_LOG;
	return KNullDesC;
    }

// ---------------------------------------------------------------------------
// CNcsAttachmentField::LayoutLineCount
// ---------------------------------------------------------------------------
//
TInt CNcsAttachmentField::LayoutLineCount() const
    {
    FUNC_LOG;
    return ( IsVisible() ? iAttachmentLabelCount : 0 );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::Draw()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::Draw( const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
    CWindowGc& gc = SystemGc();

    // Draw text box
    if ( IsFocused() )
    	{
    	TBool highlighedFound( EFalse );
    	for ( TInt i( 0 ); !highlighedFound && i<iAttachmentLabelCount; ++i )
    	    {
    	    if ( iAttachmentLabels[i]->IsFocused() )
    	        {
    	        highlighedFound = ETrue;
    	        // highlight for label when focused
    	        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    	        TRgb imageColor;
    	        if( KErrNone != AknsUtils::GetCachedColor( 
                        skin, imageColor,
                        KAknsIIDFsHighlightColors, 
                        EAknsCIFsHighlightColorsCG4 ) &&
                    KErrNone != AknsUtils::GetCachedColor(
                        skin, imageColor,
                        KAknsIIDQsnHighlightColors,
                        EAknsCIQsnHighlightColorsCG2 ) )
    	            {
    	            imageColor = KRgbBlue;
    	            }
	    
    	        gc.SetPenStyle( CGraphicsContext::ENullPen );
    	        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    	        gc.SetBrushColor( imageColor );

    	        TRect highlightRect( 
    	                iAttachmentLabels[i]->TextHitAreaRect() );
    	        highlightRect.Grow( 0, 1 );
    	        gc.DrawRect( highlightRect );
    	        }
    	    }
    	}
    
    // Draw icons
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    gc.BitBltMasked( iAttachmentIconPos, iAttachmentBitmap, 
    		         iAttachmentBitmap->SizeInPixels(), 
    		         iAttachmentMask, ETrue );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SizeChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SizeChanged()
    {
    FUNC_LOG;
    LayoutControls();
	UpdateFontSize();
	UpdateColors();
    // The attachment texts needs to be updated when label size changes because it
	// may now get truncated differently.
	TRAP_IGNORE( UpdateAttachmentTextsL() );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::PositionChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::PositionChanged()
    {
    FUNC_LOG;
    LayoutControls();
    UpdateColors();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::MinimumHeight()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::MinimumHeight() const
    {
    FUNC_LOG;
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
// CNcsAttachmentField::SetContainerWindowL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetContainerWindowL( const CCoeControl& aContainer )
	{
    FUNC_LOG;
	CCoeControl::SetContainerWindowL( aContainer );

	UpdateComponentArrayL();
	}

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateComponentArrayL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateComponentArrayL()
    {
    FUNC_LOG;
    InitComponentArrayL();
    CCoeControlArray& controls = Components();
    TInt controlsCount( controls.Count() );
    controls.SetControlsOwnedExternally( ETrue );
    
    // Create the new component array items if needed
    if ( iAttachmentLabelCount > controlsCount )
        {
        // append controls for new attachment labels after the existing ones
        for ( TInt i( controlsCount ); i<iAttachmentLabelCount; ++i )
            {
            controls.AppendLC( iAttachmentLabels[i] );
            CleanupStack::Pop( iAttachmentLabels[i] );
        
            UpdateFontSize( iAttachmentLabels[i] );
        
            // Align according to mirrored layout variation
            iAttachmentLabels[i]->SetAlignment( AknLayoutUtils::LayoutMirrored() ?
                    EHRightVCenter : EHLeftVCenter );
            }
        }
    // Remove unnecessary component array items from the end
    else if ( iAttachmentLabelCount < controlsCount )
        {
        for ( TInt i( controlsCount-1 ); i>=iAttachmentLabelCount ; --i )
            {
            controls.Remove( iAttachmentLabels[i] );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::SetTextsLD()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::SetTextsLD( 
    CDesCArray* aAttachmentNames, CDesCArray* aAttachmentSizes )
    {
    FUNC_LOG;
    delete iAttachmentNames; 
    iAttachmentNames = NULL;
    iAttachmentNames = aAttachmentNames;
    delete iAttachmentSizes;
    iAttachmentSizes = NULL;
    iAttachmentSizes = aAttachmentSizes;

    if ( iAttachmentNames )
        {
        __ASSERT_DEBUG( iAttachmentNames && 
                        iAttachmentSizes && 
                        iAttachmentNames->Count() == iAttachmentSizes->Count(), 
                        Panic( ENcsBasicUi ) );
 
        TInt attachmentLabelCountBeforeUpdate( iAttachmentLabelCount );
    
        UpdateAttachmentTextsL();
    
        // set the focused attachment label
        if ( iAttachmentLabelCount == 0 )
            {
            // No attachments
            iFocusedLabelIndex = KNoAttachmentLabelFocused;
            }
        else if ( iFocusedLabelIndex >= iAttachmentLabelCount ||
                  attachmentLabelCountBeforeUpdate < iAttachmentLabelCount )
            {
            // Set the focused index to last, we get here either if the last
            // attachment label in list was removed or a new one was added.
            // In either case focused should be the new last attachment label.
            iFocusedLabelIndex = iAttachmentLabelCount-1;
            }
        // In any other case, don't re-set the focused label index
        }
    else // no attachments 
        {
        iAttachmentLabelCount = 0;
        iFocusedLabelIndex = KNoAttachmentLabelFocused;
        }
    
    UpdateComponentArrayL();
    LayoutControls();
    iSizeObserver->UpdateFieldSizeL();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::FocusedAttachmentLabelIndex()
// -----------------------------------------------------------------------------
//
TInt CNcsAttachmentField::FocusedAttachmentLabelIndex()
    {
    FUNC_LOG;
    return iFocusedLabelIndex;
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateAttachmentTextsL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateAttachmentTextsL()
    {
    FUNC_LOG;
    // create new array of labels. 
    iAttachmentLabelCount = 0;
    TInt count = iAttachmentNames->MdcaCount();
    TInt existingLabelCount = iAttachmentLabels.Count();
    TInt i( 0 );    
    
    // First, set the new texts using existing attachment labels objects
    for ( ; i<count; ++i )
        {
        if ( i==existingLabelCount )
            {
            // no more re-usable label objects in their array.. break this 
            // loop and continue with the next one (that creates new labels)
            break;
            }
        UpdateSingleAttachmentLabelTextL( iAttachmentLabels[i], i );
        ++iAttachmentLabelCount;
        }
    
    // Create new attachment labels if needed
    if ( i<count )
        {
        // stringloader used to get the initial (dummy) attachment text..
        HBufC* textBuf = StringLoader::LoadLC( iLabelTextId );
        
        for ( ; i<count; ++i )
            {
            // create and initialize (fonts & colors) new label
            CNcsLabel* label = new ( ELeave ) CNcsLabel( *this, NULL );
            CleanupStack::PushL( label );
            label->SetTextL( *textBuf );
            UpdateFontSize( label );
            UpdateColors( label );
            UpdateSingleAttachmentLabelTextL( label, i );
     
            // append then new label to attachment label array
            iAttachmentLabels.AppendL( label );
            CleanupStack::Pop( label );
            ++iAttachmentLabelCount;
            }
        CleanupStack::PopAndDestroy( textBuf );
        }
    }


// -----------------------------------------------------------------------------
// CNcsAttachmentField::ResizeIcons()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::ResizeIcons()
    {
    FUNC_LOG;
    const TSize frontIconSize( NcsUtility::HeaderDetailIconRect( 
            Rect(), 1, NcsUtility::EIconFront ).Size() );
    if ( frontIconSize != iAttachmentBitmap->SizeInPixels() )
        {
        AknIconUtils::SetSize( iAttachmentBitmap, frontIconSize, 
                               EAspectRatioPreservedAndUnusedSpaceRemoved );
        AknIconUtils::SetSize( iAttachmentMask, frontIconSize, 
                               EAspectRatioPreservedAndUnusedSpaceRemoved );
        }
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::FocusChanged()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::FocusChanged( TDrawNow aDrawNow )
	{
    FUNC_LOG;
	if ( IsFocused() )
	    {
	    for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
	        {
	        iAttachmentLabels[i]->SetFocus( i==iFocusedLabelIndex );
	        }
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
		for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
		    {
		    iAttachmentLabels[i]->SetFocus( EFalse );
		    }
		}
	
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
    FUNC_LOG;
    for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
        {
        UpdateColors( iAttachmentLabels[i] );
        // currently all attachments are underlined
        iAttachmentLabels[i]->SetUnderlining( ETrue );
        }
    
    CFreestyleEmailUiAppUi* appUi =
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    iBorderColor = appUi->LayoutHandler()->ComposerFieldBorderColor();
    iBgColor = appUi->LayoutHandler()->ComposerFieldBackgroundColor();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateColors()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateColors( CNcsLabel* aLabel )
    {
    FUNC_LOG;    
    if ( aLabel )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        TRgb labelTextColor = KRgbBlack;
        TRgb labelEmphasisColor = KRgbBlack;
        TRgb labelHighlightFullEmphasisColor = KRgbBlack;
    
        AknsUtils::GetCachedColor(
            skin,
            labelTextColor,
            KAknsIIDQsnTextColors,
            aLabel->IsFocused() ? 
                EAknsCIQsnTextColorsCG24 : EAknsCIQsnTextColorsCG6 );
        AknsUtils::GetCachedColor(
            skin,
            labelEmphasisColor,
            KAknsIIDQsnHighlightColors,
            EAknsCIQsnHighlightColorsCG3 );
        AknsUtils::GetCachedColor(
            skin,
            labelHighlightFullEmphasisColor,
            KAknsIIDQsnHighlightColors,
            EAknsCIQsnHighlightColorsCG2 );
        
        TRAP_IGNORE( aLabel->OverrideColorL( 
            EColorLabelText, labelTextColor ) );
        TRAP_IGNORE( aLabel->OverrideColorL(
            EColorLabelTextEmphasis, labelEmphasisColor ) );
        TRAP_IGNORE( aLabel->OverrideColorL(
            EColorLabelHighlightFullEmphasis, 
            labelHighlightFullEmphasisColor ) );
        }
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateFontSize()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateFontSize()
	{
    FUNC_LOG;
    if ( iAttachmentLabelCount )
        {
        // same font used for all labels, so just get the first one's
        iEditorFont = NcsUtility::GetLayoutFont( 
            iAttachmentLabels[0]->Rect(), NcsUtility::ENcsHeaderDetailFont );            
        }
    for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
        {
        iAttachmentLabels[i]->SetFont( iEditorFont );
        }
	}

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateFontSize()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateFontSize( CNcsLabel* aLabel )
    {
    FUNC_LOG;
    iEditorFont = NcsUtility::GetLayoutFont( 
            aLabel->Rect(), NcsUtility::ENcsHeaderDetailFont );
    aLabel->SetFont( iEditorFont );
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::LayoutControls()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::LayoutControls()
    {
    FUNC_LOG;
    const TRect rect( Rect() );
    for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
        {
        NcsUtility::LayoutDetailLabel( iAttachmentLabels[i], rect, i );
        if ( i == 0 )
            {
            // icons need to be updated for first round only
            UpdateIconPositions( rect );
            }
        }
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateIconPositions()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateIconPositions( const TRect& aRect )
    {
    FUNC_LOG;
    iAttachmentIconPos = NcsUtility::HeaderDetailIconRect( 
            aRect, 1, NcsUtility::EIconFront ).iTl;
    }

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
        CreateIcons();
        DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::HandlePointerEventL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::HandlePointerEventL( 
        const TPointerEvent& aPointerEvent )
    {
    FUNC_LOG;
    CCoeControl::HandlePointerEventL( aPointerEvent );
    
    if ( Rect().Contains( aPointerEvent.iPosition ) &&
         aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        TBool focusedAttachmentLabelIndexChanged( EFalse );
        TInt i( 0 );
        for ( ; i<iAttachmentLabelCount; ++i )
            {
            if ( iAttachmentLabels[i]->TextHitAreaRect().Contains( 
                    aPointerEvent.iPosition ) )
                {
                // tactile feedback
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( this, ETouchFeedbackBasic );
                    }
                
                if ( iFocusedLabelIndex != i )
                    {
                    focusedAttachmentLabelIndexChanged = ETrue;
                    iFocusedLabelIndex =  i;
                    }
                break;
                }
            }

        if ( i == iAttachmentLabelCount )
            {
            // Pointer event happened outside any of the labels. 
            // Invalidate focused label index.. this will cause the next
            // for loop to set all label focuses to EFalse.
            iFocusedLabelIndex = KNoAttachmentLabelFocused;
            focusedAttachmentLabelIndexChanged = ETrue;
            }

        if ( focusedAttachmentLabelIndexChanged )
            {
            // only redraw if focused attachment label changed
            for ( TInt i( 0 ); i<iAttachmentLabelCount; ++i )
                {
                iAttachmentLabels[i]->SetFocus( i==iFocusedLabelIndex );
                }
            UpdateColors();
            DrawNow();
            }
        }
    }

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
    CFreestyleEmailUiAppUi* fsAppUi = 
        static_cast<CFreestyleEmailUiAppUi*>( ControlEnv()->AppUi() );
    delete iAttachmentBitmap; 
    iAttachmentBitmap = NULL;
    delete iAttachmentMask; 
    iAttachmentMask = NULL;
    fsAppUi->FsTextureManager()->ProvideBitmapL( 
            EAttachmentIcon, iAttachmentBitmap, iAttachmentMask );
    ResizeIcons();
    }

// -----------------------------------------------------------------------------
// CNcsAttachmentField::UpdateSingleAttachmentLabelTextL()
// -----------------------------------------------------------------------------
//
void CNcsAttachmentField::UpdateSingleAttachmentLabelTextL( 
        CNcsLabel* aLabel, TInt aIndex )
    {
    FUNC_LOG;  
    // create decorated size text including the preceeding space character
    HBufC* decoratedSize = StringLoader::LoadLC( 
        R_FSE_VIEWER_ATTACHMENTS_LIST_SIZE, 
        iAttachmentSizes->MdcaPoint( aIndex ) );
    HBufC* decoratedSizeWithSpace = HBufC::NewL( 
        decoratedSize->Length() + 
        KSpace().Length() +
        KBidiRleStartChar().Length() +
        KBidiPdfChar().Length() );
    decoratedSizeWithSpace->Des().Append( KSpace );
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        // Put size part into embedded right-to-left block to ensure 
        // correct rendering even when name part is written from 
        // left-to-right.
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
    
    // clip attachment name to fit the field so that size information has
    // enough space too.
    const CFont& font = *aLabel->Font();

    TInt labelWidth = aLabel->Size().iWidth;
    TInt sizeTextInPixels = 
        font.TextWidthInPixels( *decoratedSizeWithSpace );
    TInt pixelsLeftForAttachmentName = labelWidth - sizeTextInPixels;

    HBufC* attachmentText = NULL; // will contain the complete text
    // safe check if the size information is too wide for some reason.
    if ( pixelsLeftForAttachmentName < 0 )
        {
        attachmentText = HBufC::NewLC( 
                iAttachmentNames->MdcaPoint( aIndex ).Length() 
                + decoratedSizeWithSpace->Length() );
        attachmentText->Des().Append( iAttachmentNames->MdcaPoint( aIndex ) );
        attachmentText->Des().Append( *decoratedSizeWithSpace );
        TPtr attachmentTextBufferModDes = attachmentText->Des();
        AknTextUtils::ClipToFit( attachmentTextBufferModDes, 
                                 font, labelWidth );
        }
    else
        { // normal case
        HBufC* attacmentNameBuffer = 
            iAttachmentNames->MdcaPoint( aIndex ).AllocLC();
        TPtr attachmentNameBufferModDes = attacmentNameBuffer->Des();
        AknTextUtils::ClipToFit( attachmentNameBufferModDes, 
                                 font, pixelsLeftForAttachmentName );

        attachmentText = HBufC::NewL( 
                attacmentNameBuffer->Length() 
                + decoratedSizeWithSpace->Length() );
        attachmentText->Des().Append( *attacmentNameBuffer );
        attachmentText->Des().Append( *decoratedSizeWithSpace );
        CleanupStack::PopAndDestroy( attacmentNameBuffer );
        CleanupStack::PushL( attachmentText );
        }
    aLabel->SetTextL( *attachmentText );
    CleanupStack::PopAndDestroy( attachmentText );
    CleanupStack::PopAndDestroy( decoratedSizeWithSpace );
    }

