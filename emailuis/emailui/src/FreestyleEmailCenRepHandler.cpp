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
* Description:  Freestyle Email source file
*
*/


//  INCLUDE FILES
#include "emailtrace.h"
#include <centralrepository.h>
#include <e32svr.h>
#include <aknnotewrappers.h> // for note

#include "FreestyleEmailCenRepHandler.h"
#include "FreestyleEmailCenRepKeys.h"

// CONSTANTS
//_LIT( KFsCrHandlerDelimiter, "," );
//_LIT( KFsCrHandlerEmpty, "" );
//const TInt KFsEmailFileTypesStringLenght = 256;


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CFSEmailCRHandler::CFSEmailCRHandler

// Default class constructor.
// ----------------------------------------------------
//
CFSEmailCRHandler::CFSEmailCRHandler()
	{
    FUNC_LOG;
	}

// ----------------------------------------------------
// CFSEmailCRHandler::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CFSEmailCRHandler::ConstructL()
	{
    FUNC_LOG;

	TRAP_IGNORE( iCentralRepository = CRepository::NewL( KFreestyleEmailCenRep ) );
	
	// has to be trapped because setup wizard is not included in S60 3.1 SDK,
	// and thus the NewL() would leave with KErrNotFound.
	TRAP_IGNORE( iSetupWizCentRep = CRepository::NewL( KSetupWizardCenRep ) );
	}

// ----------------------------------------------------
// CFSEmailCRHandler::NewL
// Two-phased class constructor.
// Singleton
// ----------------------------------------------------
//
CFSEmailCRHandler* CFSEmailCRHandler::InstanceL()
	{
    FUNC_LOG;
	CFSEmailCRHandler* singleton = NULL;

	// Check Thread Local Storage for instance pointer
	singleton = static_cast<CFSEmailCRHandler*>( UserSvr::DllTls( KTlsHandleCRHandler ) );
	if ( !singleton )
		{
		singleton = new ( ELeave ) CFSEmailCRHandler();
		CleanupStack::PushL( singleton );
		singleton->ConstructL();
		CleanupStack::Pop( singleton );
		
		// Store a pointer of a new instance in Thread Local Storage
		TInt err = UserSvr::DllSetTls( KTlsHandleCRHandler, singleton );
		if ( err )
			{
			delete singleton;
			singleton = NULL;
			User::Leave( err );
			}
		}
	return singleton;
	}

// ----------------------------------------------------
// CFSEmailCRHandler::~CFSEmailCRHandler
// Destructor of CFSEmailCRHandler class.
// ----------------------------------------------------
//
CFSEmailCRHandler::~CFSEmailCRHandler()
	{
    FUNC_LOG;
    if( iCentralRepository )
    	{
    	delete iCentralRepository;
    	iCentralRepository = NULL;
    	}
    if( iSetupWizCentRep )
    	{
    	delete iSetupWizCentRep;
    	iSetupWizCentRep = NULL;
    	}
    UserSvr::DllFreeTls( KTlsHandleCRHandler );
    }


// ----------------------------------------------------
// CFSEmailCRHandler::WarnBeforeDelete
// Returns setting warn before delete from CR
// ----------------------------------------------------
//

TInt CFSEmailCRHandler::WarnBeforeDelete()
	{
    FUNC_LOG;
	TInt buf;
	TInt err=KErrNotFound;
	if ( iCentralRepository )
	    {
	    err = iCentralRepository->Get( KFreestyleEmailWarnBeforeDelete, buf );
	    }
	if ( err != KErrNone )
	    {
	    buf = KFreestyleEmailWarnBeforeDeleteDefault;
	    }
    return buf;
    }

// ----------------------------------------------------
// CFSEmailCRHandler::SetMessageHeader
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetWarnBeforeDelete( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailWarnBeforeDelete, aValue );
        }
    }

/*
// ----------------------------------------------------
// CFSEmailCRHandler::SetMessageHeader
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetMessageHeader( TInt aValue )
	{
	iCentralRepository->Set( KFreestyleEmailMessageHeader, aValue );
    }

// ----------------------------------------------------
// CFSEmailCRHandler::MessageHeader
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::MessageHeader()
	{
	TInt buf;
	iCentralRepository->Get( KFreestyleEmailMessageHeader, buf );
    return buf;
    }
*/
// ----------------------------------------------------
// CFSEmailCRHandler::SetMessageListLayout
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetMessageListLayout( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailMessageListLayout, aValue );
        }
    }

