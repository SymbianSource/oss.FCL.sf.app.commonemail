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
* Description: This file defines class CFsWatermark.
 *
*/



#ifndef C_FSWATERMARK_H
#define C_FSWATERMARK_H
//<cmail> SF
#include <alf/alfdecklayout.h>
#include <alf/alfcontrol.h>
#include <alf/alftexture.h>
//</cmail>

/**
 *  Class used for placing watermark.
 *
 *  @lib fs_generic
 */
NONSHARABLE_CLASS(CFsWatermark) : public CBase
    {
    
public:

    /**
     * Factory method.
     * 
     * @param aParentControl Parent control for watermark layout.
     * @param aParentLayout Parent layout for watermarkLayout.
     */
    static CFsWatermark* NewL( CAlfControl& aParentControl, 
            CAlfLayout& aParentLayout );

    /**
     * C++ destructor.
     */
    virtual ~CFsWatermark();

    /**
     * Returns the position of the watermark.
     * 
     * @return Reference to the object holding position of watermark.
     */
    TAlfTimedPoint Pos();
    
    /**
     * Sets the position of the watermark.
     * 
     * @param aPos Target position for watermark.
     */
    void SetPos(TAlfTimedPoint& aPos);
    
    /**
     * Returns the size of the watermark.
     * 
     * @return Reference to the object holding size of watermark.
     */
    TAlfTimedPoint Size();
    
    /**
     * Sets the size of the watermark.
     * 
     * @param aPos Target size for watermark.
     */
    void SetSize(TAlfTimedPoint& aSize);
    
    /**
     * Returns opacity of the watermark.
     * 
     * @return Reference to the object holding opacity of watermark.
     */
    const TAlfTimedValue Opacity();
    
    /**
     * Sets the opacity of the watermark.
     * 
     * @param aOpacity Target opacity for watermark.
     */
    void SetOpacity(TAlfTimedValue& aOpacity);
    
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
    CFsWatermark();

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


#endif /*C_FSWATERMARK_H*/
