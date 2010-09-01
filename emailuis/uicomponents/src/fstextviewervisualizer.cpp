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
* Description:  Implementation of class CFsTextViewerVisualizer
*
*/


//<cmail>
//#include <fsconfig.h>
#include "emailtrace.h"
#include <AknUtils.h>
#include <alf/alfcontrol.h>
#include <alf/alftextvisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimagebrush.h>
#include <alf/alftextstyle.h>
#include <alf/alfborderbrush.h>
#include <alf/alfvisual.h>
#include <alf/alfdecklayout.h>
#include <alf/alfdisplay.h>
#include <alf/alfanchorlayout.h>
#include <alf/alfbrusharray.h>
#include <alf/alfgradientbrush.h>
// </cmail>
#include <w32std.h>
#include <biditext.h>
//start screen saver
#include <e32property.h>
#include <ScreensaverInternalPSKeys.h>
//end screen saver
#include "fsscrollbarset.h"
#include "fsalfscrollbarlayout.h"
#include "fstextviewercoordinator.h"
#include "fstextviewervisualizer.h"
#include "fstextviewercontrol.h"
#include "fstextparser.h"
#include "fsrichtext.h"
#include "fsrichtexttext.h"
#include "fsrichtextsmiley.h"
#include "fsrichtextpicture.h"
#include "fssmileydictionary.h"
#include "fssmileyparser.h"
#include "fstexture.h"
#include "fstextstylemanager.h"
#include "fstextviewervisualizerdata.h"
#include "fstextviewerselectsmanager.h"
#include "fstextviewermarkfacility.h"
#include "fstextviewerskindata.h"
#include "fstextviewerwatermark.h"
#include "fstextviewervisualizer.h"
#include "fstextviewervisualizersettings.h"
#include "fstextureloader.h"
#include "fstextviewerlayouter.h"
#include "fslayoutmanager.h"

const TInt KMinLettersInNewLine = 3;

// ---------------------------------------------------------------------------
// TFsTextureObserver::FsTextureEvent
// ---------------------------------------------------------------------------
void TFsTextureObserver::FsTextureEvent( 
        const TFsTextureEvent& /*aEventType*/,
        CFsTexture& /*aTexture*/)
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerVisualizer* CFsTextViewerVisualizer::NewL( 
        CFsTextViewerControl& aControl,
        CFsRichText* aRichText,
        CFsSmileyDictionary* aSmDict,
        CFsTextViewerVisualizerSettings* aSettings )
    {
    FUNC_LOG;
    CFsTextViewerVisualizer* self = 
        new (ELeave) CFsTextViewerVisualizer( aSettings );
    CleanupStack::PushL( self );
    self->ConstructL( aControl, aRichText, aSmDict );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerVisualizer
// ---------------------------------------------------------------------------
CFsTextViewerVisualizer::~CFsTextViewerVisualizer()
    {
    FUNC_LOG;
    delete iBorder;
    iBorder = NULL;

    delete iParser;
    iParser = NULL;

    delete iTextureLoader;
    iTextureLoader = NULL;

    delete iHelper;
    iHelper = NULL;
    
    delete iLayouter;
    iLayouter = NULL;

    delete iNavigator;
    iNavigator = NULL;

    delete iSelector;
    iSelector = NULL;

    delete iMarkFacility;
    iMarkFacility = NULL;

    delete iTextStyleManager;
    iTextStyleManager = NULL;

    delete iSkinData;
    iSkinData = NULL;

    delete iBackgroundBrush;
    iBackgroundBrush = NULL;
    
    delete iScrollbar;
    iScrollbar = NULL;
    }

// ---------------------------------------------------------------------------
// ScrollUpL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ScrollUpL( TInt aOffset /*=EUseDefaultScrollOffset*/, TInt aTime )
    {
    FUNC_LOG;
    
    iRootLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    TInt effectiveOffset = iLineOffset;
    if ( aOffset > EUseDefaultScrollOffset )
        {
        iLineOffset = aOffset;
        }
    else 
        {
        if ( iLineOffset > iScrollingOffset )
            {
            iLineOffset -= iScrollingOffset;
            }
        else
            {
            iLineOffset = 0;
            }
        }
    effectiveOffset -= iLineOffset;
    iEffectiveOffset.SetTarget( iLineOffset, aTime != EUseDefaultScrollTime 
            ? aTime 
            : iScrollingDelay );
    iNavigator->ScrollAllL( effectiveOffset, aTime != EUseDefaultScrollTime 
            ? aTime 
            : iScrollingDelay, iContentLayout );
    
    if ( iBackgroundLayout )
        {
        TAlfTimedPoint offset;
        offset.iY.SetTarget( iLineOffset, aTime != EUseDefaultScrollTime 
                ? aTime 
                : iScrollingDelay );
        iBackgroundLayout->SetScrollOffset( offset );
        }
    
    if ( iMarkingLayout )
            {
            for ( TInt i = 0; i < iMarkingLayout->Count(); ++i )
                {
                iMarkingLayout->Visual( i ).
                    Move( TPoint( 0, effectiveOffset ), aTime > EUseDefaultScrollTime 
                            ? aTime 
                            : iScrollingDelay );
                }
            }
    if ( iCursor )
        {
        iCursor->Move( TPoint( 0, effectiveOffset ), iScrollingDelay );
        }
    
    iRootLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    
    UpdateScrollbarL();
    }

// ---------------------------------------------------------------------------
// ScrollDownL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ScrollDownL( TInt aOffset /*=EUseDefaultScrollOffset*/, TInt aTime )
    {
    FUNC_LOG;

    iRootLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    TAlfRealRect layoutSize = iContentLayout->DisplayRect();
    TInt effectiveOffset = iLineOffset;
	TInt maxLineOffset = iTotalHeight - layoutSize.iBr.iY;
	
    // If offset given, use it. Otherwise scroll one page down.
    if ( aOffset > EUseDefaultScrollOffset )
        {
        iLineOffset = aOffset;
        }
    else 
        {
        if ( iLineOffset + iScrollingOffset <= maxLineOffset )
            {
            iLineOffset += iScrollingOffset;
            }
        else
            {
            iLineOffset = maxLineOffset;
            }
        }
    effectiveOffset -= iLineOffset;

    iNavigator->ScrollAllL( 
            effectiveOffset, aTime > EUseDefaultScrollTime 
            ? aTime 
            : iScrollingDelay, iContentLayout );
    iEffectiveOffset.SetTarget( iLineOffset, aTime > EUseDefaultScrollTime 
            ? aTime 
            : iScrollingDelay );
    if ( iBackgroundLayout )
        {
//            TAlfTimedPoint offset( iBackgroundLayout->ScrollOffset() );
        TAlfTimedPoint offset;
        offset.iY.SetTarget( iLineOffset, aTime > EUseDefaultScrollTime 
                ? aTime 
                : iScrollingDelay );
        iBackgroundLayout->SetScrollOffset( offset );
        }
        
    if ( iMarkingLayout )
        {
        for ( TInt i = 0; i < iMarkingLayout->Count(); ++i )
            {
            iMarkingLayout->Visual( i ).
                        Move( TPoint( 0, effectiveOffset ), aTime > 
                        EUseDefaultScrollTime ? 
                        aTime : iScrollingDelay );
            }
        }
    if ( iCursor )
        {
        iCursor->Move( TPoint( 0, effectiveOffset ), iScrollingDelay );
        }
    UpdateScrollbarL();
    
    iRootLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    }

// ---------------------------------------------------------------------------
// PageUpL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::PageUpL( TInt aTime )
    {
    FUNC_LOG;
    ScrollUpL( iLineOffset - iContentSize.iHeight, aTime );
    }

// ---------------------------------------------------------------------------
// PageDownL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::PageDownL( TInt aTime )
    {
    FUNC_LOG;
    if ( iLineOffset + iContentSize.iHeight 
            > iTotalHeight - iContentSize.iHeight )
        {
        ScrollDownL( iTotalHeight - iContentSize.iHeight, aTime );
        }
    else 
        {
        ScrollDownL( iLineOffset + iContentSize.iHeight, aTime );
        }
    }

// ---------------------------------------------------------------------------
// NextVisibleHotspot
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizer::NextVisibleHotspotL( TBool& aChanged )
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    retVal = iSelector->MoveNextL( aChanged );

    return retVal;
    }

