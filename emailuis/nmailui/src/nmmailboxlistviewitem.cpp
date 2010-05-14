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

#include "nmuiheaders.h"

/*!
    \class NmMailboxListViewItem
    \brief Mailbox list view item inherited from HbListViewItem
*/

/*!
    Constructor
*/
NmMailboxListViewItem::NmMailboxListViewItem(QGraphicsItem *parent)
: HbListViewItem(parent),
mLayout(NULL)
{
}

/*!
    Destructor
*/
NmMailboxListViewItem::~NmMailboxListViewItem()
{
}

/*!
    Item creation
*/
HbListViewItem* NmMailboxListViewItem::createItem()
{
    return new NmMailboxListViewItem(*this);
}

/*!
    update child items
*/
void NmMailboxListViewItem::updateChildItems()
{
    // To create primitives
    HbListViewItem::updateChildItems();

    EmailMailboxInfo mailboxInfo;
    
    NmMailboxMetaData *mailbox =
            modelIndex().data(Qt::DisplayRole).value<NmMailboxMetaData*>();
    if (mailbox){
        mLayout = new QGraphicsLinearLayout(Qt::Horizontal, 0);
        mLayout->setContentsMargins(0,0,0,0);

        HbLabel *mbIcon = new HbLabel();
        mbIcon->setObjectName("MailboxListViewMailboxIcon");
        
        QString domainName = mailbox->address();
        QString iconName = mailboxInfo.mailboxIcon(domainName);
        mbIcon->setIcon(HbIcon(iconName));
        mbIcon->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        HbLabel *mbName = new HbLabel();
        mbName->setObjectName("MailboxListViewMailboxName");
        mbName->setPlainText(mailbox->name());
        mbName->setAlignment(Qt::AlignVCenter);
        mbName->setFontSpec(HbFontSpec(HbFontSpec::Primary));
        mbName->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

        mLayout->addItem(mbIcon);
        mLayout->addItem(mbName);
        mLayout->setItemSpacing(0,0);

        setLayout(mLayout); // Ownership is transferred
    }
}

/*!
    boolean value indicating model index availability
*/
bool NmMailboxListViewItem::canSetModelIndex(const QModelIndex &index)
{
    Q_UNUSED(index);
    return true;
}

/*!
    polishEvent. Call to base class polish event.
*/
void NmMailboxListViewItem::polishEvent()
{
    QGraphicsWidget::polishEvent();
}

/*!
    paint. Paint function for line painting.
*/
void NmMailboxListViewItem::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    Q_UNUSED(painter);
}
