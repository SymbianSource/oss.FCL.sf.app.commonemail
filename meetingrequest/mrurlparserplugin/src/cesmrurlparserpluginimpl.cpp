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



#include "emailtrace.h"
#include <EPos_CPosLandmark.h>
#include <ecom/implementationproxy.h>
#include <barsread.h>
#include <esmrurlparserplugindata.rsg>
#include <data_caging_path_literals.hrh>
#include <barsc.h>
#include <f32file.h>
#include <lbsposition.h>

#include "cesmrurlparserpluginimpl.h"
#include "esmrinternaluid.h"


_LIT( KResourceName, "esmrurlparserplugindata.rsc" );
_LIT( KResourceFileLocFormat, "r%02d" );
_LIT( KResourceFormat, "rsc" );

const TInt KCoordinateMaxLength = 10;
const TInt KNumberOfDecimals = 4;
const TUint KDecimalSeparator = '.';



// ======== MEMBER FUNCTIONS ========



// ---------------------------------------------------------------------------
// Non-leaving constructor
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl::CESMRUrlParserPluginImpl()
:   iIsInitialized(EFalse)
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
// ConstructL for leaving construction
// ---------------------------------------------------------------------------
//
void CESMRUrlParserPluginImpl::ConstructL()// codescanner::LFunctionCantLeave
    {
    FUNC_LOG;

    }


