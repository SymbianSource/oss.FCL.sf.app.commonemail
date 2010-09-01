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
* Description:  Implementation of the class CFscContactFieldDynamicProperties
 *
*/


#include "emailtrace.h"
#include "CFscContactFieldDynamicProperties.h"

// From VirtualPhonebook
#include <MVPbkBaseContactField.h>
#include <MVPbkFieldType.h>

CFscContactFieldDynamicProperties* CFscContactFieldDynamicProperties::NewLC()
    {
    FUNC_LOG;
    CFscContactFieldDynamicProperties* self = new (ELeave) CFscContactFieldDynamicProperties();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CFscContactFieldDynamicProperties::CFscContactFieldDynamicProperties()
    {
    FUNC_LOG;
    // Do nothing
    }

CFscContactFieldDynamicProperties::~CFscContactFieldDynamicProperties()
    {
    FUNC_LOG;
    iHiddenFieldResourceIds.Close();
    }

void CFscContactFieldDynamicProperties::ConstructL()
    {
    FUNC_LOG;
    // Do nothing
    }

TBool CFscContactFieldDynamicProperties::IsHiddenField(
        const MVPbkBaseContactField& aField) const
    {
    FUNC_LOG;
    if (iHiddenFieldResourceIds.Find(aField.BestMatchingFieldType()->FieldTypeResId() ) != KErrNotFound)
        {
        return ETrue;
        }
    return EFalse;
    }

// End of file

