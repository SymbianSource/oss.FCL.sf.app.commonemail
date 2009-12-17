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
* Description:  Definition of the class PbkxRclUtils.
*
*/


#ifndef PBKXRCLUTILS_H
#define PBKXRCLUTILS_H

#include <CPbkContactItem.h>
//<cmail>
#include "tpbkxremotecontactlookupprotocolaccountid.h"
//</cmail>

class CPbkContactEngine;

/**
* Class that offers static helper methods.
*
* @lib pbkxrclengine.lib
* @since S60 3.1
*/
class PbkxRclUtils 
    {
public: // static methods

    /**
    * Returns text of the given field of the given contact.
    *
    * If there are multiple fields with given id, first one is returned.
    * 
    * @param aItem Contact item which field text is asked.
    * @param aId Field id.
    * @return Text of the field or empty text if none is found.
    */
    static HBufC* FieldTextL( const CPbkContactItem* aItem, TPbkFieldId aId );

    /**
    * Returns tet of the given field.
    *
    * @param aField Field which text is wanted.
    * @return Field text.
    */
    static HBufC* FieldTextL( TPbkContactItemField& aField );

    /**
    * Returns the text in the given field, if such field exists and it's type is
    * text.
    *
    * If there are more than one fields with given type, the first one is returned.
    *
    * @param aCard Contact card.
    * @param aFieldType Type of the field.
    */
    static TPtrC FieldText( const CContactCard* aCard, TFieldType aFieldType );

    /**
    * Gets the default protocol account id.
    *
    * Leaves with KErrNotFound if default account id is not set.
    */
    static TPbkxRemoteContactLookupProtocolAccountId DefaultProtocolAccountIdL();

    /**
    * Stores default protocol account id.
    *
    * @param aId Account id to be stored.
    */
    static void StoreDefaultProtocolAccountIdL(
        TPbkxRemoteContactLookupProtocolAccountId aId );

    /**
    * Returns ETrue if given text contains at least one alpha 
    * characters.
    *
    * @param aText Text to be investigated.
    * @return ETrue if given text contains at least one alpha character.
    */
    static TBool HasAlphaCharsInString( const TDesC& aText );

    /**
    * Creates new contact item of the given contact card.
    *
    * @param aCard Contact card.
    * @param aEngine Contact engine.
    * @return Created item. Ownership is transferred.
    */
    static CPbkContactItem* CreateContactItemL( 
        CContactCard* aCard, 
        CPbkContactEngine& aEngine );

    };

#endif
