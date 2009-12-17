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
* Description:  ESMR ui factory class definition
*
*/

#ifndef CESMRUIFACTORY_H
#define CESMRUIFACTORY_H

#include <e32base.h>
//<cmail>
#include "mesmruifactory.h"
//</cmail>

// FORWARD DECLARATIONS:
class CEikonEnv;

/**
 * UI Factory implementation.
 *
 * @see mesmruifactory.h
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS ( CESMRUiFactory ) :
        public CBase,
        public MESMRUiFactory
    {
public: // Construction and destruction
    /**
     * Two-phased constructor. Creates and initialzes new CESMRUiFactory
     * object. Ownership is transferred to caller.
     *
     * @return Pointer to CESMRUiFactory object.
     */
     IMPORT_C static CESMRUiFactory * NewL();

    /**
     * C++ destructor
     */
    IMPORT_C ~CESMRUiFactory ();

public: // From MESMRUiFactory
    IMPORT_C MESMRUiBase* CreateUIL(
            CESMRPolicy* aPolicy,
            MESMRCalEntry& aEntry,
            MAgnEntryUiCallback& aCallback );

private: // Implementation
    CESMRUiFactory();
    void ConstructL();
    void LoadResourceFileL(CEikonEnv* aEikEnv,
                           const TDesC& aFilename,
                           TInt& aResourceOffset);
private:
    /// Own: Resource offset, stored for later deletion
    TInt iResourceOffset;
    };

#endif // CESMRUIFACTORY_H

// EOF

