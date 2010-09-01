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
* Description:  Interface of triggered comoponent.
*
*/


#ifndef M_FSTRIGGEREDCOMPONENT_H
#define M_FSTRIGGEREDCOMPONENT_H

/**
 *  Interface for triggered component.
 * This interface should be implemented by every component which wants
 * to be opened when button to which it is connected is pressed.
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib fs_generic.lib
 */
NONSHARABLE_CLASS( MFsTriggeredComponent )
    {
public:
    /**
     * Invoked when button to which comonent is attached is pressed.
     */
    virtual void OpenComponent() = 0;
    };

#endif // M_FSTRIGGEREDCOMPONENT_H
