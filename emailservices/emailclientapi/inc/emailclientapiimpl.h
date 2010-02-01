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
* Description: Definition of Email API main interface.
*
*/

#ifndef EMAILCLIENTAPIIMPL_H
#define EMAILCLIENTAPIIMPL_H


#include <ecom/implementationinformation.h> 
#include "emailapiutils.h"
#include "cfsmailcommon.h"
//#include "emailinternalclientapi.h"
#include "mfsmaileventobserver.h"    
#include <memailclientapi.h>
#include "emailclientpluginmanager.h"


using namespace EmailInterface;

class CFSMailBox;
class MEmailEventObserver;
class CVwsSessionWrapper;
class CFSMailPlugin;
class TFSMailMsgId;
class CEmailMailboxCache;
class CFSClientAPI;

/**
*  "Framework" class including plugin management
* @since S60 v5.2
*/
NONSHARABLE_CLASS( CEmailClientApi ) : public CBase, public MEmailClientApi, public MEmailClientPluginManager
{
public:
    /**
    * Constructor
    * @return
    */
    static CEmailClientApi* NewL();
    
    /**
     * Destructor
     */
    ~CEmailClientApi();
    
    /**
     *
     */
     CEmailClientApi();       

public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();
    
public: // from MEmailClientApi

    /** @see MEmailClientApi */   
    virtual MEmailMailbox* MailboxL( const TMailboxId& aId );
   
    /** @see MEmailClientApi */   
    virtual MEmailMailbox* MailboxL( const TPtrC& aAddress );
           
    /**
    * Gets all mailboxes in pointer array. It is recommended to free other
    * mailboxes besides needed one to optimise memory usage.
    */    
    virtual TInt GetMailboxesL( RMailboxPtrArray& aMailboxes );
    
    /**
    */
    virtual void LaunchEmailL( const TLaunchPolicy aPolicy );
    
public: // from MEmailPluginManager
    virtual CFSMailPlugin* GetPluginByUid(TUid aUid);    

public: // new methods

    /**
     * Gets array of mailbox ids
     * @param aMailboxes mailbox ids from all plugins
     * @return number of mailboxes returned in aMailboxes
     */
    TInt GetMailboxIdsL( REmailMailboxIdArray& aMailboxes );
        
private:

    /**
     * Constructor
     */
    void ConstructL();

    /**
     * Push ECom implementation info to cleanup operation to cleanup stack
     */
    void CleanupImplInfoPushL( RPointerArray<CImplementationInformation>& aArray );
    
    /**
     * Cleanup operation for ECom implementation info
     */
    static void CleanupImplInfo( TAny* aAny );
   

    // Group of methods for book keeping of loaded protocol plugins by this object.
    // In certain situations it is preferable to have a plugin in memory
    // instead of loading/unloading frequently.
    
    /** Increases plugin reference count if CEmailClientApi already doesn't
     *  have active instance. When plugin is needed internally, this method
     * should be accessor to a plugin. Counterpart is ReleasePlugin() when
     * CEmailClientApi doesn't itself need plugin any more.
     */
    CFSMailPlugin* UsePlugin( CPluginData& aPluginData );
    
    /**
    * For finding plugin data
    */
    TInt IndexOfLoadedPluginData( const TPluginData& aPluginData ) const;
    
    /**
    * Used in comparing plugin data in array (part of find algorithm)
    */    
    static TBool PluginDataEquals( const TPluginData& a1, const TPluginData& a2 );
    
    /**
    * This object doesn't need related protocol plugin
    */
    void ReleasePlugin( CPluginData& aPluginData );
    
    /**
    * This object doesn't need any protocol plugin
    */
    void ReleaseAllPlugins();
    
    /**
    * Return mailbox cache, creates it when called first time
    */
    CEmailMailboxCache& MailboxInfoCacheL();
    
    /**
    * Caches mailbox ids and their mapping to related plugin data for quick
    * lookup. When a mailbox with specific id is next time asked, related
    * protocol plugin is already known.
    */
    void UpdateMailboxInfoCacheL();

    /**
    * Internal, used from UpdateMailboxInfoCacheL
    */    
    TBool CachePluginMailboxesL( 
        CPluginData& aPluginData, 
        CFSMailPlugin& aPlugin );
    
private:    
    typedef RPointerArray<CPluginData> RPluginDataArray;
    
    /**
    * Iterator for CPluginData elements in an array
    */
    class TPluginIterator
    {
    public:
        // constructor
        inline TPluginIterator( RPluginDataArray& aArray ) : 
            iArray( aArray ), iIndex( 0 ) {}
        
        /**
        * return next element
        */
        CPluginData* Next();
    private:
        // iterator array
        RPluginDataArray& iArray;
        // current iterator index
        TInt iIndex;
    };
#if 0 // Not supported    
    /**
     * Internal helper class for handling events related to mailboxes
     *(adding/removal/rename).
     */
    class CObserverHandler : public CBase, public MFSMailEventObserver
    {
    public:
        static CObserverHandler* NewL( const RPluginDataArray& aArray  );
        ~CObserverHandler();
        CObserverHandler( const RPluginDataArray& aArray );
        void ConstructL();
        void AddObserverL( MMailboxEventObserver& aObserver );
        void RemoveObserver( MMailboxEventObserver& aObserver );
        
    public: // from MFSMailEventObserver

        virtual void EventL(TFSMailEvent aEvent, TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3);
    
    private: // new methods
        
        // used for finding oberver
        static TBool Equals( const MMailboxEventObserver& a1, const MMailboxEventObserver& a2 );
        
        // Plugins needed when first observer subscribes events
        void ClaimPluginsAndStartObservingL();
        
        // plugins can be released when last observer removed
        void ReleasePlugins();
                
    private: // data

        // Plugin refCount is incremented by one while any observer is 
        // registered. This prevents plugin unloading if no other entity uses 
        // the plugin. 
        class TEntry
        {
        public:
            inline TEntry( CPluginData& aData ) : iData( aData ), iPlugin( NULL ) {}
            CPluginData& iData;    
            
            // claimed from same object where iData refers to
            CFSMailPlugin* iPlugin;
        };
        
        // plugin entry array
        RArray<CObserverHandler::TEntry>     iEntryArray;
        
        // reference to CEmailClientApi::iPluginDataArray
        const RPluginDataArray&              iPluginDataArray;
        
        // registered observers
        RPointerArray<MMailboxEventObserver> iMailboxObservers;
    };
#endif    
    /**
     * Returns reference to observer handler.
     */
#if 0 // Not supported    
    CEmailClientApi::CObserverHandler& ObserverHandlerL();
#endif    
    
private:
    // plugin data array for all protocol plugins
    RPluginDataArray    iPluginDataArray;
    
    // book keeping of loaded plugins
    RArray<TPluginData> iLoadedPluginsArray;
    
#if 0 // Not supported    
    // orchestrates event monitoring and related plugin references
    CEmailClientApi::CObserverHandler*   iObserverHandler;
    
#endif
    
    // Mailbox ids are cached when first time listed. Related plugin uid
    // is include in the cache.
    CEmailMailboxCache* iMailboxCache;

    CFSClientAPI* iClientAPI;
    
};

/**
* Email application launcher. 
*/
NONSHARABLE_CLASS( CEmailLauncher ) : public CBase
{
public:
    static CEmailLauncher* NewL();
    
    ~CEmailLauncher();
                                    
    /**
    *
    */
     void LaunchL( const TMailboxId& aMailboxId );
private:
    
    CEmailLauncher();
    
    void ConstructL();
    
    // Owned window server session.
    CVwsSessionWrapper* iViewSrvSession;
};


#endif // EMAILCLIENTAPIIMPL_H

// End of file.
