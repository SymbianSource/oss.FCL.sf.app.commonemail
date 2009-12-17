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
* Description:  MRUI attachmentinfo definition
*
*/


#ifndef TESMRATTACHMENTINFO_H
#define TESMRATTACHMENTINFO_H

#include <e32base.h>

/**
 * TESMRAttachmentInfo encapsulate
 *
 * @lib esmrservices.lib
 */
class TESMRAttachmentInfo
    {
public:
    enum TESMRAttachmentState
        {
        EAttachmentStateUnknown,
        EAttachmentStateNotDownloaded,
        EAttachmentStateDownloaded
        };
    };

#endif // TESMRATTACHMENTINFO_H

// EOF
