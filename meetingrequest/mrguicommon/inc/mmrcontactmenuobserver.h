/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Contact menu handler observer interface definition.
*
*/

#ifndef MMRCONTACTMENUOBSERVER_H
#define MMRCONTACTMENUOBSERVER_H

/**
 * Observer interface for CESMRContactMenuHandler.
 */
class MMRContactMenuObserver
    {
public:
    
    /**
     * Called by CESMRContactMenuHandler when asynchronous
     * contact action query has completed.
     */
    virtual void ContactActionQueryComplete() = 0;
    
protected:
    
    virtual ~MMRContactMenuObserver() {};
    
    };

#endif MMRCONTACTMENUOBSERVER_H
