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
* Description:  Visual aligning class
*
*/


#ifndef C_FSTEXTVIEWERLAYOUTER_H
#define C_FSTEXTVIEWERLAYOUTER_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
#include <biditext.h>

class CFsTextViewerCoordinator;
class MFsRichTextObject;
class CFsTextViewerVisualizerData;
class TFsRangedVisual;
class CWsScreenDevice;
class CAlfLayout;
class CFsRichText;
class CAlfEnv;
class CFsTextParser;
class CFsTextViewerVisualizer;

class CFsTextViewerLayouter : public CBase
    {
    public:
        static CFsTextViewerLayouter* NewL( CAlfEnv* aEnv );
        ~CFsTextViewerLayouter();
        
    public:
        TInt MeasureBidiTextBoundsWidth(
            const CFont& aFont,
            const TDesC& aText,
            CFont::TMeasureTextInput::TFlags aOrder);
        TSize CalculateSizeL( MFsRichTextObject* aObject );
        TInt FindMaxLengthL( MFsRichTextObject* aObject );
        TFsRangedVisual* PrepareRangedVisL( 
                        MFsRichTextObject* aObject,
                        CAlfLayout* aLayout );
        void SetDisplaySize( TSize aDisplaySize );
        TBool AppendObjectL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TBool aRegisterSelects = ETrue );
        TBool InsertObjectL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TBool aRegisterSelects = ETrue );
        void ConstructLineL( TInt aHeight = 1 );
        void SetMargins( const TInt aLeft, const TInt aRight );
        void SetHelper( CFsTextViewerCoordinator* aHelper );
        void SetNavigator( CFsTextViewerVisualizerData* aNavigator );
        TInt Lines();
        void EnableSmileys( TBool aEnable = ETrue );
        void SetPosition( TInt aLine, TInt aLineOffset );
        void ChangeDirection( TBidiText::TDirectionality aDirection );
        void SetDirection( TBidiText::TDirectionality aDirection );
        TBidiText::TDirectionality GetDirection();
        void JoinRangedVisuals( TFsRangedVisual* aFirst, TFsRangedVisual* aSecond );
        TInt GetCurrentLineOffset();
        void SetCurrentLineOffset( TInt aOffset );
        TBool IsNewLine();
        
    private:
        void ConstructL();
        CFsTextViewerLayouter( CAlfEnv* aEnv );
        
    private:
        TBool ReorderLineFromLeftL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TSize aObjectSize, TBool aRegisterSelects = ETrue );
        TBool ReorderLineFromRightL( MFsRichTextObject* aObject, CAlfLayout* aLayout, CFsTextParser* aParser, TSize aObjectSize, TBool aRegisterSelects = ETrue );
        
    private:
        CFsTextViewerCoordinator* iHelper;
        CFsTextViewerVisualizerData* iNavigator;
        CWsScreenDevice* iScreenDev;
        CFsRichText* iCFsRichText;
        TBool iSmileyEnabled;
        TSize iDisplaySize;
        CAlfEnv* iEnv;
        RPointerArray<TFsRangedVisual> iCurrentLineArray;
        TInt iCurrentLineOffset;
        TInt iLineHeight;
        TInt iLastCol;
        TInt iLeftMargin;
        TInt iRightMargin;
        TInt iLines;
        TInt iLowestPicPosition;
        TBidiText::TDirectionality iMainDirection;
        TBool iNewLine;
        
    public:
        CFsTextViewerVisualizer* iVisualizer;
        CAlfControl* iControl;
    };

#endif //C_FSTEXTVIEWERLAYOUTER_H
