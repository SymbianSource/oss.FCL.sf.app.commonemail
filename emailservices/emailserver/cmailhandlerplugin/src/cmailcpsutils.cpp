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
* Description:  Class to handle content publishing for widget
*
*/

#include "emailtrace.h"
#include "cmailcpsutils.h"
#include "cmailhandlerplugin.mbg"

using namespace EmailInterface;

// ---------------------------------------------------------
// CMailCpsUtils::ResolveIndicatorIcon
// ---------------------------------------------------------
//
TInt TMailCpsUtils::ResolveIcon(
    TBool aUnread,
    TBool aCalMsg,
    TBool aAttas,
    EmailInterface::TEmailPriority aPrio,
    TBool aRe,
    TBool aFw )
    {
    FUNC_LOG;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_attach;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_attach_high_prio;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_attach_low_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ENormal && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ENormal && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == EHigh && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach_high_prio;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ELow && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_attach_low_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == EHigh && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_high_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ELow && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_forwarded_low_prio;    
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_high_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_low_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ENormal && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ENormal && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == EHigh && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach_high_prio;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ELow && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_attach_low_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ENormal && aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_forwarded;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ENormal && aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_forwarded_attach;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == EHigh && aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_forwarded_attach_high_prio;
    if ( !aUnread && !aCalMsg && aAttas && aPrio == ELow && aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_forwarded_attach_low_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == EHigh && aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_forwarded_high_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == EHigh && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_high_prio;
    if ( !aUnread && !aCalMsg && !aAttas && aPrio == ELow && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_read_replied_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach;
    if ( aUnread && !aCalMsg && aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach_high_prio;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_attach_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ENormal && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ENormal && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach;
    if ( aUnread && !aCalMsg && aAttas && aPrio == EHigh && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach_high_prio;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ELow && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_attach_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == EHigh && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_high_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ELow && !aRe && aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_forwarded_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_high_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ENormal && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ENormal && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach;
    if ( aUnread && !aCalMsg && aAttas && aPrio == EHigh && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach_high_prio;
    if ( aUnread && !aCalMsg && aAttas && aPrio == ELow && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_attach_low_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == EHigh && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_high_prio;
    if ( aUnread && !aCalMsg && !aAttas && aPrio == ELow && aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_unread_replied_low_prio;
    if ( aUnread && aCalMsg && !aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread;
    if ( aUnread && aCalMsg && aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments;
    if ( aUnread && aCalMsg && !aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_high_prio;
    if ( aUnread && aCalMsg && aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments_high_prio;
    if ( aUnread && aCalMsg && !aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_low_prio;
    if ( aUnread && aCalMsg && aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_unread_attachments_low_prio;
    if ( !aUnread && aCalMsg && !aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read;
    if ( !aUnread && aCalMsg && aAttas && aPrio == ENormal && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments;
    if ( !aUnread && aCalMsg && !aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_high_prio;
    if ( !aUnread && aCalMsg && aAttas && aPrio == EHigh && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments_high_prio;
    if ( !aUnread && aCalMsg && !aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_low_prio;
    if ( !aUnread && aCalMsg && aAttas && aPrio == ELow && !aRe && !aFw ) return EMbmCmailhandlerpluginQgn_indi_cmail_calendar_event_read_attachments_low_prio;

    // default
    return EMbmCmailhandlerpluginQgn_indi_cmail_read;
    }
