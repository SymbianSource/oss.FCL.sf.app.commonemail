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
* Description:  Base operation class
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"


// ----------------------------------------------------------------------------
// CIpsPlgBaseOperation::~CIpsPlgBaseOperation()
// ----------------------------------------------------------------------------
//
CIpsPlgBaseOperation::~CIpsPlgBaseOperation()
    {
    FUNC_LOG;
    // Cancel() should be called in deriving classes;
    }

        
// ----------------------------------------------------------------------------
// CIpsPlgBaseOperation::CIpsPlgBaseOperation()
// ----------------------------------------------------------------------------
//
CIpsPlgBaseOperation::CIpsPlgBaseOperation(
    CMsvSession& aMsvSession,
    TInt aPriority,
    TRequestStatus& aObserverRequestStatus,
    TInt aFSRequestId,
    TFSMailMsgId aFSMailboxId )
    :
    CMsvOperation( aMsvSession, aPriority, aObserverRequestStatus ),
    iFSRequestId( aFSRequestId ), iFSMailboxId( aFSMailboxId )
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsPlgBaseOperation::FSRequestId()
// ----------------------------------------------------------------------------    
//   
TInt CIpsPlgBaseOperation::FSRequestId() const
    {
    FUNC_LOG;
    return iFSRequestId;
    }
    
// ----------------------------------------------------------------------------
// CIpsPlgBaseOperation::FSMailboxId()
// ----------------------------------------------------------------------------    
//   
TFSMailMsgId CIpsPlgBaseOperation::FSMailboxId() const
    {
    FUNC_LOG;
    return iFSMailboxId;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
//   
TInt CIpsPlgBaseOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeUnknown;
    }

