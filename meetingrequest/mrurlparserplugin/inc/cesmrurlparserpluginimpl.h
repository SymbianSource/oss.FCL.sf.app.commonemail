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


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>                // RFs
#include <barsc.h>                  // RResourceFile
#include <barsread.h>               // TResourceReader

#include "cesmrurlparserplugin.h"

class CPosLandmark;


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
    
private:

    CESMRUrlParserPluginImpl();

    void ConstructL();
    
    TInt DoFindLocationUrlL( const TDesC& aText, 
                             TPtrC& aUrl );
    
    TInt LocateResourceFile( 
            const TDesC& aResource,
            const TDesC& aPath,
            TFileName &aResourceFile,
            RFs* aFs );
    
    void InitializeL();
    
    HBufC* ReadResourceStringLC( TInt aResourceId );
    
    void CheckCoordinateParamL( const TDesC& aParam );
    
    void GetCoordinateParamValuesL( const TDesC& aUrl, TPtrC& aLatitude,
                                    TPtrC& aLongitude );
private: // data

    /**
     * Tells if this instance is yet initialized
     */
    TBool iIsInitialized;
    
    /**
     * File server session
     */
    RFs iFs;
    
    /**
     * Resource file
     */
    RResourceFile iFile;
    
    /**
     * Resource file reader
     */    
    TResourceReader iReader;
    };


#endif //   CESMRURLPARSERPLUGINIMPL_H
