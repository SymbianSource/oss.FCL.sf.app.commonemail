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
* Description:  Visualizer class for TextViewer component
*
*/


#ifndef C_FSTEXTVIEWERVISUALIZER_H
#define C_FSTEXTVIEWERVISUALIZER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
#include <gdi.h>

#include <eikscrlb.h>  // TAknDoubleSpanScrollBarModel

#include "fstextureobserver.h"
#include "fsscrollbarset.h"

class CAlfImageBrush;
class CAlfTextVisual;
class CAlfControl;
class CAlfBorderBrush;
class CAlfVisual;
class CAlfLayout;
class CAlfTexture;
class CAlfImageVisual;
class CFsAlfScrollbarLayout;
class CAlfDeckLayout;
class CAlfAnchorLayout;
class CAlfGridLayout;
class CAlfBrush;
class CFsTextViewerControl;
class CFsRichText;
class CFsTextParser;
class CFsTextViewerCoordinator;
class MFsRichTextObject;
class CFsTextStyleManager;
class CFsTextViewerVisualizerData;
class CFsTextViewerSelectsManager;
class CFsTextViewerMarkFacility;
class CFsSmileyDictionary;
class CFsTextViewerSkinData;
class CFsTextViewerLoader;
class TFsRangedVisual;
class CFsTextViewerWatermark;
class CFsTextViewerVisualizerSettings;
class CWsScreenDevice;
class CFsTextureLoader;
class MFsTextureObserver;
class CFsTextViewerLayouter;

enum TFsScrollDirection
    {
    EFsDirectionUp = 0,
    EFsDirectionDown,
    EFsDirectionLeft,
    EFsDirectionRight
    };

class TFsTextureObserver 
    : public MFsTextureObserver
    {
    public: 
        void FsTextureEvent( const TFsTextureEvent& aEventType,
                             CFsTexture& aTexture 
                           );
    };

