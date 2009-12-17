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
* Description:  Definition of the class CPbkxRemoteContactLookupEnv.
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPENV_H
#define CPBKXREMOTECONTACTLOOKUPENV_H

#include <e32base.h>
#include <e32std.h>
#include "mpbkxremotecontactlookupserviceui.h"
#include "mpbkxremotecontactlookupsettingsui.h"

// <cmail> S60 UID update
const TInt KPbkxRemoteContactLookupServiceImplImpUid = 0x2001FE0D;
// </cmail> S60 UID update

/**
 *  Remote Contact Lookup UI API.
 *  Defines an ECom interface for the Remote Contact Lookup UI
 *
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupEnv : public CBase
    {

public:

    /**
     * Two-phased constructor.
     *
     * @return instance of CPbkxRemoteContactLookupEnv
     */
    static CPbkxRemoteContactLookupEnv* NewL();

    /**
    * Destructor.
    */
    virtual ~CPbkxRemoteContactLookupEnv();
    
    /**
     * Return instance of Service UI API. The ownership is not transfered
     * to the caller and the same instance will be returned for every
     * call.
     *
     * @return instance of MPbkxRemoteContactLookupServiceUi
     */
     virtual MPbkxRemoteContactLookupServiceUi* ServiceUiL() = 0;
    
    /**
     * Return instance of Settings UI API. The ownership is not transfered
     * to the caller and the same instance will be returned for every
     * call.
     *
     * @return instance of MPbkxRemoteContactLookupSettingsUi 
     */
     virtual MPbkxRemoteContactLookupSettingsUi* SettingsUiL() = 0;
          
private: // data

	/** iDtor_ID_Key Instance identifier key. When instance of an
	 *               implementation is created by ECOM framework, the
	 *               framework will assign UID for it. The UID is used in
	 *               destructor to notify framework that this instance is
	 *               being destroyed and resources can be released.
     */
	TUid iDtor_ID_Key;
    };

#include "cpbkxremotecontactlookupenv.inl"

#endif // CPBKXREMOTECONTACTLOOKUPENV_H
