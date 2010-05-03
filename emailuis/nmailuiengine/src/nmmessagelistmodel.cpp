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

#include "nmuiengineheaders.h"


/*!
    \class NmMessageListModel
    \brief The NmMessageListModel class represents data model for mailbox list.
    @alpha

    The NmMessageListModel class uses NmMessageEnvelope class to represent data returned in its'
    data method to get all information needed for one list row for a widget by calling the method
    once.
*/

/*!
	Constructor
 */
NmMessageListModel::NmMessageListModel(NmDataManager &dataManager, QObject *parent)
:QStandardItemModel(parent),
mDataManager(dataManager),
mDividersActive(false),
mParentPtr(NULL)
{
    // Check for setting whether dividers are active
    // mDividersActive = ...
    // update also the test cases
}

/*!
	Destructor
 */
NmMessageListModel::~NmMessageListModel()
{
    clear();
}

/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
 */

QVariant NmMessageListModel::data(const QModelIndex &index, int role) const
{
    QVariant qVariant;
    if (index.isValid() && Qt::DisplayRole == role) {
    	NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        qVariant = QVariant::fromValue(item);
    }
    return qVariant;
}

/*!
    This refreshes the data of the model.
 */
void NmMessageListModel::refresh(
                    const NmId mailboxId, 
                    const NmId folderId,
                    const QList<NmMessageEnvelope*> &messageEnvelopeList)
{
    // Store current mailbox and folder id
    mCurrentMailboxId = mailboxId;
    mCurrentFolderId = folderId;
    // clear the model
    clear();
    // Add items
    NmMessageEnvelope* insertedMessage(NULL);
    int parentCount(0);
    int childCount(0);
    for (int i(0); i < messageEnvelopeList.count(); i++) {
        NmMessageEnvelope* nextMessage = messageEnvelopeList[i];
        if (mDividersActive && !messagesBelongUnderSameDivider(
        		insertedMessage, nextMessage)) {
            insertDividerIntoModel(nextMessage, parentCount);
            parentCount++;
            childCount = 0;
        }
        insertMessageIntoModel(nextMessage, childCount, false);
        insertedMessage = nextMessage;
        childCount++;
    }
    //NMLOG(QString("nmailuiengine: model row count = %1").arg(rowCount()));
}

/*!
    insertDividerIntoModel. Function inserts divider into model.
 */
void NmMessageListModel::insertDividerIntoModel(
    NmMessageEnvelope *messageForDivider,
    int parentRow)
{
    mParentPtr = createTitleDividerItem(messageForDivider);
    insertRow(parentRow,mParentPtr);
    mParentPtr->callEmitDataChanged();
}

/*!
    Function inserts message into model. Message can be inserted
    either to root or to parent. If parent is zero, item is added into root.
 */
void NmMessageListModel::insertMessageIntoModel(
		NmMessageEnvelope *messageEnvelope, int childRow, bool emitSignal)
{
    NmMessageListModelItem *mailItem = createMessageItem(messageEnvelope);
    if (mParentPtr) {
        // Add under parent divider
        mParentPtr->insertRow(childRow, mailItem);
    }
    else {
        // No dividers, add under root item
        insertRow(childRow,mailItem);
    }
    if (emitSignal) {
        mailItem->callEmitDataChanged();
    }
}

/*!
    Function checks whether the messages can be drawn under same title divider
    Check is done depending of the current sorting criteria
 */
bool NmMessageListModel::messagesBelongUnderSameDivider(
    const NmMessageEnvelope *message1,
    const NmMessageEnvelope *message2) const
{
    bool retVal(false);
    // First check pointer validity
    if (message1 && message2) {
        // Switch here for other sort modes 
        // Date based comparison
        QDateTime sentTime1 = message1->sentTime().toLocalTime();
        QDateTime sentTime2 = message2->sentTime().toLocalTime();
        if ( sentTime1.date() == sentTime2.date()) {
            retVal = true;
        }
    }
    return retVal;
}


