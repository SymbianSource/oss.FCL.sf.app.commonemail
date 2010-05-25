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
* Description:  Message extension interface
*
*/

#ifndef CMAILBOXSTATEEXT_H
#define CMAILBOXSTATEEXT_H

#include "cemailextensionbase.h"
#include "emailextensionuids.hrh"
#include "cfsmailcommon.h"

// interface UID value passed to CFSMailClient::ExtensionL

/**
* Data providider interface.
*/
class MEmailMailboxState
{
public:
    /**
    * Returns currently active folder and related mailbox that
    * is currently being used.
    * @param aActiveMailboxId id of currently active mailbox 
    * @param aActiveFolderId id of currently active mail folder
    * @return Symbian OS error code
    */
    virtual TInt GetActiveFolderId( 
        TFSMailMsgId& aActiveMailboxId,
        TFSMailMsgId& aActiveFolderId ) const = 0;
};

/**
 * Extension interface for passing UI context data to protocol plugins. 
 */
class CMailboxStateExtension : public CEmailExtension
{
public:
    
    /**
     * Sets data provider interface
     * @param aDataProvider data provider
     */
    virtual void SetStateDataProvider( MEmailMailboxState* aDataProvider ) = 0;

protected:    
    inline CMailboxStateExtension();

protected:
    MEmailMailboxState* iDataProvider;
};

inline CMailboxStateExtension::CMailboxStateExtension() :
         CEmailExtension( KEmailMailboxStateExtensionUid )
    {
    }
         
#endif // CMAILBOXSTATEEXT_H
