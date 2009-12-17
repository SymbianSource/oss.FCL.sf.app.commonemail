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
* Description:  Interface definition for ESMR task factory
*
*/


#ifndef MESMRTASKFACTORY_H
#define MESMRTASKFACTORY_H

//<cmail>
#include "esmrdef.h"
//</cmail>

class MESMRTask;
class MESMRMeetingRequestEntry;

/**
 *  MESMRTaskFactory defines interface for creating ES MR tasks.
 *
 *  @code
 *    MESMRTaskFactory* factory = ...
 *  @endcode
 *
 *  @lib esmrtasks.lib
 */
class MESMRTaskFactory
    {
public:
    /**
     * Virtual C++ destructor.
     */
    virtual ~MESMRTaskFactory() { }

    /**
     * Creates ES MR task. Ownership is transferred to
     * caller.
     * @param aCommand Command id
     * @param aEntry Reference to entry being handled.
     * @return Pointer to created task object.
     */
    virtual MESMRTask* CreateTaskL(
            TESMRCommand aCommand,
            MESMRMeetingRequestEntry& aEntry ) = 0;
    };

#endif // MESMRTASKFACTORY_H
