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
# Contributors:
#
# Description: Make file for mrgui icons.
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
SOURCEDIR=$(EPOCROOT)epoc32\s60\bitmaps
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
                /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_none.svg\
                /c32,8 $(SOURCEDIR)\qgn_indi_cmail_calendar_tracking_reject.svg \
				/c32,8 $(SOURCEDIR)\qgn_indi_cmail_attachment.svg \
				/c32,8 $(SOURCEDIR)\qgn_graf_cmail_email_ctrlbar.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_center.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_corner_bl.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_corner_br.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_corner_tl.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_corner_tr.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_side_bottom.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_side_l.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_side_r.svg \
				/c32,8 $(SOURCEDIR)\qsn_fr_cmail_list_side_top.svg \
				/c32,8 $(SOURCEDIR)\qgn_indi_cmail_home_arrow_right.svg

# </cmail>

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
