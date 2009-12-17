/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Handles interaction with setup wizard
*
*/


#ifndef CFSEMAILUIWIZARDOBSERVER_H
#define CFSEMAILUIWIZARDOBSERVER_H

//  INCLUDES
#include <AknWaitNoteWrapper.h>
//<cmail>
#include "CFSMailCommon.h"
//</cmail>


// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CFSEmailUiLauncherGridVisualiser;

/**
 * Freestyle email wizard observer
 * Handles wizard lauching and the events received from one.
 *
 */
class CFSEmailUiWizardObserver : public CBase,
                                 public MAknBackgroundProcess
	{

public:
  
    /**
     * Destructor.
     *
     */  
  	 ~CFSEmailUiWizardObserver();

    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
     * @returns CFSMailPluginManager instance
     */
  	 static CFSEmailUiWizardObserver* NewL( 
  	    CFreestyleEmailUiAppUi* aAppUi,
        CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser );
  	
    /**
     * Two-phased constructor.
     * Creates a new instance of class
     *
     * @returns CFSMailPluginManager instance
     */
  	 static CFSEmailUiWizardObserver* NewLC( 
  	    CFreestyleEmailUiAppUi* aAppUi,
        CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser );
    
    /**
     * Called from CFreestyleEmailUiAppUi::ConstructL.
     * Launches wizard if we don't have any mailboxes configured yet when 
     * Email Ui is started, 
     * OR
     * if there were parameters set by the wizard in central repository,
     * a wait note is started, and - via StepL - the framework is notified 
     * about the parameters by calling its method WizardDataAvailableL.
     *
     */
     void DoWizardStartupActionsL();
	 
	/**
     * When FS EMail is running and a setup wizard is finished, 
     * it will send the UI a message with a KNullDesC as message content.
     * UI will recognize this, start wait note and - via StepL - it will call 
     * the framework's method WizardDataAvailableL.
     * FS EMail will be brought to foreground if it is not already.
     */
	 void HandleMessage( TUint32 aClientHandleOfTargetWindowGroup,
                         TUid aMessageUid,
                         const TDesC8& aMessageParameters );
	 
	/**
     * If FS Email was brought to foreground in HandleMessage, 
     * a wait note is started and - via StepL - framework's WizardDataAvailableL 
     * will be called.
     */
	 void HandleForegroundEventL( TBool aForeground );
	 
public: //from MFSMailEventObserver
    /**
     * Framework event message. 
     * Used here to catch the events thrown from new mailbox creation process.
     * In the wizard observer only the events TFSEventNewMailbox and
     * TFSEventMailboxCreationFailure are catched. When either of these two 
     * is received the wait note is dismissed.
     *
     * @param aEvent One of pre-defined events in TFSMailEvent
     * @param aMailbox maibox related for this event
     * @param aParam1 intepretation is event specific (see TFSMailEvent descriptions)
     * @param aParam2 intepretation is event specific
     * @param aParam3 intepretation is event specific
     */
    void EventL( TFSMailEvent aEvent, TFSMailMsgId aMailbox, 
                 TAny* aParam1, TAny* aParam2, TAny* aParam3);
	 
public: // from MAknBackgroundProcess
    
    /**
     * The execution accesses this function when the "Creating new mailbox" wait note
     * is executed. From this method, framework's WizardDataAvailableL will be called.
     * This is done only once, ensured by the flag iInformAboutParams.
     */
    void StepL();
	
	/**
     * @return ETrue if the displayed wait note should be dismissed.
     */
	TBool IsProcessDone() const;
	
	void ProcessFinished();
	
	void DialogDismissedL( TInt /*aButtonId*/ ); 
	
	/**
     * @return Returns KErrNone because the method StepL does not leave.
     */
	TInt CycleError( TInt aError );
	 
protected:

    /**
     * C++ default constructor.
     *
     */
    CFSEmailUiWizardObserver( CFreestyleEmailUiAppUi* aAppUi,
                              CFSEmailUiLauncherGridVisualiser* aMainUiGridVisualiser );
    
    /**
     * ConstructL
     *
     */
	void ConstructL();

private:
    
    /**
     * Helper method which tells framework (with WizardDataAvailableL) that 
     * there are parameters in the central repository with which framework 
     * can create a new mail account.
     * After calling FW, clears the centrep keys.
     * If WizardDataAvailableL leaved or returned an error other than KErrNone, 
     * an error message is displayed.
     */
    void InformFrameworkAboutParamsL();
    
    /**
     * Helper method that launches the "Creating new mailbox" wait note. 
     * After calling the wait note's ExecuteL, StepL is called and this way
     * framework's WizardDataAvailableL gets called.
     */
    void StartWaitNoteL();

private: // Data
    
    CFreestyleEmailUiAppUi* iAppUi;                             //does not own
    CFSEmailUiLauncherGridVisualiser* iMainUiGridVisualiser;    //does not own
    
    // For displaying the "Creating new mailbox" wait note
    CAknWaitNoteWrapper* iWaitNote;
    
    // Tells if the wait note is to be dismissed.
    TBool iProcessComplete;
    
    // WizardDataAvailableL is called (new mailbox possibly created) only after we have 
	// received a certain kind of message in HandleMessageL() from mailbox setup wizard.
	// This boolean is used to notify that that kind of message has been received and 
	// that FS EMail was brought to foreground after that.
    TBool iForegroundAfterWizardMessage;
    
	// this makes sure that the code in StepL() in run only once (in one process)
	TBool iInformAboutParams;
    
    // For some reason the framework is sending the UI multiple TFSEventNewMailbox events
    // which causes crash in FW when the Inbox of the new mailbox is accessed 
    // for the second time.
    // Use of this can be removed when UI gets only one TFSEventNewMailbox for sure.
    TBool iNewMailboxEventHandlingAllowed;
    
    // <cmail>
    // Id of the created mailbox
    TFSMailMsgId iNewlyCreatedMailboxId;
    // flag informing that new mailbox was created
    TBool iNewMailboxCreated;
    // </cmail>
    };
    

#endif  // CFSEMAILUIWIZARDOBSERVER_H

// End of File
