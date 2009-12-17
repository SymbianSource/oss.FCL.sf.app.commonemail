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
* Description:  Implementation of CFsTextInputVisual class
*
*/

//<cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
//#include <fsconfig.h>     
//#ifdef __FS_ALFRED_SUPPORT
#include "emailtrace.h"
#include <alf/alfcontrol.h>
#include <alf/alfdisplay.h>
/*#else // !__FS_ALFRED_SUPPORT
#include <hitchcock.h>
#endif // __FS_ALFRED_SUPPORT*/
//</cmail> removed __FS_ALFRED_SUPPORT flag (other places in this file not tagged!)
#include <coecntrl.h>

#include "fstextinputvisual.h"

// ======== MEMBER FUNCTIONS ========

void CFsTextInputVisual::ConstructL( CAlfControl& aOwner )
    {
    FUNC_LOG;
    CAlfVisual::ConstructL( aOwner );
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CFsTextInputVisual::CFsTextInputVisual( CAlfControl& aOwner, 
    CCoeControl& aCoeControl ) :
    iCoeControl( aCoeControl )
    {
    FUNC_LOG;
    //No implementation nedeed
    }

// ---------------------------------------------------------------------------
// Constructs and returns an CFsTextInputVisual object. The object is added
// to the aOwnerControl.
// ---------------------------------------------------------------------------
//
CFsTextInputVisual* CFsTextInputVisual::AddNewL( CAlfControl& aOwnerControl,
    CCoeControl& aCoeControl,
    CAlfLayout* aParentLayout )
    {
    FUNC_LOG;
    CFsTextInputVisual* self = 
        new ( ELeave ) CFsTextInputVisual( aOwnerControl, 
                                           aCoeControl );
    CleanupStack::PushL( self );
    self->ConstructL( aOwnerControl );
    
    if ( aParentLayout )
        {
        aOwnerControl.Append( self, aParentLayout );
        }
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFsTextInputVisual::~CFsTextInputVisual()
    {
    FUNC_LOG;
    //No implementation nedeed
    }

// ---------------------------------------------------------------------------
// From CHuiVisual
// Sets rect of the CFsTextInputField and redraws content.
// ---------------------------------------------------------------------------
//
void CFsTextInputVisual::DrawSelf( CAlfGc& /*aGc*/, 
                                   const TRect& aDisplayRect ) const
    {
    FUNC_LOG;
    TRect rect( aDisplayRect );
//<cmail>    
//#ifdef __FS_ALFRED_SUPPORT
// this needs investigation once the FAL is needed again. Currently
// it is not part of the requirements for fs_generic.dll.
//#else // !__FS_ALFRED_SUPPORT
// this needs investigation once the FAL is needed again. Currently
// it is not part of the requirements for fs_generic.dll.
/*    CCoeControl& nativeControl = *( Display()->NativeControl() );
    TPoint p = nativeControl.Position();
    TRect disp_rect( nativeControl.Position(), nativeControl.Size() );
    TSize visual_size = rect.Size();
    rect.Move( p );
    //Check if left corner of text field bounds to the disp width
    if ( rect.iBr.iX > disp_rect.iBr.iX )
        {
        rect.iBr.iX = disp_rect.iBr.iX;
        }
        
    if ( rect.iBr.iY > disp_rect.iBr.iY )
        {
        rect.iBr.iY = disp_rect.iBr.iY;
        }
    
    iCoeControl.SetRect( rect );
    iCoeControl.DrawNow();*/
//#endif // __FS_ALFRED_SUPPORT
//</cmail>
    }

// ---------------------------------------------------------------------------
// Controls visibility of the CFsTextInputField
// ---------------------------------------------------------------------------
//
void CFsTextInputVisual::SetVisibility( TBool aVisibility )
    {
    FUNC_LOG;
    iVisible = aVisibility;
    }

