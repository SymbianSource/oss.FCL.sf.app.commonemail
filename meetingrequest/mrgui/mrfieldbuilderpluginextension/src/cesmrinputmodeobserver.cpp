/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "cesmrinputmodeobserver.h"

#include "cesmrclslistshandler.h"

#include <e32cmn.h>                             // TUid
#include <AknFepInternalCRKeys.h>               // KCRUidAknFep, KAknFepPredTxtFlag
#include <centralrepository.h>                  // CRepository

#include <CPcsDefs.h>                           // PCS TKeyboardModes enum
#include <eikenv.h>

// Unnamed namespace for local definitions
namespace{

_LIT( KErrorMessage, "Input mode observer: wrong callback method.");

}//namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CESMRInputModeObserver* CESMRInputModeObserver::NewL( CRepository& aCr, CESMRClsListsHandler& aListHandler )
    {
    FUNC_LOG;
    CESMRInputModeObserver* self = new ( ELeave ) CESMRInputModeObserver( aCr, aListHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::~CESMRInputModeObserver
// Destructor.
// -----------------------------------------------------------------------------
//
CESMRInputModeObserver::~CESMRInputModeObserver()
    {
    FUNC_LOG;
    if( iCenRepNotifyWrapper )
        {
        iCenRepNotifyWrapper->StopListening();
        delete iCenRepNotifyWrapper;
        }
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::CESMRInputModeObserver
// Constructor.
// -----------------------------------------------------------------------------
//
CESMRInputModeObserver::CESMRInputModeObserver( CRepository& aCr, CESMRClsListsHandler& aListHandler ) :
    iAkvkFepCenRep( aCr ), iClsListHandler( aListHandler )
    {
    FUNC_LOG;
    // Nothing
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::ConstructL()
    {
    FUNC_LOG;
    iCenRepNotifyWrapper = CCenRepNotifyHandler::NewL( *this, iAkvkFepCenRep,
                                CCenRepNotifyHandler::EIntKey, KAknFepPredTxtFlag );
    iCenRepNotifyWrapper->StartListeningL();
    }

// callback methods for CenRepNotifyHandler
void CESMRInputModeObserver::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    FUNC_LOG;
    // set new input mode for matchers through list handler
    if( aId == KAknFepPredTxtFlag )
        {
        TKeyboardModes inputMode = EQwerty;
        if( aNewValue )
            {
            inputMode = EItut;
            }

        TRAPD( error, iClsListHandler.InputModeChangedL( inputMode ) );
        if ( error != KErrNone )
            {
            CEikonEnv::Static()-> // codescanner::eikonenvstatic
                HandleError(error);
            }
        }
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::HandleNotifyReal
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::HandleNotifyReal( TUint32 /*aId*/, TReal /*aNewValue*/ )
    {
    FUNC_LOG;
    User::Panic( KErrorMessage, KErrNotSupported ); 
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::HandleNotifyString
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::HandleNotifyString( TUint32 /*aId*/, const TDesC16& /*aNewValue*/ )
    {
    FUNC_LOG;
    User::Panic( KErrorMessage, KErrNotSupported ); 
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::HandleNotifyBinary
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::HandleNotifyBinary( TUint32 /*id*/, const TDesC8& /*aNewValue*/ )
    {
    FUNC_LOG;
    User::Panic( KErrorMessage, KErrNotSupported  );  
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::HandleNotifyGeneric
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::HandleNotifyGeneric( TUint32 /*aId*/ )
    {
    FUNC_LOG;
    User::Panic( KErrorMessage, KErrNotSupported  );  
    }

// -----------------------------------------------------------------------------
// CESMRInputModeObserver::HandleNotifyError
// -----------------------------------------------------------------------------
//
void CESMRInputModeObserver::HandleNotifyError( TUint32 /*aId*/, TInt aError, CCenRepNotifyHandler* /*aHandler*/ )
    {
    FUNC_LOG;
    User::Panic( KErrorMessage, aError  );  
    }

// End of file

