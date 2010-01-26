/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  data container class
*
*/


#ifndef CMAILMAILBOXDETAILS_H_
#define CMAILMAILBOXDETAILS_H_

class CMailMessageDetails;

/**
 *  Class holding data related to mailbox 
 *
 *  @lib fsmailserver.exe
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CMailMailboxDetails ) : public CBase
    {
public:
    /**
     * Two-phased constructor.
     * @param aMailboxId Id of the mailbox
     * @param aMailboxName name of the mailbox
     */ 
    static CMailMailboxDetails* NewL( TFSMailMsgId aMailboxId, const TDesC& aMailboxName );

    /**
    * Destructor.
    */
    virtual ~CMailMailboxDetails();

    /**
    * Destructor.
    */
    void SetWidgetInstance( const TDesC& aWidgetInstance );

    /**
     * Changes the mailbox name.
     */
    void SetMailboxName( const TDesC& aMailboxName );

private:

    /**
     * Constructor
     * @param aMailboxId Id of the mailbox
     * @param aMailboxName name of the mailbox
     */ 
    CMailMailboxDetails( const TFSMailMsgId aMailboxId );

    /**
     * ConstructL
     * @param aMailboxName name of the mailbox
     */ 
    void ConstructL( const TDesC& aMailboxName ); 

public: // data
    // For bookkeeping
    // id of the mailbox in the message store
    TFSMailMsgId                        iMailboxId;
    // array of message details objects
    RPointerArray<CMailMessageDetails>  iMessageDetailsArray;
    // Data
    // mailbox name
    HBufC*                              iMailboxName;
    
    HBufC*                              iWidgetInstance;
    };

#endif /*CMAILMAILBOXDETAILS_H_*/
