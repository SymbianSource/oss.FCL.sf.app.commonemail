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
* Description:  CLS list change observer abstract class definition
 *
*/


#ifndef MESMRCLSLISTSOBSERVER_H
#define MESMRCLSLISTSOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include "cesmrclsitem.h"       // CESMRClsItem

/**
 * Freestyle email application's cls list change observer interface
 */
class MESMRClsListsObserver
    {
public:
    /**
     * Notifies the observer to update shown contact list with new matching items array
     * Observer should save the pointer for further usage. Everytime this UpdateL method
     * is called new pointer should be saved. Ownership is trasfered to the observer.
     * Observer should delete all the items and reset the array when new is received.
     *
     * First items in the array are the one found in contacts which have email address.
     * After first items are matches from MRU list. Last items are contact matches which don't
     * have email address.
     *
     * @param aMatchingItems list of matching contact and MRU list items
     */
    virtual void ArrayUpdatedL( RPointerArray<CESMRClsItem>& aMatchingItems ) = 0;
    
    /**
     * Notify if error occured during the fill operation for the matching items array
     *
     * @param aErrorCode type of failure of the array operation
     */
    virtual void OperationErrorL( TInt aErrorCode )= 0;
    };

#endif  // CESMRCLSLISTSOBSERVER_H

// End of File
