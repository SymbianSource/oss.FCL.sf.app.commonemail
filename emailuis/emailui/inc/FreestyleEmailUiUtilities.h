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
* Description:  General utilities for FS Email UI
*
*/


#ifndef __FREESTYLEMAILUI_UTILITIES_H_
#define __FREESTYLEMAILUI_UTILITIES_H_

#include <e32base.h>	// CTimer
#include <e32cmn.h>
#include <hlplch.h>
#include <msvstd.h>

#include <AknQueryDialog.h>

//<cmail>
#include "mesmricalviewercallback.h"
#include "fsccontactactionmenudefines.h"
//</cmail>
#include <DocumentHandler.h>
//<cmail>
#include "AknServerApp.h"
//</cmail>

#include "FreestyleEmailExportedUtilities.h"
#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiTextureManager.h"
#include "FreestyleEmailUiContactHandler.h"


class CGulIcon;
class CFSMailBox;
class CFSMailMessage;
class CFSMailClient;
class CMsvSession;
class TPbkContactItemField;
class CPbkContactItem;
class CFscContactActionMenu;
class CFscContactActionService;
class CAlfTexture;
class CFreestyleEmailUiTextureManager;
class CFSMailMessagePart;
class CFSMailAddress;
class CFSMailFolder;
class MAknServerAppExitObserver;
class CFSEmailUiGenericTimer;
class CAknGlobalNote;
class CAknWaitDialog;
// <cmail>
class CESMRIcalViewer;
// </cmail>

const TInt KActionMenuItemNormalPriority = 10;
const TInt KGenericTimerDefaultInterval = 1000;

typedef RArray<TActionMenuCustomItemId> RFsEActionMenuIdList;


enum TFollowUpNewState
	{
	EFollowUp = 0, // don't change, as these are the in the order they are in popup selection list
	EFollowUpComplete = 1, // don't change
	EFollowUpClear = 2, // don't change
	EFollowUpNoChanges = 3
	};

enum TMessageSizeClass
    {
    ETiny = KKilo,
    ESmall = 10*KKilo,
    EMedium = 100*KKilo,
    ELarge = 500*KKilo,
    EVeryLarge = KMega,
    EHuge = 5*KMega,
    EColossal = KMaxTInt
    };

