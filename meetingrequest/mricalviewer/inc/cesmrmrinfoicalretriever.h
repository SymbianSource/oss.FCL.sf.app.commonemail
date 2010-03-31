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
* Description:  ESMR mrinfo calendar entry retriever definition
*
*/


#ifndef CESMRMRINFOICALRETRIEVERCMD_H
#define CESMRMRINFOICALRETRIEVERCMD_H

#include <e32base.h>
//<cmail>
#include "mmrinfoobject.h"
//</cmail>
#include "cesmricalviewercommandbase.h"

class TESMRInputParams;
class CCalEntry;
class CESMRAttachmentInfo;

/**
 * CESMRInfoIcalRetrieverCmd is the command used to execute
 * calendar entry retrievals from calendar db based on 
 * MMRInfoObject objects
 */
NONSHARABLE_CLASS( CESMRInfoIcalRetrieverCmd ) :
        public CESMRIcalViewerCommandBase
    {
public: // Construction and destruction
    /**
     * Creates and initializes new CESMRInfoIcalRetriever object. Ownership
     * is transferred to caller.
     * @param aCalSession Reference to calendar database session
     * @aInputParameters set of parameters for the command class
     * @return Pointer to CESMRInfoIcalRetriever object.
     */
    static CESMRInfoIcalRetrieverCmd* NewL(
            CCalSession& aCalSession,
            TESMRInputParams& aInputParameters );

    /**
     * C++ destructor.
     */
    ~CESMRInfoIcalRetrieverCmd();

public: // From MESMRIcalRetriever
    void ExecuteCommandL(
            CFSMailMessage& aMessage,
            MESMRIcalViewerObserver& aObserver );
    void CancelCommand();

private: // Implementation
    TBool RetrieveCalendarEntryL();
    CESMRInfoIcalRetrieverCmd(
            CCalSession& aCalSession,
            TESMRInputParams& aInputParameters );
    void ConstructL();
    void CreateEntryL();
    void FillCommonFieldsL();
    void FillRecurrenceL();
    
private: // Data
    // Own: Supported fields array
    RArray<MMRInfoObject::TESMRInfoField> iSupportedFields;
    /// Own: Calendar entry object
    CCalEntry* iConvertedEntry;
    /// Ref: Reference to MRINFO object
    MMRInfoObject* iMRInfoObject;
    /// Ref: Reference to input parameters
    TESMRInputParams& iInputParameters;
    /// Own: Result structure
    MESMRIcalViewerObserver::TIcalViewerOperationResult iResult;
    };

#endif 