/*!
    Function handles message events
 */
void NmMessageListModel::handleMessageEvent(
    NmMessageEvent event,
    const NmId &folderId,
    const QList<NmId> &messageIds)
{
    NmId mailboxId = mCurrentMailboxId;

    if (folderId == 0) {
        // const cast is used here because also the input parameter has to be changed
        const_cast<NmId&>(folderId) = mDataManager.getStandardFolderId(mailboxId, NmFolderInbox);
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageList, mailboxId, folderId);
        emit setNewParam(startParam);
    }
    if (mCurrentFolderId == 0) {
        // Folder id was not known at time mailbox opened
        // and we know subscription is valid only for current 
        // mailbox, because of events.
        mCurrentFolderId = folderId; 
    }
    // Handle events if they concern currently displayed folder
    if (mCurrentFolderId == folderId) {
        NMLOG(QString("nmailuiengine: handleMessageEvent"));
        if (NmMessageChanged == event) {
            for (int a(0); a < messageIds.count(); a++) {
                updateMessageEnvelope(mailboxId, folderId,messageIds[a]);
            }
        }
        else if (NmMessageCreated == event) {
            for (int a(0); a < messageIds.count(); a++) {
                if(!itemFromModel(messageIds[a])) {
                    insertNewMessageIntoModel(mailboxId, folderId, messageIds[a]);
                }
            }
        } else {
            for (int a(0); a < messageIds.count(); a++) {
                removeMessageFromModel(messageIds[a]);
                emit removeMessage(messageIds[a]);
            }
        }
    }
}

/*!
    Function inserts new message into correct position to model.
    A new title divider is created if it is needed.
 */
void NmMessageListModel::insertNewMessageIntoModel(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &msgId)
{
    NMLOG(QString("NmMessageListModel::insertNewMessageIntoModel"));
    // envelope ownership is here
    NmMessageEnvelope *newMsgEnve = mDataManager.envelopeById(mailboxId, folderId, msgId);
    if (newMsgEnve) {
        int rowIndex(getInsertionIndex(*newMsgEnve));
        if (!mDividersActive) {
            if (rowIndex > -1){
                insertMessageIntoModel(newMsgEnve, rowIndex, true);
            } else {
                NmMessageListModelItem *mailItem = createMessageItem(newMsgEnve);
                appendRow(mailItem);
                mailItem->callEmitDataChanged();
            }
        } else {
            // model contain items, message goes to the middle of the list
            if (rowIndex > 0) {
                int dividerIndex(0);
                QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
                QModelIndex index = items[rowIndex]->index();
                NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
                NmMessageEnvelope *replacedEnve = item->envelopePtr();
                if (messagesBelongUnderSameDivider(newMsgEnve,replacedEnve)) {
                    // find the index of the divider and insert message as child object
                    // dividerIndex is not actually used here but function sets
                    // the correct parent pointer
                    dividerIndex = dividerInsertionIndex(rowIndex);
                    insertMessageIntoModel(newMsgEnve, 0, true);
                } else {
                    // create new divider and find the correct index for it
                    dividerIndex = dividerInsertionIndex(rowIndex);
                    insertDividerIntoModel(newMsgEnve, dividerIndex);
                    // Insert message as a first child under new divider
                    insertMessageIntoModel(newMsgEnve, 0, true);
                }
                items.clear();
            }
            // first mail item in the model,
            // insert new divider to index 0 and new message as a child.
            else if (0 == rowIndex) {
                insertDividerIntoModel(newMsgEnve, rowIndex);
                insertMessageIntoModel(newMsgEnve, 0, true);
            }
            // this will go to the last item of the list and create new title divider
            else {
                mParentPtr = createTitleDividerItem(newMsgEnve);
                appendRow(mParentPtr);
                mParentPtr->callEmitDataChanged();
                NmMessageListModelItem *mailItem = createMessageItem(newMsgEnve);
                // Add under parent divider
                mParentPtr->appendRow(mailItem);
                mailItem->callEmitDataChanged();
            }
        }
    }
    delete newMsgEnve;
}

