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
* Description:  Implementation of address selection parameters.
*
*/


#include "emailtrace.h"
#include "TFscAddressSelectParams.h"

// Phonebook 2
#include <MPbk2ContactNameFormatter.h>
#include "CFscFieldPropertyArray.h"

// Virtual Phonebook
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <CVPbkContactManager.h>


// --------------------------------------------------------------------------
// TFscAddressSelectParams::TFscAddressSelectParams
// --------------------------------------------------------------------------
//
 TFscAddressSelectParams::TFscAddressSelectParams
        ( MVPbkStoreContact& aContact,
          const CVPbkContactManager& aContactManager,
          MPbk2ContactNameFormatter& aNameFormatter,
          const CFscFieldPropertyArray& aFieldPropertyArray,
          TResourceReader& aReader, TInt aTitleResId ) :
            iContact( aContact ),
            iContactManager( aContactManager ),
            iNameFormatter( aNameFormatter ),
            iFieldPropertyArray( aFieldPropertyArray ),
            iResReader( aReader ),
            iTitleResId( aTitleResId ),
            iFocusedField( NULL ),
            iDefaultPriorities( NULL ),
            iUseDefaultDirectly( EFalse ),
            iIncludeContactNameInPrompt( ETrue ),
            iSuppressWarnings( EFalse )
    {
    FUNC_LOG;
    }

// --------------------------------------------------------------------------
// TFscAddressSelectParams::SetFocusedField
// --------------------------------------------------------------------------
//
 void TFscAddressSelectParams::SetFocusedField
        ( const MVPbkStoreContactField* aFocusedField )
    {
    FUNC_LOG;
    iFocusedField = aFocusedField;
    }

// --------------------------------------------------------------------------
// TFscAddressSelectParams::SetDefaultPriorities
// --------------------------------------------------------------------------
//
 void TFscAddressSelectParams::SetDefaultPriorities
        ( const RVPbkContactFieldDefaultPriorities& aDefaultPriorities )
    {
    FUNC_LOG;
    iDefaultPriorities = &aDefaultPriorities;
    }
 
// --------------------------------------------------------------------------
// TPbk2AddressSelectParams::SetUseDefaultDirectly
// --------------------------------------------------------------------------
//
void TFscAddressSelectParams::SetUseDefaultDirectly
         ( TBool aUseDefaultDirectly )
     {
    FUNC_LOG;
     iUseDefaultDirectly = aUseDefaultDirectly;
     }

// End of File

