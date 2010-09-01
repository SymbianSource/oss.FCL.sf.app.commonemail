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
* Description:  MR Attachment field command to remove attachment
 *
*/

#ifndef CMRREMOVEATTACHMENTCOMMAND_H
#define CMRREMOVEATTACHMENTCOMMAND_H

#include <e32base.h>
#include <AknServerApp.h>
#include "cmrattachmentcommand.h"

// Forward declarations
class CCalEntry;

/**
 * CMRRemoveAttachmentCommand is responsible for removing attachment from entry
 */
NONSHARABLE_CLASS( CMRRemoveAttachmentCommand ) : 
        public CMRAttachmentCommand,
        public MAknServerAppExitObserver
    {
public: // Construction and destruction
    
    static CMRRemoveAttachmentCommand* NewL();
    ~CMRRemoveAttachmentCommand();
    
public: // From CMRAttachmentCommand

    void ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex );
    
private: // Implementation
    CMRRemoveAttachmentCommand();
    void ConstructL();

    };

#endif // CMRREMOVEATTACHMENTCOMMAND_H

// EOF
