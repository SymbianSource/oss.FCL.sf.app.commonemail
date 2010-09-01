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
* Description:  Implements control for the application.
*
*/



#include "emailtrace.h"
#include "FreestyleEmailUiMailListControl.h"
#include "FreestyleEmailUiMailListVisualiser.h"

// <cmail> Needed for pointer events. "As a touch device user I can focus an item in inbox list view."
#include <alf/alfevent.h>
// </cmail>



// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailListControl* CFreestyleEmailUiMailListControl::NewL(CAlfEnv& aEnv, CFSEmailUiMailListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiMailListControl* self = CFreestyleEmailUiMailListControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailListControl* CFreestyleEmailUiMailListControl::NewLC(CAlfEnv& aEnv, CFSEmailUiMailListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiMailListControl* self = new (ELeave) CFreestyleEmailUiMailListControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailListControl::CFreestyleEmailUiMailListControl(CFSEmailUiMailListVisualiser* aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiMailListControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);    
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailListControl::~CFreestyleEmailUiMailListControl()
    {
    FUNC_LOG;
    }


// <cmail> Touch
// ---------------------------------------------------------------------------
// Called when control is displayed or hidden
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiMailListControl::NotifyControlVisibility( TBool  aIsVisible, CAlfDisplay& /*aDisplay*/ )
    {
    if ( iVisualiser )
        {
        if (aIsVisible)
            {
            iVisualiser->DisableMailList(EFalse);
            }
        else
            {
            iVisualiser->DisableMailList(ETrue);
            }
        }
    }
// </cmail>

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiMailListControl::TextureLoadingCompleted( CAlfTexture& /* aTexture*/,
                                                         TInt /*aTextureId*/,
                                                         TInt /*aErrorCode*/ )
    {
    FUNC_LOG;
    
    }


TBool CFreestyleEmailUiMailListControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	// <cmail> "As a touch device user I can focus an item in inbox list view."
	
    TBool result(EFalse); 
	if ( iVisualiser ) // Offer events to view
		{
        result = iVisualiser->OfferEventL( aEvent );
		}
	else
	    {
	    result = KErrGeneral; 
	    }
	return result;
    
	// </cmail>
	}

void CFreestyleEmailUiMailListControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer aCommand to view
		{
		return iVisualiser->HandleCommandL( aCommand );
		}
	}
		

