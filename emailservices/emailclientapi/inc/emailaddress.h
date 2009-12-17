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
* Description: mail address implementation definition
*
*/

#ifndef EMAILADDRESS_H
#define EMAILADDRESS_H

#include <memailaddress.h>
#include "emailapiutils.h"

using namespace EmailInterface;

NONSHARABLE_CLASS( CEmailAddress ) : public CBase, public MEmailAddress
    {
public:
    static CEmailAddress* NewL( const TRole aRole, const TDataOwner aOwner );
    static CEmailAddress* NewLC( const TRole aRole, const TDataOwner aOwner );
    
    ~CEmailAddress();

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();

public: // from MEmailAddress
    virtual void SetAddressL( const TDesC& aAddress );
    virtual TPtrC Address() const;
    
    virtual void SetDisplayNameL( const TDesC& aDisplayName );
    virtual TPtrC DisplayName() const;
    
    virtual TRole Role() const;
    virtual void SetRole( const TRole aRole );
            
private:        
        CEmailAddress( const TRole aRole, const TDataOwner aOwner );
        
private:
        RBuf iAddress;
        RBuf iDisplayName;
        TRole iRole;
        TDataOwner iOwner;
    };

#endif // EMAILADDRESS_H

// End of file
