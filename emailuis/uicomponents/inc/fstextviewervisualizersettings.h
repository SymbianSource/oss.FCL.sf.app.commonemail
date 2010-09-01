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
* Description: This file defines classes TFsPartBg, CFsTextViewerVisualizerSettings
*
*/


#ifndef C_FSTEXTVIEWERVISUALIZERSETTINGS_H
#define C_FSTEXTVIEWERVISUALIZERSETTINGS_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>
// <cmail> SF
#include <alf/alftimedvalue.h>
// </cmail>
class CAlfBrush;
class CAlfTexture;
class CFsTextViewerWatermark;
class CFsTextViewerKeys;

class TFsPartBg
    {
    public:
        TInt iStart;
        TInt iEnd;
        CAlfBrush* iBrush;
    };

class CFsTextViewerVisualizerSettings : public CBase
    {
    public:
        CFsTextViewerVisualizerSettings();
        ~CFsTextViewerVisualizerSettings();

    public:
        TInt iScrollingOffset;
        TInt iScrollingDelay;
        TInt iCursorDelay;
        TInt iExpandDelay;
        TInt iRightMargin;
        TInt iLeftMargin;
        TInt iScrollbarWidth;

        TBool iScrollByCursor;
        TBool iSmileyEnabled;
        TBool iMirroring;
        TBool iWatermarkSet;
        TBool iBackgroundColorSet;
        TBool iBackgroundImageSet;
        TBool iKeysSet;
        TBool iPictureSizeSet;

        CFsTextViewerWatermark* iWatermark;
        CAlfBrush* iBackgroundBrush;
        
        TSize iSmileySize;
        TSize iPicSize;
        
        CFsTextViewerKeys* iKeys;

        RArray<TFsPartBg> iPartBgs;

        CAlfTexture* iHighlightLine;
        TReal32 iHighlightLineOpacity;
        CAlfTexture* iActionButton;
        TReal32 iActionButtonOpacity;
        TReal32 iActionButtonMargin;
        TReal32 iActionButtonWidth;
        TReal32 iActionButtonHeight;

        TAlfTimedValue iBackgroundOpacity;
    };

#endif //C_FSTEXTVIEWERVISUALIZERSETTINGS_H
