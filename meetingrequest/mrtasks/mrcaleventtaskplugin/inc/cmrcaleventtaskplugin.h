/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing the MR Cal Event Task Plugin interface
*
*/

#ifndef CMRCALEVENTTASKPLUGIN_H
#define CMRCALEVENTTASKPLUGIN_H

// INCLUDES
#include <e32base.h>

#include "cesmrtaskfactory.h"

// FORWARD DECLARATIONS


// CLASS DECLARATIONS
/**
 * This class is used for creating implement of view through ECom framework.
 */
NONSHARABLE_CLASS( CMRCalEventTaskPlugin ) : public CESMRTaskFactory
    {
public:
    /**
     * Creates new CMRCalEventTaskPlugin object.
     * @return New CMRCalEventTaskPlugin object
     */
    static CMRCalEventTaskPlugin* NewL( TAny* aCalDbMgr );
    
    /**
    * Destructor
    */
    ~CMRCalEventTaskPlugin();
    
public: // From MESMRTaskFactory
    MESMRTask* CreateTaskL( TESMRCommand aCommand, MESMRCalEntry& aEntry );

private: // Implementation

    CMRCalEventTaskPlugin( MESMRCalDbMgr& aCalDbMgr );
    
    MESMRTask* CreateStoreCalEventToDBTaskL(
            MESMRCalEntry& aEntry );
    MESMRTask* CreateCalEntryUiDeleteTaskL(
            MESMRCalEntry& aEntry );
    MESMRTask* CreateCalEntryUiEditTaskL(
            MESMRCalEntry& aEntry );
    MESMRTask* CreateTodoMarkAsDoneTaskL(
            MESMRCalEntry& aEntry );
    MESMRTask* CreateTodoMarkAsNotDoneTaskL(
            MESMRCalEntry& aEntry );
    MESMRTask* CreateMoveCalEntryToCurrentDBTaskL(
            MESMRCalEntry& aEntry );
    
private: // Data
    /*
     * Reference to cal db manager
     * Not own.
     */
    MESMRCalDbMgr& iCalDbMgr;
    };
    
#endif // CMRCALEVENTTASKPLUGIN_H

// End of file
