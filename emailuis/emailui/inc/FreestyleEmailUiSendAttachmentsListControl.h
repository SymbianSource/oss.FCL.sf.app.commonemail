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
* Description:  FreestyleEmailUi attachments list control class
*
*/



#ifndef FREESTYLEMAILUI_SENDATTACHMENTSLISTCONTROL_H_
#define FREESTYLEMAILUI_SENDATTACHMENTSLISTCONTROL_H_

#include <MsgAttachmentUtils.h>

// LOCAL INCLUDES
#include "FreestyleEmailUiAttachmentsListModel.h"
#include "FreestyleEmailUiUtilities.h"
#include "fsccontactactionmenudefines.h"

// FORWARD DECLARATIONS
class CFSEmailUiSendAttachmentsListVisualiser;
class CFSEmailUiSendAttachmentsListModel;
class CFreestyleEmailUiAppUi;
class CGulIcon;
class CFscContactActionService;
class CFscContactActionMenu;
class CFSEmailUiSendAttachmentsListModelItem;
class CAknWaitNote;

/**
* CFsGenericListAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
// <cmail>
class CFreestyleEmailUiSendAttachmentsListControl 
	: public CAlfControl, public MFsActionMenuPositionGiver, public MMGFetchVerifier, public MFSEmailUiGenericTimerCallback
    {  
// </cmail>
public:
	
    /**
    * Two-phased constructor.
    * 
    * @param aEnv Reference to the ALF environment.
    */
    static CFreestyleEmailUiSendAttachmentsListControl*	NewL( 
        CAlfEnv& aEnv,
        CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
        CFreestyleEmailUiAppUi* aAppUi );
	    
    /**
    * Two-phased constructor.
    * 
    * @param aEnv Reference to the ALF environment.
    */
    static CFreestyleEmailUiSendAttachmentsListControl* NewLC(
        CAlfEnv& aEnv,
        CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
        CFreestyleEmailUiAppUi* aAppUi );

	    
    /**
    * Virtual destructor.
    */
    virtual ~CFreestyleEmailUiSendAttachmentsListControl();
    
    // <cmail>
public: // from MMGFetchVerifier
    TBool VerifySelectionL( const MDesCArray* aSelectedFiles );
    // </cmail>

public: // new methods

    /**
    * OfferEventL
    * From ALF
    */
    TBool OfferEventL( const TAlfEvent& aEvent );  

    TBool IsAttachmentAddingLocked() const;
    
	// Fire timer callback
	void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer );
    
    /**
    * AppendAttachmentToListL
    * Adds new attachment to list and email body
    * @param aType Type of attachment to add
    * 
    * @return   ETrue if attachment was added.
    *           EFalse if user cancelled or operation failed.
    */	
    TBool AppendAttachmentToListL(MsgAttachmentUtils::TMsgAttachmentFetchType aType = MsgAttachmentUtils::EUnknown);

    /**
    * AppendFileToModelL
    * Adds new file to list model
    */
    void AppendFileToModelL( 
        const TFSMailMsgId aPartId, 
        const TDesC& aFileName,
        TInt aSize,
        TFileType aFileType = EUnidentifiedType,
        TBool aReadOnly = EFalse,
        TBool aRemoteFile = EFalse );
	
    /**
    * TotalAttachmentSize
    * Return total size of a attachments in the model
    */
    TInt TotalAttachmentSize() const;
		
    /**
    * RemoveAttachmentFromListL
    * removes item by given index
    * or
    * removes currently highlighted item from model
    */
    void RemoveAttachmentFromListL( const TInt aIndex = -1 );

    /**
    * RemoveAllAttachmentsL
    * Removes all items from model
    */
    void RemoveAllAttachmentsL();
	
    /**
    * OpenHighlightedFileL
    */
    void OpenHighlightedFileL();
		
    /**
    * Model
    * Retuns pointer to list model
    */
    CFSEmailUiSendAttachmentsListModel* Model();

    /**
    * DeleteModel
    * Deletes model, used from visualiser because of alf dest sequence
    */   
    void DeleteModel();

    //<cmail> touch
    /**
     * Helper class for setting msk up to date.  
     */   
    void SetMskL();

    /**
    * ShowActionMenu
    * Displays popup menu when user hits right with joystick
    * and handles selected command
    */      
    void ShowActionMenuL( CFSEmailUiSendAttachmentsListModelItem* aItem );
    //</cmail>
    
    
public: // methods from base classes

    /**
    * From CCoeControl
    */
    void HandleCommandL( TInt aCommand );

// <cmail>
    /**
     * From MFsActionMenuPositionGiver
     */
    TPoint ActionMenuPosition();
// </cmail>
	    
private: // constructors
	
    /**
    * Constructor.
    * 
    * @param aEnv Reference to the ALF environment.
    * @param aVisualiser Reference to visualiser for command handling.
    */
    CFreestyleEmailUiSendAttachmentsListControl(
        CFSEmailUiSendAttachmentsListVisualiser* aVisualiser,
        CFreestyleEmailUiAppUi* aAppUi );
	    
    /**
    * Two-phased constructor.
    */
    void ConstructL(CAlfEnv& aEnv);

private: // methods used internally
    
    /**
    * LoadModelContentL
    * Loads draft email attachments to model
    */
    void LoadModelContentL();

    //<cmail> touch
    /**
    * ShowActionMenu
    * Displays popup menu when user hits right with joystick
    * and handles selected command
    */		
    //void ShowActionMenuL( CFSEmailUiSendAttachmentsListModelItem* aItem );
    //</cmail> touch
		
    /**
    * FileDrmProtectedL
    */	
    TBool FileDrmProtectedL( RFile& aFile );
	
    /**
    * FileDrmProtectedL
    */	
    TBool FileDrmProtectedL( const TDesC& aFilePath );
			    
private: // data

    // Visualizer. Not owned.
    CFSEmailUiSendAttachmentsListVisualiser*	iVisualiser;
    
    // Model. Owned.
    CFSEmailUiSendAttachmentsListModel* 		iModel;
    
    // AppUi. Not owned.
    CFreestyleEmailUiAppUi* 					iAppUi;
    
    // Contact action service. Owned.
    CFscContactActionService* 					iService;

    // Wait note for long running operations. Owns itself.
    CAknWaitDialog*                             iWaitNote;
    
    // adding attachment locked
    TBool iAttachmentAddingLocked;
    
    // Timer to postpone the Info Dialog 
    CFSEmailUiGenericTimer* iDialogTimer;
    };

#endif /*FREESTYLEMAILUI_SENDATTACHMENTSLISTCONTROL_H_*/
