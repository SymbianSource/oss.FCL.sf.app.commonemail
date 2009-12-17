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
* Description:  Implements control for the attachment list view
*
*/



#include "emailtrace.h"
#include "FreestyleEmailUiAttachmentsListControl.h"
#include "FreestyleEmailUiAttachmentsListVisualiser.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAttachmentsListControl* CFreestyleEmailUiAttachmentsListControl::NewL(CAlfEnv& aEnv, CFSEmailUiAttachmentsListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiAttachmentsListControl* self = CFreestyleEmailUiAttachmentsListControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAttachmentsListControl* CFreestyleEmailUiAttachmentsListControl::NewLC(CAlfEnv& aEnv, CFSEmailUiAttachmentsListVisualiser* aVisualiser)
    {
    FUNC_LOG;
    CFreestyleEmailUiAttachmentsListControl* self = new (ELeave) CFreestyleEmailUiAttachmentsListControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAttachmentsListControl::CFreestyleEmailUiAttachmentsListControl(CFSEmailUiAttachmentsListVisualiser* aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiAttachmentsListControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFreestyleEmailUiAttachmentsListControl::~CFreestyleEmailUiAttachmentsListControl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFreestyleEmailUiAttachmentsListControl::TextureLoadingCompleted( CAlfTexture& /* aTexture*/,
                                                         TInt /*aTextureId*/,
                                                         TInt /*aErrorCode*/ )
    {
    FUNC_LOG;
    
    }


TBool CFreestyleEmailUiAttachmentsListControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer events to view
		{
		return iVisualiser->OfferEventL( aEvent );
		}
	return KErrGeneral;
	}

void CFreestyleEmailUiAttachmentsListControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	if ( iVisualiser ) // Offer aCommand to view
		{
		return iVisualiser->HandleCommandL( aCommand );
		}
	}
		

