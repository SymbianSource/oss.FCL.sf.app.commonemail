/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Used for observing property changes and notifying listeners.
 *
 */

// CLASS HEADER
#include "FreestyleEmailUiPropertySubscriber.h"


// CONSTRUCTION AND DESTRUCTION

// -----------------------------------------------------------------------------
// Private C++ constructor.  
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiPropertySubscriber::CFreestyleEmailUiPropertySubscriber(
	MFreestyleEmailUiPropertyChangedObserver& aObserver ) 
    : CActive( EPriorityStandard ),
    iPropertyChangeObserver( aObserver )
	{
	// No implementation required.
    }


// -----------------------------------------------------------------------------
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiPropertySubscriber* CFreestyleEmailUiPropertySubscriber::NewLC(
    const TUid aUid, 
    const TUint32 aKey,
    MFreestyleEmailUiPropertyChangedObserver& aObserver )
	{
    CFreestyleEmailUiPropertySubscriber* self =
		new ( ELeave ) CFreestyleEmailUiPropertySubscriber( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aUid, aKey );
    return self;
    }


// -----------------------------------------------------------------------------
// Two-phase constructor.
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiPropertySubscriber* CFreestyleEmailUiPropertySubscriber::NewL(
    const TUid aUid, 
    const TUint32 aKey,
    MFreestyleEmailUiPropertyChangedObserver& aObserver )
    {
    CFreestyleEmailUiPropertySubscriber* self =
		CFreestyleEmailUiPropertySubscriber::NewLC( aUid, aKey, aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiPropertySubscriber::ConstructL( const TUid aUid,
													const TUint32 aKey )
    {
    User::LeaveIfError( iProperty.Attach( aUid, aKey ) );
    CActiveScheduler::Add( this ); // Add to scheduler.
    // The initial subscription and processing of the current property value.
    RunL();
    }


// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CFreestyleEmailUiPropertySubscriber::~CFreestyleEmailUiPropertySubscriber()
    {
    Cancel(); // Cancel any pending request.
    iProperty.Close();
    }


// PRIVATE METHODS FROM CACTIVE

// -----------------------------------------------------------------------------
// Handles completion.
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiPropertySubscriber::RunL()
    {
    // Resubscribe before processing a new value in order to prevent missing
    // updates.
    iProperty.Subscribe( iStatus );
    SetActive();
     
    TInt intValue( 0 );

    if ( iProperty.Get( intValue ) == KErrNone )
    	{
        iPropertyChangeObserver.PropertyChangedL( intValue );
        }
    }


// -----------------------------------------------------------------------------
// Cancels the pending requests.
// -----------------------------------------------------------------------------
//
void CFreestyleEmailUiPropertySubscriber::DoCancel()
    {
    iProperty.Cancel();
    }


// End of file.
