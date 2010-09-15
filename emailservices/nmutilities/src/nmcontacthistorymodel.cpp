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
* Description: Public implementation of Contact History Model API
*
*/

#include <QList>

#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModel)
// --------------------------------------------------------------------------
/*!
    Constructor for NmContactHistoryModel

    \param modelType ModelType to be used, Email / Messaging
*/
NmContactHistoryModel::NmContactHistoryModel(
    const NmContactHistoryModelType modelType)
{
    d_ptr = new NmContactHistoryModelPrivate(modelType);
    Q_CHECK_PTR(d_ptr);

    QObject::connect(d_ptr, SIGNAL(queryCompleted(int)), this,
        SLOT(handleQueryCompleted(int)));
}

/*!
    Destructor of NmContactHistoryModel
 */
NmContactHistoryModel::~NmContactHistoryModel()
{
    delete d_ptr;
}

/*!
    Queries contact databases from private implementation

    \param query String to be queried.
*/
void NmContactHistoryModel::query(const QString &query)
{
    d_ptr->queryDatabases(query);
}

/*!
     Signaled once query is completed, emits modelCompleted.
     Private Slot.

     \param err, 0 if building model was success
*/
void NmContactHistoryModel::handleQueryCompleted(int err)
{

    int lastUpdateIndex = (d_ptr->mPrivateItemList.count())-1;

    if (lastUpdateIndex != -1)
    {
        // Notify views that we are about to change model data.
        beginInsertRows(QModelIndex(),0,lastUpdateIndex);
        d_ptr->refreshDataModel();
        endInsertRows();

        // Emit dataChanged();
        bool validIndex = hasIndex ( lastUpdateIndex, 0 );

        if (validIndex)
        {
            dataChanged(index(0,0), index(lastUpdateIndex,0));
        }
    }

    emit modelCompleted(err);
}

/*!
    Returns the number of rows under the given parent.

    From QAbstractItemModel
*/
int NmContactHistoryModel::rowCount(const QModelIndex &parent) const
{
    return d_ptr->rowCount(parent);
}

/*!
    Returns the data stored under the given role for the item referred
    to by the index.

    From QAbstractItemModel
*/
QVariant NmContactHistoryModel::data(const QModelIndex &index, int role) const
{
    return d_ptr->data(index, role);
}

// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModel)
// --------------------------------------------------------------------------

