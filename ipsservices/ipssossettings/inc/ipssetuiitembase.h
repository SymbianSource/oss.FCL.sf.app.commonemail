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
* Description:  Base item behavior declearation.
*
*/


#ifndef IPSSETUIITEMBASE_H
#define IPSSETUIITEMBASE_H

#include <e32base.h>

#include "ipssetutilsconsts.h"
#include "ipssetutilsflags.h"

class CIpsSetUiItem;

typedef CArrayPtrFlat<CIpsSetUiItem> CIpsSetUiBaseItemArray;


/**
 *  Base class for all setting items.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiItem : public CBase
    {
public:

// Constructors and Destructor

    /**
     * Destructor
     */
    virtual ~CIpsSetUiItem();

    /**
     * @return New base item.
     */
    static CIpsSetUiItem* NewL();

    /**
     * @return New base item.
     */
    static CIpsSetUiItem* NewLC();

// Operators

    /**
     * Copy operator
     *
     * @param aBaseItem Item, which values are copied.
     */
    CIpsSetUiItem& operator=(
        const CIpsSetUiItem& aBaseItem );

// New virtual functions

    /**
     * @return Type of the item.
     */
    virtual inline TIpsSetUiSettingsType Type() const;

    /**
     * @return Numeric user value.
     */
    virtual inline TInt64 Value() const;

    /**
     * @param aValue Sets the numeric user value.
     */
    virtual inline void SetValue( const TInt64 aValue );

    /**
     * @return Read-only reference to textual user data.
     */
    virtual inline const TDesC& Text() const;

    /**
     * @param aText Sets the textual user data.
     */
    virtual inline void SetText( const TDesC& aText );

    /**
     * @return ETrue, if the item has got container for setting items.
     */
    virtual inline TBool HasLinkArray() const;

protected:

// Constructors

    /**
     * Constructor
     */
    CIpsSetUiItem();

    /**
     * 2nd-phase constructor
     */
    void ConstructL();

public:

// Data

    /**
     * Container for flags.
     */
    TIpsSetUtilsFlags        iItemFlags;

    /**
     * Type of the item.
     */
    TIpsSetUiSettingsType     iItemType;

    /**
     * Text to be shown in the item list.
     * Owned.
     */
    TIpsSetUtilsTextPlain*  iItemLabel;

    /**
     * Unique ID of the item.
     */
    TUid                    iItemId;

    /**
     * Resource the item belongs to.
     */
    TInt                    iItemResourceId;
    };

#include "ipssetuiitembase.inl"

#endif //IPSSETUIITEMBASE_H

// End of file
