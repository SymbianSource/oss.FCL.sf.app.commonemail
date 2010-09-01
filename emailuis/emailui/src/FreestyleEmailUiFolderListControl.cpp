/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements control for the application.
*
*/

#include "emailtrace.h"
#include <AknUtils.h>
// <cmail> SF
#include <alf/alfevent.h>
// </cmail>
#include "FreestyleEmailUiFolderListControl.h"
#include "FreestyleEmailUiFolderListVisualiser.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListControl* CFSEmailUiFolderListControl::NewL(CAlfEnv& aEnv, CFSEmailUiFolderListVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFSEmailUiFolderListControl* self = CFSEmailUiFolderListControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListControl* CFSEmailUiFolderListControl::NewLC(CAlfEnv& aEnv, CFSEmailUiFolderListVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFSEmailUiFolderListControl* self = new (ELeave) CFSEmailUiFolderListControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListControl::CFSEmailUiFolderListControl(CFSEmailUiFolderListVisualiser& aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiFolderListControl::~CFSEmailUiFolderListControl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListControl::OfferEventL
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiFolderListControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	// Handle expand/collapse functionality, but only in full screen mode
	if ( aEvent.IsKeyEvent() && iVisualiser.IsFullScreen() )
		{
		// Recalculate the horizontal position when scrolling is finished
		if ( aEvent.Code() == EEventKeyUp &&
		    ( aEvent.KeyEvent().iScanCode == EStdKeyUpArrow ||
		      aEvent.KeyEvent().iScanCode == EStdKeyDownArrow ) )
			{
            iVisualiser.DoHorizontalScrollL();
			}

		// Check expanding an collapsing
		else if ( aEvent.Code() == EEventKey )
		    {
            // Swap left and right with mirrored layout
            TInt scanCode = aEvent.KeyEvent().iScanCode;
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                if ( scanCode == EStdKeyLeftArrow ) scanCode = EStdKeyRightArrow;
                else if ( scanCode == EStdKeyRightArrow ) scanCode = EStdKeyLeftArrow;
                }
            
            // Left click collapses the current node, right click expands it
            switch ( scanCode )
                {
                case EStdKeyLeftArrow:
                    {
                    iVisualiser.ExpandOrCollapseL(
                            CFSEmailUiFolderListVisualiser::EFolderListCollapse );
                    }
                    break;
    
                case EStdKeyRightArrow:
                    {
                    iVisualiser.ExpandOrCollapseL(
                            CFSEmailUiFolderListVisualiser::EFolderListExpand );
                    }
                    break;
                    
                default:
                    break;
                }
		    }
		}

// <cmail> Touch
	// Offer events to view
	
	TBool result = EFalse;
	
    if ( aEvent.IsPointerEvent() )
        {
        
        result = iVisualiser.HandlePointerEventL( aEvent ); 
        }
    else
        {
        result = iVisualiser.OfferEventL( aEvent );
        }
    return result;
// </cmail>
	}

// ---------------------------------------------------------------------------
// CFSEmailUiFolderListControl::HandleCommandL
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListControl::HandleCommandL( TInt aCommand )
	{
    FUNC_LOG;
	// Offer aCommand to view
	return iVisualiser.HandleCommandL( aCommand );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CFSEmailUiFolderListControl::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
	{
    FUNC_LOG;
	iVisualiser.ExpandOrCollapseL(
	        CFSEmailUiFolderListVisualiser::EFolderListAutomatic );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
/*
void CFSEmailUiFolderListControl::StartTestTimer()
	{
	iFolderListTimer.Start( KAutoExpandCollapseDelay * 5 );
	}
*/

