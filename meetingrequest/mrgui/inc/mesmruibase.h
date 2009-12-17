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
* Description:  ESMR ui base class
*
*/

#ifndef MESMRUIBASE_H
#define MESMRUIBASE_H

/**
 *  MESMRUiBase defines the interface for different ui views.
 *
 *  @lib esmrgui.lib
 */
class MESMRUiBase
    {
public:
    /**
     * Launches and executes UI view.
     */
    virtual TInt ExecuteViewLD() = 0;
    };

#endif //MESMRUIBASE_H

// EOF

