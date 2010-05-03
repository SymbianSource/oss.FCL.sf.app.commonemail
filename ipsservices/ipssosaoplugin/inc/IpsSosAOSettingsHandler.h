/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Class to handle imap/pop/smtp settings & cenrep
*
*/


#ifndef IPSSOSAOSETTINGSHANDLER_H_
#define IPSSOSAOSETTINGSHANDLER_H_

#include <e32base.h> 
#include <imapset.h>

#include "ipsplgcommon.h"
#include "nmipssettingitems.h"



/**
* class CIpsSosAOSettingsHandler
*
* @lib ipssosaoplugin.lib
*/
NONSHARABLE_CLASS (CIpsSosAOSettingsHandler) : public CBase
    {
    
public:
    
    /*
     * NewL
     */
    static CIpsSosAOSettingsHandler* NewL( TUid aProtocol );
    
    /*
     * Destructor
     */
    virtual ~CIpsSosAOSettingsHandler();
    
    /**
     * List folders subscribed to be synced.
     */
    void GetSubscribedImapFoldersL( 
            TMsvId aServiceId, 
            RArray<TMsvId>& aFoldersArray );
    
    /**
     * 
     */
    void ConstructImapPartialFetchInfo( 
            TImImap4GetPartialMailInfo& aInfo, 
            CImImap4Settings& aImap4Settings );
    
    /**
     * @return state of alwaysonline
     */
    IpsServices::TIpsSetDataAoStates AlwaysOnlineState() const;
    
    /**
     * @param aAlwaysOnlineState, new state for alwaysonline
     */
    void SetAlwaysOnlineState(
        const IpsServices::TIpsSetDataAoStates aAlwaysOnlineState,
        TBool aIgnoreStateFlag=EFalse );
    
    /**
     * Fetches the state of email notification -flag
     * @return ETrue or EFalse, according to flag state
     */
    IpsServices::TIpsSetDataEmnStates EmailNotificationState() const;
    
    /**
     * Sets the state of email notification -flag
     */
    void SetEmailNotificationState(
        const IpsServices::TIpsSetDataEmnStates aEmnState );
        
    /**
     * Returns flag which indicates has mailbox received 
     * it's first OMA EMN. 
     */
    TBool FirstEmnReceived() const;
    
    /**
     * Sets a flag when mailbox receives it's first OMA EMN.
     * (email notification)
     */
    void SetFirstEmnReceived( TBool aValue );
    
    /**
    *
    */
    TBool EmnReceivedButNotSyncedFlag() const;
        
    /**
    *
    */
    void SetEmnReceivedButNotSyncedFlag( TBool aFlag );
    
    /**
     * @return week days selected when sync is active
     */
    TUint SelectedWeekDays() const;
    
    /**
     * @return 
     */
    TTime SelectedTimeStart() const;
    
    /**
     * @return 
     */
    TTime SelectedTimeStop() const;
    
    /**
     * @param aLastUpdateInfo
     */
    void SetLastUpdateInfo( const IpsServices::TAOInfo& aLastUpdateInfo );

    /**
     * @return
     */
    IpsServices::TAOInfo LastUpdateInfo() const;
    
    /**
     * @return
     */
    TInt InboxRefreshTime() const;
    
    /**
    * Are we allowed to sync only in home network
    * @return true, if home only is selected
    */
    TBool RoamHomeOnlyFlag();
    
protected:
    
    /*
     * Constructor 
     */
    CIpsSosAOSettingsHandler();
    
    
private:
    
    /*
     * 2nd phase constructor 
     */
    void ConstructL( TUid aProtocol );
    
    
    /**
     * Gets FS plugin id related to protocol
     * @return FS plugin id
     */
    TInt GetFSPluginId() const;
        
private:
    
    TPckgBuf<TIpsPlgPropertyEvent>  iPropertyBuf;
    TUid                            iMtmType;
    
    };

#endif /*IPSSOSAOSETTINGSHANDLER_H_*/
