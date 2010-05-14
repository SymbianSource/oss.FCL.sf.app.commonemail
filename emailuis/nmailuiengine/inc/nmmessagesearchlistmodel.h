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

#ifndef NMMESSAGESEARCHLISTMODEL_H_
#define NMMESSAGESEARCHLISTMODEL_H_

#include <QSortFilterProxyModel>
#include <QVector>

#include "nmuienginedef.h" // For NMUIENGINE_EXPORT
#include "nmcommon.h"


class NMUIENGINE_EXPORT NmMessageSearchListModel : public QSortFilterProxyModel
{
    Q_OBJECT

public: // Construction and destruction

    NmMessageSearchListModel(QObject *parent = 0);

    ~NmMessageSearchListModel();


public:

    void clearSearchResults();

    int searchResultCount() const;


public: // From QSortFilterProxyModel

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


protected: // From QSortFilterProxyModel

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;


public slots:

    bool addSearchResult(const NmId &messageId);

    void refreshContent();


private: // Data

    QVector<NmId> mSearchResults;
};

#endif /* NMMESSAGESEARCHLISTMODEL_H_ */

// End of file.
