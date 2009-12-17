/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CFsTextViewer
*
*/


//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
// <cmail> SF
#include "emailtrace.h"
#include <alf/alfbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alfgradientbrush.h>
// </cmail>

#include "fstextviewer.h"
#include "fstextviewercontrol.h"
#include "fstextviewervisualizer.h"
#include "fstextviewerkeys.h"
#include "fstextviewervisualizersettings.h"
#include "fstextviewervisualizerdata.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewer* CFsTextViewer::NewL ( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    CFsTextViewer* self = new ( ELeave ) CFsTextViewer( aEnv );
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewer
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewer::~CFsTextViewer()
    {
    FUNC_LOG;
    delete iControl->Keys();

    delete iVisualizer;
    iVisualizer = NULL;

    for ( TInt i = 0; i < iSettings->iPartBgs.Count(); ++i )
        {
        delete iSettings->iPartBgs[i].iBrush;
        }

    delete iSettings;
    iSettings = NULL;
    }

// ---------------------------------------------------------------------------
// GetControl
// ---------------------------------------------------------------------------
EXPORT_C CAlfControl* CFsTextViewer::GetControl() const
    {
    FUNC_LOG;
    return static_cast< CAlfControl* >( iControl );
    }

// ---------------------------------------------------------------------------
// SetText
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetTextL( 
        CFsRichText* aText, 
        CFsSmileyDictionary* aSmDict )
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        delete iVisualizer;
        iVisualizer = NULL;
        }

    iVisualizer = CFsTextViewerVisualizer::NewL( 
            *iControl, 
            aText, 
            aSmDict, 
            iSettings );
    if ( iBackColorSet )
        {
        iVisualizer->SetBackgroundColorL( iColor );
        }
    if ( iTexture )
        {
        iVisualizer->SetBackgroundImageL( *iTexture );
        }

    iVisualizer->RefreshPartBgsL();

    iControl->SetVisualizer( iVisualizer );
    }

// ---------------------------------------------------------------------------
// Show
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::Show()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// Hide
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::Hide()
    {
    FUNC_LOG;
    
    }

// ---------------------------------------------------------------------------
// SetObserver
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetObserver( MFsTextViewerObserver* aObserver )
    {
    FUNC_LOG;
    iControl->SetObserver( aObserver );
    }

// ---------------------------------------------------------------------------
// SetScrollOffsetInPx
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetScrollOffsetInPx( TInt aPxOffset )
    {
    FUNC_LOG;
    iVisualizer->SetScrollOffsetInPx( aPxOffset );
    }

// ---------------------------------------------------------------------------
// SetScrollOffsetInLines
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetScrollOffsetInLines( TInt aLineOffset )
    {
    FUNC_LOG;
    iVisualizer->SetScrollOffsetInLines( aLineOffset );
    }

// ---------------------------------------------------------------------------
// SetSmileySize
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetSmileySize( TSize aSize )
    {
    FUNC_LOG;
    iVisualizer->SetSmileySize( aSize );
    }

// ---------------------------------------------------------------------------
// SetScrollDelay
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetScrollDelay( TInt aDelay )
    {
    FUNC_LOG;
    iVisualizer->SetScrollDelay( aDelay );
    }

// ---------------------------------------------------------------------------
// SetCursorDelay
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetCursorDelay( TInt aDelay )
    {
    FUNC_LOG;
    iVisualizer->SetCursorDelay( aDelay );
    }

// ---------------------------------------------------------------------------
// EnableSmileys
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::EnableSmileys( TBool aStatus )
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        iVisualizer->EnableSmileys( aStatus );
        }
    }

// ---------------------------------------------------------------------------
// Smileys
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsTextViewer::Smileys()
    {
    FUNC_LOG;
    TBool retVal = ETrue;
    if ( iVisualizer )
        {
        retVal = iVisualizer->SmileysEnabled();
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// ReloadTextL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::ReloadTextL()
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        iVisualizer->ReloadL();
        }
    }

// ---------------------------------------------------------------------------
// GetCurrentHotspotL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::GetCurrentHotspotL( TInt& aBegin, TInt& aEnd )
    {
    FUNC_LOG;
    iVisualizer->GetHotspotRangeL( aBegin, aEnd );
    }