class CFsTextViewerVisualizer : public CBase
    {
    public:
	    // Enumerator "constants" which indicate usage of default values
        enum
            {
            EUseDefaultScrollOffset = -10000,
            EUseDefaultScrollTime = -1
            };
    
    public:
        static CFsTextViewerVisualizer* NewL( 
        CFsTextViewerControl& aControl, 
        CFsRichText* aRichText,
        CFsSmileyDictionary* aSmDict,
        CFsTextViewerVisualizerSettings* aSettings );
        ~CFsTextViewerVisualizer();

    public:
        void ScrollUpL( TInt aOffset = EUseDefaultScrollOffset, TInt aTime = EUseDefaultScrollTime );
        void ScrollDownL( TInt aOffset = EUseDefaultScrollOffset, TInt aTime = EUseDefaultScrollTime );
        void PageUpL( TInt aTime = EUseDefaultScrollTime );
        void PageDownL( TInt aTime = EUseDefaultScrollTime );
        TBool NextVisibleHotspotL( TBool& aChanged );
        TBool PrevVisibleHotspotL( TBool& aChanged );
        void MoveLeftL();
        void MoveRightL();
        void MoveDownL();
        void MoveUpL();
        void EnableMarkL( TBool aEnabled = ETrue );
        void SwitchMarkL();
        TBool MarkEnabled();
        CAlfLayout* ExpandL( TInt aId, TInt aStart, CAlfVisual* aVisual );
        void ExpandExistingL( 
                CAlfLayout* aLayout, 
                TSize aTargetSize, 
                TInt aId );
        void CollapseL( CAlfLayout* aLayout, TInt aId );
        TSize LayoutVisualsL( CAlfLayout* aLayout, TInt aStart, TInt aEnd );
        CFsTextViewerVisualizerData* Navigator();
        CFsTextParser* Parser();
        void ClickedL();
        TBool IsHotspotHighlighted();
        TBool SmileysEnabled();
        void EnableSmileys( TBool aStatus = ETrue );
        void SetScrollOffsetInPx( TInt aPxOffset );
        void SetScrollOffsetInLines( TInt aLineOffset );
        void SetSmileySize( TSize aSize );
        void SetScrollDelay( TInt aDelay );
        void SetCursorDelay( TInt aDelay );
        void Reset();
        void ReloadL();
        void GetMarkedRange( TInt& aBegin, TInt& aEnd );
        void GetHotspotRangeL( TInt& aBegin, TInt& aEnd );
        TSize CalculateSizeL( MFsRichTextObject* aObject ); //to delete
        TInt CalculateWidthL( const TDesC& aText, TInt aTextStyleId ); //to delete
        void SetWatermarkL( CFsTextViewerWatermark* aWatermark );
        CAlfLayout* RootLayout();
        void SetBackgroundColorL( const TRgb& aColor );
        void SetBackgroundImageL( CAlfTexture& aTexture );
        void SetBackgroundOpacity();
        void RefreshPartBgsL();
        void RedrawPartBgsL();
        CAlfControl* Control();
        CAlfLayout* StackLayout();
        CAlfLayout* BackgroundLayout();
        void RemoveEndLinesL( TInt aLines );
        void ReplaceTextL( 
                CFsRichText* aRichText, 
                CFsSmileyDictionary* aSmDict = NULL );
        void FocusLineL( TInt aLine, TBool aShowAsFirstLine = EFalse );
        void UpdateDisplaySize();
        void ReloadPicturesL( TInt aLine );
        TBool GetVisibleFocusedHotspotLine( TInt& aLineNum ) const;
        // <cmail>
        void SetCurrentHotspotByOrderL( TInt aIndex, TBool aFocusLine = EFalse );
        // </cmail>
        void SetCurrentHotspotByCharL( TInt aIndex );
        CAlfLayout* GetStatusLayout();
        void UpdateStatusLayout();
        void RemoveStatusLayout();
        HBufC* GetCurrentHotspotTextL();
        // <cmail>
        TInt GetCurrentHotspotByOrder();
        // </cmail>
        CAknDoubleSpanScrollBar* GetScrollBar();
        void NotifyControlVisibilityChange( TBool aIsVisible );

        TInt GetTotalHeight();
        TInt GetViewTopPosition();
        TPoint GetViewCenterPosition();

    public:
        CFsTextViewerSkinData* iSkinData;
        CFsTextStyleManager* iTextStyleManager;
        CFsTextViewerVisualizerSettings* iSettings;
        
        // Size of the view's main pane (including content and scroll bar)
        TSize iDisplaySize;
        // Size of the actual text content area (without scroll bar)
        TSize iContentSize;
        
        TInt iFirstLine;
        TInt iTotalLines;

    private:
        void ConstructL( 
                CFsTextViewerControl& aControl,
                CFsRichText* aRichText,
                CFsSmileyDictionary* aSmDict );
        CFsTextViewerVisualizer( 
                CFsTextViewerVisualizerSettings* aSettings );

    private:
        void ConstructVisualsL( 
                TBool aRefreshBg = ETrue );
        TFsRangedVisual PrepareRangedVisL( 
                MFsRichTextObject* aObject, 
                CAlfLayout* aLayout );
        void DrawCursorL( TInt aTime );
        void DrawMarkL( TInt aTime = 0 );
        void InitDefaultsL();
        void ConstructSelfL( CFsRichText* aRichText, 
                CFsSmileyDictionary* aSmDict, TBool aNew = ETrue );
        TInt FindMaxLengthL( CAlfVisual* aVisual ); //to delete
        TInt FindMaxLengthL( MFsRichTextObject* aObject ); //to delete
        void UpdateScrollbarL();
        void SetColumnsL( TBool aShowScrollBar );

    private:
        CFsTextViewerControl* iControl;
        CAlfTextVisual* iText;
        CAlfBorderBrush* iBorder;
        CAlfAnchorLayout* iContentLayout;
        CAlfDeckLayout* iRootLayout;
        CAlfLayout* iMarkingLayout;
        CAlfLayout* iBackgroundLayout;
        CAlfGridLayout* iComponentLayout;
        CAlfEnv* iEnv;
        CAlfAnchorLayout* iStatusLineLayout;
        CFsTextParser* iParser;
        CFsTextViewerCoordinator* iHelper;
        TInt iScrollingOffset;
        CAlfVisual* iCursor;
        TInt iScrollingDelay;
        TInt iCursorDelay;
        TInt iTotalHeight;
        TInt iLineOffset;

		/*
		 * Scroll bar
		 */
        CAknDoubleSpanScrollBar* iScrollbar;
        TAknDoubleSpanScrollBarModel iScrollbarModel;
        CAlfImageVisual* iDummyScrollbar;
        /**
         * Visibility of scrollbar
         */
        TFsScrollbarVisibility iScrollbarVisibility;
        TBool iScrollbarVisible;
        /**
         * Scrollbar position
         */
        TInt iScrollbarPos;

        TBool iMark;
        TBool iCursorEnabled;
        TBool iScrollByCursor;
        CFsRichText* iCFsRichText;
        CFsTextViewerVisualizerData* iNavigator;
        TBool iSmileyEnabled;
        TInt iExpandDelay;
        CFsTextViewerMarkFacility* iMarkFacility;
        CFsTextViewerSelectsManager* iSelector;
        CFsSmileyDictionary* iSmileyDict;
        CFsTextViewerWatermark* iWatermark;
        CAlfBrush* iBackgroundBrush;
        CAlfImageVisual* iWatermarkVisual;
        TBool iIsLayouted;
        TInt iRichTextLength;
        CWsScreenDevice* iScreenDev;
        CFsTextureLoader* iTextureLoader;
        TFsTextureObserver iFsTextureObserver;
        TAlfTimedValue iEffectiveOffset;
        CFsTextViewerLayouter* iLayouter;
    };

#endif // C_FSTEXTVIEWERVISUALIZER_H

