/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines class CMailMessageDetails.
*
*/

#ifndef CMAILEXTERNALACCOUNT_H_
#define CMAILEXTERNALACCOUNT_H_

#include <e32def.h>
#include <e32base.h>

/**
 *  Container class external email account
 *
 *  @lib
 *  @since S60 v9.2
 */
NONSHARABLE_CLASS( CMailExternalAccount ) : public CBase
    {
public:
    /** public constructor */
    static CMailExternalAccount* NewL(
        const TInt aMailboxId,
        const TInt aPluginId,
        HBufC* aContentId );

    /** destructor */
    virtual ~CMailExternalAccount();

    // Member accessors
    TInt MailboxId();
    TInt PluginId();
    TDesC& ContentId();
    
private:

    void ConstructL();

    CMailExternalAccount(
        const TInt aMailboxId,
        const TInt aPluginId,
        HBufC* aContentId );

private: // data
    // actual identifier of the mailbox (unique within plugin context)
    TInt iMailboxId;
    // identifier for plugin
    TInt iPluginId;
    // identifier for the widget on homescreen
    HBufC* iContentId; 
    };

#endif /* CMAILEXTERNALACCOUNT_H_ */