// ---------------------------------------------------------------------------
// GetMarkedArea
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::GetMarkedArea( TInt& aBegin, TInt& aEnd )
    {
    FUNC_LOG;
    iVisualizer->GetMarkedRange( aBegin, aEnd );
    }

// ---------------------------------------------------------------------------
// EnableCursorUpDown
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::EnableCursorUpDown( TBool aStatus )
    {
    FUNC_LOG;
    iControl->EnableCursorScroll( aStatus );
    }

// ---------------------------------------------------------------------------
// CursorUpDown
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsTextViewer::CursorUpDown()
    {
    FUNC_LOG;
    return iControl->CursorScroll();
    }

// ---------------------------------------------------------------------------
// CFsTextViewer
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetCustomKeys( CFsTextViewerKeys* aKeys )
    {
    FUNC_LOG;
    if ( iControl )
        {
        if ( aKeys != iControl->Keys() )
            {
            CFsTextViewerKeys* keys = NULL;
            
            keys = iControl->Keys();
            
            delete keys;
            keys = NULL;
            }
        iControl->SetKeys( aKeys );
        }
    }

// ---------------------------------------------------------------------------
// Keys
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewerKeys* CFsTextViewer::Keys()
    {
    FUNC_LOG;
    CFsTextViewerKeys* retVal = NULL;
    if ( iControl )
        {
        retVal = iControl->Keys();
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// SetWatermark
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetWatermarkL( 
        CFsTextViewerWatermark* aWatermark )
    {
    FUNC_LOG;
    iWatermark = aWatermark;
    if ( iVisualizer )
        {
        iVisualizer->SetWatermarkL( iWatermark );
        }
    }

// ---------------------------------------------------------------------------
// RootLayout
// ---------------------------------------------------------------------------
EXPORT_C CAlfLayout* CFsTextViewer::RootLayout()
    {
    FUNC_LOG;
    CAlfLayout* retVal = NULL;
    if ( iVisualizer )
        {
        retVal = iVisualizer->RootLayout();
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
// SetBackgroundColor
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetBackgroundColorL( const TRgb& aColor )
    {
    FUNC_LOG;
    iColor = aColor;
    iBackColorSet = ETrue;
    if ( iVisualizer )
        {
        iVisualizer->SetBackgroundColorL( iColor );
        }
    }

// ---------------------------------------------------------------------------
// SetBackgroundImage
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetBackgroundImageL( CAlfTexture& aTexture )
    {
    FUNC_LOG;
    iTexture = &aTexture;
    if ( iVisualizer )
        {
        iVisualizer->SetBackgroundImageL( *iTexture );
        }
    }

// ---------------------------------------------------------------------------
// SetBackgroundOpacity
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetBackgroundOpacity( const TAlfTimedValue& aOpacity )
    {
    FUNC_LOG;
    iSettings->iBackgroundOpacity = aOpacity;
    if ( iVisualizer )
        {
        iVisualizer->SetBackgroundOpacity();
        }
    }

// ---------------------------------------------------------------------------
// BackgroundOpacity
// ---------------------------------------------------------------------------
EXPORT_C const TAlfTimedValue& CFsTextViewer::BackgroundOpacity() const 
    {
    FUNC_LOG;
    return iSettings->iBackgroundOpacity;
    }

// ---------------------------------------------------------------------------
// SetPartBgColor
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetPartBgColorL( 
        TInt aStartChar, 
        TInt aEndChar, 
        const TRgb& aColor )
    {
    FUNC_LOG;
    TFsPartBg background;
    background.iBrush = CAlfGradientBrush::NewL(iControl->Env());
    static_cast<CAlfGradientBrush*>( background.iBrush )->SetColor( aColor );
    background.iStart = aStartChar;
    background.iEnd = aEndChar;
    iSettings->iPartBgs.AppendL( background );
    if ( iVisualizer )
        {
        iVisualizer->RefreshPartBgsL();
        }
    }

// ---------------------------------------------------------------------------
// SetPartBgImage
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetPartBgImageL( TInt aStartChar, TInt aEndChar, 
                                                CAlfTexture& aTexture )
    {
    FUNC_LOG;
    TFsPartBg background;
    background.iBrush = CAlfImageBrush::NewL( iControl->Env(),TAlfImage( aTexture ) );
    background.iStart = aStartChar;
    background.iEnd = aEndChar;
    iSettings->iPartBgs.AppendL( background );
    if ( iVisualizer )
        {
        iVisualizer->RefreshPartBgsL();
        }
    }

// ---------------------------------------------------------------------------
// SetLiteLineBg
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetLiteLineBg( 
        CAlfTexture& aTexture, 
        TReal32 aOpacity )
    {
    FUNC_LOG;
    iSettings->iHighlightLine = &aTexture;
    iSettings->iHighlightLineOpacity = aOpacity;
    }

// ---------------------------------------------------------------------------
// SetActionButton
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetActionButton( 
        CAlfTexture& aTexture, 
        TReal32 aOpacity )
    {
    FUNC_LOG;
    iSettings->iActionButton = &aTexture;
    iSettings->iActionButtonOpacity = aOpacity;
    }

// ---------------------------------------------------------------------------
// SetActionButtonMargin
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetActionButtonMargin( TReal32 aMargin )
    {
    FUNC_LOG;
    iSettings->iActionButtonMargin = aMargin;
    }

// ---------------------------------------------------------------------------
// SetActionButtonSize
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetActionButtonSize( 
        TReal32 aWidth, 
        TReal32 aHeight )
    {
    FUNC_LOG;
    iSettings->iActionButtonWidth = aWidth;
    iSettings->iActionButtonHeight = aHeight;
    }

// ---------------------------------------------------------------------------
// CFsTextViewer
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetMirroring( TBool aEnable )
    {
    FUNC_LOG;
    iSettings->iMirroring = aEnable;
    }

// ---------------------------------------------------------------------------
// GetTotalLines
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewer::GetTotalLines()
    {
    FUNC_LOG;
    return iVisualizer->iTotalLines;
    }

// ---------------------------------------------------------------------------
// GetFirstDisplayedLine
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewer::GetFirstDisplayedLine()
    {
    FUNC_LOG;
    TInt retVal = 0;
    if ( iVisualizer->Navigator()->GetLastVisible() )
        {
        retVal = iVisualizer->Navigator()->GetFirstVisible()->iLineNumber;
        }

    return retVal;
    }

// <cmail>
// ---------------------------------------------------------------------------
// GetLineOffset - get offset of first char in the line (line offset)
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewer::GetFirstCharInLine(TInt aLineNum)
    {
    FUNC_LOG;
    return iVisualizer->Navigator()->GetFirstCharInLine(aLineNum);
    }


// ---------------------------------------------------------------------------
// GetCharLineNumber - get the line number for the given line offset
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewer::GetLineNumber(TInt aChar)
    {
    FUNC_LOG;
    return iVisualizer->Navigator()->GetCharLineNumber(aChar);
    }

// </cmail>

// ---------------------------------------------------------------------------
// GetLastDisplayedLine
// ---------------------------------------------------------------------------
EXPORT_C TInt CFsTextViewer::GetLastDisplayedLine()
    {
    FUNC_LOG;
    TInt retVal = 0;
    if ( iVisualizer->Navigator()->GetLastVisible() )
        {
        retVal = iVisualizer->Navigator()->GetLastVisible()->iLineNumber + 1;
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// RemoveEndLines
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::RemoveEndLinesL( TInt aLines )
    {
    FUNC_LOG;
    iVisualizer->RemoveEndLinesL( aLines );
    }

// ---------------------------------------------------------------------------
// ReplaceTextL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::ReplaceTextL( 
        CFsRichText* aRichText, 
        CFsSmileyDictionary* aSmDict )
    {
    FUNC_LOG;
    iVisualizer->ReplaceTextL( aRichText, aSmDict );
    }

// ---------------------------------------------------------------------------
// FocusLineL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::FocusLineL( TInt aLine, TBool aShowAsFirstLine )
    {
    FUNC_LOG;
    if ( iVisualizer && aLine > 0 )
        {
        iVisualizer->FocusLineL( aLine, aShowAsFirstLine );
        }
    }

// ---------------------------------------------------------------------------
// SetLeftMargin
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetLeftMargin( TInt aMargin )
    {
    FUNC_LOG;
    if ( iSettings )
        {
        iSettings->iLeftMargin = aMargin;
        }
    }

// ---------------------------------------------------------------------------
// SetRightMargin
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetRightMargin( TInt aMargin )
    {
    FUNC_LOG;
    if ( iSettings )
        {
        iSettings->iRightMargin = aMargin;
        }
    }

// ---------------------------------------------------------------------------
// ReloadPicturesL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::ReloadPicturesL( TInt aLine )
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        iVisualizer->ReloadPicturesL( aLine );
        }
    }

// ---------------------------------------------------------------------------
// GetVisibleFocusedHotspotLine
// ---------------------------------------------------------------------------
EXPORT_C TBool CFsTextViewer::GetVisibleFocusedHotspotLine( 
        TInt& aLineNum ) const
    {
    FUNC_LOG;
    TBool retVal = EFalse;
    if ( iVisualizer )
        {
        retVal = iVisualizer->GetVisibleFocusedHotspotLine( aLineNum );
        }

    return retVal;
    }

// ---------------------------------------------------------------------------
// HideActionButtonFromCurrentHighlightedHotspot
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::HideActionButtonFromCurrentHighlightedHotspot()
    {
    FUNC_LOG;
    if ( iVisualizer && iVisualizer->Navigator())
        {
        iVisualizer
            ->Navigator()
            ->HideActionButtonFromCurrentHighlightedHotspot();
        }
    }
    
// ---------------------------------------------------------------------------
// ShowActionButtonForCurrentHighlightedHotspotL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::ShowActionButtonForCurrentHighlightedHotspotL()
    {
    FUNC_LOG;
    if ( iVisualizer && iVisualizer->Navigator())
        {
        iVisualizer
            ->Navigator()
            ->ShowActionButtonForCurrentHighlightedHotspotL();
        }
    }

// ---------------------------------------------------------------------------
// SetCurrentHotspotByOrderL
// ---------------------------------------------------------------------------
// <cmail>
EXPORT_C void CFsTextViewer::SetCurrentHotspotByOrderL( TInt aIndex, TBool aFocusLine )
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        iVisualizer->SetCurrentHotspotByOrderL( aIndex, aFocusLine );
        }
    }

EXPORT_C TInt CFsTextViewer::GetCurrentHotspotByOrder()
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        return iVisualizer->GetCurrentHotspotByOrder();
        }
    else
        {
        return KErrNotFound;
        }
    }
