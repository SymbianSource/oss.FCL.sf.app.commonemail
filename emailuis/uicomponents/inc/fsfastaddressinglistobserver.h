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
* Description:  Declaration od MFsFastAddressingListObserver interface
*
*/


#ifndef M_FSFASTADDRESSLISTOBSERVER_H
#define M_FSFASTADDRESSLISTOBSERVER_H

#include <e32def.h>
#include <e32cmn.h>

/**
 *  Observer interface for CFsFastAddressingList.
 *
 *  The user of the CFsFastAddressingList class must implement this class
 *  in order to be notified when an end user enters text into the text
 *  input field of the fast addressing list component or selects a list
 *  item from the list of matches.
 */
class MFsFastAddressingListObserver
    {
public:

    /**
     * This will be called when the user enters some text into the text
     * input field. This will also called when the text input field is
     * cleared.
     *
     * @param aText the new contents of the text input field.
     */
    virtual void TextInputFieldUpdated( const TDesC& aText )=0;

    /**
     * This will be called once the user selects an item from the list of 
     * matches. The list needs to be explicitly closed.
     * 
     * @param aListItemIndex the index of the selected list item
     */
    virtual void ListItemSelected( const TInt aListItemIndex )=0;

    };


#endif // M_FSFASTADDRESSLISTOBSERVER_H
