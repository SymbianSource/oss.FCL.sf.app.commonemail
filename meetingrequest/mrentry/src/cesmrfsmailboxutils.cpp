/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ESMR FS mailbox utilities implementation
*
*/


#include "cesmrfsmailboxutils.h"

#include "esmrhelper.h"
#include "esmrinternaluid.h"

#include "cfsmailclient.h"
#include "cfsmailbox.h"
#include "cmrcalendarinfo.h"

#include <calentry.h>
#include <caluser.h>
#include <ct/rcpointerarray.h>

#include "emailtrace.h"

// Unnamed namespace for local definitions
namespace {

// Definition for email address comparison
const TInt KEqualEmailAddress( 0 );

/**
 * Listes all FS mailboxes. On return aMailboxes contains FS mailboxes
 * @param aMailClient FS mail client
 * @aMailboxes Reference to mailboxes
 */
void ListMailBoxesL(
        CFSMailClient& aMailClient,
        RPointerArray<CFSMailBox>& aMailboxes )
    {
    TFSMailMsgId msgId;
    aMailClient.ListMailBoxes(
            msgId,
            aMailboxes );
    }

/**
 * Helper class for Email extension cleanup
 */
class TExtensionCleanup
    {
    public:

    TExtensionCleanup(
            CFSMailBox* aMailbox,
            CEmailExtension* aExtension )
        : iMailbox( aMailbox ),
          iExtension( aExtension )
        {
        }

    void Close()
        {
        iMailbox->ReleaseExtension( iExtension );
        }

    private:
        /// Not own: mailbox
        CFSMailBox* iMailbox;
        // Not own: mailbox extension
        CEmailExtension* iExtension;
    };


}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRFsMailboxUtils::CESMRFsMailboxUtils
// ---------------------------------------------------------------------------
//
inline CESMRFsMailboxUtils::CESMRFsMailboxUtils(
        CMRMailboxUtils& aMailboxUtils )
:   iMRMailboxUtils( aMailboxUtils )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CESMRFsMailboxUtils::~CESMRFsMailboxUtils
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFsMailboxUtils::~CESMRFsMailboxUtils()
    {
    FUNC_LOG;
    if ( iMailClient )
        {
        iMailClient->Close();
        }
    }

