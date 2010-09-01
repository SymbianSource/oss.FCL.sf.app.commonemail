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
* Description:  Custom draw component for message editor
*
*/


#ifndef CNCSCUSTOMDRAW_H
#define CNCSCUSTOMDRAW_H

#include <lafmain.h>

class CNcsEditor;

/**
* Class which handles setting editor background color, selected text color,
* and selected text background color.
*
* Text color setting is done by editor itself.
*/
class CNcsCustomDraw : public CLafEdwinCustomDrawBase
    {
public: // constructor and destructor

    CNcsCustomDraw( 
        const MLafEnv& aEnv,
        const CCoeControl& aControl,
        CLafEdwinCustomDrawBase* aCustomDrawer,
        CNcsEditor* aEditor,
        TBool aHeaderField );
    
    virtual ~CNcsCustomDraw();

public: // new methods

    void UpdateColors();

public: // from CLafEdwinCustomDrawBase

	virtual void DrawBackground(
        const TParam& aParam,
        const TRgb& aBackground,
        TRect& aDrawn) const;
	
    virtual void DrawLineGraphics(
        const TParam& aParam,
        const TLineInfo& aLineInfo ) const;
	
    virtual void DrawText(
        const TParam& aParam,
        const TLineInfo& aLineInfo,
        const TCharFormat& aFormat,
        const TDesC& aText,
        const TPoint& aTextOrigin,
        TInt aExtraPixels) const;

    virtual TRgb SystemColor(
        TUint aColorIndex,
        TRgb aDefaultColor) const;

private: // methods used internally

    enum TColors
        {
        EColorBackground = 0x1,
        EColorSelectionText = 0x2,
        EColorSelectionBackground = 0x4
        };

    void SetHasColor( TColors aColor );

    TBool HasColor( TColors aColor ) const;

private:

    TInt iColors;

    CLafEdwinCustomDrawBase* iCustomDrawer;
    CNcsEditor* iEditor;
    TBool iHeaderField;

    TRgb iBackgroundColor;
    TRgb iSelectionTextColor;
    TRgb iSelectionBackgroundColor;

    };

#endif
