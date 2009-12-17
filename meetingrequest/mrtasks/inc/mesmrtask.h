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


#ifndef MESMRTASK_H
#define MESMRTASK_H

/**
 *  MESMRTask defines interface for ES MR tasks.
 *
 *  @lib esmrtasks.lib
 */
class MESMRTask
    {
public:
    /**
     * Virtual destructor.
     */
    virtual ~MESMRTask() { }

    /**
     * Executes task.
     */
    virtual void ExecuteTaskL() = 0;
    };

#endif  // MESMRTASK_H
