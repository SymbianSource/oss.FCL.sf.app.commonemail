/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  constants needed in content publishing
*
*/


#ifndef CMAILCPSIFCONSTS_H_
#define CMAILCPSIFCONSTS_H_

// Hardcoded maximum value for number of rows in widget
const TInt KMaxRowCount = 3;
// Hardcoded maximum value for number of mailboxes
const TInt KMaxMailboxCount = 18;
const TInt KMaxUnreadCount = 999;
const TInt KMaxDescLen = 256;
_LIT(KNullDes, "");
_LIT8(KNullDes8, "");
const TInt KNullIcon = 0;

_LIT( KGreaterThan,">");
_LIT( KLessThan,"<");
_LIT( KAt,"@");
_LIT( KDot,".");
_LIT( KMifPrefix, "mif(");
// Mif file name and path, drive letter need to be parsed run time
_LIT( KMifPath, "\\resource\\apps\\cmailhandlerplugin.mif");
_LIT( KSpace, " ");
// Resource file name and path, drive letter need to be parsed run time
_LIT( KResourceFile, "\\resource\\apps\\emailwidget.rsc" );
_LIT( KDissociated,"0");

_LIT(KStartSeparator, "<");
_LIT(KEndSeparator, ">");

//
// EMAIL WIDGET SPECIFIC CONSTANTS USED IN PUBLISHING
//

_LIT(KPubId, "0x2001F413:CMailWidget");
_LIT(KContTypeText, "text");
_LIT(KContTypeImage, "image");

_LIT(KContTypeSetupText, "ct_setup_text");
_LIT(KKeySetupText, "key_setup_text");
_LIT(KContTypeSetupIcon, "ct_setup_icon");
_LIT8(KKeySetupIcon, "key_setup_icon");

_LIT(KContTypeBodyText, "ct_body_text_");
_LIT(KKeyBodyText, "key_body_text_");

_LIT(KContTypeIndicatorIcon, "ct_indicator_icon_");
_LIT8(KKeyIndicatorIcon, "key_indicator_icon_");

_LIT(KContTypeMailboxIcons, "ct_mailbox_icons_");
_LIT8(KKeyMailboxIcons, "key_mailbox_icons_");

_LIT(KContTypeTime, "ct_time_");
_LIT(KKeyTime, "key_time_");

// Wizard launching action
_LIT(KTriggerWizard, "trigger_wizard");
_LIT8(KTriggerWizard8, "trigger_wizard");
const TUid KUidWizardApp = { 0x10281c96 };
const TUid KUidEmailWizardView = { 0x10281C9A };

// email ui launching action
_LIT(KTriggerEmailUi, "trigger_emailui");
_LIT8(KTriggerEmailUi8, "trigger_emailui");
const TUid KUidEmailUi = { 0x2001E277 };
const TUid KMailListId = { 0x00000002 };

// settings app launching
const TUid KUidEMWSettings = { 0x2002429E };
const TUid KUidEMWSettingsView = { 0 };

//
// CONSTANTS NEEDED BY CONTENT PUBLISHING SERVICE
//
_LIT8( KAdd, "Add" );
_LIT8( KContentId, "content_id" );
_LIT8( KContentType, "content_type" );
_LIT8( KCPInterface, "IDataSource" );
_LIT8( KCPContentInterface, "IContentPublishing" );
_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KCommand, "cmd" );
_LIT8( KType, "type" );
_LIT8( KDataMap, "data_map" );
_LIT8( KDelete, "Delete" );
_LIT( KCpData, "cp_data" );
_LIT8( KFilter, "filter" );
_LIT8( KId, "id" );
_LIT8( KItem, "item" );
_LIT8( KItemId, "item_id" );
_LIT8( KPublisherId, "publisher" );
_LIT8( KPluginId, "plugin_id");
_LIT8( KActionMap, "action_map" );
_LIT8( KDataForActionHandler, "data" );
_LIT( KData, "data" );
_LIT8( KLaunchMethod, "launch_method" );
_LIT8( KApplicationUid, "application_uid" );
_LIT( KActionValueViewActivation, "view_activation");
_LIT8( KAdditionalData, "additional_data" );
_LIT8( KMessageUid, "message_uid" );
_LIT8( KViewId, "view_id" );
_LIT8( KViewAppUid, "view_app_uid" );
//  constants for requesting notification
_LIT( KPubData, "publisher");
_LIT( KActionName,"notify");
_LIT8( KDeActive, "deactive");
_LIT8( KActive, "active");
_LIT8( KSuspend , "suspend");
_LIT8( KResume, "resume");
_LIT8( KInactive, "inactive");
_LIT( KDeActive16, "deactive");
_LIT( KActive16, "active");
_LIT( KSuspend16, "suspend");
_LIT( KResume16, "resume");
_LIT( KInactive16, "inactive");
_LIT( KSettings16, "settings");
_LIT( KCPAll, "all");
_LIT8( KExecute, "execute" );
_LIT( KContentAndPublisher_Registry, "cp_data:publisher");
_LIT8( KOperation, "operation" );
_LIT8( KRequestNotification, "RequestNotification" ); 
_LIT8( KChangeInfo, "change_info" );
_LIT8( KActionTrigger, "action_trigger" );

enum TWidgetMailDetails 
    {
    ESender,
    ESubject,
    ETime
    };

#endif /*CMAILCPSIFCONSTS_H_*/
