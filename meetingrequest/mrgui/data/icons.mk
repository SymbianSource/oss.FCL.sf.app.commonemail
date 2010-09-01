#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
#
# Contributors:
#
# Description: Make file for mrgui icons. This file is only for ABLD building! 
#

ifeq (WINS,$(findstring WINS,$(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=$(EPOCROOT)epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\esmrgui.mif
HEADERFILENAME=$(HEADERDIR)\esmrgui.mbg
# <cmail> icons changed
SOURCEDIR=$(EPOCROOT)epoc32\s60\icons
# </cmail>

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# <cmail> icons changed

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_high.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_normal.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_priority_low.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_alarm_day.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_alarm.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_subject.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_recurrence.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_alarm.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_checkbox_on.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_checkbox_off.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_clock.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_date_end.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_date_start.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_meet_req_location.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_synchronisation.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_occasion.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_tentative.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_accept.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_none.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_reject.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_attachment.svg \
            /c32,8 $(SOURCEDIR)\qgn_graf_cmail_email_ctrlbar.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_center.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_corner_bl.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_corner_br.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_corner_tl.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_corner_tr.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_side_b.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_side_l.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_side_r.svg \
            /c32,8 $(SOURCEDIR)\qsn_fr_list_side_t.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_action_arrow_right.svg \
            /c32,8 $(SOURCEDIR)\qgn_menu_browserng.svg \
            /c32,8 $(SOURCEDIR)\qgn_menu_intranet.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_wml_bookmark.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_cancel_download.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_open.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_download.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_download_all.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_save.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_save_all.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_cmail_action_attachment_list_options_view_all.svg \ 
            /c32,8 $(SOURCEDIR)\qgn_prop_search_contacts.svg \
            /c32,8 $(SOURCEDIR)\qgn_prop_nrtyp_date.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cdr_meeting.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cdr_invitation.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cdr_reminder.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cdr_birthday.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cdr_todo.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_arrow_previous.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_cmail_arrow_next.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_mce_tb_image.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_mce_tb_video.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_mce_tb_audio.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_tb_template.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_tb_note.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_mce_tb_pres.svg \
            /c32,8 $(SOURCEDIR)\qgn_indi_mce_tb_other.svg \  
            /c32,8 $(SOURCEDIR)\qgn_indi_tb_email_accept.svg \  
            /c32,8 $(SOURCEDIR)\qgn_indi_tb_email_tentative.svg \  
            /c32,8 $(SOURCEDIR)\qgn_indi_tb_email_decline.svg \  
            /c32,8 $(SOURCEDIR)\qgn_indi_sett_protected_add.svg \
	    /c32,8 $(SOURCEDIR)\qgn_indi_cal_locat_event_add.svg \

# </cmail>

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
