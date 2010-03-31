/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MBUtils settings implementation
*
*/

//INCLUDE FILES
#include "cmrmbutilsmailboxlistener.h"
#include "CFSMailCommon.h"
#include "CFSMailClient.h"
#include "CFSMailBox.h"
#include "fsmtmsuids.h"
#include "emailtrace.h"

#include <ct/rcpointerarray.h>
#include <cmrmailboxinfo.h>
#include <cmrmailboxutils.h>

// CONSTANTS

// Unnamed namespace for local definitions
namespace {   // codescanner::namespace
// Length for CRC buffer
const TInt KCRCBufferLen(32);
}

// ======== MEMBER FUNCTIONS ========


// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::CMRMBUtilsMailboxListener
// ----------------------------------------------------------------------------
//
CMRMBUtilsMailboxListener::CMRMBUtilsMailboxListener(
			RPointerArray<CMRMailBoxInfo>& aMailboxes,
			CFSMailClient& aMailClient,
			MMRMailboxUtilsObserver& aMRMailboxObserver )
:	iMailboxes( aMailboxes ),
    iMailClient( aMailClient ),
    iMRMailboxObserver( aMRMailboxObserver )
	{
	// No implementation
	}

// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::~CMRMBUtilsMailboxListener
// ----------------------------------------------------------------------------
//
CMRMBUtilsMailboxListener::~CMRMBUtilsMailboxListener()
	{
	// No implementation
	}

// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::NewL
// ----------------------------------------------------------------------------
//
CMRMBUtilsMailboxListener* CMRMBUtilsMailboxListener::NewL(
			RPointerArray<CMRMailBoxInfo>& aMailboxes,
			CFSMailClient& aMailClient,
			MMRMailboxUtilsObserver& aMRMailboxObserver )
	{
	FUNC_LOG;

	CMRMBUtilsMailboxListener* self =
			new (ELeave) CMRMBUtilsMailboxListener(
			        aMailboxes,
			        aMailClient,
			        aMRMailboxObserver );

	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::ConstructL
// ----------------------------------------------------------------------------
//
void CMRMBUtilsMailboxListener::ConstructL()
	{
	FUNC_LOG;

	ListMailboxesL();
	}

// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::EventL
// ----------------------------------------------------------------------------
//
void CMRMBUtilsMailboxListener::EventL(
			TFSMailEvent aEvent,
			TFSMailMsgId /*aMailbox*/,
			TAny* /*aParam1*/,
			TAny* /*aParam2*/,
			TAny* /*aParam3*/ )
	{
	FUNC_LOG;

	MMRMailboxUtilsObserver::TEventType event(
	        MMRMailboxUtilsObserver::EMRNullEvent);

	switch ( aEvent )
	    {
	    case TFSEventNewMailbox:
	        {
	        event = MMRMailboxUtilsObserver::EMRMailboxCreated;
	        }
	        break;

	    case TFSEventMailboxRenamed:
        case TFSEventMailboxCapabilityChanged:
        case TFSEventMailboxSettingsChanged:
            {
            event = MMRMailboxUtilsObserver::EMRMailboxCreated;
            }
            break;

	    case TFSEventMailboxDeleted:
	        {
	        event = MMRMailboxUtilsObserver::EMRMailboxDeleted;
	        }
	        break;

	    default:
	        {
	        // No actions for rest of the events
	        }
	        break;
	    }

	if ( MMRMailboxUtilsObserver::EMRNullEvent != event)
	    {
	    ListMailboxesL();
	    iMRMailboxObserver.HandleMRMailboxEventL( event );
	    }
	}

// ----------------------------------------------------------------------------
// CMRMBUtilsMailboxListener::ListMailboxesL
// ----------------------------------------------------------------------------
//
void CMRMBUtilsMailboxListener::ListMailboxesL()
    {
    FUNC_LOG;

    RCPointerArray<CFSMailBox> mailBoxes;
    CleanupClosePushL( mailBoxes );

    TFSMailMsgId msgId;
    iMailClient.ListMailBoxes( msgId, mailBoxes );

    iMailboxes.ResetAndDestroy();
    TInt mailboxCount( mailBoxes.Count() );
    for (TInt i(0); i < mailboxCount; ++i )
        {
        CFSMailBox* mailbox = mailBoxes[i];

        TBool supportRespond(
                mailbox->HasCapability( EFSMBoxCapaMeetingRequestRespond ) );

        TBool supportCreate(
                mailbox->HasCapability( EFSMBoxCapaMeetingRequestCreate ) );

        TBool supportUpdate(
                mailbox->HasCapability( EFSMBoxCapaCanUpdateMeetingRequest ) );

        TBool supportsMeetingRequest(
                supportRespond &&
                supportCreate &&
                supportUpdate );

        if ( supportsMeetingRequest )
            {
            // Mailbox supports meeting request creation
            TFSMailMsgId mailboxId( mailbox->GetId() );

            TBuf<KCRCBufferLen> crcBuffer;
            crcBuffer.AppendNum( mailboxId.PluginId().iUid );
            crcBuffer.AppendNum( mailboxId.Id() );

            TUint32 cmailMailboxCrc( 0 );
            Mem::Crc32( cmailMailboxCrc,
                    crcBuffer.Ptr(),
                    crcBuffer.Size() );

            TPtrC mailboxName( mailbox->GetName() );
            TPtrC mailboxUser( mailbox->OwnMailAddress().GetEmailAddress() );

            CMRMailBoxInfo* info = CMRMailBoxInfo::NewL(
                    mailboxName,
                    mailboxUser,
                    TUid::Uid( KUidMsgTypeFsMtmVal),
                    cmailMailboxCrc );

            CleanupStack::PushL( info );
            User::LeaveIfError( iMailboxes.Append( info ) );
            CleanupStack::Pop( info );
            }
        }

    // mailBoxes
    CleanupStack::PopAndDestroy(); // codescanner::cleanup
    }

// EOF
