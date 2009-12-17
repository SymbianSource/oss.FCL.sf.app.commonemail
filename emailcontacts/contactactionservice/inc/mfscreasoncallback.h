/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration of interface MFscReasonCallback.
*
*/


#ifndef M_FSCREASONCALLBACK_H
#define M_FSCREASONCALLBACK_H

#include <e32base.h>

/**
 *   Utility method interface for contact actions
 *
 *  @since S60 3.1
 */
class MFscReasonCallback
    {
   
public: // Public methods

    /**
     * Visibility reason
     *   Method can be used to query a reason text based on 
     *   action UID and reason id. 
     *   Reason ids are used with visibility information.
     *
     * @param aActionUid Uid of action
     * @param aReasonId Id of reason text
     * @param aReason Reason text. 
     *   Ownership of the descriptor is transfered.
     */
    virtual void GetReasonL(
        TUid aActionUid,
        TInt aReasonId,
        HBufC*& aReason ) const = 0;

    };

#endif // M_FSCREASONCALLBACK_H
