/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ICal viewer's observer interface.
*
*/


#ifndef MESMRICALVIEWEROBSERVER_H
#define MESMRICALVIEWEROBSERVER_H

// INCLUDES
#include <e32cmn.h>
//<cmail>
#include "esmricalviewerdefs.hrh"      // tesmrmeetingrequestmethod
#include "esmricalvieweropcodes.hrh"   // tesmricalvieweroperationtype
//</cmail>

// FORWARD DECLARATIONS
class CFSMailMessage;

// CLASS DECLARATIONS
/**
 * ICal viewer's observer interface.
 * This is used to signal client about asynchronous operation's completion.
 */
class MESMRIcalViewerObserver
    {
    public: // Types
        /**
         * Result for iCal viewer operation
         */
        struct TIcalViewerOperationResult
            {
            /// Own: Type of the operation
            TESMRIcalViewerOperationType iOpType;
            /// Own: Operation result e.g. KErrCancel if user has canceled operation.
            ///         System wide error code.
            TInt iResultCode;
            /// Own: Relevant only if the iOpType is EResolveMRMethod
            TESMRMeetingRequestMethod iMethod;
            /// Ref: Mail message related to this operation
            CFSMailMessage* iMessage;
            /// Own: Relevant only when user has responsed to MR
            ///      Otherwise this is always set to EESMRAttendeeStatusUnknown
            TESMRAttendeeStatus iAttendeeStatus;
            };

    public: // New functions
        /**
         * Called when the operation is completed succesfully.
         *
         * @param aResult A result of the operation.
         */
        virtual void OperationCompleted(
                        TIcalViewerOperationResult aResult ) = 0;

        /**
         * Called when error occurs during the operation.
         *
         * @param aResult error result of the operation.
         */
        virtual void OperationError(
                        TIcalViewerOperationResult aResult ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* MRIcalViewerObserverExtension(
                TUid /*aExtensionUid*/ ) { return NULL; }

    protected: // construction and destruction
        /**
         * C++ Destructor
         */
        virtual ~MESMRIcalViewerObserver() { }
    };

#endif // MESMRICALVIEWEROBSERVER_H

// End of file
