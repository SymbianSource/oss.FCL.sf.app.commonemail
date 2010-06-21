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
* Description:  FreestyleEmailUi attachments list model and model item definition.
*
*/


#ifndef __FREESTYLEEMAILUI_ATTACHMENTSLISTMODEL_H__
#define __FREESTYLEEMAILUI_ATTACHMENTSLISTMODEL_H__

// SYSTEM INCLUDES
#include <e32base.h>
//<cmail>
#include "fstreelist.h"
//</cmail>

// INTERNAL INCLUDES
#include "FreestyleEmailDownloadInformationMediator.h"

// FORWARD DECLARATIONS
class CFreestyleEmailUiAppUi;
class CFSMailMessage;

// CONSTANTS
// Limit values of download progress
const TInt KNone = 0;
const TInt KComplete = 100;

// data of one attachment
struct TAttachmentData
	{
	// size of the attachment in bytes
	TInt fileSize;
	// download progress in %, 0-100
	TInt downloadProgress;
	// attachment file type
	TFileType fileType;
	// attachment file name
	TFileName fileName;
	// attachment tree entry id in attachment list
	TFsTreeItemId treeId;
	// attachment message part information
	TPartData partData;
	};

// Observer interface used to inform view about updates in model
class MFSEmailUiAttachmentsStatusObserver
    {
public:
    /**
    * Informs that the status of the given attachment has changed.
    * @param    aIndex  The index of the changed object in the model. Note that
    *                   it's possible that a new object has been added to the model.
    *                   In that case, this index is greater than the previous last
    *                   index of the model. KErrNotFound indicates that the list
    *                   was completely updated and thus UI needs to refresh
    *                   itself totally.
    */
    virtual void DownloadStatusChangedL( TInt aIndex ) = 0;
    };


//////////////////////////////////////////////////////////////////////////////
// Base class contains common functionality for the Attachments list model and 
// Download Manager model
class CFSEmailUiAttachmentsModelBase : public CBase, public MFSEmailDownloadInformationObserver
    {
public: // methods
    ~CFSEmailUiAttachmentsModelBase();

	// starts download
	TBool StartDownloadL( TFsTreeItemId aTreeId );
    TBool StartDownloadL( const TAttachmentData& aAttachment );
	// saves attachment
    TBool SaveAttachmentL( TFsTreeItemId aTreeId, const TDesC& aFileName, TInt& aSavedCount );
	TBool SaveAttachmentL( const TAttachmentData& aAttachment, const TDesC& aFileName, TInt& aSavedCount );
	// cancels download operation
    void CancelDownloadL( TFsTreeItemId aTreeId );
	void CancelDownloadL( const TAttachmentData& aAttachment );
	// removes the downloaded file contents
    void RemoveContentL( TFsTreeItemId aTreeId );
	void RemoveContentL( const TAttachmentData& aAttachment );
	// is the message part currently being downloaded  
	TBool IsDownloading( const TAttachmentData& aAttachment );
    // is the message part completely downloaded
    TBool IsCompletelyDownloadedL( const TAttachmentData& aAttachment );
    // is the message part an embedded message object
    TBool IsMessage( const TAttachmentData& aAttachment ) const;
	// returns ETrue if there has been at least one download started after program start
	TBool IsThereAnyCurrentDownloads();
    // Creates and returns a size description matching the given size. Ownership is transferred to CleanupStack.
    static HBufC* CreateSizeDescLC( TInt aSize );
    // Get AppUi handle
	CFreestyleEmailUiAppUi& AppUi();

public: // pure virtual methods
    // get attachment data from list using tree entry 
    virtual TAttachmentData* GetItem( TFsTreeItemId aTreeId ) = 0;
	// get attachment data from list using tree entry 
	virtual TAttachmentData& GetItemL( TFsTreeItemId aTreeId ) = 0;
	// get message object related to the given tree entry
	virtual TPartData GetMessageL( TFsTreeItemId aTreeId ) = 0;
	// downloads all attachments
	virtual TBool DownloadAllAttachmentsL() = 0;
	// saves all attachments
	virtual TBool SaveAllAttachmentsL( const TDesC& aFileName ) = 0;
	// cancels all downloads
	virtual void CancelAllDownloadsL() = 0;
	// check if model contains any attachment of message type
	virtual TBool IsThereAnyMessageAttachments() const = 0;

protected: // methods
    CFSEmailUiAttachmentsModelBase( CFreestyleEmailUiAppUi& aAppUi );
    void ConstructL();

protected: // data
    CFreestyleEmailUiAppUi& iAppUi;
    };


/////////////////////////////
// The Attachments list model
class CFSEmailUiAttachmentsListModel : public CFSEmailUiAttachmentsModelBase 
    {
public:
	// download progress infromation callback, from MFSEmailDownloadInformationObserver
	virtual void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );
	// two-phased constructor
	static CFSEmailUiAttachmentsListModel* NewL( CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	// two-phased constructor
	static CFSEmailUiAttachmentsListModel* NewLC(CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	// destructor
	~CFSEmailUiAttachmentsListModel();
	// returns the whole model
	const RArray<TAttachmentData>& GetModel();
    // adds node id to attachment data 
    void SetNodeIdL( TInt aAttachmentIndex, TFsTreeItemId aTreeItemId );
    // updates attachment list 
    void UpdateListL( TPartData aMessage );
    void UpdateListL( CFSMailMessage* aEmbeddedMessage );
    // Creates and returns mail subject text. Ownership is transferred to CleanupStack.
    HBufC* CreateMailSubjectTextLC() const;
    // Returns ETrue if there is multiple attachment downloads requested
    TBool IsMultiplyDownloadsOngoing();

public: // from CFSEmailUiAttachmentsModelBase
    // get attachment data from list using tree entry 
    TAttachmentData* GetItem( TFsTreeItemId aTreeId );
	// get attachment data from list using tree entry 
	TAttachmentData& GetItemL( TFsTreeItemId aTreeId );
	// get message object related to the given tree entry
	TPartData GetMessageL( TFsTreeItemId aTreeId );
	// downloads all attachments
	TBool DownloadAllAttachmentsL();
	// saves all attachments
	TBool SaveAllAttachmentsL( const TDesC& aFileName );
	// cancels all downloads
	void CancelAllDownloadsL();
    // check if model contains any attachment of message type
    TBool IsThereAnyMessageAttachments() const;
    // check if model contains any attachment of non-message type
    TBool IsThereAnyNonMessageAttachments() const;

private:
	CFSEmailUiAttachmentsListModel( CFreestyleEmailUiAppUi& aAppUi, MFSEmailUiAttachmentsStatusObserver& aObserver );
	void ConstructL();

private:
	MFSEmailUiAttachmentsStatusObserver& iObserver;
	TPartData iMessage;
	RArray<TAttachmentData> iAttachments;
    };

#endif //__FREESTYLEEMAILUI_MAILLISTMODEL_H__

