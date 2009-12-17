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
* Description: This file defines class MFSNotificationHandlerNotifierInitiatorObserver.
*
*/



#ifndef M_FSNOTIFICATIONHANDLERNOTIFIERINITIATOROBSERVER_H
#define M_FSNOTIFICATIONHANDLERNOTIFIERINITIATOROBSERVER_H


/**
 *  Observer interface to observe notification completion callbacks
 *
 *  ?more_complete_description
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class MFSNotificationHandlerNotifierInitiatorObserver
    {

public:

    /**
     * Informs instance of this observer/owner class that notification
     * showing has completed and that the initiator
     * can be removed.
     *
     * @param aInitiator Pointer to the initiator that has completed.
     */
    virtual void NotifierInitiatorDoneL(
        CFSNotificationHandlerNotifierInitiator* aInitiator ) = 0;

protected:

    };


#endif // M_FSNOTIFICATIONHANDLERNOTIFIERINITIATOROBSERVER_H
