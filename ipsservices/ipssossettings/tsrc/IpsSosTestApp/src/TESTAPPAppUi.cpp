/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
/*
* ============================================================================
*  Name     : CTestAppAppUi from TestAppAppUi.cpp
*  Part of  : TestApp
* ============================================================================
*/

// INCLUDE FILES
#include <ipssostestapp.rsg>

#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <e32property.h>
#include <msvstd.h>
#include <ImumDaSettingsData.h>
#include <ImumDaSettingsDatacollection.h>
#include <ImumDaMailboxServices.h>
#include <ImumDaMailboxUtilities.h>
#include <ImumDomainApi.h>
#include <Imumdasettingskeys.h>
#include <senduiconsts.h>
#include <cemailaccounts.h>
//#include <ODSUtility.h>

#include "TestAppAppUi.h"
#include "TestAppContainer.h"
#include "TestApp.hrh"
#include "ipssetdataapi.h"
#include "ipssetwizardadapter.h"

//#include "ODSConfigSettings.h"

const TUid KTestAppCRUidWizardSettings   = {0x2000B112};

const TInt KTestAppWizardDataPopIndicator = 0;
const TInt KTestAppWizardDataImapIndicator = 1;

const TInt KTestAppWizardDataSecurityOff = 0;
const TInt KTestAppWizardDataSecurityTLS = 1;
const TInt KTestAppWizardDataSecuritySSL = 2;

_LIT( KTestAppFsBasicPluginStr, "fsp" );


           

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CTestAppAppUi::LaunchPluginL()
// ----------------------------------------------------------
//
void CTestAppAppUi::LaunchPluginL()
    {
    CIpsSetDataApi::LaunchSettingsL( iMailboxId );
    } 

// ----------------------------------------------------------
// CTestAppAppUi::WriteSettingsL()
// ----------------------------------------------------------
//  
void CTestAppAppUi::HandleSessionEventL(
        TMsvSessionEvent /*aEvent*/, 
        TAny* /*aArg1*/, 
        TAny* /*aArg2*/, 
        TAny* /*aArg3*/ )
    {
    
    }

// ----------------------------------------------------------
// CTestAppAppUi::WriteSettingsL()
// ----------------------------------------------------------
//    
void CTestAppAppUi::WriteSettingsViaODSUtilityL()
    {

    }


// ----------------------------------------------------------
// CTestAppAppUi::WriteSettingsToWizardCenRepL()
// ----------------------------------------------------------
// 
void CTestAppAppUi::WriteSettingsToWizardCenRepL()
    {
    CRepository* repository;
    repository = CRepository::NewL( KTestAppCRUidWizardSettings );
    CleanupStack::PushL( repository );
    TInt error = KErrNone; 
    error = repository->Set( CIpsSetWizardAdapter::ECRKSetupWizAccountType, 
            KTestAppFsBasicPluginStr );
    
    error = repository->Set( CIpsSetWizardAdapter::ECRKMaxAccountsReached,
            EFalse );
    
    
    if ( iProtocol == KSenduiMtmImap4UidValue )
        {
        error = repository->Set( 
                CIpsSetWizardAdapter::ECRKPopImapProtocolIndicatorId,
                KTestAppWizardDataImapIndicator );
        }
    else if ( iProtocol == KSenduiMtmPop3UidValue )
        {
        error = repository->Set( 
                CIpsSetWizardAdapter::ECRKPopImapProtocolIndicatorId,
                KTestAppWizardDataPopIndicator );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapEmailAddressId,
            iEmailAddress );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapUsernameId,
            iUserName );
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapPasswordId,
            iPassword );
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapIncomingServerId,
            iIncomingServerAddress );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapIncomingSecurityAuthId, 0 );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapIncomingSecurityProtocolId,
            iIncomingSecurity );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapIncomingPortId,
            iIncomingPort );
    
    iOutgoingServerAddress.ZeroTerminate();
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapOutgoingServerId,
            iOutgoingServerAddress );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapOutgoingSecurityAuthId, 0 );

    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapOutgoingSecurityProtocolId,
            iOutgoingSecurity );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapOutgoingPortId,
            iOutgoingPort );

    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapAccessPointId,
            iAccessPoint );
    
    error = repository->Set( 
            CIpsSetWizardAdapter::ECRKPopImapMailboxName,
            iMailboxName );
    
    
    CMsvSession* msvSession = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( msvSession );
    CIpsSetDataApi* dataApi = CIpsSetDataApi::NewL( *msvSession );    
    CleanupStack::PushL( dataApi );
    
    error = dataApi->HandleMailboxCreation( TUid::Uid(iProtocol), *msvSession );
    
    CleanupStack::PopAndDestroy( 3, repository );
    }
