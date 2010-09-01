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
* Description:  Decleares class with container for setting items.
*
*/


#ifndef IPSSETUIITEMLINK_H
#define IPSSETUIITEMLINK_H

#include <e32base.h>
#include "ipssetuiitembase.h"

/**
 *  Base item for branch items.
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v1.0
 */
class CIpsSetUiItemLink : public CIpsSetUiItem
    {
    public: // Constructors and Destructor

        /**
         * Destructor
         */
        virtual ~CIpsSetUiItemLink();

        /**
         * @return New link item.
         */
        static CIpsSetUiItemLink* NewL();

        /**
         * @return New link item.
         */
        static CIpsSetUiItemLink* NewLC();

    public: // Operators

        /**
         * Copy operator
         *
         * @param aBaseItem Item, which values are copied.
         */
        virtual CIpsSetUiItemLink& operator=(
            const CIpsSetUiItemLink& aBaseItem );

    public: // Functions from base classes

        /**
         * @return ETrue, when item contains a branch of items.
         */
        virtual inline TBool HasLinkArray() const;

    protected: // Constructors

        /**
         * Constructor
         */
        CIpsSetUiItemLink();

        /**
         * 2nd-phase constructor
         */
        void ConstructL();

    public: // Data

        /**
         * Forces the item look like a other type.
         */
        TIpsSetUiSettingsType    iItemAppereance;

        /**
         * Container for branch of items.
         * Owned.
         */
        CIpsSetUiBaseItemArray*   iItemLinkArray;

        /**
         * Container for setting text.
         * Owned.
         */
        TIpsSetUtilsTextPlain*      iItemSettingText;
    };

#include "ipssetuiitemlink.inl"

#endif //IPSSETUIITEMLINK_H

// End of file
