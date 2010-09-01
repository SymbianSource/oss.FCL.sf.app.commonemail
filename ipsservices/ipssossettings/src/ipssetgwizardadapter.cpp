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
*  Description: This file implements class CIpsSetWizardAdapter.
*
*/


//INCLUDES
#include "emailtrace.h"
#include <e32base.h>
#include <SendUiConsts.h>
#include <centralrepository.h>      // CRepository
#include <AlwaysOnlineManagerClient.h>
#include <cmdestination.h> 

#include "ipssetwizardadapter.h"
#include "ipssetdatamanager.h"
#include "ipssetuiitemaccesspoint.h"
#include "ipssetutils.h"
#include "ipssetdataextension.h"


#include "ipssetdata.h"


// becuase of RD_IPS_AO_PLUGIN flag, can be removed
// when flag is removed
#include "ipsplgsosbaseplugin.hrh"

//CONSTANTS
const TInt KMaxMailboxes = 10;
const TInt KImapDefFoldersRetrieve = 50; // Default value for retrieve From other folders

// security enums in fs wizard cenrep cenrep
// Security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
const TInt KWizardDataSecurityOff = 0;
const TInt KWizardDataSecurityTLS = 1;
const TInt KWizardDataSecuritySSL = 2;

const TInt KWizardTCPMaxPort = 65535;

