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
* Description:  General constants for FS Email UI
*
*/


#ifndef __FREESTYLEEMAILUICONSTANTS_H__
#define __FREESTYLEEMAILUICONSTANTS_H__

//#define PBKX_DEBUG_LEVEL 0 // Print only errors
//#define PBKX_DEBUG_LEVEL 1 // Print errors and warnings
//#define PBKX_DEBUG_LEVEL 2 // Print errors, warnings, traces and infos
#define PBKX_DEBUG_LEVEL 3 // Print errors, warnings, traces, infos, details

//<cmail>
#include "CFSMailCommon.h"
//</cmail>
#include <f32file.h>

class CFSMailMessage;
class MFSEmailUiFolderListCallback;

/**
* application UID and executable name
*/
// <cmail> S60 UID update
const TUid KFSEmailUiUid = { 0x2001E277 };
// </cmail> S60 UID update
_LIT( KFSEmailUiExec, "FreestyleEmailUi.exe" );


/**
* view ids
*/
const TUid AppGridId = { 1 };
const TUid MailListId = { 2 };
const TUid SearchListViewId = { 3 };
const TUid MailViewerId = { 4 };
const TUid MailEditorId = { 5 };
const TUid SettingsViewId = { 6 };
const TUid MsgDetailsViewId = { 7 };
const TUid FolderListId = { 8 };
const TUid AttachmentMngrViewId = { 9 };
const TUid GlobalSettingsViewId = { 10 };
const TUid DownloadManagerViewId = { 11 };
const TUid SendAttachmentMngrViewId = { 12 };
const TUid HtmlViewerId = { 13 };
const TUid UiDummyViewId = { 14 };


// Display groups
const TInt KGridDisplayGroup = 1;
const TInt KMailListDisplayGroup = 2;
const TInt KEmailViewerDisplayGroup = 3;
const TInt KEmailEditorDisplayGroup = 4;
const TInt KFolderListDisplayGroup = 5;
const TInt KSettingsListDisplayGroup = 6;
const TInt KAttachmentManagerDisplayGroup = 7;
const TInt KMsgDetailsViewerControlGroup = 8;
const TInt KDownloadManagerDisplayGroup = 9;
const TInt KSearchResultDisplayGroup = 10;
const TInt KStatusIndicatorDisplayGroup = 11;
const TInt KHtmlViewerDisplayGroup = 12;
const TInt KDummyControlGroup = 13;
const TInt KDownloadIndicatorControlGroup = 14;
const TInt KSendAttachmentManagerDisplayGroup = 15;

/**
 * TLS handles
 */
const TInt KTlsHandleCRHandler = 0xC0CEDE;
const TInt KTlsHandleDownloadInfo = 0xCECECE;
const TInt KTlsHandleMailListModel = 0xCECEDE;

/**
* Action menu custom item ids
*/
enum TActionMenuCustomItemId
	{
	FsEActionMenuDismissed = -10,
	FsEActionMenuCasItemSelectedAndExecuted,
	
	FsEActionMenuOpen = 1,
	FsEActionMenuOpenCalendarEvent,
	FsEActionMenuReply,
	FsEActionMenuReplyAll,
	FsEActionMenuForward,
	FsEActionMenuDelete,
	FsEActionMenuAccept,
	FsEActionMenuTentative,
	FsEActionMenuDecline,
	FsEActionMenuRemoveFormCal,
	FsEActionMenuMarkRead,
	FsEActionMenuMarkUnread,
	FsEActionMenuCall,
    // <cmail> video call
    FsEActionMenuCallVideo,
    // </cmail>
	FsEActionMenuCreateMessage,
	FsEActionMenuCreateEmail,
	FsEActionMenuContactDetails,
	FsEActionMenuAddToContacts,
	FsEActionMenuRemoteLookup,
	FsEActionMenuMove,
	FsEActionMenuMoveToDrafts,
	FsEActionMenuOpenInWeb,
	FsEActionMenuOpenInIntranet,
	FsEActionMenuBookmark,
	FsEActionAttachmentOpen,
	FsEActionAttachmentDownload,
	FsEActionAttachmentCancelDownload,
	FsEActionAttachmentSave,
    FsEActionAttachmentAdd,
    FsEActionAttachmentRemove,
	FsEActionAttachmentViewAll,
	FsEActionAttachmentDownloadAll,
	FsEActionAttachmentCancelAllDownloads,
	FsEActionAttachmentSaveAll,
    FsEActionAttachmentRemoveAll,
    FsEActionAttachmentClearFetchedContent,
    FsEActionAttachmentRemoveUnfetchedAttachment
	};