// </cmail>
// ---------------------------------------------------------------------------
// SetCurrentHotspotByCharL
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::SetCurrentHotspotByCharL( TInt aIndex )
    {
    FUNC_LOG;
    if ( iVisualizer )
        {
        iVisualizer->SetCurrentHotspotByCharL( aIndex );
        }
    }

// ---------------------------------------------------------------------------
// GetStatusLayout
// ---------------------------------------------------------------------------
EXPORT_C CAlfLayout* CFsTextViewer::GetStatusLayout()
    {
    FUNC_LOG;
    return iVisualizer->GetStatusLayout();
    }

// ---------------------------------------------------------------------------
// UpdateStatusLayout
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::UpdateStatusLayout()
    {
    FUNC_LOG;
    iVisualizer->UpdateStatusLayout();
    }

// ---------------------------------------------------------------------------
// RemoveStatusLayout
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewer::RemoveStatusLayout()
    {
    FUNC_LOG;
    iVisualizer->RemoveStatusLayout();
    }

// ---------------------------------------------------------------------------
// GetCurrentHotspotTextL
// ---------------------------------------------------------------------------
EXPORT_C HBufC* CFsTextViewer::GetCurrentHotspotTextL()
    {
    FUNC_LOG;
    return iVisualizer->GetCurrentHotspotTextL();
    }

// ---------------------------------------------------------------------------
// CFsTextViewer
// ---------------------------------------------------------------------------
CFsTextViewer::CFsTextViewer( CAlfEnv& /* aEnv */ )
    {
    FUNC_LOG;
    iBackColorSet = EFalse;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewer::ConstructL( CAlfEnv& aEnv )
    {
    FUNC_LOG;
    iControl = CFsTextViewerControl::NewL( aEnv );
    iControl->SetKeys( CFsTextViewerKeys::NewL() );
    iSettings = new ( ELeave ) CFsTextViewerVisualizerSettings();
    }

