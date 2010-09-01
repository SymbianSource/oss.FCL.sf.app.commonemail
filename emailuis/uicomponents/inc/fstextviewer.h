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
* Description:  API class for TextViewer component
*
*/


#ifndef C_FSTEXTVIEWER_H
#define C_FSTEXTVIEWER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
// <cmail> SF
#include <alf/alfenv.h>
// </cmail>
class CFsRichText;
class CFsTextViewerVisualizer;
class CFsTextViewerControl;
class MFsTextViewerObserver;
class CFsSmileyDictionary;
class CFsTextViewerKeys;
class CFsTextViewerWatermark;
class CAlfLayout;
class CAlfTexture;
class CFsTextViewerVisualizerSettings;

class CFsTextViewer : public CBase
    {
    public:
        IMPORT_C static CFsTextViewer* NewL ( CAlfEnv& aEnv );
        IMPORT_C ~CFsTextViewer();
       
    public:
        IMPORT_C CAlfControl* GetControl() const;
        IMPORT_C void SetTextL( 
                CFsRichText* aText, 
                CFsSmileyDictionary* aSmDict );
        IMPORT_C void Show();
        IMPORT_C void Hide();
        IMPORT_C void SetObserver( MFsTextViewerObserver* aObserver );
        IMPORT_C void SetScrollOffsetInPx( TInt aPxOffset );
        IMPORT_C void SetScrollOffsetInLines( TInt aLineOffset );
        IMPORT_C void SetSmileySize( TSize aSize );
        IMPORT_C void SetPictureSize( TSize aSize );
        IMPORT_C void SetScrollDelay( TInt aDelay );
        IMPORT_C void SetCursorDelay( TInt aDelay );
        IMPORT_C void EnableSmileys( TBool aStatus = ETrue );
        IMPORT_C TBool Smileys();
        IMPORT_C void ReloadTextL();
        IMPORT_C void GetCurrentHotspotL( TInt& aBegin, TInt& aEnd );
        IMPORT_C void GetMarkedArea( TInt& aBegin, TInt& aEnd );
        IMPORT_C void EnableCursorUpDown( TBool aStatus = ETrue );
        IMPORT_C TBool CursorUpDown();
        IMPORT_C void SetCustomKeys( CFsTextViewerKeys* aKeys );
        IMPORT_C CFsTextViewerKeys* Keys();
        IMPORT_C void SetWatermarkL( CFsTextViewerWatermark* aWatermark );
        IMPORT_C CAlfLayout* RootLayout();
        IMPORT_C void SetBackgroundColorL( const TRgb& aColor );
        IMPORT_C void SetBackgroundImageL( CAlfTexture& aTexture );
        IMPORT_C void SetBackgroundOpacity( const TAlfTimedValue& aOpacity );
        IMPORT_C const TAlfTimedValue& BackgroundOpacity() const;
        IMPORT_C void SetPartBgColorL( 
                TInt aStartChar, 
                TInt aEndChar, 
                const TRgb& aColor );
        IMPORT_C void SetPartBgImageL( 
                TInt aStartChar, 
                TInt aEndChar, 
                CAlfTexture& aTexture );
        IMPORT_C void SetLiteLineBg( 
                CAlfTexture& aTexture, 
                TReal32 aOpacity );
        IMPORT_C void SetActionButton( 
                CAlfTexture& aTexture, 
                TReal32 aOpacity );
        IMPORT_C void SetActionButtonMargin( TReal32 aMargin );
        IMPORT_C void SetActionButtonSize( TReal32 aWidth, TReal32 aHeight );
        IMPORT_C void SetMirroring( TBool aEnable = ETrue );
        IMPORT_C TInt GetTotalLines();
        IMPORT_C TInt GetFirstDisplayedLine();
        //<cmail>
        IMPORT_C TInt GetFirstCharInLine(TInt aChar);
        IMPORT_C TInt GetLineNumber(TInt aLineOffset);
        //</cmail>
        IMPORT_C TInt GetLastDisplayedLine();
        IMPORT_C void RemoveEndLinesL( TInt aLines );
        IMPORT_C void ReplaceTextL( 
                CFsRichText* aRichText, 
                CFsSmileyDictionary* aSmDict = NULL );
        IMPORT_C void FocusLineL( 
                TInt aLine, 
                TBool aShowAsFirstLine = EFalse );
        IMPORT_C void SetLeftMargin( TInt aMargin );
        IMPORT_C void SetRightMargin( TInt aMargin );
        IMPORT_C void ReloadPicturesL( TInt aLine = -1 );
        IMPORT_C TBool GetVisibleFocusedHotspotLine( TInt& aLineNum ) const;
        IMPORT_C void HideActionButtonFromCurrentHighlightedHotspot();
        IMPORT_C void ShowActionButtonForCurrentHighlightedHotspotL();
        // <cmail>
        IMPORT_C void SetCurrentHotspotByOrderL( TInt aIndex, TBool aFocusLine = EFalse );
        // </cmail>
        IMPORT_C void SetCurrentHotspotByCharL( TInt aIndex );
        IMPORT_C CAlfLayout* GetStatusLayout();
        IMPORT_C void UpdateStatusLayout();
        IMPORT_C void RemoveStatusLayout();
        IMPORT_C HBufC* GetCurrentHotspotTextL();
        // <cmail>
        IMPORT_C TInt GetCurrentHotspotByOrder();
        // </cmail>
        
    private:
        CFsTextViewer( CAlfEnv& aEnv );
        void ConstructL( CAlfEnv& aEnv );
    
    private:
        CFsTextViewerVisualizer* iVisualizer;
        CFsTextViewerControl* iControl;
        CFsTextViewerWatermark* iWatermark;
        CAlfTexture* iTexture;
        TRgb iColor;
        TBool iBackColorSet;
        CFsTextViewerVisualizerSettings* iSettings;
    };

#endif // C_FSTEXTVIEWER_H