class TFsEmailUiUtility
    {
public:
    // Clean up any static variables of the class
    static void DeleteStaticData();
    
    static void LaunchHelpL( const TDesC& aContext );
    static CFSMailBox* GetMceDefaultMailboxL( CFSMailClient& aClient, CMsvSession& aMsvSession );

    static CFSMailBox* GetMailboxForMtmIdL( CFSMailClient& aClient, CMsvSession& aMsvSession, TMsvId aMtmId );

    static TBool IsRemoteLookupSupported( CFSMailBox& aMailBox );
		
    static void CopyToClipboardL( const TDesC& aBuf );

    static TInt ShowConfirmationQueryL( TInt aResourceStringId, const TDesC& aResourceParameter = KNullDesC );
    static TInt ShowConfirmationQueryL( const TDesC& aPromptText );

    static void ShowErrorNoteL( TInt aResourceStringId, TBool aWaitingDialog = EFalse );
    static void ShowInfoNoteL( TInt aResourceStringId, TBool aWaitingDialog = EFalse );
	
    static void ShowGlobalErrorNoteL( TInt aResourceStringId );
    static void ShowGlobalInfoNoteL( TInt aResourceStringId );
    static void ShowDiscreetInfoNoteL( TInt aResourceStringId );
    static void ShowDiscreetInfoNoteL( const TDesC& aNoteText );    
     
    /**
    * Creates and shows a local wait note. Caller must pass a member variable pointer
    * which will hold the created wait note instance. This must NOT be an automatic variable.
    */
    static void ShowWaitNoteL( CAknWaitDialog*& aDialog, TInt aTextResourceId, 
            TBool aAllowCancel, TBool aVisibilityDelayOff = EFalse );
    
    /**
    * Launches a global wait note with no CBA buttons and text from given resource.
    * A cleanup item is pushed to CleanupStack and the wait note is dismissed
    * when the PopAndDestroy() is called for it.
    * 
    * @param   aTextResourceId Resource ID of the text string to be shown in the note
    * @return  ID of the opened wait note
    */
    static TInt ShowGlobalWaitNoteLC( TInt aTextResourceId );
    static void CloseGlobalWaitNote( TAny* aNoteIdPointer );
    
    /**
    * Shows file selection dialog.
    *
    * @param aFileName Contains the selected file name on return if user
    *                  selected a file.
    * @return ETrue if user selected file, EFalse otherwise.
    */
    static TBool ShowSelectFileDialogL( TDes& aFileName );

    static TBool ShowSaveFolderDialogL( TDes& aFileName );
 
    /** 
    * Displays overwrite query if needed
    * 
    * @param aFilePath Path to where the file is to be saved
    * @param aAttachmentPart Message part of the attachment to be saved
    * @return ETrue if OK to save the file to a given path, EFalse if not
    */	
	static TBool OkToSaveFileL( const TDesC& aFilePath, CFSMailMessagePart& aAttachmentPart );

	static void ShowFilesSavedToFolderNoteL( TInt aCount );
	
    static void ShowCreateMessageQueryL( const TDesC& aAddressData, TBool aSendingToEmail = EFalse );
    
    static TInt CountRecepients( CFSMailMessage* aMsgPtr );

    static TInt CountRecipientsSmart( CFreestyleEmailUiAppUi& aAppUi, CFSMailMessage* aMsgPtr );

    static TBool IsMessageBodyLargeL( CFSMailMessage* aMessage );
    
    /** 
     * Opens the given file for viewing using document handler.
     * 
     * @param aFileToOpen File to be opened for viewing. The file handle is
     *      closed after viewing.
     */
    static void OpenFileL( RFile& aFileToOpen, TBool aAllowSave  );

    /**
     * Opens the given file for viewing using document handler.
     *
     * @param aFileTope File to be opened for viewing. The file handle is
     *      closed after the viewing.
     * @param TDataType MIME type of the file.
     */
    static void OpenFileL( RFile& aFileToOpen, TDataType& aDataType, TBool aAllowSave );

    /**
     * Opens the given attachment for viewing.
     * 
     * @param aAttachmentPart The message part data that identifies the
     *      attachment to be opened.
     */
    static void OpenAttachmentL( const TPartData& aAttachmentPart );

    /**
    * Opens the given file for viewing using document handler.
    *
    * @param aFileTope File to be opened for viewing. The file handle is
    *      closed after the viewing.
    * @param TDataType MIME type of the file.
    * @param aAllowSave Flag to control availability of Save option in file viewer app.
    *                   When ETrue, the viewer app may show Save in options menu and/or when
    *                   closing the viewer.
    */
    static void OpenAttachmentL( CFSMailMessagePart& aAttachmentPart, TBool aAllowSave = ETrue );

    /**
     * SetMessageFollowupFlagL
     * 
     * @param aShowFlagCompleted Whether to show the completed flag.
     *                           When false we are in the editor view.
     */
  	static TFollowUpNewState SetMessageFollowupFlagL( CFSMailMessage& aMsg, TBool aShowFlagCompleted = ETrue ); 
    static void SetMessageFollowupStateL( CFSMailMessage& aMsg, TFollowUpNewState aNewFollowUpState ); 
    static TBool RunFollowUpListDialogL( TFollowUpNewState& aSelectedOption, TBool aShowFlagCompleted = ETrue );
	
	static TFileType GetFileType( const TDesC& aFileName, const TDesC& aMimeType );
	
	static CAlfTexture& GetAttachmentIcon( TFileType aAttachmentType, CFreestyleEmailUiTextureManager& aTextureManager );
	static TFSEmailUiTextures GetAttachmentIconTextureId( TFileType aAttachmentType );
	
    static CAlfTexture& GetMsgIcon( CFSMailMessage* aMsgPtr, CFreestyleEmailUiTextureManager& aTextureManager );
    static TFSEmailUiTextures GetMsgIconTextureId( CFSMailMessage* aMsgPtr );
		
    static void MoveMessageToDraftsL( CFSMailBox& aMailBox, CFSMailMessage& aMsg );
		
    static TBool IsMessagePartFullyFetched( const CFSMailMessagePart& aPart );

    static TBool IsMessageStructureKnown( const CFSMailMessagePart& aPart );
		
    static void CreatePlainTextPartL( CFSMailMessage& aMsg, CFSMailMessagePart*& aPart );
		
    static TBool IsCompleteOrCancelEvent( TFSProgress aEvent );
		
    static TBool HasUnfetchedAttachmentsL( CFSMailMessage& aMsg );
		
    static TBool IsFollowUpSupported( const CFSMailBox& aMailBox );
		
    // Set new email message icon on/off
    static void ToggleEmailIconL( TBool aIconOn );
	static void ToggleEmailIconL( TBool aIconOn, const TFSMailMsgId& aMailBox );
    
    static HBufC* DateTextFromMsgLC( const CFSMailMessage* aMessage,  TBool aAddYearNumer = ETrue );
    static HBufC* TimeTextFromMsgLC( const CFSMailMessage* aMessage );
    static HBufC* WeekDayTextFromMsgLC( const CFSMailMessage* aMessage, TBool aUseToday, TBool aUseYesterday,
                                        TBool& aWasToday, TBool& aWasYesterday );
    static HBufC* CreateSizeDescLC( TInt aSizeInBytes, 
                                    TBool aShowSizeInBytes = EFalse );
    static HBufC* ListMsgTimeTextFromMsgLC( const CFSMailMessage* aMessage, TBool aDividersInUse );
    static TInt CompareMailAddressesL( CFSMailAddress* aAddr1, CFSMailAddress* aAddr2 );
    static HBufC* CreateCompareNameForAddressLC( const CFSMailAddress& aAddress );
    static TInt CompareMailSubjectsL( const CFSMailMessage* aMessage1, const CFSMailMessage* aMessage2 );
    static HBufC* CreateSubjectWithoutLocalisedPrefixLC( const CFSMailMessage* aMessage, TBool aSuppressNotAvailableText = EFalse );
    static HBufC* CreateSubjectTextLC( const CFSMailMessage* aMessage );
    static HBufC* CreateBodyTextLC( const CFSMailMessage* aMessage );
    static void FilterListItemTextL( TDes& aText );
    static TMessageSizeClass MessageSizeClass( const CFSMailMessage& aMessage );
        
    static HBufC* ConvertHtmlToTxtL( const TDesC& aHtml );
		
    static void DisplayMsgsMovedNoteL( TInt aMsgCount, 
                                       const TFSMailMsgId aDestinationFolderId,
                                       TBool aIsWaitingNote );

    static TBool IsOfflineModeL();
			
    /**
    * Shows mailbox creation query:
    * "No e-mail mailboxes defined. Create a mailbox?"
    *
    * @return ETrue if user answered "Yes", EFalse if user answered "No"
    */
    static TBool DisplayCreateMailboxQueryL();
    
//	static CAlfTexture& GetAttachmentIconL( TFileType aAttachmentType, CFreestyleEmailUiTextureManager& aTextureManager );
//	static TFSEmailUiTextures GetAttachmentIconTextureIdL( TFileType aAttachmentType );

    // Brings Freestyle Email UI application to foreground if it's not already
    static void BringFsEmailToForeground();
  
    // Formats display name
	static void StripDisplayName( HBufC& aDisplayName );

    // Creates a displayname of firstname and lastname components
    static HBufC* CreateDisplayNameLC( const TDesC& aFirstname, const TDesC& aLastname );

    // Creates a displayname of firstname, lastname, emailfield components
    static HBufC* CreateDisplayNameLC( const TDesC& aFirstname, const TDesC& aLastname, const TDesC& aEmailField );
    
    /**
     * Determines if the descriptor is a Chinese word
     */
    static TBool IsChineseWord( const TDesC& aWord );
    
    // Get the icon file name including the drive letter and full path
    static void GetFullIconFileNameL( TDes& aFileName );
    
    // Check if given file names have the same file extension (omitting the character case)
    static TBool DoFileExtensionsMatchL( const TDesC& aFileName1, const TDesC& aFileName2 );
    
    /**
     * Makes a copy of given file to the temp directory of FsEmailUI. 
     * Caller is reponsible of closing the returned file handle.
     */
    static RFile CopyFileToTempDirL( const RFile& aSourceFile, const TDesC& aTargetFileName );
    
    // Deletes all files from our temporary directory
    static void EraseTempDir();
    
    // Checks if FSMailServer is running. If not, then launches it.
    static void EnsureFsMailServerIsRunning( RWsSession& aWsSession );
    
    //
    static void SetDownloadSave( TBool aValue );    
    //
    static void SetSaveSelect( TBool aValue );      
private:
    static TFSEmailUiTextures GetReadMsgIcon( CFSMailMessage* aMsgPtr );
    static TFSEmailUiTextures GetUnreadMsgIcon( CFSMailMessage* aMsgPtr );
    // <cmail>
    static TBool IsMrCancellationMsgL( CFSMailMessage* aMsgPtr );
    // </cmail>
      		
    /**
     * Get EmailUI temp directory path. Creates the directory if it does not exist.
     * Caller must supply large enough descriptor. Maximum path length in Symbian is 256 characters.
     */
    static void GetTempPathL( RFs& aFs, TDes& aPath );    

    /**
     * Show "save to folder" or "select file" dialog.
     * @param   aType       Dialog type as defined in enumeration TCommonDialogType
     * @param   aFileName   Output argument for the selected file/folder name
     * @return  ETrue       if file/folder selected (i.e. dialog not cancelled)
     */
    static TBool ShowFileDialogL( TInt aType, TDes& aFileName );

    // Checks if FSMailServer is running. If not, then launches it.
    static void EnsureFsMailServerIsRunningL( RWsSession& aWsSession );

private: // static data
    static CAknGlobalNote* iGlobalWaitNote;
    static CAknWaitDialog* iOpeningWaitNote;
    static TBool iSaveSelect;
    static TBool iDownloadSave;
    
    // <cmail>
    // A separate MR viewer instance for checking the MR method type.
    // The instance owned by AppUi can't be used for this because that instance
    // may be in use at time we need to check the method.
    static CESMRIcalViewer* iMrViewer;
    
    class CMrViewerEmptyCallback : public CBase, public MESMRIcalViewerCallback
        {
        virtual void ProcessAsyncCommandL( TESMRIcalViewerOperationType, const CFSMailMessage&, MESMRIcalViewerObserver* ) {}
        virtual void ProcessSyncCommandL( TESMRIcalViewerOperationType, const CFSMailMessage& ) {}
        virtual TBool CanProcessCommand( TESMRIcalViewerOperationType ) const { return EFalse; }
        };
    static CMrViewerEmptyCallback* iMrViewerCallback;
    // </cmail>
    };

