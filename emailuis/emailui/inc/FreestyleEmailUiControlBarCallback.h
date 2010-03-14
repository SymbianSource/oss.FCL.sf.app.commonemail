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
* Description:  FreestyleEmailUi control bar list selection callback API
*
*/

 

#ifndef M_FREESTYLEEMAILUICONTROLBARCALLBACK_H
#define M_FREESTYLEEMAILUICONTROLBARCALLBACK_H

//<cmail>
#include "cfsmailcommon.h"
//</cmail>

// FORWARD DECLARATIONS
class CFSMailFolder;
class CFSMailBox;

enum TFSEmailUiCtrlBarResponse
	{
	EFSEmailUiCtrlBarResponseSelect,
	EFSEmailUiCtrlBarResponseCancel,
	EFSEmailUiCtrlBarResponseSwitchList
	};

// CLASS DECLARATION
/**
 *  Abstract interface of folder list selection callback
 */
class MFSEmailUiFolderListCallback
    {
public:
    /**
     * Callback used when folder is selected in folder list
     *
     * @param aSelectedFolder Reference to selected folder
     * @param aResponse Response type
     */
	 virtual void FolderSelectedL( TFSMailMsgId aSelectedFolderId,
	 							  TFSEmailUiCtrlBarResponse aResponse ) = 0;

    /**
     * Callback used when mailbox is selected in folder list
     *
     * @param aSelectedMailbox Reference to selected mailbox
     */
	virtual void MailboxSelectedL( TFSMailMsgId /*aSelectedMailboxId*/ ) 
	    { /*empty default implementation*/ }

	/**
	 * Getter for the folder button rect. Need to be implemented when
	 * using folder list in popup mode. Empty default implementation is provided.
	 */
	virtual TRect FolderButtonRect()
	    { return TRect(); }
    };


/**
 *  Abstract interface of sort list selection callback
 */
class MFSEmailUiSortListCallback
    {
public:
    /**
     * Callback used when sort order is selected in sort list
     *
     * @param aSortOrder New sort order type
     * @param aResponse Response type
     */
	 virtual void SortOrderChangedL( TFSMailSortField aSortField,
	 							    TFSEmailUiCtrlBarResponse aResponse ) = 0;

    /**
     * Getter for the sort button rect. Need to be implemented when
     * using sort list in popup mode. Empty default implementation is provided.
     */
    virtual TRect SortButtonRect()
        { return TRect(); }
    };

#endif // M_FREESTYLEEMAILUICONTROLBARCALLBACK_H
