/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Parser class for parsing landmarks to url and vice versa
*
*/

#ifndef CMRURLPARSEREXTENSION_H
#define CMRURLPARSEREXTENSION_H

#include "e32base.h"

class CPosLandmark;

/**
 *  Parser class for parsing URL to landmark and vice versa
 *
 */
class CMRUrlParserExtension : public CBase
    {

public:

    static CMRUrlParserExtension* NewL();
    static CMRUrlParserExtension* NewLC();


   /**
    * Destructor.
    */
    virtual ~CMRUrlParserExtension();

    /**
     * Finds location URL from given text input
     *
     * @param aText text where URL is searched from
     * @param aUrl points to founded URL after execution
     * @param aPos on return URL start position.
     * @leave KErrNotFound/KErrArgument or other
     *         unexpected Symbian wide errorcode
     */
    void FindLocationUrlL( const TDesC& aText, TPtrC& aUrl, TInt& aPos );

    /**
     * Creates landmark object from location URL
     * May leave with KErrNotFound/KErrArgument or other Symbian wide errorcode
     *
     * @param aUrl contains location URL from which landmark is created
     * @return pointer to created landmark object, caller takes ownership
     */
    CPosLandmark* CreateLandmarkFromUrlL( const TDesC& aUrl );

private:

    CMRUrlParserExtension();

private: // data

    };


#endif // CMRURLPARSEREXTENSION_H
