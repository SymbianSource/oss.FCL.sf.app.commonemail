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
#include <hbextendedlocale.h>
#include "nmicons.h"
#include "nmcommon.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"
#include "nmmessageenvelope.h"

NmHsWidgetEmailRow::NmHsWidgetEmailRow(QGraphicsItem *parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
      mSenderLabel(0),
      mSubjectLabel(0),
      mTimeLabel(0),
      mNewMailIcon(0),
      mSeparatorIcon(0),
      mMessageId(0)
{
    qDebug() << "NmHsWidgetEmailRow::NmHsWidgetEmailRow IN -->>";
    
    loadDocML();
    
    qDebug() << "NmHsWidgetEmailRow::NmHsWidgetEmailRow OUT <<--";
}

/*!
    Destructor
*/
NmHsWidgetEmailRow::~NmHsWidgetEmailRow()
{
    qDebug() << "NmHsWidgetEmailRow::~NmHsWidgetEmailRow IN -->>";

    qDebug() << "NmHsWidgetEmailRow::~NmHsWidgetEmailRow OUT <<--";
}

/*!
    Returns id of message shown
*/
NmId NmHsWidgetEmailRow::messageId()
{
    qDebug() << "NmHsWidgetEmailRow::messageId()";
    return mMessageId;

}

/*!
    Loads layout data and child items from docml file
*/
void NmHsWidgetEmailRow::loadDocML()
{
    qDebug() << "NmHsWidgetEmailRow::loadDocML IN -->>";
    
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
    layout->setSpacing(KNmHsWidgetContentsMargin);        
    
    // Use document loader to load the contents
    HbDocumentLoader loader;
    bool ok = false;
    loader.load( KNmHsWidgetMailRowDocML, &ok);
    Q_ASSERT_X(ok, "nmhswidget", "invalid email docml file");
    
    //find container widget
    QGraphicsWidget *container = loader.findWidget(KNmHsWidgetMailRowContainer);
    Q_ASSERT_X((container != 0), "nmhswidget", "email container not found!");
    layout->addItem(container);

    //separator
    mSeparatorIcon = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailSeparatorIcon));
    HbIcon separatorIcon("qtg_graf_divider_h_thin"); 
    mSeparatorIcon->setIcon(separatorIcon);
    
    //child items possible to update
    mSenderLabel = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowSenderLabel));
    mSubjectLabel = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowSubjectLabel));
    mTimeLabel = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowTimeLabel));

    //Icons
    mNewMailIcon = static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowNewMailIcon));
    HbIcon newEmailIcon("qtg_fr_list_new_item_c"); 
    mNewMailIcon->setIcon(newEmailIcon);
    // KNmHsWidgetMailRowLeftIcon is not yet used, because followup information is not shown in client side
    // and thus it is not wanted to be shown in widget side
    mStatusIcons.append( static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowRightIcon)));
    mStatusIcons.append( static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowMiddleIcon)));
    mStatusIcons.append( static_cast<HbLabel*>(loader.findWidget(KNmHsWidgetMailRowLeftIcon)));

    //hide all the icons first to avoid blinking
    hideIcons();
    
    setLayout(layout);
    
    qDebug() << "NmHsWidgetEmailRow::loadDocML OUT <<--";
}

/*!
    Sets the data provided as a parameter to the UI components
    \param envelope message envelope representing an email
*/
void NmHsWidgetEmailRow::updateMailData( const NmMessageEnvelope& envelope )
    {
    qDebug() << "NmHsWidgetEmailRow::updateMailData IN -->>";

    //hide all icons, so no previous data is messing with the new
    hideIcons();

    mMessageId = envelope.id();
    //Show sender name if it is available, otherwise show email address
    QString senderDisplayName = envelope.sender().displayName();
    if ( !senderDisplayName.isNull() && !senderDisplayName.isEmpty()  )
        {
        mSenderLabel->setPlainText( senderDisplayName );
        }
    else
        {
        mSenderLabel->setPlainText ( envelope.sender().address() );
        }
    
    //Set subject
    //Todo: what about empty subject?
    mSubjectLabel->setPlainText( envelope.subject() );
    
    //Set Date with locale support
    //Time shown if message is sent today, otherwise show date
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime now = QDateTime::currentDateTime();
    QDateTime time = envelope.sentTime(); 
    if ( time.date() == now.date() )
        {
        //time format specification
        QString timeSpec = r_qtn_time_usual;
        mTimeLabel->setPlainText( locale.format(time.time(), timeSpec) );
        }
    else
        {
        QString dateSpec = r_qtn_date_without_year;
        mTimeLabel->setPlainText( locale.format(time.date(), dateSpec) );
        }
    
    //set new icons to widget based on the data
    setIconsToWidget( envelope );
    qDebug() << "NmHsWidgetEmailRow::updateMailData OUT <<--";
    }

/*!
    hide icons from widget
*/
void NmHsWidgetEmailRow::hideIcons()
    {
    qDebug() << "NmHsWidgetEmailRow::hideIcons IN -->>";
    for( int i = 0; i<mStatusIcons.count();i++)
        {
        mStatusIcons[i]->hide();
        }
    mNewMailIcon->hide();
    qDebug() << "NmHsWidgetEmailRow::hideIcons OUT <<--";
    }


/*!
    Set icons to widget
*/
void NmHsWidgetEmailRow::setIconsToWidget( const NmMessageEnvelope& envelope )
{
    qDebug() << "NmHsWidgetEmailRow::setIconsToWidget IN -->>";

    bool unreadMail = !envelope.isRead();
    bool attachment = envelope.hasAttachments();
    int priority = envelope.priority();
    
    if(unreadMail){
        //Todo: set unread icon when available
        mNewMailIcon->show();
    }
    
    // Here we have list for priority and attachment icons. Later it is easy to add
    // followup icon or something else if needed.
    QList<HbIcon> iconList;
    
    // Priority icon is added to list first thus it is always shown most right.
    switch (priority){
        case NmMessagePriorityLow:
            iconList.append(NmIcons::getIcon(NmIcons::NmIconPriorityLow));
            break;
        case NmMessagePriorityHigh:
            iconList.append(NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
            break;
        case NmMessagePriorityNormal:
        default:
            // Normal priority has no icon
            break;
        }
    
    // Attachment icon is always shown on the left side of attachment icon if it
    // exists. Otherwise in the most right.
    if(attachment){
        iconList.append(NmIcons::getIcon(NmIcons::NmIconAttachment));
    }

    // Here we show icons added to the iconList in the order they have been added.
    for(int count = 0; count<iconList.count(); count++){
        mStatusIcons[count]->setIcon(iconList[count]);
        mStatusIcons[count]->show();
    }
    
    qDebug() << "NmHsWidgetEmailRow::setIconsToWidget OUT <<--";
}

