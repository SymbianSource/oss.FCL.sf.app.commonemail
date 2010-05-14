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

#include "nmuiengineheaders.h"

/*!
    \class NmMessageSearchListModel
    \brief A proxy model used to filter out messages from the message list model
           that are not amongst the search results.
*/

/*!
    Class constructor.
*/
NmMessageSearchListModel::NmMessageSearchListModel(QObject *parent)
: QSortFilterProxyModel(parent)
{
    // No implementation required.
}


/*!
    Class destructor.
*/
NmMessageSearchListModel::~NmMessageSearchListModel()
{
    clearSearchResults();
}


/*!
    Clears search results and removes the accepted source model items from this
    proxy model.
*/
void NmMessageSearchListModel::clearSearchResults()
{
    mSearchResults.clear();
    invalidateFilter();
}


/*!
    Returns the number of search results i.e. IDs in the container.

    \return The number of search results.
*/
int NmMessageSearchListModel::searchResultCount() const
{
    return mSearchResults.count();
}


/*!
    From QSortFilterProxyModel.
    Returns the data from the source model according to the given proxy index.

    \param index The index of this proxy model.
    \param role The item role.

    \return The item data wrapped in a QVariant.
*/
QVariant NmMessageSearchListModel::data(const QModelIndex &index,
                                        int role /* = Qt::DisplayRole */) const
{
    QVariant retVal;
    QAbstractItemModel *model = sourceModel();

    if (model) {
        // Return the data from the source model by mapping the given index
        // in respect to the source model.
        QModelIndex sourceIndex = mapToSource(index);
        retVal = model->data(sourceIndex, role);
    }
    else {
        // Since no source model set, use the base class method implementation.
        retVal = QSortFilterProxyModel::data(index, role);
    }

    return retVal;
}


/*!
    From QSortFilterProxyModel.
    Filters the items of the source model depending on the current search results.

    \param source_row A source model row index.
    \param source_parent The parent model index of the row.

    \return True if the row should be accepted to the search list model, false
            otherwise.
*/
bool NmMessageSearchListModel::filterAcceptsRow(int source_row,
                                                const QModelIndex &source_parent) const
{
    bool accept(false);

    // Get the source model.
    NmMessageListModel *model = qobject_cast<NmMessageListModel*>(sourceModel());

    if (model) {
        // Extract the model item corresponding the given row and index.
        QModelIndex listIndex = model->index(source_row, 0, source_parent);
        QVariant qVariant = model->data(listIndex);
        NmMessageListModelItem *item = qVariant.value<NmMessageListModelItem*>();

        if (item) {
            // Get the message ID from the item and compare it to the current
            // search results.
            const NmId itemId = item->envelope().messageId();

            if (mSearchResults.contains(itemId)) {
                // The message ID matches the search result => do accept the row.
                accept = true;
            }
        }
    }

    return accept;
}


/*!
    Adds the given message ID to the search results.

    \param messageId The ID to add.

    \return True if the given ID was added, false otherwise.
*/
bool NmMessageSearchListModel::addSearchResult(const NmId &messageId)
{
    bool resultAdded(false);

    // Make sure not to add the same ID twice.
    if (!mSearchResults.contains(messageId)) {
        // Add the given ID to the results.
        mSearchResults.append(messageId);
        resultAdded = true;

        // Invalidate the filter in order to update the model. This forces
        // filterAcceptsRow() to be ran again for each row in the source model.
        invalidateFilter();
    }

    return resultAdded;
}


/*!
    Refreshes the content by forcing the model to re-process the source model
    items using the current filter (search results).
*/
void NmMessageSearchListModel::refreshContent()
{
    invalidateFilter();
}


// End of file.
