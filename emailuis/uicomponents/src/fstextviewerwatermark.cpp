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
* Description:  Watermark texture storage class
*
*/


#include "emailtrace.h"
#include "fstextviewerwatermark.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewerWatermark* CFsTextViewerWatermark::NewL ( 
                                CAlfTexture& aTexture, TReal32 aOpacity )
    {
    FUNC_LOG;
    CFsTextViewerWatermark* self = new ( ELeave ) 
                                CFsTextViewerWatermark( &aTexture, aOpacity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerWatermark
// ---------------------------------------------------------------------------
EXPORT_C CFsTextViewerWatermark::~CFsTextViewerWatermark()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// SetRect
// ---------------------------------------------------------------------------
EXPORT_C void CFsTextViewerWatermark::SetRect( TRect aRect )
    {
    FUNC_LOG;
    iRect = aRect;
    }

// ---------------------------------------------------------------------------
// Rect
// ---------------------------------------------------------------------------
EXPORT_C TRect CFsTextViewerWatermark::Rect()
    {
    FUNC_LOG;
    return iRect;
    }

// ---------------------------------------------------------------------------
// Texture
// ---------------------------------------------------------------------------
CAlfTexture& CFsTextViewerWatermark::Texture()
    {
    FUNC_LOG;
    return *iTexture;
    }

// ---------------------------------------------------------------------------
// Opacity
// ---------------------------------------------------------------------------
TReal32 CFsTextViewerWatermark::Opacity()
    {
    FUNC_LOG;
    return iOpacity;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerWatermark
// ---------------------------------------------------------------------------
CFsTextViewerWatermark::CFsTextViewerWatermark( 
        CAlfTexture* aTexture, 
        TReal32 aOpacity ) :
    iTexture( aTexture ),
    iOpacity( aOpacity )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerWatermark::ConstructL()
    {
    FUNC_LOG;
    }

