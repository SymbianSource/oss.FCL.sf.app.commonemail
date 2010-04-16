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
* Description:  ES Protocol Framework branding manager interface definition
*
*/


#include <nmcommonheaders.h>
#include "emailtrace.h"
#include <FSMAILBRANDMANAGER.rsg>
#include <barsread.h>
#include <bautils.h>
//<cmail>
#include "CFSMailClient.h"
//</cmail>

#include "CFSMailBrandManagerImpl.h"
#include "CFSMailBrand.h"


const TInt KBrandArrayGranularity = 5;
_LIT( KResourceFilePath,"\\resource\\fsmailbrandmanager.rsc" );

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::CFSMailBrandManagerImpl
// -----------------------------------------------------------------------------
CFSMailBrandManagerImpl::CFSMailBrandManagerImpl(
    CFSMailClient& aMailClient ) :
    iMailClient( aMailClient )
	{
    FUNC_LOG;

	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::ConstructL()
    {
    FUNC_LOG;

    // Read resource file, get the drive letter according to the DLL drive
    TFileName dllFileName;
    Dll::FileName( dllFileName );
    
 	TParse parse;
    User::LeaveIfError( parse.Set( KResourceFilePath, &dllFileName, NULL ) );
    TFileName resourceFileName( parse.FullName() );

    if ( !iFsSession.Handle() )
        {
        User::LeaveIfError( iFsSession.Connect() );
        }

    BaflUtils::NearestLanguageFile( iFsSession, resourceFileName );
    
    iResourceFile.OpenL( iFsSession, resourceFileName );
        
    iResourceFile.ConfirmSignatureL();

    HBufC8* resourceBuffer = iResourceFile.AllocReadLC( R_BRANDING_DATA );

    TResourceReader reader;
    reader.SetBuffer( resourceBuffer );

	ConstructFromResourceL( reader );

    CleanupStack::PopAndDestroy( resourceBuffer );

    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::~CFSMailBrandManagerImpl
// -----------------------------------------------------------------------------
CFSMailBrandManagerImpl::~CFSMailBrandManagerImpl()
	{
    FUNC_LOG;
	
	iResourceFile.Close();
    iFsSession.Close();
	
	if ( iBrands )
		{
		iBrands->ResetAndDestroy();
		}
	delete iBrands;
	}
	
// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::NewL
// -----------------------------------------------------------------------------
CFSMailBrandManagerImpl* CFSMailBrandManagerImpl::NewL(
    CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    CFSMailBrandManagerImpl* self =
        CFSMailBrandManagerImpl::NewLC( aMailClient );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::NewLC
// -----------------------------------------------------------------------------
CFSMailBrandManagerImpl* CFSMailBrandManagerImpl::NewLC(
    CFSMailClient& aMailClient )
    {
    FUNC_LOG;
    CFSMailBrandManagerImpl* self =
        new( ELeave ) CFSMailBrandManagerImpl( aMailClient );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::UpdateMailboxNamesL
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::UpdateMailboxNamesL( const TFSMailMsgId aMailBoxId )
    {
    FUNC_LOG;
    
    // list all mailboxes
    RPointerArray<CFSMailBox> mailBoxes;
    TInt rcode = iMailClient.ListMailBoxes( TFSMailMsgId(), mailBoxes );
    if( rcode == KErrNone )
        {
        // Check is there need to change the name of the mailbox
        TPtrC name = GetTextL( EFSMailboxName, aMailBoxId);
        if( name.Length() )
            {
            // check duplicates
            TInt orderNumber(2);
            TBool duplicate(ETrue);
            HBufC* newName = HBufC::NewL(name.Length()+6);
            TPtrC brandedName = name;
            while(duplicate != EFalse)
                {
                duplicate = EFalse;
                for(TInt i = 0; i < mailBoxes.Count( );i++)
                    {
                    if(brandedName == mailBoxes[i]->GetName() && 
                       aMailBoxId != mailBoxes[i]->GetId( ))
                        {
                        duplicate = ETrue;
                        newName->Des().Copy(name);
                        newName->Des().Append(' ');
                        newName->Des().Append('(');
                        newName->Des().AppendNum(orderNumber++);
                        newName->Des().Append(')');
                        brandedName.Set(newName->Des());
                        break;
                        }
                    }
                }
            iMailClient.SetMailboxName(aMailBoxId,brandedName);
            delete newName;
            }
        }
       mailBoxes.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetGraphicL
// -----------------------------------------------------------------------------
CGulIcon* CFSMailBrandManagerImpl::GetGraphicL(
    TFSBrandElement aElement, 
	const TFSMailMsgId& aMailboxId )
	{
    FUNC_LOG;
	CFSMailBox* mailBox( NULL );
	TRAPD( mailboxError,
	       mailBox = iMailClient.GetMailBoxByUidL( aMailboxId ) );
	if ( mailboxError != KErrNone )
	    {
	    User::Leave( mailboxError );
	    }	
	User::LeaveIfNull( mailBox );
	
    CleanupStack::PushL( mailBox );    
    TDesC& brandId = mailBox->GetBrandingIdL();

	CFSMailBrand* brand = FindMatchingBrandL( brandId );
	CleanupStack::PopAndDestroy( mailBox );
	if ( brand == NULL )
	    {
	    return NULL;    
	    }
	
	return brand->GetGraphicL( aElement );
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetGraphicIdsL
// -----------------------------------------------------------------------------
TInt CFSMailBrandManagerImpl::GetGraphicIdsL(
    TFSBrandElement aElement, 
	const TFSMailMsgId& aMailboxId,
    TDes& aIconIds  )
	{
    FUNC_LOG;
	CFSMailBox* mailBox( NULL );
	TRAPD( mailboxError,
	       mailBox = iMailClient.GetMailBoxByUidL( aMailboxId ) );
	if ( mailboxError != KErrNone )
	    {
	    User::Leave( mailboxError );
	    }	
	User::LeaveIfNull( mailBox );
	
    CleanupStack::PushL( mailBox );    
    TDesC& brandId = mailBox->GetBrandingIdL();

	CFSMailBrand* brand = FindMatchingBrandL( brandId );
	CleanupStack::PopAndDestroy( mailBox );

	if ( brand == NULL )
	    {
	    return KErrNotFound;
	    }
    return brand->GetGraphicIdsL( aElement, aIconIds );
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetTextL
// -----------------------------------------------------------------------------
TPtrC CFSMailBrandManagerImpl::GetTextL(
    TFSBrandElement aElement,
    const TFSMailMsgId& aMailboxId )
	{
    FUNC_LOG;
	CFSMailBox* mailBox( NULL );
	TRAPD( mailboxError,
	       mailBox = iMailClient.GetMailBoxByUidL( aMailboxId ) );
	if ( mailboxError != KErrNone )
	    {
	    User::Leave( mailboxError );
	    }
	    
	User::LeaveIfNull( mailBox );
    CleanupStack::PushL( mailBox );
    TDesC& brandId = mailBox->GetBrandingIdL();
	
	CFSMailBrand* brand = FindMatchingBrandL( brandId );
	CleanupStack::PopAndDestroy( mailBox );
	if ( brand == NULL )
	    {
	    return KNullDesC();    
	    }	
	
	return brand->GetText( aElement );
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetTextL
// -----------------------------------------------------------------------------
TPtrC CFSMailBrandManagerImpl::GetTextL(
	TFSBrandElement aElement,
	const TDesC& aBrandId )
	{
    FUNC_LOG;

	CFSMailBrand* brand = FindMatchingBrandL( aBrandId );
	if ( brand == NULL )
	    {
	    return KNullDesC();    
	    }	

	return brand->GetText( aElement );
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetColorL
// -----------------------------------------------------------------------------
TInt CFSMailBrandManagerImpl::GetColorL(
    TFSBrandElement aElement,
    const TFSMailMsgId& aMailboxId,
    TRgb& aColor )
	{
    FUNC_LOG;
	CFSMailBox* mailBox = iMailClient.GetMailBoxByUidL( aMailboxId );
	User::LeaveIfNull( mailBox );
    CleanupStack::PushL( mailBox );    
    TDesC& brandId = mailBox->GetBrandingIdL();
	
	CFSMailBrand* brand = FindMatchingBrandL( brandId );
	CleanupStack::PopAndDestroy( mailBox );
	if ( brand == NULL )
	    {
	    return KErrNotFound;  
	    }
	    
    return brand->GetColor( aElement, aColor );
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::ConstructFromResourceL
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::ConstructFromResourceL( TResourceReader& aReader )
	{
    FUNC_LOG;
	iBrands = new ( ELeave )
				CArrayPtrSeg< CFSMailBrand >( KBrandArrayGranularity );
						
	// Read White Label Brand from Cenrep
	CFSMailBrand* newBrand = CFSMailBrand::NewLC( aReader, true );
	iBrands->AppendL( newBrand );
	CleanupStack::Pop( newBrand );
	
	
	// Read general brands from resource file
	TInt brandCount = aReader.ReadInt16();

	for ( TInt i = 0; i < brandCount; i++ )
		{
		CFSMailBrand* newBrand = CFSMailBrand::NewLC( aReader, false );
		iBrands->AppendL( newBrand );
		CleanupStack::Pop( newBrand );
		}
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::FindMatchingBrandL
// -----------------------------------------------------------------------------
CFSMailBrand* CFSMailBrandManagerImpl::FindMatchingBrandL( const TDesC& aBrandId )
    {
    FUNC_LOG;
    TInt brandCount( iBrands->Count() );
    for ( TInt i( 0 ); i < brandCount; i++ )
        {
        if ( (*iBrands)[ i ]->IsMatching( aBrandId ) )
            return (*iBrands)[ i ];
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::MailboxMatchingBrandIdL
// -----------------------------------------------------------------------------
CFSMailBox* CFSMailBrandManagerImpl::MailboxMatchingBrandIdL(
    const TDesC& aBrandId ) const
    {
    FUNC_LOG;
    RPointerArray<CFSMailBox> mailboxes;
    TInt outcome( KErrNone );
    
    outcome = iMailClient.ListMailBoxes( TFSMailMsgId(), mailboxes );
    if ( outcome != KErrNone )
        {
        mailboxes.ResetAndDestroy();
        User::Leave( outcome );
        }
    
    TInt mailboxCount( mailboxes.Count() );
    TInt mailboxIndexer( 0 );
    while ( mailboxIndexer < mailboxCount )
        {
        TDesC* brandId( NULL );
        TRAPD( brandGetError,
               brandId = &mailboxes[mailboxIndexer]->GetBrandingIdL() )
        if ( brandGetError != KErrNone )
            {
            mailboxes.ResetAndDestroy();
            User::Leave( brandGetError );
            }
            
        if ( *brandId == aBrandId )
            {
            CFSMailBox* matchingMailbox = mailboxes[mailboxIndexer];
            mailboxes.Remove( mailboxIndexer );
            mailboxes.ResetAndDestroy();
            return matchingMailbox;
            }
        
        ++mailboxIndexer;
        }
        
    mailboxes.ResetAndDestroy();
    User::Leave( KErrNotFound );
    return NULL; // To prevent warning
    }

