/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  3rd party api testplugin mailbox data
*
*/

#include <memaildata.h>
#include <memailmailboxdata.h>
#include <memailmessagedata.h>

#include "emailtrace.h"
#include "mailboxdataimpl.h"
#include "messagedataimpl.h"
#include "t_testpluginconst.hrh"
#include "applaunchdataimpl.h"

// Hardcoded default values

// Path to graphics file with icon/mask numbers (client must provide separate file with brand graphics)
_LIT( KDefaultBrandingIcon, "mif(Z:\\resource\\apps\\google_brand_graphics.mif 16384 16385)" );
_LIT( KDefaultName, "HARDCODED" );           // default mailbox name
const TInt KDefaultMailboxId = 12345;
const TUid KDefaultAppUid  = { 0x200009EE }; // App to be launched from widget
const TUid KDefaultViewUid = { 0x00000001 }; // View to be activated from widget
const TBool KDefaultIsOutboxEmpty( ETrue );
const TBool KDefaultUnseen( EFalse );

// ---------------------------------------------------------
// ---------------------------------------------------------
CMailboxDataImpl* CMailboxDataImpl::NewL()
    {
    FUNC_LOG;
    CMailboxDataImpl* self = new(ELeave) CMailboxDataImpl();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CMailboxDataImpl::~CMailboxDataImpl()
    {
    delete iName;

    for ( TInt i = 0; i < iMessages.Count(); i++ )
        {
        delete static_cast<CMessageDataImpl*>(iMessages[i]);
        iMessages.Remove(i);
        }
    iMessages.ResetAndDestroy();
    delete iAppLaunchParams;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
CMailboxDataImpl::CMailboxDataImpl() :
    iName( NULL ),
    iBrandingIcon( NULL),
    iIsOutboxEmpty( KDefaultIsOutboxEmpty ),
    iUnseen( KDefaultUnseen ),
    iAppLaunchParams( NULL )
    {
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
void CMailboxDataImpl::ConstructL()
    {
    iAppLaunchParams = CAppLaunchDataImpl::NewL( KDefaultAppUid, KDefaultViewUid );
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TUint CMailboxDataImpl::MailboxId() const
    {
    FUNC_LOG;
    return KDefaultMailboxId;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const TDesC& CMailboxDataImpl::Name() const
    {
    FUNC_LOG;
    return KDefaultName;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const TDesC& CMailboxDataImpl::BrandingIcon() const
    {
    FUNC_LOG;
    return KDefaultBrandingIcon;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMailboxDataImpl::IsOutboxEmpty() const
    {
    FUNC_LOG;
    return iIsOutboxEmpty;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
const RPointerArray<EmailInterface::MMessageData>& CMailboxDataImpl::LatestMessagesL() const
    {
    FUNC_LOG;
    return iMessages;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TInt CMailboxDataImpl::MessageCount() const
    {
    return iMessages.Count();
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
TBool CMailboxDataImpl::Unseen() const
    {
    FUNC_LOG;
    return iUnseen;
    }

// ---------------------------------------------------------
// ---------------------------------------------------------
EmailInterface::MEmailLaunchParameters& CMailboxDataImpl::LaunchParameters() const
    {
    FUNC_LOG;
    return *( static_cast<EmailInterface::MEmailLaunchParameters*>( iAppLaunchParams ) );
    }
