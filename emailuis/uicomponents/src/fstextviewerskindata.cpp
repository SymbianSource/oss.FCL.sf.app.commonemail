/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CFsTextViewerSkinData.
*
*/


#include "emailtrace.h"
#include <AknsUtils.h>

#include "fstextviewerskindata.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
CFsTextViewerSkinData* CFsTextViewerSkinData::NewL()
    {
    FUNC_LOG;
    CFsTextViewerSkinData* self = new ( ELeave ) CFsTextViewerSkinData();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CFsTextViewerSkinData
// ---------------------------------------------------------------------------
CFsTextViewerSkinData::~CFsTextViewerSkinData()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CFsTextViewerSkinData
// ---------------------------------------------------------------------------
CFsTextViewerSkinData::CFsTextViewerSkinData()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CFsTextViewerSkinData::ConstructL()
    {
    FUNC_LOG;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    AknsUtils::GetCachedColor( skin, 
                                iCursorColor, 
                                KAknsIIDQsnHighlightColors, 
                                EAknsCIQsnHighlightColorsCG2 );
                                
    AknsUtils::GetCachedColor( skin, 
                                iMarkedTextColor, 
                                KAknsIIDQsnHighlightColors, 
                                EAknsCIQsnHighlightColorsCG2 );
                                
    AknsUtils::GetCachedColor( skin, 
                                iHotspotSelectedColor, 
                                KAknsIIDQsnHighlightColors, 
                                //EAknsCIFsTextColorsCG9
                                EAknsCIQsnTextColorsCG9 );
                                
    AknsUtils::GetCachedColor( skin, 
                                iHotspotDimmedColor, 
                                KAknsIIDQsnHighlightColors, 
                                //EAknsCIFsHighlightColorsCG3
                                EAknsCIQsnHighlightColorsCG3 );
    }

