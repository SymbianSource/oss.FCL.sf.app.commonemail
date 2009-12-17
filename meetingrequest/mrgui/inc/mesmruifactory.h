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
* Description:  ESMR ui factory base class
*
*/

#ifndef MESMRUIFACTORY_H
#define MESMRUIFACTORY_H

class CESMRPolicy;
class MESMRCalEntry;
class MESMRUiBase;
class MAgnEntryUiCallback;

/**
 *  MESMRUiFactory is responsible of creating correct UI
 *  implemenation based on given policy (CESMRPolicy).
 *  Created UI can use callback object for executing commands.
 *
 *  @see cesmruibase.h, cesmruifactory.h
 *  @lib esmrgui.lib
 */
class MESMRUiFactory
    {
public: // Destruction
    /**
     *  C++ destructor
     */
    virtual ~MESMRUiFactory() { }

public:
    /**
    * Creates UI view
    * @param aPolicy Reference to MRUI policy
    * @param aEntry Reference to MRUI calendar entry
    * @param aCallback Reference to callback object
    */
    virtual MESMRUiBase* CreateUIL(
            CESMRPolicy* aPolicy,
            MESMRCalEntry& aEntry,
            MAgnEntryUiCallback& aCallback ) = 0;
    };

#endif // M_ESMRUIFACTORY_H

// EOF
