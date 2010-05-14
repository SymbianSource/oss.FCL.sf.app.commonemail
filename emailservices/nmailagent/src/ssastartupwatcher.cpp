// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "ssastartupwatcher.h"

/*!
    Factory function to create a new instance of the CSSAStartupWatcher
    \return new CSSAStartupWatcher object
*/
CSSAStartupWatcher* CSSAStartupWatcher::NewL(TStartupCallBack aCallback)
{
	CSSAStartupWatcher* self = new (ELeave) CSSAStartupWatcher(KDmHierarchyIdStartup, 
	    KSM2AppServicesDomain3, aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
}

/*!
    Factory function to create a new instance of the CSSAStartupWatcher
    \return new CSSAStartupWatcher object
*/
CSSAStartupWatcher* CSSAStartupWatcher::New(TStartupCallBack aCallback)
{
    CSSAStartupWatcher* self = NULL;
    TRAP_IGNORE( self = NewL(aCallback); );
    return self;
}

/*!
    Constructor of CWatcherSSAStartupMgr

    \param aHierarchyId The Id of the domain hierarchy to connect to
    \param aDomainId The Id of the domain to connect to
*/
CSSAStartupWatcher::CSSAStartupWatcher(TDmHierarchyId aHierarchyId, 
    TDmDomainId aDomainId, TStartupCallBack aCallback)
: CDmDomain(aHierarchyId,aDomainId), 
  iCallback(aCallback)
{
}

/*!
    Destructor
*/
CSSAStartupWatcher::~CSSAStartupWatcher()
{
	Cancel();
}

/**
Second-stage constructor.

This method indicates how the CWatcherSSAStartupMgr interacts with the
Domain manager to keep aware of the startup state change.
*/
void CSSAStartupWatcher::ConstructL()
{
	// Connect to the Domain Manager
	CDmDomain::ConstructL();

	// Get the start-up state from the Domain Manager.
	TDmDomainState state = GetState();

	// Something wrong with the Domain Manager
	if( state <= EStartupStateUndefined ) {
	    User::Leave(KErrNotReady);
	}

	// Already passed the non critical state
	if( state >= EStartupStateNonCritical ) {
	    NotifyState(state);
	}
	else {
	    RequestTransitionNotification();
	}
}

/*!
    Notify state change
*/
void CSSAStartupWatcher::NotifyState(TInt aValue) 
{
    (*iCallback)(aValue);
}

/*!
    Executed when the startup state change is done, it does the same thing
    as the method InitialiseL().
*/
void CSSAStartupWatcher::RunL()
{
	// Leave if our previous request to be notified a state change has
	// returned an error and let RunError handle this.
	if (iStatus.Int()!=KErrNone) {
	    NotifyState( iStatus.Int() );
	}
	else {
	    // Get the start-up state from the Domain Manager.
	    TDmDomainState state = GetState();

	    // Reached the non critical startup state
	    if( state >= EStartupStateNonCritical ) {
	        NotifyState(state);
	    }
	    else {
	        AcknowledgeLastState(KErrNone);
	        RequestTransitionNotification();
	    }
	}
}

// End of file