// ----------------------------------------------------------
// CTestAppAppUi::WriteSettingsViaImumL()
// ----------------------------------------------------------
// 
void CTestAppAppUi::WriteSettingsViaImumL()
    {
    CImumDomainApi* api = CreateDomainApiLC(  );
    MImumDaMailboxServices& services = api->MailboxServicesL();

    CImumDaSettingsData* settings = services.CreateSettingsDataL( KNullUid );
    CleanupStack::PushL( settings );

    
    settings->SetAttr( TImumDaSettings::EKeyEmailAddress, iEmailAddress );
    settings->SetAttr( TImumDaSettings::EKeyMailboxName, iMailboxName ); 
    settings->SetAttr( TImumDaSettings::EKeyUserAlias, iUserAlias );
    
    TUid protocolUid = KSenduiMtmImap4Uid;  
    if ( TImumDaSettings::EValueProtocolPop3 == iProtocol )
        {
        protocolUid = KSenduiMtmPop3Uid;
        }
    MImumDaSettingsDataCollection& incoming = settings->AddSetL( protocolUid );

    incoming.SetAttr( TImumDaSettings::EKeyProtocol , iProtocol );
    incoming.SetAttr( TImumDaSettings::EKeyAccessPoint, iAccessPoint );
    incoming.SetAttr( TImumDaSettings::EKeyServer, iIncomingServerAddress );
    incoming.SetAttr( TImumDaSettings::EKeyUsername, iUserName );
    incoming.SetAttr( TImumDaSettings::EKeyPassword, iPassword );
    incoming.SetAttr( TImumDaSettings::EKeyPort, iIncomingPort );
    incoming.SetAttr( TImumDaSettings::EKeySecurity, iIncomingSecurity  ); 
   
    MImumDaSettingsDataCollection& smtpSettings = settings->AddSetL( KSenduiMtmSmtpUid );

    smtpSettings.SetAttr( TImumDaSettings::EKeyProtocol, TImumDaSettings::EValueProtocolSmtp );
    smtpSettings.SetAttr( TImumDaSettings::EKeyAccessPoint, iAccessPoint );
    smtpSettings.SetAttr( TImumDaSettings::EKeyServer, iOutgoingServerAddress );    
    smtpSettings.SetAttr( TImumDaSettings::EKeySecurity, iOutgoingSecurity );           
    smtpSettings.SetAttr( TImumDaSettings::EKeyPort, iOutgoingPort ); 
    
    TInt error( KErrNone ) ;              
    // this leaves if incorrect settings
    TRAP( error, iMailboxId = services.CreateMailboxL( *settings ) );   
    
    CleanupStack::PopAndDestroy( 2, api );
    if ( error == KErrNone )
        {
        // creation is done, save id
        SetRecentMsvIdL( iMailboxId ); 
        }
    else
        {
        User::Leave( error );
        }
        
    }

