/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR UI launcher object definition
*
*/


#ifndef CESMRICALVIEWERIMPL_H
#define CESMRICALVIEWERIMPL_H

#include <e32base.h>
//<cmail>
#include "cesmricalviewer.h"
//</cmail>

// FORWARD DECLARATIONS:
class CFSMailMessage;
class MESMRIcalViewerCallback;
class MESMRIcalViewerObserver;
class CESMRIcalViewerCmdHandler;

// CLASS DECLARATIONS
/**
 * CESMRIcalViewerImpl implements Ecom interface for launching
 * viewing FS email messages containing meeting requests.
 *
 */
NONSHARABLE_CLASS( CESMRIcalViewerImpl ) :
    public CESMRIcalViewer
    {
public:
    /**
     * Creates and initializes new CESMRIcalViewerImpl object. Ownership
     * is transferred to caller. Callback is delivered via ECOM framework.
     * @param aCallback Pointer to callback interface.
     * @return Pointer to CESMRIcalViewerImpl object
     */
    static CESMRIcalViewerImpl* NewL(
            TAny* aCallback );

    /**
     * C++ destructror
     */
    ~CESMRIcalViewerImpl();

public: // From MESMRIcalViewer
    TBool CanViewMessage(
          CFSMailMessage& aMessage );
    void ExecuteViewL(
          CFSMailMessage& aMessage,
          MESMRIcalViewerObserver& aObserver );
    void ResponseToMeetingRequestL(
          TESMRAttendeeStatus aAttendeeStatus,
          CFSMailMessage& aMessage,
          MESMRIcalViewerObserver& aObserver );
    void RemoveMeetingRequestFromCalendarL(
          CFSMailMessage& aMessage,
          MESMRIcalViewerObserver& aObserver );
    void ResolveMeetingRequestMethodL(
          CFSMailMessage& aMessage,
          MESMRIcalViewerObserver& aObserver );
    TESMRMeetingRequestMethod ResolveMeetingRequestMethodL(
          CFSMailMessage& aMessage );
    void CancelOperation();

private: // Implementation
    CESMRIcalViewerImpl(
            MESMRIcalViewerCallback& aCallback );
    void ConstructL();

private: // Data
    /// Not own: Reference to callback interface
    MESMRIcalViewerCallback& iCallback;
    /// Own: Command handler
    CESMRIcalViewerCmdHandler* iCmdHandler;
    };

#endif // CESMRICALVIEWERIMPL_H

// EOF
