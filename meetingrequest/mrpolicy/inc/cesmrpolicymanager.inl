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
* Description:  ESMR policy manager inline methods 
*
*/


// -----------------------------------------------------------------------------
// CESMRPolicyManager::CurrentPolicy
// -----------------------------------------------------------------------------
//
inline CESMRPolicy& CESMRPolicyManager::CurrentPolicy()
    {
    CESMRPolicy* currentPolicy = iCurrentPolicy;
    if ( !currentPolicy && iPolicyStack.Count() )
        {
        currentPolicy = iPolicyStack[ iPolicyStack.Count() - 1 ];
        }
    
    ASSERT( currentPolicy );
    return *currentPolicy;
    }
