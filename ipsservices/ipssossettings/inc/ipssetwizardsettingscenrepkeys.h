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
* Description:  Contains central repository related defines.
*
*/


#ifndef IPSSETWIZARDSETTINGSCENREPKEYS_H
#define IPSSETWIZARDSETTINGSCENREPKEYS_H

//Wizard settings cenrep uid
const TUid KCRUidWizardSettings = {0x2000B112};

//default values for the cenrep keys below:
_LIT( KCRKSetupWizAccountTypeDefault, "" );
const TInt KCRKAccountsConfiguredDefault = KErrNotFound;
const TInt KCRKMaxAccountsReachedDefault = KErrNotFound;
_LIT( KCRKPopImapEmailAddressIdDefault, "" );
_LIT( KCRKPopImapUsernameIdDefault, "" );
_LIT( KCRKPopImapPasswordIdDefault, "" );
_LIT( KCRKPopImapIncomingServerIdDefault, "" );
const TInt KCRKPopImapProtocolIndicatorIdDefault = KErrNotFound;
const TInt KCRKPopImapIncomingSecurityAuthIdDefault = KErrNotFound;
const TInt KCRKPopImapIncomingSecurityProtocolIdDefault = KErrNotFound;
const TInt KCRKPopImapIncomingPortIdDefault = KErrNotFound;
_LIT( KCRKPopImapOutgoingServerIdDefault, "" );
const TInt KCRKPopImapOutgoingSecurityAuthIdDefault = KErrNotFound;                      
const TInt KCRKPopImapOutgoingSecurityProtocolIdDefault = KErrNotFound;
const TInt KCRKPopImapOutgoingPortIdDefault = KErrNotFound;
_LIT( KCRKPopImapOperatorOutgoingServerIdDefault, "" );
const TInt KCRKPopImapOperatorSecurityAuthIdDefault = KErrNotFound;
const TInt KCRKPopImapOperatorSecurityProtocolIdDefault = KErrNotFound;
const TInt KCRKPopImapOperatorPortIdDefault = KErrNotFound;
_LIT( KCRKPopImapAccessPointIdDefault, "");
_LIT( KCRKPopImapMailboxNameDefault, "");
const TInt KCRKPopImapDataHideDefault = EFalse;


// cen rep keys, where wizard stores email setting values 
// these enumerations are taken from ods wizard,
// see ODSUtilityConstants.h under ODSUtility
enum TIpsSetWizardSettings
    {
    ECRKSetupWizAccountType = 0x00000001,
    ECRKSetupWizCentrepStartKey = 0x00000002,
    
    ECRKAccountsConfigured = 0x00000040,
    ECRKMaxAccountsReached = 0x00000041,

    ECRKPOPIMAPStartID = 0x00040000,
    ECRKPopImapEmailAddressId = 0x00040001,
    ECRKPopImapUsernameId = 0x00040002,
    ECRKPopImapPasswordId = 0x00040003,
    ECRKPopImapIncomingServerId = 0x00040004,
    
    // protocol POP/IMAP
    ECRKPopImapProtocolIndicatorId = 0x00040005,
    ECRKPopImapIncomingSecurityAuthId = 0x00040006,
    ECRKPopImapIncomingSecurityProtocolId = 0x00040007,
    
    // in port, 0 = default, > 0 = port number
    ECRKPopImapIncomingPortId = 0x00040008,
    ECRKPopImapOutgoingServerId = 0x00040009,
    ECRKPopImapOutgoingSecurityAuthId = 0x0004000A,                      
    ECRKPopImapOutgoingSecurityProtocolId = 0x0004000B,
    
    // out port, 0 = default, > 0 = port number
    ECRKPopImapOutgoingPortId = 0x0004000C,
    ECRKPopImapOperatorOutgoingServerId = 0x0004000D,
    ECRKPopImapOperatorSecurityAuthId = 0x0004000E,                      
    ECRKPopImapOperatorSecurityProtocolId = 0x0004000F,
    ECRKPopImapOperatorPortId = 0x00040010,
    ECRKPopImapAccessPointId = 0x00040011,
    ECRKPopImapMailboxName = 0x00040012,
    //<cmail>
    ECRKHideUsernameInSettings = 0x00040013,
    //</cmail>
    };

            
#endif // IPSSETWIZARDSETTINGSCENREPKEYS_H