/**
*  Action menu handler
*
*  Is constructed in CFreestyleEmailUiAppUi when application is launched.
*  After that the static functions can be used to operate with action menu.
*
*/
class CFSEmailUiActionMenu : public CBase
    {
	friend class CFreestyleEmailUiAppUi;
public:
    /**
    * Get pointer to the action menu, can be used if built-in methods are
    * not enough. Ownership is not transferred.
    *
    * @return Action menu pointer
    */
    static CFscContactActionMenu* GetActionMenu();
		
    /**
    * Remove all items from action menu
    */
    static void RemoveAllL();
		
    /**
    * Add one email specific custom item by id
    *
    * @param aItemId Item id of the item to be added
    */
    static void AddCustomItemL( TActionMenuCustomItemId aItemId );
		
    /**
    * Add several email specific custom items by list of ids
    *
    * @param aItemList List of item ids to be added
    */
    static void AddCustomItemsL( RFsEActionMenuIdList aItemList );

	// <cmail>
    /**
    * Execute previously constructed action menu
    *
    * @param aPosition Vertical position where menu is to be openned
    * @param aIndex Index of focused/highlighted item when menu is opened
    * @return Id of the selected item
    */
    static TActionMenuCustomItemId ExecuteL( TFscContactActionMenuPosition aPosition = EFscCenter,
                                             TInt aIndex = 0, MFsActionMenuPositionGiver* aPositionGiver = 0 );
    /**
    * Remove all old items, add new items from the aItemList and then
    * execute the action menu
    *
    * @param aItemList List of item ids to be added
    * @param aPosition Vertical position where menu is to be openned
    * @param aIndex Index of focused/highlighted item when menu is opened
    * @return Id of the selected item
    */
    // 
    static TActionMenuCustomItemId ExecuteL( RFsEActionMenuIdList aItemList,
                                             TFscContactActionMenuPosition aPosition = EFscCenter,
                                             TInt aIndex = 0, MFsActionMenuPositionGiver* aPositionGiver = 0 );
	
    /**
    * Dismiss menu
    *   Menu gets closed if it was visible when the method was called. 
    *   Otherwise method call does not do anything
    *
    * @param aSlide If ETrue menu slides away from the screen. 
    *   Otherwise it disappears instantly
    */
	static void Dismiss( TBool aSlide );
    // </cmail> 
		
    /**
    * Create icon by bitmap and mask
    *
    * @param aBitmapId Icon bitmap
    * @param aMaskId Icon mask
    * @return Created icon object
    */
    static CGulIcon* CreateIconL( TInt aBitmapId, TInt aMaskId );

protected:
    static CFSEmailUiActionMenu* NewL( CFreestyleEmailUiAppUi* aAppUi );
    ~CFSEmailUiActionMenu();

private:
    CFSEmailUiActionMenu( CFreestyleEmailUiAppUi* aAppUi );
    void ConstructL();
        
    static TActionMenuCustomItemId ResolveSelectedCustomItemIdL();
    static void CreateIconsL();
        
private:
    CFscContactActionService* iService;   
    static CFscContactActionMenu* iActionMenu;
    static TFileName iIconFilePath;
    static RPointerArray<CGulIcon> iIconArray;
    CFreestyleEmailUiAppUi* iAppUi;
	};

