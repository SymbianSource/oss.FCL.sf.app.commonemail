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
#include <HbFrameDrawer>
#include <HbFrameItem>
#include "nmicons.h"
#include "nmcommon.h"
#include "nmhswidgetemailrow.h"
#include "nmhswidgetconsts.h"
#include "nmmessageenvelope.h"

NmHsWidgetEmailRow::NmHsWidgetEmailRow(QGraphicsItem *parent, Qt::WindowFlags flags) :
    HbWidget(parent, flags), 
    mSenderLabel(0), 
    mSubjectLabel(0), 
    mTimeLabel(0), 
    mNewMailIcon(0),
    mSeparatorIcon(0), 
    mMessageId(0)
{
    qDebug() << "NmHsWidgetEmailRow::NmHsWidgetEmailRow IN -->>";

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
 Must be called after constructor.
  /return true if loading succeeded, otherwise false. False indicates that object is unusable.
 */
bool NmHsWidgetEmailRow::loadDocML()
{
    HbFrameDrawer* backgroundFrameDrawer = 0;
    HbFrameItem* backgroundLayoutItem = 0;
    QT_TRY{
        qDebug() << "NmHsWidgetEmailRow::loadDocML IN -->>";
    
        // Use document loader to load the contents
        HbDocumentLoader loader;
        bool ok(false);
        loader.load(KNmHsWidgetMailRowDocML, &ok);
        if (!ok) {
            qDebug() << "NmHsWidgetEmailRow::loadDocML fail @ loader <<--";
            return false;
        }
    
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    
        //Do the layout stuff and Set layout before next return to avoid memoryleak
        layout->setContentsMargins(KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin,
            KNmHsWidgetContentsMargin, KNmHsWidgetContentsMargin);
        layout->setSpacing(KNmHsWidgetContentsMargin);
        setLayout(layout);
    
        //find container widget
        QGraphicsWidget *container = loader.findWidget(KNmHsWidgetMailRowContainer);
        if (!container) {
            qDebug() << "NmHsWidgetEmailRow::loadDocML fail @ container <<--";
            return false;
        }
        layout->addItem(container);
    
        //child items possible to update
        //separator
        mSeparatorIcon = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailSeparatorIcon));
    
        //labels
        mSenderLabel = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowSenderLabel));
        mSubjectLabel = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowSubjectLabel));
        mTimeLabel = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowTimeLabel));
    
        //icons
        mNewMailIcon = static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowNewMailIcon));
        // KNmHsWidgetMailRowLeftIcon is not yet used, because followup information is not shown in client side
        // and thus it is not wanted to be shown in widget side
        mStatusIcons.append(static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowRightIcon)));
        mStatusIcons.append(static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowMiddleIcon)));
        mStatusIcons.append(static_cast<HbLabel*> (loader.findWidget(KNmHsWidgetMailRowLeftIcon)));
    
        //Verify that items are valid
        if (!mSenderLabel || !mSubjectLabel || !mTimeLabel || !mNewMailIcon || !mSeparatorIcon) {
            qDebug() << "NmHsWidgetEmailRow::loadDocML fail @ labels & icons <<--";
            return false;
        }
        //Verify all mStatusIcons
        for (int i = 0; i < mStatusIcons.length(); i++) {
            if (!mStatusIcons[i]) {
                qDebug() << "NmHsWidgetEmailRow::loadDocML status icons <<--";
                return false;
            }
        }
    
        //separator icon
        HbIcon separatorIcon("qtg_graf_divider_h_thin");
        mSeparatorIcon->setIcon(separatorIcon);
        
        //new email icon
        backgroundFrameDrawer = new HbFrameDrawer("qtg_fr_list_new_item",
            HbFrameDrawer::ThreePiecesVertical);
        backgroundLayoutItem = new HbFrameItem(backgroundFrameDrawer);
        mNewMailIcon->setBackgroundItem(backgroundLayoutItem);
    
        //hide all the icons first to avoid blinking
        hideIcons();
    
        qDebug() << "NmHsWidgetEmailRow::loadDocML OK OUT <<--";
        return true;
    }
    QT_CATCH(...){
        if(!backgroundLayoutItem && backgroundFrameDrawer){
            delete backgroundFrameDrawer;
            backgroundFrameDrawer = NULL;
        }
        return false;
    }
}

