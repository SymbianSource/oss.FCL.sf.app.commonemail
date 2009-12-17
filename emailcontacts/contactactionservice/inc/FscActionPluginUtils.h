/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utilities for action plugins.
 *
*/

#ifndef FSCACTIONPLUGINUTILS_H_
#define FSCACTIONPLUGINUTILS_H_

#include <e32base.h>

/**  
 * States of operations
 */
enum TActionPluginLastEvent
    {
    EActionEventIdle,
    EActionEventCanExecuteLaunched,
    EActionEventCanExecuteFinished,
    EActionEventContactRetrieve,
    EActionEventGroupRetrieve,
    EActionEventExecuteLaunched,
    EActionEventCanceled
    };

#endif /*FSCACTIONPLUGINUTILS_H_*/
