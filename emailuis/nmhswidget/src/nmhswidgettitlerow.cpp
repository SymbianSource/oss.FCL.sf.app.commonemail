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

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include "nmicons.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetconsts.h"

NmHsWidgetTitleRow::NmHsWidgetTitleRow(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mMailboxIcon(0),
      mMailboxInfo(0),
      mAccountName(0),
      mUnreadCount(0)
{
    qDebug() << "NmHsWidgetTitleRow::NmHsWidgetTitleRow IN -->>";
    
    loadDocML();
    
    qDebug() << "NmHsWidgetTitleRow::NmHsWidgetTitleRow OUT <<--";
}

/*!
    Destructor
*/
NmHsWidgetTitleRow::~NmHsWidgetTitleRow()
{
    qDebug() << "NmHsWidgetTitleRow::~NmHsWidgetTitleRow IN -->>";

    qDebug() << "NmHsWidgetTitleRow::~NmHsWidgetTitleRow OUT <<--";
}
    
/*!
    Loads layout data and child items from docml file
*/
void NmHsWidgetTitleRow::loadDocML()
{
    qDebug() << "NmHsWidgetTitleRow::loadDocML IN -->>";
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    layout->setSpacing(KNmHsWidgetContentsMargin);    
    
    // Use document loader to load the contents
    HbDocumentLoader loader;
    bool ok = false;
    loader.load( KNmHsWidgetTitleRowDocML, &ok);
    Q_ASSERT_X(ok, "nmhswidget", "invalid title docml file");
    
    // find container widget
    QGraphicsWidget *container = loader.findWidget(KNmHsWidgetTitleRowContainer);
    Q_ASSERT_X((container != 0), "nmhswidget", "container not found!");
    layout->addItem(container);
    
    //child items possible to update
    mMailboxIcon = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetTitleRowMailboxIcon));
    mMailboxIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconDefaultMailbox));
    mMailboxInfo = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetTitleRowMailboxNameLabel));

    setLayout(layout);
    
    qDebug() << "NmHsWidgetTitleRow::loadDocML OUT <<--";
}

/*!
    Slot for updating account name, calls updateData to update ui.
*/
void NmHsWidgetTitleRow::updateAccountName(const QString& accountName )
    {
    mAccountName = accountName;
    updateData();
    }

/*!
    Slot for updating unread count, calls updateData to update ui.
*/
void NmHsWidgetTitleRow::updateUnreadCount(const int& unreadCount )
    {
    mUnreadCount = unreadCount;
    updateData();
    }

/*!
    Update the data displayed in the UI
*/
void NmHsWidgetTitleRow::updateData()
    {
    //TODO: Sprint 3: use localization for displaying data
    QString info = mAccountName;
    info.append(" (");
    info.append( QString::number(mUnreadCount));
    info.append(")");
    mMailboxInfo->setPlainText(info);
    }
