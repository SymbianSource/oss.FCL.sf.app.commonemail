/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclServiceUiContext
*
*/

#ifndef CPBKXRCLSERVICEUICONTEXTIMPL_H
#define CPBKXRCLSERVICEUICONTEXTIMPL_H

#include "cpbkxremotecontactlookupserviceuicontext.h"
#include "tpbkxremotecontactlookupprotocolaccountid.h"
#include "mpbkxremotecontactlookupprotocolsessionobserver.h"
#include "mpbkxrclcontactretrieval.h"
#include "cpbkxrcleventscheduler.h"

#include <AknWaitDialog.h>

class CPbkxRclSearchResultDlg;
class CPbkxRclResultInfoDlg;
class CPbkxRemoteContactLookupProtocolSession;
class CCoeEnv;
class CPbkContactEngine;
class CPbkxRemoteContactLookupProtocolAdapter;
class CPbkxRclProtocolEnvImpl;
class CPbkxRemoteContactLookupProtocolAccount;
class CPbkxRclActionServiceWrapper;

/**
* Implementation class of service ui context.
*
* This class implements the RCL search logic and UI flow.
*
* @lib pbkxrclengine.lib
* @since S60 3.1
*/

class CPbkxRclServiceUiContextImpl : 
    public CPbkxRemoteContactLookupServiceUiContext,
    public MPbkxRemoteContactLookupProtocolSessionObserver,
    public MPbkxRclContactRetrieval,
    public MPbkxRclEventHandler,
    public MEikCommandObserver,
    public MProgressDialogCallback
    {

public: // constructors and destructor

  /**
    * Constructs new object.
    *
    * @param aId Acccount id.
    * @param aMode Mode in which context operates.
    * @return Created object.
    */

    static CPbkxRclServiceUiContextImpl* NewL(
        TPbkxRemoteContactLookupProtocolAccountId aId,
        TMode aMode );

  /**
    * Constructs new object.
    *
    * Constructed object is leaved in cleanup stack.
    *
    * @param aId Acccount id.
    * @param aMode Mode in which context operates.
    * @return Created object.
    */
    static CPbkxRclServiceUiContextImpl* NewLC(
        TPbkxRemoteContactLookupProtocolAccountId aId,
        TMode aMode );
 

  /**
    * Destructor.
    */
    virtual ~CPbkxRclServiceUiContextImpl();


public: // methods from CPbkxRemoteContactLookupServiceUiContext

   /**
     * Execute UI flow.
     *
     * @param aQueryText loose query text     
     * @param aResult remote lookup results
     */
    virtual void ExecuteL( const TDesC& aQueryText, TResult& aResult );


public: // from MPbkxRemoteContactLookupProtocolSessionObserver

    /**
     * Notifies that the loose search has been completed.
     * 
     * @param aStatus error code that tells how search went
     * @param aResults search results, ownership is moved to the caller of the
     * loose search.
     */

    virtual void LooseSearchCompleted( 
        TInt aStatus, 
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults );
  
    /**
     * Notifies that the contact fields retrieval has been completed.
     * 
     * @param aStatus error code that tells how retrieval went
     */

    virtual void ContactFieldsRetrievalCompleted( TInt aStatus );

public: // from MPbkxRclContactRetrieval

    /**
    * Retrieves details of the contact with given index.
    *
    * @param aContactIndex Index of the contact for which details are retrieved.
    * @param aWaitNoteText Text shown in wait note.
    * @return ContactCard with details retrieved or NULL. NULL is returned
    *         when either contact retrieval failed or user cancelled
    *         contact retrieval.
    */

    virtual CContactCard* RetrieveDetailsL( 
        TInt aContactIndex, 
        const TDesC& aWaitNoteText );

    /**
    * Returns the number of contacts.
    *
    * @return The number of contacts.
    */

    virtual TInt ContactCount() const;

    /**
    * Sets the index of currently selected contact.
    *
    * @param aIndex Index of the currently selected contact.
    */

    virtual void SetSelectedContactL( TInt aIndex );

public: // from MPbkxRclEventHandler

    /**
    * Callback method from event scheduler.
    *
    * Operation is executed each time this method is called.
    */

    virtual void EventTriggered();

public: // methods from MEikCommandObserver

    /**
    * Processes command from search result view or result information view.
    *
    * @param aCommand Command id.
    */

    virtual void ProcessCommandL( TInt aCommandId );

public: // from MProgressDialogCallback

    /**
    * This method is called when wait note closed by either user pressing
    * cancel or dialog closed by code.
    */
    virtual void DialogDismissedL( TInt aButtonId );

private: // enumerations used internally


    // Possible operations for context.

    enum TOperation
        {
        ENoOperation,              // No operation
        EOpenSearchResultDlg,      // Opens search result dialog
        EOpenResultInfoDlg,        // Opens result info dialog
        ECloseSearchResultDlg,     // Closes search result dialog
        ECloseResultInfoDlg,       // Closes result info dialog
        EOpenSearchQueryDefault,   // Opens default search query
        EOpenSearchQueryEmpty,     // Opens empty search query
        EExecuteSearchWithNoQuery, // Executes search without prompting query
        EExit                      // Stop context execution
        };
  
    // Possible state values for context.

    enum TState
        {
        EInitial = 0x1,          // Initial state
        EResultDlgOnTop = 0x2,   // Result dialog topmost
        EInfoDlgOnTop = 0x4,     // Info dialog topmost
        EResultSelected = 0x8,   // Result is selected
        EReturnToCaller = 0x10   // Return to the calling application    
        };


    // Wait object indexes.

    enum TWaitObjectIndex
        {
        EMainWait = 0,              // Main wait loop.
        EContactRetrievalWait = 1   // Contact retrieval wait loop
        };

private: // methods used internally

    /**
    * Executes search.
    *
    * @param aShowQueryDialog Flag indicating whether search query dialog is 
    *        shown.
    * @param aQueryText Default value for query text.
    */

    void ExecuteSearchL( TBool aShowQueryDialog, const TDesC& aQueryText );

    /**
    * Displays wait dialog.
    *
    * @param aDialogResourceId Dialog resource id.
    * @param aText Text shown in wait dialog.
    */

    void DisplayWaitDialogL( TInt aDialogResourceId, const TDesC& aText );

    /**
    * Displays note dialog.
    *
    * @param aDialogResourceId Dialog resource id.
    * @param aText Text shown in dialog.
    * @param aTimeout Flag indicating whether note has timeout.
    * @return Possible return value from note.
    */

    TBool DisplayNoteDialogL( 
        TInt aDialogResourceId, 
        const TDesC& aText,
        TBool aTimeout = EFalse );

    /**
    * Displays query dialog.
    *
    * @param aDialogResourceId Dialog resource id.
    * @param aText Text shown in dialog.
    * @return Possible return value from note.
    */
    //Fix for: EASV-7KFGG3
    TBool DisplayQueryDialogL(        
        TInt aDialogResourceId, 
        const TDesC& aText );

    /**
    * Closes wait dialog.
    */

    void CloseWaitDialogL();

    /**
    * Displays search result dialog.
    */

    void DisplaySearchResultDialogL();

    /**
    * Displays result info dialog.
    */

    void DisplayResultInfoDialogL();


    /**
    * Creates contact card array.
    *
    * Contact items are casted to contact cards.
    */

    void CreateContactCardArray();

    /**
    * Does actions after loose search has been completed.
    * 
    * @param aStatus Status returned from adapter.
    * @param aResults Search results.
    */

    void DoLooseSearchCompletedL(
        TInt aStatus,
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults );


    /**
    * Does actions after contact field retrieval has been completed.
    *
    * @param aStatus Status received from adapter.
    */

    void DoContactFieldsRetrievalCompletedL( TInt aStatus );


    /**
    * Executes operation.
    */

    void DoHandleOperationL();

  

    /**
    * Handles actions after search result dialog is closed based
    * on state of the context.
    */

    void HandleSearchResultDialogExitL();


    /**
    * This method is called when we are about to exit the context.
    *
    * Exit reason is set and possible error note is shown.
    */

    void HandleContextExitL();


    /**
    * Creates new contact item of the selected contact item.
    *
    * Ownership is transferred.
    *
    * @return Selected contact item.
    */

    CContactItem* GetSelectedContactL();


    /**
    * Searches for a result which corresponding contact item has given index.
    *
    * This method is needed because contact items are sorted and they are
    * in different order as results. So we must find a result which contains
    * the contact item with given index.
    *
    * Ownership is not transferred.
    *
    * @param aIndex Index of the contact item which result is searched.
    * @return Protocol result.
    */

    CPbkxRemoteContactLookupProtocolResult* GetResultByIndex( TInt aIndex );

    /**
    * Starts active wait.
    *
    * Execution is halted until corresponding stop is called.
    *
    * @param aIndex Index of the wait object to be started.
    */

    void StartActiveWaitL( TWaitObjectIndex aIndex );
    

    /**
    * Stops active wait with given index.
    *
    * @param aIndex Index of the wait object to be stopped.
    */

    void StopActiveWait( TWaitObjectIndex aIndex );


    /**
    * Changes the state of the context.
    *
    * @param aState The state in which context is set.
    */

    void SetState( TState aState );
  

    /**
    * Unsets state.
    *
    * @param aState State to be unset.
    */

    void UnsetState( TState aState );


    /**
    * Returns ETrue if given state is set.
    *
    * @param aState State which status is checked.
    * @return ETrue if context is in given state, EFalse otherwise.
    */

    TBool IsStateSet( TState aState );


    /**
    * Resets all state flags of context.
    */

    void ResetState();

    /**
    * Adds new operation to be executed.
    *
    * @param aOperation Operation to be added.
    */

    void AddOperation( TOperation aOperation );


    /**
    * Handles fatal error based on current state.
    *
    * @param aError Occurred error.
    */

    void HandleError( TInt aError );


    /**
    * Sorts contacts based on last name.
    */

    static TInt Sort( const CContactCard& aFirst, const CContactCard& aSecond );
    
	/**
	* Static callback for CPeriodic timer
	*/
    static TInt TimerCallBack(TAny* aAny);
    
    /**
	* This method is called in TimerCallBack() when time is out
	*/
    void TimeOut();

private: // constructors

    /**
    * Constructors.
    *
    * @param aId Protocol account id.
    * @param aMode Mode.
    */

    CPbkxRclServiceUiContextImpl( 
        TPbkxRemoteContactLookupProtocolAccountId aId,
        TMode aMode );

    /**
    * Second-phase constructor.
    */

    void ConstructL();

private: // data structure used internally

    class TOperationQueue
        {
    public: // constructor

        /**
        * Constructor.
        */

        TOperationQueue();

    public: // new methods

        /**
        * Adds new operation to queue.
        *
        * @param aOperation Operation to be added.
        */

        void Add( TOperation aOperation );

        /**
        * Pops operation to be executed from the queue.
        *
        * @return Operation to be executed.
        */

        TOperation Pop();


        /**
        * Returns number of the operations in the queue.
        */

        TInt Count() const;

    private: // data
        
        // Maximum number of operations.
        static const TInt KMaxOperations = 10;

        // Current operation index.
        TInt iCurrent;

        // Count of operations.
        TInt iCount;

        // Fixed size operation array.
        TOperation iOperations[KMaxOperations];
        };


private: // data

    // Account id.
    TPbkxRemoteContactLookupProtocolAccountId iAccountId;

    // State in which context is in.
    TInt iState;

    // Resource file offset.
    TInt iResourceFileOffset;

    // Original query criteria given to context. Owned.
    RBuf iQueryCriteria;

    // Query text given to execute. Owned.
    RBuf iQueryText;

    // Search mode.
    TMode iMode;

    // Selected contact index.
    TInt iSelectedIndex;

    // For waiting asynchronous operations to finish. Owned.
    RPointerArray<CActiveSchedulerWait> iWaitObjects;

    // Wait dialog. Owned.
    CAknWaitDialog* iWaitDialog;

    // Search result dialog. Owned.
    CPbkxRclSearchResultDlg* iSearchResultDialog;

    // Result information dialog. Owned.
    CPbkxRclResultInfoDlg* iResultInfoDialog;

    // Remote contact query results. Owned.
    RPointerArray<CPbkxRemoteContactLookupProtocolResult> iSearchResults;

    // Contact cards from the results. Not owned.
    RPointerArray<CContactCard> iContactCards;

    // Adapter used in searches. Owned.
    CPbkxRemoteContactLookupProtocolAdapter* iAdapter;

    // Session used to execute remote searches. Owned.
    CPbkxRemoteContactLookupProtocolSession* iSession;

    // Protocol environment given to adapter. Owned.
    CPbkxRclProtocolEnvImpl* iEnvImpl;

    // Account which is used. Owned.
    CPbkxRemoteContactLookupProtocolAccount* iAccount;

    // Search result. Not owned.
    TResult* iResult;

    // Contact engine used to help create phone book contact items. Owned.
    CPbkContactEngine* iContactEngine;

    // Action service wrapper used by search result dialog and
    // result information dialog. Owned.
    CPbkxRclActionServiceWrapper* iActionServiceWrapper;

    // For generating timed events. Owned.
    CPbkxRclEventScheduler* iEventScheduler;

    // Operation queue.
    TOperationQueue iOperations;

    // Context exit code. If something goes wrong, this code is set.
    TInt iExitCode;

    // Timer for generating time out event 
    CPeriodic* iTimer;
    };

#endif
