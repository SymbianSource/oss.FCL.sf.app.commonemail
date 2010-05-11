/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CMRViewers ECom API
*
*/


#ifndef CESMRVIEWERSIMPL_H
#define CESMRVIEWERSIMPL_H		

//  INCLUDES
#include <CMRViewers.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CESMRViewerController;

// CLASS DECLARATION

/**
* CESMRViewer implements MR Viewer API ECom interface.
* @see CMRViewers
*/
NONSHARABLE_CLASS(CESMRViewersImpl) :
        public CMRViewers
    {
    public: // Constructors and destructors

        /**
        * ECom plug-in instantiation method.
        * HBufC8* descriptor is received as a constructor parameter,
        * ownership is transferred to caller (ECom plug-in).
        *
        * @param aMtmUid descriptor used for resolving MR Utils.
        *        Ownership is trasferred from caller.
        * @return instantiated ECom plug-in
        */
        static CESMRViewersImpl* NewL( TAny* aMtmUid );

        /**
        * Destructor.
        */
        ~CESMRViewersImpl();

    protected: // From MAgnEntryUi
        TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                           const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
                           MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
                           MAgnEntryUiCallback& aCallback);
        void SetHelpContext( const TCoeHelpContext& aContext );

    protected: // Constructors and destructors
        CESMRViewersImpl();
        void ConstructL(TAny* aMtmUid);

    protected: // Implementation
        TInt ExecuteViewInternalL( 
        		RPointerArray<CCalEntry>& aEntries,
				const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
				MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
				MAgnEntryUiCallback& aCallback);
        
    protected: // data
        /// Own: Pointer ES MR Controller object
        CESMRViewerController* iController;
        /// Own: MTM UID
        HBufC8* iMtmUid;
    };

#endif // CESMRVIEWERSIMPL_H

// End of File
