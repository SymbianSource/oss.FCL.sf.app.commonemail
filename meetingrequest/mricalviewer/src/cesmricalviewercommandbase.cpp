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
* Description:  ESMR command base class implementation
*
*/


// INCLUDE FILES
#include "emailtrace.h"
#include "cesmricalviewercommandbase.h"
#include <calsession.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

// Panic literal
_LIT( KESMRIcalViewerCommandBase, "ESMRIcalViewerCommandBase" );

/** Panic codes */
enum TESMRIcalViewerCommandBasePanic
	{
	EObserverChanged = 0,
	EMailMessageNotSet = 1, // Email message is not set
	};

void Panic( TESMRIcalViewerCommandBasePanic aPanic )
	{
	User::Panic( KESMRIcalViewerCommandBase, aPanic );	
	}

#endif

}  // namespace

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase(
		TESMRIcalViewerOperationType aType,
		CCalSession& aCalSession )
:	iOperationType( aType ),
	iCalSession( aCalSession )
	{
    FUNC_LOG;
	
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
CESMRIcalViewerCommandBase::~CESMRIcalViewerCommandBase()
	{
    FUNC_LOG;
	
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCommandBase::BaseConstructL()
	{
    FUNC_LOG;
	// not yet implementation
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
TESMRIcalViewerOperationType CESMRIcalViewerCommandBase::OperationType() const
	{
    FUNC_LOG;
	return iOperationType;
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
CCalSession& CESMRIcalViewerCommandBase::CalSession()
	{
    FUNC_LOG;
	return iCalSession;
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCommandBase::SetObserver( 
			MESMRIcalViewerObserver* aObserver )
	{
    FUNC_LOG;
	if ( iObserver && iObserver != aObserver )
		{
		__ASSERT_DEBUG( !iObserver, Panic( EObserverChanged ) );
		}

	iObserver = aObserver;
	}
	
// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
MESMRIcalViewerObserver* CESMRIcalViewerCommandBase::Observer()
	{
    FUNC_LOG;
	return iObserver;
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
void CESMRIcalViewerCommandBase::SetMessage( 
		CFSMailMessage* aMessage )
	{
    FUNC_LOG;
	iMessage = aMessage;
	}

// -----------------------------------------------------------------------------
// CESMRIcalViewerCommandBase::CESMRIcalViewerCommandBase
// -----------------------------------------------------------------------------
//
CFSMailMessage* CESMRIcalViewerCommandBase::Message()
	{
    FUNC_LOG;
	__ASSERT_DEBUG( iMessage, Panic( EMailMessageNotSet ) ); 
	return iMessage;
	}

// EOF

