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
* Description: This file defines class MESMRFieldEventObserver.
*
*/

#ifndef MESMRNAVIARROWEVENTOBSERVER_H_
#define MESMRNAVIARROWEVENTOBSERVER_H_

/**
 *  MESMRNaviArrowEventObserver defines an interface for observing events
 *  from NaviArrow.
 *
 */
class MESMRNaviArrowEventObserver
    {
public:

    /**
     * Event notification function.
     *
     * @param aCommand The navi command
     */
    virtual void HandleNaviArrowEventL( const TInt aCommand ) = 0;

protected:

    virtual ~MESMRNaviArrowEventObserver() {}
    };

#endif /* MESMRNAVIARROWEVENTOBSERVER_H_ */
