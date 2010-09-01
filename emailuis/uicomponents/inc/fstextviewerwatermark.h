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


#ifndef C_FSTEXTVIEWERWATERMARK_H
#define C_FSTEXTVIEWERWATERMARK_H

//<cmail> removed __FS_ALFRED_SUPPORT flag
//#include <fsconfig.h>
//</cmail> removed __FS_ALFRED_SUPPORT flag
#include <e32base.h>

class CAlfTexture;

class CFsTextViewerWatermark : public CBase
    {
    public:
        IMPORT_C static CFsTextViewerWatermark* NewL ( 
                CAlfTexture& aTexture, 
                TReal32 aOpacity = 0.33f );
        IMPORT_C ~CFsTextViewerWatermark();
        IMPORT_C void SetRect( TRect aRect );
        IMPORT_C TRect Rect();
        CAlfTexture& Texture();
        TReal32 Opacity();

    private:
        CFsTextViewerWatermark( 
                CAlfTexture* aTexture, 
                TReal32 aOpacity = 0.33f );
        void ConstructL();

    private:
        CAlfTexture* iTexture;
        TReal32 iOpacity;
        TRect iRect;
    };

#endif //C_FSTEXTVIEWERWATERMARK_H
