#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Makefile for FreestyleEmailUiIcons
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

# <cmail> icons chnaged

TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=$(EPOCROOT)epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\FreestyleEmailUi.mif
HEADERFILENAME=$(HEADERDIR)\FreestyleEmailUi.MBG
SOURCEDIR=$(EPOCROOT)epoc32\s60\bitmaps

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /H$(HEADERFILENAME) \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_msg_read_replied_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_msg_read_delete.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read_attachments.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read_attachments_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read_attachments_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_read_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread_attachments.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread_attachments_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread_attachments_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_event_unread_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_delete.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_low.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_normal.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_high.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_marked_add.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_attachment.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_forwarded_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_forwarded.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_forwarded_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_forwarded_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_forwarded_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_forwarded_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_read_replied_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_sent.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_forwarded_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied_attach.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied_attach_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied_attach_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied_high_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_unread_replied_low_prio.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_attachment.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_navi_follow_up_complete.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_navi_follow_up.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_info.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_remote_lookup.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_inbox.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_grid_selector \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_list_selector.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_expand.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_collapse.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_drop_inbox.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_email_ctrlbar.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_search.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_arrow_right.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_arrow_left.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_sync.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_connecting.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_connected.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_disconnected.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_disconnected_error.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_disconnected_low_battery.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_drop_menu_bg.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_drop_onepc.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_email_content.svg \
	/c16,8 $(SOURCEDIR)\qgn_graf_cmail_email_header.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_accept.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_reject.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_tentative.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_add_to_contacts.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_cancel_download.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_open.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_download.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_download_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_save.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_save_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_add.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_cancel_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_delete_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_delete.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_view_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_call2.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_call_video.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_contact_detail.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_create_meeting.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_create_msg.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_delete.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_forward.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_mark_read.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_mark_unread.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_move_msg.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_move_to_drafts.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_remote_lookup.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_calendar_delete.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_reply_all.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_reply.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_view_calendar.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_folders_asc.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_folders_desc.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_drop_email_account.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_drop_more_folders.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_deleted_items.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_drafts.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_inbox.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_subfolders.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_outbox.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_sent.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_folders_server_folder.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_call.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_email.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_settings.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_new_mailbox.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_launcher_help.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_not_downloaded.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_pdf.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_ppt.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_rtf.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_unidentified.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_xls.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_doc.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_html.svg \
	/c16,8 $(SOURCEDIR)\qgn_indi_cmail_viewer_attach_image.svg \
	/c16,8 $(SOURCEDIR)\qgn_prop_cmail_wml_bookmark.svg \
	/c16,8 $(SOURCEDIR)\qgn_menu_browserng.svg \
	/c16,8 $(SOURCEDIR)\qgn_menu_intranet.svg \
	/c8,1 $(SOURCEDIR)\qgn_graf_cmail_blank.svg 
	
# </cmail>	

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing