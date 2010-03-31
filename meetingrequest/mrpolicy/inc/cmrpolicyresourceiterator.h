/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for MR policy resolver ECom interface
*
*/


#ifndef CMRPOLICYRESOURCEITERATOR_H
#define CMRPOLICYRESOURCEITERATOR_H

#include <e32base.h>
#include <barsc.h>

class CESMRPolicy;

/**
 *  CMRPolicyResolver defines ECom plugin interface for policy resolving .
 *  @see MESMRTaskFactory
 *
 *  @lib esmrtasks.lib
 */
NONSHARABLE_CLASS( CMRPolicyResourceIterator ) : public CBase
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new ES MR task factory.
     * @param aCalDbMgr Reference to cal db manager.
     */
    IMPORT_C static CMRPolicyResourceIterator* NewL( const TDesC& aResourceFile,
                                                     TInt aResourceId );

    /**
     * C++ destructor.
     */
    IMPORT_C ~CMRPolicyResourceIterator();
    
public:
    
    /**
     * Tests if iterator has more policies available.
     * 
     * @return ETrue if iterator has policy
     */
    IMPORT_C TBool HasNext() const;
    
    /**
     * Loads and instantiates next policy from resource.
     * Ownership of policy is transferred to caller.
     * 
     * @return next policy
     */
    IMPORT_C CESMRPolicy* NextPolicyL();
    
    /**
     * Loads and instantiates next policy from resource.
     * Policy is left into CleanupStack.
     * 
     * @return next policy
     */
    IMPORT_C CESMRPolicy* NextPolicyLC();
    
    /**
     * Resets iterator.
     */
    IMPORT_C void Reset();

private: // Implementation
    
    CMRPolicyResourceIterator();
    
    void ConstructL(
            const TDesC& aResourceFile,
            TInt aResourceId );
    
    void ReadPolicyFromResourceL(
            const TDesC& aPolicyFile,
            TInt aPolicyArrayResourceId );
    
    CESMRPolicy* ReadPolicyLC( TInt aResourceId );
    
private: // Data
    
    /**
     * Policy resource file.
     * Own.
     */
    RResourceFile iPolicyResourceFile;
    
    /**
     * Array of policy resource ids
     * Own.
     */
    RArray< TInt > iPolicyResourceIds;
    
    /**
     * Array index (iterator)
     */
    TInt iArrayIndex;
    
    };

#endif // CMRPOLICYRESOURCEITERATOR_H
