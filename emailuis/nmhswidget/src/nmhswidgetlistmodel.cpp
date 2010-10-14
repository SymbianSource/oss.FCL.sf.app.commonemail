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

#include <hbnamespace.h>
#include <nmmessageenvelope.h>
#include "nmhswidgetlistmodel.h"
#include "nmhswidgetlistmodelitem.h"

/*!
    \class NmHsWidgetListModel
    \brief The NmHsWidgetListModel class represents data model for message list.
    @alpha

    The NmHsWidgetListModel class uses NmHsWidgetListModelItem class to represent data returned in its'
    data method to get all information needed for one list row for a widget by calling the method
    once.
*/

/*!
    Constructor
 */
NmHsWidgetListModel::NmHsWidgetListModel(QObject *parent)
:QStandardItemModel(parent)
{
    NM_FUNCTION;
}

/*!
	Destructor
 */
NmHsWidgetListModel::~NmHsWidgetListModel()
{
    NM_FUNCTION;
    
    clear();
}

/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
 */
QVariant NmHsWidgetListModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    
    QVariant qVariant;
    if (index.isValid() && Qt::DisplayRole == role) {
        NmHsWidgetListModelItem *item = static_cast<NmHsWidgetListModelItem*>(itemFromIndex(index));
        NmMessageEnvelope *mailbox = item->itemMetaData();
        qVariant = QVariant::fromValue(mailbox);
    }
    return qVariant;
}


/*!
    Create mailbox item
    \param mailbox
 */
NmHsWidgetListModelItem* NmHsWidgetListModel::createMessageListModelItem(const NmMessageEnvelope* envelope)
{
    NM_FUNCTION;
    
    NmMessageEnvelope *newMeta = new NmMessageEnvelope(*envelope);
    NmHsWidgetListModelItem *item = new NmHsWidgetListModelItem();
    item->setItemMetaData(newMeta);
    item->setData(Hb::StandardItem, Hb::ItemTypeRole);
    return item;
}


/*!
    Function determines model index in which the new message should be inserted.
*/
int NmHsWidgetListModel::getInsertionIndex(
    const NmMessageEnvelope &envelope) const
{
    NM_FUNCTION;
   
   int ret(NmNotFoundError);
   
   // Date+descending sort mode based comparison.
   // Go through model and compare sent times with QDateTime >= comparison operation.
   QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
   int count(items.count());
   bool found(false);
   int i(0);
   while (i < count && !found) {
       QModelIndex index = items[i]->index();
       NmHsWidgetListModelItem *item = static_cast<NmHsWidgetListModelItem*>(itemFromIndex(index));
       found = envelope.sentTime() >= item->itemMetaData()->sentTime();
       if (found) {
           ret = i;
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
    Inserts given messages into the model
*/
void NmHsWidgetListModel::addMessages(const QList<NmMessageEnvelope*> &messageEnvs)
{
    NM_FUNCTION;
    int orig_count = rowCount();
    foreach(NmMessageEnvelope* env, messageEnvs){
        int insertionIndex = getInsertionIndex(*env);
        NmHsWidgetListModelItem *newItem = createMessageListModelItem(env);
        insertRow(insertionIndex, newItem);
    }

    if( !messageEnvs.isEmpty() ){
        if (orig_count == 0){
            emit modelIsEmpty(false);
        }
        emit messagesAddedToModel(); //emit messages added to model 
    }
}

/*!
    This refreshes the data of the model.
    NOTE: safe guard any call to this function with try-catch.
 */
void NmHsWidgetListModel::refresh(const QList<NmMessageEnvelope*> &envelopeList)
{
    NM_FUNCTION;
    
    clear();
    foreach(NmMessageEnvelope *env, envelopeList){
       NmHsWidgetListModelItem *item = createMessageListModelItem(env);
       appendRow(item);
    }
    
    //As we refresh all data, emit signal in any case
    if( rowCount() == 0 ){
        emit modelIsEmpty(true);
    }else{
        emit modelIsEmpty(false);
    }
}

/*!
    Updates existing messages in model with given envelopes
*/
void NmHsWidgetListModel::updateMessages(const QList<NmMessageEnvelope*> &messageEnvs)
{
    NM_FUNCTION; 
    QList<QStandardItem*> modelItemList = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    foreach(NmMessageEnvelope *env, messageEnvs){
        foreach (QStandardItem *it, modelItemList){
            QModelIndex index = it->index();
            NmHsWidgetListModelItem *item = static_cast<NmHsWidgetListModelItem*>(itemFromIndex(index));
            if (env->messageId() == item->itemMetaData()->messageId()) {
                item->setItemMetaData(env);
                break;
            }
        }
    }
}


/*!
    Removes the given messages from the model
*/
void NmHsWidgetListModel::removeMessages(const QList<NmId> &messageIds)
{
    NM_FUNCTION; 
    foreach(NmId msgId, messageIds){
        //lets refresh the item list every time, so that it really is up to date after each iteration 
        //(and the count stays valid!)
        QList<QStandardItem*> modelItemList = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
        foreach(QStandardItem *it, modelItemList){
            QModelIndex index = it->index();
            NmHsWidgetListModelItem *item = static_cast<NmHsWidgetListModelItem*>(itemFromIndex(index));
            if (msgId == item->itemMetaData()->messageId()) {
                removeRow(index.row());
                break;
            }
        }
    }
    
    //if model is empty after removing messages, signal it.
    if( rowCount() == 0 ){
        emit modelIsEmpty(true);  
    }
    
}

/*!
    Clears the model and emits modelIsEmpty signal
*/
void  NmHsWidgetListModel::removeAllMessages()
{
    clear();
    emit modelIsEmpty(true);
}
