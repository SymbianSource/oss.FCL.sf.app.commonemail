/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Definition of asynchronous text formatter.
*
*/

#include "emailtrace.h"
#include "FSAsyncTextFormatter.h"
#include <FRMTLAY.H>
#include <e32base.h>

const TUint KCharsFormattedPerRun = 2000;

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::CFSAsyncTextFormatter
// Constructor.
// ---------------------------------------------------------------------------
//
CFSAsyncTextFormatter::CFSAsyncTextFormatter() : CActive( EPriorityLow )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::~CFSAsyncTextFormatter
// Destructor.
// ---------------------------------------------------------------------------
//
CFSAsyncTextFormatter::~CFSAsyncTextFormatter()
    {
    FUNC_LOG;
    Cancel();
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFSAsyncTextFormatter* CFSAsyncTextFormatter::NewL()
    {
    FUNC_LOG;
    CFSAsyncTextFormatter* self = new (ELeave) CFSAsyncTextFormatter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CFSAsyncTextFormatter::ConstructL()
    {
    FUNC_LOG;
    iCurrentDocPos = 0;
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::FormatAll
// Starts formatting process.
// ---------------------------------------------------------------------------
//
void CFSAsyncTextFormatter::StartFormatting( CTextLayout* aTextLayout, 
        MAsyncTextFormatterObserver* aObserver )
    {
    FUNC_LOG;
    iTextLayout = aTextLayout;
    iObserver = aObserver;
    iCurrentDocPos = 0;
    
    if ( !IsActive() )
        {
        TRequestStatus * status = &iStatus;
        User::RequestComplete(status, 0);
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::RunL
// Handles an active object's request completion event.
// ---------------------------------------------------------------------------
//
void CFSAsyncTextFormatter::RunL()
    {
    FUNC_LOG;
    
    if ( iCurrentDocPos < iTextLayout->DocumentLength() )
        {
        iCurrentDocPos += KCharsFormattedPerRun;
        if ( iCurrentDocPos > iTextLayout->DocumentLength() )
            {
            iCurrentDocPos = iTextLayout->DocumentLength();
            }
        iTextLayout->ExtendFormattingToCoverPosL( iCurrentDocPos );
        
        // run again
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    else
        {
        if ( iObserver )
            {
            iObserver->FormatAllTextComplete();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFSAsyncTextFormatter::DoCancel
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------
//
void CFSAsyncTextFormatter::DoCancel()
    {
    FUNC_LOG;
    if ( IsActive() && iObserver )
        {
        iObserver->FormatAllTextCancelled();
        }
    }
