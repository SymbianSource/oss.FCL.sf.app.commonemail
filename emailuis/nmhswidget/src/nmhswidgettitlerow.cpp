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
#include <QtGui>
#include <QGraphicsLinearLayout>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include <HbPushButton>
#include "nmicons.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetconsts.h"

NmHsWidgetTitleRow::NmHsWidgetTitleRow(QGraphicsItem *parent, Qt::WindowFlags flags) :
    HbWidget(parent, flags), 
    mMailboxIcon(0), 
    mMailboxInfo(0), 
    mUnreadCountLabel(0),
    mCollapseExpIconLabel(0), 
    mAccountName(0), 
    mUnreadCount(0)
{
    qDebug() << "NmHsWidgetTitleRow::NmHsWidgetTitleRow IN -->>";

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
 Loads layout data and child items from docml file. Must be called after constructor.
 /return true if loading succeeded, otherwise false. False indicates that object is unusable
 */
bool NmHsWidgetTitleRow::loadDocML()
{
    QT_TRY{
        qDebug() << "NmHsWidgetTitleRow::loadDocML IN -->>";
    
        // Use document loader to load the contents
        HbDocumentLoader loader;
        bool ok(false);
        loader.load(KNmHsWidgetTitleRowDocML, &ok);
        if (!ok) {
            qDebug() << "NmHsWidgetTitleRow::loadDocML Fail @ loader -->>";
            return false; //failure
        }
    
        //Create layout
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    
        layout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
        layout->setSpacing(KNmHsWidgetContentsMargin);
        setLayout(layout); //pass the ownership
    
        // find container widget
        QGraphicsWidget *container = loader.findWidget(KNmHsWidgetTitleRowContainer);
        if (!container) {
            qDebug() << "NmHsWidgetTitleRow::loadDocML Fail @ container -->>";
            return false;
        }
        layout->addItem(container);
    
        //child items possible to update
        mMailboxIcon = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetTitleRowMailboxIcon));
        mMailboxInfo = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetTitleRowMailboxNameLabel));
        mUnreadCountLabel = static_cast<HbLabel*> (loader.findWidget(
            KNmHsWidgetTitleRowUnreadCountLabel));
        mCollapseExpIconLabel = static_cast<HbPushButton*> (loader.findWidget(
            KNmHsWidgetTitleRowCollapseExpandIconLabel));
    
        if (!mMailboxIcon || !mMailboxInfo || !mUnreadCountLabel || !mCollapseExpIconLabel) {
            qDebug() << "NmHsWidgetTitleRow::loadDocML Fail @ icons & labels -->>";
            return false;
        }
    
        //Expand collapse button
        connect(mCollapseExpIconLabel, SIGNAL(clicked()), this, SIGNAL(expandCollapseButtonPressed()));
    
        qDebug() << "NmHsWidgetTitleRow::loadDocML OUT <<--";
        return true;
    }
    QT_CATCH(...){
        return false;
    }
}

/*!
 Slot for updating account name, calls updateData to update ui.
 */
void NmHsWidgetTitleRow::updateAccountName(const QString& accountName)
{
    qDebug() << "NmHsWidgetTitleRow::updateAccountName IN -->>";
    mAccountName = accountName;
    updateData();
    qDebug() << "NmHsWidgetTitleRow::updateAccountName OUT <<--";
}

/*!
 Set account icon name
 */
void NmHsWidgetTitleRow::setAccountIcon(const QString& accountIconName)
{
    qDebug() << "NmHsWidgetTitleRow::setAccountIcon -- accountIconName" << accountIconName;
    mMailboxIcon->setIcon(accountIconName);
    qDebug() << "NmHsWidgetTitleRow::setAccountIcon OUT <<--";
}

/*!
 Slot for updating unread count, calls updateData to update ui.
 */
void NmHsWidgetTitleRow::updateUnreadCount(const int& unreadCount)
{
    qDebug() << "NmHsWidgetTitleRow::updateUnreadCount IN -->>";
    mUnreadCount = unreadCount;
    updateData();
    qDebug() << "NmHsWidgetTitleRow::updateUnreadCount OUT <<--";
}

/*!
 Update the data displayed in the UI
 */
void NmHsWidgetTitleRow::updateData()
{
    qDebug() << "NmHsWidgetTitleRow::updateData() IN -->>";
    mMailboxInfo->setPlainText(mAccountName);
    //If unread count is -1, hide the unread count label completely.
    //This indicates that there are no mails at all (or the initial sync is not done)
    if (mUnreadCount != -1) {
        QString unreadCount(hbTrId("txt_mail_widget_list_l1").arg(mUnreadCount));
        mUnreadCountLabel->setPlainText(unreadCount);
        mUnreadCountLabel->setVisible(true);
    }
    else {
        mUnreadCountLabel->setVisible(false);
    }
    qDebug() << "NmHsWidgetTitleRow::updateData() OUT <<--";
}

/*!
 mousePressEvent(QGraphicsSceneMouseEvent *event)
 */
void NmHsWidgetTitleRow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NmHsWidgetTitleRow::mousePressEvent() IN -->>";
    Q_UNUSED(event); 
	emit mailboxLaunchTriggered();
    qDebug() << "NmHsWidgetTitleRow::mousePressEvent() OUT <<--";
}
