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
* Description:  Definition of class MFscContactActionMenuListObserver.
*
*/


#ifndef M_FSCCONTACTACTIONMENULISTOBSERVER_H
#define M_FSCCONTACTACTIONMENULISTOBSERVER_H

#include <e32base.h>

// FORWARD DECLARATIONS

/**
 *  Contact Action Menu List observer interface
 *
 *  @since S60 3.1
 */
class MFscContactActionMenuListObserver
    {

public: // Enums

    /**
     * List events
     */
    enum TFscContactActionMenuListEvent
        {
        EFscMenuEventItemClicked, // Event is generaten when item gets clicked
        EFscMenuEventCloseKey, // Menu close triggering event
        EFscMenuEventExitKey, // Exit key detected event
        EFscMenuEventMenuHidden, // Menu fully hidden event
        EFscMenuLayoutChanged,
        EFscMenuEventError // Error event
        };
    
public: // Public methods

    /*
     * Handle action menu list event
     *
     * @param aEvent Event code
     * @param aError Error code used with error event
     */
    virtual void HandleListEvent( TFscContactActionMenuListEvent aEvent,
                                  TInt aError = KErrNone ) = 0;
    
    /**
     * Check if selected opeartion is completed. 
     */
    virtual TBool IsOperationCompleted() = 0;

    };

#endif // M_FSCCONTACTACTIONMENULISTOBSERVER_H

