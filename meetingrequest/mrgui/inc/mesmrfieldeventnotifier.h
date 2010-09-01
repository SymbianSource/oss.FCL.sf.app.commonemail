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
* Description:  Definition of the class MESMRFieldEventNotifier
*
*/

#ifndef MESMRFIELDEVENTNOTIFIER_H
#define MESMRFIELDEVENTNOTIFIER_H


#include <e32std.h>

class MESMRFieldEventQueue;
class MESMRFieldEventObserver;

/**
 *  MESMRFieldEventObserver defines an interface for observing events
 *  from fields.
 *
 *  @lib esmrfieldbuildercommon.lib
 */
class MESMRFieldEventNotifier
    {

public:

    /**
     * Sets event queue to use by this notifier
     *
     * @param aEvent the event from field
     */
    virtual void SetEventQueueL( MESMRFieldEventQueue* aEventQueue ) = 0;
    
    /**
     * Gets the observer interface of the notifier or NULL.
     */
    virtual MESMRFieldEventObserver* EventObserver() const = 0;

protected:

    virtual ~MESMRFieldEventNotifier() {}

    };


#endif // MESMRFIELDEVENTNOTIFIER_H