/*!
    Function check model index in which the new message should be inserted
    with the currently active sort mode.
 */
int NmMessageListModel::getInsertionIndex(
    const NmMessageEnvelope &envelope) const
{
    // NMLOG(QString("nmailuiengine: getInsertionIndex"));
    int ret(NmNotFoundError);
    
    // Date+descending sort mode based comparison.
    // Go through model and compare sent times with QDateTime >= comparison operation.
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    while (i < count && !found) {
        QModelIndex index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()) {
        	found = envelope.sentTime() >= item->envelope().sentTime();
        	if (found) {
                ret = i;
        	}
        }
        i++;
    }
    if (0 == count) {
        ret = NmNoError;
    }
    items.clear();
    return ret;
}

/*!
    Function finds preceding title divider index and sets the
    mParentPtr variable.
 */
int NmMessageListModel::dividerInsertionIndex(int messageIndex)
{
    bool found(false);
    int ret(NmNoError);
    QModelIndex index;
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    for (int i = messageIndex-1; i >= 0 && !found; i--) {
        index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if(item->itemType() == NmMessageListModelItem::NmMessageItemTitleDivider) {
            found = true;
            mParentPtr = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
            ret = index.row();
        }
    }
    items.clear();
    return ret;
}

/*!
    Create title divider item
 */
NmMessageListModelItem *NmMessageListModel::createTitleDividerItem(
		NmMessageEnvelope *messageForDivider)
{
    NmMessageListModelItem *item = new NmMessageListModelItem();
    item->setItemType(NmMessageListModelItem::NmMessageItemTitleDivider);

    QDate sentLocalDate = messageForDivider->sentTime().toLocalTime().date();
    QDate currentdate = QDate::currentDate();

    if (sentLocalDate == currentdate) {
        // NOTE: when dividers are taken in to use hbTrId macro should used here,
        // to be clarified how to access ts-file (located under ui component)
        item->setTitleDivider(tr("Today", "txt_nmailuiengine_divider_today"));
        item->setText(tr("Today", "txt_nmailuiengine_divider_today"));
    }
    else {
        QLocale locale;
        QString divider = locale.toString(sentLocalDate, QLocale::LongFormat);
        item->setTitleDivider(divider);
        item->setText(divider);
        }
    item->setEnvelope(*messageForDivider);
    item->setData(Hb::ParentItem, Hb::ItemTypeRole);
    return item;
}

/*!
    Create message item
 */
NmMessageListModelItem *NmMessageListModel::createMessageItem(
		NmMessageEnvelope *envelope)
{

    NmMessageListModelItem *mailItem = new NmMessageListModelItem();
    mailItem->setEnvelope(*envelope);
    mailItem->setItemType(NmMessageListModelItem::NmMessageItemMessage);
    mailItem->setData(Hb::StandardItem, Hb::ItemTypeRole);
    return mailItem;
}

/*!
    Returns divider state
 */
bool NmMessageListModel::dividersActive()
{
    return mDividersActive;
}

/*!
    Set divider state
 */
void NmMessageListModel::setDividers(bool active)
{
    mDividersActive = active;
}

/*!
   Change item property if differs
 */
void NmMessageListModel::setEnvelopeProperties(
    NmEnvelopeProperties property,
    const QList<NmId> &messageIds)
{
    for (int i(0); i < messageIds.count(); i++) {
        updateEnvelope(property, messageIds[i]);
    }
}

/*!
   Returns the id of the current mailbox
 */
NmId NmMessageListModel::currentMailboxId()
{
    return mCurrentMailboxId;
}

/*!
   Remove message from model if message exists in model
 */