/**
*  Generic timer's callback
*
*  Classes inherited from this one can be used as a callback for the generic
*  timer. TimerEvent() is called when the timer elapses.
*
*/
class MFSEmailUiGenericTimerCallback
    {
public:
    /**
     * @param   aTriggeredTimer The timer which launched this event.
     */
    virtual void TimerEventL( CFSEmailUiGenericTimer* aTriggeredTimer ) = 0;
    };

/**
*  Generic timer
*
*  Caller gives MFSEmailUiGenericTimerCallback inherited pointer as a
*  parameter, and that pointer is used as a callback when the timer elapses.
*  Timer interval is given as a parameter for the Start function. If interval
*  not given, or it is less than zero, default interval of 1 second is used.
*
*/
class CFSEmailUiGenericTimer : public CTimer
    {
public:
    
    /**
    * Symbian two-pahse constructor.
    *
    * @param aCallback Callback class
    * @param aPriority Timers priority, EPriorityStandard by default
    */
    static CFSEmailUiGenericTimer* NewL(
        MFSEmailUiGenericTimerCallback* aCallback,
        const TInt aPriority = CActive::EPriorityStandard );

    /**
    * Symbian two-pahse constructor.
    *
    * @param aCallback Callback class
    * @param aPriority Timers priority, EPriorityStandard by default
    */
    static CFSEmailUiGenericTimer* NewLC(
        MFSEmailUiGenericTimerCallback* aCallback,
        const TInt aPriority = CActive::EPriorityStandard );
        
    /**
    * Destructor.
    */
    ~CFSEmailUiGenericTimer();
        
    /**
    * Starts the timer with specified interval, or with the default value.
    *
    * NOTE: Give time in milliseconds (not microseconds)!
    *
    * @param aInterval Timer interval as milliseconds
    */
    void Start( TInt aInterval = KGenericTimerDefaultInterval );

    /**
    * Stops the timer.
    */
    void Stop();

    /**
    * CActive object's RunL
    */
    void RunL();
        
protected:
    /**
    * Constructor.
    */
    CFSEmailUiGenericTimer( MFSEmailUiGenericTimerCallback* aCallback,
                            const TInt aPriority );
		
private:
    /**
    * 2nd phase constructor.
    */
    void ConstructL();

private:
    /* Pointer to callback class */
    MFSEmailUiGenericTimerCallback* iCallback;
    };


