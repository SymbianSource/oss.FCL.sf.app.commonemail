/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMAILUIWIDGETSHEADERS_H_
#define NMAILUIWIDGETSHEADERS_H_

// Qt
#include <QtPlugin>
#include <QTimer>
#include <QCoreApplication>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsGridLayout>

// Symbian & qt extensions
#ifdef Q_OS_SYMBIAN
#include <cntservicescontact.h>
#include <xqservicerequest.h>
#include <qtcontacts.h>
#include <qmobilityglobal.h>
#include <xqserviceutil.h>
#include <xqappmgr.h>
#include <xqserviceipcmarshal.h>
#endif

// Orbit
#include <HbInstantFeedback>
#include <HbFrameItem>
#include <HbEffect>
#include <hbmessagebox.h>
#include <hbscrollarea.h>
#include <hbstyleloader.h>
#include <hbtextitem.h>
#include <hbprogressbar.h>
#include <hbdocumentloader.h>
#include <hbinstance.h>
#include <hbcolorscheme.h>
#include <hbtapgesture.h>
#include <hbevent.h>
#include <hbinputmethod.h>

// nmail
#include <nmaddress.h>
#include <nmcommon.h>

// nmailuiwidgets
#include "nmrecipientlineedit.h"
#include "nmhtmllineedit.h"
#include "nmeditortextedit.h"
#include "nmbaseviewscrollarea.h"
#include "nmattachmentlistitem.h"
#include "nmattachmentlistwidget.h"

#endif /* NMAILUIWIDGETSHEADERS_H_ */