// ---------------------------------------------------------------------------
// PrevVisibleHotspot
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizer::PrevVisibleHotspotL( TBool& aChanged )
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    retVal = iSelector->MovePrevL( aChanged );

    return retVal;
    }

// ---------------------------------------------------------------------------
// MoveLeft
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::MoveLeftL()
    {
    FUNC_LOG;
    iNavigator->MoveToPrevCharL();

    DrawCursorL( iCursorDelay );
    }

// ---------------------------------------------------------------------------
// MoveRight
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::MoveRightL()
    {
    FUNC_LOG;
    iNavigator->MoveToNextCharL();

    DrawCursorL( iCursorDelay );
    }

// ---------------------------------------------------------------------------
// MoveDownL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::MoveDownL()
    {
    FUNC_LOG;
    if ( iNavigator->GetCursorLine() == iTotalLines )
        {
        return;
        }
    TBool goOn = ETrue;

    TInt currentX = iNavigator->GetCursorRectL().iTl.iX;
    TInt currentY = iNavigator->GetCursorRectL().iTl.iY;

    while ( goOn )
        {
        if ( !iNavigator->MoveToNextCharL() )
            {
            goOn = EFalse;
            }
        DrawMarkL();
        if ( iNavigator->GetCursorRectL().iTl.iX <= currentX )
            {
            goOn = EFalse;
            }
        }

    TInt currentX2 = iNavigator->GetCursorRectL().iTl.iX;
    currentY = iNavigator->GetCursorRectL().iTl.iY;

    goOn = ETrue;
    while ( goOn )
        {
        currentX2 = iNavigator->GetCursorRectL().iTl.iX;

        if ( !iNavigator->MoveToNextCharL() )
            {
            goOn = EFalse;
            }

        DrawMarkL();
        
        if ( iNavigator->GetCursorRectL().iTl.iX >= currentX || 
                    iNavigator->GetCursorRectL().iTl.iX <= currentX2 )
            {
            iNavigator->MoveToPrevCharL();
            DrawMarkL();
            goOn = EFalse;
            }
        }
    
    DrawCursorL( iCursorDelay );
    }

// ---------------------------------------------------------------------------
// MoveUpL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::MoveUpL()
    {
    FUNC_LOG;
    if ( iNavigator->GetCursorLine() == 1 )
        {
        return;
        }
    TBool goOn = ETrue;

    TInt currentX = iNavigator->GetCursorRectL().iTl.iX;
    TInt currentY = iNavigator->GetCursorRectL().iTl.iY;

    while ( goOn )
        {
        if ( !iNavigator->MoveToPrevCharL() )
            {
            goOn = EFalse;
            }
        DrawMarkL();
        if ( iNavigator->GetCursorRectL().iTl.iY < currentY )
            {
            goOn = EFalse;
            }
        }

    TInt currentX2 = iNavigator->GetCursorRectL().iTl.iX;
    currentY = iNavigator->GetCursorRectL().iTl.iY;

    goOn = ETrue;
    while ( goOn )
        {
        currentX2 = iNavigator->GetCursorRectL().iTl.iX;

        if ( !iNavigator->MoveToPrevCharL() )
            {
            goOn = EFalse;
            }
        DrawMarkL();
        if ( iNavigator->GetCursorRectL().iTl.iX <= currentX || 
                            iNavigator->GetCursorRectL().iTl.iX >= currentX2 )
            {
            iNavigator->MoveToNextCharL();
            DrawMarkL();
            goOn = EFalse;
            }
        }
    
    DrawCursorL( iCursorDelay );
    }

// ---------------------------------------------------------------------------
// EnableMark
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::EnableMarkL( TBool aEnabled )
    {
    FUNC_LOG;
    iMark = aEnabled;
    if ( iMark )
        {
        if ( iMarkingLayout )
            {
            iMarkingLayout->RemoveAndDestroyAllD();
            }
        iMarkingLayout = CAlfLayout::AddNewL( *iControl, iRootLayout );
        iMarkFacility->SetLayout( iMarkingLayout );
        iMarkFacility->SetHeadOffsetL( iNavigator->GetCursorCharOffset() );
        }
    }

// ---------------------------------------------------------------------------
// SwitchMark
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SwitchMarkL()
    {
    FUNC_LOG;
    iMark = !iMark;
    if ( iMark )
        {
        if ( iMarkingLayout )
            {
            iMarkingLayout->RemoveAndDestroyAllD();
            }
        iMarkingLayout = CAlfLayout::AddNewL( *iControl, iRootLayout );
        iMarkingLayout->SetSize( iContentSize );
        iMarkFacility->SetLayout( iMarkingLayout );
        iMarkFacility->SetHeadOffsetL( iNavigator->GetCursorCharOffset() );
        }
    }

// ---------------------------------------------------------------------------
// MarkEnabled
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizer::MarkEnabled()
    {
    FUNC_LOG;
    return iMark;
    }

// ---------------------------------------------------------------------------
// ExpandL
// ---------------------------------------------------------------------------
CAlfLayout* CFsTextViewerVisualizer::ExpandL( 
        TInt aId, 
        TInt aStart, 
        CAlfVisual* aVisual )
    {
    FUNC_LOG;
    iContentLayout->SetFlag( EAlfVisualFlagFreezeLayout );
    TInt brY = aVisual->DisplayRectTarget().iBr.iY + iContentLayout->ScrollOffset().IntValueNow().iY;
    TInt posY = aVisual->Pos().Target().iY;
    TInt posX = aVisual->Pos().Target().iX;
    iParser->SetExpandStatusL( aId, ETrue );
    CAlfAnchorLayout* expandLayout = 
        CAlfAnchorLayout::AddNewL( *iControl, iContentLayout );    
    
    expandLayout->SetFlag( EAlfVisualFlagManualLayout );
    expandLayout->SetClipping();
    TInt end = iParser->GetEndIndexOfExpandAreaL( aId );
    TSize layoutSize = LayoutVisualsL( expandLayout, aStart, end );
    iNavigator->UpdateLinesNumbers();
    iTotalLines = iNavigator->GetTotalLines();
    iSelector->DimAllL();

    iTotalHeight += layoutSize.iHeight;

    expandLayout->SetPos( TPoint( 0, brY ) );
    expandLayout->SetSize( TSize( 0, 0 ), 0 );
    expandLayout->SetSize( layoutSize, iSettings->iExpandDelay );

    iNavigator->Sort();
    
    iNavigator->ScrollFromCharL( end, layoutSize.iHeight, iSettings->iExpandDelay, iContentLayout ); //<cmail> leaving

    if ( iNavigator->GetCursorCharOffset() >= end )
        {
        TRect target = iCursor->DisplayRectTarget();
        target.Move( 0, layoutSize.iHeight );
        iCursor->SetRect( target, iSettings->iExpandDelay );
        }

    UpdateScrollbarL();
    TFsVisRange range;
    range.iStart = aStart;
    range.iEnd = end;
    iNavigator->AppendLayoutL( 
            static_cast<CAlfVisual*>( expandLayout ), range );
    iContentLayout->ClearFlag( EAlfVisualFlagFreezeLayout );
    return expandLayout;
    }

// ---------------------------------------------------------------------------
// ExpandExisting
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ExpandExistingL( 
        CAlfLayout* aLayout, 
        TSize aTargetSize, TInt aId )
    {
    FUNC_LOG;
    TSize layoutSize = aTargetSize;
    TInt end = iParser->GetEndIndexOfExpandAreaL( aId );

    aLayout->SetSize( aTargetSize, iSettings->iExpandDelay );

    iNavigator->ScrollFromCharL( end, layoutSize.iHeight, iSettings->iExpandDelay, iContentLayout );  //<cmail> leaving  

    for ( TInt i = 0; i < aLayout->Count(); ++i )
        {
        iNavigator->EnableVisual( &aLayout->Visual( i ) );
        }

    if ( iNavigator->GetCursorCharOffset() >= end )
        {
        TRect target = iCursor->DisplayRectTarget();
        target.Move( 0, layoutSize.iHeight );
        iCursor->SetRect( target, iSettings->iExpandDelay );
        }

    iTotalHeight += layoutSize.iHeight;

    iNavigator->Sort();

    UpdateScrollbarL();

    iNavigator->UpdateLinesNumbers();
    iTotalLines = iNavigator->GetTotalLines();

    RefreshPartBgsL();
    }

