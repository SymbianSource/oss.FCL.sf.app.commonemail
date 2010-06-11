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

#include <QtGui>
#include <QGraphicsLinearLayout>
#include <hbdocumentloader.h>
#include <hblabel.h>
#include <HbPushButton>
#include <HbColorScheme>
#include <HbEvent>
#include "nmicons.h"
#include "nmhswidgettitlerow.h"
#include "nmhswidgetconsts.h"
#include "emailtrace.h"

NmHsWidgetTitleRow::NmHsWidgetTitleRow(QGraphicsItem *parent, Qt::WindowFlags flags) :
    HbWidget(parent, flags), 
    mMailboxIcon(0), 
    mMailboxInfo(0), 
    mUnreadCountLabel(0),
    mCollapseExpIconLabel(0), 
    mAccountName(),
    mUnreadCount(0)
{
    NM_FUNCTION;
}

/*!
 Destructor
 */
NmHsWidgetTitleRow::~NmHsWidgetTitleRow()
{
    NM_FUNCTION;
}

/*!
 Loads layout data and child items from docml file. Must be called after constructor.
 /return true if loading succeeded, otherwise false. False indicates that object is unusable
 */
bool NmHsWidgetTitleRow::loadDocML()
{
    NM_FUNCTION;
    QT_TRY{
        // Use document loader to load the contents
        HbDocumentLoader loader;
        bool ok(false);
        loader.load(KNmHsWidgetTitleRowDocML, &ok);
        if (!ok) {
            NM_ERROR(1,"NmHsWidgetTitleRow::loadDocML Fail @ loader");
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
            NM_ERROR(1,"NmHsWidgetTitleRow::loadDocML Fail @ container");
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
            NM_ERROR(1,"NmHsWidgetTitleRow::loadDocML Fail @ icons & labels");
            return false;
        }
    
        //Expand collapse button
        connect(mCollapseExpIconLabel, SIGNAL(clicked()), this, SIGNAL(expandCollapseButtonPressed()));
        
        //set fonts color
        setFontsColor(false);

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
    NM_FUNCTION;
    mAccountName = accountName;
    updateData();
}

/*!
 Set account icon name
 */
void NmHsWidgetTitleRow::setAccountIcon(const QString& accountIconName)
{
    NM_FUNCTION;
    mMailboxIcon->setIcon(accountIconName);
}

/*!
 Slot for updating unread count, calls updateData to update ui.
 */
void NmHsWidgetTitleRow::updateUnreadCount(const int& unreadCount)
{
    NM_FUNCTION;
    mUnreadCount = unreadCount;
    updateData();
}


/*!
 Slot for updating expand collapse icon
 */
void NmHsWidgetTitleRow::setExpandCollapseIcon(const bool& expand)
{
    NM_FUNCTION;
    if(expand){
        HbIcon icon("qtg_mono_arrow_up");
        mCollapseExpIconLabel->setIcon(icon);
    }
    else{
        HbIcon icon("qtg_mono_arrow_down");
        mCollapseExpIconLabel->setIcon(icon);
    }
}

/*!
 Update the data displayed in the UI
 */
void NmHsWidgetTitleRow::updateData()
{
    NM_FUNCTION;
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
}

/*!
    sets fonts color.
    param bool pressed indicates if row is pressed down or not
*/
void NmHsWidgetTitleRow::setFontsColor( bool pressed )
    {
    NM_FUNCTION;
    QColor newFontColor;
    
    if(pressed){
        newFontColor = HbColorScheme::color("qtc_hs_list_item_pressed");
    }
    else{
        newFontColor = HbColorScheme::color("qtc_hs_list_item_title_normal");
    }
 
    mMailboxInfo->setTextColor(newFontColor);
    mUnreadCountLabel->setTextColor(newFontColor);
    }


/*!
 mousePressEvent(QGraphicsSceneMouseEvent *event)
 */
void NmHsWidgetTitleRow::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    Q_UNUSED(event); 
    setFontsColor(true);
}

/*!
    mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
*/
void NmHsWidgetTitleRow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NM_FUNCTION;
    Q_UNUSED(event);
    setFontsColor(false);
    emit mailboxLaunchTriggered();
}

/*
 * NmHsWidgetTitleRow::event()
 */
bool NmHsWidgetTitleRow::event( QEvent *event )
{
    NM_FUNCTION;
    QEvent::Type eventType = event->type();
    if( eventType == HbEvent::ThemeChanged ){
        setFontsColor(false);
        return true;
    }
    return HbWidget::event(event);
}
