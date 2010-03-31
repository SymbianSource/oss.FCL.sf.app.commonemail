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
* Description: MBUtils ECOM definition
*
*/
#ifndef C_MRBCMEETINGREQUESTPLUGIN_H
#define C_MRBCMEETINGREQUESTPLUGIN_H

#include "cmrbceventplugin.h"
#include <e32base.h>
#include <calenservices.h>

class CMRBCPluginCmdHandler;

/**
 * CMRBCMeetingRequestPlugin implements CMRBCEventPlugin interface
 * for meeting requests
 */
NONSHARABLE_CLASS( CMRBCMeetingRequestPlugin ) :
    public CMRBCEventPlugin
    {
public:
    /**
     * Creates new CMRBCMeetingRequestPlugin object.
     * @param aCalenServices Reference to calendar services
     * @return New CMRBCMeetingRequestPlugin object
     */
    static CMRBCMeetingRequestPlugin* NewL(
            TAny* aCalenServices );
    
    /**
     * C++ destructor
     */
    ~CMRBCMeetingRequestPlugin();
    
public: // From base class MMRBCEventPlugin
    MCalenCommandHandler* CommandHandler();

private:
    CMRBCMeetingRequestPlugin(
            MCalenServices& aCalenServices );
    void ConstructL();
    
private:
    /// Ref: Reference to calendar services 
    MCalenServices& iCalenServices;
    /// Own: MR Command handler
    CMRBCPluginCmdHandler* iCommandHandler;
    };

#endif // C_MRBCMEETINGREQUESTPLUGIN_H

// EOF
