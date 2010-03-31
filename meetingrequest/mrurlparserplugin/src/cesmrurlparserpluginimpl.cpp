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



#include "cesmrurlparserpluginimpl.h"
#include "cmrurlparserextension.h"

#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkEncoder.h>
#include <EPos_CPosLandmarkParser.h>
#include <EPos_CPosLmOperation.h>
#include <lbsposition.h>
#include <EscapeUtils.h>
#include <finditemengine.h>
#include <calentry.h>

#include "emailtrace.h"

namespace
{
_LIT8( KLandmarkUrlMimeType, "maps.ovi.com" );
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::CESMRUrlParserPluginImpl
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl::CESMRUrlParserPluginImpl()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::NewL
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
// CESMRUrlParserPluginImpl::NewLC
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl* CESMRUrlParserPluginImpl::NewLC()
    {
    FUNC_LOG;

    CESMRUrlParserPluginImpl* self = new( ELeave ) CESMRUrlParserPluginImpl;
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::~CESMRUrlParserPluginImpl
// ---------------------------------------------------------------------------
//
CESMRUrlParserPluginImpl::~CESMRUrlParserPluginImpl()
    {
    FUNC_LOG;

    delete iParser;
    delete iEncoder;
    delete iExtension;

    ReleaseLandmarkResources();
    }

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::FindLocationUrl
// Finds location URL from given text input
// ---------------------------------------------------------------------------
//
TInt CESMRUrlParserPluginImpl::FindLocationUrl(
        const TDesC& aText,
        TPtrC& aUrl )

    {
    FUNC_LOG;

    TInt pos(0);
    TRAPD( error, DoFindLocationUrlL( aText, pos, aUrl ) );
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
// CESMRUrlParserPluginImpl::CreateUrlFromLandmarkL
// Creates location URL from landmark object
// ---------------------------------------------------------------------------
//
HBufC* CESMRUrlParserPluginImpl::CreateUrlFromLandmarkL(
        const CPosLandmark& aLandmark )
    {
    FUNC_LOG;

    CPosLandmarkEncoder& encoder = InitializeEncoderL();
    CBufBase* buffer = encoder.SetUseOutputBufferL();
    CleanupStack::PushL( buffer );
    encoder.AddLandmarkL( aLandmark );
    CPosLmOperation* operation = encoder.FinalizeEncodingL();
    ExecuteAndDeleteLD( operation );

    // Convert URL to Unicode and escape encode non-ASCII characters
    HBufC* unicode = EscapeUtils::ConvertToUnicodeFromUtf8L( buffer->Ptr( 0 ) );
    CleanupStack::PushL( unicode );
    HBufC* url = EscapeUtils::EscapeEncodeL( *unicode, EscapeUtils::EEscapeNormal );

    CleanupStack::PopAndDestroy( 2, buffer ); // unicode

    //Transfer ownership of url
    return url;
    }


// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::CreateLandmarkFromUrlL
// Creates landmark object from location URL
// ---------------------------------------------------------------------------
//
CPosLandmark* CESMRUrlParserPluginImpl::CreateLandmarkFromUrlL(
        const TDesC& aUrl )
    {
    FUNC_LOG;

    CPosLandmark* landmark = NULL;

    // Try to create landmark using landmark parser
    TRAPD( error, landmark = CreateLandmarkFromUrlInternalL( aUrl ) );

    if ( error )
        {
        // Try extension
        landmark = ExtensionL().CreateLandmarkFromUrlL( aUrl );
        }

    //transfer ownership
    return landmark;
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::CreateGeoValueLC
// Converts URL to Calendar GEO property
// ----------------------------------------------------------------------------
//
CCalGeoValue* CESMRUrlParserPluginImpl::CreateGeoValueLC( const TDesC& aUrl )
    {
    FUNC_LOG;

    // Convert URL to landmark
    CPosLandmark* landmark = CreateLandmarkFromUrlL( aUrl );
    CleanupStack::PushL( landmark );

    // Get position data from landmark
    TLocality position;
    User::LeaveIfError( landmark->GetPosition( position ) );
    CleanupStack::PopAndDestroy( landmark );

    // Convert position data to GEO value
    CCalGeoValue* geoVal = CCalGeoValue::NewL();
    CleanupStack::PushL( geoVal );
    geoVal->SetLatLongL( position.Latitude(), position.Longitude() );

    return geoVal;
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::InitializeParserL
// ----------------------------------------------------------------------------
//
CPosLandmarkParser& CESMRUrlParserPluginImpl::InitializeParserL()
    {
    FUNC_LOG;

    if ( !iParser )
        {
        iParser = CPosLandmarkParser::NewL( KLandmarkUrlMimeType );
        }

    return *iParser;
    }

// ----------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::InitializeEncoderL
// ----------------------------------------------------------------------------
//
CPosLandmarkEncoder& CESMRUrlParserPluginImpl::InitializeEncoderL()
    {
    FUNC_LOG;

    delete iEncoder;
    iEncoder = NULL;
    iEncoder = CPosLandmarkEncoder::NewL( KLandmarkUrlMimeType );
    return *iEncoder;
    }

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::DoFindLocationUrlL
// Finds location URL from given text input
// ---------------------------------------------------------------------------
//

void CESMRUrlParserPluginImpl::DoFindLocationUrlL(
        const TDesC& aText,
        TInt& aPos,
        TPtrC& aUrl )
    {
    FUNC_LOG;

    aPos = KErrNotFound;
    aUrl.Set( KNullDesC );

    // Search URL from given text
    CFindItemEngine* itemEngine = CFindItemEngine::NewL(
            aText,
            CFindItemEngine::EFindItemSearchURLBin );
    CleanupStack::PushL (itemEngine );

    // For each found item
    CFindItemEngine::SFoundItem item;
    itemEngine->Item( item );

    TInt itemCount = itemEngine->ItemCount();
    for ( TInt i = 0; i < itemCount; ++i )
        {
        TPtrC url = aText.Mid( item.iStartPos, item.iLength );

        CPosLandmark* landmark = NULL;
        TRAPD( error, landmark = CreateLandmarkFromUrlL( url ); )
        delete landmark;

        if ( !error ) // Location url found
            {
            aPos = item.iStartPos;
            aUrl.Set( url );
            // Stop iteration
            break;
            }
        else
            {
            itemEngine->NextItem( item );
            }
        }

    CleanupStack::PopAndDestroy( itemEngine );

    if ( aPos <  0 )
        {
        ExtensionL().FindLocationUrlL( aText, aUrl, aPos );
        }
    }

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::ExtensionL
// ---------------------------------------------------------------------------
//
CMRUrlParserExtension& CESMRUrlParserPluginImpl::ExtensionL()
    {
    FUNC_LOG;

    if ( !iExtension )
        {
        iExtension = CMRUrlParserExtension::NewL();
        }

    return *iExtension;
    }

// ---------------------------------------------------------------------------
// CESMRUrlParserPluginImpl::CreateLandmarkFromUrlInternalL
// Creates landmark object from location URL
// ---------------------------------------------------------------------------
//
CPosLandmark* CESMRUrlParserPluginImpl::CreateLandmarkFromUrlInternalL(
        const TDesC& aUrl )
    {
    FUNC_LOG;

    // Decode URL and convert it to UTF-8 format
    HBufC* decodedUrl = EscapeUtils::EscapeDecodeL( aUrl );
    CleanupStack::PushL( decodedUrl );
    HBufC8* url = EscapeUtils::ConvertFromUnicodeToUtf8L( *decodedUrl );
    CleanupStack::PopAndDestroy( decodedUrl );
    CleanupStack::PushL( url );

    // Create landmark using correct parser
    CPosLandmarkParser& parser = InitializeParserL();
    parser.SetInputBuffer( *url );
    CPosLmOperation* operation = parser.ParseContentL();
    ExecuteAndDeleteLD( operation );
    CPosLandmark* landmark = parser.LandmarkLC();
    CleanupStack::Pop( landmark );
    CleanupStack::PopAndDestroy( url );

    //transfer ownership
    return landmark;
    }

//EOF

