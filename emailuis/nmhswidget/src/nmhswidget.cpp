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
#include <QDebug>
#include <QtGui>
#include <QGraphicsLinearLayout>
#include <hbcolorscheme.h>
#include "nmhswidget.h"
#include "nmhswidgetemailengine.h"
#include "nmmessageenvelope.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"

NmHsWidget::NmHsWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags), 
      mEngine( new NmHsWidgetEmailEngine() ),
      mRowLayout(0),
      mTitleRow( new NmHsWidgetTitleRow() )
{
    qDebug() << "NmHsWidget::NmHsWidget IN -->>";
   
    this->setContentsMargins( KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    
    //Setup layout
    mRowLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mRowLayout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    mRowLayout->setSpacing(KNmHsWidgetContentsMargin);
    mRowLayout->addItem(mTitleRow);
    setLayout(mRowLayout);
    
    //Do the initial data setting
    updateAccountName( mEngine->accountName() );
    updateMailData();
    updateUnreadCount( mEngine->unreadCount() );
    
    //Get signals about changes in mail data
    connect(mEngine, SIGNAL( mailDataChanged() )
            ,this, SLOT( updateMailData() ) );
    
    //Get Signals about changes in unread count
    connect(mEngine, SIGNAL( unreadCountChanged(const int&) )
            ,this, SLOT( updateUnreadCount(const int&) ) );
    
    //Get signals about account name changes
    connect(mEngine, SIGNAL( accountNameChanged(const QString&) )
            ,this, SLOT( updateAccountName(const QString&) ) );
    
    qDebug() << "NmHsWidget::NmHsWidget OUT <<--";
}

/*!
    Destructor
*/
NmHsWidget::~NmHsWidget()
{
    qDebug() << "NmHsWidget::~NmHsWidget IN -->>";
    if(mEngine)
        {
        delete mEngine;
        mEngine = NULL;
        }
    qDebug() << "NmHsWidget::~NmHsWidget OUT <<--";
}

/*!
    Initializes the widget.
*/
void NmHsWidget::onInitialize()
{
    qDebug() << "NmHsWidget::onInitialize IN -->>";
    
    qDebug() << "NmHsWidget::onInitialize OUT <<--";  
}

/*!
    Uninitializes the widget.
*/
void NmHsWidget::onUninitialize()
{
    qDebug() << "NmHsWidget::onUninitialize IN -->>";
    
    qDebug() << "NmHsWidget::onUninitialize OUT <<--";
}

/*!
    Paints widget
*/
void NmHsWidget::paint(QPainter *painter, 
                          const QStyleOptionGraphicsItem *option, 
                          QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //----- backgroud ----------------
    painter->setOpacity(KNmHsWidgetBackgroundOpacity);
    QColor backgroundColor(HbColorScheme::color(KBackgroundColorAttribute));
    // frame is not wanted to be drawn, thus pen color is same as backround color
    painter->setPen(QPen(backgroundColor, 0));
    painter->setBrush(backgroundColor);
    painter->drawRoundedRect(rect(), KNmHsWidgetShoulderRadius, 
            KNmHsWidgetShoulderRadius);
    painter->setOpacity(1); //set opacity back to default
}

/*!
    updateAccountName slot 
*/
void NmHsWidget::updateAccountName(const QString& accountName)
{
    qDebug() << "NmHsWidget::updateAccountName IN -->>";
    qDebug() << "New account name is: " << accountName;
    
    mTitleRow->updateAccountName(accountName);
        
    qDebug() << "NmHsWidget::updateAccountName OUT <<--"; 
}

/*!
    updateUnreadCount slot
*/
void NmHsWidget::updateUnreadCount(const int& unreadCount )
{
    qDebug() << "NmHsWidget::updateUnreadCount IN -->>";
    qDebug() << "New unread count is: " << QString::number(unreadCount);
   
    mTitleRow->updateUnreadCount(unreadCount);
    
    qDebug() << "NmHsWidget::updateUnreadCount OUT <<--"; 
}

/*!
    updateMailData slot
*/
void NmHsWidget::updateMailData()
{
    qDebug() << "NmHsWidget::updateData IN -->>";
    QList<NmMessageEnvelope> envelopes;
    int count = mEngine->getEnvelopes(envelopes, 2);
    //TODO: needs error check for -1 ?
    updateMailRowsList(count);
    
    for(int i=0; i<envelopes.count(); i++)
        {
        qDebug() << "env:" << QString::number(i) << "dispName: " << envelopes[i].sender().displayName();
        qDebug() << "env:" << QString::number(i) << "dispName: " << envelopes[i].sender().address();
        qDebug() << "env:" << QString::number(i) << "subject:" << envelopes[i].subject();
        qDebug() << "env:" << QString::number(i) << "isRead: " << envelopes[i].isRead();
        qDebug() << "env:" << QString::number(i) << "date: " << envelopes[i].sentTime();
        qDebug() << "env:" << QString::number(i) << "priority: " << envelopes[i].priority();
        qDebug() << "env:" << QString::number(i) << "attachments: " << envelopes[i].hasAttachments();
        mMailRows[i]->updateMailData( envelopes[i] );
        }
    qDebug() << "NmHsWidget::updateData OUT <<--"; 
}

/*!
    Updates list to include correct amount of mail row widgets
*/
void NmHsWidget::updateMailRowsList(int mailCount)
{
    qDebug() << "NmHsWidget::updateMailRowsList IN -->>";
    while (mMailRows.count() != mailCount)
        {
        //more mails to show than rows
        if (mMailRows.count() < mailCount)
            {
            NmHsWidgetEmailRow *row = new NmHsWidgetEmailRow();
            mMailRows.append(row);
            mRowLayout->addItem(row);            
            }
        //too many rows
        else if (mMailRows.count() > mailCount)
            {
            mRowLayout->removeItem(mMailRows.last());
            delete mMailRows.takeLast();
            }
        }
    __ASSERT_ALWAYS( mMailRows.count() == mailCount, User::Panic(_L("Invalid"), 500) );
    qDebug() << "NmHsWidget::updateMailRowsList OUT <<--";
}