// ----------------------------------------------------------
// CTestAppAppUi::ReadSettingsL()
// ----------------------------------------------------------
//        
void CTestAppAppUi::ReadSettingsL( TInt aFileId )
    {
    TBuf<100> fileName( KIpsSosTestFilePath );
    fileName.Append( KIpsSosTestFileName );
    if ( KIpsSosTestDefaultFile != aFileId )
        {
        fileName.AppendNum( aFileId );
        }
    fileName.Append( KIpsSosTestFileExtension );
    // open the RFs
    RFs fileSession;
    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );   // << fileSession

    // open the RFileReadStream
    RFileReadStream fileReader;
    User::LeaveIfError( fileReader.Open( fileSession, fileName, EFileShareExclusive ) );
    CleanupClosePushL( fileReader );    // << fileReader

    TBuf8<200> lineBuffer;
    TChar delim( KIpsSosTestLineFeed );

    TInt error( KErrNone );
    while ( KErrNone == error ) // Expected KErrEof
        {
        TRAP( error, fileReader.ReadL( lineBuffer, delim ) );
        TInt length = lineBuffer.Length();
        if ( length )
            {
            // remove line feed
            lineBuffer.Delete( length - KIpsSosTestDelimeterSize, KIpsSosTestDelimeterSize );
            // remove spaces
            lineBuffer.TrimAll();
            // ignore comment lines
            TInt comment = lineBuffer.FindF( KIpsSosTestIniCommentLine );
            if ( KErrNotFound == comment )
                {
                // find keys & values
                TInt separator = lineBuffer.FindF( KIpsSosTestIniSeparator );
                if ( KErrNotFound != separator  )
                    {
                    TPtrC8 key( KNullDesC8 );
                    TPtrC8 value( KNullDesC8 );
                    key.Set( lineBuffer.Left( separator ) );
                    TInt valueStart( separator );
                    valueStart += KIpsSosTestIniSeparator().Length();
                    value.Set( lineBuffer.Mid( valueStart ) );

                    HBufC* value16 = HBufC::NewLC( value.Length() );
                    value16->Des().Copy( value );
                    // protocol
                    if ( !key.CompareF( KIpsSosTestProtocol ) )
                        {
                        if ( !value.CompareF( KIpsSosTestPOP ) )
                            {
                            iProtocol = KSenduiMtmImap4UidValue;
                            }
                        else
                            {
                            iProtocol = KSenduiMtmPop3UidValue; 
                            }
                        }
                    // mailbox name
                    else if ( !key.CompareF( KIpsSosTestMailboxName ) )
                        {
                        iMailboxName = value16->Des();
                        }
                    // email address
                    else if ( !key.CompareF( KIpsSosTestEmailAddress ) )
                        {
                        iEmailAddress =  value16->Des();
                        }
                    // user name
                    else if ( !key.CompareF( KIpsSosTestUserName ) )
                        {
                        iUserName = value16->Des();
                        }
                    // password
                    else if ( !key.CompareF( KIpsSosTestPassword ) )
                        {
                        iPassword = value16->Des();
                        }
                   // alias
                   else if ( !key.CompareF( KIpsSosTestUserAlias ) )
                        {
                        iUserAlias =  value16->Des();
                        }
                    // signature
                    else if ( !key.CompareF( KIpsSosTestSignature ) )
                        {
                        iSignature =  value16->Des();
                        }
                    // folder path
                    else if ( !key.CompareF( KIpsSosTestFolderPath ) )
                        {
                        iFolderPath = value16->Des();
                        }
                    // access point
                    else if ( !key.CompareF( KIpsSosTestAccessPoint ) )
                        {
                        if ( !value.CompareF( KIpsSosTestAPAlwaysAsk) )
                            {
                             
                            iAccessPoint = _L(""); 
                            }
                        else
                            {
                            iAccessPoint = value16->Des(); 
                            }
                        }
                    // incoming server
                    else if ( !key.CompareF( KIpsSosTestIncomingServerAddress ) )
                        {
                        iIncomingServerAddress = value16->Des();
                        } 
                    // incoming port                       
                    else if ( !key.CompareF( KIpsSosTestIncomingPort ) )
                        {
                        if ( !value.CompareF( KIpsSosTestPortValueDefault ) )
                            {
                            iIncomingPort = KIpsSosTestPortDefault; 
                            }
                        else
                            {
                            TLex8 lex( value );
                            TInt port = 0;
                            TInt error = lex.Val( port );
                            iIncomingPort = port; 
                            }
                        } 
                    // incoming security
                    else if ( !key.CompareF( KIpsSosTestIncomingSecurity ) )
                        {
                        if ( !value.CompareF( KIpsSosTestSecurityTls ) )
                            {
                            iIncomingSecurity  = KTestAppWizardDataSecurityTLS;
                            }
                        else if ( !value.CompareF( KIpsSosTestSecuritySsl ) )
                            {
                            iIncomingSecurity  = KTestAppWizardDataSecuritySSL;
                            }
                        else 
                            {
                            iIncomingSecurity  = KTestAppWizardDataSecurityOff;
                            }
                        } 
                    // outgoing server
                    else if ( !key.CompareF( KIpsSosTestOutgoingServerAddress ) )
                        {
                        iOutgoingServerAddress =  value16->Des();
                        } 
                    // outgoing port
                    else if ( !key.CompareF( KIpsSosTestOutgoingPort ) )
                        {
                        if ( !value.CompareF( KIpsSosTestPortValueDefault) )
                            {
                            iOutgoingPort = KIpsSosTestPortDefault;
                            }
                        else
                            {
                            TLex8 lex( value );
                            TInt port = 0;
                            TInt error = lex.Val( port );
                            iOutgoingPort = port; 
                            }
                        } 
                    // outgoing security
                    else if ( !key.CompareF( KIpsSosTestOutgoingSecurity ) )
                        {
                        if ( !value.CompareF( KIpsSosTestSecurityTls ) )
                            {
                            iOutgoingSecurity  = KTestAppWizardDataSecurityTLS;
                            }
                        else if ( !value.CompareF( KIpsSosTestSecuritySsl ) )
                            {
                            iOutgoingSecurity  = KTestAppWizardDataSecuritySSL;
                            }
                        else 
                            {
                            iOutgoingSecurity  = KTestAppWizardDataSecurityOff;
                            }
                        }
                    else
                        {
                        ShowNoteL( KIpsSosTestUnknownValue );
                        }
                    CleanupStack::PopAndDestroy( value16 );
                    }
                }
            }
        }
        
    // close the RFileReadStream
    CleanupStack::PopAndDestroy( &fileReader );     // >>> fileReader

    // close the RFs
    CleanupStack::PopAndDestroy( &fileSession );    // >>> fileSession

    } 
    
