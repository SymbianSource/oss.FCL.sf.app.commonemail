/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRemoteContactLookupServiceUiContext
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPSERVICEUICONTEXT_H
#define CPBKXREMOTECONTACTLOOKUPSERVICEUICONTEXT_H

#include <cntitem.h>

/**
 *  Execution context for Service UI. This class provides functionality to
 *  execute the actual UI flow of this service.
 *
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupServiceUiContext : public CBase
    {

public:

    /** Describes the mode in which the remote lookup is performed. */
    enum TMode 
        {
        /** Normal mode - all the common contact actions are available. 
            Search query editor is displayed.
        */
        EModeNormal,         

        /* Like normal mode, but search query editor is not displayed.
           In this mode, search query text must be given.
        */
        EModeExistingCriteria,
        
        /** Contact selector mode - remote lookup is used to select contact
            for the caller. 
            Search query editor is displayed.
        */
        EModeContactSelector
        };
    
    class TResult 
        {
    public:

        /**
        * Possible exit reasons.
        */
        enum TExitReason
            {
            EExitUndefined,        // Undefined exit reason. This should never be returned.
            EExitUserClosed,        // User closed rcl without selecting anything.
            EExitContactSelected,  // User selected contact and rcl was closed.
            EExitError,            // RCL closed because of error.
            EExitApplication       // User selected exit from menu. Also calling application
                                   // should exit when receiving this.
            };
    
        TResult() : iSelectedContactItem( NULL), iExitReason( EExitUndefined ) {}

        /**
         * Selected contact item.
         */         
        CContactItem* iSelectedContactItem;

        /**
        * Exit reason.
        */
        TExitReason iExitReason;
        };
    
    /**
     * Execute remote lookup UI flow.
     *
     * @param aQueryText loose query text     
     * @param aResult remote lookup results are populated here.
     */
    virtual void ExecuteL( const TDesC& aQueryText, TResult& aResult ) = 0;
    };

#endif // CPBKXREMOTECONTACTLOOKUPSERVICEUICONTEXT_H
