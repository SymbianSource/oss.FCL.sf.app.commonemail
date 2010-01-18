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
* Description:  Mailbox deleter.
*
*/

#ifndef FREESTYLEEMAILUIMAILBOXDELETER_H
#define FREESTYLEEMAILUIMAILBOXDELETER_H

// SYSTEM INCLUDE FILES
#include <e32base.h>
#include <AknWaitDialog.h>

// INTERNAL INCLUDE FILES
#include "MFSMailRequestObserver.h"

// FORWARD DECLARATIONS
class CFSMailClient;
class TFSMailMsgId;
class CFSMailBox;

class MFSEmailUiMailboxDeleteObserver
    {
public:
    
    /**
     * Called when selected mailboxes have been deleted.
     */
    virtual void MailboxDeletionComplete() = 0;

protected:
    
    MFSEmailUiMailboxDeleteObserver() {};
    
    ~MFSEmailUiMailboxDeleteObserver() {};
    };


// Mailbox deleter
class CFSEmailUiMailboxDeleter : public CBase, 
                                 public MProgressDialogCallback, 
                                 public MFSMailRequestObserver
    {
public:
    
    /**
     * Constructs a CFSEmailUiMailboxDeleter instance and returns a pointer to 
     * it.
     * @param aMailClient Reference to a valid CFSMailClient.
     * @param aObserver Observer for receiving notification when mailbox 
     * deletion has been completed.
     * @return CFSEmailUiMailboxDeleter Pointer to class instance.
     */
    static CFSEmailUiMailboxDeleter* NewL( CFSMailClient& aMailClient, 
                                           MFSEmailUiMailboxDeleteObserver& aObserver );

    /**
     * Constructs a CFSEmailUiMailboxDeleter instance and returns a pointer to 
     * it.
     * @param aMailClient Reference to a valid CFSMailClient.
     * @param aObserver Observer for receiving notification when mailbox 
     * deletion has been completed.
     * @return CFSEmailUiMailboxDeleter Pointer to class instance. 
     */
    static CFSEmailUiMailboxDeleter* NewLC( CFSMailClient& aMailClient, 
                                            MFSEmailUiMailboxDeleteObserver& aObserver );

    /**
     * Destructor.
     */
    ~CFSEmailUiMailboxDeleter();
    
public:    
    
	/**
     * Displays a list of available mailboxes and allows the user to mark 
     * one or more mailboxes for deletion. Deletes the selected mailboxes and
     * notifies the observer given in the construction when the deletion is
     * complete.
     */
    void DeleteMailboxL();

    /**
     * Deletes the given mailbox. Displays also a confirmation dialog.
     * @param aMailboxId The ID of the mail box to delete.
     */
    void DeleteMailboxL( const TFSMailMsgId& aMailboxId );


protected: // from MProgressDialogCallback

    void DialogDismissedL( TInt aButtonId );

protected: // from MFSMailRequestObserver

    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    
protected:
    
    void ConstructL();
    
    CFSEmailUiMailboxDeleter( CFSMailClient& aMailClient, 
                              MFSEmailUiMailboxDeleteObserver& aObserver  );

private:

	/**
	 * Returns the mailboxes that can be deleted. Note that the given array is
	 * left in the clean up stack.
	 * @param aMailboxes The array where the mailboxes are added to.
	 * @return The number of deletable mailboxes found.
	 */
	TInt GetDeletableMailboxesLC( RPointerArray<CFSMailBox>& aMailboxes );

    /**
     * Deletes a single mailbox.
     * @param aMailboxes Array containing the mailbox to be deleted.
     * @return ETrue if user confirmed the deletion, EFalse if not.
     */
    TBool DeleteSingleMailboxL( const RPointerArray<CFSMailBox>& aMailboxes );
    
    /**
     * Deletes multiple mailboxes.
     * @param aMailboxes Array containing the mailboxes to be deleted.
     * @return ETrue if user confirmed the deletion, EFalse if not.
     */
    TBool DeleteMultipleMailboxesL( const RPointerArray<CFSMailBox>& aMailboxes );
    
    /**
     * Deletes the selected mailboxes i.e. those of which IDs are in the
     * iMailboxesToDelete array.
     */
    void DoDeleteSelectedMailboxesL();
    
    /**
     *  Confirms mailbox deletion. Behavior depends on the number of mailboxes
     *  to be deleted. aMailboxName is not used if aCount > 1.
     *  @param aCount Number of mailboxes to be removed.
     *  @param aMailboxName Name of the (first) mailbox to delete.
     *  @return ETrue if user confirmed the deletion, EFalse if not.
     */
    TBool ConfirmMailboxDeletionL( TInt aCount, const TDesC& aMailboxName );

    /**
     * Creates the mark icon.
     * @return The listbox mark icon.
     */
    CGulIcon* CreateMarkIconLC();
    
    /**
     * Creates the default mailbox icon for unbranded mailboxes.
     * @return The default mailbox icon.
     */
    CGulIcon* CreateDefaultMailboxIconLC();

    /**
     * Creates the listbox model and icon array for the listbox.
     * @param aModel Listbox model.
     * @param aIconArray Icon array.
     * @param aMailboxes Mailboxes to be added to the listbox model.
     */
    void CreateListboxModelAndIconArrayL( CDesCArrayFlat& aModel,
                                          CArrayPtr<CGulIcon>& aIconArray, 
                                          const RPointerArray<CFSMailBox>& aMailboxes );

    /**
     * Deletes next mailbox in iMailboxesToDelete.
     */
    void DoDeleteNextMailboxL();
    
    /**
     * Callback that calls DoDeleteNextMailboxL().
     */
    static TInt IdleCallbackL(TAny* aPtr);
                                          
private: // data

    // Mail client
    CFSMailClient& iMailClient;
    
    // Observer
    MFSEmailUiMailboxDeleteObserver& iObserver; 
    
    // Mailboxes to delete
    RArray<TFSMailMsgId> iMailboxesToDelete;
   
    // Operation id of the currently ongoing delete operation
    TInt iMailboxDeleteOperationId;

    // Wait dialog
    CAknWaitDialog* iWaitDialog;
    
    // Idletimer. This is used in RequestResponseL() to ensure that
    // iMailboxDeleteOperationId gets updated properly when deleting multiple
    // mailboxes.
    CIdle* iIdle;
        
    };

#endif // FREESTYLEEMAILUIMAILBOXDELETER_H
