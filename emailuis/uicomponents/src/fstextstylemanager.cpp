/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This component converts CRichText style's to HuiTextStyle
*
*/


#include "emailtrace.h"
#include <avkon.hrh>
//<cmail> SF
#include <alf/alftextstylemanager.h>
#include <alf/alftextstyle.h>
//</cmail>
//#include <alffontspecification.h>
//#include <alffontmanager.h>
#include "fstextstylemanager.h"

#define KDummyID 0

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFsTextStyleManager::NewL
// ---------------------------------------------------------------------------
//
CFsTextStyleManager* CFsTextStyleManager::NewL( 
		CAlfTextStyleManager& aAlfTextStyleManager )
	{
    FUNC_LOG;
	CFsTextStyleManager* self = new ( ELeave ) CFsTextStyleManager( 
			aAlfTextStyleManager );
	return self;
	}

// ---------------------------------------------------------------------------
// CFsTextStyleManager::~CFsTextStyleManager
// ---------------------------------------------------------------------------
//
CFsTextStyleManager::~CFsTextStyleManager()
	{
    FUNC_LOG;
	// Clean the style array
	iStyles.Close();
	}

// ---------------------------------------------------------------------------
// CFsTextStyleManager::GetStyleIDL
// ---------------------------------------------------------------------------
//
TInt CFsTextStyleManager::GetStyleIDL( const TCharFormat& aCharFormat )
	{
    FUNC_LOG;
	TInt alfTextStyleID = FindId( aCharFormat );
 
	if( alfTextStyleID == KErrNotFound )
		{
		alfTextStyleID = CreateNewTextStyleL( aCharFormat );
		}
 
	return alfTextStyleID;
	}
	
// ---------------------------------------------------------------------------
// CFsTextStyleManager::GetStyleIDWithOpositeUnderlineL
// ---------------------------------------------------------------------------
//
TInt CFsTextStyleManager::GetStyleIDWithOpositeUnderlineL( TInt aId )
    {
    FUNC_LOG;
    TInt retVal = 0;
    
    TInt arrayId = FindPositionOfId(aId);
    
    TCharFormat opositeCharFormat = iStyles[arrayId].iCharFormat;
    
    if(opositeCharFormat.iFontPresentation.iUnderline == EUnderlineOn)
        {
        opositeCharFormat.iFontPresentation.iUnderline = EUnderlineOff;
        }
    else
        {
        opositeCharFormat.iFontPresentation.iUnderline = EUnderlineOn;    
        }
    
    retVal = GetStyleIDL(opositeCharFormat);
    
    return retVal;
    }
// ---------------------------------------------------------------------------
// CFsTextStyleManager::CFsTextStyleManager
// ---------------------------------------------------------------------------
//
CFsTextStyleManager::CFsTextStyleManager( 
		CAlfTextStyleManager& aAlfTextStyleManager )
	: iAlfTextStyleManager( aAlfTextStyleManager )
	{
    FUNC_LOG;
	// Nothing
	}

// ---------------------------------------------------------------------------
// CFsTextStyleManager::FindId
// ---------------------------------------------------------------------------
//
TInt CFsTextStyleManager::FindId( const TCharFormat& aCharFormat )
	{
    FUNC_LOG;
	// Create a temporary TFsTextStyleData object for search.
	// The ID is not matched.
	TFsTextStyleData hunter( aCharFormat, KDummyID );

	// The comparing of the instances is done in TFsTextStyleData::Match
	TInt id = iStyles.Find( hunter, 
			TIdentityRelation<TFsTextStyleData>( TFsTextStyleData::Match ) );

	// If match was found return the ID.
	if( id != KErrNotFound )
		{
		id = iStyles[id].iTextStyleID;
		}
	return id;
	}

