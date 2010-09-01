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
* Description:  UI base class
*
*/

#ifndef CESMRUIBASE_H
#define CESMRUIBASE_H

#include <akndialog.h>
#include "mesmruibase.h"

/**
 * Base class for UI. UI can be constructed and launched by using this class.
 */
NONSHARABLE_CLASS( CESMRUiBase ): public CAknDialog, public MESMRUiBase
    {
public:
    /**
     * Creates new instance of UI base class.
     *
     * @return Pointer to constructed base ui class
     */
    IMPORT_C static CESMRUiBase* NewL();

    // Desctructor
    ~CESMRUiBase();

public: // from CAknDialog
    SEikControlInfo CreateCustomControlL( TInt aType );
    void PostLayoutDynInitL();
    void HandleResourceChange( TInt aType );
    void ProcessCommandL(TInt aCommand);

public: // from MESUIBase
    void ExecuteViewLD();

protected:
    // Default C++ constructor
    CESMRUiBase();

private:
    // Second phase constructor
    void ConstructL();
    };
#endif //CESMRUIBASE_H
