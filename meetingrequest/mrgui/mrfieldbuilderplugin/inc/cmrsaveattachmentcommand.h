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
* Description:  MR Attachment field command to save attachment
 *
*/

#ifndef CMRSAVEATTACHMENTCOMMAND_H
#define CMRSAVEATTACHMENTCOMMAND_H

#include <e32base.h>
#include "cmrattachmentcommand.h"

// Forward declarations
class CCalEntry;
class CDocumentHandler;

/**
 * CMRSaveAttachmentCommand is responsible for saving attachment
 */
NONSHARABLE_CLASS( CMRSaveAttachmentCommand ) :
        public CMRAttachmentCommand
    {
public: // Construction and destructors

    static CMRSaveAttachmentCommand* NewL( CDocumentHandler& aDocHandler );
    ~CMRSaveAttachmentCommand();

public: // CMRAttachmentCommand

    void ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex );

private: // Implementation
    CMRSaveAttachmentCommand( CDocumentHandler& aDocHandler );
    void ConstructL();

private: // Data
    /// Ref: Document handler
    CDocumentHandler& iDocHandler;
    };

#endif // CMRSAVEATTACHMENTCOMMAND_H

// EOF
