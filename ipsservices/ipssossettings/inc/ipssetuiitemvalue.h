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
* Description:  Decleares class with numeric user data container.
*
*/



#ifndef IPSSETUIITEMVALUE_H
#define IPSSETUIITEMVALUE_H

#include <e32base.h>

#include "ipssetuiitembase.h"
#include "ipssetutilsconsts.h"

/**
 *  Container class for numeric user data.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiItemValue : public CIpsSetUiItem
    {
public:

// Constructors and Destructor

    /**
     * Destructor
     */
    virtual ~CIpsSetUiItemValue();

    /**
     * @return New numeric dataitem.
     */
    static CIpsSetUiItemValue* NewL();

    /**
     * @return New numeric dataitem.
     */
    static CIpsSetUiItemValue* NewLC();

// Operators

    /**
     * Copy operator
     *
     * @param aBaseItem Item, which values are copied.
     */
    CIpsSetUiItemValue& operator=(
        const CIpsSetUiItemValue& aBaseItem );

// New Functions

    /**
     * @return User data.
     */
    virtual inline TInt64 Value() const;

    /**
     * @param aValue Sets user data.
     */
    virtual inline void SetValue( const TInt64 aValue );

private:

// Constructors

    /**
     * Constructor
     */
    CIpsSetUiItemValue();

    /**
     * 2nd-phase constructor
     */
    void ConstructL();

private: // Data

    /**
     * Container for numeric userdata
     */
    TInt64              iUserValue;

public: // Data

    /**
     * Maximum value for the item.
     */
    TInt                iItemMaxLength;

    /**
     * Text that should showed in the setting list for the item.
     */
    TIpsSetUtilsTextPlain*  iItemSettingText;
    };

#include "ipssetuiitemvalue.inl"

#endif // IPSSETUIITEMVALUE_H

// End of file
