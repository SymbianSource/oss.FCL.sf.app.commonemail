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

#ifndef NMHSWIDGETLISTMODEL_H_
#define NMHSWIDGETLISTMODEL_H_

#include <QStandardItemModel>

class NmMessageEnvelope;
class NmHsWidgetListModelItem;
class NmId;

class NmHsWidgetListModel : public QStandardItemModel
{
Q_OBJECT
public:

    NmHsWidgetListModel(QObject *parent = 0);
    virtual ~NmHsWidgetListModel();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    
public slots:
    void refresh(const QList<NmMessageEnvelope*> &messageEnvs);
    void addMessages(const QList<NmMessageEnvelope*> &messageEnvs);
    void updateMessages(const QList<NmMessageEnvelope*> &messageEnvs);
    void removeMessages(const QList<NmId> &messageIds);
    void removeAllMessages();

signals:
    void messagesAddedToModel(); //emitted only when single items are inserted to model
    void modelIsEmpty(bool isEmpty);

private:
    NmHsWidgetListModelItem *createMessageListModelItem(const NmMessageEnvelope* env);
    int getInsertionIndex(const NmMessageEnvelope &envelope) const;
};

#endif /* NMHSWIDGETLISTMODEL_H_ */
