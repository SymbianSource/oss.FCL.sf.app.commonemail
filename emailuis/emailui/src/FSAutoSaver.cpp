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
* Description: This file implements class CFsAutoSaver.
*
*/
// <cmail>
#include "emailtrace.h"
#include <alf/alfenv.h>
// </cmail>
#include"FSAutoSaver.h"
#include"FreestyleEmailUiConstants.h"


// -----------------------------------------------------------------------------
// CFsAutoSaver::NewL
// 
// -----------------------------------------------------------------------------
//    
CFsAutoSaver* CFsAutoSaver::NewL( CAlfEnv& aEnv, TInt aTimeDelayMs )
	{
    FUNC_LOG;
	CFsAutoSaver* self = new (ELeave) CFsAutoSaver( aEnv, aTimeDelayMs );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
	
// -----------------------------------------------------------------------------
// CFsAutoSaver::CFsAutoSaver
// 
// -----------------------------------------------------------------------------
//    
CFsAutoSaver::CFsAutoSaver( CAlfEnv& aEnv, TInt aTimeDelayMs ): iEnv( aEnv ),
	iTimeDelayMs( aTimeDelayMs ), iEnabled( ETrue )
	{
    FUNC_LOG;
	}
	
	
// -----------------------------------------------------------------------------
// CFsAutoSaver::ConstructL
// 
// -----------------------------------------------------------------------------
//    
void CFsAutoSaver::ConstructL()
	{
    FUNC_LOG;
	iTimer = CFSEmailUiGenericTimer::NewL( this );
	}


// -----------------------------------------------------------------------------
// CFsAutoSaver::~CFsAutoSaver
// 
// -----------------------------------------------------------------------------
//    
CFsAutoSaver::~CFsAutoSaver()
	{
    FUNC_LOG;
	delete iTimer;
	}


// -----------------------------------------------------------------------------
// CFsAutoSaver::ReportActivity
// 
// -----------------------------------------------------------------------------
//    
void CFsAutoSaver::ReportActivity()
	{
    FUNC_LOG;
	iTimer->Stop();
	iTimer->Start( iTimeDelayMs );
	}


// -----------------------------------------------------------------------------
// CFsAutoSaver::TimerEventL
// 
// -----------------------------------------------------------------------------
//    
void CFsAutoSaver::TimerEventL( CFSEmailUiGenericTimer* /*aTriggeredTimer*/ )
	{
    FUNC_LOG;
	if ( iEnabled )
		{
		iEnv.Send( TAlfActionCommand( KCmdEditorAutoSave ) );
		}
	}


// -----------------------------------------------------------------------------
// CFsAutoSaver::Enable
// 
// -----------------------------------------------------------------------------
//    
void CFsAutoSaver::Enable( TBool aEnable )
	{
    FUNC_LOG;
	iEnabled = aEnable;
	if ( !iEnabled )
		{
		iTimer->Stop();
		}
	}


// -----------------------------------------------------------------------------
// CFsAutoSaver::IsEnabled
// 
// -----------------------------------------------------------------------------
//    
TBool CFsAutoSaver::IsEnabled()
	{
    FUNC_LOG;
	return iEnabled;
	}

