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
* Description:  settings extension for mailbox
*
*/
#ifndef CEMAILSETTINGSEXTENSIONIMPL_H
#define CEMAILSETTINGSEXTENSIONIMPL_H
                          
#include "cemailsettingsextension.h" 
#include "cemailextensionbase.h"

/**
* Implementation of mailbox settings extension
*/
NONSHARABLE_CLASS( CEmailSettingsExtensionImpl ) : public CEmailSettingsExtension
{
public:

    CEmailSettingsExtensionImpl();
    CEmailSettingsExtensionImpl(CMsvSession* iSession);    
    ~CEmailSettingsExtensionImpl();
    
public: // from CEmailSettingsExtension
    /**
    * setting value getter
    */
    virtual void GetSettingValue( const TDesC& aKey, TDesC8& aValue  ) const;

    /**
    * setting value setter
    */    
    virtual void SetSettingValue( const TDesC& aKey, const RBuf8& aValue);
    
    /**
    * Test setting value
    */    
    virtual TBool IsSetL( const TDesC& aKey );
    
    /**
    * mailbox id getter & setter
    */    
    virtual void SetMailBoxId( const TFSMailMsgId& aMailBoxId );
    virtual TFSMailMsgId GetMailBoxId( );
    
private: // data
    
    // owner mailbox id
    TFSMailMsgId iMailBoxId;

    // Symbian message server session
    CMsvSession* iSession;

};

#endif // CEMAILSETTINGSEXTENSIONIMPL_H
