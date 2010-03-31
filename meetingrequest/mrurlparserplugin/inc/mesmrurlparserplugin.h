/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface definition for Location URL Parser plug-in
*
*/


#ifndef MESMRURLPARSERPLUGIN_H
#define MESMRURLPARSERPLUGIN_H



class CPosLandmark;
class CCalGeoValue;

/**
 *  MESMRUrlParserPlugin defines the interface for Location URL Parser
 *
 */
class MESMRUrlParserPlugin
    {

public:
    /**
     * Finds location URL from given text input
     *
     * @param aText text where URL is searched from
     * @param aUrl points to founded URL after execution
     * @return URL start position in aText, if not found returns KErrNotFound
     */
    virtual TInt FindLocationUrl( const TDesC& aText, TPtrC& aUrl ) = 0;

    /**
     * Creates location URL from landmark object
     *
     * @param aLandmark contains landmark object that is parsed to location URL
     * @return pointer to created location URL
     */
    virtual HBufC* CreateUrlFromLandmarkL( const CPosLandmark& aLandmark ) = 0;

    /**
     * Creates landmark object from location URL
     *
     * @param aUrl contains location URL from which landmark is created
     * @return pointer to created landmark object
     */
    virtual CPosLandmark* CreateLandmarkFromUrlL( const TDesC& aUrl ) = 0;

    /**
     * Converts valid location URL to vCal GEO value.
     * May leave with KErrNotFound/KErrArgument or other Symbian wide errorcode
     *
     * @param aUrl location URL to convert
     * @return new GEO value
     */
    virtual CCalGeoValue* CreateGeoValueLC( const TDesC& aUrl ) = 0;

protected:

   /**
    * Destructor.
    */
    virtual ~MESMRUrlParserPlugin(){}
    };

#endif //   MESMRURLPARSERPLUGIN_H
