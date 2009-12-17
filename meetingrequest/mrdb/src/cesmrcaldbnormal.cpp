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
* Description: This file implements class CESMRCalDbNormal.
*
*/


//INCLUDE FILES
#include "emailtrace.h"
#include "cesmrcaldbnormal.h"

// From System
#include <calentryview.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calsession.h>
#include <calcommon.h>
#include <caluser.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Cal db normal panic text
_LIT( KESMRCalDbNormalTxt, "CESMRCalDbNormal" );

// Cal db normal panic codes
enum TESMRCalDbNormalPanicCode
    {
    EESMRCalDbNormalUnexpectedCalEngStatus = 1
    };

void Panic( TESMRCalDbNormalPanicCode aReason )
    {
    User::Panic( KESMRCalDbNormalTxt, aReason );
    }

#endif
    
}  // namespace


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CESMRCalDbNormal::CESMRCalDbNormal
// ----------------------------------------------------------------------------
//
CESMRCalDbNormal::CESMRCalDbNormal(
    CCalSession& aCalSession,
    MESMRCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
    : CESMRCalDbBase( aDbObserver, aCmdObserver )      
    {
    FUNC_LOG;
    // This subclass of CMRUtilsCalDbBase doesn't own the session    
    iCalSession = &aCalSession;
    }
    
// ----------------------------------------------------------------------------
// CESMRCalDbNormal::~CESMRCalDbNormal
//
// Destructor.
// ----------------------------------------------------------------------------
//        
CESMRCalDbNormal::~CESMRCalDbNormal()
    {
    FUNC_LOG;
    delete iCalEntryView;
    delete iCalInstanceView;
    }
    
// ----------------------------------------------------------------------------
// CESMRCalDbNormal::NewL
// ----------------------------------------------------------------------------
//
CESMRCalDbNormal* CESMRCalDbNormal::NewL(
    CCalSession& aCalSession,
    MESMRCalDbObserver& aDbObserver,
    MCalProgressCallBack& aCmdObserver )
	{
    FUNC_LOG;
    
    CESMRCalDbNormal* self =
	    new( ELeave ) CESMRCalDbNormal( aCalSession,
	                                         aDbObserver,
	                                         aCmdObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();

    
    return self;
	}
    
// ----------------------------------------------------------------------------
// CESMRCalDbNormal::ConstructL
// ----------------------------------------------------------------------------
//    
void CESMRCalDbNormal::ConstructL()
    {
    FUNC_LOG;
    
    iCalEntryView = CCalEntryView::NewL( *iCalSession, *this );
    iDbStatus = MESMRCalDbObserver::EInitEntryView;
    // See Completed() for continuation...    
    
    }

// ----------------------------------------------------------------------------
// CESMRCalDbNormal::Completed
// ----------------------------------------------------------------------------
//    
void CESMRCalDbNormal::Completed( TInt aError )
    {
    FUNC_LOG;
    
    if ( aError == KErrNone )
        {        
        if ( iDbStatus == MESMRCalDbObserver::EInitEntryView )
            { // Entry view successfully initialized, create instance view next
            
            TRAPD( err, iCalInstanceView =
                            CCalInstanceView::NewL( *iCalSession, *this ) );
            if ( err == KErrNone )
                {  // Instance view initialization started:
                iDbStatus = MESMRCalDbObserver::EInitInstanceView;    
                }
            else
                { // Instance view creation failed, cal engine not usable:
                iDbStatus = MESMRCalDbObserver::EFinishedError;
                }                                 
            }
        else
            { // Instance view successfully initialized
            __ASSERT_DEBUG( iDbStatus == MESMRCalDbObserver::EInitInstanceView,
                            Panic( EESMRCalDbNormalUnexpectedCalEngStatus ) );
            iDbStatus = MESMRCalDbObserver::EFinishedOk;
            }
        }            
    else
        { // Initialization error has occurred, cal engine not usable:
        iDbStatus = MESMRCalDbObserver::EFinishedError;
        }
            
    // If either of the final states has been reached, then notify observer:            
    if ( iDbStatus == MESMRCalDbObserver::EFinishedOk ||
         iDbStatus == MESMRCalDbObserver::EFinishedError )
        {
        iDbObserver.HandleCalDbStatus( this, iDbStatus );            
        }
    
    }

// End of file

