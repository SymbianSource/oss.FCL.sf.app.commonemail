/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file defines class MFSNotificationHandlerTimerObserver.
*
*/



#ifndef M_FSNOTIFICATIONHANDLERTIMEROBSERVER_H
#define M_FSNOTIFICATIONHANDLERTIMEROBSERVER_H


/**
 *  Observer interface to observer timer callbacks
 *
 *  ?more_complete_description
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class MFSNotificationHandlerTimerObserver
    {

public:

    /**
     * Callback function which is called after timer
     * completion.
     *
     * @since S60 ?S60_version
     * @param aError Completion code of timer
     */
    virtual void TimerCallBackL( TInt aError ) = 0;

protected:

    };


#endif // M_FSNOTIFICATIONHANDLERTIMEROBSERVER_H
