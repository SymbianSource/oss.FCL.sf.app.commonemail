/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CESMRURLPARSERPLUGINIMPL_H
#define CESMRURLPARSERPLUGINIMPL_H

#include "cesmrurlparserplugin.h"

class CPosLandmark;
class CPosLandmarkParser;
class CPosLandmarkEncoder;
class CMRUrlParserExtension;


/**
 *  Parser class for parsing URL to landmark and vice versa
 *
 */
class CESMRUrlParserPluginImpl : public CESMRUrlParserPlugin
    {

public:

    static CESMRUrlParserPluginImpl* NewL();
    static CESMRUrlParserPluginImpl* NewLC();


   /**
    * Destructor.
    */
    virtual ~CESMRUrlParserPluginImpl();

    /**
     * Finds location URL from given text input
     *
     * @param aText text where URL is searched from
     * @param aUrl points to founded URL after execution
     * @return URL start position in aText,
     *         if not found returns KErrNotFound/KErrArgument or other
     *         unexpected Symbian wide errorcode
     */
    TInt FindLocationUrl( const TDesC& aText, TPtrC& aUrl );

    /**
     * Creates location URL from landmark object
     * May leave with KErrNotFound/KErrArgument or other Symbian wide errorcode
     *
     * @param aLandmark contains landmark object that is parsed to location URL
     * @return pointer to created location URL, caller takes ownership
     */
    HBufC* CreateUrlFromLandmarkL( const CPosLandmark& aLandmark );

    /**
     * Creates landmark object from location URL
     * May leave with KErrNotFound/KErrArgument or other Symbian wide errorcode
     *
     * @param aUrl contains location URL from which landmark is created
     * @return pointer to created landmark object, caller takes ownership
     */
    CPosLandmark* CreateLandmarkFromUrlL( const TDesC& aUrl );

    /**
     * Converts valid location URL to vCal GEO value.
     * May leave with KErrNotFound/KErrArgument or other Symbian wide errorcode
     *
     * @param aUrl location URL to convert
     * @return new GEO value
     */
    CCalGeoValue* CreateGeoValueLC( const TDesC& aUrl );

private:

    CESMRUrlParserPluginImpl();

    CPosLandmarkParser& InitializeParserL();
    
    CPosLandmarkEncoder& InitializeEncoderL();
    
    void DoFindLocationUrlL( const TDesC& aText,
                             TInt& aPos,
                             TPtrC& aUrl );
    
    CMRUrlParserExtension& ExtensionL();
    
    CPosLandmark* CreateLandmarkFromUrlInternalL( const TDesC& aUrl );

private: // data

    /**
     *  Landmark parser. Own.
     */
    CPosLandmarkParser* iParser;
    
    /**
     * Landmark encoder. Own.
     */
    CPosLandmarkEncoder* iEncoder;
    
    /**
     * Parser extension. Own.
     */
    CMRUrlParserExtension* iExtension;
    };


#endif //   CESMRURLPARSERPLUGINIMPL_H
