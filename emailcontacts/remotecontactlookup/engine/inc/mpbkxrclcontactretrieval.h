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
* Description:  Definition of the class MPbkxRclContactRetrieval.
*
*/


#ifndef MPBKXRCLCONTACTRETRIEVAL_H
#define MPBKXRCLCONTACTRETRIEVAL_H

/**
* Class through which contact details can be updated.
*/
class MPbkxRclContactRetrieval
    {
public: // pure virtual methods

    /**
    * Retrieves details of the contact with given index.
    *
    * @param aContactIndex Index of the contact for which details are retrieved.
    * @param aWaitNoteText Text shown in wait note.
    * @return ContactCard with details retrieved or NULL, if something went wrong.
    */
    virtual CContactCard* RetrieveDetailsL( 
        TInt aContactIndex,
        const TDesC& aWaitNoteText ) = 0;

    /**
    * Returns the number of contacts.
    *
    * @return The number of contacts.
    */
    virtual TInt ContactCount() const = 0;
    
    /**
    * Sets the index of currently selected contact.
    *
    * @param aIndex Index of the currently selected contact.
    */
    virtual void SetSelectedContactL( TInt aIndex ) = 0;

    };

#endif
