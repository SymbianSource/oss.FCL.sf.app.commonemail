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

/** contents margin/spacing in all layouts / container widgets */
const int KNmHsWidgetContentsMargin = 0;
/** radius for container widget's rounded rect */
const int KNmHsWidgetShoulderRadius = 10;
/** background opacity */
const qreal KNmHsWidgetBackgroundOpacity = 0.50;
/** color scheme used in widget background */
//const QString backgroundColorAttribute = "popupbackground";
#define KBackgroundColorAttribute  "popupbackground"

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
