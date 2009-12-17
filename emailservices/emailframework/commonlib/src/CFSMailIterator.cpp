/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  email list iterator
*
*/


#include "emailtrace.h"
#include "CFSMailIterator.h"

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailIterator::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailIterator* CFSMailIterator::NewLC( 	MFSMailIterator& aIterator,
													CFSMailRequestHandler* aRequestHandler )
{
    FUNC_LOG;
  CFSMailIterator* api = new (ELeave) CFSMailIterator();
  CleanupStack:: PushL(api);
  api->ConstructL( aIterator, aRequestHandler );
  return api;
} 

// -----------------------------------------------------------------------------
// CFSMailIterator::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailIterator* CFSMailIterator::NewL( 	MFSMailIterator& aIterator,
													CFSMailRequestHandler* aRequestHandler )
{
    FUNC_LOG;
  CFSMailIterator* api =  CFSMailIterator::NewLC( aIterator, aRequestHandler );
  CleanupStack:: Pop(api);
  return api;
}

// -----------------------------------------------------------------------------
// CFSMailIterator::CFSMailIterator
// -----------------------------------------------------------------------------
CFSMailIterator::CFSMailIterator()
{
    FUNC_LOG;
	iIterator = NULL;
}

// -----------------------------------------------------------------------------
// CFSMailIterator::ConstructL
// -----------------------------------------------------------------------------
void CFSMailIterator::ConstructL( 	MFSMailIterator& aIterator,
									CFSMailRequestHandler* aRequestHandler )
{
    FUNC_LOG;
	iIterator = &aIterator;
	iRequestHandler = aRequestHandler;
 }

// -----------------------------------------------------------------------------
// CFSMailIterator::~CFSMailIterator
// -----------------------------------------------------------------------------
EXPORT_C CFSMailIterator::~CFSMailIterator()
{
    FUNC_LOG;
	if(iIterator)
		{
		delete iIterator;
		}
}

// -----------------------------------------------------------------------------
// CFSMailIterator::NextL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailIterator::NextL(
        TFSMailMsgId aCurrentMessageId, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages)
	{
    FUNC_LOG;
	// call plugin iterator
	TBool ret = iIterator->NextL(aCurrentMessageId,aCount,aMessages);
	
	return ret;
	}
	
	
// -----------------------------------------------------------------------------
// CFSMailIterator::NextL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailIterator::NextL(
        const TDesC&  aStartWith, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages)
	{
    FUNC_LOG;

	// call plugin iterator
	TBool ret = iIterator->NextL(aStartWith,aCount,aMessages);
	
	return ret;
	}
	
// -----------------------------------------------------------------------------
// CFSMailIterator::PreviousL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailIterator::PreviousL(
        TFSMailMsgId aCurrentMessageId, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages)
	{
    FUNC_LOG;
		// call plugin iterator
	TBool ret = iIterator->PreviousL(aCurrentMessageId,aCount,aMessages);
		
	return ret;

	}
	
// -----------------------------------------------------------------------------
// CFSMailIterator::PreviousL
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailIterator::PreviousL(
        const TDesC&  aStartWith, 
        TUint aCount, 
        RPointerArray<CFSMailMessage>& aMessages)
	{
    FUNC_LOG;

	// call plugin iterator
	TBool ret = iIterator->PreviousL(aStartWith,aCount,aMessages);
	
	return ret;

	}

