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
* Description:  Storage class for visuals belonging to visualizer
*
*/


#ifndef C_FSTEXTVIEWERVISUALIZERDATA_H
#define C_FSTEXTVIEWERVISUALIZERDATA_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
#include <gdi.h>
#include <biditext.h>

class CAlfVisual;
class TAlfRealRect;
class CAlfLayout;
class CAlfAnchorLayout;
class CAlfTextVisual;
class CAlfControl;
class CAlfBorderBrush;
class CFsTextParser;
class CFsTextViewerSelectsManager;
class CFsTextViewerCoordinator;
class CFsTextParser;

class TKeyArrayPtr : public TKeyArrayFix
    {
    public:
        inline TKeyArrayPtr(TInt aOffset, TKeyCmpText aType) 
            :TKeyArrayFix(aOffset, aType) {}
        inline TKeyArrayPtr(TInt aOffset, TKeyCmpText aType, TInt aLength)
            :TKeyArrayFix(aOffset, aType, aLength) {}
        inline TKeyArrayPtr(TInt aOffset, TKeyCmpNumeric aType) 
            :TKeyArrayFix(aOffset, aType) {}
        virtual TAny* At(TInt aIndex) const;
    };

class TFsVisRange
    {
    public:
        TInt iStart;
        TInt iEnd;
    };

class TFsRangedVisual
    {
    public:
        CAlfVisual* iVisual;
        CAlfLayout* iLayout;
        TRect iRect;
        TBool iIsActive;
        TBool iIsText;
        TBool iEmbed;
        TBool iHotspot;
        TBool iIsVisible;
        TBool iSmiley;
        TBool iMoved;
        TBidiText::TDirectionality iDirection;
        TInt iStart;
        TInt iEnd;
        TInt iHotspotType;
        TInt iId;
        TInt iTextStyleId;
        TInt iLineNumber;
        TRgb iTextColor;
    };

class TFsPartBgVisual
    {
    public:
        TInt iStart;
        TInt iEnd;
        CAlfVisual* iVisual;
        TRect iRect;
    };

class TFsCursorPos
    {
    public: 
        TInt iVisual;
        TInt iOffset;
    };

class CFsTextViewerVisualizerData : public CBase
    {
    public:
        static CFsTextViewerVisualizerData* NewL( 
                CAlfControl* aControl, 
                CFsTextParser* aParser, 
                CFsTextViewerSelectsManager* aSelector );
        ~CFsTextViewerVisualizerData();

    public:
        void AppendVisualL( TFsRangedVisual aVisual );
        void AppendVisualL( TFsRangedVisual* aVisual );
        void AppendVisualL( CAlfVisual* aVisual, TFsVisRange aRange );
        void AppendLayoutL( CAlfVisual* aVisual, TFsVisRange aRange );
        void AppendBgL( TFsPartBgVisual aBg );
        TBool RemoveBg( CAlfVisual* aVisual );
        void ResetBgArray();
        void InsertVisualL( 
                TInt aIndex, 
                CAlfVisual* aVisual, 
                TFsVisRange aRange );
        void PopVisual();
        void RemoveVisual( TInt aIndex );
        void DisableVisual( TInt aIndex, TBool aStatus = ETrue );
        void DisableVisual( CAlfVisual* aVisual, TBool aStatus = ETrue );
        void EnableVisual( TInt aIndex, TBool aStatus = ETrue );
        void EnableVisual( CAlfVisual* aVisual, TBool aStatus = ETrue );
        CAlfVisual* GetVisual( TInt aIndex );
        TInt GetVisLength( TInt aIndex );
        TInt GetVisStartOffset( TInt aIndex );
        TInt GetVisEndOffset( TInt aIndex );
        TInt GetLayEndOffset( TInt aIndex );
        TFsVisRange GetVisOffsets( TInt aIndex );
        TInt Count();
        TInt Find( CAlfVisual* aVisual );
        TInt FindLayout( CAlfVisual* aVisual );
        TInt Find( TInt aOffset );
        TBool MoveToNextCharL();
        TBool MoveToPrevCharL();
        TInt FindPrevChar();
        TRect GetCursorRectL();
        TInt GetCursorCharOffset();
        void SetCursorCharOffset( TInt aOffset );
        void Sort();
        TInt RestoreL( TFsRangedVisual* aVisual );
        void ScrollAllL( TInt aTrans, TInt aTime, CAlfLayout* aRootLayout );
        TInt GetTextStyle( CAlfTextVisual* aVisual );
        void DimAllHotspotsL();
        void RefreshSelectsL();
        TRect GetVisualPos( TInt aCharIndex );
        TFsRangedVisual* GetVisualAtIndex( TInt aIndex );
        void ScrollFromCharL( TInt aCharIndex, TInt aTrans, TInt aTime, CAlfLayout* aRootLayout ); //<cmail> leaving
        void SetScreenHeight( TInt aScreenHeight );
        TFsRangedVisual* GetVisualWithChar( TInt aCharIndex );
        TFsRangedVisual* GetFirstVisible();
        TFsRangedVisual* GetLastVisible();
        TFsRangedVisual* GetRangedVisual( CAlfVisual* aVisual );
        void UpdateLinesNumbers();
        TInt GetTotalLines();
        TInt RemoveFromLineL( 
                TInt aLineNumber, 
                CFsTextViewerCoordinator* aHelper );
        TInt GetFirstCharInLine( TInt aLineNumber );
        //<cmail>
        TInt GetCharLineNumber( TInt aCharNumber );
        //</cmail>
        void SetParser( CFsTextParser* aParser );
        void FindAndHighlightHotspotL( TInt aLine );
        TInt GetCursorLine();
        void ReloadPicturesL( TInt aLine );
        TInt GetTotalHeight();
        void HideActionButtonFromCurrentHighlightedHotspot();
        void ShowActionButtonForCurrentHighlightedHotspotL();
        void HideAll();
        void RestoreVisibleL();
        void RegisterHotspotsL();
        void SetStatusLineLayout( CAlfAnchorLayout* aLayout );
        TInt GetLineOffset( TInt aLine );

    public:
        enum TFsScrollDirection
            {
            EFsScrollDown = 1,
            EFsScrollUp
            };

    private: 
        void ConstructL();
        CFsTextViewerVisualizerData( 
                CAlfControl* iControl, 
                CFsTextParser* aParser, 
                CFsTextViewerSelectsManager* aSelector );
		// <cmail> needed anymore ?
        //void DoScrollFromCharL( TInt aCharIndex, TInt aTrans, TInt aTime, TRect aVisibleArea );
		// </cmail>

    private:
        RPointerArray<TFsRangedVisual> iVisArray;
        RArray<TFsRangedVisual> iLayoutArray;
        RArray<TFsPartBgVisual> iBgArray;
        CAlfAnchorLayout* iStatusLineLayout;
        TFsCursorPos iPosition;
        TInt iCurrentChar;
        TInt iPrevChar;
        TInt iLength;
        CAlfControl* iControl;
        CFsTextParser* iParser;
        CFsTextViewerSelectsManager* iSelector;
        TInt iScreenOffset;
        TInt iTopBorder;
        TInt iBottomBorder;
        TInt iScreenHeight;
        TInt iVisualAmount;
        TFsScrollDirection iScrollDirection;        
        CAlfBorderBrush* iDebugBrush;
    };

#endif //C_FSTEXTVIEWERVISUALIZERDATA_H