/**
 * File handle shutter.
 * 
 * Instance of file handle shutter can be set to handle the closing of an open
 * file handle on embedded viewer application exit.
 */
class CFsEmailFileHandleShutter : public CBase,
    public MAknServerAppExitObserver
    {
public:

    /**
     * Two-phased constuctor. Creates an instance of CFsEmailFileHandleShutter
     * which is set to observe embedded application close events delegated
     * by the given document handler.
     * 
     * @param Reference to document handler instance, which the file handle
     *      shutter is set to observe. 
     */
    static CFsEmailFileHandleShutter* NewL(
        CDocumentHandler& aDocumentHandler );

    /**
     * Destructor.
     */
    ~CFsEmailFileHandleShutter();

    /**
     * Sets the file handle that is to be closed when viewer app exits.
     *
     * @param aFile The file handle.
     */
    void SetFile( RFile aFile );

// from base class MAknServerAppExitObserver

    /**
     * Handle the exit of a connected server app.
     * 
     * @param aReason The reason that the server application exited.
     * This will either be an error code, or the command id that caused the
     * server app to exit.
     */
    void HandleServerAppExit( TInt aReason );

private:

    /** Default constructor. */
    CFsEmailFileHandleShutter();

    /** Second phase constructor. */
    void ConstructL( CDocumentHandler& aDocumentHandler );

private: // data

    TBool iHandleOpen;
    RFile iFileHandle;

    };

class CFSEmailUiCallDialog : public CAknQueryDialog
    {
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent &aKeyEvent, TEventCode aType);
    };

#endif /*__FREESTYLEMAILUI_UTILITIES_H_*/
