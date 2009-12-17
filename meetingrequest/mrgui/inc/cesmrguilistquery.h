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
* Description:  esmrgui list queries
*
*/


#ifndef CESMRGUILISTQUERY_H
#define CESMRGUILISTQUERY_H

//<cmail>
#include "esmrdef.h"
//</cmail>
#include <e32base.h>

class CEikonEnv;
/**
 * CESMRGuilistQuery encapsulates list queries
 *
 * @lib esmrgui.lib
 */
NONSHARABLE_CLASS( CESMRGUIListQuery ) : public CBase
    {
public:
    /**
    * Enumeration definition for different kind of
    * confirmation queries
    */
    enum TESMRListQueryType
        {
        EESMRSendUpdateToAllQuery
        };
public:
    /**
     * C++ Destructor.
     */
    ~CESMRGUIListQuery();

     /**
     * Static version of ExecuteLD.
     * @param aType The type of confirmation query.
     * @return TInt selected item or KErrCancel if no selection was made.
     */
     IMPORT_C static TInt ExecuteL( TESMRListQueryType aType );

private: // Implementation
    CESMRGUIListQuery( TESMRListQueryType aType );
    void ConstructL();
    TInt ExecuteLD();

private: // Data
    /// Own: Attendee status
    TESMRListQueryType iType;

    /// Own: Resource offset
    TInt iResourceOffset;

    /// Ref:
    CEikonEnv* iEnv;
    };

#endif // CESMRGUILISTQUERY_H

// EOF

