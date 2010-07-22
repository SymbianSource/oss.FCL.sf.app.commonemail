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

#include "emailtrace.h"

//<qmail>
#include <nmcommonheaders.h>
//</qmail>

#include <FSMAILBRANDMANAGER.rsg>
#include <barsread.h>
#include <bautils.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // CleanupResetAndDestroy
// <gmail_brand_issue>
#include <etelmm.h>
#include <mmtsy_names.h>
#include <startupdomainpskeys.h>
#include <tzlocalizer.h> // CTzLocalizer
#include <centralrepository.h>
// </gmail_brand_issue>
//<cmail>
#include "CFSMailClient.h"
//</cmail>

#include "CFSMailBrandManagerImpl.h"
#include "CFSMailBrand.h"


const TInt KBrandArrayGranularity = 5;
_LIT( KResourceFilePath,"\\resource\\fsmailbrandmanager.rsc" );

// The following are needed to convert "Gmail" brand name to "Google Mail"
// in certain countries.
// <gmail_brand_issue>

_LIT( KBrandNameGmail, "Gmail" );
_LIT( KBrandNameGoogleMail, "Google Mail" );

_LIT( KMCCGermany, "262" );
_LIT( KMCCUK1, "234" );
_LIT( KMCCUK2, "235" );

const TUint8 KGermanyTzId = 36;
const TUint8 KUKTzId = 104;

const TInt KMCCValueMaxLength = 3;

#ifdef __WINS__
LOCAL_C void RetrieveNextToken( TDes8& aContent, TDes& aToken )	
	{
    NM_FUNCTION;
    
	_LIT8( KComma, "," );
	TInt pos = aContent.Find( KComma );
	if ( pos != KErrNotFound ) 
		{
		aToken.Copy( aContent.MidTPtr( 0, pos ) );
		aContent.Copy(
		    aContent.RightTPtr( aContent.Length() - pos - 1 ) );
		}
	}
