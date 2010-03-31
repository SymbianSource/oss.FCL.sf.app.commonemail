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
* Description:  MR Attachment field for editor and viewer
 *
*/

#ifndef CMROPENATTACHMENTCOMMAND_H
#define CMROPENATTACHMENTCOMMAND_H

#include <e32base.h>
#include <aknserverapp.h>
#include "cmrattachmentcommand.h"

// Forward declarations
class CCalEntry;
class CDocumentHandler;

/**
 * CMROpenAttachmentCommand is responsible for opening attachment.
 */
NONSHARABLE_CLASS( CMROpenAttachmentCommand ) :
        public CMRAttachmentCommand
    {
public: // Destruction

    /**
     * Creates and initializes new CMROpenAttachmentCommand object.
     * Ownership is transferred to caller.
     *
     * @return Pointer to CMROpenAttachmentCommand object.
     */
    static CMROpenAttachmentCommand* NewL( CDocumentHandler& aDocHandler );
    ~CMROpenAttachmentCommand();

public: // From base class CMRAttachmentCommand
    void ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex );

private: // Implementation
    CMROpenAttachmentCommand( CDocumentHandler& aDocHandler );
    void ConstructL();

private:

    /// Ref: Document handler
    CDocumentHandler& iDocHandler;
    };

#endif // CMROPENATTACHMENTCOMMAND_H

// EOF
