/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsSetDataSignature.
*
*/


#include "emailtrace.h"
#include <txtrich.h>    // CRichText
#include <txtfmlyr.h>   // CParaFormatLayer, CCharFormatLayer

#include "ipssetdatasignature.h"

// ---------------------------------------------------------------------------
// CIpsSetDataSignature::NewL()
// ---------------------------------------------------------------------------
//
CIpsSetDataSignature* CIpsSetDataSignature::NewL()
    {
    FUNC_LOG;
    CIpsSetDataSignature* self = new ( ELeave ) CIpsSetDataSignature;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ----------------------------------------------------------------------------
// CIpsSetDataSignature::CIpsSetDataSignature()
// ----------------------------------------------------------------------------
CIpsSetDataSignature::CIpsSetDataSignature()
    {
    FUNC_LOG;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataSignature::CIpsSetDataSignature()
// ----------------------------------------------------------------------------
CIpsSetDataSignature::CIpsSetDataSignature(
    CRichText*        aRichText,
    CParaFormatLayer* aParaFormat,
    CCharFormatLayer* aCharFormat )
    :
    iRichText( aRichText ),
    iParaFormatLayer( aParaFormat ),
    iCharFormatLayer( aCharFormat )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsSetDataSignature::ConstructL()
// ---------------------------------------------------------------------------
//
void CIpsSetDataSignature::ConstructL()
    {
    FUNC_LOG;
    
    }

// ----------------------------------------------------------------------------
// CIpsSetDataSignature::~CIpsSetDataSignature()
// ----------------------------------------------------------------------------
CIpsSetDataSignature::~CIpsSetDataSignature()
    {
    FUNC_LOG;
    delete iRichText;
    delete iParaFormatLayer;
    delete iCharFormatLayer;
    }

// ----------------------------------------------------------------------------
// CIpsSetDataSignature::CopyL()
// ----------------------------------------------------------------------------
void CIpsSetDataSignature::CopyL( const CIpsSetDataSignature& aSignature )
    {
    FUNC_LOG;
    delete iRichText;
    iRichText = NULL;

    delete iParaFormatLayer;
    iParaFormatLayer = NULL;

    delete iCharFormatLayer;
    iCharFormatLayer = NULL;

    if ( aSignature.iParaFormatLayer )
        {
        iParaFormatLayer = aSignature.iParaFormatLayer->CloneL();
        }
    if ( aSignature.iCharFormatLayer )
        {
        iCharFormatLayer = aSignature.iCharFormatLayer->CloneL();
        }
    if ( aSignature.iRichText )
        {
        iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
        iRichText->AppendTakingSolePictureOwnershipL( *aSignature.iRichText );
        }
    }
        
// ----------------------------------------------------------------------------
// CIpsSetDataSignature::CreateEmptyRichTextL()
// ----------------------------------------------------------------------------
void CIpsSetDataSignature::CreateEmptyRichTextL()
    {
    FUNC_LOG;
    delete iRichText;
    iRichText = NULL;
    
    delete iParaFormatLayer;
    iParaFormatLayer = NULL;

    delete iCharFormatLayer;
    iCharFormatLayer = NULL;

    iParaFormatLayer = CParaFormatLayer::NewL();
    iCharFormatLayer = CCharFormatLayer::NewL();
    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    }

// End of File


