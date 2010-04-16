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
* Description:  ES Protocol Framework branding manager interface definition
*
*/


#ifndef CFSMAILBRANDMANAGER_H
#define CFSMAILBRANDMANAGER_H

// INCLUDES
#include <e32base.h>
#include <barsc.h>

#include "MFSMailBrandManager.h"

// FORWARD DECLARATIONS
class TResourceReader;
class CFSMailClient;
class CFSMailBox;
class CFSMailBrand;

/**
 *  Branding manager implementation header.
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS ( CFSMailBrandManagerImpl ) : public CBase, public MFSMailBrandManager
	{
		public:
			
    /**
     * Two-phased constructor.
     *
     */
     static CFSMailBrandManagerImpl* NewL( CFSMailClient& aMailClient );
     
    /**
     * Two-phased constructor.
     *
     */
     static CFSMailBrandManagerImpl* NewLC( CFSMailClient& aMailClient );
			
    /**
     * Destructor.
     */
     virtual ~CFSMailBrandManagerImpl();
			
private:	// From MFSMailBrandManager

	/**
	 * @see MFSMailBrandManager::GetGraphicL
	 */
	CGulIcon* GetGraphicL( TFSBrandElement aElement,
	                       const TFSMailMsgId& aMailboxId );

	/**
	 * @see MFSMailBrandManager::GetTextL
	 */
	TPtrC GetTextL( TFSBrandElement aElement,
	                const TFSMailMsgId& aMailboxId );

	/**
	 * @see MFSMailBrandManager::GetTextL
	 */
	TPtrC GetTextL( TFSBrandElement aElement,
			        const TDesC& aBrandId );


	/**
	 * @see MFSMailBrandManager::GetColorL
	 */
	TInt GetColorL( TFSBrandElement aElement,
	                const TFSMailMsgId& aMailboxId,
	                TRgb& aColor );

	/**
	 * @see MFSMailBrandManager::UpdateMailboxNamesL
	 */
	void UpdateMailboxNamesL(  const TFSMailMsgId aMailboxId );

	/**
	 * @see MFSMailBrandManager::GetGraphicIdsL
	 */
	TInt GetGraphicIdsL( TFSBrandElement aElement,
	                     const TFSMailMsgId& aMailboxId,
                         TDes& aIconIds  );

private:

    /**
     * costructor
     */
	 CFSMailBrandManagerImpl( CFSMailClient& aMailClient );
	
    /**
     * Two-phased constructor.
     */
	void ConstructL();

    /**
	 * Function which constructs brands used by this class from given
	 * resource file.
	 *
	 * @param aReader Resource reader that can be used to read branding data.
	 */
	void ConstructFromResourceL( TResourceReader& aReader );
	
	/**
	 * Function which maps brand id received from a mailbox to a brand
	 * object.
	 *
	 * @param aBrandId Id to identify the brand object.
	 */
	CFSMailBrand* FindMatchingBrandL( const TDesC& aBrandId );
	
	/**
	 * Function which goes through the mail boxes and finds the one whose
	 * brand matches the given brand id.
	 *
	 * @param aBrandId Id identifying the brand.
	 */
	CFSMailBox* MailboxMatchingBrandIdL( const TDesC& aBrandId ) const;
	
private: // data

    /**
     * Mail client reference.
     */
    CFSMailClient& iMailClient;

    /**
     * Array containing all brands.
     * Own.
     */
	CArrayPtrSeg< CFSMailBrand >* iBrands;
	
	/**
     * File server session used for example for reading brand resource file.
     */
	RFs iFsSession;
	
	/**
     * RResourceFile used to read a resource file into buffer. Used
     * for example to read the resource file containing the branding data.
     */
	RResourceFile iResourceFile;
		
	};
	
	
#endif CFSMAILBRANDMANAGER_H
