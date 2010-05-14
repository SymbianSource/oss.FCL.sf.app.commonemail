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

#ifndef NMHSWIDGETCONSTS_H
#define NMHSWIDGETCONSTS_H

/**************************************************
 * engine
 *************************************************/
//Maximum amount of envelopes that can be provided to client in getData function
//This is also the amount of envelopes that is kept in mData all the time
const int KMaxNumberOfEnvelopesProvided = 2;
//Maximum value for unread count
const int KMaxUnreadCount = 999; 

/**************************************************
 * nmhswidget
 *************************************************/
/** contents margin/spacing in all layouts / container widgets */
const int KNmHsWidgetContentsMargin = 0;
/** Widget's background image*/
const QString KNmHsWidgetBackgroundImage = "qtg_fr_hswidget_normal";
/** max number of mails shown in widget */
const int KMaxNumberOfMailsShown = KMaxNumberOfEnvelopesProvided;
/** Widget's localization file name*/
const QString KNmHsWidgetLocFileName = "mailwidget_";
/** Widget's localization file location*/
const QString KNmHsWidgetLocLocation = "z:/resource/qt/translations/";
/** Widget's state property values */
const QString KNmHsWidgetStateCollapsed = "collapsed";
const QString KNmHsWidgetStateExpanded = "expanded";

/**************************************************
 * Title row widget
 *************************************************/
/**  docml file including title row UI definitions */
#define KNmHsWidgetTitleRowDocML ":/docml/nmhswidgettitle.docml"
/**  docml file including title row UI definitions */
#define KNmHsWidgetTitleRowContainer  "titleLayoutWidget"
/** mailbox icon */
#define KNmHsWidgetTitleRowMailboxIcon  "mailboxIcon"
/** mailbox name label */
#define KNmHsWidgetTitleRowMailboxNameLabel "mailboxNameLabel"
/** unread count label */
#define KNmHsWidgetTitleRowUnreadCountLabel  "unreadCountLabel"
/** expand collapse icon */
#define KNmHsWidgetTitleRowCollapseExpandIconLabel "collapseExpandIcon"

/**************************************************
 * Mail row widget
 *************************************************/
/**  docml file including mail row UI definitions */
#define KNmHsWidgetMailRowDocML   ":/docml/nmhswidgetmail.docml"
/** container widget name for mail row UI items */
#define KNmHsWidgetMailRowContainer   "emailLayoutWidget"
/** sender label */
#define KNmHsWidgetMailRowSenderLabel   "labelSender"
/** time label */
#define KNmHsWidgetMailRowTimeLabel   "labelTime"
/** subject label */
#define KNmHsWidgetMailRowSubjectLabel   "labelSubject"
/** new mail icon */
#define KNmHsWidgetMailRowNewMailIcon   "iconNewMail"
/** left status icon */
#define KNmHsWidgetMailRowLeftIcon   "iconOne"
/** middle status icon */
#define KNmHsWidgetMailRowMiddleIcon   "iconTwo"
/** right status icon */
#define KNmHsWidgetMailRowRightIcon   "iconThree"
/** separator icon */
#define KNmHsWidgetMailSeparatorIcon   "iconSeparator"


#endif  // NMHSWIDGETCONSTS_H
