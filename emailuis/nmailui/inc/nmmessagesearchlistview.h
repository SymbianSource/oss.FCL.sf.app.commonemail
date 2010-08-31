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

#ifndef NMMESSAGESEARCHLISTVIEW_H_
#define NMMESSAGESEARCHLISTVIEW_H_

#include "nmbaseview.h"
#include <nmactionobserver.h>
#include <QModelIndex>

class HbAbstractViewItem;
class HbDocumentLoader;
class HbGroupBox;
class HbLabel;
class HbLineEdit;
class HbMenu;
class HbPushButton;
class HbTreeView;

class NmActionResponse;
class NmApplication;
class NmMessageListModel;
class NmMessageListModelItem;
class NmUiEngine;
class NmUiStartParam;


class NmMessageSearchListView : public NmBaseView, public NmActionObserver
{
    Q_OBJECT

public:

    NmMessageSearchListView(NmApplication &application,
                            NmUiStartParam *startParam,
                            NmUiEngine &uiEngine,
                            NmMessageListModel &msglistModel,
                            HbDocumentLoader *documentLoader,
                            QGraphicsItem *parent = 0);
    ~NmMessageSearchListView();


public: // From NmBaseView

    NmUiViewId nmailViewId() const;
    void viewReady();


public: // From NmActionObserver

    void handleActionCommand(NmActionResponse &menuResponse);


private: // Data types

    enum NmSearchInputMode {
        NmNormalMode = 0,
        NmHighlightedMode,
        NmDimmedMode
    };


private:

    void loadViewLayout();
    void initTreeView();
    void setViewTitle();
    void noMessagesLabelVisibility(bool visible);
    void updateSearchResultCountInfo();
    void setSearchInputMode(NmSearchInputMode mode);


public slots:

    void reloadViewContents(NmUiStartParam *startParam);


private slots:

    void criteriaChanged(QString text);
    void showItemContextMenu(HbAbstractViewItem *index, const QPointF &coords);
    void itemActivated(const QModelIndex &index);
    void handleSelection();
    void itemsAdded(const QModelIndex &parent, int start, int end);
    void itemsRemoved();
    void refreshList();
    void toggleSearch();
    void handleSearchComplete();


private: // Data

    NmApplication &mApplication;
    NmUiEngine &mUiEngine;
    NmMessageListModel &mMsgListModel;
    HbDocumentLoader *mDocumentLoader; // Owned
    QObjectList mWidgetList;
    HbMenu *mItemContextMenu; // Owned
    HbTreeView *mMessageListWidget; // Not owned
    HbGroupBox *mInfoLabel; // Not owned
    HbLabel *mNoMessagesLabel; // Not owned
    HbLineEdit *mLineEdit; // Not owned
    HbPushButton *mPushButton; // Not owned
    NmMessageListModelItem *mLongPressedItem; // Not owned
	QModelIndex mActivatedIndex;
    bool mViewReady;
    bool mSearchInProgress;
};


#endif /* NMMESSAGESEARCHLISTVIEW_H_ */

// End of file.
