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
* Description:  Implements top control class for email viewer
*
*/



#include "emailtrace.h"
#include "FreestyleEmailUiMailViewerControl.h"
#include "FreestyleEmailUiMailViewerVisualiser.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailViewerControl* CFreestyleEmailUiMailViewerControl::NewL(CAlfEnv& aEnv, CFSEmailUiMailViewerVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiMailViewerControl* self = CFreestyleEmailUiMailViewerControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailViewerControl* CFreestyleEmailUiMailViewerControl::NewLC(CAlfEnv& aEnv, CFSEmailUiMailViewerVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiMailViewerControl* self = new (ELeave) CFreestyleEmailUiMailViewerControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailViewerControl::CFreestyleEmailUiMailViewerControl(CFSEmailUiMailViewerVisualiser& aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiMailViewerControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);    
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiMailViewerControl::~CFreestyleEmailUiMailViewerControl()
    {
    FUNC_LOG;
    }



// ---------------------------------------------------------------------------
// Forwards key events for visualiser to handle
// ---------------------------------------------------------------------------
//
TBool CFreestyleEmailUiMailViewerControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	return iVisualiser.OfferEventL( aEvent );
	}

// ---------------------------------------------------------------------------
// Forwards aCommand to the visualiser to handle
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiMailViewerControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	iVisualiser.HandleCommandL( aCommand );
	}
		

