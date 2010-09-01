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
* Description:  Parser class extension for parsing landmarks to url and vice versa
*
*/



#include "cmrurlparserextension.h"

#include <EPos_CPosLandmark.h>
#include <lbsposition.h>
#include <utf.h>
#include <finditemengine.h>
#include <calentry.h>

#include "emailtrace.h"

namespace
{
// Old location base URL format
_LIT( KLocationUrl, "http://www.ovi.com/maps/" );

// Latitude parameter
_LIT( KUrlParamLatitude, "lat=" );

// Parameter separator
_LIT( KUrlParamSeparator, "&" );

// Longitude parameter
_LIT( KUrlParamLongitude, "lon=" );

// Location sieve URL
_LIT( KLocationSieveUrl, "http://www.ovi.com/maps/lat=*&lon=*" );

const TUint KDecimalSeparator = '.';
const TUint KMinusSign = '-';

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// CheckCoordinateParamL
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
void CheckCoordinateParamL( const TDesC& aParam )
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

    for( TInt i = 0; i < aParam.Length(); i++)
        {
        character = lex.Get();
        //checks if first character is minus sign and continues if it is
        if( i == 0 && (TUint)character == KMinusSign)
            {
            continue;
            }
        
        //check that only one decimal separator exists
        if ( (TUint)character == KDecimalSeparator )
            {
            if ( decimalSeparatorFound )
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
// GetCoordinateParamValuesL
// Returns longitude and latitude if found correctly
// ----------------------------------------------------------------------------
//
void GetCoordinateParamValuesL( const TDesC& aUrl,
                                TPtrC& aLatitude,
                                TPtrC& aLongitude )
    {
    FUNC_LOG;
    
    //Find out if lat and lon params and separator exists in aUrl
    TInt latPos = aUrl.Find( KUrlParamLatitude );
    TInt lonPos = aUrl.Find( KUrlParamLongitude );
    TInt separatorPos = aUrl.Find( KUrlParamSeparator );
    
    if( latPos == KErrNotFound || lonPos == KErrNotFound
         || separatorPos == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }

    //takes from aUrl parts with actual coordinate data
    aLatitude.Set( aUrl.Mid(
            latPos + KUrlParamLatitude().Length(),
            separatorPos - latPos - KUrlParamLatitude().Length() ) );
    
    aLongitude.Set( aUrl.Right(
            aUrl.Length() - separatorPos - 1 - KUrlParamLongitude().Length() ) );
    }
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMRUrlParserExtension::CMRUrlParserExtension
// ---------------------------------------------------------------------------
//
CMRUrlParserExtension::CMRUrlParserExtension()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRUrlParserExtension::NewL
// ---------------------------------------------------------------------------
//
CMRUrlParserExtension* CMRUrlParserExtension::NewL()
    {
    FUNC_LOG;
    
    CMRUrlParserExtension* self = CMRUrlParserExtension::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMRUrlParserExtension::NewLC
// ---------------------------------------------------------------------------
//
CMRUrlParserExtension* CMRUrlParserExtension::NewLC()
    {
    FUNC_LOG;
    
    CMRUrlParserExtension* self = new( ELeave ) CMRUrlParserExtension;
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMRUrlParserExtension::~CMRUrlParserExtension
// ---------------------------------------------------------------------------
//
CMRUrlParserExtension::~CMRUrlParserExtension()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CMRUrlParserExtension::FindLocationUrl
// Finds location URL from given text input
// ---------------------------------------------------------------------------
//
void CMRUrlParserExtension::FindLocationUrlL(
        const TDesC& aText,
        TPtrC& aUrl,
        TInt& aPos)

    {
    FUNC_LOG;
    
    //Seach if base string is found from aText
    TInt urlLocation = aText.FindF( KLocationUrl );
    if ( urlLocation < KErrNone )
        {
        User::Leave(KErrNotFound);
        }

    //Take out unnecessary part before URL and search if endmark (whitespace)
    //is found. If endmark is not found,
    //then all the rest of descriptor is part of URL
    TPtrC urlAndAfter = aText.Mid( urlLocation );
    TInt urlAndAfterLength = urlAndAfter.Length();
    TInt urlEndmarkLocation = KErrNotFound;

    for( TInt i = 0; i < urlAndAfterLength; i++)
        {
        if( TChar( urlAndAfter[i] ).IsSpace() )
            {
            urlEndmarkLocation = i;
            break;
            }
        }

    if( urlEndmarkLocation == KErrNotFound )
        {
        urlEndmarkLocation = urlAndAfterLength;
        }

    //Take out part from beginning of URL to endmark
    TPtrC urlToEndMark = urlAndAfter.Left( urlEndmarkLocation );

    //Now we should have only URL left, check with "sieve" that it is about in
    //right format
    TInt sievedStartPoint = urlToEndMark.MatchF( KLocationSieveUrl );

    if( sievedStartPoint == KErrNotFound )
        {
        User::Leave(KErrNotFound);
        }

    //Check that parameters are in right format
    TPtrC latValue;
    TPtrC lonValue;

    GetCoordinateParamValuesL(
            urlToEndMark,
            latValue,
            lonValue );
    CheckCoordinateParamL( latValue );
    CheckCoordinateParamL( lonValue );

    //Set aURL to correspond URL part of aText and aPos with url position
    aUrl.Set( urlToEndMark );
    aPos = urlLocation;
    }

// ---------------------------------------------------------------------------
// CMRUrlParserExtension::CreateLandmarkFromUrlL
// Creates landmark object from location URL
// ---------------------------------------------------------------------------
//
CPosLandmark* CMRUrlParserExtension::CreateLandmarkFromUrlL(
        const TDesC& aUrl )
    {
    FUNC_LOG;
    
    TInt matchPos = User::LeaveIfError( aUrl.MatchF( KLocationSieveUrl ) );
    
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

//EOF

