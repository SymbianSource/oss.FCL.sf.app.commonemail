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
* Description:  Definition for mrui address selection query
*
*/


#ifndef CESMRADDRESSSELECTIONQUERY_H_
#define CESMRADDRESSSELECTIONQUERY_H_

//  INCLUDES
#include <e32base.h>
//<cmail>
#include "esmrdef.h"
//</cmail>

// FORWARD DECLARATIONS:
class CDesC16ArrayFlat;
class CAknPopupList;
class CEikonEnv;

// CLASS DECLARATION

/**
 *  CESMRAddressSelectionQuery provides address selection to user 
 *  when contact has multiple addresses
 *
 */
NONSHARABLE_CLASS( CESMRAddressSelectionQuery : public CBase )
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CESMRAddressSelectionQuery* NewL();

        /**
        * Destructor.
        */
        ~CESMRAddressSelectionQuery();
        
    public: //Implementation
    	/*
    	 * ExecuteAddresXsSelectionListL shows the address selection list
    	 * and returns the selection
    	 * 
    	 * @param aAddressArray the address data from a contact
    	 * @return index of the list query selection or negative for cancel
    	 */
    	TInt ExecuteAddressSelectionListL( const RPointerArray<HBufC>& aAddressArray );

    private: // Constructors
    	CESMRAddressSelectionQuery();
        void ConstructL();

    private: // Implementation
    	TInt ExecuteL( const RPointerArray<HBufC>& aAddressArray );
    	void SetListQueryTextsL( CDesC16ArrayFlat* aItemArray, 
    							 CAknPopupList* aPopupList,
    							 const RPointerArray<HBufC>& aAddressArray );

    private: // Data
        /// Own: Resource offset;
        TInt iResourceOffset;
    	/// Ref:
        CEikonEnv* iEnv;
    };


#endif // CESMRADDRESSSELECTIONQUERY_H_

// End of File
