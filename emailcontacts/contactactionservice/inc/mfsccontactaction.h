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
* Description:  Declaration of interface MFscContactAction.
*
*/


#ifndef M_FSCCONTACTACTION_H
#define M_FSCCONTACTACTION_H

#include <e32base.h>

//<cmail>
#include "fsccontactactionservicedefines.h"
//</cmail>

// FORWARD DECLARATIONS
class CGulIcon;

/**
 *  Contact action.
 *  Defines an interface for contact actions.
 *
 *  @since S60 3.1
 */
class MFscContactAction
    {
   
public: // Public methods

    /**
     * Action uid
     *
     * @return Uid of the action
     */
    virtual TUid Uid() const = 0;

    /**
     * Action type
     *
     * @return type of the action
     */
    virtual TUint64 Type() const = 0;

    /**
     * Icon of the action
     *  If the action does not have an Icon NULL is returned. 
     *  Ownership of the icon is not transferred.
     *
     * @return icon of the action
     */
    virtual const CGulIcon* Icon() const = 0;

    /**
     * Action Menu text of the action
     *
     * @return action menu text
     */
    virtual const TDesC& ActionMenuText() const = 0;

    /**
     * Visibility reason
     *   Method can be used to query a reason text based on 
     *   reason id. Reason ids are used with visibility information.
     *
     * @param aReasonId Id of reason text
     * @param aReason Reason text. 
     *   Ownership of the descriptor is transfered.
     */
    virtual void ReasonL( TInt aReasonId, HBufC*& aReason ) const = 0;

    };

#endif // M_FSCCONTACTACTION_H
