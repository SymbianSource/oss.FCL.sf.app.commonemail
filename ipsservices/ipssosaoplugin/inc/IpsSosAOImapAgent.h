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
* Description: 
*     Contains imap specified sync logic
*
*/


#ifndef IPSSOSAOIMAPAGENT_H_
#define IPSSOSAOIMAPAGENT_H_

#include <imapconnectionobserver.h>
#include <imapset.h>
#include <mtclreg.h>
//<cmail>
#include "MFSMailRequestObserver.h"
//</cmail>

#include "IpsSosAOBaseAgent.h"

class CImap4ClientMtm;
//<QMail>

//</QMail>
/**
* class CIpsSosAOImapAgent;
*
* Is created by IpsSosAOBaseAgent, contains imap spesific implementations of base class pure
* virtual functions.
*
*/
NONSHARABLE_CLASS (CIpsSosAOImapAgent) : public CIpsSosAOBaseAgent,
                           public MMsvImapConnectionObserver,
                           public MFSMailRequestObserver
    {
public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOImapAgent* NewL( 
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOImapAgent();
    
public: // from CIpsSosAOBaseAgent
    
    virtual void GetServerAddress( 
            TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const;
    
    virtual void GetUsername( 
            TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const;
    
    virtual TBool IsConnected() const;
    
    virtual void StartSyncL();
    
    virtual void StartFetchMessagesL( const RArray<TMsvId>& aFetchMsgArray );
    
    virtual void CancelAllAndDisconnectL();
    
    virtual void CancelAllAndDoNotDisconnect();
    
    virtual void DoNotDisconnect();
    
    virtual void ClearDoNotDisconnect();
    
    virtual void HoldOperations();

    virtual void ContinueHoldOperations();
    
    virtual CIpsSosAOBaseAgent::TAgentState GetState() const;
    
    virtual void LoadSettingsL();
    
public: // MMsvImapConnectionObserver  
    
    /**
    * @param TImapConnectionEvent, imap connection event
    */
    virtual void HandleImapConnectionEvent(
        TImapConnectionEvent aConnectionEvent);
    
public: // MFSMailRequestObserver
    
    /**
    * used as dummy observer, needed for population operation
    */
    virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    
private:
    
    /**
    * Constructor
    */
    CIpsSosAOImapAgent(             
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Second phase constructor
    */
    void ConstructL();
    
    virtual void DoCancel();
    
    virtual void RunL();
    
    virtual TInt RunError( TInt aError );

    void PopulateAllL();
    
    inline void SetActiveAndCompleteThis();

private:
    
    CMsvSession&                        iSession;
    MIpsSosAOAgentOperationResponse&    iOpResponse;
    TMsvId                              iServiceId;
    TAgentState                         iState;
    TBool                               iDoNotDisconnect;
    CImImap4Settings*                   iImapSettings;
    CMsvOperation*                      iOngoingOp;

    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    TInt                                iError;
    CImap4ClientMtm*                    iImapClientMtm;
    CClientMtmRegistry*                 iMtmReg;
    RArray<TMsvId>                      iFoldersArray;
	//<QMail>

    //</QMail>
    };

#endif /*IPSSOSAOIMAPAGENT_H_*/