// ---------------------------------------------------------------------------
// CFsTextStyleManager::FindPositionOfId
// ---------------------------------------------------------------------------
//
TInt CFsTextStyleManager::FindPositionOfId(TInt aId)
    {
    FUNC_LOG;
    TInt retVal = -1;
    
    TCharFormat dummyCharFormat;
    TFsTextStyleData hunter( dummyCharFormat, aId );
    
    retVal = iStyles.Find( hunter, 
			TIdentityRelation<TFsTextStyleData>( TFsTextStyleData::MatchId ) );
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// CFsTextStyleManager::CreateNewTextStyleL
// ---------------------------------------------------------------------------
//
TInt CFsTextStyleManager::CreateNewTextStyleL( const TCharFormat& aCharFormat )
	{
    FUNC_LOG;
	// First create the new style for modification
	TInt alfTextStyleID = iAlfTextStyleManager.CreatePlatformTextStyleL( 
			EAknLogicalFontPrimaryFont );

	// Create and store the style info
	TFsTextStyleData newStyleData( aCharFormat,alfTextStyleID );
	iStyles.AppendL(newStyleData);


	
	// Get a handle to the just created style
	CAlfTextStyle* alfTextStyle = iAlfTextStyleManager.TextStyle( alfTextStyleID );

	// Set the font
	// Dummy id is used because the HuiFontManager API doesn't provide means
	// for obtining proper ID's. This may change in the future. However this
	// shouldn't be a problem because in THuiFont implementation only the
	// TFontSpec is used when using the font.
	//
	// NOTE: There is still something weird here. While testing in the emulator
	// the test app occasionally crashed with out of memory error. Then if
	// setting the font was removed everything was ok. And if you then added
	// the back the setting of the font, everything was still ok..
    //TAlfFont alfFont( KDummyID, aCharFormat.iFontSpec );
    //alfTextStyle->SetFont( alfFont );	      
		
    // Set the other style properties
    alfTextStyle->SetTextColor( aCharFormat.iFontPresentation.iTextColor );
    alfTextStyle->SetTextSizeInTwips( aCharFormat.iFontSpec.iHeight );

	if( aCharFormat.iFontPresentation.iStrikethrough == EStrikethroughOn )
	    {
	    alfTextStyle->SetStrikeThrough( ETrue );
	    }
        else
        {
        alfTextStyle->SetStrikeThrough( EFalse );
        }
	
	if( aCharFormat.iFontPresentation.iUnderline == EUnderlineOn )
	    {
	    alfTextStyle->SetUnderline( ETrue );
	    }
        else
        {
        alfTextStyle->SetUnderline( EFalse );
        }

	if( aCharFormat.iFontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold )
	    {
	    //SetStrokeWeight( ETrue );
	    //??
	    alfTextStyle->SetBold( ETrue );
	    }
        else 
            {
            alfTextStyle->SetBold( EFalse );
            }

	if( aCharFormat.iFontSpec.iFontStyle.Posture() == EPostureItalic )
	    {
	    //alfTextStyle->SetPosture( ETrue );
	    //??
	    alfTextStyle->SetItalic(ETrue);
	    }
        else
        {
        alfTextStyle->SetItalic(EFalse);
        }


	if( aCharFormat.iFontPresentation.iHighlightStyle != 
	TFontPresentation::EFontHighlightNone )
		{
		/*alfTextStyle->SetBackgroundColor( 
				aCharFormat.iFontPresentation.iHighlightColor );*/
		}
	
    return alfTextStyleID;
	}

// ---------------------------------------------------------------------------
// TFsTextStyleData::TFsTextStyleData
// ---------------------------------------------------------------------------
//
TFsTextStyleData::TFsTextStyleData( const TCharFormat& aCharFormat, 
		TInt aTextStyleID )
	: iCharFormat( aCharFormat ), iTextStyleID( aTextStyleID )
	{
    FUNC_LOG;
	// Nothing
	}	

// ---------------------------------------------------------------------------
// TFsTextStyleData::Match
// ---------------------------------------------------------------------------
//
TBool TFsTextStyleData::Match( const TFsTextStyleData& aFirst, 
		const TFsTextStyleData& aSecond )
	 {
    FUNC_LOG;
	 // Compare only the iCharFormat.
	 // Optimization here would be comparing only the information that is
	 // needed in CreateNewTextStyleL.
	 return aFirst.iCharFormat.IsEqual(aSecond.iCharFormat);
	 }

// ---------------------------------------------------------------------------
// TFsTextStyleData::Match
// ---------------------------------------------------------------------------
//	 
TBool TFsTextStyleData::MatchId( const TFsTextStyleData& aFirst, 
			const TFsTextStyleData& aSecond )
    {
    FUNC_LOG;
    return aFirst.iTextStyleID == aSecond.iTextStyleID;
    }