// ---------------------------------------------------------------------------
// Collapse
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::CollapseL( CAlfLayout* aLayout, TInt aId )
    {
    FUNC_LOG;
    TRect target = aLayout->DisplayRectTarget();

    TInt start = iParser->GetStartIndexOfExpandAreaL( aId );
    TInt end = iParser->GetEndIndexOfExpandAreaL( aId );
    TInt caption = iParser->GetBodyIndexOfExpandAreaL ( aId );

    TSize layoutSize = TSize( 
               aLayout->Size().Target().iX,
               aLayout->Size().Target().iY );
    
    aLayout->SetSize( TSize( 0, 0 ) );

    iNavigator->ScrollFromCharL( end, layoutSize.iHeight * -1, iSettings->iExpandDelay, iContentLayout );//<cmail> leaving

    for ( TInt i = 0; i < aLayout->Count(); ++i )
        {
        iNavigator->EnableVisual( &aLayout->Visual( i ), EFalse );
        }

    if ( iNavigator->GetCursorCharOffset() >= start 
            && iNavigator->GetCursorCharOffset() <= end )
        {
        iNavigator->MoveToPrevCharL();
        iNavigator->MoveToNextCharL();
        DrawCursorL( iSettings->iExpandDelay );
        }
    else if ( iNavigator->GetCursorCharOffset() > end )
        {
        target = iCursor->DisplayRectTarget();
        target.Move( 0, -1 * layoutSize.iHeight );
        iCursor->SetRect( target, iSettings->iExpandDelay );
        }

    iTotalHeight -= layoutSize.iHeight;

    iNavigator->Sort();

    RefreshPartBgsL();

    iNavigator->UpdateLinesNumbers();
    iTotalLines = iNavigator->GetTotalLines();

    UpdateScrollbarL();
    }

// ---------------------------------------------------------------------------
// LayoutVisualsL
// ---------------------------------------------------------------------------
TSize CFsTextViewerVisualizer::LayoutVisualsL( 
        CAlfLayout* aLayout, 
        TInt aStart, 
        TInt aEnd )
    {
    FUNC_LOG;
    TInt backupLineOffset = iLayouter->GetCurrentLineOffset();
    iLayouter->SetCurrentLineOffset( 0 );
    iParser->SetParsingPosition( aStart + 10 );
    TInt currentEnd = 0;
    
    TBidiText::TDirectionality direction;
    TBidiText::TDirectionality lastDirection;
    
    if ( iSettings->iMirroring )
        {
        direction = TBidiText::ERightToLeft;
        }
    else 
        {
        direction = TBidiText::ELeftToRight;
        }
    lastDirection = direction;
    
    MFsRichTextObject* object = NULL;
    iLayouter->ConstructLineL( 1 );
    while ( iParser->IsNextObject() && currentEnd <= aEnd )
        {
        if ( object )
            {
            delete object;
            object = NULL;
            }
        object = iParser->GetNextObjectL();
        currentEnd = object->GetEndOfObject();
        if ( object->GetType() == MFsRichTextObject::EFsNewLine )
            {
            TCharFormat charFormat;
            TCharFormatMask charFormatMask;
            iCFsRichText->GetSpecificCharFormat( charFormat, charFormatMask, object->GetBeginOfObject() );
            TInt styleId;
            styleId = iTextStyleManager->GetStyleIDL( charFormat );
            CFont* font;
            User::LeaveIfNull( iScreenDev );
            CAlfTextStyle* textStyle(
                iEnv->TextStyleManager().TextStyle( styleId ) );

            HBufC* typeface( textStyle->TypefaceNameL() );
            CleanupStack::PushL( typeface );
            TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

            User::LeaveIfError(
                iScreenDev->GetNearestFontInTwips( font, fontSpec ) );
            TInt height;
            height = font->HeightInPixels();
            iScreenDev->ReleaseFont( font );
            CleanupStack::PopAndDestroy( typeface );
            
            iLayouter->ConstructLineL( height );
            lastDirection = direction;
            }
        else
            {
            if ( ( object->GetType() == MFsRichTextObject::EFsText
                    || object->GetType() == MFsRichTextObject::EFsSmiley )
                    && static_cast<CFsRichTextText*>(object)->GetTextDirection() != direction 
                    && lastDirection != direction )
                {
                iLayouter->InsertObjectL( object, aLayout, iParser, EFalse );
                }
            else 
                {
                iLayouter->AppendObjectL( object, aLayout, iParser, EFalse );
                }
            if ( object->GetType() == MFsRichTextObject::EFsText )
                {
                lastDirection = static_cast<CFsRichTextText*>(object)->GetTextDirection();
                }
            }
        }
    
    iLayouter->ConstructLineL();
    
    if ( object )
        {
        delete object;
        object = NULL;
        }
    
    iNavigator->ScrollAllL( 0, 0, iContentLayout );
    
    TSize retVal;
    retVal = TSize( iContentLayout->Size().Target().iX, iLayouter->GetCurrentLineOffset() );
    iLayouter->SetCurrentLineOffset( backupLineOffset );
    return retVal;
    }

// ---------------------------------------------------------------------------
// Navigator
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerData* CFsTextViewerVisualizer::Navigator()
    {
    FUNC_LOG;
    return iNavigator;
    }

// ---------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------
CFsTextParser* CFsTextViewerVisualizer::Parser()
    {
    FUNC_LOG;
    return iParser;
    }

// ---------------------------------------------------------------------------
// Clicked
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ClickedL()
    {
    FUNC_LOG;
    iSelector->ClickedL();
    }

TBool CFsTextViewerVisualizer::SmileysEnabled()
    {
    FUNC_LOG;
    return iSmileyEnabled;
    }

// ---------------------------------------------------------------------------
// IsHotspotHighlighted
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizer::IsHotspotHighlighted()
    {
    FUNC_LOG;
    return iSelector->IsHotspotHighlighted();
    }

// ---------------------------------------------------------------------------
// EnableSmileys
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::EnableSmileys( TBool aStatus )
    {
    FUNC_LOG;
    iSmileyEnabled = aStatus;
    }

// ---------------------------------------------------------------------------
// SetScrollOffsetInPx
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetScrollOffsetInPx( TInt aPxOffset )
    {
    FUNC_LOG;
    iScrollingOffset = aPxOffset;
    }

// ---------------------------------------------------------------------------
// SetScrollOffsetInLines
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetScrollOffsetInLines( TInt aLineOffset )
    {
    FUNC_LOG;
    TInt lineHeight = iControl->Env().TextStyleManager().TextStyle( 
                                    EAlfTextStyleNormal )->TextSizeInPixels();
    iScrollingOffset = aLineOffset * lineHeight;
    }

// ---------------------------------------------------------------------------
// SetSmileySize
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetSmileySize( TSize aSize )
    {
    FUNC_LOG;
    iParser->SetSizeOfSmile( aSize );
    }

// ---------------------------------------------------------------------------
// SetScrollDelay
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetScrollDelay( TInt aDelay )
    {
    FUNC_LOG;
    iScrollingDelay = aDelay;
    }

// ---------------------------------------------------------------------------
// SetCursorDelay
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetCursorDelay( TInt aDelay )
    {
    FUNC_LOG;
    iCursorDelay = aDelay;
    }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::Reset()
    {
    FUNC_LOG;
    iLineOffset = 0;

    delete iSkinData;
    iSkinData = NULL;
    
    delete iLayouter;
    iLayouter = NULL;
    
    delete iBorder;
    iBorder = NULL;

    delete iHelper;
    iHelper = NULL;

    delete iNavigator;
    iNavigator = NULL;

    delete iSelector;
    iSelector = NULL;

    delete iMarkFacility;
    iMarkFacility = NULL;
    
    delete iScrollbar;
    iScrollbar = NULL;
    }

