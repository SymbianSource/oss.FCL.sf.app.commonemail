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
* Description:  Defines searcher class interface.
*
*/


#ifndef IPSSETUIFINDERINTERFACE_H
#define IPSSETUIFINDERINTERFACE_H

#include <e32base.h>
#include "ipssetuiitembase.h"

/**
 * Result of the find operation.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class TIpsSetFinderItem
    {
    public: // Data
        CIpsSetUiItem* iItem;
        TInt              iIndex;
    };

/**
 * Array of result items.
 */
typedef CArrayFixFlat<TIpsSetFinderItem> CIpsSetUiFinderArray;

/**
 * Interface for finder class for event handling.
 *
 * @since FS v1.0
 * @lib IpsSosSettings.lib
 */
class MIpsSetUiFinder
    {
    public: // New virtual functions

        virtual TBool SearchDoError() = 0;

        virtual TBool SearchDoContinuationCheck(
            const CIpsSetUiItem& aItem,
            const TInt aIndex ) = 0;

        virtual TBool SearchDoItemCheck(
            CIpsSetUiItem& aItem ) = 0;

        virtual TInt IsHidden( const CIpsSetUiItem& aItem ) const = 0;
    };

#endif /* IPSSETUIFINDERINTERFACE_H */
