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
#include "FreestyleEmailUiSearchListControl.h"
#include "FreestyleEmailUiSearchListVisualiser.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSearchListControl* CFreestyleEmailUiSearchListControl::NewL(CAlfEnv& aEnv, CFSEmailUiSearchListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiSearchListControl* self = CFreestyleEmailUiSearchListControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSearchListControl* CFreestyleEmailUiSearchListControl::NewLC(CAlfEnv& aEnv, CFSEmailUiSearchListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiSearchListControl* self = new (ELeave) CFreestyleEmailUiSearchListControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSearchListControl::CFreestyleEmailUiSearchListControl(CFSEmailUiSearchListVisualiser* aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSearchListControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);    
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiSearchListControl::~CFreestyleEmailUiSearchListControl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiSearchListControl::TextureLoadingCompleted( CAlfTexture& /* aTexture*/,
                                                         TInt /*aTextureId*/,
                                                         TInt /*aErrorCode*/ )
    {
    FUNC_LOG;
    
    }


TBool CFreestyleEmailUiSearchListControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer events to view
		{
		return iVisualiser->OfferEventL( aEvent );
		}
	return KErrGeneral;
	}

void CFreestyleEmailUiSearchListControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer aCommand to view
		{
		return iVisualiser->HandleCommandL( aCommand );
		}
	}
		

