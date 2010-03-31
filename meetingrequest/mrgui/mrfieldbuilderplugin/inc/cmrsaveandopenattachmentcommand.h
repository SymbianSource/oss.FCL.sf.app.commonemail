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
* Description:  MR Attachment field command to save and open attachment
 *
*/

#ifndef CMRSAVEANDOPENATTACHMENTCOMMAND_H
#define CMRSAVEANDOPENATTACHMENTCOMMAND_H

#include <e32base.h>
#include <aknserverapp.h>
#include "cmrattachmentcommand.h"

// Forward declarations
class CCalEntry;
class CDocumentHandler;

/**
 * CMRSaveAttachmentCommand is responsible for saving attachment
 */
NONSHARABLE_CLASS( CMRSaveAndOpenAttachmentCommand ) : 
        public CMRAttachmentCommand
    {
public: // Construction and destructors
    
    static CMRSaveAndOpenAttachmentCommand* NewL( CDocumentHandler& aDocHandler );
    ~CMRSaveAndOpenAttachmentCommand();
    
public: // CMRAttachmentCommand

    void ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex );
    
private: // Implementation
    CMRSaveAndOpenAttachmentCommand( CDocumentHandler& aDocHandler );
    void ConstructL();

private:
    
    /// Ref: Document handler
    CDocumentHandler& iDocHandler;
    
    };

#endif // CMRSAVEANDOPENATTACHMENTCOMMAND_H

// EOF