/*!
 Sets the data provided as a parameter to the UI components
 \param envelope message envelope representing an email
 */
void NmHsWidgetEmailRow::updateMailData(const NmMessageEnvelope& envelope)
{
    qDebug() << "NmHsWidgetEmailRow::updateMailData IN -->>";

    //hide all icons, so no previous data is messing with the new
    hideIcons();

    mMessageId = envelope.messageId();
    //Show sender name if it is available, otherwise show email address
    QString senderDisplayName = envelope.sender().displayName();
    if (!senderDisplayName.isNull() && !senderDisplayName.isEmpty()) {
        mSenderLabel->setPlainText(senderDisplayName);
    }
    else {
        mSenderLabel->setPlainText(envelope.sender().address());
    }

    //Set subject
    mSubjectLabel->setPlainText(envelope.subject());
    
    mMessageSentTime = envelope.sentTime();
    updateDateTime();
    
    //set new icons to widget based on the data
    setIconsToWidget( envelope );
    qDebug() << "NmHsWidgetEmailRow::updateMailData OUT <<--";
    }

/*!
    updateDateTime to label using correct locale
*/
void NmHsWidgetEmailRow::updateDateTime()
    {
    qDebug() << "NmHsWidgetEmailRow::updateDateTime IN -->>";
    //Set Date with locale support
    //Time shown if message is sent today, otherwise show date
    HbExtendedLocale locale = HbExtendedLocale::system();
    QDateTime now = QDateTime::currentDateTime();
    if ( mMessageSentTime.date() == now.date() )
        {
        //time format specification
        QString timeSpec = r_qtn_time_usual;
        mTimeLabel->setPlainText( locale.format(mMessageSentTime.time(), timeSpec) );
        }
    else
        {
        QString dateSpec = r_qtn_date_without_year;
        mTimeLabel->setPlainText( locale.format(mMessageSentTime.date(), dateSpec) );
        }
    qDebug() << "NmHsWidgetEmailRow::updateDateTime OUT <<--";
    }

/*!
 hide icons from widget
 */
void NmHsWidgetEmailRow::hideIcons()
{
    qDebug() << "NmHsWidgetEmailRow::hideIcons IN -->>";
    for (int i = 0; i < mStatusIcons.count(); i++) {
        mStatusIcons[i]->hide();
    }
    mNewMailIcon->hide();
    qDebug() << "NmHsWidgetEmailRow::hideIcons OUT <<--";
}

/*!
 Set icons to widget
 */
void NmHsWidgetEmailRow::setIconsToWidget(const NmMessageEnvelope& envelope)
{
    qDebug() << "NmHsWidgetEmailRow::setIconsToWidget IN -->>";

    bool unreadMail = !envelope.isRead();
    bool attachment = envelope.hasAttachments();
    int priority = envelope.priority();

    if (unreadMail) {
        mNewMailIcon->show();
    }

    // Here we have list for priority and attachment icons. Later it is easy to add
    // followup icon or something else if needed.
    QList<HbIcon> iconList;

    // Priority icon is added to list first thus it is always shown most right.
    switch (priority) {
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
    if (attachment) {
        iconList.append(NmIcons::getIcon(NmIcons::NmIconAttachment));
    }

    // Here we show icons added to the iconList in the order they have been added.
    for (int count = 0; count < iconList.count(); count++) {
        mStatusIcons[count]->setIcon(iconList[count]);
        mStatusIcons[count]->show();
    }

    qDebug() << "NmHsWidgetEmailRow::setIconsToWidget OUT <<--";
}

/*!
 mousePressEvent(QGraphicsSceneMouseEvent *event)
 */
void NmHsWidgetEmailRow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NmHsWidgetTitleRow::mousePressEvent() IN -->>";
    Q_UNUSED(event); 
    qDebug() << "NmHsWidgetTitleRow::mousePressEvent() OUT <<--";
}

/*!
    mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
*/
void NmHsWidgetEmailRow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NmHsWidgetTitleRow::mouseReleaseEvent() IN -->>";
    Q_UNUSED(event);
    emit mailViewerLaunchTriggered(mMessageId);
    qDebug() << "NmHsWidgetTitleRow::mouseReleaseEvent() OUT <<--";
}
