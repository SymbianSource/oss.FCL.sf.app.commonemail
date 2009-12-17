/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CNCSEDITORCUSTOMDRAW_H
#define CNCSEDITORCUSTOMDRAW_H

#include <lafmain.h>
#include <aknlayoutscalable_apps.cdl.h>
#include "ncsconstants.h"

class CNcsEditor;

/**
* Class which handles mail editor row lines to "to" "cc" and "subject" fields.
*/
class CNcsEditorCustomDraw: public CBase,
                            public MFormCustomDraw
    {
public: // constructor and destructor

    static CNcsEditorCustomDraw* NewL( const MFormCustomDraw* aParentCustomDraw,
                                 const CNcsEditor* aParentControl,
                                 TAknTextComponentLayout aLayout );
    
    virtual ~CNcsEditorCustomDraw();

public: // from CLafEdwinCustomDrawBase

	virtual void DrawBackground( const TParam& aParam,
                                 const TRgb& aBackground,
                                 TRect& aDrawn) const;
	
    virtual void DrawLineGraphics( const TParam& aParam,
                                   const TLineInfo& aLineInfo ) const;
	
    virtual void DrawText( const TParam& aParam,
                           const TLineInfo& aLineInfo,
                           const TCharFormat& aFormat,
                           const TDesC& aText,
                           const TPoint& aTextOrigin,
                           TInt aExtraPixels) const;

    virtual TRgb SystemColor( TUint aColorIndex,
                              TRgb aDefaultColor) const;

    void UpdateLayout( TAknTextComponentLayout aLayout );
    
private: // methods used internally

    /**
    * C++ contructor
    */
    CNcsEditorCustomDraw( const MFormCustomDraw* aParentCustomDraw,
                          const CNcsEditor* aParentControl,
                          const TAknTextComponentLayout aLayout );
                                 
    /**
    * 2nd phase constructor
    */
    void ConstructL();
    
private:    
    /**
     * default custom drawer.
     * Not owned.
     */
    const MFormCustomDraw* iCustomDrawer;

    /**
     * Parent control.
     * Not owned. 
     */
    const CNcsEditor* iEditor;
    
    TAknTextComponentLayout iTextPaneLayout;
    };

#endif //CNCSEDITORCUSTOMDRAW_H
