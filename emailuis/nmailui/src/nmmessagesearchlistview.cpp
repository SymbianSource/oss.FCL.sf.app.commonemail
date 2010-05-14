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

static const char *NMUI_MESSAGE_SEARCH_LIST_VIEW_XML = ":/docml/nmmessagesearchlistview.docml";
static const char *NMUI_MESSAGE_SEARCH_LIST_VIEW = "NmMessageListView";
static const char *NMUI_MESSAGE_SEARCH_LIST_TREE_LIST = "MessageTreeList";
static const char *NMUI_MESSAGE_SEARCH_LIST_NO_MESSAGES = "MessageListNoMessages";
static const char *NMUI_MESSAGE_SEARCH_LIST_INFO_LABEL = "InfoLabel";
static const char *NMUI_MESSAGE_SEARCH_LIST_LINE_EDIT = "LineEdit";
static const char *NMUI_MESSAGE_SEARCH_LIST_PUSH_BUTTON = "PushButton";


#include "nmuiheaders.h"


/*!
    \class NmMessageSearchListView
    \brief The view for searching messages.
*/


/*!
    Class constructor.
*/
NmMessageSearchListView::NmMessageSearchListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMessageListModel &msgListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam, application, parent),
  mApplication(application),
  mUiEngine(uiEngine),
  mMsgListModel(msgListModel),
  mDocumentLoader(documentLoader),
  mItemContextMenu(NULL),
  mMessageListWidget(NULL),
  mInfoLabel(NULL),
  mNoMessagesLabel(NULL),
  mLineEdit(NULL),
  mPushButton(NULL),
  mLongPressedItem(NULL),
  mViewReady(false),
  mSearchInProgress(false)
{
    loadViewLayout();
    initTreeView();
}


/*!
    Class destructor.
*/
NmMessageSearchListView::~NmMessageSearchListView()
{
    delete mDocumentLoader;

    mWidgetList.clear();

    if (mItemContextMenu){
        mItemContextMenu->clearActions();
    }

    delete mItemContextMenu;
}


/*!
    From NmBaseView.

    Returns the view ID.

    \return The view ID.
*/
NmUiViewId NmMessageSearchListView::nmailViewId() const
{
    return NmUiViewMessageSearchList;
}


/*!
    From NmBaseView.

    Does the lazy loading after the view layout has been loaded.
*/
void NmMessageSearchListView::viewReady()
{
    if (!mViewReady){
        // Set the mailbox name to the title pane.
        setViewTitle();

        // Refresh the list.
        QMetaObject::invokeMethod(this, "refreshList", Qt::QueuedConnection);

        // Highlight the search input.
        setSearchInputMode(NmHighlightedMode);

        mViewReady = true;
    }
}


/*!
    From NmMenuObserver.

    Handles action commands which are usually given by the user by selecting
    menu items.

    \param actionResponse The command details.
*/
void NmMessageSearchListView::handleActionCommand(NmActionResponse &actionResponse)
{
    // Handle options menu commands here.
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandUpdateMailboxName: {
                setViewTitle();
                break;
            }
            case NmActionResponseCommandMailboxDeleted: {
                mApplication.prepareForPopView();
                break;
            }
            default: {
                break;
            }
        }
    }
    // Handle context menu commands here.
    if (actionResponse.menuType() == NmActionContextMenu) {
        if (mLongPressedItem){
            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                mStartParam->mailboxId(), mStartParam->folderId(),
                mLongPressedItem->envelope().messageId());

            mApplication.enterNmUiView(startParam);
            mLongPressedItem = NULL;
       }
    }
}


