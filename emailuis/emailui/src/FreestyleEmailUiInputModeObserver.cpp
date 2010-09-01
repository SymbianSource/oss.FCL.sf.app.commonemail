/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Input mode observer class implementation
*
*/



#include "emailtrace.h"
#include <e32cmn.h>								// TUid
#include <AknFepInternalCRKeys.h>				// KCRUidAknFep, KAknFepPredTxtFlag
#include <centralrepository.h>					// CRepository
#include "FSEmailBuildFlags.h"
#include "FreestyleEmailUiInputModeObserver.h"	// CFSEmailUiInputModeObserver
#include "FreestyleEmailUiCLSListsHandler.h"	// CFSEmailUiClsListsHandler

#include <CPcsDefs.h>							// PCS TKeyboardModes enum

_LIT( KErrorMessage, "Input mode observer: wrong callback method.");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFSEmailUiInputModeObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFSEmailUiInputModeObserver* CFSEmailUiInputModeObserver::NewL( CRepository& aCr, CFSEmailUiClsListsHandler& aListHandler )
	{
    FUNC_LOG;
	CFSEmailUiInputModeObserver* self = new ( ELeave ) CFSEmailUiInputModeObserver( aCr, aListHandler );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CFSEmailUiInputModeObserver::~CFSEmailUiInputModeObserver
// Destructor.
// -----------------------------------------------------------------------------
//
CFSEmailUiInputModeObserver::~CFSEmailUiInputModeObserver()
	{
    FUNC_LOG;
	if( iCenRepNotifyWrapper )
		{
		iCenRepNotifyWrapper->StopListening();
		delete iCenRepNotifyWrapper;
		}
	}

// -----------------------------------------------------------------------------
// CFSEmailUiInputModeObserver::CFSEmailUiInputModeObserver
// Constructor.
// -----------------------------------------------------------------------------
//
CFSEmailUiInputModeObserver::CFSEmailUiInputModeObserver( CRepository& aCr, CFSEmailUiClsListsHandler& aListHandler ) :
	iAkvkFepCenRep( aCr ), iClsListHandler( aListHandler )
	{
    FUNC_LOG;
	// Nothing
	}


// -----------------------------------------------------------------------------
// CFSEmailUiInputModeObserver::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CFSEmailUiInputModeObserver::ConstructL()
	{
    FUNC_LOG;
	iCenRepNotifyWrapper = CCenRepNotifyHandler::NewL( *this, iAkvkFepCenRep, 
								CCenRepNotifyHandler::EIntKey, KAknFepPredTxtFlag );
	iCenRepNotifyWrapper->StartListeningL();
	}

// callback methods for CenRepNotifyHandler
void CFSEmailUiInputModeObserver::HandleNotifyInt(
	TUint32 aId, TInt aNewValue )
	{
    FUNC_LOG;
	// set new input mode for matchers through list handler
	if ( aId == KAknFepPredTxtFlag )
		{
		TKeyboardModes inputMode = EQwerty;		
		if ( aNewValue )
			{
			inputMode = EItut;
			}

		TRAP_IGNORE( iClsListHandler.InputModeChangedL( inputMode ) );
		}
	}
void CFSEmailUiInputModeObserver::HandleNotifyReal( TUint32 /*aId*/, TReal /*aNewValue*/ )
	{
    FUNC_LOG;
	User::Panic( KErrorMessage, KErrNotSupported );	// replace error message
	}
void CFSEmailUiInputModeObserver::HandleNotifyString( TUint32 /*aId*/, const TDesC16& /*aNewValue*/ )
	{
    FUNC_LOG;
	User::Panic( KErrorMessage, KErrNotSupported );	// replace error message
	}
void CFSEmailUiInputModeObserver::HandleNotifyBinary( TUint32 /*id*/, const TDesC8& /*aNewValue*/ )
	{
    FUNC_LOG;
	User::Panic( KErrorMessage, KErrNotSupported  );	// replace error message
	}
void CFSEmailUiInputModeObserver::HandleNotifyGeneric( TUint32 /*aId*/ )
	{
    FUNC_LOG;
	User::Panic( KErrorMessage, KErrNotSupported  );	// replace error message
	}
void CFSEmailUiInputModeObserver::HandleNotifyError( TUint32 /*aId*/, TInt aError, CCenRepNotifyHandler* /*aHandler*/ )
	{
    FUNC_LOG;
	User::Panic( KErrorMessage, aError  );	// replace error message
	}



// End of file

