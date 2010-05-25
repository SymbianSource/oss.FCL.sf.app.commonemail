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
* Description: This file implements class CEmailMailbox.
*
*/

#ifndef EMAILMAILBOX_H
#define EMAILMAILBOX_H

#include "cfsmailcommon.h"
#include <memailmailbox.h>
#include "mfsmaileventobserver.h"
#include "mfsmailrequestobserver.h"
#include "mmailboxcontentobserver.h"

using namespace EmailInterface;

class CFSMailBox;
class CFSMailAddress;
class CFSMailPlugin;
class CPluginData;
class CEmailAddress;

NONSHARABLE_CLASS( CEmailMailbox ) : 
    public CBase, 
    public MEmailMailbox
{
public:
    
    static CEmailMailbox* NewL( 
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );
    
    ~CEmailMailbox();           

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();
    
public:    // from MEmailMailbox
    /**@see MEmailMailbox */
    virtual TMailboxId MailboxId() const;
    
    /**@see MEmailMailbox */
    virtual MEmailAddress* AddressL() const;
    
    /**@see MEmailMailbox */
    virtual TPtrC MailboxName() const;
    
    /**@see MEmailMailbox */
    virtual  TInt GetFoldersL( RFolderArray& aFolders ) const;

    /**@see MEmailMailbox */
    virtual MEmailFolder* FolderL( const TFolderId& aFolderId ) const;
    
    /**@see MEmailMailbox */
    virtual MEmailFolder* FolderByTypeL( 
        const TFolderType aFolderType ) const;
        
    /**@see MEmailMailbox */
    virtual MEmailMessage* MessageL( const TMessageId& aMessageId );
    
    /**@see MEmailMailbox */
    virtual MEmailMessage* CreateDraftMessageL() const;
    
    /**@see MEmailMailbox */
    virtual MEmailMessage* CreateReplyMessageL( const TMessageId& aMessageId, const TBool aReplyToAll = ETrue ) const;
    
    /**@see MEmailMailbox */
    virtual MEmailMessage* CreateForwardMessageL( const TMessageId& aMessageId ) const;
    
    /**@see MEmailMailbox */
    virtual void SynchroniseL( MMailboxSyncObserver& aObserver );
        
    /**@see MEmailMailbox */
    virtual void CancelSynchronise();
    
    /**@see MEmailMailbox */
    virtual void RegisterObserverL( MMailboxContentObserver& aObserver );
        
    /**@see MEmailMailbox */
    virtual void UnregisterObserver( MMailboxContentObserver& aObserver );
    
    /**@see MEmailMailbox */
    virtual MEmailMessageSearchAsync* MessageSearchL();

    /**@see MEmailMailbox */
    virtual void ShowInboxL();

    /**@see MEmailMailbox */
    virtual void EditNewMessageL();
    

public: // new methods

private:       
    TFSMailMsgId FsMailboxId() const;
    
private: // constructors
    CEmailMailbox( CPluginData& aPluginData, const TMailboxId& aPlugin );
    
    void ConstructL();

private: // inner class for mailbox event handling
    class TObserverEventMapper : public MFSMailEventObserver
    {
    public:
        TObserverEventMapper( CFSMailPlugin* aPlugin, TUid aPluginUid, const TMailboxId& aMailboxId );
        ~TObserverEventMapper();
        void AddObserverL( MMailboxContentObserver& aObserver );
        void RemoveObserver( MMailboxContentObserver& aObserver );
    public: // from MFSMailEventObserver
        virtual void EventL(TFSMailEvent aEvent, TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        typedef void (CEmailMailbox::TObserverEventMapper::*TEventMapFunc)( TMailboxId, TAny*, TAny*, TAny* );
    private: // mappers, so say we all!
        void ConvertParamsL( TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, REmailMessageIdArray& aMessageIds, TFolderId& aFolderId );
        void IgnoreEventL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void NewMessageL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageChangedL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageDeletedL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageMoved( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void MessageCopiedL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void NewFolderL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FolderChangeL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FoldersDeletedL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void FoldersMovedL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
        void ExceptionL( TMailboxId     aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3 );
    
    private:                                                                                                                         
        static TBool Equals( const MMailboxContentObserver& a1, const MMailboxContentObserver& a2 );
    private:
        CFSMailPlugin*  iPlugin;
        //const TUid      iUid;
        TFSMailMsgId    iFsMailboxId;
        RPointerArray<MMailboxContentObserver> iClientObservers;
    };
    class CEmailRequestObserver: public CBase,
                                 public MFSMailRequestObserver                                 
        {
    public:
        CEmailRequestObserver();
        
        virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        void SetObserverL( MMailboxSyncObserver* aObserver );
    private:
        MMailboxSyncObserver* iObserver;
        };
        
private:        

    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMailboxId      iMailboxId;
    
    TObserverEventMapper* iEventMapper;

    mutable CEmailAddress* iAddress;

    CFSMailBox*     iFsMailbox;
    
    CEmailRequestObserver* iSyncObserver;    
    
    TInt iRequestId;
};
    
#endif // EMAILMAILBOX_H

// End of file
