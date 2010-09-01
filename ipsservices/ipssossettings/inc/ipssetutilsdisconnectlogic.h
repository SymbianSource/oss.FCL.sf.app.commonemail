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
* Description: This file defines class CIpsSetUtilsDisconnectLogic.
*
*/


#ifndef IPSSETUTILSDISCONNECTLOGIC_H_
#define IPSSETUTILSDISCONNECTLOGIC_H_

#include <e32base.h>
#include <AknWaitNoteWrapper.h>
#include <msvapi.h>

class CBaseMtm;
class CClientMtmRegistry;

/**
 *  Contains logic to disconnect mailbox when editing settings
 *
 *  @lib IpsSosSettings.lib
 *  @since FS v2.0
 */
NONSHARABLE_CLASS( CIpsSetUtilsDisconnectLogic ) : public CActive,
                                    public MAknBackgroundProcess
    {
public: // Constructors and destructor

    /**
     * Destructor
     */
    ~CIpsSetUtilsDisconnectLogic();

    /**
     * 2-phase contructor
     *
     * @return Approver object with client ownership.
     */
    static CIpsSetUtilsDisconnectLogic* NewL( 
            CMsvSession& aSession, 
            TMsvId aMailboxId );

public: // from MAknBackgroundProcess
        
    /**
    * 
    */
    virtual void StepL();
    
    /**
    *
    */
    virtual TBool IsProcessDone() const;
    
    /**
    * 
    */
    virtual void ProcessFinished();

    /**
    * 
    */
    virtual void DialogDismissedL( TInt aButtonId );

    /**
    * 
    */
    virtual TInt CycleError( TInt aError );

public: // from CActive
    
    /**
    * 
    */
    virtual void DoCancel();
    
    /**
    * 
    */
    virtual void RunL();
    
    /**
    * 
    */
    virtual TInt RunError( TInt aError );
  
public:
  
    /*
     * Checks mailbox connection state and disconnects 
     * @return KErrNone if mailbox is disconnected, 
     *         KErrCancel if user not want to disconnect
     */
    TInt RunDisconnectLogicL( TBool aDoShowQuery );
    
private:

    /**
     * Constructor.
     */
    CIpsSetUtilsDisconnectLogic( 
            CMsvSession& aSession, 
            TMsvId aMailboxId );

    /**
     * 2nd phase of construction.
     */
    void ConstructL();

    /**
    * 
    */    
    TInt CheckConnectionAndQueryDisconnectL( 
            TMsvEntry& aEntry, 
            TBool aDoShowQuery );
 
    /**
    * 
    */    
    void CreateDisconnectOperationL(const TMsvEntry& aEntry );
    
private: 
    
    CMsvSession&            iSession;
    TInt                    iMailboxId;
    TBool                   iIsDisconnectComplete;

    CMsvOperation*          iOperation;
    CBaseMtm*               iBaseMtm;
    CClientMtmRegistry*     iMtmReg;
    CAknWaitNoteWrapper*    iAsyncWaitNote;
    
    };
    
       

#endif /*IPSSETUTILSDISCONNECTLOGIC_H_*/