// ---------------------------------------------------------------------------
// Symbian style NewL constructor
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl* CESMRUrlParserPluginImpl::NewL()
    {
    FUNC_LOG;
    CESMRUrlParserPluginImpl* self = CESMRUrlParserPluginImpl::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Symbian style NewLC constructor
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl* CESMRUrlParserPluginImpl::NewLC()
    {
    FUNC_LOG;
    CESMRUrlParserPluginImpl* self = new( ELeave ) CESMRUrlParserPluginImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl::~CESMRUrlParserPluginImpl()
    {
    FUNC_LOG;
    iFile.Close();
    iFs.Close();
    }

// ---------------------------------------------------------------------------
// Finds location URL from given text input
// ---------------------------------------------------------------------------
//
TInt CESMRUrlParserPluginImpl::FindLocationUrl( const TDesC& aText, 
                                                      TPtrC& aUrl )

    {
    FUNC_LOG;
    TInt pos(0);
    TRAPD( error, pos = DoFindLocationUrlL( aText, aUrl ) );
    if ( error != KErrNone )
        {
        return error;
        }
    else
        {
        return pos;
        }
    }

// ---------------------------------------------------------------------------
// Finds location URL from given text input
// ---------------------------------------------------------------------------
//
TInt CESMRUrlParserPluginImpl::DoFindLocationUrlL( // codescanner::intleaves
                                       const TDesC& aText, 
                                       TPtrC& aUrl )
    {
    FUNC_LOG;
    //If this parserplugin instance is not yet initialized
    //init it now
    if( !iIsInitialized )
        {
        InitializeL();
        }

    //Read basestring from resource file
    HBufC* baseString = ReadResourceStringLC(R_QTN_LOCATION_URL);
    
    //Seach if base string is found from aText
    TInt urlLocation = aText.Find(*baseString);
    CleanupStack::PopAndDestroy(baseString);
    if( urlLocation < KErrNone )
        {
        User::Leave(KErrNotFound);
        }
    
    //Take out unnecessary part before URL and search if endmark (whitespace)
    //is found. If endmark is not found, 
    //then all the rest of descriptor is part of URL
    HBufC* urlAndAfterBuf = aText.Mid( urlLocation ).AllocLC();
    TInt urlAndAfterBufLength = urlAndAfterBuf->Length();
    TInt urlEndmarkLocation = KErrNotFound; 
    
    for( TInt i = 0; i < urlAndAfterBufLength; i++)
        {
        if( TChar( (*urlAndAfterBuf)[i] ).IsSpace() )
            {
            urlEndmarkLocation = i;
            break;
            }
        }
     
    if( urlEndmarkLocation == KErrNotFound )
        {
        urlEndmarkLocation = urlAndAfterBufLength;
        }
    
    //Take out part from beginning of URL to endmark
    HBufC* urlToEndMark = urlAndAfterBuf->Left( urlEndmarkLocation ).AllocLC();
        
    //Now we should have only URL left, check with "sieve" that it is about in 
    //right format
    HBufC* sieve = ReadResourceStringLC(R_QTN_LOCATION_SIEVE_URL);
    TInt sievedStartPoint = urlToEndMark->Match( *sieve );
    
    CleanupStack::PopAndDestroy(sieve);
    CleanupStack::PopAndDestroy(urlToEndMark);
    CleanupStack::PopAndDestroy(urlAndAfterBuf);
    
    if( sievedStartPoint == KErrNotFound )
        {
        User::Leave(KErrNotFound);
        }
    
    //Check that parameters are in right format
    TPtrC latValue;
    TPtrC lonValue;
    
    GetCoordinateParamValuesL( aText.Mid(urlLocation,urlEndmarkLocation),
                               latValue, lonValue );
    CheckCoordinateParamL( latValue );
    CheckCoordinateParamL( lonValue );
    
    //Set aURL to correspond URL part of aText and return with url position
    aUrl.Set( aText.Mid(urlLocation,urlEndmarkLocation));
    return urlLocation;
    }


// ---------------------------------------------------------------------------
// Creates location URL from landmark object
// ---------------------------------------------------------------------------
//
HBufC* CESMRUrlParserPluginImpl::CreateUrlFromLandmarkL( 
                                               const CPosLandmark& aLandmark )
    {
    FUNC_LOG;
    //If this parserplugin instance is not yet initialized
    //init it now
    if( !iIsInitialized )
        {
        InitializeL();
        }
    
    //Take longitude and latitude out of landmark
    TLocality position;
    User::LeaveIfError( aLandmark.GetPosition( position ) );
    TReal64 latitude = position.Latitude();
    TReal64 longitude = position.Longitude();
   
    //Define TReal format type
    TRealFormat format( KCoordinateMaxLength );
    format.iType = KRealFormatFixed;
    format.iPlaces = KNumberOfDecimals;
    format.iPoint = TChar(KDecimalSeparator);

    //Read strings from resourcefile
    HBufC* baseUrl = ReadResourceStringLC( R_QTN_LOCATION_URL );                        
    HBufC* lat = ReadResourceStringLC( R_QTN_LOCATION_URL_LATITUDE );               
    HBufC* separator = ReadResourceStringLC( R_QTN_LOCATION_URL_PARAM_SEPARATOR );        
    HBufC* lon = ReadResourceStringLC( R_QTN_LOCATION_URL_LONGITUDE );
    
    //Concatenate all strings and coordinates
    HBufC* url = HBufC::NewL( baseUrl->Length() + lat->Length() + separator->Length() 
                         + lon->Length() + format.iWidth + format.iWidth );
    TPtr pointer = url->Des();
    pointer.Append( *baseUrl );
    pointer.Append( *lat );
    pointer.AppendNum( latitude, format );
    pointer.Append( *separator );
    pointer.Append( *lon );
    pointer.AppendNum( longitude, format );
    
    CleanupStack::PopAndDestroy( lon );
    CleanupStack::PopAndDestroy( separator );
    CleanupStack::PopAndDestroy( lat  );
    CleanupStack::PopAndDestroy( baseUrl );
    
    //Transfer ownership of url
    return url;
    }


// ---------------------------------------------------------------------------
// Creates landmark object from location URL
// ---------------------------------------------------------------------------
//
CPosLandmark* CESMRUrlParserPluginImpl::CreateLandmarkFromUrlL( 
                                                           const TDesC& aUrl )
    {
    FUNC_LOG;
    //If this parserplugin instance is not yet initialized
    //init it now
    if( !iIsInitialized )
        {
        InitializeL();
        }
    
    //Read sieve from resourcefile and check if URL matches the sieveformat
    HBufC* sieve = ReadResourceStringLC( R_QTN_LOCATION_SIEVE_URL );
    TInt matchPos = User::LeaveIfError( aUrl.Match( *sieve ) );
    CleanupStack::PopAndDestroy( sieve );
    if( matchPos != 0 )
        {
        //URL was found but is not int the beginning of desc
        User::Leave( KErrArgument );
        }

    //Parse actual coordinate values out of url
    TPtrC latValue;
    TPtrC lonValue;
    GetCoordinateParamValuesL( aUrl, latValue, lonValue );
    
    //Check that parameters are in right format
    CheckCoordinateParamL( latValue );
    CheckCoordinateParamL( lonValue );

    //Convert parameters to TReal values
    TLex lexConverter( latValue );
    TReal64 realLatitude;
    lexConverter.Val( realLatitude );
     
    lexConverter.Assign( lonValue );
    TReal64 realLongitude;
    lexConverter.Val( realLongitude );
    
    //Create landmark with coordinatevalues
    CPosLandmark* landmark = CPosLandmark::NewLC();
    TLocality position;
    position.SetCoordinate( realLatitude, realLongitude );
    landmark->SetPositionL( position );
    CleanupStack::Pop( landmark );
    //transfer ownership
    return landmark;
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::LocateResourceFile
//
// For locating resource file    
// ----------------------------------------------------------------------------
// 
TInt CESMRUrlParserPluginImpl::LocateResourceFile( 
        const TDesC& aResource,
        const TDesC& aPath,
        TFileName &aResourceFile,
        RFs* aFs )
    {
    FUNC_LOG;
    TFindFile resourceFile( *aFs );
    TInt err = resourceFile.FindByDir(
            aResource,
            aPath );
    
    if ( KErrNone == err )
        {
        aResourceFile.Copy( resourceFile.File() );
        }
    else
        {
        const TChar KFileFormatDelim( '.' );
        TFileName locResourceFile;
        
        TInt pos = aResource.LocateReverse( KFileFormatDelim );
        if ( pos != KErrNotFound )
            {       
            locResourceFile.Copy( aResource.Mid(0, pos + 1) );
            
            TInt language( User::Language() );
            locResourceFile.AppendFormat( KResourceFileLocFormat, language );
            
            TFindFile resourceFile( *aFs );
            err = resourceFile.FindByDir(
                    locResourceFile,
                    aPath );            
            
            if ( KErrNone == err )
                {
                aResourceFile.Copy( resourceFile.File() );
                aResourceFile.Replace(
                        aResourceFile.Length() - KResourceFormat().Length(),
                        KResourceFormat().Length(),
                        KResourceFormat() );
                }
            }
        }
    

    return err; 
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::InitializeL
//
// Initializes resources
// ----------------------------------------------------------------------------
// 
void CESMRUrlParserPluginImpl::InitializeL()
    {
    FUNC_LOG;
    if( iIsInitialized )
        {
        return;
        }
    //Connect to RFs session
    User::LeaveIfError( iFs.Connect( KFileServerDefaultMessageSlots ) );
    
    //Find and open resource file containing URL strings
    TFileName fileName;
    User::LeaveIfError( LocateResourceFile( KResourceName,
                                            KDC_RESOURCE_FILES_DIR,
                                            fileName,
                                            &iFs ) );
    iFile.OpenL( iFs, fileName );
    iFile.ConfirmSignatureL();
    iIsInitialized = ETrue;
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::ReadResourceStringLC 
// Reads resource string from specified id
// ----------------------------------------------------------------------------
// 
HBufC* CESMRUrlParserPluginImpl::ReadResourceStringLC( TInt aResourceId )
    {
    FUNC_LOG;
    HBufC8* string = iFile.AllocReadLC(aResourceId);
    iReader.SetBuffer(string);
    HBufC* stringBuffer = iReader.ReadTPtrC().AllocL();
    CleanupStack::PopAndDestroy( string );
    CleanupStack::PushL( stringBuffer );
    //stringBuffer ownership is transfered
    return stringBuffer; 
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::CheckCoordinateParam 
//
// Checks if coordinate parameter (lon or lat) is in correct format
// Leaves if not
// Correct form is:
// -minus sign allowed only in first position
// -only one decimalseparator sign allowed
// -only digits allowed
// -aParam length not allowed to be zero    
// ----------------------------------------------------------------------------
// 
void CESMRUrlParserPluginImpl::CheckCoordinateParamL( const TDesC& aParam )
    {
    FUNC_LOG;
    if( aParam.Length() == 0 )
        {
        User::Leave( KErrArgument );
        }
    
    //check that aParam contains only digits and only one decimalseparator
    //and minus sign is first, if it exists
    TChar character;
    TBool decimalSeparatorFound = EFalse;
    TLex lex;
    lex.Assign( aParam );
    const TUint KMinusSign = '-';
    
    for( TInt i = 0; i < aParam.Length(); i++)
        {
        character = lex.Get();
        //checks if first character is minus sign and continues if it is
        if( i == 0 && (TUint)character == KMinusSign)
            {
            continue;
            }
        //check that only one decimalseparator exists
        if( (TUint)character == KDecimalSeparator )
            {
            if( decimalSeparatorFound )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                decimalSeparatorFound = ETrue;
                }
            }
        //check that character is either digit or decimalseparator
        if( !( character.IsDigit() ) && (TUint)character != KDecimalSeparator )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::GetCoordinateParamValuesL 
// Returns longitude and latitude if found correctly
// ----------------------------------------------------------------------------
// 
void CESMRUrlParserPluginImpl::GetCoordinateParamValuesL( const TDesC& aUrl,
                                                          TPtrC& aLatitude,
                                                          TPtrC& aLongitude )
    {
    FUNC_LOG;
    //Read latitude and longitude strings from resourcefile
    HBufC* lat = ReadResourceStringLC( R_QTN_LOCATION_URL_LATITUDE );
    HBufC* lon = ReadResourceStringLC( R_QTN_LOCATION_URL_LONGITUDE );
    HBufC* separator = ReadResourceStringLC( R_QTN_LOCATION_URL_PARAM_SEPARATOR );
    
    //Find out if lat and lon params and separator exists in aUrl
    TInt latPos = aUrl.Find( *lat );
    TInt lonPos = aUrl.Find( *lon );
    TInt separatorPos = aUrl.Find( *separator );
    if( latPos == KErrNotFound || lonPos == KErrNotFound 
         || separatorPos == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    
    //takes from aUrl parts with actual coordinate data
    aLatitude.Set( aUrl.Mid( latPos + lat->Length(), separatorPos - latPos - lat->Length() ) );
    aLongitude.Set( aUrl.Right( aUrl.Length() - separatorPos - 1 - lon->Length() ) );

    CleanupStack::PopAndDestroy( separator );
    CleanupStack::PopAndDestroy( lon );
    CleanupStack::PopAndDestroy( lat );
    }
//EOF