void NmMessageListModel::removeMessageFromModel(const NmId &msgId)
{
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    while (!found && i < count) {
        QModelIndex index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()
                && msgId == item->envelope().id()) {
            found = true;
            // dividers are not active, just remove message
            if (!mDividersActive) {
                removeItem(index.row(), *item);
            } else {
                // check the type from previous item
                QModelIndex prevIndex = items[i-1]->index();
                NmMessageListModelItem* previous = static_cast<NmMessageListModelItem*>(itemFromIndex(prevIndex));
                if (NmMessageListModelItem::NmMessageItemTitleDivider == previous->itemType()) {
                    // previous item is title divider
                    if (i < (count-1)) {
                        // if this is not last item in the list, check next item
                        QModelIndex nextIndex = items[i+1]->index();
                        NmMessageListModelItem* next = static_cast<NmMessageListModelItem*>(itemFromIndex(nextIndex));
                        if (NmMessageListModelItem::NmMessageItemMessage == next->itemType()) {
                            // next item is message, divider should not be removed
                            removeItem(index.row(), *item);
                        } else {
                            // next item is another divider, remove also divider
                            removeItem(index.row(), *item);
                            removeItem(prevIndex.row(), *previous);
                        }
                    } else {
                        // this is the last message in list, remove also divider
                        removeItem(index.row(), *item);
                        removeItem(prevIndex.row(), *previous);
                    }
                }  else {
                    // previous item is message, title divider should not be deleted
                    removeItem(index.row(), *item);
                }
            }
        }
        i++;
    }
    items.clear();
}

/*!
   Helper function to remove row
 */
void NmMessageListModel::removeItem(int row, NmMessageListModelItem &item)
{
    QStandardItem *parent = item.parent();
    removeRow(row, indexFromItem(parent));
}

/*!
   Search item from model
 */
NmMessageListModelItem *NmMessageListModel::itemFromModel(const NmId &messageId)
{
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    NmMessageListModelItem *ret(NULL);
    while (i < count && !found) {
        NmMessageListModelItem *item =
            static_cast<NmMessageListModelItem*>(itemFromIndex(items[i]->index()));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()
            && messageId == item->envelope().id()) {
            found = true;
            ret = item;
        }
        i++;
    }
    return ret;
}

/*!
   Helper function for envelope comparison
 */
bool NmMessageListModel::changed(const NmMessageEnvelope &first, const NmMessageEnvelope &second)
{
    return first != second;
}

/*!
   Updates envelope if something is changed
 */
void NmMessageListModel::updateMessageEnvelope(const NmId &mailboxId,
        const NmId &folderId,
        const NmId &msgId)
{
    NmMessageListModelItem *item = itemFromModel(msgId);
    // envelope ownership is here
    NmMessageEnvelope *newEnvelope = mDataManager.envelopeById(mailboxId, folderId, msgId);
    if (item && newEnvelope) {
        if (changed(*item->envelopePtr(), *newEnvelope)) {
            // function takes envelope ownership 
            item->setEnvelope(newEnvelope);
        } else {
            delete newEnvelope;
            newEnvelope = NULL;
        }
    }
}

/*!
   Update envelope property
 */
void NmMessageListModel::updateEnvelope(NmEnvelopeProperties property, const NmId &msgId)
{
    NmMessageListModelItem *item = itemFromModel(msgId);
    if (item) {
        bool changed(false);
        NmMessageEnvelope *envelope = item->envelopePtr();
        switch (property)
        {
        case MarkAsRead:
        {
            if (!envelope->isRead()) {
                envelope->setRead(true);
                changed = true;
            }
            break;
        }
        case MarkAsUnread:
        {
            if (envelope->isRead()) {
                envelope->setRead(false);
                changed = true;
            }
            break;
        }
        default:
            break;
        }
        if (changed) {
            item->callEmitDataChanged();
        }
    }
}
