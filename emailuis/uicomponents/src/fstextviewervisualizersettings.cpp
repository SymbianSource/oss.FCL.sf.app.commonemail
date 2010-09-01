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
* Description: This file implements class CFsTextViewerVisualizerSettings.
*
*/


#include "emailtrace.h"
#include "fstextviewervisualizersettings.h"

// ---------------------------------------------------------------------------
// CFsTextViewerVisualizerSettings
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerSettings::CFsTextViewerVisualizerSettings()
    {
    FUNC_LOG;
    iScrollingOffset = 30;
    iCursorDelay = 300;
    iScrollingDelay = 500;
    iExpandDelay = 1000;
    iRightMargin = 0;
    iLeftMargin = 0;
    
    iSmileyEnabled = ETrue;
    iScrollByCursor = EFalse;
    iWatermarkSet = EFalse;
    iBackgroundColorSet = EFalse;
    iBackgroundImageSet = EFalse;
    iKeysSet = EFalse;
    iPictureSizeSet = EFalse;
    iMirroring = EFalse;

    iActionButtonMargin = 0.01;
    iBackgroundOpacity = TAlfTimedValue( 1 );
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerVisualizerSettings
// ---------------------------------------------------------------------------
CFsTextViewerVisualizerSettings::~CFsTextViewerVisualizerSettings()
    {
    FUNC_LOG;
    iPartBgs.Close();
    }

