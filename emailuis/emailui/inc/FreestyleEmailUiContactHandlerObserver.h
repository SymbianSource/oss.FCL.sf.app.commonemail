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
* Description:  Contact handler observer interface
*
*/


#ifndef FREESTYLEEMAILUICONTACTHANDLEROBSERVER_H_
#define FREESTYLEEMAILUICONTACTHANDLEROBSERVER_H_

#include <e32cmn.h>
#include "FreestyleEmailUiContactHandler.h"

class CFSEmailUiClsItem;

/**
 * Contact handler observer interface.
 */
class MFSEmailUiContactHandlerObserver
{
public:

    /**
     * Operation completed.
     * 
     * @param aCmd Completed contact handler operation.
     * @param aContacts array containing the search results
     */
    virtual void OperationCompleteL( TContactHandlerCmd aCmd,
        const RPointerArray<CFSEmailUiClsItem>& aContacts ) = 0;

    /**
     * Error in contact handler operation.
     *
     * @param aCmd Command.
     * @param aError Error.
     */
    virtual void OperationErrorL( TContactHandlerCmd aCmd,
        TInt aError ) = 0;

};

#endif /*FREESTYLEEMAILUICONTACTHANDLEROBSERVER_H_*/
