/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CESMREntryCmdIteratorAO.
*
*/


//INCLUDE FILES
#include "emailtrace.h"
#include "cesmrentrycmditeratorao.h"
#include "cesmrcaldbmgr.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Definition text for panics
_LIT( KESMREntryCmdIteratorTxt, "ESMRUtilsEntryCmdIterator" );

// Panic code definition
enum TESMREntryCmdIteratorPanicCode
    {
    // Entry array not empty when iterator is inactive
    EESMRUtilsEntryCmdIteratorNonEmptyArray = 1 
    };

void Panic( TESMREntryCmdIteratorPanicCode aReason )
    {
    User::Panic( KESMREntryCmdIteratorTxt, aReason );
    }
#endif

}  // namespace


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::CESMREntryCmdIteratorAO
//
// Constructor.
// ----------------------------------------------------------------------------
//
CESMREntryCmdIteratorAO::CESMREntryCmdIteratorAO(
    MESMRCalDbMgr& aDbMgr )
    : CActive( EPriorityStandard ),
      iOpCode( CMRUtils::ENoOperation ),
      iDbMgr( aDbMgr )
    {
    FUNC_LOG;
    }
    
// ----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::~CESMREntryCmdIteratorAO
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CESMREntryCmdIteratorAO::~CESMREntryCmdIteratorAO()
    {
    FUNC_LOG;
    Cancel();
    iCalEntryArray.Close();    
    }
    
// ----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::NewL
// ----------------------------------------------------------------------------
//
CESMREntryCmdIteratorAO* CESMREntryCmdIteratorAO::NewL(
    MESMRCalDbMgr& aDbMgr )
	{
    FUNC_LOG;
	CESMREntryCmdIteratorAO* self =
	    new( ELeave ) CESMREntryCmdIteratorAO( aDbMgr );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
    
// ----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::ConstructL
// ----------------------------------------------------------------------------
//    
void CESMREntryCmdIteratorAO::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::ConstructL
// ----------------------------------------------------------------------------
//    
void CESMREntryCmdIteratorAO::StartCmdIterationL(
    const RPointerArray<CCalEntry>& aCalEntryArray,
    TInt aOpCode )
    {
    FUNC_LOG;
    if ( IsActive() )
        {        
        User::Leave( KErrInUse );
        }

    // array must be empty when we are not active:
    __ASSERT_DEBUG( iCalEntryArray.Count() == 0,
                    Panic( EESMRUtilsEntryCmdIteratorNonEmptyArray ) );
    
            
    TInt count( aCalEntryArray.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {        
        iCalEntryArray.AppendL( aCalEntryArray[i] );
        }
        
    iOpCode = aOpCode;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::RunL
// -----------------------------------------------------------------------------
//
void CESMREntryCmdIteratorAO::RunL()
    {
    FUNC_LOG;
    TInt lastIndex( iCalEntryArray.Count() -1 );
    if ( lastIndex < 0 )
        { // no more items, we are finished
        iDbMgr.Completed( KErrNone );
        }
    
    else
        {            
        CCalEntry* lastEntry = iCalEntryArray[lastIndex];    
        
        switch ( iOpCode )
            {
            case CMRUtils::EDeleteEntries:
                {
                iDbMgr.DeleteEntryCondL( *lastEntry );
                break;
                }
            }
            
        // The last entry was handled and can be removed from array:
        iCalEntryArray.Remove( lastIndex );
        delete lastEntry;
        
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );        
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::DoCancel
// -----------------------------------------------------------------------------
//
void CESMREntryCmdIteratorAO::DoCancel()
    {
    FUNC_LOG;
    iCalEntryArray.ResetAndDestroy();
    iOpCode = CMRUtils::ENoOperation;
    }
    
// -----------------------------------------------------------------------------
// CESMREntryCmdIteratorAO::RunError
// -----------------------------------------------------------------------------
//
TInt CESMREntryCmdIteratorAO::RunError( TInt aError )
    {
    FUNC_LOG;
    iCalEntryArray.ResetAndDestroy();    
    iDbMgr.Completed( aError );
    iOpCode = CMRUtils::ENoOperation;    
    return KErrNone;
    }

// End of file