// ----------------------------------------------------------
// CTestAppAppUi::ShowNoteL()
// ----------------------------------------------------------
//    
void CTestAppAppUi::ShowNoteL( const TDesC& aMessage )
    {
    CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
    informationNote->ExecuteLD( aMessage );
    }

// ----------------------------------------------------------
// CTestAppAppUi::DeleteAllSettingsL()
// ----------------------------------------------------------
//            
void CTestAppAppUi::DeleteAllSettingsL( )
    {
    TInt error( KErrNone );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    // imap
    RArray<TImapAccount> imapArray;
    CleanupClosePushL( imapArray );
    accounts->GetImapAccountsL( imapArray );
    for ( TInt mailbox(0); mailbox < imapArray.Count(); mailbox++ )
        {
        TImapAccount imap = imapArray[mailbox];
        TRAP( error, accounts->DeleteImapAccountL( imap ) );
        }
    CleanupStack::PopAndDestroy( &imapArray );
    // pop
    RArray<TPopAccount> popArray;
    CleanupClosePushL( popArray );
    accounts->GetPopAccountsL( popArray );
    
    for ( TInt mailbox(0); mailbox < popArray.Count(); mailbox++ )
        {
        TPopAccount pop = popArray[mailbox];;
        TRAP( error, accounts->DeletePopAccountL( pop ) );
        }
    CleanupStack::PopAndDestroy( &popArray );
    // smtp
    RArray<TSmtpAccount> smtpArray;
    accounts->GetSmtpAccountsL( smtpArray );
    CleanupClosePushL( smtpArray );
    
    for ( TInt mailbox(0); mailbox < smtpArray.Count(); mailbox++ )
        {
        TSmtpAccount smtp = smtpArray[mailbox];
        TRAP( error, accounts->DeleteSmtpAccountL( smtp ) );
        }
    CleanupStack::PopAndDestroy( &smtpArray );
    
    CleanupStack::PopAndDestroy( accounts );
    }
    
