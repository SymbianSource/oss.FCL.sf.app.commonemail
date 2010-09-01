/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages dynamic properties of the contact
 *
*/


#ifndef CFSCCONTACTFIELDDYNAMICPROPERTIES_H
#define CFSCCONTACTFIELDDYNAMICPROPERTIES_H

#include <e32base.h>
#include "MFscContactFieldDynamicProperties.h"

/**
 * Contact field dynamic properties.
 */
NONSHARABLE_CLASS( CFscContactFieldDynamicProperties ) :
public CBase,
public MFscContactFieldDynamicProperties
    {
public:
    /**
     * Constructor.
     */
    static CFscContactFieldDynamicProperties* NewLC();

    /**
     * Destructor.
     */
    ~CFscContactFieldDynamicProperties();

public: // From MFscContactFieldDynamicProperties
    TBool IsHiddenField( const MVPbkBaseContactField& aField ) const;

private: // Implementation
    CFscContactFieldDynamicProperties();
    void ConstructL();

private: // data
    /// Own: array of hidden field type resource ids
    RArray<TInt> iHiddenFieldResourceIds;
    };

#endif // CFSCCONTACTFIELDDYNAMICPROPERTIES_H
// End of file
