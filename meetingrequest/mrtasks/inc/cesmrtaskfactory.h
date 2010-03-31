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
* Description:  Interface definition for ESMR task factory implementation
*
*/


#ifndef CESMRTASKFACTORY_H
#define CESMRTASKFACTORY_H

#include <e32base.h>
#include "mesmrtaskfactory.h"

class MESMRCalDbMgr;

/**
 *  CESMRTaskFactory defines ECom plugin interface for creating ES MR tasks.
 *  @see MESMRTaskFactory
 *
 *  @lib esmrtasks.lib
 */
class CESMRTaskFactory : public CBase,
                         public MESMRTaskFactory
    {
public: // Construction and destruction
    /**
     * Creates and initialzes new ES MR task factory.
     * @param aCalDbMgr Reference to cal db manager.
     */
    inline static CESMRTaskFactory* NewL(
            TESMRCalendarEventType aType,
            MESMRCalDbMgr& aCalDbMgr );

    /**
     * C++ destructor.
     */
    inline ~CESMRTaskFactory();

public: // From MESMRTaskFactory
    MESMRTask* CreateTaskL(
            TESMRCommand aCommand,
            MESMRCalEntry& aEntry ) = 0;

private: // Data
    
    /**
     * iDtor_ID_Key Instance identifier key. When instance of an
     * implementation is created by ECOM framework, the
     * framework will assign UID for it. The UID is used in
     * destructor to notify framework that this instance is
     * being destroyed and resources can be released.
     */
    TUid iDtor_ID_Key;    
    };

#include "cesmrtaskfactory.inl"

#endif // C_ESMRTASKFACTORY_H
