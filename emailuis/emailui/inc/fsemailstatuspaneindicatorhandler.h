/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles mailiindicator updating
*
*/


#ifndef __TFSEMAILSTATUSPANEINDICATORHANDLER_H_
#define __TFSEMAILSTATUSPANEINDICATORHANDLER_H_

#include "FreestyleEmailUiConstants.h"

class TFsEmailStatusPaneIndicatorHandler
    {
public:
    /*
     *  Updates the statuspane mailindicator if needed.
     */ 
    static void StatusPaneMailIndicatorHandlingL( TInt aMailBoxId  );    
    };
#endif /* __TFSEMAILSTATUSPANEINDICATORHANDLER_H */