// ---------------------------------------------------------------------------
// ReloadL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ReloadL()
    {
    FUNC_LOG;
    TInt currentLength = iCFsRichText->DocumentLength();
    
    TInt lastCursorPosition = 0;
    
    if( iCursor )
        {
        lastCursorPosition = iNavigator->GetCursorCharOffset();
        }
    
    if ( iRichTextLength ==  currentLength )
        {
        Reset();
        iParser->ParseTextL();
        iParser->SetParsingPosition( 0 );
        ConstructSelfL( iCFsRichText, iSmileyDict, EFalse );
        ConstructVisualsL();
        iNavigator->Sort();
        iSelector->DimAllL();
        iRichTextLength = currentLength;
        }
    else
        {
        iParser->ParseTextL();
        iParser->SetParsingPosition( iRichTextLength );
        iRichTextLength = currentLength;

        ConstructVisualsL( EFalse );
        iNavigator->UpdateLinesNumbers();
        iNavigator->Sort();
        }
    UpdateScrollbarL();
    
    if ( !iCursor )
        {
        iCursor = CAlfImageVisual::AddNewL( *iControl, iRootLayout );
        iCursor->SetFlag( EAlfVisualFlagManualLayout );
        iCursor->EnableBrushesL();
        iCursor->Brushes()->AppendL( iBorder, EAlfDoesNotHaveOwnership );
        iCursor->SetRect( TRect( 0, 0, 0, 0 ) );
        
        while ( lastCursorPosition > iNavigator->GetCursorCharOffset() )
            {
            iNavigator->MoveToNextCharL();
            }

        DrawCursorL(0);
        }
    }

// ---------------------------------------------------------------------------
// GetMarkedRange
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::GetMarkedRange( TInt& aBegin, TInt& aEnd )
    {
    FUNC_LOG;
    TInt begin = 0;
    TInt end = 0;
    iMarkFacility->GetRange( begin, end );
    aBegin = begin;
    aEnd = end;
    }

// ---------------------------------------------------------------------------
// GetHotspotRange
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::GetHotspotRangeL( TInt& aBegin, TInt& aEnd )
    {
    FUNC_LOG;
    TInt id = iSelector->GetCurrentSelectId();

    if ( 0 <= id )
        {
        aBegin = iParser->GetStartIndexOfHotSpotAreaL( id, ETrue );
        aEnd = iParser->GetEndIndexOfHotSpotAreaL( id, ETrue );
        }
    else
        {
        aBegin = 0;
        aEnd = 0;
        }
    }

// ---------------------------------------------------------------------------
// CalculateSizeL
// ---------------------------------------------------------------------------
TSize CFsTextViewerVisualizer::CalculateSizeL( MFsRichTextObject* aObject )
    {
    FUNC_LOG;
    TSize retVal;

    switch ( aObject->GetType() )
        {
        case MFsRichTextObject::EFsText:
            {
            CFont* font;
            User::LeaveIfNull(iScreenDev);
        
            CAlfTextStyle* textStyle(
                iEnv->TextStyleManager().TextStyle(
                    static_cast<CFsRichTextText*>( aObject )->GetStyleId()
                ) );

            HBufC* typeface( textStyle->TypefaceNameL() );
            CleanupStack::PushL( typeface );
            TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

            User::LeaveIfError(
                iScreenDev->GetNearestFontInTwips( font, fontSpec ) );

            retVal = TSize(
                    font->TextWidthInPixels(
                        static_cast<CFsRichTextText*>(aObject)->GetText() ), 
                    font->FontMaxHeight() );
            iScreenDev->ReleaseFont( font );
            CleanupStack::PopAndDestroy( typeface );
            break;
            }
        case MFsRichTextObject::EFsTexture:
            {
            retVal = static_cast<CFsRichTextPicture*>
                (aObject)->GetTextureSize();
            break;
            }
        case MFsRichTextObject::EFsSmiley:
            {
            if ( iSmileyEnabled )
                {
                retVal = static_cast<CFsRichTextSmiley*>
                     (aObject)->GetPictureObject()->GetTextureSize();
                }
            else
                {
                CFont* font;
                CFsRichTextText* object = 
                    static_cast<CFsRichTextSmiley*>
                     (aObject)->GetTextObject();
                User::LeaveIfNull(iScreenDev);
        
                CAlfTextStyle* textStyle(
                    iEnv->TextStyleManager().TextStyle(
                        object->GetStyleId() ) );

                HBufC* typeface( textStyle->TypefaceNameL() );
                CleanupStack::PushL( typeface );
                TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

                User::LeaveIfError(
                    iScreenDev->GetNearestFontInTwips( font, fontSpec ) );

                retVal = TSize(
                        font->TextWidthInPixels( object->GetText() ), 
                        font->FontMaxHeight() );
                iScreenDev->ReleaseFont( font );
                CleanupStack::PopAndDestroy( typeface );
                break;
                }
            }
        default:
            {
            break;
            }
        }
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// CalculateWidthL
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizer::CalculateWidthL( 
        const TDesC& aText, 
        TInt aTextStyleId )
    {
    FUNC_LOG;
    TInt retVal;

    CFont* font;
    User::LeaveIfNull( iScreenDev );
        
    CAlfTextStyle* textStyle(
        iEnv->TextStyleManager().TextStyle( aTextStyleId ) );
    HBufC* typeface( textStyle->TypefaceNameL() );
    CleanupStack::PushL( typeface );

    User::LeaveIfError(
        iScreenDev->GetNearestFontInTwips(
            font,
            TFontSpec( *typeface, textStyle->TextSizeInTwips() ) ) );

    retVal = font->TextWidthInPixels( aText );

    iScreenDev->ReleaseFont( font );
    CleanupStack::PopAndDestroy( typeface );

    return retVal;
    }

// ---------------------------------------------------------------------------
// SetWatermark
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetWatermarkL( 
        CFsTextViewerWatermark* aWatermark )
    {
    FUNC_LOG;
    iWatermark = aWatermark;
    if ( iRootLayout )
        {
        iRootLayout->SetFlag( EAlfVisualFlagManualLayout );
        iWatermarkVisual = CAlfImageVisual::AddNewL( *iControl, iRootLayout );
        iWatermarkVisual->SetFlag( EAlfVisualFlagManualLayout );
        iWatermarkVisual->SetImage( TAlfImage( iWatermark->Texture() ) );
        iWatermarkVisual->SetScaleMode( CAlfImageVisual::EScaleFit );
        iWatermarkVisual->SetSize( TSize( 
                    iWatermark->Rect().Height(), 
                    iWatermark->Rect().Width() ) );
        iWatermarkVisual->SetPos( iWatermark->Rect().iTl );
        iWatermarkVisual->SetOpacity( iWatermark->Opacity() );
        iContentLayout->MoveToFront();
        }
    }


// ---------------------------------------------------------------------------
// RootLayout
// ---------------------------------------------------------------------------
CAlfLayout* CFsTextViewerVisualizer::RootLayout()
    {
    FUNC_LOG;
    return iRootLayout;
    }

// ---------------------------------------------------------------------------
// SetBackgroundColor
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetBackgroundColorL( const TRgb& aColor )
    {
    FUNC_LOG;
    if ( iBackgroundBrush )
        {
        for ( TInt i = 0; i < iRootLayout->Brushes()->Count(); ++i )
            {
            if ( &iRootLayout->Brushes()->At( i ) == iBackgroundBrush )
                {
                iRootLayout->Brushes()->Remove( i );
                }
            }
        delete iBackgroundBrush;
        iBackgroundBrush = NULL;
        }
    iBackgroundBrush = CAlfGradientBrush::NewL( *iEnv );
    CAlfGradientBrush* brush = 
        static_cast<CAlfGradientBrush*>( iBackgroundBrush );
    brush->SetLayer( EAlfBrushLayerBackground );
    brush->SetColor( aColor );
    brush->SetOpacity( iSettings->iBackgroundOpacity );
    iRootLayout->EnableBrushesL();
    iRootLayout->Brushes()->AppendL( brush, EAlfDoesNotHaveOwnership );
    }

// ---------------------------------------------------------------------------
// SetBackgroundImage
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetBackgroundImageL( CAlfTexture& aTexture )
    {
    FUNC_LOG;
    if ( iBackgroundBrush )
        {
        for ( TInt i = 0; i < iRootLayout->Brushes()->Count(); ++i )
            {
            if ( &iRootLayout->Brushes()->At( i ) == iBackgroundBrush )
                {
                iRootLayout->Brushes()->Remove( i );
                }
            }
        delete iBackgroundBrush;
        iBackgroundBrush = NULL;
        }
    iBackgroundBrush = CAlfImageBrush::NewL( *iEnv, TAlfImage( aTexture ) );
    iBackgroundBrush->SetLayer( EAlfBrushLayerBackground );
    iBackgroundBrush->SetOpacity( iSettings->iBackgroundOpacity );
    iRootLayout->EnableBrushesL();
    iRootLayout->Brushes()->AppendL( 
            iBackgroundBrush, 
            EAlfDoesNotHaveOwnership );
    }

// ---------------------------------------------------------------------------
// SetBackgroundOpacity
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetBackgroundOpacity()
    {
    FUNC_LOG;
    if ( iBackgroundBrush )
        {
        iBackgroundBrush->SetOpacity( iSettings->iBackgroundOpacity );
        }
    }

// ---------------------------------------------------------------------------
// RefreshPartBgs
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::RefreshPartBgsL()
    {
    FUNC_LOG;
    iNavigator->ResetBgArray();
    if ( iIsLayouted )
        {
        RedrawPartBgsL();
        }
    }

// ---------------------------------------------------------------------------
// RedrawPartBgs
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::RedrawPartBgsL()
    {
    FUNC_LOG;
    for ( TInt i = 0; i < iSettings->iPartBgs.Count(); ++i )
            {
            TFsPartBgVisual background;
            background.iRect.iTl = 
                iNavigator->GetVisualPos( iSettings->iPartBgs[i].iStart ).iTl;
            background.iRect.iBr 
                = iNavigator->GetVisualPos( iSettings->iPartBgs[i].iEnd ).iBr;
            background.iRect.iTl.iX = 0;
            background.iRect.iBr.iX = 
                iDisplaySize.iWidth 
                + iSettings->iLeftMargin 
                + iSettings->iRightMargin;
            background.iVisual = STATIC_CAST(CAlfVisual*, iControl->AppendVisualL(EAlfVisualTypeVisual,iBackgroundLayout));
            background.iVisual->EnableBrushesL();
            background.iVisual->Brushes()->AppendL( 
                    iSettings->iPartBgs[i].iBrush, 
                    EAlfDoesNotHaveOwnership );
            background.iVisual->SetRect( background.iRect );
            iNavigator->AppendBgL( background );
            }
    }

// ---------------------------------------------------------------------------
// Control
// ---------------------------------------------------------------------------
CAlfControl* CFsTextViewerVisualizer::Control()
    {
    FUNC_LOG;
    return iControl;
    }

// ---------------------------------------------------------------------------
// StackLayout
// ---------------------------------------------------------------------------
CAlfLayout* CFsTextViewerVisualizer::StackLayout()
    {
    FUNC_LOG;
    return iRootLayout;
    }

// ---------------------------------------------------------------------------
// BackgroundLayout
// ---------------------------------------------------------------------------
CAlfLayout* CFsTextViewerVisualizer::BackgroundLayout()
    {
    FUNC_LOG;
    return iBackgroundLayout;
    }

// ---------------------------------------------------------------------------
// RemoveEndLines
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::RemoveEndLinesL( TInt aLines )
    {
    FUNC_LOG;
    TInt cutCharIndex = iNavigator->RemoveFromLineL( 
            iTotalLines - aLines + 1, 
            iHelper );
    
    iNavigator->Sort(); 
    iNavigator->UpdateLinesNumbers();
    iTotalLines = iNavigator->GetTotalLines();
    iTotalHeight = iNavigator->GetTotalHeight();
    iLayouter->SetPosition( iTotalLines, iTotalHeight );
    UpdateScrollbarL();

    iCFsRichText->DeleteL( 
            cutCharIndex, 
            iCFsRichText->DocumentLength() - cutCharIndex );
    iRichTextLength = iCFsRichText->DocumentLength();

    delete iParser;
    iParser = NULL;

    iParser = CFsTextParser::NewL( 
            *iCFsRichText, 
            iControl->Env(), 
            iTextureLoader );
    iParser->SetSizeOfSmile( TSize( 20,20 ) );
    iParser->SetTextStylemanager(iTextStyleManager);

    iNavigator->SetParser( iParser );
       
    if ( iSmileyDict )
        {
        CFsSmileyParser* smParser = CFsSmileyParser::NewL();
        smParser->SetSmileyDictionary( iSmileyDict );
        iParser->SetSmileyParser(smParser);
        }

    iParser->ParseTextL();

    if ( iCursor )
        {
        DrawCursorL( 0 );
        }
    }

// ---------------------------------------------------------------------------
// ReplaceTextL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ReplaceTextL( 
        CFsRichText* aRichText, 
        CFsSmileyDictionary* aSmDict )
    {
    FUNC_LOG;
    if ( 0 == aRichText->DocumentLength() )
        {
        return;
        }

    TInt lineOffset = iLineOffset;
    
    Reset();
    
    delete iParser;
    iParser = NULL;

    iCFsRichText = aRichText;
    _LIT(oneSpace, " ");
    
    aRichText->InsertL( aRichText->DocumentLength() , oneSpace );
    iLineOffset = 0;
    ConstructSelfL( aRichText, aSmDict ? aSmDict : iSmileyDict );
    iParser->ParseTextL();
    iRichTextLength = aRichText->DocumentLength();
    
    ConstructVisualsL();
    iNavigator->Sort();
    iSelector->DimAllL();
    
    if ( lineOffset > iTotalHeight - iContentSize.iHeight )
        {
        lineOffset = iTotalHeight - iContentSize.iHeight;
        }
    ScrollUpL( lineOffset, 0 );
    }