const TUid FsEActionMenuOpenUid = { FsEActionMenuOpen };
const TUid FsEActionMenuReplyUid = { FsEActionMenuReply };
const TUid FsEActionMenuReplyAllUid = { FsEActionMenuReplyAll };
const TUid FsEActionMenuForwardUid = { FsEActionMenuForward };
const TUid FsEActionMenuDeleteUid = { FsEActionMenuDelete };
const TUid FsEActionMenuAcceptUid = { FsEActionMenuAccept };
const TUid FsEActionMenuTentativeUid = { FsEActionMenuTentative };
const TUid FsEActionMenuRemoveFromCalUid = { FsEActionMenuRemoveFormCal };
const TUid FsEActionMenuDeclineUid = { FsEActionMenuDecline };
const TUid FsEActionMenuMarkReadUid = { FsEActionMenuMarkRead };
const TUid FsEActionMenuMarkUnreadUid = { FsEActionMenuMarkUnread };
const TUid FsEActionMenuCallUid = { FsEActionMenuCall };
// <cmail> video call
const TUid FsEActionMenuCallVideoUid = { FsEActionMenuCallVideo };
// </cmail>
const TUid FsEActionMenuCreateMessageUid = { FsEActionMenuCreateMessage };
const TUid FsEActionMenuContactDetailsUid = { FsEActionMenuContactDetails };
const TUid FsEActionMenuAddToContactsUid = { FsEActionMenuAddToContacts };
const TUid FsEActionMenuRemoteLookupUid = { FsEActionMenuRemoteLookup };
const TUid FsEActionMenuMoveUid = { FsEActionMenuMove };
const TUid FsEActionMenuUidMoveToDrafts = { FsEActionMenuMoveToDrafts };

const TUid FsEActionMenuOpenInWebUid = { FsEActionMenuOpenInWeb };
const TUid FsEActionMenuOpenInIntranetUid = { FsEActionMenuOpenInIntranet };
const TUid FsEActionMenuBookmarkUid = { FsEActionMenuBookmark };
const TUid FsEActionAttachmentOpenUid = { FsEActionAttachmentOpen };
const TUid FsEActionAttachmentDownloadUid = { FsEActionAttachmentDownload };
const TUid FsEActionAttachmentCancelDownloadUid = { FsEActionAttachmentCancelDownload };
const TUid FsEActionAttachmentSaveUid = { FsEActionAttachmentSave };
const TUid FsEActionAttachmentDownloadAllUid = { FsEActionAttachmentDownloadAll };
const TUid FsEActionAttachmentCancelAllDownloadsUid = { FsEActionAttachmentCancelAllDownloads };
const TUid FsEActionAttachmentSaveAllUid = { FsEActionAttachmentSaveAll };


/**
 * Settings launch modes
 */
 enum TMailSettingsLaunchMode 
 	{
 	KMailSettingsOpenMainList = 0x112,
 	KMailSettingsOpenMailboxSettings,
 	KMailSettingsOpenGlobalSettings,
 	KMailSettingsOpenServiceSettings,
 	KMailSettingsReturnToPrevious,
 	KMailSettingsReturnFromPluginSettings,
 	KOpenMailSettingsFromGS,
 	KMailSettingsOpenPluginSettings
 	};

 	
/**
* editor launch modes
*/
enum TEditorLaunchMode 
	{
	KEditorCmdCreateNew,
	KEditorCmdReply,
	KEditorCmdReplyAll,
	KEditorCmdForward,
	KEditorCmdOpen,
	KEditorCmdInternalMailto,
	KEditorCmdOpenAttachmentsList,
	KEditorCmdReturnToPrevious
	};

/**
* editor launch parameters
*/
struct TEditorLaunchParams
    {
    TFSMailMsgId iMailboxId;
    TFSMailMsgId iFolderId;
    TFSMailMsgId iMsgId;
    TBool iActivatedExternally;	// ETrue makes editor close application when it exits
    TAny* iExtra;
    };

