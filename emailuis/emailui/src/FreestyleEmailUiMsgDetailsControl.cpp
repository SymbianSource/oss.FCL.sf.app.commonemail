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
#include "FreestyleEmailUiMsgDetailsControl.h"
#include "FreestyleEmailUiMsgDetailsVisualiser.h"


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMsgDetailsControl* CFSEmailUiMsgDetailsControl::NewL(CAlfEnv& aEnv, CFSEmailUiMsgDetailsVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFSEmailUiMsgDetailsControl* self = CFSEmailUiMsgDetailsControl::NewLC(aEnv, aVisualiser);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMsgDetailsControl* CFSEmailUiMsgDetailsControl::NewLC(CAlfEnv& aEnv, CFSEmailUiMsgDetailsVisualiser& aVisualiser)
    {
    FUNC_LOG;
    CFSEmailUiMsgDetailsControl* self = new (ELeave) CFSEmailUiMsgDetailsControl(aVisualiser);
    CleanupStack::PushL(self);
    self->ConstructL(aEnv);
    return self;
    }

// ---------------------------------------------------------------------------
//C++ constructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMsgDetailsControl::CFSEmailUiMsgDetailsControl(CFSEmailUiMsgDetailsVisualiser& aVisualiser)
    : CAlfControl(), 
	iVisualiser(aVisualiser)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsControl::ConstructL(CAlfEnv& aEnv)
    {
    FUNC_LOG;
    CAlfControl::ConstructL(aEnv);    
    }

// ---------------------------------------------------------------------------
// Virtual destructor.
// ---------------------------------------------------------------------------
//
CFSEmailUiMsgDetailsControl::~CFSEmailUiMsgDetailsControl()
    {
    FUNC_LOG;
    }


// ---------------------------------------------------------------------------
// Event handling
// ---------------------------------------------------------------------------
//
TBool CFSEmailUiMsgDetailsControl::OfferEventL( const TAlfEvent& aEvent )
	{
    FUNC_LOG;
	// Offer events to view
	return iVisualiser.OfferEventL( aEvent );
	}

// ---------------------------------------------------------------------------
// Command handling
// ---------------------------------------------------------------------------
//
void CFSEmailUiMsgDetailsControl::HandleCommandL(TInt aCommand)
	{
    FUNC_LOG;
	// Offer aCommand to view
	return iVisualiser.HandleCommandL( aCommand );
	}

