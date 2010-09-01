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
* Description:  Declaration of class TFscContactActionVisibility.
*
*/


#ifndef T_FSCCONTACTACTIONVISIBILITY_H
#define T_FSCCONTACTACTIONVISIBILITY_H

#include <e32base.h>

// FORWARD DECLARATIONS

/**
 *  Contact Action Visibility.
 *
 *  @since S60 3.1
 */
class TFscContactActionVisibility
    {

public: // Enums
    
    /*
     * Visibility state enumeration
     */
    enum TVisibility
        {
        EFscActionVisible,
        EFscActionDimmed,
        EFscActionHidden
        };
    
public: // Public constructors
  
    /**
     * Default constructor
     */
    TFscContactActionVisibility() {}

    /**
     * Constructor
     *
     * @param aVisibility Visibility of action
     * @param aReasonId Reason id for the visibility. 
     *   Reason ids are defined and returned by action plug-ins. 
     *   Reason id can be used to ask a reason for given visibility 
     *   using MFscContactAction::ReasonL method. If action does not 
     *   have specific reason for visibility, 0 is used.
     */
    TFscContactActionVisibility( 
        TVisibility aVisibility,
        TInt aReasonId ) 
    : iVisibility( aVisibility ), 
      iReasonId( aReasonId ) {}

public: // Public variables

    TVisibility iVisibility; // Item visibility
    TInt iReasonId; // Reason id for the visibility
    
    };

#endif // T_FSCCONTACTACTIONVISIBILITY_H
