/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Caches language specific text fields from a
*  resource file and provides access to them.
*
*
*/

#ifndef CESMRAGNVERSIT2STRINGPROVIDER_H
#define CESMRAGNVERSIT2STRINGPROVIDER_H

//System includes
#include <e32base.h>                // CBase
#include <f32file.h>                // RFs
#include <barsc.h>                  // RResourceFile
#include <barsread.h>               // TResourceReader

//User includes
#include "esmragnversit2strings.hrh"    // TStringId

/**
 * Class to load and cache strings from resource files.
 * @internalTechnology
 */
class CESMRAgnVersit2StringProvider : public CBase
    {
public:     // Construction/destruction.
    /**
     * Creates a new CESMRAgnVersit2StringProvider and returns it
     * @return pointer to the new CESMRAgnVersit2StringProvider
     * @internalTechnology
     */
    static CESMRAgnVersit2StringProvider* NewL();
    
    /**
     * C++ Destructor.
     */
    ~CESMRAgnVersit2StringProvider();

public:     // Methods.
    /**
     * Returns a string based on the TStringId parameter. If the array item at index
     * aStringId is NULL, then that particular string has not been requested yet. If
     * this is the case, then the string is retrieved from the resource file and set
     * to aIndex in the string array.
     * @param aStringId string index to return
     * @return reference to a descriptor containing the string
     * @internalTechnology
     */
    const TDesC& StringL(TStringId aStringId);

private:    // Construction.
    CESMRAgnVersit2StringProvider();
    void ConstructL();

private:    // Methods.
    void ReadStringFromResourceL(TStringId aStringId);
    void ReadAllStringsL();

private:    // Members.
    RFs iFs;
    HBufC* iResourceBuffer;
    RResourceFile iResourceFile;
    TResourceReader iResourceReader;
    RPointerArray<HBufC> iStringArray;
    };

#endif

// End of file.
