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
* Description:  handles interface to CenRep settings
*
*/


#include <e32base.h>
#include <aknsettingitemlist.h>

#include "emailtrace.h"
#include "CFSMailClient.h"
#include "emailwidgetsettingsmailboxes.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::NewL
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsMailboxes* CEmailWidgetSettingsMailboxes::NewL()
    {
    FUNC_LOG;
    CEmailWidgetSettingsMailboxes* self =  new(ELeave) CEmailWidgetSettingsMailboxes();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::~CEmailWidgetSettingsMailboxes
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsMailboxes::~CEmailWidgetSettingsMailboxes()
    {
    FUNC_LOG;
    if ( iMailClient )
        {
        iMailClient->Close();
        }    
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::CEmailWidgetSettingsMailboxes
// ---------------------------------------------------------------------------
//
CEmailWidgetSettingsMailboxes::CEmailWidgetSettingsMailboxes()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::ConstructL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsMailboxes::ConstructL()
    {
    FUNC_LOG;    
    // Mail client for FS Email framework
    iMailClient = CFSMailClient::NewL();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::GetMailboxIdL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsMailboxes::GetMailboxIdL(TInt aNum, 
                                                  TFSMailMsgId& aId  )
    {
    FUNC_LOG;    
    // Mailboxes will be fetched to this array
    RPointerArray<CFSMailBox> mailBoxes;

    // List all mailboxes
    TFSMailMsgId plugin;
    iMailClient->ListMailBoxes( plugin, mailBoxes );

    aId = mailBoxes[aNum]->GetId();
    
    // Release allocated memory
    mailBoxes.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::GetMailboxNameL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsMailboxes::GetMailboxNameL(TInt aNum,
                                                    TDes& aMailboxName)
    {
    FUNC_LOG;
    // Mailboxes will be fetched to this array
    RPointerArray<CFSMailBox> mailBoxes;

    // List all mailboxes
    TFSMailMsgId plugin;
    iMailClient->ListMailBoxes( plugin, mailBoxes );

    TDesC& mailboxName( mailBoxes[aNum]->GetName() );

    TBuf<3>iconIndex;
    iconIndex.AppendNum(aNum+1);
    
    aMailboxName.Append(iconIndex);
    aMailboxName.Append(_L("\t"));
    aMailboxName.Append(mailboxName);
    aMailboxName.Append(_L("\t"));
    aMailboxName.Append(_L("\t"));    
       
    mailBoxes.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::GetDomainL
// ---------------------------------------------------------------------------
//
void CEmailWidgetSettingsMailboxes::GetDomainL(TInt aNum, TDes& aDomain)
    {
    FUNC_LOG;
    // Mailboxes will be fetched to this array
    RPointerArray<CFSMailBox> mailBoxes;

    // List all mailboxes
    TFSMailMsgId plugin;
    iMailClient->ListMailBoxes( plugin, mailBoxes );

    CFSMailAddress& ownMailAddress( mailBoxes[aNum]->OwnMailAddress( ) );
    TDesC& mailAddress( ownMailAddress.GetEmailAddress());

    aDomain.Append(mailAddress.Right(mailAddress.Length() - 
                                     mailAddress.FindC(_L("@"))-1));

    mailBoxes.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CEmailWidgetSettingsMailboxes::MailboxCount
// ---------------------------------------------------------------------------
//
TInt CEmailWidgetSettingsMailboxes::MailboxCount()
    {
    FUNC_LOG;
    // Mailboxes will be fetched to this array
    RPointerArray<CFSMailBox> mailBoxes;

    // List all mailboxes
    TFSMailMsgId plugin;
    iMailClient->ListMailBoxes( plugin, mailBoxes );
    
    TInt count = mailBoxes.Count();

    // Release allocated memory
    mailBoxes.ResetAndDestroy();
    
    return count;
    }