// ---------------------------------------------------------------------------
// FocusLineL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::FocusLineL( TInt aLine, TBool aShowAsFirstLine )
    {
    FUNC_LOG;

    TInt totalLines = iNavigator->GetTotalLines();
    
    if( aLine > totalLines )
    	{
    	aLine = totalLines;
    	}
    
    TInt firstVisible = iNavigator->GetFirstVisible()->iLineNumber;
    TInt lastVisible = iNavigator->GetLastVisible()->iLineNumber;

    TInt targetLineOffset = iNavigator->GetLineOffset( aLine );

    if ( aShowAsFirstLine && aLine <= lastVisible && aLine >= firstVisible )
        {
        if ( iLineOffset 
                + targetLineOffset  
                > iTotalHeight - iContentSize.iHeight )
            {
            ScrollDownL( iTotalHeight - iContentSize.iHeight, 0 );
            }
        else 
            {
            ScrollDownL( iLineOffset 
                    + targetLineOffset, 
                        0 );
            }
        }
    else if ( aLine == 1 )
        {
        ScrollUpL( 1, 0 );
        }
    else if ( aLine > lastVisible )
        {
        if ( iLineOffset 
                + targetLineOffset  
                > iTotalHeight - iContentSize.iHeight )
            {
            ScrollDownL( iTotalHeight - iContentSize.iHeight, 0 );
            }
        else 
            {
            ScrollDownL( 
                    iLineOffset 
                    + targetLineOffset, 0 );
            }
        }
    else if ( aLine < firstVisible )
        {
        ScrollUpL( 
                iLineOffset 
                + targetLineOffset, 0 );
        }

    iNavigator->FindAndHighlightHotspotL( aLine );
    }

// ---------------------------------------------------------------------------
// UpdateDisplaySize
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::UpdateDisplaySize()
    {
    FUNC_LOG;

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    iDisplaySize = mainPaneRect.Size();

    // Submit iContentLayout's size as display size for external classes, as
    // it's the actual area where viewer content is drawn (excluding e.g.
    // scroll bar area), so that's what should be used in most cases.
    iContentSize = iContentLayout->Size().IntTarget().AsSize();

    // Set helper display size before subtracting the margins, as helper
    // takes account also the margins in it's own calculations
    iHelper->SetSize( iContentSize );
    iHelper->SetLeftMargin( iSettings->iLeftMargin );
    iHelper->SetRightMargin( iSettings->iRightMargin );

    iContentSize.iWidth -= iSettings->iLeftMargin;
    iContentSize.iWidth -= iSettings->iRightMargin;

    iLayouter->SetDisplaySize( iContentSize );
    iLayouter->SetMargins( iSettings->iLeftMargin, iSettings->iRightMargin );
    iNavigator->SetScreenHeight( iContentSize.iHeight );
    }

// ---------------------------------------------------------------------------
// ReloadPicturesL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ReloadPicturesL( TInt aLine )
    {
    FUNC_LOG;
    iParser->ParseTextL();
    iNavigator->ReloadPicturesL( aLine );
    }

