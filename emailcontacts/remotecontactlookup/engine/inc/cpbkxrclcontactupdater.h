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
* Description:  Definition of the class CPbkxRclContactUpdater
*
*/


#ifndef CPBKXRCLCONTACTUPDATER_H
#define CPBKXRCLCONTACTUPDATER_H

#include <e32base.h>

class CPbkContactEngine;
class CPbkContactItem;
class TPbkContactItemField;
class CPbkFieldInfo;

/**
* Class that updates existing contact data.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class CPbkxRclContactUpdater : public CBase
    {
public: // constructors and destructor

    /**
    * Constructs contact updater.
    *
    * @param aEngine Contact engine.
    * @return Created object.
    */
    static CPbkxRclContactUpdater* NewL( CPbkContactEngine& aEngine );

    /**
    * Constructs contact updater.
    *
    * Pointer to the created object is left on cleanupstack.
    *
    * @param aEngine Contact engine.
    * @return Created object.
    */
    static CPbkxRclContactUpdater* NewLC( CPbkContactEngine& aEngine );
    
    /**
    * Destructor.
    */
    virtual ~CPbkxRclContactUpdater();

public: // new methods

    /**
    * Updates the given field to the user selected contact.
    *
    * @param aField Field to be updated to the contact.
    *
    * @return Update text shown to the user. Ownership is transferred.
    */
    HBufC* UpdateContactL( TPbkContactItemField& aField );

private: // methods used internally

    /**
    * Constructs the list of possible fields and if there are several,
    * user is asked to select a field.
    *
    * If there is already selected field, user is asked if he wants to
    * replace the existing field.
    *
    * NULL is returned if user cancels the updating.
    *
    * @param aContactItem Contact for which field is updated.
    * @param aFieldInfo Field info of the field which data is updated
    *                   to the contact.
    * @return Field which is updated.
    */
    TPbkContactItemField* SelectUpdatedFieldL(
        CPbkContactItem& aContactItem,
        CPbkFieldInfo& aFieldInfo );

    /**
    * Constructs update text shown to the user after successfull updating.
    *
    * @param aContactItem Contact item that was updated.
    * @param aField Updated field.
    * @return Update text.
    */
    HBufC* ConstructUpdatedTextL( 
        CPbkContactItem& aContactItem,
        TPbkContactItemField& aField );

    /**
    * Constructs a list of possible fields of given type.
    *
    * There are three possible types: home, work, and none. If at least two
    * types are found, user is asked to select a type.
    *
    * @param aFieldInfo Source field info of the field that is updated 
    *                   to the contact.
    * @param aFieldCount Number of possible fields.
    * @return Destination field info which is updated. 
    *         Ownership is not transferred. 
    */
    CPbkFieldInfo* SelectFieldL(
        CPbkFieldInfo& aFieldInfo,
        TInt& aFieldCount );

private: // constructors

    /**
    * Constructor.
    *
    * @param aEngine Contact engine.
    */
    CPbkxRclContactUpdater( CPbkContactEngine& aEngine );

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private: // data

    // Contact engine.
    CPbkContactEngine& iEngine;
    // Offset to resource file.
    TInt iResourceFileOffset;
    };

#endif

