/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition for mrui dynamic item selection list
*
*/


#ifndef CESMRDYNAMICITEMSELECTIONLIST_H_
#define CESMRDYNAMICITEMSELECTIONLIST_H_

//  INCLUDES
#include <e32base.h>
// <cmail>
#include "esmrdef.h"
// </cmail>
#include <bamdesca.h>

// FORWARD DECLARATIONS:
class CDesC16ArrayFlat;
class CAknPopupList;
class CEikonEnv;

// CLASS DECLARATION

/**
 *  CESMRDynamicItemSelectionList provides dynamic selection list
 *
 */
NONSHARABLE_CLASS( CESMRDynamicItemSelectionList : public CBase )
    {
    public:
        enum TESMRSelectionListType
            {
            EESMRAddressSelectionList = 0,
            EESMRRecentLocationList
            };
        
    public:
        /**
        * Two-phased constructor.
        */
        static CESMRDynamicItemSelectionList* NewL();

        /**
        * Destructor.
        */
        ~CESMRDynamicItemSelectionList();
        
    public: //Implementation
    	/*
    	 * ExecuteL shows the selection list
    	 * and returns the selection
    	 * 
    	 * Formats for selection list:
    	 *     EESMRAddressSelectionList: Label\tAddressData
    	 *     EESMRRecentLocationList: Data only 
    	 * 
    	 * @param aDataArray the data to be shown in the list
    	 * @param aType the type of the list to show the data
    	 * @return index of the list query selection or negative for cancel
    	 */
    	TInt ExecuteL( 
    	        MDesCArray& aDataArray,
    	        const TESMRSelectionListType aType );
    	TInt ExecuteL(
    	        const RPointerArray<HBufC>& aDataArray,
    	        const TESMRSelectionListType aType );

    private: // Constructors
    	CESMRDynamicItemSelectionList();
        void ConstructL();

    private: // Implementation

    private: // Data
        /// Own: Resource offset;
        TInt iResourceOffset;
    	/// Ref:
        CEikonEnv* iEnv;
    };


#endif // CESMRDYNAMICITEMSELECTIONLIST_H_

// End of File
