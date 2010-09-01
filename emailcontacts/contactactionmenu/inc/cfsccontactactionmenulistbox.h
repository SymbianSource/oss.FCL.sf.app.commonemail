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
* Description:  Definition of class CFscContactActionMenuListBox.
*
*/


#ifndef C_FSCCONTACTACTIONMENULISTBOX_H
#define C_FSCCONTACTACTIONMENULISTBOX_H

#include <e32base.h>
#include <eiklbo.h>
#include <aknlists.h> 

//<cmail>
#include "fsccontactactionmenudefines.h"
//</cmail>

// DEFINES
#define CFSCCONTACTACTIONMENULISTBOXBASE CEikColumnListBox

// FORWARD DECLARATIONS

/**
 *  Contact Action Menu List Box implementation.
 *
 *  @since S60 3.1
 */
class CFscContactActionMenuListBox 
    : public CFSCCONTACTACTIONMENULISTBOXBASE
    {

public: // Public constructor and destructor

    /**
     * Constructor.
     */
    CFscContactActionMenuListBox();

    /**
    * Destructor.
    */
    virtual ~CFscContactActionMenuListBox();
    
    /**
     * Second phase constructor.
     *
     * @param aParent Parent control
     * @param aFlags Listbox flags
     */
    void ConstructL( const CCoeControl* aParent, TInt aFlags = 0 );
    
public: // From Base class
    
    /**
     * Draw
     *
     * @param aRect rect
     */
    virtual void Draw( const TRect& aRect ) const;  

    /**
     * MopSupplyObject
     * 
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
   
public: // Own Methods

    /**
     * Set background rect of the listbox.
     * @param aRect Rectangle parameter. 
     */
    virtual void SetBackgroundRect( const TRect& aRect );
                                       
private: // data
    
    /**
     * Rectangle of list background
     */ 
    TRect iBackgroundRect; 

    /**
     * Background control
     *   Owned
     */
    CAknsFrameBackgroundControlContext* iBgContext; 
    };

#endif // C_FSCCONTACTACTIONMENULISTBOX_H