// ----------------------------------------------------
// CFSEmailCRHandler::MessageListLayout
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::MessageListLayout()
	{
    FUNC_LOG;
	TInt buf;
	TInt err=KErrNotFound;
	
	if ( iCentralRepository )
	    {
	    err = iCentralRepository->Get( KFreestyleEmailMessageListLayout, buf );
	    }
	if ( err != KErrNone )
	    {
	    buf = KFreestyleEmailMessageListLayoutDefault;
	    }
    return buf;
    }

// ----------------------------------------------------
// CFSEmailCRHandler::SetBodyPreview
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetBodyPreview( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailBodyPreview, aValue );
        }
    }

// ----------------------------------------------------
// CFSEmailCRHandler::BodyPreview
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::BodyPreview()
	{
    FUNC_LOG;
	TInt buf;
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {	
        err = iCentralRepository->Get( KFreestyleEmailBodyPreview, buf );
        }
	if ( err != KErrNone )
        {
        buf = KFreestyleEmailBodyPreviewDefault;
        }
    return buf;
    }

// ----------------------------------------------------
// CFSEmailCRHandler::SetTitleDividers
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetTitleDividers( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailTitleDividers, aValue );
        }
    }

// ----------------------------------------------------
// CFSEmailCRHandler::TitleDividers
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::TitleDividers()
	{
    FUNC_LOG;
	TInt buf;
	
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {
        err = iCentralRepository->Get( KFreestyleEmailTitleDividers, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailTitleDividersDefault;
        }	
    return buf;
    }


// ----------------------------------------------------
// CFSEmailCRHandler::SetDownloadNotifications
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetDownloadNotifications( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailDownloadNotification, aValue );
        }
    }

// ----------------------------------------------------
// CFSEmailCRHandler::DownloadNotifications
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::DownloadNotifications()
	{
    FUNC_LOG;
	TInt buf;
	
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {	
        err = iCentralRepository->Get( KFreestyleEmailDownloadNotification, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailDownloadNotificationDefault;
        }	
    return buf;
    }

// ----------------------------------------------------
// CFSEmailCRHandler::SetActiveIdle
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetActiveIdle( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailActiveIdle, aValue );
        }
    }

// ----------------------------------------------------
// CFSEmailCRHandler::ActiveIdle
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::ActiveIdle()
	{
    FUNC_LOG;
	TInt buf;
	
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {	
        err = iCentralRepository->Get( KFreestyleEmailActiveIdle, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailActiveIdleDefault;
        }	
    return buf;
    }
/*
// ----------------------------------------------------
// CFSEmailCRHandler::SetDownloadHTMLImages
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetDownloadHTMLImages( TInt aValue )
    {
    FUNC_LOG;
	if ( iCentralRepository )
		{
    	iCentralRepository->Set( KFreestyleEmailDownloadHTMLImages, aValue );
		}
    }


// ----------------------------------------------------
// CFSEmailCRHandler::DownloadHTMLImages
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::DownloadHTMLImages()
    {
    FUNC_LOG;
    TInt buf;
    
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {    
        err = iCentralRepository->Get( KFreestyleEmailDownloadHTMLImages, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailDownloadHTMLImagesDefault;
        }
    return buf;
    }
*/
/*
// ----------------------------------------------------
// CFSEmailCRHandler::AttachmentFileTypes
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::AttachmentFileTypes()
	{
	TInt buf;
	iCentralRepository->Get( KFreestyleEmailAttachmentFileTypes, buf );
    return buf;	
	}

// ----------------------------------------------------
// CFSEmailCRHandler::SetAttachmentFileTypes
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetAttachmentFileTypes( TInt aValue )
	{
	iCentralRepository->Set( KFreestyleEmailAttachmentFileTypes, aValue );
	}
*/

// ----------------------------------------------------
// CFSEmailCRHandler::AttachmentFileTypesArrayL
// Array of user defined file types for attachment download
// ----------------------------------------------------
//
/*
void CFSEmailCRHandler::AttachmentFileTypesArray( CDesCArray& aArray )
	{
	TInt position = 0;
	TPtrC fileType;
	TBuf<KFsEmailFileTypesStringLenght> buffer;
	iCentralRepository->Get( KFreestyleEmailAttachmentFileTypesDes, buffer );
	
	while ( (position = buffer.Find( KFsCrHandlerDelimiter )) != KErrNotFound )
		{
		fileType.Set( buffer.Left( position ) );
		aArray.AppendL( fileType );
		buffer.Replace( 0, position+1, KFsCrHandlerEmpty );
		}
	aArray.AppendL( buffer );
	}
*/

// ----------------------------------------------------
// CFSEmailCRHandler::AttachmentFileTypesDes
// String of user defined file types for attachment download
// ----------------------------------------------------
//
/*
void CFSEmailCRHandler::AttachmentFileTypesDes( TDes16& aValue )
	{
	iCentralRepository->Get( KFreestyleEmailAttachmentFileTypesDes, aValue );
	}
*/

// ----------------------------------------------------
// CFSEmailCRHandler::SetAttachmentFileTypesDes
// ----------------------------------------------------
//
/*
void CFSEmailCRHandler::SetAttachmentFileTypesDes( TDesC& aValue )
	{
	iCentralRepository->Set( KFreestyleEmailAttachmentFileTypesDes, aValue );
	}
*/

// ----------------------------------------------------
// CFSEmailCRHandler::EditorCCVisible
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::EditorCCVisible()
	{
    FUNC_LOG;
	TInt buf;
	
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {	
        err = iCentralRepository->Get( KFreestyleEmailEditorCCVisible, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailEditorCCVisibleDefault;
        }	
    return buf;
	}

// ----------------------------------------------------
// CFSEmailCRHandler::SetEditorCCVisible
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetEditorCCVisible( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailEditorCCVisible, aValue );
        }
	}