// ----------------------------------------------------------
// CTestAppAppUi::ConstructL()
// ----------------------------------------------------------
//
void CTestAppAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    iAppContainer = new (ELeave) CTestAppContainer;
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CTestAppAppUi::~CTestAppAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CTestAppAppUi::~CTestAppAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    }

// ------------------------------------------------------------------------------
// CTestAppAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CTestAppAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* /*aMenuPane*/ )
    {
    switch( aResourceId )
        {
        case R_TESTAPP_MENU:
            {            
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------
// CTestAppAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CTestAppAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CTestAppAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CTestAppAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ETest1Cmd:
            {
            // read settings
            TInt error( KErrNone );
            TRAP( error, ReadSettingsL( KIpsSosTestDefaultFile ) );
            if ( KErrNone != error )
                {
                if ( KErrNotFound == error )
                    {
                    ShowNoteL( KIpsSosTestFileNotFound );
                    }
                else
                    {
                    ShowNoteL( KIpsSosTestErrorInReading );
                    }
                return;
                }
            // write settings
            TRAP( error, WriteSettingsToWizardCenRepL() );
            if ( KErrNone != error )
                {
                ShowNoteL( KIpsSosTestErrorInWriting );
                }
            else
                {
                ShowNoteL( KIpsSosTestSettingsCreated );
                }
            break;
            }   
        case ETest2Cmd:
            {
            // query
            TInt queryValue( KErrNotFound );
            CAknNumberQueryDialog* dlg = CAknNumberQueryDialog::NewL( queryValue, CAknQueryDialog::ENoTone );
            dlg->PrepareLC( R_NUMBER_QUERY_DIALOG );
            dlg->SetPromptL( _L("Give init file id") );
            TInt ret = dlg->RunLD();
            TInt error( KErrNone );
            // read settings
            TRAP( error, ReadSettingsL( queryValue ) );
            if ( KErrNone != error )
                {
                if ( KErrNotFound == error )
                    {
                    ShowNoteL( KIpsSosTestFileNotFound );
                    }
                else
                    {
                    ShowNoteL( KIpsSosTestErrorInReading );
                    }
                return;
                }
            // write settings
            TRAP( error, WriteSettingsToWizardCenRepL() );
            if ( KErrNone != error )
                {
                ShowNoteL( KIpsSosTestErrorInWriting );
                return;
                }
            // launch plugin
            TRAP( error, LaunchPluginL() ); 
            if ( KErrNone != error )
                {
                ShowNoteL( KIpsSosTestErrorLaunching );
                }     
            break;
            }
        case ETest3Cmd:
            {
            TMsvId recent = RecentMsvId(); 
            if ( KErrNotFound != recent )  
                {
                iMailboxId = recent;
                // launch plugin
                TInt error( KErrNone );
                TRAP( error, LaunchPluginL() ); 
                if ( KErrNone != error )
                    {
                    ShowNoteL( KIpsSosTestErrorLaunching );
                    }  
                }
            else
                {
                ShowNoteL( KIpsSosTestRecentNotFound );
                }
            break;
            }
        
        default:
            User::Leave( KErrArgument );
            break;
        }
    }
    
// ----------------------------------------------------------
// CTestAppAppUi::SetRecentMsvIdL()
// ----------------------------------------------------------
//   
void CTestAppAppUi::SetRecentMsvIdL( TMsvId aMsvId )
    {
    // define integer-type property
    TInt err = RProperty::Define( KIpsSosTestPropertyCat, EIpsSosTestRecentMsvId, RProperty::EInt );
    if ( KErrAlreadyExists != err && KErrNone != err )
        {
        User::Leave( err );
        }
    // publish property using category and key
    err = RProperty::Set( KIpsSosTestPropertyCat, EIpsSosTestRecentMsvId, aMsvId );
    }

// ----------------------------------------------------------
// CTestAppAppUi::RecentMsvId()
// ----------------------------------------------------------
//    
TMsvId CTestAppAppUi::RecentMsvId(  )
    {
    TInt recent( KErrNotFound );
    // get property using category and key
    TInt err = RProperty::Get( KIpsSosTestPropertyCat, EIpsSosTestRecentMsvId, recent );
    return recent;
    }
    
    
// End of File
