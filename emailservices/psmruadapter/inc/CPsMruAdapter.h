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
* Description:  Predictive Search MRU ADapter 
*
*/


#ifndef C_PS_MRU_ADAPTER_H
#define C_PS_MRU_ADAPTER_H

// INCLUDES
#include <CPsDataPlugin.h>
//<cmail>
#include "mfsmaileventobserver.h"
//</cmail>

// FORWARD DECLARATION
class CPsData;
class CFSMailClient;
class MDesCArray;

class CDelayMailboxCreationHelper;
// CLASS DECLARATION

/**
* MRU Data Store
* This class acts as a MRU Data store, which allows access to 
* FS Email Framework MRU data 
* 
* @since S60 v3.2
*/
class CPsMruAdapter : public CPsDataPlugin, public MFSMailEventObserver
    {
    public: 
    
        /**
         * Two phase construction
         */
        static CPsMruAdapter* NewL(TAny* aPsDataPluginParameters);
    
        /**
         * Destructor
         */
        virtual ~CPsMruAdapter();
    
        /**
         * Returns the additional data for given item id
         * Returns NULL (in current implementation)
         */
        TAny* RequestForDataExtensionL( TInt ItemId );

// from base class CPsDataPlugin

        /**
         * Requests for data from this store
         * The adapters need to pass an instance of the observer. The data 
         * to the algorithm is provided via the observer callback 
         */
        void  RequestForDataL( TDesC& aDataStoreURI );      

        /**
         * Gets the supported data stores URIs
         * 
         * @param aDataStores supported data stores URIs
         */  
        void GetSupportedDataStoresL( RPointerArray<TDesC>& aDataStoresURIs );

        /**
         * Checks if the given data store is supported.
         * 
         * @param aDataStoreURI data store
         * @return True if this store is supported        
         */  
        TBool IsDataStoresSupportedL( TDesC& aDataStoreURI ) ;

        /**
         * Gets the supported data fields for specific stores URIs
         * 
         * @param aDataFields supported data fields for aStoreURI         
         */ 
        void GetSupportedDataFieldsL( RArray<TInt>& aDataFields );

// from base class MFSMailEventObserver

        /**
         * Handles mailbox events from plugins.
         */
        void EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox, 
            TAny* aParam1, TAny* aParam2, TAny* aParam3 );

        /**
         * If problem with NewMailbox this function will be called 
         * by CDelayMailboxCreationHelper to try it after some delay
         */
        TBool DeleayedMailboxCreationEventL();
        
        /**
         * If problem with NewMailbox this function will be called 
         * by CDelayMailboxCreationHelper to try it after some delay
         */
        void DeleayMailboxCreationEventL( TFSMailMsgId &aMailbox );
        
    private:

        /**
         * Constructor
         */
        CPsMruAdapter();
        
        /**
         * 2nd phase construtor
         */
        void ConstructL(MDataStoreObserver* aObserverForDataStore,
            MStoreListObserver* aStoreListObserver);
        
        /**
         * Updates given data store
         */         
        TBool FillDataStoreL( TDesC& aDataStoreURI );
        
        /**
         * Updates given data store
         */                 
        TBool FillDataStoreL( TFSMailMsgId& aId );
        
        /**
         * Updates a list of supported data stores
         */         
        void UpdateSupportedDataStoresList();       
        
        /**
         * Starts observing mailbox
         */
        TBool AddMailboxObserverL( TFSMailMsgId& aId );
        
        /**
         * Stops observing mailbox
         */
        TBool RemoveMailboxObserver( TFSMailMsgId& aId );
        
        /**
         * Stops all mailbox observers
         */
        void RemoveAllMailboxObservers();
        
        /**
         * Converts Uri to TFsMailMsgId
         */
        TBool GetMailboxIdentifierFromUri( TDesC& aUri, TFSMailMsgId& aId );
        
        /**
         * Converts TFsMailMsgId to Uri
         */
        TBool GetUriFromMailboxIdentifier( TFSMailMsgId& aId, HBufC& aUri );
        
        /**
        * Updates given data store
        */                  
        TBool FillDataStoreL( TFSMailMsgId& aId, TDesC& aDataStoreURI );        
        
        /**
        * Calls MDataStoreObserver::AddData for every mru email - used for trapping 
        */                  
        void AddMruEmailsL( MDesCArray* aMruList, TDesC& aDataStoreURI );

    private: // data

        /**
         * Holds the observer object to communicate add/modify/delete of contacts
         * Not owned.
         */
        MDataStoreObserver* iDataStoreObserver;
        
        /**
        * An observer instance used to send the datastore to the adapter
        */
        MStoreListObserver* iStoreListObserver;     
            
        /**
        * Supported Uris(data stores)
        */
        RPointerArray<HBufC> iSupportedUris;    
        
        /**
         * List of mailboxes currently being observed
         */
        RArray<TFSMailMsgId> iObservedMailboxes;
        
        /**
        * Fs Email framework client
        */
        CFSMailClient* iMailClient;

        /**
        * Class for postponing the mailbox creation in MRU list - Event may come beforet the mailbox exists
        */
        CDelayMailboxCreationHelper *iDelayMailboxCreationPtr;
        
        /**
        * This mailboxes should be handled by CDelayMailboxCreationHelper
        */
        RArray<TFSMailMsgId> iDelayedCreatedMailboxes;

        TFSMailMsgId  iPreviousMailboxId; 
        TFSMailMsgId  iPreviousParentFolderId; // not owned
        /* folder type for a mailbox referenced by iPreviousMailbox and iPreviousParentFolderId */
        TFSFolderType iPreviousParentFolderType; 
    }; // class CPsMruAdapter

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


// Helper class for thread synchronization 
// used when mailbox is not ready when TFSEventNewMailbox event comes 
// Waits 30*0.5 sec to try posponed mailbox registration

// wait 500 ms to try adding the mailbox - because sync problems 
#define KDelayToRunAddMailbox 500 
// try it 15 second - 30 times
#define KNumberOfDelayedTrials 30


class CDelayMailboxCreationHelper : public CTimer
    {
    public:
        static CDelayMailboxCreationHelper* NewLC( CPsMruAdapter *aPsMruAdapterPtr );
        static CDelayMailboxCreationHelper* NewL( CPsMruAdapter *aPsMruAdapterPtr );
        // d-tor
        virtual ~CDelayMailboxCreationHelper();
        // Start timer to call CPsMruAdapter::DeleayedMailboxCreationEventL by RunL 
        void StartDelayedCall();    
    protected:
        // on timer event - calls CPsMruAdapter::DeleayedMailboxCreationEventL
        virtual void RunL();
        // when leave from RunL, if err handled return KErrNone
        TInt RunError( TInt aError );
        // two phase constr.
        void ConstructL();
    private:
        CDelayMailboxCreationHelper( CPsMruAdapter *aPsMruAdapterPtr ); // EPriorityLow, EPriorityIdle
        // callback not owning pointer assigned during construction
        CPsMruAdapter *iPsMruAdapterPtr;
        // unsuccessful calls still allowed counter
        TInt iNumberOfDelayedTrials;
    }; // class CDelayMailboxCreationHelper

#endif // C_PS_MRU_ADAPTER_H
