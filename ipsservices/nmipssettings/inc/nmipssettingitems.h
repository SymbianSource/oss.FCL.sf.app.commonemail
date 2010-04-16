/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef NMIPSSETTINGITEMS_H
#define NMIPSSETTINGITEMS_H

namespace IpsServices {
     
     /*!
         Enumeration for identifying mailbox settings.
     */
    enum SettingItem {
        LoginName = 0,                          // String
        Password,                               // String
        MailboxName,                            // String
        EmailAddress,                           // String
        ReplyAddress,                           // String
        EmailAlias,                             // String
        MyName,                                 // String
        DownloadPictures,                       // Integer: 0=Off, 1=On
        MessageDivider,                         // Integer: 0=Off, 1=On
        ReceptionActiveProfile,                 // Integer: Keep-Up-To-Date=0, Save Energy=1, Manual Fetch=2, User Defined=3
        ReceptionUserDefinedProfile,            // Integer: 0=Disabled, 1=Enabled
        ReceptionInboxSyncWindow,               // Integer: 0=All messages
        ReceptionGenericSyncWindowInMessages,   // Integer: 0=All messages
        ReceptionWeekDays,                      // Integer bitmask: 0x01=Mon,0x02=Tue,0x04=Wed,0x08=Thu,0x10=Fri,0x20=Sat,0x40=Sun
        ReceptionDayStartTime,                  // Integer: 0-23
        ReceptionDayEndTime,                    // Integer: 0-23
        ReceptionRefreshPeriodDayTime,          // Integer: 5,15,60,240,0="When open mailbox"
        ReceptionRefreshPeriodOther,            // Integer: 5,15,60,240,0="When open mailbox"
        UserNameHidden                          // Integer: 0=Off, 1=On
    }; 

} // namespace

#endif // NMIPSSETTINGITEMS_H