#endif // __WINS__
// </gmail_brand_issue>

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::CFSMailBrandManagerImpl
// -----------------------------------------------------------------------------
CFSMailBrandManagerImpl::CFSMailBrandManagerImpl(
    CFSMailClient& aMailClient ) :
    iMailClient( aMailClient )
	{
    NM_FUNCTION;
	}

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::ConstructL()
    {
    NM_FUNCTION;

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
    NM_FUNCTION;
	
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
    // list all mailboxes
    RPointerArray<CFSMailBox> mailBoxes;
    CleanupResetAndDestroyPushL( mailBoxes );
    iMailClient.ListMailBoxes( TFSMailMsgId(), mailBoxes );
    // Check is there need to change the name of the mailbox
    TPtrC name = GetTextL( EFSMailboxName, aMailBoxId);
    // <gmail_brand_issue>
    VerifyMailAccountName( name );
    // </gmail_brand_issue>
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
    CleanupStack::PopAndDestroy( &mailBoxes );
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetGraphicL
// -----------------------------------------------------------------------------
CGulIcon* CFSMailBrandManagerImpl::GetGraphicL(
    TFSBrandElement aElement, 
	const TFSMailMsgId& aMailboxId )
	{
    NM_FUNCTION;
    
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
// CFSMailBrandManagerImpl::GetGraphicL
// -----------------------------------------------------------------------------
CGulIcon* CFSMailBrandManagerImpl::GetGraphicL(
    TFSBrandElement aElement, 
    const TDesC& aBrandId )
    {
    NM_FUNCTION;
    
    CFSMailBrand* brand = FindMatchingBrandL( aBrandId );
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
    NM_FUNCTION;
    
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
// CFSMailBrandManagerImpl::GetGraphicIdsL
// -----------------------------------------------------------------------------
TInt CFSMailBrandManagerImpl::GetGraphicIdsL(
    TFSBrandElement aElement, 
    const TDesC& aBrandId,
    TDes& aIconIds  )
    {
    NM_FUNCTION;

    CFSMailBrand* brand = FindMatchingBrandL( aBrandId );
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;

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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
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
    NM_FUNCTION;
    
    if( aBrandId.Length() )
        {
        TInt brandCount( iBrands->Count() );
        for ( TInt i( 0 ); i < brandCount; i++ )
            {
            if ( (*iBrands)[ i ]->IsMatching( aBrandId ) )
                return (*iBrands)[ i ];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::MailboxMatchingBrandIdL
// -----------------------------------------------------------------------------
CFSMailBox* CFSMailBrandManagerImpl::MailboxMatchingBrandIdL(
    const TDesC& aBrandId ) const
    {    
    NM_FUNCTION;
    
    RPointerArray<CFSMailBox> mailboxes;

    CleanupResetAndDestroyPushL( mailboxes );

    iMailClient.ListMailBoxes( TFSMailMsgId(), mailboxes );

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
        
    CleanupStack::PopAndDestroy( &mailboxes );
    User::Leave( KErrNotFound );
    return NULL; // To prevent warning
    }

// <gmail_brand_issue>
// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetMCCValueL
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::GetMCCValueL( TDes& aMcc ) const
    {
    NM_FUNCTION;
    
    aMcc.Zero();
    
    TInt err = KErrNone;

#ifndef __WINS__

    TBool isSimPresent = EFalse;
    
    TInt simStatus( 0 );

	err = RProperty::Get( KPSUidStartup, KPSSimStatus, simStatus );

	if ( err == KErrNone &&
	        simStatus != ESimNotPresent &&
	        simStatus != ESimNotSupported )
	    {
	    isSimPresent = ETrue;
	    }

    if ( isSimPresent )
        {
        // We cannot let the method leave here
        TRAP( err, GetMCCValueFromSIML( aMcc ) );

        // If reading from SIM fails one time try again after 0.5 secs
        if ( err != KErrNone )
            {
            const TInt KHalfSecond = 500000;

            User::After( KHalfSecond );
            
            err = KErrNone;

            // We cannot let the method leave here
            TRAP( err, GetMCCValueFromSIML( aMcc ) );
            }
        }

#else // __WINS__

    _LIT( KSIMInfo, "C:\\data\\Settings\\SIMInfo.txt" );

    RFs fs;

    User::LeaveIfError( fs.Connect() );

    RFile simFile;

    err = simFile.Open( fs, KSIMInfo(), EFileShareReadersOnly |
                                        EFileStream |
                                        EFileRead );
    if ( err == KErrNone )
        {
        TBuf8<100> content;
        TBuf<100> dummy;

        simFile.Read( content );

    	simFile.Close();

    	fs.Close();

        if ( content.Length() > 0 )
        	{
            RetrieveNextToken( content, dummy );
            RetrieveNextToken( content, dummy );
            RetrieveNextToken( content, dummy );
            RetrieveNextToken( content, dummy );

            RMobilePhone::TMobilePhoneSubscriberId subscriberId;

            RetrieveNextToken( content, subscriberId );

            if ( subscriberId.Length() >= KMCCValueMaxLength )
                {
                aMcc = subscriberId.Left( KMCCValueMaxLength );
                }
        	}
        }

#endif // __WINS__

    }

// ----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetMCCValueFromSIML
// ----------------------------------------------------------------------------
// 
void CFSMailBrandManagerImpl::GetMCCValueFromSIML( TDes& aMcc ) const
    {
    NM_FUNCTION;
    
    RTelServer telServer;

    CleanupClosePushL( telServer );

    User::LeaveIfError( telServer.Connect() );

    User::LeaveIfError( telServer.LoadPhoneModule( KMmTsyModuleName ) );
    
    RMobilePhone mobilePhone;

    CleanupClosePushL( mobilePhone );

    User::LeaveIfError( mobilePhone.Open( telServer, KMmTsyPhoneName ) );
    
    TRequestStatus status;

    RMobilePhone::TMobilePhoneSubscriberId subscriberId;       

    mobilePhone.GetSubscriberId( status, subscriberId );
    
    User::WaitForRequest( status );

    User::LeaveIfError( status.Int() );
    
    CleanupStack::PopAndDestroy( &mobilePhone );

    CleanupStack::PopAndDestroy( &telServer );

    if ( subscriberId.Length() >= KMCCValueMaxLength )
        {
        aMcc = subscriberId.Left( KMCCValueMaxLength );
        }
    }

// ----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::GetCurrentCountryL
// ----------------------------------------------------------------------------
// 
TUint8 CFSMailBrandManagerImpl::GetCurrentCountryL() const
    {
    NM_FUNCTION;
    
    CTzLocalizer* localizer = CTzLocalizer::NewLC();

    CTzLocalizedCity* city = localizer->GetFrequentlyUsedZoneCityL(
        CTzLocalizedTimeZone::ECurrentZone );
        
    CleanupStack::PushL( city );

    CTzLocalizedCityGroup* cityGroup = 
        localizer->GetCityGroupL( city->GroupId() );

    TUint8 countryId = cityGroup->Id();
    
    delete cityGroup;
    cityGroup = NULL;
    
    CleanupStack::PopAndDestroy( 2, localizer );
    
    return countryId;
    }

// -----------------------------------------------------------------------------
// CFSMailBrandManagerImpl::VerifyMailAccountName
// -----------------------------------------------------------------------------
void CFSMailBrandManagerImpl::VerifyMailAccountName(
        TPtrC& aBrandedName ) const
    {
    NM_FUNCTION;
    
    // Due to legal reasons we don't show brand name "Gmail" in Germany and UK
    if ( !aBrandedName.CompareF( KBrandNameGmail ) )
        {
        // First check timezone id
        TUint8 timeZone = 0;
        
        TRAPD( err, timeZone = GetCurrentCountryL() );
        
        if ( err == KErrNone && ( timeZone == KGermanyTzId ||
                                  timeZone == KUKTzId ) )
            {
            aBrandedName.Set( KBrandNameGoogleMail );
            }
        // Then if necessary check MCC
        else
            {
            TBuf<KMCCValueMaxLength> mcc;

            TRAPD( err2, GetMCCValueL( mcc ) );
            
            if ( err2 == KErrNone && ( mcc == KMCCGermany ||
                                       mcc == KMCCUK1 ||
                                       mcc == KMCCUK2 ) )
                {
                aBrandedName.Set( KBrandNameGoogleMail );
                }
            }

        }
    }
// </gmail_brand_issue>
