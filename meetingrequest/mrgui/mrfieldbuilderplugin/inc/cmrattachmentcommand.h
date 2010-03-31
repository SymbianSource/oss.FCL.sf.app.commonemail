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

#ifndef CMRATTACHMENTCOMMAND_H
#define CMRATTACHMENTCOMMAND_H

#include <e32base.h>
#include <apmstd.h>
#include "esmrdef.h"

// Forward declarations
class CCalEntry;
class CESMRRichTextLink;
class RFile;

/**
 * CMRAttachmentCommandHandler is responsible for handling attachment
 * related commands.
 */
NONSHARABLE_CLASS( CMRAttachmentCommand ) : public CBase
    {
public: // Destruction
    
    /**
     * C++ destructor
     */
    virtual ~CMRAttachmentCommand();
    
public: // Interface

    /**
     * Executes attachment command.
     * @param aEntry Reference to attachemnt entry
     * @param aAttachmentIndex Index of attachment to be processed
     */
    virtual void ExecuteAttachmentCommandL(
            CCalEntry& aEntry,
            TInt aAttachmentIndex ) = 0;
    
protected:

    CMRAttachmentCommand();
    TDataType GetMimeType( RFile& aFile );
    
private:   
    };

#endif // CMRATTACHMENTCOMMAND_H

// EOF
