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


//<cmail> SF
#include "emailtrace.h"
#include <alf/alfimage.h>
#include <alf/alfimagebrush.h>
#include <alf/alfbrusharray.h>
//</cmail>
#include "fsalfwatermark.h"

// ---------------------------------------------------------------------------
// Factory method.
// ---------------------------------------------------------------------------
//
CFsAlfWatermark* CFsAlfWatermark::NewL( CAlfControl& aParentControl, 
            CAlfLayout& aParentLayout )
    {
    FUNC_LOG;
    CFsAlfWatermark* self = new (ELeave) CFsAlfWatermark();
    CleanupStack::PushL(self);
    self->ConstructL(aParentControl, aParentLayout);
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CFsAlfWatermark::~CFsAlfWatermark()
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
TAlfTimedPoint CFsAlfWatermark::Pos()
    {
    FUNC_LOG;
    return iWatermarkLayout->Pos();
    }
    

// ---------------------------------------------------------------------------
// Gets reference to the size of watermark.
// ---------------------------------------------------------------------------
//
TAlfTimedPoint CFsAlfWatermark::Size()
    {
    FUNC_LOG;
    return iWatermarkLayout->Size();
    }
    

// ---------------------------------------------------------------------------
// Gets reference to the opacity of watermark.
// ---------------------------------------------------------------------------
//
TAlfTimedValue& CFsAlfWatermark::Opacity()
    {
    FUNC_LOG;
    return const_cast<TAlfTimedValue&>(iWatermarkLayout->Opacity());
    }
    

// ---------------------------------------------------------------------------
// Sets the texture for watermark.
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsAlfWatermark::SetWatermarkTextureL( CAlfTexture& aTexture )
// </cmail>
    {
    FUNC_LOG;

    if ( iWatermarkLayout->Brushes()->Count() == 0 )
        {
    	CAlfImageBrush* brush = CAlfImageBrush::NewL(iWatermarkLayout->Env(),TAlfImage(aTexture));
    	iWatermarkLayout->Brushes()->AppendL(brush,EAlfHasOwnership);
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
void CFsAlfWatermark::ConstructL( CAlfControl& aParentControl, 
        CAlfLayout& aParentLayout )
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
CFsAlfWatermark::CFsAlfWatermark()
    :iWatermarkLayout(NULL),
    iWatermarkTexture(NULL)
    {
    FUNC_LOG;
    
    }

