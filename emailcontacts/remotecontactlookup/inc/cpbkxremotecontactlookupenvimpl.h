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
* Description:  Definition of the class CPbkxRemoteContactLookupEnvImpl.
*
*/


#ifndef CPBKXREMOTECONTACTLOOKUPENVIMPL_H
#define CPBKXREMOTECONTACTLOOKUPENVIMPL_H

#include "cpbkxremotecontactlookupenv.h"

class CPbkxRclSearchEngine;
class CPbkxRclSettingsEngine;

/**
 *  Remote Contact Lookup Ui API implementation class.
 *  @see CPbkxRemoteContactLookupEnv
 *
 *  @lib pbkxrclservice.lib
 *  @since S60 3.1
 */
class CPbkxRemoteContactLookupEnvImpl : public CPbkxRemoteContactLookupEnv
    {

public:

    /**
     * Two-phased constructor.
     */
    static CPbkxRemoteContactLookupEnvImpl* NewL();
   
    /**
    * Destructor.
    */
    virtual ~CPbkxRemoteContactLookupEnvImpl();

public:    
    
// from base class CPbkxRemoteContactLookupEnv

     MPbkxRemoteContactLookupServiceUi* ServiceUiL();

     MPbkxRemoteContactLookupSettingsUi* SettingsUiL();

private:

    CPbkxRemoteContactLookupEnvImpl();

    void ConstructL();

private: // data

    CPbkxRclSearchEngine* iServiceUi;
    CPbkxRclSettingsEngine* iSettingsUi;
    };

#endif // CPBKXREMOTECONTACTLOOKUPENVIMPL_H
