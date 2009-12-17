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
* Description:  Definition of the class CPbkxRclCbRSender.
*
*/


#ifndef CPBKXRCLCBRSENDER_H
#define CPBKXRCLCBRSENDER_H

#include <e32base.h>

class CPbkContactItem;
class CAiwServiceHandler;

/**
* Class that sends call back request using CAiwServiceHandler.
*
* @lib pbkxrclengine.lib
* @since 3.1
*/
class CPbkxRclCbRSender : public CBase
    {
public: // constructor and destructor

    /**
    * Two-phased constructor.
    *
    * @return Created object.
    */
    static CPbkxRclCbRSender* NewL();

    /**
    * Two-phased constructor.
    *
    * Leaves pointer to the created object in cleanupstack.
    *
    * @return Created object.
    */
    static CPbkxRclCbRSender* NewLC();

    /**
    * Destructor.
    */
    virtual ~CPbkxRclCbRSender();

public: // new methods

    /**
    * Sends callback request to the given contact.
    *
    * @param aContactItem Contact item for which callback request is sent.
    */
    void SendCallbackRequestL( CPbkContactItem& aContactItem );

private: // constructors

    /**
    * Constructor.
    */
    CPbkxRclCbRSender();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private: // data

    // Service handler for sending the callback request. Owned.
    CAiwServiceHandler* iServiceHandler;
    
    };

#endif