// ---------------------------------------------------------------------------
// GetVisibleFocusedHotspotLine
// ---------------------------------------------------------------------------
TBool CFsTextViewerVisualizer::GetVisibleFocusedHotspotLine( 
        TInt& aLineNum ) const
    {
    FUNC_LOG;
    TBool retVal = EFalse;

    if ( iSelector->IsCurrentHotspotVisible() )
        {
        aLineNum = iSelector->GetCurrentHotspotLine() + 1;
        retVal = ETrue;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// SetCurrentHotspotByOrderL
// ---------------------------------------------------------------------------
// <cmail>
void CFsTextViewerVisualizer::SetCurrentHotspotByOrderL( TInt aIndex, TBool aFocusLine )
    {
    FUNC_LOG;
    
    iSelector->SelectByIndexL( aIndex );
    if (aFocusLine)
        {
        FocusLineL(iSelector->GetCurrentHotspotLine(), EFalse);
        iSelector->SelectByIndexL( aIndex );
        }
    }
// </cmail>

// ---------------------------------------------------------------------------
// SetCurrentHotspotByCharL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::SetCurrentHotspotByCharL( TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex >= 0 )
        {
        iSelector->SelectByCharL( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// GetStatusLayout
// ---------------------------------------------------------------------------
CAlfLayout* CFsTextViewerVisualizer::GetStatusLayout()
    {
    FUNC_LOG;
    return iStatusLineLayout;
    }

// ---------------------------------------------------------------------------
// UpdateStatusLayout
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::UpdateStatusLayout()
    {
    FUNC_LOG;
    if ( !iStatusLineLayout )
        {
        // <cmail>
        TRAPD( err, iStatusLineLayout = CAlfAnchorLayout::AddNewL( *iControl, iContentLayout ) ); // Modified
        if ( err != KErrNone ) 
	        {
			return;	 // debug printing of error       	
	        }
        // </cmail>
        iStatusLineLayout->SetClipping( ETrue );
        iStatusLineLayout->SetSize( TSize( 0, 0 ) );
        iStatusLineLayout->SetFlag( EAlfVisualFlagManualLayout );
        }
    iNavigator->SetStatusLineLayout( iStatusLineLayout );
    iTotalHeight = iNavigator->GetTotalHeight();
    // <cmail>
    TRAP_IGNORE( iNavigator->ScrollAllL( 0, 0, iContentLayout ) );
    // </cmail>
    }

// ---------------------------------------------------------------------------
// RemoveStatusLayout
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::RemoveStatusLayout()
    {
    FUNC_LOG;
    iStatusLineLayout->RemoveAndDestroyAllD();
    // <cmail>
    TRAPD( err, iStatusLineLayout = CAlfAnchorLayout::AddNewL( *iControl, iContentLayout ) ); // Modified
    if ( err != KErrNone ) 
        {
		return;	 //debug printing of error       	
        }
    // </cmail>

    iStatusLineLayout->SetClipping( ETrue );
    iStatusLineLayout->SetSize( TSize( 0, 0 ) );
    iStatusLineLayout->SetFlag( EAlfVisualFlagManualLayout );

    iNavigator->SetStatusLineLayout( iStatusLineLayout );
    iTotalHeight = iNavigator->GetTotalHeight();
    // <cmail>
    TRAP_IGNORE( iNavigator->ScrollAllL( 0, 0, iContentLayout ) );
    // </cmail>
    }

// ---------------------------------------------------------------------------
// GetCurrentHotspotTextL
// ---------------------------------------------------------------------------
HBufC* CFsTextViewerVisualizer::GetCurrentHotspotTextL()
    {
    FUNC_LOG;
    HBufC* retVal = NULL;

    TInt id = iSelector->GetCurrentSelectId();
    TInt begin = 0;
    TInt end = 0;

    if ( 0 <= id )
        {
        begin = iParser->GetStartIndexOfHotSpotAreaL( id, EFalse );
        end = iParser->GetEndIndexOfHotSpotAreaL( id, EFalse );
        }

    if ( begin >= 0 && end >= begin  )
        {
        retVal = iParser->GetTextL( begin, end );
        }

    return retVal;
    }

// <cmail>
TInt CFsTextViewerVisualizer::GetCurrentHotspotByOrder()
    {
    return iSelector->GetCurrentSelectedIndex();
    }
// </cmail>

CAknDoubleSpanScrollBar* CFsTextViewerVisualizer::GetScrollBar()
    {
    return iScrollbar;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ConstructL( 
        CFsTextViewerControl& aControl,
        CFsRichText* aRichText,
        CFsSmileyDictionary* aSmDict )
    {
    FUNC_LOG;
    iControl = &aControl;
    iEnv = &iControl->Env();
    
    InitDefaultsL();

    if ( 0 == aRichText->DocumentLength() )
        {
        return;
        }
    iCFsRichText = aRichText;
    
    _LIT(oneSpace, "  ");
    aRichText->InsertL( aRichText->DocumentLength() , oneSpace );

    ConstructSelfL( aRichText, aSmDict );
    iParser->ParseTextL();
    iRichTextLength = aRichText->DocumentLength();
    
    ConstructVisualsL();
    iNavigator->Sort();
    iSelector->DimAllL();
    }

// ---------------------------------------------------------------------------
// CFsTextViewerVisualizer
// ---------------------------------------------------------------------------
CFsTextViewerVisualizer::CFsTextViewerVisualizer( 
        CFsTextViewerVisualizerSettings* aSettings )
    : iSettings( aSettings ),
      iIsLayouted( EFalse )
    {
    FUNC_LOG;
    }    

// ---------------------------------------------------------------------------
// ConstructVisualsL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ConstructVisualsL( 
        TBool aRefreshBg )
    {
	const TInt KDisableScreensaver     = 1;
	const TInt KEnableScreensaver      = 0;
	
	//screen saver off
	User::ResetInactivityTime(); 	
	TInt error=RProperty::Set(KPSUidScreenSaver,KScreenSaverAllowScreenSaver,KDisableScreensaver);
	//end
    FUNC_LOG;
    UpdateDisplaySize();
    iLayouter->EnableSmileys( iSmileyEnabled );
    
    MFsRichTextObject* object = NULL;
    
    while ( iParser->IsNextObject() )
        {
        if ( object )
            {
            delete object;
            object = NULL;
            }
        object = iParser->GetNextObjectL();
		if( !object ) continue;
        iLayouter->ChangeDirection( iParser->GetParagraphDirection( object->GetBeginOfObject() ) );
        if ( iLayouter->IsNewLine() )
            {
            if ( object->GetType() == MFsRichTextObject::EFsText )
                {
                if ( static_cast<CFsRichTextText*>(object)->GetTextDirection() == TBidiText::ERightToLeft )
                    {
                    iLayouter->SetDirection( TBidiText::ERightToLeft );
                    }
                }
            }
        
        if ( object->GetType() == MFsRichTextObject::EFsNewLine )
            {
            TCharFormat charFormat;
            TCharFormatMask charFormatMask;
            iCFsRichText->GetSpecificCharFormat( charFormat, charFormatMask, object->GetBeginOfObject() );
            TInt styleId;
            styleId = iTextStyleManager->GetStyleIDL( charFormat );
            CFont* font;
            User::LeaveIfNull( iScreenDev );
            CAlfTextStyle* textStyle(
                iEnv->TextStyleManager().TextStyle( styleId ) );

            HBufC* typeface( textStyle->TypefaceNameL() );
            CleanupStack::PushL( typeface );
            TFontSpec fontSpec( *typeface, textStyle->TextSizeInTwips() );

            User::LeaveIfError(
                iScreenDev->GetNearestFontInTwips( font, fontSpec ) );
            TInt height;
            height = font->HeightInPixels();
            iScreenDev->ReleaseFont( font );
            CleanupStack::PopAndDestroy( typeface );
            
            iLayouter->ConstructLineL( height );
            }
        else
            {
            iLayouter->AppendObjectL( object, iContentLayout, iParser );
            }
        }
    
    iLayouter->ConstructLineL();
    
    if ( object )
        {
        delete object;
        object = NULL;
        }
    
    iNavigator->ScrollAllL( 0, 0, iContentLayout );
    iNavigator->RegisterHotspotsL();
    iTotalHeight = iNavigator->GetTotalHeight();
    iTotalLines = iLayouter->Lines();
    iIsLayouted = ETrue;
    
    if ( aRefreshBg )
        {
        RefreshPartBgsL();
        }
    //screen saver on
	error=RProperty::Set(KPSUidScreenSaver,KScreenSaverAllowScreenSaver,KEnableScreensaver);
    //end
    }


// ---------------------------------------------------------------------------
// PrepareRangedVisL
// ---------------------------------------------------------------------------
TFsRangedVisual CFsTextViewerVisualizer::PrepareRangedVisL( 
        MFsRichTextObject* aObject, CAlfLayout* aLayout )
    {
    FUNC_LOG;
    TFsRangedVisual retVal;
    retVal.iVisual = NULL;
    retVal.iHotspot = EFalse;
    retVal.iEmbed = EFalse;

    switch ( aObject->GetType() )
        {
        case MFsRichTextObject::EFsText:
            {
            retVal.iStart = aObject->GetBeginOfObject(); 
            retVal.iEnd = aObject->GetEndOfObject();
            retVal.iIsActive = ETrue;
            retVal.iLayout = aLayout;
            retVal.iIsText = ETrue;
            retVal.iTextColor = 
                static_cast<CFsRichTextText*>(aObject)->GetTextColor();
            break;
            }
        case MFsRichTextObject::EFsTexture:
            {
            retVal.iStart = aObject->GetBeginOfObject(); 
            retVal.iEnd = aObject->GetEndOfObject();
            retVal.iIsActive = ETrue;
            retVal.iLayout = aLayout;
            retVal.iIsText = EFalse;
            break;
            }
        case MFsRichTextObject::EFsSmiley:
            {
            if ( iSmileyEnabled )
                {
                retVal.iStart = aObject->GetBeginOfObject(); 
                retVal.iEnd = aObject->GetEndOfObject();
                retVal.iIsActive = ETrue;
                retVal.iLayout = aLayout;
                retVal.iIsText = EFalse;
                }
            else
                {
                retVal.iStart = aObject->GetBeginOfObject(); 
                retVal.iEnd = aObject->GetEndOfObject();
                retVal.iIsActive = ETrue;
                retVal.iLayout = aLayout;
                retVal.iIsText = ETrue;
                }
            break;
            }
        case MFsRichTextObject::EFsNewLine:
            {
            break;
            }
        default:
            {
            break;
            }
        }

    if ( retVal.iIsActive )
        {
        if ( aObject->GetEmbeded() && aLayout == iContentLayout )
            {
            retVal.iId = aObject->GetIdOfEmbeded();
            retVal.iEmbed = ETrue;
            }
        if ( aObject->GetHotSpot() )
            {
            retVal.iHotspotType = aObject->GetTypeOfHotSpot();
            retVal.iId = aObject->GetIdOfHotSpot();
            retVal.iHotspot = ETrue;
            }
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// DrawCursor
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::DrawCursorL( TInt aTime )
    {
    FUNC_LOG;
    if ( !iCursor )
        {
        iCursor = CAlfImageVisual::AddNewL( *iControl, iRootLayout );
        iCursor->SetFlag( EAlfVisualFlagManualLayout );
        iCursor->EnableBrushesL();
        iCursor->Brushes()->AppendL( iBorder, EAlfDoesNotHaveOwnership );
        iCursor->SetRect( TRect( 0, 0, 0, 0 ) );
        }
    
    if ( iMark ) 
        {
        DrawMarkL( aTime );
        }

    TRect target = iNavigator->GetCursorRectL();
    target.iBr.iX = target.iTl.iX;

    iCursor->SetRect( target, aTime );

    if ( target.iTl.iY < 0 )
        { 
        ScrollUpL( iLineOffset + target.iTl.iY, 0 );
        }
    else if ( target.iBr.iY > iContentSize.iHeight )
        {
        ScrollDownL( iLineOffset + target.iBr.iY - iContentSize.iHeight, 0 );
        }
    }

// ---------------------------------------------------------------------------
// DrawMark
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::DrawMarkL( TInt /* aTime */ )
    {
    FUNC_LOG;
    if ( iMark )
        {
        iMarkFacility->FollowCursorL();
        }
    }

// ---------------------------------------------------------------------------
// InitDefaultsL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::InitDefaultsL()
    {
    FUNC_LOG;
    iScrollingOffset = iSettings->iScrollingOffset;
    iCursorDelay = iSettings->iCursorDelay;
    iScrollingDelay = iSettings->iScrollingDelay;
    iExpandDelay = iSettings->iExpandDelay;
    iMark = EFalse;
    iCursorEnabled = ETrue;
    iSmileyEnabled = ETrue;
    iLineOffset = 0;
    iSmileyDict = NULL;
    }

// ---------------------------------------------------------------------------
// ConstructSelfL
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::ConstructSelfL( 
        CFsRichText* aRichText, 
        CFsSmileyDictionary* aSmDict, 
        TBool aNew )
    {
    FUNC_LOG;
    iSkinData = CFsTextViewerSkinData::NewL();

    iBorder = CAlfBorderBrush::NewL( *iEnv, 1, 1, 0, 0  );
    iBorder->SetColor( iSkinData->iCursorColor );

    iRootLayout = CAlfDeckLayout::AddNewL( *iControl );
    iRootLayout->SetFlag( EAlfVisualChanged );

    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
    iDisplaySize = mainPaneRect.Size();

    iRootLayout->SetSize( iDisplaySize );
    iRootLayout->SetFlag( EAlfVisualFlagManualLayout );

    if ( iBackgroundBrush )
        {
        iRootLayout->EnableBrushesL();
        iRootLayout->Brushes()->AppendL( 
                iBackgroundBrush, 
                EAlfDoesNotHaveOwnership );
        }

    if ( iWatermark )
        {
        if ( iWatermarkVisual )
            {
            iWatermarkVisual->RemoveAndDestroyAllD();
            }
        SetWatermarkL( iWatermark );
        }

    iBackgroundLayout = CAlfLayout::AddNewL( *iControl, iRootLayout );
    iBackgroundLayout->SetFlag( EAlfVisualFlagManualLayout );
    iBackgroundLayout->SetClipping( EFalse );
    iBackgroundLayout->SetSize( iDisplaySize );
    iBackgroundLayout->EnableScrollingL();

    iComponentLayout =CAlfGridLayout::AddNewL(*iControl,
            1, 1, iRootLayout);
    iComponentLayout->SetClipping(ETrue);
    iComponentLayout->EnableBrushesL();
    iComponentLayout->UpdateChildrenLayout();
    
    iContentLayout = CAlfAnchorLayout::AddNewL( *iControl, iComponentLayout );
    iContentLayout->MoveToFront();
    iContentLayout->EnableScrollingL();
    iContentLayout->SetClipping( EFalse );
    
    iStatusLineLayout = CAlfAnchorLayout::AddNewL( *iControl, iContentLayout );
    iStatusLineLayout->SetClipping( ETrue );
    iStatusLineLayout->SetSize( TSize( 0, 0 ) );
    iStatusLineLayout->SetFlag( EAlfVisualFlagManualLayout );
    
    iScrollbarVisibility = EFsScrollbarAuto;

    iScrollbar = new (ELeave) CAknDoubleSpanScrollBar(0);
    iScrollbar->ConstructL(ETrue, iControl, 0, CEikScrollBar::EVertical, 0 );

    TInt pageHeight = iContentLayout->Size().IntTarget().iY;
    
    iScrollbarModel.SetScrollSpan( pageHeight );
    iScrollbarModel.SetWindowSize( pageHeight );
    iScrollbarModel.SetFocusPosition( 0 );
    
    iScrollbar->MakeVisible( EFalse );
    iScrollbarVisible = EFalse;
    iScrollbar->SetModelL( &iScrollbarModel );

    iDummyScrollbar = CAlfImageVisual::AddNewL( *iControl );
    iComponentLayout->Append( iDummyScrollbar );
    
    SetColumnsL( ETrue );
    
    if ( aNew )
        {
        if ( !iTextStyleManager ) 
            {
            iTextStyleManager = CFsTextStyleManager::NewL( 
                    iControl->Env().TextStyleManager() );
            }
    
        iTextureLoader = CFsTextureLoader::NewL( 
                iControl->Env().TextureManager(), 
                iFsTextureObserver ); 
    
        iParser = CFsTextParser::NewL( 
                *aRichText, 
                iControl->Env(), 
                iTextureLoader );
        iParser->SetSizeOfSmile( TSize( 20,20 ) );
        iParser->SetTextStylemanager( iTextStyleManager );
        
        if ( aSmDict )
            {
            CFsSmileyParser* smParser = CFsSmileyParser::NewL();
            smParser->SetSmileyDictionary( aSmDict );
            iParser->SetSmileyParser(smParser);
            iSmileyDict = aSmDict;
            }
        }

    iHelper = CFsTextViewerCoordinator::NewL();

    iSelector = CFsTextViewerSelectsManager::NewL( this );

    iNavigator = CFsTextViewerVisualizerData::NewL( 
            iControl, 
            iParser, 
            iSelector );
    iNavigator->SetScreenHeight( iDisplaySize.iHeight );
    iNavigator->SetStatusLineLayout( iStatusLineLayout );

    iMarkFacility = CFsTextViewerMarkFacility::NewL( 
            iMarkingLayout, 
            iControl, 
            iNavigator, 
            iSkinData );
    
    iLayouter = CFsTextViewerLayouter::NewL( iEnv );
    iLayouter->SetNavigator( iNavigator );
    iLayouter->SetHelper( iHelper );
    iLayouter->SetMargins( iSettings->iLeftMargin, iSettings->iRightMargin );
    iLayouter->iVisualizer = this;
    iLayouter->iControl = iControl;
                                                
    iScreenDev = CCoeEnv::Static()->ScreenDevice();
    iEffectiveOffset.SetTarget( 0, 0 );
    
    UpdateDisplaySize();
    }

// ---------------------------------------------------------------------------
// FindMaxLength
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizer::FindMaxLengthL( CAlfVisual* aVisual )
    {
    FUNC_LOG;
    CAlfTextVisual* visual = static_cast<CAlfTextVisual*>(aVisual);
    TInt retVal = visual->Text().Length();
    TInt length = visual->Text().Length();
    TInt maxWidth = iContentSize.iWidth;
    TInt width = visual->Size().Target().iX;

    while ( width >= maxWidth )
        {
        width = visual->SubstringExtents( 0, --retVal ).Width();
        }

    if ( length - retVal < KMinLettersInNewLine )
        {
        retVal -= KMinLettersInNewLine - ( length - retVal );
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// FindMaxLength
// ---------------------------------------------------------------------------
TInt CFsTextViewerVisualizer::FindMaxLengthL( MFsRichTextObject* aObject )
    {
    FUNC_LOG;
    const TDesC& text = static_cast<CFsRichTextText*>(aObject)->GetText();
    TInt retVal = text.Length();
    TInt length = text.Length();
    TInt maxWidth = iContentSize.iWidth;
    TInt textStyle = static_cast<CFsRichTextText*>(aObject)->GetStyleId();
    TInt width = CalculateWidthL( 
                text.Left( length ),
                textStyle );
    

    while ( width >= maxWidth )
        {
        width = CalculateWidthL( 
                text.Left( --retVal ),
                textStyle );
        }

    if ( length - retVal < KMinLettersInNewLine )
        {
        retVal -= KMinLettersInNewLine - ( length - retVal );
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// Updates scrollbar
// ---------------------------------------------------------------------------
void CFsTextViewerVisualizer::UpdateScrollbarL()
    {
    FUNC_LOG;
    
    // If scroll event handling is ongoing, it means that user is dragging the
    // scroll bar. In this case the scroll bar is updating itself
    // automatically, so we don't need to do anything here.
    if( iControl->IsScrollEventHandlingOngoing() )
        {
        return;
        }
    
    // ---------------------------------------------------------------------------
    //  Updates scrollbar
    // ---------------------------------------------------------------------------
    //
    iScrollbarPos = iContentLayout->ScrollOffset().IntTarget().iY;
    TInt pageHeight = iContentLayout->Size().IntTarget().iY;

    TFsScrollbarVisibility visibility = iScrollbarVisibility;

    TBool showScrollbar( EFalse );

    if ( visibility == EFsScrollbarAuto )
        {
        if ( iTotalHeight < pageHeight )
            {
            showScrollbar = EFalse;
            }
        else
            {
            showScrollbar = ETrue;
            }
        }
    else if ( visibility == EFsScrollbarShowAlways )
        {
        showScrollbar = ETrue;
        }
    else // EFsScrollbarHideAlways
        {
        showScrollbar = EFalse;
        }
    
    TInt firstVisible = iNavigator->GetFirstVisible()->iLineNumber;
    TInt firstVisibleOffset = iNavigator->GetLineOffset( firstVisible );

    iScrollbarModel.SetScrollSpan( Max( iTotalHeight, pageHeight ) );
    iScrollbarModel.SetWindowSize( pageHeight );
    iScrollbarModel.SetFocusPosition( iLineOffset );

    iScrollbar->SetModelL(&iScrollbarModel);

    if ( (showScrollbar != iScrollbarVisible) ||
            ((iComponentLayout->ColumnCount() > 1) && !showScrollbar) )
        {
        SetColumnsL( showScrollbar );
        
        iScrollbarVisible = showScrollbar;
        }

    // Layout sizes might have changed, so update the new sizes
    UpdateDisplaySize();

    iScrollbar->MakeVisible( iScrollbarVisible );
    iScrollbar->DrawNow();
    }

void CFsTextViewerVisualizer::SetColumnsL( TBool aShowScrollBar )
    {
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);

    TRect listRect = iRootLayout->DisplayRectTarget();
    listRect.Move(mainPaneRect.iTl);

    if (aShowScrollBar)
        {
        TRect scrollPane, listPane;
    
        CFsLayoutManager::LayoutMetricsRect(
                listRect,
                CFsLayoutManager::EFsLmSpFsScrollPane,
                scrollPane);
        CFsLayoutManager::LayoutMetricsRect(
                listRect,
                CFsLayoutManager::EFsLmMainSpFsListPane,
                listPane);
    
        iScrollbar->SetRect(scrollPane);
        RArray<TInt> columns;
        if (CFsLayoutManager::IsMirrored())
            {
            columns.Append(scrollPane.Width());
            columns.Append(listPane.Width());
            }
        else
            {
            columns.Append(listPane.Width());
            columns.Append(scrollPane.Width());
            }
        iComponentLayout->SetColumnsL(columns);
        columns.Close();
        if (CFsLayoutManager::IsMirrored())
            {
            iComponentLayout->SetFlag(EAlfVisualFlagFreezeLayout);
            iComponentLayout->Reorder(*iDummyScrollbar, 0, 0);
            iComponentLayout->ClearFlag(EAlfVisualFlagFreezeLayout);
            }
        }
    else
        {
        RArray<TInt> columns;
        columns.Append(listRect.Width());
        iComponentLayout->SetColumnsL(columns);
        columns.Close();
        if (CFsLayoutManager::IsMirrored())
            {
            iComponentLayout->SetFlag(EAlfVisualFlagFreezeLayout);
            iComponentLayout->Reorder(*iContentLayout, 0, 0);
            iComponentLayout->ClearFlag(EAlfVisualFlagFreezeLayout);
            }
        }
    iComponentLayout->UpdateChildrenLayout(0);
    }

// ---------------------------------------------------------------------------
//  Handle visibility change
// ---------------------------------------------------------------------------
//
void CFsTextViewerVisualizer::NotifyControlVisibilityChange( TBool aIsVisible )
    {
    if ( aIsVisible )
        {
        TRAP_IGNORE( UpdateScrollbarL() );
        }
    else
        {
        iScrollbar->MakeVisible( aIsVisible );
        iScrollbarVisible = aIsVisible;
        }
    }

TInt CFsTextViewerVisualizer::GetTotalHeight()
    {
    return iTotalHeight;
    }

TInt CFsTextViewerVisualizer::GetViewTopPosition()
    {
    return iLineOffset;
    }

TPoint CFsTextViewerVisualizer::GetViewCenterPosition()
    {
    TInt x = iContentSize.iWidth / 2;
    TInt y = GetViewTopPosition() + ( iContentSize.iHeight / 2 );
    return TPoint( x, y );
    }
