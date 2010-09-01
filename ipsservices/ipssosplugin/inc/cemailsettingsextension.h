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
* Description: mailbox settings extension
*
*/

#ifndef CEMAILSETTINGSEXTENSION_H
#define CEMAILSETTINGSEXTENSION_H

#include "cemailextensionbase.h"
#include "emailextensionuids.hrh"

#include "cfsmailcommon.h"

// interface UID value passed to CFSMailBox::ExtensionL
_LIT( EmailSyncInterval, "EmailSyncInterval" );

/**
 * settings extension interface
 * 
 */
class CEmailSettingsExtension : public CEmailExtension
{
public:
    
    /**
    * setting value getter
    */
    virtual void GetSettingValue( const TDesC& aKey, TDesC8& aValue ) const = 0;

    /**
    * setting value setter
    */    
    virtual void SetSettingValue( const TDesC& aKey, const RBuf8& aValue ) = 0;

    /**
    * Test if setting is set
    */    
    virtual TBool IsSetL( const TDesC& aKey ) = 0;

    /**
    * mailbox id getter & setter
    */    
    virtual void SetMailBoxId( const TFSMailMsgId& aMailBoxId ) = 0;
    virtual TFSMailMsgId GetMailBoxId( ) = 0;

protected:    
    CEmailSettingsExtension( const TUid& aUid );
};

#endif // CEMAILSETTINGSEXTENSION_H
