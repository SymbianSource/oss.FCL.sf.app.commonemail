/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Drive observer implementation.
*
*/

// INCLUDE FILES
#include <pathinfo.h>
#include <f32file.h>
#include "cmaildriveobserver.h"


// ---------------------------------------------------------
// CDriveWatcher::NewL
// Second phase constructor
// ---------------------------------------------------------
//
CDriveObserver* CDriveObserver::NewL( MDriveObserver& aDriveObserver )
	{
	CDriveObserver* self = CDriveObserver::NewLC(  aDriveObserver );
	CleanupStack::Pop(self);
	return self;
	}
	
// ---------------------------------------------------------
// CDriveWatcher::NewLC
// Second phase constructor
// ---------------------------------------------------------
//
CDriveObserver* CDriveObserver::NewLC( MDriveObserver& aDriveObserver )
	{
	CDriveObserver* self = new(ELeave) CDriveObserver( aDriveObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;	
	}
	
// ---------------------------------------------------------
// CDriveWatcher::~CDriveWatcher
// Destructor
// ---------------------------------------------------------
//
CDriveObserver::~CDriveObserver()
	{	
	
	delete iDriveLetter;
	Cancel();
	
	iFs.Close();
	
	}

// ---------------------------------------------------------
// CDriveWatcher::CDriveWatcher
// Default constructor
// ---------------------------------------------------------
//
CDriveObserver::CDriveObserver( MDriveObserver& aDriveObserver )
	: CActive( CActive::EPriorityStandard ), 
	iDriveObserver( aDriveObserver )
	{
	}

// ---------------------------------------------------------
// CDriveWatcher::ConstructL
// default Symbian OS constructor
// ---------------------------------------------------------
//
void CDriveObserver::ConstructL()
	{	
	CActiveScheduler::Add(this);
	User::LeaveIfError( iFs.Connect() );
	}


// ---------------------------------------------------------
// CDriveWatcher::WaitForChangeL()
// Request notification for disk change
// ---------------------------------------------------------
//	
void CDriveObserver::WaitForChange()
	{
	if ( !IsActive() )
	    {
	    iFs.NotifyChange( ENotifyEntry, iStatus, *iDriveLetter );
	    SetActive();
	    }
	}
	
// ---------------------------------------------------------
// CDriveWatcher::MediaPresentL()
// Call scanning if the memory card exists
// ---------------------------------------------------------
//	
void CDriveObserver::MediaPresentL()
	{
	TVolumeInfo volumeInfo;
	// Returns KErrNone, if drive exists
	TInt err=iFs.Volume( volumeInfo, iDrive );
	   
	if ( err == KErrNone )
		{
		iDriveObserver.DriveStateChangedL( ETrue );
		}
	else
	    {
        iDriveObserver.DriveStateChangedL( EFalse );
	    }
	}

// ---------------------------------------------------------
// Return true if drive is present and false otherwise.
 // ---------------------------------------------------------
 //
TBool CDriveObserver::IsDrivePresent()
    {    
    TVolumeInfo volumeInfo;
    // Returns KErrNone, if drive exists
    TInt err=iFs.Volume( volumeInfo, iDrive );
    
    return ( KErrNone == err );    
    }

// ---------------------------------------------------------
// CDriveWatcher::DoCancel()
// Derived from CActive, cancel request
// ---------------------------------------------------------
//
void CDriveObserver::DoCancel()
	{
	iFs.NotifyChangeCancel();
	}

// ---------------------------------------------------------
// CDriveWatcher::RunL()
// Derived from CActive, called when notification received
// Check if media is present and call new notification request
// ---------------------------------------------------------
//	
void CDriveObserver::RunL()
	{	
	if ( iStatus.Int() != KErrCancel )
	    {	    
	    WaitForChange();	   
	    MediaPresentL();
	    }
	}

// ---------------------------------------------------------
// CDriveWatcher::RunError()
// ---------------------------------------------------------
//  

TInt CDriveObserver::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CDriveObserver::SetDriveL()
// ---------------------------------------------------------
//
void CDriveObserver::SetDriveL( TDriveNumber aDrive )
    {    
    if (iDrive == aDrive)
        {
        //if drives are the same don't need to do nothing
        return;
        }
    else if ( IsActive() )
        {
        //cancel request if drive to monitor changed
        Cancel();
        }
            
    const TUint KDriveBufSize = 2;
    
    iDrive = aDrive;
    
    TChar driveAsChar = 0;
    RFs::DriveToChar( iDrive, driveAsChar );
    
    delete iDriveLetter;
    iDriveLetter = NULL;
    iDriveLetter = HBufC::NewL( KDriveBufSize );
    
    iDriveLetter->Des().Append( driveAsChar );
    iDriveLetter->Des().Append(':');       
    }
//EOF