// ---------------------------------------------------------------------------
// CESMRFsMailboxUtils::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRFsMailboxUtils* CESMRFsMailboxUtils::NewL(
            CMRMailboxUtils& aMailboxUtils )
    {
    FUNC_LOG;
    CESMRFsMailboxUtils* self =
            new (ELeave) CESMRFsMailboxUtils( aMailboxUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRFsMailboxUtils::ConstructL
// ---------------------------------------------------------------------------
//
void CESMRFsMailboxUtils::ConstructL()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRFsMailboxUtils::SetPhoneOwnerL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CESMRFsMailboxUtils::SetPhoneOwnerL(
            CCalEntry& aCalEntry,
            TMsvId aPrimaryBox )
    {
    FUNC_LOG;
    TInt err( iMRMailboxUtils.SetPhoneOwnerL( aCalEntry, aPrimaryBox ) );

    // Check that phone owner was set correctly
    if ( err == KErrNotFound )
        {
        // Check if phone owner can be set using FS mailboxes
        // Loop throug all mailboxes in this plug-in
        RCPointerArray<CFSMailBox> mailboxes;
        CleanupClosePushL( mailboxes );

        ListMailBoxesL(
                MailClientL(),
                mailboxes );

        TInt dummyIndex( -1 ); // don't care
        CCalUser* phoneOwner = PhoneOwnerL( aCalEntry, mailboxes, dummyIndex );

        if ( phoneOwner )
            {
            aCalEntry.SetPhoneOwnerL( phoneOwner );
            err = KErrNone;
            }

        CleanupStack::PopAndDestroy( &mailboxes );
        }
    return err;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::SetPhoneOwnerL
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CESMRFsMailboxUtils::SetPhoneOwnerL(
    CCalEntry& aCalEntry,
    CFSMailClient& aMailClient,
    CFSMailMessage& aMailMessage )
    {
    FUNC_LOG;
    TInt err( KErrNotFound );

    TFSMailMsgId mailboxId( aMailMessage.GetMailBoxId() );
    CFSMailBox* mailbox = aMailClient.GetMailBoxByUidL( mailboxId );

    if ( !mailbox )
        {
        return KErrNotFound;
        }

    CleanupStack::PushL( mailbox );

    TPtrC mailboxOwnerAddName(
            mailbox->OwnMailAddress().GetEmailAddress() );

    CCalUser* po = NULL;
    CCalUser* organizer = aCalEntry.OrganizerL();

    if ( organizer &&
         KEqualEmailAddress == organizer->Address().CompareF(mailboxOwnerAddName) )
        {
        po = organizer;
        }
    else
        {
        RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();
        TInt attendeeCount( attendees.Count() );

        for( TInt i(0); i < attendeeCount && !po; ++i )
            {
            CCalAttendee* attendee = attendees[i];
            TPtrC attendeeAddr( attendee->Address() );
            if ( KEqualEmailAddress == attendeeAddr.CompareF(mailboxOwnerAddName) )
                {
                po = attendee;
                }
            }
        }

    if ( po )
        {
        aCalEntry.SetPhoneOwnerL( po );
        err = KErrNone;
        }

    CleanupStack::PopAndDestroy( mailbox );

    return err;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::FSEmailPluginForEntryL
// ----------------------------------------------------------------------------
//
EXPORT_C TESMRMailPlugin CESMRFsMailboxUtils::FSEmailPluginForEntryL(
        const CCalEntry& aEntry )
    {
    FUNC_LOG;
    RCPointerArray<CFSMailBox> mailboxes;
    CleanupClosePushL( mailboxes );

    ListMailBoxesL(
            MailClientL(),
            mailboxes );

    CCalUser* phoneOwner = aEntry.PhoneOwnerL();
    TPtrC poMailAddress( phoneOwner->Address() );

    CFSMailBox* fsEmailMailbox = NULL;
    TInt mailboxCount( mailboxes.Count() );
    for (TInt j(0); j < mailboxCount && !fsEmailMailbox; ++j )
        {
        TPtrC mailboxOwnerAddName(
                mailboxes[j]->OwnMailAddress().GetEmailAddress() );

        if ( KEqualEmailAddress == mailboxOwnerAddName.CompareF(poMailAddress) )
            {
            // Correct mailbox is found
            fsEmailMailbox = mailboxes[j];
            }
        }

    TESMRMailPlugin plugin( EESMRUnknownPlugin );

    if ( fsEmailMailbox )
        {
        // Mailbox was found --> We need to resolve mailboxes plug-in
        TFSMailMsgId mailboxId( fsEmailMailbox->GetId( ) );
        TUid pluginId( mailboxId.PluginId() );

        if ( TUid::Uid( KEasFreestylePlugin ) == pluginId )
            {
            plugin = EESMRActiveSync;
            }
        else if ( TUid::Uid( KIntellisync ) == pluginId )
            {
            plugin = EESMRIntelliSync;
            }
        else if ( TUid::Uid( KFSEmailPop3 ) == pluginId ||
                TUid::Uid( KFSEmailImap4 ) == pluginId )
            {
            plugin = EESMRImapPop;
            }
        }

    CleanupStack::PopAndDestroy( &mailboxes );
    return plugin;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::DefaultMailboxSupportCapabilityL
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CESMRFsMailboxUtils::DefaultMailboxSupportCapabilityL(
        CESMRFsMailboxUtils::TMRMailboxCapability aCapability )
    {
    FUNC_LOG;

    TBool retValue( EFalse );

    CFSMailBox* defaultMailbox = DefaultMailboxL();
    CleanupStack::PushL( defaultMailbox );
    ASSERT( defaultMailbox );

    switch ( aCapability )
        {
        case CESMRFsMailboxUtils::EMRCapabilityAttachment:
            {
            retValue = defaultMailbox->HasCapability(
                            EFSMboxCapaSupportsAttahmentsInMR );
            }
            break;
        }

    CleanupStack::PopAndDestroy( defaultMailbox );

    return retValue;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::GetCalendarDatabaseIdL
// ----------------------------------------------------------------------------
//
void CESMRFsMailboxUtils::GetCalendarDatabaseIdL(
            TESMRMailPlugin aPlugin,
            TCalFileId& aDbId )
    {
    FUNC_LOG;

    TUid pluginUid( TUid::Null() );
    aDbId = KNullFileId;

    switch ( aPlugin )
        {
        case EESMRActiveSync:
            {
            pluginUid = TUid::Uid( KEasFreestylePlugin );
            break;
            }
        case EESMRIntelliSync:
            {
            pluginUid = TUid::Uid( KIntellisync );
            break;
            }
        default:
            {
            break;
            }
        }

    if ( pluginUid != TUid::Null() )
        {
        RCPointerArray<CFSMailBox> mailboxes;
        CleanupClosePushL( mailboxes );

        ListMailBoxesL(
                MailClientL(),
                mailboxes );

        for ( TInt i = 0; i < mailboxes.Count(); ++i )
            {
            CFSMailBox* mailbox = mailboxes[ i ];
            if ( mailbox->GetId().PluginId() == pluginUid )
                {
                // Resolve database id using mailbox extension
                CEmailExtension* extension =
                        mailbox->ExtensionL( KMailboxExtMrCalInfo );

                TExtensionCleanup cleanup( mailbox, extension );
                CleanupClosePushL( cleanup );

                CMRCalendarInfo* calInfo =
                        static_cast< CMRCalendarInfo* >( extension );

                if ( calInfo )
                    {
                    calInfo->GetCalendarDatabaseIdL( aDbId );
                    }

                CleanupStack::PopAndDestroy( &cleanup ); // Release extension
                break;
                }
            }

        CleanupStack::PopAndDestroy( &mailboxes );
        }
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::PhoneOwnerL
// ----------------------------------------------------------------------------
//
CCalUser* CESMRFsMailboxUtils::PhoneOwnerL(
    const CCalEntry& aCalEntry,
    const RPointerArray<CFSMailBox>& aMailBoxes,
    TInt& aMatchIndex )
    {
    FUNC_LOG;
    aMatchIndex = -1; // no match yet

    // 1. test if phone owner is organizer:
    CCalUser* organizer = aCalEntry.OrganizerL(); // ownership not transf.
    if ( organizer )
        {
        if ( IsPhoneOwnerL( *organizer, aMailBoxes, aMatchIndex ) )
            {
            return organizer;
            }
        }

    // 2. test if phone owner is attendee:
    RPointerArray<CCalAttendee>& attendees = aCalEntry.AttendeesL();
    TInt attCount( attendees.Count() );
    for ( TInt i( 0 );i < attCount; ++i )
        {
        CCalUser* att = static_cast<CCalUser*>(attendees[i]);
        if ( IsPhoneOwnerL( *att, aMailBoxes, aMatchIndex ) )
            {
            return att;
            }
        }

    // Matching attendee was not found:
    return NULL;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::IsPhoneOwnerL
// ----------------------------------------------------------------------------
//
TBool CESMRFsMailboxUtils::IsPhoneOwnerL(
    const CCalUser& aUser,
    const RPointerArray<CFSMailBox>& aMailBoxes,
    TInt& aMatchIndex )
    {
    FUNC_LOG;
    TPtrC addr = ESMRHelper::AddressWithoutMailtoPrefix( aUser.Address() );
    aMatchIndex = -1; // no match yet
    TInt boxCount( aMailBoxes.Count() );
    for ( TInt i( 0 ); i < boxCount; ++i )
        {
        TPtrC mailboxOwnerAddName(
                aMailBoxes[i]->OwnMailAddress().GetEmailAddress() );

        if ( addr.CompareF( mailboxOwnerAddName ) == KEqualEmailAddress )
            {
            // first match is enough:
            aMatchIndex = i;
            return ETrue;
            }
        }
    return EFalse; // no match
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::MailClientL
// ----------------------------------------------------------------------------
//
CFSMailClient& CESMRFsMailboxUtils::MailClientL()
    {
    FUNC_LOG;
    if ( !iMailClient )
        {
        iMailClient = CFSMailClient::NewL();
        }
    return *iMailClient;
    }

// ----------------------------------------------------------------------------
// CESMRFsMailboxUtils::DefaultMailboxL
// ----------------------------------------------------------------------------
//
CFSMailBox* CESMRFsMailboxUtils::DefaultMailboxL()
    {
    FUNC_LOG;

    CFSMailBox* defaultMailbox( NULL );

    CMRMailboxUtils::TMailboxInfo mailboxInfo;
    TInt err = iMRMailboxUtils.GetDefaultMRMailBoxL( mailboxInfo );

    if ( KErrNone == err )
        {
        RCPointerArray<CFSMailBox> mailboxes;
        CleanupClosePushL( mailboxes );

        ListMailBoxesL( MailClientL(), mailboxes );

        TInt mailboxCount( mailboxes.Count() );
        for (TInt j(0); j < mailboxCount && !defaultMailbox; ++j )
            {
            TPtrC mailboxOwnerAddName(
                    mailboxes[j]->OwnMailAddress().GetEmailAddress() );

            if ( KEqualEmailAddress ==
                 mailboxOwnerAddName.CompareF( mailboxInfo.iEmailAddress) )
                {
                // Default mailbox is found
                defaultMailbox = mailboxes[j];
                mailboxes.Remove( j );
                }
            }

        CleanupStack::PopAndDestroy( &mailboxes );
        }

    return defaultMailbox;
    }

// EOF

