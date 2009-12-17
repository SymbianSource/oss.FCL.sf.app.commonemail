/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Alf Watermark object
 *
*/



#ifndef C_FSALFWATERMARK_H
#define C_FSALFWATERMARK_H
//<cmail> SF
#include <alf/alfvisual.h>
#include <alf/alftexture.h>
#include <alf/alfdecklayout.h>
//</cmail>

/**
 *  Class used for placing watermark.
 *
 *  @lib fs_generic
 */

NONSHARABLE_CLASS(CFsAlfWatermark) : public CBase
    {
    
public:

    /**
     * Factory method.
     * 
     * @param aParentControl Parent control for watermark layout.
     * @param aParentLayout ParentLayout for watermarkLayout.
     */
    static CFsAlfWatermark* NewL( CAlfControl& aParentControl, 
            CAlfLayout& aParentLayout );

    /**
     * C++ destructor.
     */
    virtual ~CFsAlfWatermark();

    /**
     * Returns the position of the watermark.
     * 
     * @return Reference to the object holding position of watermark.
     */
    TAlfTimedPoint Pos();
    
    /**
     * Returns the size of the watermark.
     * 
     * @return Reference to the object holding size of watermark.
     */
    TAlfTimedPoint Size();
    
    /**
     * Returns opacity of the watermark.
     * 
     * @return Reference to the object holding opacity of watermark.
     */
    TAlfTimedValue& Opacity();
    
    /**
     * Sets the texture for watermark.
     * 
     * @param aTexture Reference to the new watermark texture.
     */
    // <cmail>
    void SetWatermarkTextureL( CAlfTexture& aTexture );
    // </cmail>
    
private:

    /**
     * 2nd phase constructor.
     * 
     * @param aParentControl Parent control for watermark layout.
     * @param aParentLayout ParentLayout for watermarkLayout.
     */
    void ConstructL( CAlfControl& aParentControl, CAlfLayout& aParentLayout );

    /**
     * C++ constructor.
     */
    CFsAlfWatermark();

private:

    /**
     * Pointer to the watermark layout.
     * Own.
     */
    CAlfDeckLayout* iWatermarkLayout;
    
    /**
     * Pointer to the watermark texture.
     * Not own.
     */
    CAlfTexture* iWatermarkTexture;

    };


#endif /*C_FSALFWATERMARK_H*/
