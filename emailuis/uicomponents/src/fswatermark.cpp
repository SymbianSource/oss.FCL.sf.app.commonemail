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
* Description: This file implements class CFsWatermark.
 *
*/


#include "emailtrace.h"
#include "fswatermark.h"
//<cmail> SF
#include <alf/alfbrusharray.h>
#include <alf/alfimage.h>
#include <alf/alfimagebrush.h>
//</cmail>

// ---------------------------------------------------------------------------
// Factory method.
// ---------------------------------------------------------------------------
//
CFsWatermark* CFsWatermark::NewL( CAlfControl& aParentControl, 
            CAlfLayout& aParentLayout )
    {
    FUNC_LOG;
    CFsWatermark* self = new (ELeave) CFsWatermark();
    CleanupStack::PushL(self);
    self->ConstructL(aParentControl, aParentLayout);
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsWatermark::~CFsWatermark()
    {
    FUNC_LOG;
    if ( iWatermarkLayout )
    	{
    	iWatermarkLayout->RemoveAndDestroyAllD();
    	}
    }


// ---------------------------------------------------------------------------
// Gets reference to the position of watermark.
// ---------------------------------------------------------------------------
//
TAlfTimedPoint CFsWatermark::Pos()
    {
    FUNC_LOG;
    return iWatermarkLayout->Pos();
    }
    

// ---------------------------------------------------------------------------
// Sets the target position for watermark.
// ---------------------------------------------------------------------------
//
void CFsWatermark::SetPos(TAlfTimedPoint& aPos)
    {
    FUNC_LOG;
    iWatermarkLayout->SetPos(aPos);
    }


// ---------------------------------------------------------------------------
// Gets reference to the size of watermark.
// ---------------------------------------------------------------------------
//
TAlfTimedPoint CFsWatermark::Size()
    {
    FUNC_LOG;
    return iWatermarkLayout->Size();
    }


// ---------------------------------------------------------------------------
// Sets the target size for watermark.
// ---------------------------------------------------------------------------
//
void CFsWatermark::SetSize(TAlfTimedPoint& aSize)
    {
    FUNC_LOG;
    iWatermarkLayout->SetSize(aSize);
    }
    

// ---------------------------------------------------------------------------
// Gets reference to the opacity of watermark.
// ---------------------------------------------------------------------------
//
const TAlfTimedValue CFsWatermark::Opacity()
    {
    FUNC_LOG;
    return iWatermarkLayout->Opacity();
    }
    

// ---------------------------------------------------------------------------
// Sets the target opacity for watermark.
// ---------------------------------------------------------------------------
//
void CFsWatermark::SetOpacity(TAlfTimedValue& aOpacity)
    {
    FUNC_LOG;
    iWatermarkLayout->SetOpacity(aOpacity);
    }


// ---------------------------------------------------------------------------
// Sets the texture for watermark.
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsWatermark::SetWatermarkTextureL( CAlfTexture& aTexture )
// </cmail>
    {
    FUNC_LOG;
    if (iWatermarkLayout->Brushes()->Count() == 0)
        {
        CAlfImageBrush* brush = CAlfImageBrush::NewL(iWatermarkLayout->Env(),
                TAlfImage(aTexture));
        iWatermarkLayout->Brushes()->AppendL(brush, EAlfHasOwnership);
        }
    else
        {
        CAlfImageBrush& brush = 
            static_cast<CAlfImageBrush&>(iWatermarkLayout->Brushes()->At(0));
        brush.SetImage(TAlfImage(aTexture));
        }
    }
    

// ---------------------------------------------------------------------------
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CFsWatermark::ConstructL( CAlfControl& aParentControl, CAlfLayout& aParentLayout )
    {
    FUNC_LOG;
    iWatermarkLayout = CAlfDeckLayout::AddNewL(aParentControl,&aParentLayout);
    aParentLayout.MoveVisualToBack( *iWatermarkLayout );
    iWatermarkLayout->SetFlag(EAlfVisualFlagManualLayout);
    iWatermarkLayout->EnableBrushesL();
    }


// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsWatermark::CFsWatermark()
    :iWatermarkLayout(NULL),
    iWatermarkTexture(NULL)
    {
    FUNC_LOG;
    
    }

