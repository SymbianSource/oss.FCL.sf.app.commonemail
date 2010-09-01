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
* Description:  Interface for dynamic properties of the contact class
 *
*/

#ifndef MFSCCONTACTFIELDDYNAMICPROPERTIES_H
#define MFSCCONTACTFIELDDYNAMICPROPERTIES_H

class MVPbkBaseContactField;

/**
 * Interface for dynamic properties of the contact class
 */
class MFscContactFieldDynamicProperties
    {
public:
    /**
     * Is field hidden.
     *
     * @param aField, Contact field 
     * @return ETrue if hidden else EFalse
     */
    virtual TBool IsHiddenField(const MVPbkBaseContactField& aField) const =0;

protected:
    virtual ~MFscContactFieldDynamicProperties()
        {
        }
    };

#endif // MFSCCONTACTFIELDDYNAMICPROPERTIES_H
// End of file

