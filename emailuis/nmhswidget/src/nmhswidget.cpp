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
#include <hbcolorscheme.h>
#include "nmcommon.h"
#include "nmhswidget.h"
#include "nmhswidgetemailengine.h"
#include "nmmessageenvelope.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"

NmHsWidget::NmHsWidget(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags), 
      mEngine(0),
      mRowLayout(0),
      mTitleRow( new NmHsWidgetTitleRow() ),
      mAccountId(0)
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
    \fn void NmHsWidget::onShow()

    Shows the widget
*/
void NmHsWidget::onShow()
{
    qDebug() << "NmHsWidget::onShow IN -->>";
    if (mEngine)
        {
        mEngine->activate();
        }
    qDebug() << "NmHsWidget::onShow OUT <<--";
}


/*!
    \fn void NmHsWidget::onHide()

    Hides the widget
*/
void NmHsWidget::onHide()
{
    qDebug() << "NmHsWidget::onHide IN -->>";
    if (mEngine)
        {
        mEngine->suspend();
        }
    qDebug() << "NmHsWidget::onHide OUT <<--";
}


/*!
    Initializes the widget.
*/
void NmHsWidget::onInitialize()
{
    qDebug() << "NmHsWidget::onInitialize IN -->>";

    mEngine = new NmHsWidgetEmailEngine( mAccountId ); 
    mTitleRow->updateAccountName(mEngine->accountName());
    updateMailData();
    mTitleRow->updateUnreadCount(mEngine->unreadCount());
    
    //Get signals about changes in mail data
    connect(mEngine, SIGNAL( mailDataChanged() )
            ,this, SLOT( updateMailData() ) );
    
    //Get Signals about changes in unread count
    connect(mEngine, SIGNAL( unreadCountChanged(const int&) )
            ,mTitleRow, SLOT( updateUnreadCount(const int&) ) );
    
    //Get signals about account name changes
    connect(mEngine, SIGNAL( accountNameChanged(const QString&) )
            ,mTitleRow, SLOT( updateAccountName(const QString&) ) );

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
    Sets monitored account id from given string
    Needed for home screen framework thich supports only QString type properties
*/
void NmHsWidget::setAccountId(const QString &text)
{
    qDebug() << "NmHsWidget::setAccountId IN -->>"; 
    bool ok;
    quint64 id = text.toULongLong(&ok);
    if (!ok)
        {
        // TODO: assert here if conversion failed?
        qDebug() << "NmHsWidget::setAccountId: invalid account ID data!!!"; 
        mAccountId.setId(0);
        }
    else
        {
        mAccountId.setId(id);
        }
    qDebug() << "NmHsWidget::setAccountId OUT <<--"; 
}

/*!
    Returns monitored account id as a string
    Needed for home screen framework which supports only QString type properties
*/
QString NmHsWidget::accountId() const
{
    qDebug() << "NmHsWidget::accountId()"; 
    return QString::number(mAccountId.id());
}

/*!
    Updates list to include correct amount of mail row widgets
*/
void NmHsWidget::updateMailRowsList(int mailCount)
{
    qDebug() << "NmHsWidget::updateMailRowsList IN -->>";
    qDebug() << "NmHsWidget - mMailRows.count() == " <<  mMailRows.count();
    qDebug() << "NmHsWidget - ordered count == " <<  mailCount;
    while (mMailRows.count() != mailCount)
        {
        //more mails to show than rows
        if (mMailRows.count() < mailCount)
            {
            qDebug() << "NmHsWidget - add new mail row";
            NmHsWidgetEmailRow *row = new NmHsWidgetEmailRow();
            mMailRows.append(row);
            mRowLayout->addItem(row);            
            }
        //too many rows
        else if (mMailRows.count() > mailCount)
            {
            qDebug() << "NmHsWidget - remove mail row";
            mRowLayout->removeItem(mMailRows.last());
            delete mMailRows.takeLast();
            }
        }
    __ASSERT_ALWAYS( mMailRows.count() == mailCount, User::Panic(_L("Invalid"), 500) );
    qDebug() << "NmHsWidget::updateMailRowsList OUT <<--";
}

/*!
    mousePressEvent(QGraphicsSceneMouseEvent *event)
*/
void NmHsWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NmHsWidget::mousePressEvent IN -->>";

    if (mTitleRow->rect().contains(event->pos()))
        {
        mEngine->launchMailAppInboxView();
        }
    else
        {
        for (int i=0; i < mMailRows.count(); i++)
            {
            QRectF tmpRect = mMailRows.at(i)->geometry(); // rect();
            if (tmpRect.contains(event->pos()))
                {
                mEngine->launchMailAppMailViewer(mMailRows.at(i)->messageId());
                break;
                }
            }
        }
    qDebug() << "NmHsWidget::mousePressEvent OUT <<--";
}
