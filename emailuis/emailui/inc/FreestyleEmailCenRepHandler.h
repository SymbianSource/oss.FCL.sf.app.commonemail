/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Freestyle Email application internal Central Repository handler class
*
*/

#ifndef FREESTYLEEMAILCENTRALREPOSITORYHANDLER_H
#define FREESTYLEEMAILCENTRALREPOSITORYHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

#include "FreestyleEmailUiConstants.h"

// CONSTANTS


// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
*  Wrapper of Central Repository, responsible for the operations that need to access 
*  the central respository. Parses data for faster use.
*  Singleton class
*
*/
class CFSEmailCRHandler : public CBase
    {
public:
	/**
	 * Two-phased class constructor.
	 */
	static CFSEmailCRHandler* InstanceL();

	/**
	 * Destructor of CFSEmailCRHandler class.
	 */
	virtual ~CFSEmailCRHandler();
        
public:
	
   	
	/**
	 * WarnBeforeDelete
	 */
	TInt WarnBeforeDelete();
	void SetWarnBeforeDelete( TInt aValue );
	
	/*
	 * MessageHeader
	 *
	TInt MessageHeader();
	void SetMessageHeader( TInt aValue );
	*/
	
	/**
	 * MessageListLayout
	 */
	TInt MessageListLayout();
	void SetMessageListLayout( TInt aValue );

	/**
	 * BodyPreview
	 */
	TInt BodyPreview();
	void SetBodyPreview( TInt aValue );
	
	/**
	 * TitleDividers
	 */
	TInt TitleDividers();
	void SetTitleDividers( TInt aValue );

	/**
	 * DownloadNotifications
	 */
	TInt DownloadNotifications();
	void SetDownloadNotifications( TInt aValue );

	/**
	 * ActiveIdle
	 */
	TInt ActiveIdle();
	void SetActiveIdle( TInt aValue );
	
	/**
     * Download html images
     */
    TInt DownloadHTMLImages();
    void SetDownloadHTMLImages( TInt aValue );
	
	/**
	 * AttachmentFileTypes enumerator returns
	 * 0 = All attachment types allowed
	 * 1 = user defined types
	 */
	//TInt AttachmentFileTypes();
	//void SetAttachmentFileTypes( TInt aValue );	
	
	/**
	 * AttachmentFileTypesArray
	 * gives array of user defined filetypes
	 */
	//void AttachmentFileTypesArrayL( CDesCArray& aValue );
	//void AttachmentFileTypesDes( TDes16& aValue );
	//void SetAttachmentFileTypesDes( TDesC& aValue );

	/**
	 * EditorCCVisible()
	 */
	TInt EditorCCVisible();
	void SetEditorCCVisible( TInt aValue );

   	/**
   	 * EditorBCVisible()
   	 */
	TInt EditorBCVisible();
	void SetEditorBCVisible( TInt aValue );
    
    /**
     * Methods to get and set the parameters needed to be able to 
     * create a new mailbox. These are stored in TP Wizard's central repository.
     *
     * @return KErrNone if succeeded, otherwise system wide error codes
     */
    TInt SetupWizAccountType( TDes16& aValue );
    //void SetSetupWizAccountType( const TDesC16& aValue );
    
    TInt SetupWizCentrepStartKey( TDes16& aValue );
    //void SetSetupWizCentrepStartKey( const TDesC16& aValue );
    
    /**
     * Deletes the two central repository keys that the mailbox setup wizard 
     * creates, and that are passed as parameters to plugins.
     */
    void ClearWizardParams();
    
private:
	/**
	 * Default class constructor.
	 * Only NewL can be called
	 */
	CFSEmailCRHandler();
       	
	/**
	 * Second phase class constructor.
	 */
	void ConstructL();  	

private:
	// Handle to Central Repository server
	CRepository* iCentralRepository;
	CRepository* iSetupWizCentRep;
    };

#endif  // FREESTYLEEMAILCENTRALREPOSITORYHANDLER_H

// end of file
