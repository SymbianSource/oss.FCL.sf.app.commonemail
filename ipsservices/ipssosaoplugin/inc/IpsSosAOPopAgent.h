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
* Description: 
*     Contains pop specified sync logic
*
*/


#ifndef IPSSOSAOPOPAGENT_H_
#define IPSSOSAOPOPAGENT_H_

#include <pop3set.h>
#include <mtclreg.h>

#include "IpsSosAOBaseAgent.h"


class CPop3ClientMtm;

/**
* class CIpsSosAOPopAgent;
*
* Is created by IpsSosAOBaseAgent, contains pop spesific 
* implementations of base class pure virtual functions.
*
*/
class CIpsSosAOPopAgent : public CIpsSosAOBaseAgent
    {
public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOPopAgent* NewL( 
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOPopAgent();
    
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
    
private:

    /**
    * Constructor
    */
    CIpsSosAOPopAgent(             
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
    TBool                               iSyncStartedSignaled;
    TAgentState                         iState;
    TBool                               iDoNotDisconnect;
    CImPop3Settings*                    iPopSettings;
    CMsvOperation*                      iOngoingOp;

    TPckgBuf<TPop3Progress>             iProgressBuf;
    CMsvEntrySelection*                 iSelection;
    TInt                                iError;
    CPop3ClientMtm*                     iPopClientMtm;
    CClientMtmRegistry*                 iMtmReg;
    TInt                                iPopulateLimit;
    
    };


#endif /*IPSSOSAOPOPAGENT_H_*/
