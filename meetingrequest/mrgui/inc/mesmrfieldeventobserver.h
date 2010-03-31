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



#ifndef MESMRFIELDEVENTOBSERVER_H
#define MESMRFIELDEVENTOBSERVER_H


#include <e32std.h>

class MESMRFieldEvent;

/**
 *  MESMRFieldEventObserver defines an interface for observing events
 *  from fields.
 *
 *  @lib esmrfieldbuildercommon.lib
 */
class MESMRFieldEventObserver
    {

public:

    /**
     * Event notification function.
     *
     * @param aEvent the event from field
     */
    virtual void HandleFieldEventL( const MESMRFieldEvent& aEvent ) = 0;

protected:

    virtual ~MESMRFieldEventObserver() {}

    };


#endif // MESMRFIELDEVENTOBSERVER_H
