/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer of contact search operations
 *
*/

#ifndef MESMRCONTACTHANDLEROBSERVER_H
#define MESMRCONTACTHANDLEROBSERVER_H

#include "cesmrcontacthandler.h"

class CESMRClsItem;

/**
 * Observer interface for classes who need contact search functionality
 */
class MESMRContactHandlerObserver
{
public:
    /**
     * Operation completed.
     * @param aCmd Completed contact handler operation.
     * @param aContacts array containing the search results,
     * Ownership not transfered
     */
    virtual void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CESMRClsItem>* aContacts ) = 0;

    /**
     * Error in contact handler operation.
     * @param aCmd Command.
     * @param aError Error.
     */
    virtual void OperationErrorL( TContactHandlerCmd aCmd,
        TInt aError ) = 0;
};

#endif