/*!
    From NmBaseView.

    Loads the view layout from the XML file.
*/
void NmMessageSearchListView::loadViewLayout()
{
    // Use the document loader to load the view layout.
    bool ok(false);
    setObjectName(QString(NMUI_MESSAGE_SEARCH_LIST_VIEW));
    QObjectList objectList;
    objectList.append(this);

    // Pass the view to the document loader. Instead of creating a new view, the
    // document loader uses this view when the docml file is parsed.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_SEARCH_LIST_VIEW_XML, &ok);
    }

    if (ok && mWidgetList.count()) {
        // Get the message list widget.
        mMessageListWidget = qobject_cast<HbTreeView *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_LIST_TREE_LIST));

        if (mMessageListWidget) {
            NMLOG("NmMessageSearchListView: Message list widget loaded.");

            // Set the item prototype.
            mMessageListWidget->setItemPrototype(new NmMessageListViewItem());
            mMessageListWidget->setItemRecycling(true);
            mMessageListWidget->contentWidget()->setProperty("indentation", 0);
            mMessageListWidget->setScrollDirections(Qt::Vertical);
            mMessageListWidget->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMessageListWidget->setFrictionEnabled(true);
        }

        // Load the info label.
        mInfoLabel = qobject_cast<HbLabel *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_LIST_INFO_LABEL));

        if (mInfoLabel) {
            NMLOG("NmMessageSearchListView: Info label loaded.");
            mInfoLabel->setPlainText(hbTrId("txt_mail_subhead_inbox"));
            mInfoLabel->hide();
        }

        // Load the no messages label.
        mNoMessagesLabel = qobject_cast<HbLabel *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_LIST_NO_MESSAGES));

        if (mNoMessagesLabel) {
            NMLOG("NmMessageSearchListView: No messages label loaded.");
            mNoMessagesLabel->hide();
        }

        // Load the search panel.
        mLineEdit = qobject_cast<HbLineEdit *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_LIST_LINE_EDIT));
        if (mLineEdit) {
            connect(mLineEdit, SIGNAL(textChanged(QString)), 
                this, SLOT(criteriaChanged(QString)));
        }
        
        mPushButton = qobject_cast<HbPushButton *>(
            mDocumentLoader->findWidget(NMUI_MESSAGE_SEARCH_LIST_PUSH_BUTTON));
        if (mPushButton) {
			// button is disabled when line edit is empty
			mPushButton->setEnabled(false);
            // The push button both starts and stops the search.
            connect(mPushButton, SIGNAL(clicked()), this, SLOT(toggleSearch()));
            mPushButton->setIcon(HbIcon("qtg_mono_search"));
        }
    }
    else {
        NMLOG("NmMessageSearchListView: Failed to load widgets from XML!");
    }
}


/*!
    Initializes the tree view.
*/
void NmMessageSearchListView::initTreeView()
{
    // Get the mailbox widget pointer and set the parameters.
    if (mMessageListWidget) {
        connect(mMessageListWidget, SIGNAL(activated(const QModelIndex &)),
                this, SLOT(itemActivated(const QModelIndex &)));

        connect(mMessageListWidget, SIGNAL(longPressed(HbAbstractViewItem*, QPointF)),
                this, SLOT(showItemContextMenu(HbAbstractViewItem*, QPointF)));

        mMessageListWidget->setFocus();
        mItemContextMenu = new HbMenu();
    }

    // Clear the previous content if any.
    mMsgListModel.clear();
}


/*!
    Sets the title according to the name of the current mailbox.
*/
void NmMessageSearchListView::setViewTitle()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());

        if (meta){
            setTitle(meta->name());
        }
    }
}


/*!
    Toggles the visiblity between the message list widget and the "no messages"
    label.

    \param visible If true, will display the "no messages" label. If false,
                   will display the message list widget.
*/
void NmMessageSearchListView::noMessagesLabelVisibility(bool visible)
{
    if (visible) {
        // Hide the message list widget and display the "no messages" label.
        if (mMessageListWidget) {
            mMessageListWidget->hide();
        }

        if (mNoMessagesLabel && !mNoMessagesLabel->isVisible()) {
            mNoMessagesLabel->show();
        }
    }
    else {
        // Hide the "no messages" label and display the message list widget.
        if (mNoMessagesLabel && mNoMessagesLabel->isVisible()) {
            mNoMessagesLabel->hide();
        }

        if (mMessageListWidget) {
            mMessageListWidget->show();
        }
    }
}


/*!
    Sets the mode for the search input.

    \param mode The mode to set.
*/
void NmMessageSearchListView::setSearchInputMode(NmSearchInputMode mode)
{
    if (!mLineEdit) {
        // No line edit widget!
        return;
    }

    switch (mode) {
        case NmNormalMode: {
            mLineEdit->setEnabled(true);
            break;
        }
        case NmHighlightedMode: {
            mLineEdit->setEnabled(true);
            mLineEdit->setFocus();
            break;
        }
        case NmDimmedMode: {
            mLineEdit->setEnabled(false);
            break;
        }
    }
}


