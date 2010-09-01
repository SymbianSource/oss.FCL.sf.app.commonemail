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
//#include <alffontspecification.h>
//#include <alffontmanager.h>
#include <alf/alftextstyle.h>
//</cmail>
#include "fsalftextstylemanager.h"
#include "fstextstylemanager.h"

#define KDummyID 0

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFsTextStyleManager::NewL
// ---------------------------------------------------------------------------
//
CFsAlfTextStyleManager* CFsAlfTextStyleManager::NewL( 
		CAlfTextStyleManager& aAlfTextStyleManager )
	{
    FUNC_LOG;
	CFsAlfTextStyleManager* self = new ( ELeave ) CFsAlfTextStyleManager( 
			aAlfTextStyleManager );
	return self;
	}

// ---------------------------------------------------------------------------
// CFsAlfTextStyleManager::~CFsAlfTextStyleManager
// ---------------------------------------------------------------------------
//
CFsAlfTextStyleManager::~CFsAlfTextStyleManager()
	{
    FUNC_LOG;
	// Clean the style array
	iStyles.Close();
	}

// ---------------------------------------------------------------------------
// CFsAlfTextStyleManager::GetStyleIDL
// ---------------------------------------------------------------------------
//
TInt CFsAlfTextStyleManager::GetStyleIDL( const TCharFormat& aCharFormat )
	{
    FUNC_LOG;
	TInt AlfTextStyleID = FindId( aCharFormat );
 
	if( AlfTextStyleID == KErrNotFound )
		{
	    AlfTextStyleID = CreateNewTextStyleL( aCharFormat );
		}
 
	return AlfTextStyleID;
	}
	
// ---------------------------------------------------------------------------
// CFsAlfTextStyleManager::GetStyleIDWithOpositeUnderlineL
// ---------------------------------------------------------------------------
//
TInt CFsAlfTextStyleManager::GetStyleIDWithOpositeUnderlineL( TInt aId )
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
// CFsAlfTextStyleManager::CFsAlfTextStyleManager
// ---------------------------------------------------------------------------
//
CFsAlfTextStyleManager::CFsAlfTextStyleManager( 
		CAlfTextStyleManager& aAlfTextStyleManager )
	: iAlfTextStyleManager( aAlfTextStyleManager )
	{
    FUNC_LOG;
	// Nothing
	}

// ---------------------------------------------------------------------------
// CFsAlfTextStyleManager::FindId
// ---------------------------------------------------------------------------
//
TInt CFsAlfTextStyleManager::FindId( const TCharFormat& aCharFormat )
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
// CFsAlfTextStyleManager::FindPositionOfId
// ---------------------------------------------------------------------------
//
TInt CFsAlfTextStyleManager::FindPositionOfId(TInt aId)
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
// CFsAlfTextStyleManager::CreateNewTextStyleL
// ---------------------------------------------------------------------------
//
TInt CFsAlfTextStyleManager::CreateNewTextStyleL( const TCharFormat& aCharFormat )
	{
    FUNC_LOG;
	// First create the new style for modification
	TInt AlfTextStyleID = iAlfTextStyleManager.CreatePlatformTextStyleL( 
			EAknLogicalFontPrimaryFont );

	// Create and store the style info
	TFsTextStyleData newStyleData( aCharFormat,AlfTextStyleID );
	iStyles.AppendL(newStyleData);

	// Get a handle to the just created style
	CAlfTextStyle* AlfTextStyle = iAlfTextStyleManager.TextStyle( AlfTextStyleID );

	// Set the font
	// Dummy id is used because the AlfFontManager API doesn't provide means
	// for obtining proper ID's. This may change in the future. However this
	// shouldn't be a problem because in TAlfFont implementation only the
	// TFontSpec is used when using the font.
	//
	// NOTE: There is still something weird here. While testing in the emulator
	// the test app occasionally crashed with out of memory error. Then if
	// setting the font was removed everything was ok. And if you then added
	// the back the setting of the font, everything was still ok..
    
    //TAlfFont AlfFont( KDummyID, aCharFormat.iFontSpec );
    //AlfTextStyle->SetFont( AlfFont );	      

    // Set the other style properties
    AlfTextStyle->SetTextColor( aCharFormat.iFontPresentation.iTextColor );
	AlfTextStyle->SetTextSizeInTwips( aCharFormat.iFontSpec.iHeight );

	if( aCharFormat.iFontPresentation.iStrikethrough == EStrikethroughOn )
	    {
	    AlfTextStyle->SetStrikeThrough( ETrue );
	    }
        else
        {
        AlfTextStyle->SetStrikeThrough( EFalse );
        }
	
	if( aCharFormat.iFontPresentation.iUnderline == EUnderlineOn )
	    {
	    AlfTextStyle->SetUnderline( ETrue );
	    }
        else
        {
        AlfTextStyle->SetUnderline( EFalse );
        }

	if( aCharFormat.iFontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold )
	    {
            AlfTextStyle->SetBold( ETrue );
	    }
        else 
            {
            AlfTextStyle->SetBold( EFalse );
            }


	if( aCharFormat.iFontSpec.iFontStyle.Posture() == EPostureItalic )
		{
        //???
        AlfTextStyle->SetItalic(ETrue);
		//AlfTextStyle->SetPosture( ETrue );
		}

	if( aCharFormat.iFontPresentation.iHighlightStyle != 
	TFontPresentation::EFontHighlightNone )
		{
		/*AlfTextStyle->SetBackgroundColor( 
				aCharFormat.iFontPresentation.iHighlightColor );*/
		}

    return AlfTextStyleID;
	}
/*
// ---------------------------------------------------------------------------
// TFsTextStyleData::TFsTextStyleData
// ---------------------------------------------------------------------------
//
TFsTextStyleData::TFsTextStyleData( const TCharFormat& aCharFormat, 
		TInt aTextStyleID )
	: iCharFormat( aCharFormat ), iTextStyleID( aTextStyleID )
	{
	// Nothing
	}	

// ---------------------------------------------------------------------------
// TFsTextStyleData::Match
// ---------------------------------------------------------------------------
//
TBool TFsTextStyleData::Match( const TFsTextStyleData& aFirst, 
		const TFsTextStyleData& aSecond )
	 {
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
    return aFirst.iTextStyleID == aSecond.iTextStyleID;
    }

*/

