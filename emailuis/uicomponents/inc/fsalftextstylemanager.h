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


#ifndef C_FSALFTEXTSTYLEMANAGER_H
#define C_FSALFTEXTSTYLEMANAGER_H

#include <e32base.h>
#include <txtfrmat.h>

class CAlfTextStyleManager;
class TFsTextStyleData;


/**
 *  CFsAlfTextStyleManager
 *   
 *  This class is designed to convert CRichText style definitions to
 *  TAlfTextStyles. To avoid overlapping style definitions, all previously
 *  created styles are stored in a list.
 *
 *  Example:
 *  @code
 *
 *  manager = CFsTextStyleManager::NewL( Env().TextStyleManager() );
 *  rText.GetChars( someText, charFormat, position ); //CRichText
 *  TInt styleID = manager->GetStyleIDL( charFormat );
 *  visual = CAlfTextVisual::AddNewL( *iControl, iCurveLayout );
 *  visual->SetTextStyle( styleID );
 *  
 *  @endcode
 *
 *  @since S60 v3.2
 */
class CFsAlfTextStyleManager : CBase
    {
public:

	/**
	 * Two-phased constructor.
	 * @param aAlfTextStyleManager Reference to the CAlfTextStyleManager
	 */
     static CFsAlfTextStyleManager* NewL( CAlfTextStyleManager& aAlfTextStyleManager );

     /**
      * Destructor.
      */
     ~CFsAlfTextStyleManager();

     /**
      * GetStyleIDL
      * 
      * This is the only public method in CFsAlfTextStyleManager. If there
      * already a style that matches the given TCharFormat, the ID of this
      * style is returned. If not, a new style is created.
      *
      * @since S60 3.2
      * @param aCharFormat Style format from CRichText
      * @return ID for TAlfTextStyle
      */
     TInt GetStyleIDL( const TCharFormat& aCharFormat );
     
     /**
      * FindId
      * 
      * The array of previously created styles is searched for matching
      * TCharFormat. If not found KErrNotFound is returned.
      *
      * @since S60 3.2
      * @param aCharFormat Style format from CRichText
      * @return ID for TAlfTextStyle or KErrNotFound
      */
     TInt GetStyleIDWithOpositeUnderlineL( TInt aId );

private:

		CFsAlfTextStyleManager( CAlfTextStyleManager& aAlfTextStyleManager );
		
		// void ConstructL(); This wasn't needed at the time 
		
	     /**
	      * FindId
	      * 
	      * The array of previously created styles is searched for matching
	      * TCharFormat. If not found KErrNotFound is returned.
	      *
	      * @since S60 3.2
	      * @param aCharFormat Style format from CRichText
	      * @return ID for TAlfTextStyle or KErrNotFound
	      */
        TInt FindId( const TCharFormat& aCharFormat );
        
        
        /**
	      * FindPositionOfId
	      * 
	      * The array of previously created styles is searched for matching
	      * ID. If not found KErrNotFound is returned.
	      *
	      * @since S60 3.2
	      * @param ID Style format from CRichText
	      * @return position in table for TAlfTextStyle or KErrNotFound
	      */
        TInt FindPositionOfId(TInt aId);

	     /**
	      * CreateNewTextStyleL
	      * 
	      * Creates a new text style using TCharFormat.
	      *
	      * @since S60 3.2
	      * @param aCharFormat Style format from CRichText
	      * @return ID for TAlfTextStyle
	      */
        TInt CreateNewTextStyleL( const TCharFormat& aCharFormat );

private: // data

    /**
     * Reference to the CAlfTextStyleManager
     */
    CAlfTextStyleManager& iAlfTextStyleManager;

    /**
     * Array of previously created styles
     */
    RArray<TFsTextStyleData> iStyles;

    };

#endif // C_FSALFTEXTSTYLEMANAGER_H