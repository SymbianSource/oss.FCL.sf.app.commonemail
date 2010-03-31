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
* Description: This file defines class MMRFocusHandlingTimerObserver.
*
*/

#ifndef MMRFOCUSHANDLINGTIMEROBSERVER_H
#define MMRFOCUSHANDLINGTIMEROBSERVER_H


/**
 *  Observer interface to observer timer callbacks
 */
class MMRFocusHandlingTimerObserver
    {
    public:
        /**
         * Callback function which is called after timer
         * completion.
         *
         * @param aError Completion code of timer
         */
        virtual void TimerCallBackL( TInt aError ) = 0;
        
    };


#endif // MMRFOCUSHANDLINGTIMEROBSERVER_H