/**
* attachment file types
*/
enum TFileType
	{
	EPdfType,
	EDocType,
	ERtfType,
	EPptType,
	EXlsType,
	EImageType,
	EHtmlType,
	EPlainTextType,
	EVCalType,
	EMessageType,
	EUnidentifiedType
	};


/**
* reply/forward message creation parameters
*/
struct TReplyForwardParams
    {
    HBufC* iHeader; // reply/forward header, ownership stays in UI
    HBufC* iSmartTag; // smart reply/forward tag, ownership stays in UI
    };

/**
* List launch parameters
* Use iRequestRefresh = ETrue to force list refresh when using 
* KStartListReturnToPreviousFolder parameter.
*/
static const TUid KStartListWithFolderId = { 1 };
static const TUid KStartListReturnToPreviousFolder = { 2 };

class TMailListActivationData
	{
public: // constructor
	inline TMailListActivationData()
        : iMailBoxId(), iFolderId(), iRequestRefresh(EFalse), iReturnAfterWizard(EFalse)
	    {
	    // no implementation needed
	    }
	
public: // data	
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	TBool iRequestRefresh;
    TBool iReturnAfterWizard;
	};

/**
* Viewer launch parameters
*/
class TMsgViewerActivationData
	{
public: // construction
	inline TMsgViewerActivationData() 
	    : iMailBoxId(), 
	      iFolderId(), 
	      iMessageId(),
	      iDetails( EFSMsgDataEnvelope ),
	      iEmbeddedMessage( NULL )
	      {}
	
public: // members	
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	TFSMailMsgId iMessageId;
	TFSMailDetails iDetails;
    CFSMailMessage* iEmbeddedMessage;
	};

static const TUid KStartViewerWithMsgId = { 1 };
static const TUid KStartViewerReturnToPreviousMsg = { 2 };
static const TUid KStartViewerWithEmbeddedMsgPtr = { 3 };
static const TUid KStartViewerReturnFromEmbeddedMsg = { 4 };


/**
* Search list activation parameters
*/
static const TUid KStartNewSearch = { 1 };
static const TUid KStartWithPreviousResults = { 2 };

class TSearchListActivationData
	{
public: // construction
	inline TSearchListActivationData()
	    : iMailBoxId(), iFolderId() {}
	
public: // members	
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	};

/**
* HTML viewer activation parameters
*/
static const TUid KHtmlViewerOpenNew          = { 1 };
static const TUid KHtmlViewerReturnToPrevious = { 2 };

class THtmlViewerActivationData
	{
public: // data types
    enum TActivationDataType
        {
        EFile,          // Load content from RFile
        EFileName,      // Load content from file
        EUrl,           // Load content from URL
        EMailMessage,    // Load content from mail message.
        EmbeddedEmailMessage   // Load content from embedded mail message.
                        // Only with this option embedded pictures are shown.
        };

public: // methods
    // default constructor
	inline THtmlViewerActivationData() 
	    : iActivationDataType( EMailMessage ),
	      iMailBoxId(),
	      iFolderId(),
	      iMessageId(),
	      iFile(),
	      iFileName( KNullDesC ),
	      iUrl( KNullDesC ),
	      iEmbeddedMessage( NULL ),
          iEmbeddedMessageMode( EFalse ) {}
	
	// substitution operator
	inline THtmlViewerActivationData& operator=( const THtmlViewerActivationData& aAnother )
	    {
	    if ( this != &aAnother )
	        {
    	    iActivationDataType = aAnother.iActivationDataType;
    	    iMailBoxId = aAnother.iMailBoxId;
    	    iFolderId = aAnother.iFolderId;
    	    iMessageId = aAnother.iMessageId;
    	    iFile = aAnother.iFile;
    	    iFileName.Set( aAnother.iFileName );
    	    iUrl.Set( aAnother.iUrl );
    	    iEmbeddedMessage = aAnother.iEmbeddedMessage;
    	    iEmbeddedMessageMode = aAnother.iEmbeddedMessageMode;
	        }
	    return *this;
	    }
	
public: // members	
    TActivationDataType iActivationDataType;
    TFSMailMsgId iMailBoxId; // Mail box ID of the message used in activation.
    TFSMailMsgId iFolderId; // Folder ID of the message used in activation.
    TFSMailMsgId iMessageId; // Message ID of the message used in activation.
	RFile iFile; // HTML file to be shown.
	TPtrC iFileName; // Name of the HTML file to be shown.
	TPtrC iUrl; // URL to be loaded.
	TBool iEmbeddedMessageMode; // Some UI options are blocked in embedded messages.
	CFSMailMessage* iEmbeddedMessage;
	};

