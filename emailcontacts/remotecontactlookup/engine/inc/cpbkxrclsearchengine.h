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
* Description:  Definition of the class CPbkxRclSearchEngine.
*
*/


#ifndef CPBKXRCLSEARCHENGINE_H
#define CPBKXRCLSEARCHENGINE_H

#include <e32base.h>
#include <cntitem.h>
#include <eikcmobs.h>
#include "tpbkxremotecontactlookupprotocolaccountid.h"
#include "mpbkxremotecontactlookupserviceui.h"

class CPbkxRemoteContactLookupProtocolAdapter;
class CPbkxRclProtocolEnvImpl;

/**
* Class that creates accounts and context that executes the actual
* search.
*
*  @lib pbkxrclengine.lib
*  @since S60 3.1
*/
class CPbkxRclSearchEngine : 
    public CBase,
    public MPbkxRemoteContactLookupServiceUi
    {

public: // constructors and destructors

    /**
    * Two-phased constructor.
    *
    * @return Created object.
    */
    IMPORT_C static CPbkxRclSearchEngine* NewL();
    
    /**
    * Two-phased constructor.
    *
    * Leaves pointer of the created object on the cleanupstack.
    *
    * @return Created object.
    */
    IMPORT_C static CPbkxRclSearchEngine* NewLC();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclSearchEngine();
    
public: // from MPbkxRemoteContactLookupServiceUi  

    /**
    * Returns default account identifier.
    *
    * @return default account identifier
    */
    virtual TPbkxRemoteContactLookupProtocolAccountId DefaultAccountIdL() const;

    /**
    * Creates new account with given protocol account identifier.
    *
    * @param aProtocolAccountId protocol account ID.
    * @return protocol account
    */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewAccountL(
        const TPbkxRemoteContactLookupProtocolAccountId& aProtocolAccountId ) const;

    /**
    * Create new execution context.
    *
    * @param aParams Construction parameters. 
    * @return execution context
    */     
    virtual CPbkxRemoteContactLookupServiceUiContext* NewContextL( 
        TContextParams& aParams ) const;
    
private: // methods used internally

    /**
    * Tries to create adapter with a given account id.
    *
    * @param aId Account id.
    * @return Created object.
    */
    CPbkxRemoteContactLookupProtocolAdapter* CreateAdapterLC(
        const TPbkxRemoteContactLookupProtocolAccountId& aId ) const;

private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclSearchEngine();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private: // data
    
    // Environment given to adapters. Owned.
    CPbkxRclProtocolEnvImpl* iEnvironment;

    };

#endif // CPBKXRCLSEARCHENGINE_H
