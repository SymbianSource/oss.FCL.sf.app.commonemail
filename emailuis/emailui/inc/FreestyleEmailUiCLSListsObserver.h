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
* Description:  CLS list change observer abstract class definition
*
*/


#ifndef FREESTYLEEMAILUICLSLISTSOBSERVER_H
#define FREESTYLEEMAILUICLSLISTSOBSERVER_H

// FORWARD DECLARATIONS

class CFSEmailUiClsItem;

// CLASS DECLARATION
/**
 * Freestyle email application's cls list change observer interface
 *
 *
 */
 
class MFSEmailUiClsListsObserver
	{

public:

    /**
     * Notifies the observer to update shown contact list with new matching items array
     * Observer should save the pointer for further usage. Everytime this UpdateL method
     * is called new pointer should be saved. 
     *
     * @param aMatchingItems list of matching contact and MRU list items
     *					First items in the array are the one found in contacts which have email address.
     *					After first items are matches from MRU list. Last items are contact matches which don't
     *					have email address.
     */
    virtual void ArrayUpdatedL( const RPointerArray<CFSEmailUiClsItem>& aMatchingItems ) = 0;
    virtual void OperationErrorL( TInt aErrorCode )= 0;
    };

#endif  // FREESTYLEEMAILUICLSLISTSOBSERVER_H

// End of File