/*!
    From NmBaseView.

    Reloads the view contents with new start parameters. This method is
    typically used when the view is already open and an external view activation
    occurs for this same view.
*/
void NmMessageSearchListView::reloadViewContents(NmUiStartParam *startParam)
{
    // Check the start parameter's validity; message view cannot be updated if
    // the given parameter is zero.
    if (startParam&&startParam->viewId() == NmUiViewMessageSearchList &&
        startParam->folderId() != 0) {
        // Delete the existing start parameter data.
        delete mStartParam;
        mStartParam = NULL;

        // Store the new start parameter data.
        mStartParam = startParam;

        // Update the model with new parameters.
        mUiEngine.messageListModelForSearch(startParam->mailboxId());
        refreshList();

        // Refresh the mailbox name.
        setViewTitle();
    }
    else {
        // Invalid start parameter data! Unused start parameter needs to be
        // deleted.
        NMLOG("NmMessageSearchListView: Invalid message list start parameter!");
        delete startParam;
    }
}


/*!
    Called when text is changed in the edit field. If there is no search term
    in the edit field, the search button is dimmed and disabled. If the field
    contains text, the button can be clicked.
    
    \param text The text in the field after the modification.
*/
void NmMessageSearchListView::criteriaChanged(QString text) 
{
    NMLOG(QString("NmMessageSearchListView::criteriaChanged %1").arg(text));
    
    // Check if the button should be disabled/enabled.
    bool enabled = mPushButton->isEnabled();

    if (!enabled && !text.isEmpty()) {
        mPushButton->setEnabled(true);
    }
    else if (enabled && text.isEmpty()) {
        mPushButton->setEnabled(false);
    }
}


/*!
    Displays the item context menu. This method gets called if an item on the
    list is long pressed.
*/
void NmMessageSearchListView::showItemContextMenu(
    HbAbstractViewItem *listViewItem, const QPointF &coords)
{
    // Store long press item for later use with response.
    mLongPressedItem = 
        mMsgListModel.data(listViewItem->modelIndex(),
                           Qt::DisplayRole).value<NmMessageListModelItem*>();

    if (mItemContextMenu && mLongPressedItem &&
        mLongPressedItem->itemType() == NmMessageListModelItem::NmMessageItemMessage) {
        // Clear the previous menu actions.
        mItemContextMenu->clearActions();
        NmUiExtensionManager &extMngr = mApplication.extManager();
        QList<NmAction*> list;

        // Fetch the menu actions based on the selected item.
        NmMessageEnvelope *envelope = mLongPressedItem->envelopePtr();

        if (envelope){
            NmActionRequest request(this, NmActionContextMenu,
                NmActionContextViewMessageList, NmActionContextDataMessage,
                mStartParam->mailboxId(), mStartParam->folderId(),
                envelope->messageId(),QVariant::fromValue(envelope));

            extMngr.getActions(request, list);
        }
        else{
            NmActionRequest request(this, NmActionContextMenu,
                NmActionContextViewMessageList, NmActionContextDataMessage,
                mStartParam->mailboxId(), mStartParam->folderId(),
                envelope->messageId());

            extMngr.getActions(request, list);
        }

        for (int i(0); i < list.count(); ++i) {
            mItemContextMenu->addAction(list[i]);
        }

        mItemContextMenu->setPreferredPos(coords);
        mItemContextMenu->open();
    }
}


/*!
    Stores the given index and forwards the call to handleSelection(). This
    method gets called when an item on the list is selected.  

    \param index The index of the activated item.
*/
void NmMessageSearchListView::itemActivated(const QModelIndex &index)
{
    mActivatedIndex = index;
    QMetaObject::invokeMethod(this, "handleSelection", Qt::QueuedConnection);
}


/*!
    If the selected item is a message, will open the message.
*/
void NmMessageSearchListView::handleSelection()
{
    // Do expand/collapse for title divider items
    NmMessageListModelItem* modelItem = mMsgListModel.data(
        mActivatedIndex, Qt::DisplayRole).value<NmMessageListModelItem*>();

    if (modelItem &&
        modelItem->itemType() == NmMessageListModelItem::NmMessageItemMessage)
    {
        if (mSearchInProgress) {
            // Stop the search.
            toggleSearch();
        }

        // Open the message.
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
            mStartParam->mailboxId(), mStartParam->folderId(),
            modelItem->envelope().messageId());

        mApplication.enterNmUiView(startParam);
    }
}


/*!
    Displays the message list widtet if not visible and scrolls to the
    appropriate point on the list.
*/
void NmMessageSearchListView::itemsAdded(const QModelIndex &parent, int start, int end)
{
    NMLOG("nmailui: NmMessageSearchListView::itemsAdded()");

    Q_UNUSED(parent);
    Q_UNUSED(end);

    // Display the message list widget if not visible.
    noMessagesLabelVisibility(false);

    if (!start && mMessageListWidget) {
        QList<HbAbstractViewItem*> items = mMessageListWidget->visibleItems();

        if (items.count()) {
            QModelIndex index = items.at(0)->modelIndex();

            if (1 == index.row()) {
                QModelIndex previous = mMessageListWidget->modelIterator()->previousIndex(index);

                if (previous.isValid()) {
                    mMessageListWidget->scrollTo(previous);
                }
            }
        }
    }
}


