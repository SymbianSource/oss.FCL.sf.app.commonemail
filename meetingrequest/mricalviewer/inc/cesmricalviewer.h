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
* Description:  Definition for ESMRICalViewer
*
*/


#ifndef CESMRICALVIEWER_H
#define CESMRICALVIEWER_H

// INCLUDES
#include <e32base.h>
//<cmail>
#include "mesmricalviewer.h"

// FORWARD DECLARATIONS:
class MESMRIcalViewerCallback;

// CLASS DECLARATIONS
/**
 * CESMRIcalViewer implements Ecom interface for launching
 * viewing FS email messages containing meeting requests.
 *
 */
NONSHARABLE_CLASS( CESMRIcalViewer ) :
        public CBase,
        public MESMRIcalViewer
    {
    public: // Construction and destruction
        /**
         * Creates and initializes new CESMRIcalViewer object. Ownership
         * is transferred to caller.
         * @param aCallback callback interface for esmricalviewer.
         * @return Pointer to CESMRIcalViewer object
         */
        static CESMRIcalViewer* NewL( MESMRIcalViewerCallback& aCallback );

        /**
         * C++ destructror
         */
        ~CESMRIcalViewer();

    private:
        /** iDtor_ID_Key Instance identifier key. When instance of an
         *               implementation is created by ECOM framework, the
         *               framework will assign UID for it. The UID is used in
         *               destructor to notify framework that this instance is
         *               being destroyed and resources can be released.
         */
        TUid iDtor_ID_Key;
    };

#include "cesmricalviewer.inl"
//</cmail>

#endif // CESMRICALVIEWER_H


// EOF
