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
* Description:  Interface definition for esmricalviewer callback
*
*/


#ifndef MESMRICALVIEWERCALLBACK_H
#define MESMRICALVIEWERCALLBACK_H

// INCLUDES
//<cmail>
#include "esmricalvieweropcodes.hrh"   // tesmricalvieweroperationtype
//</cmail>

// FORWARD DECLARATIONS
class CFSMailMessage;
class MESMRIcalViewerObserver;

// CLASS DECLARATIONS
/**
 * MESMRIcalViewerCallback implements callback interface
 * for esmricalviewer.
 */
class MESMRIcalViewerCallback
    {
    public:
        /**
         * Virtual destructor
         */
        virtual ~MESMRIcalViewerCallback() { }

    public: // Interface
        /**
         * Processes given command asynchronously.
         * @param aCommandId Command to be processed.
         * @param aMessage Reference to mail message.
         * @param aObserver Observer for asynchronous operations
         * @exception KErrNotSupported if the command is not supported
         */
        virtual void ProcessAsyncCommandL(
                TESMRIcalViewerOperationType aCommandId,
                const CFSMailMessage& aMessage,
                MESMRIcalViewerObserver* aObserver = NULL ) = 0;

        /**
         * Processes given command synchronously.
         * @param aCommandId Command to be processed.
         * @param aMessage Reference to mail message.
         * @exception KErrNotSupported if the command is not supported
         */
        virtual void ProcessSyncCommandL(
                TESMRIcalViewerOperationType aCommandId,
                const CFSMailMessage& aMessage ) = 0;

        /**
         * Tests if callback can process command.
         * @param aCommandId Command to be processed.
         * @return ETrue if callback can process command.
         */
        virtual TBool CanProcessCommand(
                TESMRIcalViewerOperationType aCommandId ) const = 0;

        /**
        * Returns an extension point for this interface or NULL.
        * @param aExtensionUid Uid of extension.
        * @return Extension point or NULL.
        */
        virtual TAny* MRIcalViewerCallbackExtension(
              TUid /*aExtensionUid*/) { return NULL; }
    };

#endif // MESMRICALVIEWERCALLBACK_H

// EOF
