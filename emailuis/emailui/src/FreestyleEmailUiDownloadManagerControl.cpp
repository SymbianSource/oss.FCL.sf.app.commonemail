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
* Description:  Implements control for the download manager view
*
*/



#include "emailtrace.h"
#include "FreestyleEmailUiDownloadManagerControl.h"
#include "FreestyleEmailUiDownloadManagerVisualiser.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiDownloadManagerControl* CFreestyleEmailUiDownloadManagerControl::NewL(CAlfEnv& aEnv, CFSEmailUiDownloadManagerVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiDownloadManagerControl* self = CFreestyleEmailUiDownloadManagerControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiDownloadManagerControl* CFreestyleEmailUiDownloadManagerControl::NewLC(CAlfEnv& aEnv, CFSEmailUiDownloadManagerVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiDownloadManagerControl* self = new (ELeave) CFreestyleEmailUiDownloadManagerControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiDownloadManagerControl::CFreestyleEmailUiDownloadManagerControl(CFSEmailUiDownloadManagerVisualiser* aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiDownloadManagerControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiDownloadManagerControl::~CFreestyleEmailUiDownloadManagerControl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiDownloadManagerControl::TextureLoadingCompleted( CAlfTexture& /* aTexture*/,
                                                         TInt /*aTextureId*/,
                                                         TInt /*aErrorCode*/ )
    {
    FUNC_LOG;
    
    }


TBool CFreestyleEmailUiDownloadManagerControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer events to view
		{
		return iVisualiser->OfferEventL( aEvent );
		}
	return KErrGeneral;
	}

void CFreestyleEmailUiDownloadManagerControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer aCommand to view
		{
		return iVisualiser->HandleCommandL( aCommand );
		}
	}
		

