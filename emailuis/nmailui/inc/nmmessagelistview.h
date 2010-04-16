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

#ifndef NMMESSAGELISTVIEW_H_
#define NMMESSAGELISTVIEW_H_

#include "nmbaseview.h"
#include <nmactionobserver.h>

class HbTreeView;
class HbLabel;
class HbMenu;
class NmUiEngine;
class HbDocumentLoader;
class NmApplication;
class NmUiStartParam;
class NmMessageListModel;
class NmMailboxListModel;
class HbAbstractViewItem;
class NmActionResponse;
class NmMessageListModelItem;
class QModelIndex;
class HbIconItem;

class NmMessageListView : public NmBaseView, public NmActionObserver
{
    Q_OBJECT
public:
    NmMessageListView(
        NmApplication &application,
        NmUiStartParam *startParam,
        NmUiEngine &uiEngine,
        NmMailboxListModel &mailboxListModel,
        NmMessageListModel &messageListModel,
        HbDocumentLoader *documentLoader,
        QGraphicsItem *parent = 0);
    ~NmMessageListView();
    NmUiViewId nmailViewId() const;

public: // From NmActionObserver
    void handleActionCommand(NmActionResponse &menuResponse);

public slots:
    void reloadViewContents(NmUiStartParam *startParam);

private slots:
    void showItemContextMenu(HbAbstractViewItem *index, const QPointF &coords);
    void itemActivated(const QModelIndex &index);
    void createOptionsMenu();
    void itemsAdded(const QModelIndex &parent, int start, int end);
    void itemsRemoved();
    void showNoMessagesText();
    void handleSyncStateEvent(NmSyncState syncState, const NmId & mailboxId);
    void handleConnectionEvent(NmConnectState connectState, const NmId &mailboxId);

private:
    void refreshList();
    void loadViewLayout();
    void initTreeView();
    void setMailboxName();
    void createToolBar();

private:
    NmApplication &mApplication;
    HbTreeView *mMessageListWidget;         // Not owned
    QObjectList mWidgetList;
    NmUiEngine &mUiEngine;
    NmMailboxListModel &mMailboxListModel;
    NmMessageListModel &mMessageListModel;
    HbDocumentLoader *mDocumentLoader;      // Owned
    HbMenu *mItemContextMenu;               // Owned
    NmMessageListModelItem *mLongPressedItem;  // Not owned
    HbLabel *mNoMessagesLabel;              // Not owned
    HbLabel *mFolderLabel;                  // Not owned
    HbLabel *mSyncIcon;                     // Not owned
};

#endif /* NMMESSAGELISTVIEW_H_ */
