/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ES Protocol Framework brand container class
*
*/


#ifndef CFSMAILBRAND_H
#define CFSMAILBRAND_H

// INCLUDES
#include "MailBrandManager.hrh"		// TFSBrandElement
#include <gdi.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CGulIcon;

/**
 *  email framework brand container class
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailBrand ): public CBase
	{

public:
		
    /**
     *  Class to represent branded graphic elements
     *  when stored to an instance of CFSMailBrand.
     */
	class TBrandedGraphic
		{
		public:
		    /**
             * Id to identify type of graphics branded.
             */
			TFSBrandElement iElementId;
			
			TInt iIconId;
			TInt iMaskId;
		};
	
	/**
     *  Class to represent branded text elements
     *  when stored to an instance of CFSMailBrand.
     */
	class TBrandedText
		{
		public:
		    /**
             * Id to identify type of text branded.
             */
			TFSBrandElement iElementId;
			
			/**
			 * Pointer to the text. Actual ownership of the text element
			 * is held elsewhere.
			 */
			TPtrC iText;
		};
	
	/**
     *  Class to represent branded color elements
     *  when stored to an instance of CFSMailBrand.
     */	
	class TBrandedColor
		{
		public:
		    /**
             * Id to identify type of color branded.
             */
			TFSBrandElement iElementId;
			
			TRgb iColor;
		};
		
public:

    /**
     * Two-phased constructor.
     */
     static CFSMailBrand* NewLC( TResourceReader& aReader, TBool aIsWhiteLabel );

    /**
     * Two-phased constructor.
     */
	static CFSMailBrand* NewL( TResourceReader& aReader, TBool aIsWhiteLabel );

    /**
     * destructor.
     */
     virtual ~CFSMailBrand();
	
    /**
     * Function to check whether the given brand id string
     * matches the brand id of this brand or not.
     *
     * @since S60 ?S60_version
     * @param aBrandingIdMatchString Brand id.
     * @return Boolean value. ETrue meaning that the ids match and
     *         EFalse that they don't.
     */  
     TBool IsMatching( const TDesC& aBrandingIdMatchString );
			
    /**
     * Returns branded text element of given type.
     *
     * @param aElementId Id of the branded element. This identifies which
	 *                   text user wishes to retrieve.
     * @return A TPtrC object. Returns TPtrC to a null descriptor if the
	 *         brand doesn't contain the requested branding element.
     */
     TDesC& GetText( TFSBrandElement aElementId );
			
    /**
     * Returns branded color element of given type.
     *
     * @param aElement The id of the branded element. This identifies which
	 *                 color user wishes to retrieve.
     * @param aColor A reference to a TRgb object. The color is returned
	 *               using this reference.
     *
	 * @return Error code. KErrNotFound if the brand doesn't contain the
	 *         requested branding element.
     */
     TInt GetColor( TFSBrandElement aElementId, TRgb& aColor );
			
    /**
     * Returns branded graphic element of given type.
     *
     * @param aBrandingIdMatchString Matching string. Can contain
     *                               wildcard characters.
     * @param aMailBoxId mailbox whose branded element is retrieved.
	 *
	 * @return A pointer to a CGulIcon object. The caller of this method is 
	 *         responsible of destroying the object. Returns NULL if the 
	 *         brand doesn't contain the requested branding element.
     */
     CGulIcon* GetGraphicL( TFSBrandElement aElementId );

    /**
     * Returns branded graphic element of given type.
     *
     * @param aElementId brand element
     * @param aIconIds Icon path and ids
	 *
	 * @return Error code
     */
     TInt GetGraphicIdsL( TFSBrandElement aElementId,
                          TDes& aIconIds );
		
private:
		
    /**
     * constructor
     */
     CFSMailBrand();
			
    /**
	 * Function which constructs instance of this class by reading necessary
	 * brand data from the given resource file.
	 *
	 * @param aReader Resource reader that can be used to read branding data.
	 */
     void ConstructFromResourceL( TResourceReader& aReader );
			
     /**
      *
      */
      void ConstructFromCenrepL( );

private: // data

    /**
     * Brand id matching string read from the resource file. When match
     * for user given brand id is checked, this variable contains the
     * match string. This can contain wild card characters.
     * Own.
     */
    RPointerArray<HBufC>    iBrandMatchStrings;
    
    /**
     * Filepath for icon found from this brand.
     * Own.
     */
    HBufC* iIconFilePath;

    /**
     * Graphic elements found from this brand.
     * Own.
     */
    CArrayFixSeg< TBrandedGraphic >* iGraphicElements;
    /**
     * Text elements found from this brand.
     * Own.
     */
    CArrayFixSeg< TBrandedText >* iTextElements;
    /**
     * Color elements found from this brand.
     * Own.
     */
    CArrayFixSeg< TBrandedColor >* iColorElements;
    /**
     * Actual branded text contents for text elements stored in
     * iTextElements is held here.
     * Own.
     */
    CDesCArraySeg* iTexts;

	HBufC*         iEmpty;
			
	};

#endif CFSMAILBAND_H