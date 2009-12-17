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
* Description:  Address factory class
*
*/




#ifndef MNCSFIELDSIZEOBSERVER_H
#define MNCSFIELDSIZEOBSERVER_H


// INCLUDES
#include <aknapp.h>


// CLASS DECLARATION

/**
* MNcsFieldSizeObserver
*/
class MNcsFieldSizeObserver
    {
public:  // new functions

    /**
    * Informs the observer that the field is updated.
    *
    * @since S60 v3.0
    * @return Return ETrue if you wish to redraw the edwin. EFalse otherwise.
    */
    virtual TBool UpdateFieldSizeL() = 0;
    
    /**
    * Informs the observer that the given control position has changed.
    *
    * @param aAnchor Control which position has changed.
    */
    virtual void UpdateFieldPosition( CCoeControl* aAnchor ) = 0;
    
    };

#endif // MNCSFIELDSIZEOBSERVER_H

// End of File