// ----------------------------------------------------
// CFSEmailCRHandler::EditorBCVisible
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::EditorBCVisible()
	{
    FUNC_LOG;
	TInt buf;
	
    TInt err=KErrNotFound;
    
    if ( iCentralRepository )
        {	
        err = iCentralRepository->Get( KFreestyleEmailEditorBCVisible, buf );
        }
    if ( err != KErrNone )
        {
        buf = KFreestyleEmailEditorBCVisibleDefault;
        }	
    return buf;
	}

// ----------------------------------------------------
// CFSEmailCRHandler::SetEditorBCVisible
// ----------------------------------------------------
//
void CFSEmailCRHandler::SetEditorBCVisible( TInt aValue )
	{
    FUNC_LOG;
    if ( iCentralRepository )
        {
        iCentralRepository->Set( KFreestyleEmailEditorBCVisible, aValue );
        }
	}

// ----------------------------------------------------
// CFSEmailCRHandler::SetupWizAccountType
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::SetupWizAccountType( TDes16& aValue )
	{
    FUNC_LOG;
	if ( iSetupWizCentRep )
	    {
	    return iSetupWizCentRep->Get( KSetupWizardAccountType, aValue );
	    }
	return KErrNotFound;
	}

// ----------------------------------------------------
// CFSEmailCRHandler::SetSetupWizAccountType
// ----------------------------------------------------
//
/*void CFSEmailCRHandler::SetSetupWizAccountType( const TDesC16& aValue )
	{
	if ( iSetupWizCentRep )
	    {
	    iSetupWizCentRep->Set( KSetupWizardAccountType, aValue );
	    }
	}*/

// ----------------------------------------------------
// CFSEmailCRHandler::SetupWizCentrepStartKey
// ----------------------------------------------------
//
TInt CFSEmailCRHandler::SetupWizCentrepStartKey( TDes16& aValue )
    {
    FUNC_LOG;
    if ( iSetupWizCentRep )
        {
        return iSetupWizCentRep->Get( KSetupWizardCentrepStartKey, aValue );
        }
    return KErrNotFound;
    }

// ----------------------------------------------------
// CFSEmailCRHandler::SetSetupWizCentrepStartKey
// ----------------------------------------------------
//
/*void CFSEmailCRHandler::SetSetupWizCentrepStartKey( const TDesC16& aValue )
	{
	if ( iSetupWizCentRep )
	    {
	    iSetupWizCentRep->Set( KSetupWizardCentrepStartKey, aValue );
	    }
	}*/

/*TUint32*/void CFSEmailCRHandler::ClearWizardParams()
    {
    //TUint32 errorKey = 0;
    //TInt error = iSetupWizCentRep->Delete(KSetupWizardAccountType, 0xFFFFFFFF, errorKey);
    if( iSetupWizCentRep )
        {
        iSetupWizCentRep->Delete( KSetupWizardAccountType );
        }
    //TInt error = iSetupWizCentRep->Delete(KSetupWizardCentrepStartKey, 0xFFFFFFFF, errorKey);
    if( iSetupWizCentRep )
        {
        iSetupWizCentRep->Delete( KSetupWizardCentrepStartKey );
        }
    //return errorkey;
    }

// end of file