/**
* Attachment list activation parameters
*/
class TAttachmentListActivationData
	{
public: // construction
	inline TAttachmentListActivationData()
	    : iMailBoxId(), iFolderId(), iMessageId(), iDetails(EFSMsgDataEnvelope), iEmbeddedMsgMode(EFalse) {}
	
public: // members	
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	TFSMailMsgId iMessageId;
	TFSMailDetails iDetails;
	TBool iEmbeddedMsgMode;
	};

/**
* Message details view launch parameters
*/
static const TUid KStartMsgDetailsToBeginning      = { 1 };
static const TUid KStartMsgDetailsToTo             = { 2 };
static const TUid KStartMsgDetailsToCc             = { 3 };
static const TUid KStartMsgDetailsToBcc            = { 4 };
static const TUid KStartMsgDetailsReturnToPrevious = { 5 };

class TMsgDetailsActivationData
	{
public: // construction
	inline TMsgDetailsActivationData()
	    : iMailBoxId(), iFolderId(), iMessageId() {}

public: // members
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	TFSMailMsgId iMessageId;
	};

/**
* Folder list view launch parameters
*/
static const TUid KFolderListSelectFolder = { 1 };
static const TUid KFolderListCopyMessage  = { 2 };
static const TUid KFolderListCopyMessages = { 3 };
static const TUid KFolderListMoveMessage  = { 4 };
static const TUid KFolderListMoveMessages = { 5 };

class TFolderListActivationData
	{
public: // construction
	inline TFolderListActivationData()
        : iSourceFolderType( EFSOther ), iCallback( NULL ) {}
	
public: // members	
	// Type of the source folder in case of move or copy
	TFSFolderType iSourceFolderType;
	
	// Call back used to inform the folder selection query result
	MFSEmailUiFolderListCallback* iCallback;
	};

/**
* Message part data
*/
class TPartData
	{
public: // data
	TFSMailMsgId iMailBoxId;
	TFSMailMsgId iFolderId;
	TFSMailMsgId iMessageId;
	TFSMailMsgId iMessagePartId;
	
public: // methods
	// default constructor initializes all IDs as NULL ID
	inline TPartData()
	    : iMailBoxId(), iFolderId(), iMessageId(), iMessagePartId()
	    {
	    }
	
	// construction with initial data
	inline TPartData( TFSMailMsgId aMailBoxId, TFSMailMsgId aFolderId, 
	           TFSMailMsgId aMessageId, TFSMailMsgId aMessagePartId )
	    : iMailBoxId( aMailBoxId ), iFolderId( aFolderId ), 
	      iMessageId( aMessageId ), iMessagePartId( aMessagePartId )
	    {  
	    }
	
	// construction with initial data except messagePartId
	inline TPartData( TFSMailMsgId aMailBoxId, TFSMailMsgId aFolderId, TFSMailMsgId aMessageId )
	    : iMailBoxId( aMailBoxId ), iFolderId( aFolderId ), 
	      iMessageId( aMessageId ), iMessagePartId()
	    {  
	    }
	
	// equality operator
	inline TBool operator==( const TPartData& aPart ) const 
		{
		return ( iMailBoxId==aPart.iMailBoxId && iFolderId==aPart.iFolderId &&
		         iMessageId==aPart.iMessageId && iMessagePartId==aPart.iMessagePartId );
		}
	};

/**
* CCoeStatic derived singleton ids
*/
const TUid KDelayedLoaderUid = { 1001 };


/**
* Alfred action commands
*/
const TInt KCmdEditorAutoSave = 9901;

/**
 * UID for launching wizard
 */
const TUid KAiwCmdSettingWizardFsEmail = { 0x2000CFE9 };

/**
 * UID for launching browser
 */
static const TInt KBrowserUid = 0x10008D39;

/**
 * UID for identifying Message Reader in the taskList 
 */
static const TUid KMessageReaderUid = { 0x10201B00 };

/**
 * UIDs for identifying Homescreen Modes application and its Email view
 */
static const TUid KHsModesAppUid = { 0x20002581 };
static const TUid KHsModesPrimaryEmailViewId = { 14 };

#endif
