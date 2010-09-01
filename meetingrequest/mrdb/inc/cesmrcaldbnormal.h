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
* Description:  Interface for normal agenda database operations
*
*/

#ifndef CESMRCALDBNORMAL_H
#define CESMRCALDBNORMAL_H

//  INCLUDE FILES
#include <e32base.h>
#include "cesmrcaldbbase.h"

// FORWARD DECLARATIONS
class CCalSession;

// CLASS DECLARATION

/**
 * This class wraps the normal agenda database.
 */
NONSHARABLE_CLASS(CESMRCalDbNormal) : public CESMRCalDbBase
    {
public: // Constructors and destructors
    /**
     * Symbian two-phased constructor.
     * @param aCalSession calendar session reference
     * @param aDbObserver database event/status observer
     * @param aCmdObserver asynchronous cmd progress observer
     */
    static CESMRCalDbNormal* NewL(
            CCalSession& aCalSession,
            MESMRCalDbObserver& aDbObserver,
            MCalProgressCallBack& aCmdObserver );

    /**
     * Destructor.
     */
    ~CESMRCalDbNormal();

public: // From MCalProgressCallBack
    void Completed(
            TInt aError );

private: // Constructors and destructors
    CESMRCalDbNormal(
            CCalSession& aCalSession,
            MESMRCalDbObserver& aDbObserver,
            MCalProgressCallBack& aCmdObserver );
    void ConstructL();
    };

#endif // CESMRCALDBNORMAL_H

// End of File
