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
* Description: Definition of Email message search API.
*
*/

#ifndef EMAILMESSAGESEARCHASYNCIMPL_H_
#define EMAILMESSAGESEARCHASYNCIMPL_H_

#include <memailmessage.h>
#include <memailmessagesearch.h>
#include <emailsorting.h>

#include "CFSMailClient.h"
#include "MFSMailBoxSearchObserver.h"
#include "emailapiutils.h"

using namespace EmailInterface;


NONSHARABLE_CLASS( CEmailMessageSearchAsync) : 
    public CBase, 
    public MEmailMessageSearchAsync, 
    public MFSMailBoxSearchObserver

{
public:
    /**
    * Constructor
    * @return
    */
    static CEmailMessageSearchAsync* NewL(
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );
    
    /**
     * Destructor
     */
    ~CEmailMessageSearchAsync();
    
public: // from MEmailInterface
    virtual TEmailTypeId InterfaceId() const;
    
    virtual void Release();
    

public: // from MEmailMessageSearchAsync
    /**
    * Sets sort order for search results.
    * Leaves KErrNotReady if search is ongoing.
    */
    virtual void SetSortCriteriaL( const TEmailSortCriteria& aCriteria );
    
    /**
    * Adds a search key. Leaves KErrNotReady if search is ongoing.
    */
    virtual void AddSearchKeyL( const TDesC& aSearchKey );
                                                                  
    /**
    * Enables/disables search from remote email server.
    * Leaves KErrNotReady if search is ongoing.
    */
    virtual void SetRemoteSearchL( TBool aRemote );                                                               
    
    /**
    * Indicates whether remote search is enabled.
    */
    virtual TBool IsRemoteSearch() const;
    
    /**
     * Starts search, all methods affecting search attribures leave
     * KErrNotReady while search is ongoing.
     * @param aObserver called when results are available.
     */     
    virtual void StartSearchL( MEmailSearchObserver& aObserver );

    /**
     * Cancels search.
     */
    virtual void Cancel();
                                             
    /** returns search status 
      * @return search status:
      *     < 0 : Search has failed
      *     KRequestPending : search is ongoing. note that status may be
      *         KRequestPending after HandleResultL callback because results 
      *         may be given in chunks of results. Size of chunk depends on
      *         implementation and may vary.
      *     KErrNone : initial state, or search has finished
      */
    virtual TInt Status() const;
    
    /**
     * Resets all search attribures. Cancels search if ongoing. 
     */
    virtual void Reset();
    
public:  // From MFSMailBoxSearchObserver
    /** 
     * Notifies the email search API client that a match has been found
     * 
     * @param aMatchMessage contains a pointer to the matched message.
     *         Ownership is transfered to the observer.
     *
     */
     virtual void MatchFoundL( CFSMailMessage* aMatchMessage );

    /**
     * Notifies the email search API client that the search has completed
     *
     */
     virtual void SearchCompletedL();
    
private:
    /**
     * Constructor
     */
    CEmailMessageSearchAsync(
        CPluginData& aPluginData,
        const TMailboxId& aMailboxId );

    void ConstructL();
    
private:
    
    CPluginData&    iPluginData;
    
    CFSMailPlugin*  iPlugin;

    TMailboxId      iMailboxId;
    
    TFSMailSortCriteria iCriteria;
        
    RPointerArray<TDesC> iSearchStrings;    

    MEmailSearchObserver* iObserver;
    
    mutable RSemaphore iGate;
    TBool iRemote;
};

#endif /* EMAILMESSAGESEARCHASYNCIMPL_H_ */
