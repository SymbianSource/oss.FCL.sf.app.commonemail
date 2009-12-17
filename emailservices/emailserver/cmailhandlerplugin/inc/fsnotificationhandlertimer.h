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
* Description: This file defines class CFSNotificationHandlerTimer.
*
*/


#ifndef __FSNOTIFICATIONHANDLERTIMER_H__
#define __FSNOTIFICATIONHANDLERTIMER_H__

#include <e32base.h>

#include "fsnotificationhandlertimerobserver.h"


/**
 *  Timer service which uses callbacks to inform of
 *  timer completion.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CFSNotificationHandlerTimer : public CTimer
    {
public:

    static CFSNotificationHandlerTimer* NewL(
        MFSNotificationHandlerTimerObserver& aObserver );
    static CFSNotificationHandlerTimer* NewLC(
        MFSNotificationHandlerTimerObserver& aObserver );

    /**
     * Destructor.
     */
    virtual ~CFSNotificationHandlerTimer();


protected:
  
    /**
     * Constructor.
     *
     * @since S60 ?S60_version
     * @param aObserver Observer of this timer.
     */ 
    CFSNotificationHandlerTimer(
        MFSNotificationHandlerTimerObserver& aObserver );
    void ConstructL();

// from base class CActive
    virtual void DoCancel();
    virtual void RunL();

private:


        	
private: // data

    /**
     * Observer and user of this timer.
     */    
    MFSNotificationHandlerTimerObserver& iObserver;

    };


#endif  //__FSNOTIFICATIONHANDLERTIMER_H__