/*!
    This method gets called when an item is removed from the list. If the list
    contains no items, the info label (if visible) is hidden and the "no
    messages" label is displayed. Otherwise, only the result count in the info
    label is updated.
*/
void NmMessageSearchListView::itemsRemoved()
{
    const int itemCount = mMsgListModel.rowCount();

    if (itemCount == 0) {
        if (mInfoLabel && mInfoLabel->isVisible()) {
            // Hide the info label.
            mInfoLabel->hide();
        }

        // Display the "no messages" label.
        noMessagesLabelVisibility(true);
    }
    else if (mInfoLabel && mInfoLabel->isVisible()) {
        // Update the search result count in the info label.
        QString resultsString(hbTrId("txt_mail_list_search_results"));
        resultsString.arg(itemCount);
        mInfoLabel->setPlainText(resultsString);
    }
}


/*!
    Refreshes the search list.
*/
void NmMessageSearchListView::refreshList()
{
    if (mMessageListWidget) {
        // Set the model.
        mMessageListWidget->setModel(
            static_cast<QStandardItemModel*>(&mMsgListModel));

        // Connect the signals.
        connect(&mMsgListModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
                this, SLOT(itemsAdded(const QModelIndex&, int, int)), Qt::UniqueConnection);

        connect(&mMsgListModel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
                this, SLOT(itemsRemoved()), Qt::UniqueConnection);

        connect(&mMsgListModel, SIGNAL(setNewParam(NmUiStartParam*)),
                this, SLOT(reloadViewContents(NmUiStartParam*)), Qt::UniqueConnection);

    }
}


/*!
    Starts and stops the search according to the current status.

    Starts search: Uses the input given by the user as a search string and
    starts an asynchronous search. Any previous search results are removed from
    the search list.

    Stops search: Sets the number of search results into the info label.
*/
void NmMessageSearchListView::toggleSearch()
{
    if (mSearchInProgress) {
        // Search is in progress - do cancel.
        mUiEngine.cancelSearch(mStartParam->mailboxId());
        handleSearchComplete();
    }
    else {
        // Do start the search.
        mSearchInProgress = true;
    
        // Clear previous results if any.
        mMsgListModel.clear();

        connect(&mUiEngine, SIGNAL(searchComplete()),
                this, SLOT(handleSearchComplete()), Qt::UniqueConnection);
    
        // Get the search input and start the search.
        QStringList searchStrings;

        if (mLineEdit) {
            searchStrings.append(mLineEdit->text());
        }

        mUiEngine.search(mStartParam->mailboxId(), searchStrings);

        // Hide the "no messages" label if visible and dim the search input.
        noMessagesLabelVisibility(false);
        setSearchInputMode(NmDimmedMode);

        // Display the info label.
        if (mInfoLabel) {
            mInfoLabel->setPlainText(hbTrId("txt_mail_list_searching"));
            mInfoLabel->show();
        }

        // Change the push button text.
        if (mPushButton) {
            mPushButton->setIcon(HbIcon("qtg_mono_search_stop"));
        }
    }
}


/*!
    If the search matched any messages, displays the search result count in the
    info label. If no messages were found, the method displays the "no messages"
    label. In either case, the search panel is updated.
*/
void NmMessageSearchListView::handleSearchComplete()
{
    NMLOG("NmMessageSearchListView::handleSearchComplete()");
    mSearchInProgress = false;
    
    // Change the push button text.
    if (mPushButton) {
        mPushButton->setIcon(HbIcon("qtg_mono_search"));
    }

    const int resultCount = mMsgListModel.rowCount();

    if (resultCount) {
        if (mInfoLabel) {
            // Display the result count on the info label.
            QString resultsString(hbTrId("txt_mail_list_search_results"));
            resultsString.arg(resultCount);
            mInfoLabel->setPlainText(resultsString);

            if (!mInfoLabel->isVisible()) {
                mInfoLabel->show();
            }
        }

        // Undim the search input.
        setSearchInputMode(NmNormalMode);
    }
    else {
        // No search results!
        if (mInfoLabel && mInfoLabel->isVisible()) {
            mInfoLabel->hide();
        }

        // Display the "no messags" label and highlight the search term.
        noMessagesLabelVisibility(true);
        setSearchInputMode(NmHighlightedMode);
    }
}


// End of file.
