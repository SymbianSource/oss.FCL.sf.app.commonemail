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
* Description: This file defines class TFsScrollbarFactory.
*
*/



#ifndef T_FSSCROLLBARFACTORY_H
#define T_FSSCROLLBARFACTORY_H

#include <alf/alfextensionfactory.h>

/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class TFsScrollbarFactory : public MAlfExtensionFactory
    {

public:

// from base class MAlfExtensionFactory

    /**
     * From MAlfExtensionFactory.
     * Creates new instace of required type (unique for factory) Must not take
     * ownership of created object.
     *
     * @since S60 ?S60_version
     * @param aObjectId      Object type required
     * @param aInputBuffer   Additional data attached to object construction
     * @param aResolver      Reference to Interface resolver
     */
    MAlfExtension* CreateExtensionL(
        const TInt aObjectId,
        const TDesC8& aInitialParams,
        MAlfInterfaceProvider& aResolver );

    /**
     * From MAlfExtensionFactory.
     * Free resources for instance, usually delete this
     */
    void Release();

    };


#endif // T_FSSCROLLBARFACTORY_H