const TInt KWizardMaxUidValue = 50;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsSetWizardAdapter* CIpsSetWizardAdapter::NewL( 
    CRepository& aRepository, CMsvSession& aSession ) 
    {
    FUNC_LOG;
    CIpsSetWizardAdapter* self = CIpsSetWizardAdapter::NewLC( 
        aRepository, aSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsSetWizardAdapter* CIpsSetWizardAdapter::NewLC( 
    CRepository& aRepository, CMsvSession& aSession )
    {
    FUNC_LOG;
    CIpsSetWizardAdapter* self = new( 
        ELeave ) CIpsSetWizardAdapter( aRepository, aSession );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsSetWizardAdapter::~CIpsSetWizardAdapter()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsSetWizardAdapter::CIpsSetWizardAdapter( 
    CRepository& aRepository, CMsvSession& aSession ) 
    : iWizardCenRep( aRepository ), iSession( aSession ) 
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::ConstructL()
    {
    FUNC_LOG;
    ClearFields();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::ClearFields()
    {
    FUNC_LOG;
    iAccountNumber = 0;
    iMaxAccounts = 0;
    iEmailAddress.Zero();
    iUsername.Zero();
    iPassword.Zero();
    iIncomingServer.Zero();
    iProtocol = 0;
    iInSecurityAuth = 0;
    iInSecurityProtocol = 0;
    iInIncomingPort = 0;
    iProviderOutgoingServer.Zero();
    iProviderOutSecurityAuth = 0;
    iProviderOutSecurityProtocol = 0;
    iProviderOutgoingPort = 0;
    iOperatorOutgoingServer.Zero();
    iOperatorOutSecurityAuth = 0;
    iOperatorOutSecurityProtocol = 0;
    iOperatorOutgoingPort = 0;
    iHiddenData = EFalse;
    iRecomendedAP.Zero();
    iMailboxName.Zero();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::ReadWizardSettignsL()
    {
    FUNC_LOG;
    ClearFields();
    
    TIpsSetUtilsTextPlain tempText;
    TInt error = KErrNone;
     
    error = GetIntFromCenRepL( ECRKAccountsConfigured, iAccountNumber );
    if ( error != KErrNone )
        {
        iAccountNumber = KCRKAccountsConfiguredDefault;
        }
    
    error = GetIntFromCenRepL( ECRKMaxAccountsReached, iMaxAccounts );
    if ( error != KErrNone )
        {
        iMaxAccounts = KCRKMaxAccountsReachedDefault;
        }
    
    error = GetTextFromCenRepL( ECRKPopImapEmailAddressId, iEmailAddress );
    if ( error != KErrNone )
        {
        iEmailAddress = KCRKPopImapEmailAddressIdDefault;
        }
    
    tempText.Zero();
    error = GetTextFromCenRepL( ECRKPopImapUsernameId, tempText );
    if ( error != KErrNone )
        {
        tempText = KCRKPopImapUsernameIdDefault;
        }
    ChangeAsciiPrintable( tempText, iUsername );
    
    tempText.Zero();
    error = GetTextFromCenRepL( ECRKPopImapPasswordId, tempText );
    if ( error != KErrNone )
        {
        tempText = KCRKPopImapPasswordIdDefault;
        }
    ChangeAsciiPrintable( tempText, iPassword );
    
    tempText.Zero();
    error = GetTextFromCenRepL( ECRKPopImapIncomingServerId, tempText );
    if ( error != KErrNone )
        {
        tempText = KCRKPopImapIncomingServerIdDefault;
        }
    ChangeAsciiPrintable( tempText, iIncomingServer );
    
    error = GetIntFromCenRepL( ECRKPopImapProtocolIndicatorId, iProtocol );
    if ( error != KErrNone )
        {
        iProtocol = KCRKPopImapProtocolIndicatorIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapIncomingSecurityAuthId, iInSecurityAuth );
    if ( error != KErrNone )
        {
        iInSecurityAuth = KCRKPopImapIncomingSecurityAuthIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapIncomingSecurityProtocolId, 
            iInSecurityProtocol );
    if ( error != KErrNone )
        {
        iInSecurityProtocol = KCRKPopImapIncomingSecurityProtocolIdDefault;
        }
    
    error = GetIntFromCenRepL(  ECRKPopImapIncomingPortId, iInIncomingPort );
    if ( error != KErrNone )
        {
        iInIncomingPort = KCRKPopImapIncomingPortIdDefault;
        }

    tempText.Zero();
    error = GetTextFromCenRepL( ECRKPopImapOutgoingServerId, tempText );
    if ( error != KErrNone )
        {
        tempText = KCRKPopImapOutgoingServerIdDefault;
        }
    ChangeAsciiPrintable( tempText, iProviderOutgoingServer );
    
    error = GetIntFromCenRepL( ECRKPopImapOutgoingSecurityAuthId, 
            iProviderOutSecurityAuth );
    if ( error != KErrNone )
        {
        iProviderOutSecurityAuth = KCRKPopImapOutgoingSecurityAuthIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapOutgoingSecurityProtocolId, 
            iProviderOutSecurityProtocol );
    if ( error != KErrNone )
        {
        iProviderOutSecurityProtocol = KCRKPopImapOutgoingSecurityProtocolIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapOutgoingPortId, iProviderOutgoingPort );
    if ( error != KErrNone )
        {
        iProviderOutgoingPort = KCRKPopImapOutgoingPortIdDefault;
        }
    
    error = GetTextFromCenRepL( ECRKPopImapOperatorOutgoingServerId, 
            iOperatorOutgoingServer );
    if ( error != KErrNone )
        {
        iOperatorOutgoingServer = KCRKPopImapOperatorOutgoingServerIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapOperatorSecurityAuthId, 
            iOperatorOutSecurityAuth );
    if ( error != KErrNone )
        {
        iOperatorOutSecurityAuth = KCRKPopImapOperatorSecurityAuthIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapOperatorSecurityProtocolId, 
            iOperatorOutSecurityProtocol );
    if ( error != KErrNone )
        {
        iOperatorOutSecurityProtocol = KCRKPopImapOperatorSecurityProtocolIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKPopImapOperatorPortId,  
            iOperatorOutgoingPort );
    if ( error != KErrNone )
        {
        iOperatorOutgoingPort = KCRKPopImapOperatorPortIdDefault;
        }
    
    error = GetIntFromCenRepL( ECRKHideUsernameInSettings,  
            iHiddenData );
    if ( error != KErrNone )
        {
        iHiddenData = KCRKPopImapDataHideDefault;
        }
    
    error = GetTextFromCenRepL( ECRKPopImapAccessPointId, iRecomendedAP );
    if ( error != KErrNone )
        {
        iRecomendedAP = KCRKPopImapAccessPointIdDefault;
        }
        
    tempText.Zero();
    error = GetTextFromCenRepL( ECRKPopImapMailboxName, tempText );
    if ( error != KErrNone )
        {
        tempText = KCRKPopImapMailboxNameDefault;
        }
    ChangeUnicodePrintable( tempText, iMailboxName );
    
    EmptyWizardCenRep();
    } 
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsSetWizardAdapter::GetIntFromCenRepL( TIpsSetWizardSettings aSetting, 
                                              TInt& aVariable )
    {
    FUNC_LOG;
    if ( &iWizardCenRep )
        {
        return iWizardCenRep.Get( aSetting, aVariable );
        }
    return KErrNone;
    }
        
        
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsSetWizardAdapter::GetTextFromCenRepL( TIpsSetWizardSettings aSetting, 
                                 TIpsSetUtilsTextPlain& aVariable )
    {
    FUNC_LOG;
    if ( &iWizardCenRep )
        {
        return iWizardCenRep.Get( aSetting, aVariable );
        }
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::EmptyWizardCenRep()
    {
    FUNC_LOG;
    if ( &iWizardCenRep )
        {
        iWizardCenRep.Set( ECRKPopImapEmailAddressId, KNullDesC );
        iWizardCenRep.Set( ECRKPopImapUsernameId, KNullDesC );
        iWizardCenRep.Set( ECRKPopImapPasswordId, KNullDesC );
        iWizardCenRep.Set( ECRKPopImapIncomingServerId, KNullDesC );
        iWizardCenRep.Set( 
                ECRKPopImapProtocolIndicatorId, KErrNotFound );
        iWizardCenRep.Set( 
                ECRKPopImapIncomingSecurityAuthId, KErrNotFound );
        iWizardCenRep.Set( 
                ECRKPopImapIncomingSecurityProtocolId, KErrNotFound );
        iWizardCenRep.Set( ECRKPopImapIncomingPortId, KErrNotFound );
        iWizardCenRep.Set( ECRKPopImapOutgoingServerId, KNullDesC );
        iWizardCenRep.Set( 
                ECRKPopImapOutgoingSecurityAuthId, KErrNotFound );
        iWizardCenRep.Set( 
                ECRKPopImapOutgoingSecurityProtocolId, KErrNotFound );
        iWizardCenRep.Set( ECRKPopImapOutgoingPortId, KErrNotFound );
        iWizardCenRep.Set( 
                ECRKPopImapOperatorOutgoingServerId, KNullDesC );
        iWizardCenRep.Set( 
                ECRKPopImapOperatorSecurityAuthId, KErrNotFound );
        iWizardCenRep.Set( 
                ECRKPopImapOperatorSecurityProtocolId, KErrNotFound );
        iWizardCenRep.Set( ECRKPopImapOperatorPortId, KErrNotFound );
        iWizardCenRep.Set( ECRKPopImapAccessPointId, KNullDesC );  
        iWizardCenRep.Set( ECRKPopImapMailboxName, KNullDesC );  
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::SetSettingsL( CIpsSetData& aSetData )
    {
    FUNC_LOG;
    TImIAPChoice iap;
    TPortValue port;
    TSecurityValue security;
    
    aSetData.SetProtocol( 
        (iProtocol == 0) ? KSenduiMtmPop3Uid : KSenduiMtmImap4Uid );
        
    if(iProtocol == 0)// KSenduiMtmPop3Uid - for Pop account- default is Headers Only
        {
        aSetData.SetDownloadSizeL( CIpsSetData::EHeaders,KIpsSetDataHeadersOnly);
        }
    else 
        {
        aSetData.SetDownloadSizeL( CIpsSetData::EHeadersPlus, KIpsSetDataDefaultDownloadSizeKb );

        TInt retImFolders = aSetData.RetrieveLimit(CIpsSetData::EImap4Folders); //TRetrieveLimit
        if(retImFolders==-1)
            {
            TInt retPopInbox = aSetData.RetrieveLimit(CIpsSetData::EPop3Limit); 
            TInt retImInbox  = aSetData.RetrieveLimit(CIpsSetData::EImap4Inbox); 
            aSetData.SetRetrieveLimit(retPopInbox,retImInbox,KImapDefFoldersRetrieve);
            }
        }
    aSetData.SetEmailAddressL( iEmailAddress );
    aSetData.SetUserNameL( iUsername, iUsername );
    aSetData.SetUserPwdL( iPassword, iPassword );
    aSetData.SetMailServerL( iIncomingServer, iProviderOutgoingServer );
    
    security = SetSecurityValues( iInSecurityProtocol, iProviderOutSecurityProtocol );
    aSetData.SetSecurity( security.inSecurity, security.outSecurity );

    port = CheckPortValues( iProtocol, 
                            iInIncomingPort, 
                            iProviderOutgoingPort, 
                            security );
    aSetData.SetPort( port.inport, port.outport );                             
    
    aSetData.SetDataHide( iHiddenData );
    aSetData.SetMailboxName( CheckMailboxNamesL( iMailboxName ) );
    aSetData.SetUserAuthentication( CIpsSetData::EUseIncoming );

    iap = SetIAPByNameL( iRecomendedAP );
    
    TBool setIAP = ETrue;

    RCmManager cmmgr;
    cmmgr.OpenLC();
    TCmDefConnValue defConnValue;
    cmmgr.ReadDefConnL( defConnValue );

    //if IAP isn't set by SettingWizard, we try to get the SNAP.
    if ( iap.iIAP <= 0 )
        {
        //Use SNAP when we got it and it does contain access point.
        //For other situations as below, we set 'ask when need' or default IAP
        //1. defConnValue.iType == ECmDefConnAskOnce
        //2. Got SNAP, but it does not contain any access point
        //3. Got default IAP when defConnValue.iType == ECmDefConnConnectionMethod
        iap.iDialogPref = ECommDbDialogPrefPrompt;
        iap.iIAP = 0;        

        //Check default connection is destination or connection method
        if ( defConnValue.iType == ECmDefConnDestination )
            {
            //Get SNAP from cmmgr
            RCmDestination destination;
            CleanupClosePushL( destination );
            destination = cmmgr.DestinationL( defConnValue.iId );
            //check if SNAP has IAPs
            if ( destination.ConnectionMethodCount() > 0 )
                {
                //if SNAP contain access point, we use SNAP
                CImIAPPreferences* iapPref = CImIAPPreferences::NewLC();
                iapPref->SetSNAPL( defConnValue.iId );
              
                aSetData.SetIapL( *iapPref, *iapPref );
                CleanupStack::PopAndDestroy( iapPref );
                 
                setIAP = EFalse;
                }
                CleanupStack::PopAndDestroy( &destination );             
            }
        else if( defConnValue.iType == ECmDefConnConnectionMethod ) 
            {
            iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
            iap.iIAP = defConnValue.iId;
            }
        }
    else
        {
        // recommended IAP is defined
        // try setting SNAP that contains the IAP
        TBool found( EFalse );
        RArray< TUint32 > destArray;
        CleanupClosePushL( destArray );
     
        //Get list of all destinations
        cmmgr.AllDestinationsL( destArray );
     
        TInt arrayCount( destArray.Count() );

        CMManager::TSnapMetadataField metaFieldPurpose( CMManager::ESnapMetadataPurpose );
        CMManager::TSnapMetadataField metaFieldLocalized( CMManager::ESnapMetadataDestinationIsLocalised );

        //Traverse list of destinations
        for ( TInt i = 0; i < arrayCount && !found; ++i )
            {
            RCmDestination destination = cmmgr.DestinationL( destArray[ i ] );
            CleanupClosePushL( destination );
            
            TUint32  destinationPurpose = destination.MetadataL ( metaFieldPurpose );
            TUint32  destinationLocalized = destination.MetadataL ( metaFieldLocalized );
            
            TInt iapCount ( destination.ConnectionMethodCount() );
            if ( destinationPurpose == CMManager::ESnapPurposeInternet &&
                 destinationLocalized == CMManager::ELocalisedDestInternet && 
                 iapCount )
                {
                // found the Internet destination and it has at least one access point
                for ( TInt c=0;c < iapCount && !found; c++ )
                    {
                    RCmConnectionMethod connMethod = destination.ConnectionMethodL( c );
                    CleanupClosePushL( connMethod );
                    
                    TUint32 iapId = connMethod.GetIntAttributeL( CMManager::ECmIapId );
                    HBufC* connName = connMethod.GetStringAttributeL( CMManager::ECmName );
                    CleanupStack::PushL( connName );
                    CleanupStack::PopAndDestroy( connName ); 
                    if ( iapId == iap.iIAP )
                        {
                        CImIAPPreferences* iapPref = CImIAPPreferences::NewLC();
                        INFO_1( "Setting SNAP %d by IAP", destination.Id() )
                        iapPref->SetSNAPL( destination.Id());
                        aSetData.SetIapL( *iapPref, *iapPref );   
                        found = ETrue;
                        setIAP = EFalse;
                        CleanupStack::PopAndDestroy( iapPref );
                        }
                    CleanupStack::PopAndDestroy( &connMethod ); 
                    }
                }
            CleanupStack::PopAndDestroy( &destination );
            }
        CleanupStack::PopAndDestroy( &destArray );
        }    

        //If no valid IAP or destination, set zero.
        if ( setIAP )
            {
            INFO_1( "IAP is uncategorized, using IAP: %d", iap.iIAP )
            aSetData.SetIapL( iap, iap );                
            }
    CleanupStack::PopAndDestroy( &cmmgr );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TSecurityValue CIpsSetWizardAdapter::SetSecurityValues( 
    const TInt aInSecurity, const TInt aOutSecurity )
    {
    FUNC_LOG;
    TSecurityValue values;


    switch( aInSecurity )
        {
        case KWizardDataSecurityOff:
            {
            values.inSecurity = CIpsSetData::ESecurityOff;
            break;
            }
        case KWizardDataSecurityTLS:
            {
            values.inSecurity = CIpsSetData::EStartTls;
            break;
            }
        case KWizardDataSecuritySSL:
            {
            values.inSecurity = CIpsSetData::ESslTls;
            break;
            }
        default:
            {
            values.inSecurity = CIpsSetData::ESecurityOff;
            break;
            }
        }
        
    switch( aOutSecurity )
        {
        case KWizardDataSecurityOff:
            {
            values.outSecurity = CIpsSetData::ESecurityOff;
            break;
            }
        case KWizardDataSecurityTLS:
            {
            values.outSecurity = CIpsSetData::EStartTls;
            break;
            }
        case KWizardDataSecuritySSL:
            {
            values.outSecurity = CIpsSetData::ESslTls;
            break;
            }
        default:
            {
            values.outSecurity = CIpsSetData::ESecurityOff;
            break;
            }
        }
    return values;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TPortValue CIpsSetWizardAdapter::CheckPortValues( const TInt aProtocol, 
    const TInt aInport, const TInt aOutport, const TSecurityValue aSecurity )
    {
    FUNC_LOG;
    TPortValue values;
        
    if( aInport <= 0 || aInport > KWizardTCPMaxPort )
        {
        values.inport = IpsSetUtils::GetDefaultSecurityPort( 
            aSecurity.inSecurity, ETrue, aProtocol );
        }
    else
        {
        values.inport = aInport;
        }        
        
    if( aOutport <= 0 || aOutport > KWizardTCPMaxPort )
        {
        values.outport = IpsSetUtils::GetDefaultSecurityPort( 
            aSecurity.outSecurity, EFalse, aProtocol );
        }
    else
        {
        values.outport = aOutport;
        }
    return values;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TImIAPChoice CIpsSetWizardAdapter::SetIAPByNameL( TDes& aIAPName )
    {
    FUNC_LOG;
    TInt iapcount;
    TImIAPChoice iap;
    TBuf<KWizardMaxUidValue> buf;
    CIpsSetUiItemAccessPoint* ipsSetAP = CIpsSetUiItemAccessPoint::NewLC();
    //Initial IAP list
    ipsSetAP->InitializeSelectionL();
    
    iapcount = ipsSetAP->iIapList.Count();

    //Set default AP to always ask
    iap.iDialogPref = ECommDbDialogPrefPrompt;
    iap.iIAP = 0;   

    //run through all found access points
    TBool found( EFalse );
    for( TInt i = 0;!found &&  i < iapcount; i++ )
        {
        HBufC* name( ipsSetAP->AccessPointNameLC( i ) );
        // If AP stored in cenrep key as UID value, we need to do
        // comparison by UID also -> Used in TP wizard
        buf.Zero();
        buf.Num( ipsSetAP->iIapList[i] );
        if( ( name && aIAPName.Compare( name->Des() ) == 0 ) ||
                aIAPName.Compare( buf ) == 0 )
            {
            //AP found!
            found = ETrue;
            iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
            iap.iIAP = ipsSetAP->iIapList[i];
            }
        CleanupStack::PopAndDestroy( name );
        }
    CleanupStack::PopAndDestroy( ipsSetAP );
    return iap;
    }
    
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::CreateEmailAccountL( CIpsSetData& aSetData )
    {
    FUNC_LOG;
    TInt accountcount;
    
    CIpsSetDataManager* setManager = CIpsSetDataManager::NewLC( iSession );
    
    //create mailbox
    User::LeaveIfError( setManager->CreateEmailAccount( aSetData, EFalse ) );
    
    CEmailAccounts* accounts = CEmailAccounts::NewLC();    
    RArray<TSmtpAccount> RIpsSmtpArray;        
    accounts->GetSmtpAccountsL( RIpsSmtpArray );
    accountcount = RIpsSmtpArray.Count();

    //increase mailbox count
    if ( &iWizardCenRep )
        { 
        iWizardCenRep.Set( ECRKAccountsConfigured, accountcount );
        }
    
    if( accountcount >= KMaxMailboxes && &iWizardCenRep )
        {
        iWizardCenRep.Set( ECRKMaxAccountsReached, ETrue );
        }
   
    RIpsSmtpArray.Close();
    CleanupStack::PopAndDestroy( accounts );
    CleanupStack::PopAndDestroy( setManager );
    
    // notify ao about new mailbox
    TInt mailboxId = KErrNotFound;
    if ( aSetData.ExtendedSettings() )
        {
        mailboxId = aSetData.ExtendedSettings()->MailboxId();
        }
    
    RAlwaysOnlineClientSession aosession; 
    TInt err = aosession.Connect();
    CleanupClosePushL( aosession );
    if ( mailboxId != KErrNotFound && err == KErrNone )
        {
        TPckgBuf<TMsvId> mboxBuf( mailboxId );
        TRAP( err, aosession.RelayCommandL( 
                EServerAPIEmailTurnOn, 
               mboxBuf ) );
        }
    CleanupStack::PopAndDestroy( &aosession );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TDes& CIpsSetWizardAdapter::CheckMailboxNamesL( TDes& aName )
    {
    FUNC_LOG;
    RArray<TSmtpAccount> RIpsSmtpArray;
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TInt endNumber = 0; 
    
    accounts->GetSmtpAccountsL( RIpsSmtpArray );
    
    TIpsSetUtilsTextPlain tempName = aName;
    
    //Run through all mailboxes
    for ( TInt mailboxCount = RIpsSmtpArray.Count()-1; mailboxCount >= 0; mailboxCount-- )
        {
        TIpsSetUtilsTextPlain mailbox = 
            RIpsSmtpArray[mailboxCount].iSmtpAccountName;    
        
        if( !tempName.Compare( mailbox ) )
            {
            //if mailbox name already exists increase value after name
            tempName = aName;
            endNumber++;
            tempName.AppendNum( endNumber );
            //reset mailbox counter
            mailboxCount = RIpsSmtpArray.Count();
            }
        }
    aName = tempName;
    RIpsSmtpArray.Reset();
    CleanupStack::PopAndDestroy( accounts );
    return aName;
    }
 
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::HandleWizardDataL()
    {
    FUNC_LOG;
    // This leaves if reading is not successfull
    ReadWizardSettignsL();
    
    if( iMaxAccounts == EFalse )
        {
        //create settings
        CIpsSetData *setData = CIpsSetData::NewLC();
        SetSettingsL( *setData );

        //create mailbox
        CreateEmailAccountL( *setData ); 

        CleanupStack::PopAndDestroy( setData );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::ChangeAsciiPrintable(
        const TDesC16& aOrgText, TDes& aValText )
     {
    FUNC_LOG;
     aValText.Zero();
     const TInt KMax7BitValue = 126;
     const TInt KMin7BitValue = 32;

     const TInt length = aOrgText.Length();
     const TText16* pText = aOrgText.Ptr();

     const TInt valMax = aValText.MaxLength();
     TInt valIndex = 0;
     for(TInt i=0; i<length; i++)
        {
        if ( valIndex >= valMax )
            {
            break;
            }
        if  (!((pText[i] > KMax7BitValue) || (pText[i] < KMin7BitValue)) )
            {
            aValText.Append( pText[i] );
            ++valIndex;
            }
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsSetWizardAdapter::ChangeUnicodePrintable(
        const TDesC16& aOrgText, TDes& aValText )
    {
    FUNC_LOG;
    aValText.Zero();
    const TText16* pText = aOrgText.Ptr();
    const TInt orgLength = aOrgText.Length();
    
    const TInt valMax = aValText.MaxLength();
    TInt valIndex = 0;
    
    for(TInt i =0; i < orgLength ; ++i) 
      {
      if ( valIndex >= valMax )
          {
          break;
          }
      
      TChar aChar = pText[i]; 
      if ( aChar.IsPrint() ) 
         { 
         aValText.Append( aChar );
         } 
      } 
    } 


