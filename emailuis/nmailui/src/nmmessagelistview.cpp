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

static const char *NMUI_MESSAGE_LIST_VIEW_XML = ":/docml/nmmessagelistview.docml";
static const char *NMUI_MESSAGE_LIST_VIEW = "NmMessageListView";
static const char *NMUI_MESSAGE_LIST_TREE_LIST = "MessageTreeList";
static const char *NMUI_MESSAGE_LIST_NO_MESSAGES = "MessageListNoMessages";
static const char *NMUI_MESSAGE_LIST_FOLDER_LABEL = "folderLabel";
static const char *NMUI_MESSAGE_LIST_SYNC_ICON = "syncIcon";

static const int nmNoMessagesDelay = 200;

#include "nmuiheaders.h"

/*!
	\class NmMessageListView
	\brief Message list view
*/

/*!
    Constructor
*/
NmMessageListView::NmMessageListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMailboxListModel &mailboxListModel,
    NmMessageListModel &messageListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam, parent),
mApplication(application),
mMessageListWidget(NULL),
mUiEngine(uiEngine),
mMailboxListModel(mailboxListModel),
mMessageListModel(messageListModel),
mDocumentLoader(documentLoader),
mItemContextMenu(NULL),
mLongPressedItem(NULL),
mNoMessagesLabel(NULL)
{
    // Load view layout
    loadViewLayout();
    // Set mailbox name to title pane
    setMailboxName();
    // Init tree view
    initTreeView();
    // Refresh list
    refreshList();
}

/*!
    Destructor
*/
NmMessageListView::~NmMessageListView()
{
    delete mDocumentLoader;
    mWidgetList.clear();
    if (mItemContextMenu){
        mItemContextMenu->clearActions();
    }
    delete mItemContextMenu;
    mUiEngine.releaseMessageListModel(mStartParam->mailboxId());
}

/*!
    View layout loading from XML
*/
void NmMessageListView::loadViewLayout()
{
    // Use document loader to load the view
    bool ok = false;
    setObjectName(QString(NMUI_MESSAGE_LIST_VIEW));
    QObjectList objectList;
    objectList.append(this);
    // Pass the view to documentloader. Document loader uses this view
    // when docml is parsed, instead of creating new view.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_LIST_VIEW_XML, &ok);
    }

    if (ok == true && mWidgetList.count()) {
        // Get message list widget
        mMessageListWidget = qobject_cast<HbTreeView *>
            (mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_TREE_LIST));
        if (mMessageListWidget) {
            NMLOG("nmailui: List object loaded");
            // Set item prototype.
            mMessageListWidget->setItemPrototype(new NmMessageListViewItem());
            mMessageListWidget->setItemRecycling(true);
            mMessageListWidget->contentWidget()->setProperty("indentation", 0);
            mMessageListWidget->setScrollDirections(Qt::Vertical);
            mMessageListWidget->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMessageListWidget->setFrictionEnabled(true);
        }
        else {
            NMLOG("nmailui: List object loading failed");
        }
        mNoMessagesLabel = qobject_cast<HbLabel *>
            (mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_NO_MESSAGES));
        if (mNoMessagesLabel) {
            mNoMessagesLabel->setAlignment(Qt::AlignCenter);
            mNoMessagesLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
            mNoMessagesLabel->hide();
            mNoMessagesLabel->setPlainText(hbTrId("txt_mail_dblist_no_messages"));
        }
        else{
            NMLOG("nmailui: (no messages) object loading failed");
        }
        
        mFolderLabel = qobject_cast<HbLabel *>(mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_FOLDER_LABEL));
        if (mFolderLabel) {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_inbox"));
        }
        
        mSyncIcon = qobject_cast<HbLabel *>(mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_SYNC_ICON));
        if (mSyncIcon) {
            mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOffline));
        }

        // Connect options menu about to show to create options menu function
        QObject::connect(menu(), SIGNAL(aboutToShow()),
        		this, SLOT(createOptionsMenu()));
        
        QObject::connect(&mUiEngine, SIGNAL(syncStateEvent(NmSyncState, const NmId &)),
                                this, SLOT(handleSyncStateEvent(NmSyncState, const NmId &)));
        
        QObject::connect(&mUiEngine, SIGNAL(connectionEvent(NmConnectState, const NmId &)),
                                        this, SLOT(handleConnectionEvent(NmConnectState, const NmId &)));
        
        // Menu needs one dummy item so that aboutToShow signal is emitted.
        NmAction *dummy = new NmAction(0);
        menu()->addAction(dummy);
    }
    else {
        NMLOG("nmailui: Resource loading failed");
    }

    //create toolbar
    createToolBar();
}

/*!
    initTreeView
*/
void NmMessageListView::initTreeView()
{
    // Get mailbox widget pointer and set parameters
    if (mMessageListWidget){
        QObject::connect(mMessageListWidget,
                SIGNAL(activated(const QModelIndex &)), this,
                SLOT(itemActivated(const QModelIndex &)));
        QObject::connect(mMessageListWidget,
                SIGNAL(longPressed(HbAbstractViewItem*, QPointF)), this,
                SLOT(showItemContextMenu(HbAbstractViewItem*, QPointF)));
        mMessageListWidget->setFocus();
        mItemContextMenu = new HbMenu();
    }
}


/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmMessageListView::reloadViewContents(NmUiStartParam* startParam)
{
    // Check start parameter validity, message view cannot
    // be updated if given parameter is zero.
    if (startParam&&startParam->viewId()==NmUiViewMessageList&&
        startParam->folderId()!=0) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam=NULL;
        // Store new start parameter data
        mStartParam=startParam;
        // Update the model with new parameters
        mUiEngine.messageListModel(startParam->mailboxId(), startParam->folderId());
        refreshList();
        // Refresh the mailboxname 
        setMailboxName();
    }
    else {
        NMLOG("nmailui: Invalid message list start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
    }
}


/*!
    Return view id
*/
NmUiViewId NmMessageListView::nmailViewId() const
{
    return NmUiViewMessageList;
}

/*!
    Refresh list
*/
void NmMessageListView::refreshList()
{
    NmId mailboxId = mMessageListModel.currentMailboxId();
    if (mSyncIcon && mailboxId == mMessageListModel.currentMailboxId()) {
        if (mUiEngine.syncState(mailboxId) == Synchronizing) {
            mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconSynching));
        }
        else {
            if (mUiEngine.connectionState(mailboxId) == Connected) {
                mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOnline));
            }
            else {
                mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOffline));
            }
        }
    }
    
    if (mMessageListWidget) {
        mMessageListWidget->setModel(static_cast<QStandardItemModel*>(&mMessageListModel));
        QObject::connect(&mMessageListModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)),
                this, SLOT(itemsAdded(const QModelIndex&,int,int)));
        QObject::connect(&mMessageListModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)),
                this, SLOT(itemsRemoved()));
        QObject::connect(&mMessageListModel, SIGNAL(setNewParam(NmUiStartParam*)),
                this, SLOT(reloadViewContents(NmUiStartParam*)));
        
        if (mMessageListModel.rowCount()==0){
            // Single shot timer to display text smoothly after layout has been constructed
            QTimer::singleShot(nmNoMessagesDelay, this, SLOT(showNoMessagesText()));
        }
    }
}

void NmMessageListView::handleSyncStateEvent(NmSyncState syncState, const NmId & mailboxId)
{
    if (mSyncIcon && mailboxId == mMessageListModel.currentMailboxId()) {
        if (syncState == Synchronizing) {
            mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconSynching));
        }
        else {
            if (mUiEngine.connectionState(mailboxId) == Connected) {
                mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOnline));
            }
            else {
                mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOffline));
            }
        }
    }
}

void NmMessageListView::handleConnectionEvent(NmConnectState connectState, const NmId &mailboxId)
{
    if (mSyncIcon && mailboxId == mMessageListModel.currentMailboxId() && mUiEngine.syncState(mailboxId) != Synchronizing) {
        if (connectState == Connected) {
            mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOnline));
        }
        else {
            mSyncIcon->setIcon(NmIcons::getIcon(NmIcons::NmIconOffline));
        }
    }
}

/*!
    Long keypress handling
*/
void NmMessageListView::showItemContextMenu(HbAbstractViewItem *listViewItem, const QPointF &coords)
{
    // Store long press item for later use with response
    mLongPressedItem = mMessageListModel.data(
            listViewItem->modelIndex(), Qt::DisplayRole).value<NmMessageListModelItem*>();
    if (mItemContextMenu && mLongPressedItem && mLongPressedItem->itemType() ==
        NmMessageListModelItem::NmMessageItemMessage) {

        // Clear previous items from context menu
        mItemContextMenu->clearActions();
        NmUiExtensionManager &extMngr = mApplication.extManager();
        QList<NmAction*> list;
        // Fetch items from extension based on item
        NmMessageEnvelope *envelope = mLongPressedItem->envelopePtr();
        if (envelope){
            NmActionRequest request(this, NmActionContextMenu, NmActionContextViewMessageList,
                    NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
                    envelope->id(),QVariant::fromValue(envelope));
            extMngr.getActions(request, list);
        }
        else{
            NmActionRequest request(this, NmActionContextMenu, NmActionContextViewMessageList,
                    NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
                    envelope->id());
            extMngr.getActions(request, list);
        }
        for (int i=0;i<list.count();i++) {
            mItemContextMenu->addAction(list[i]);
        }
        mItemContextMenu->exec(coords);
    }
}

/*!
    Item activation handling. Expand/collapse for title dividers
    opening for mail items
*/

void NmMessageListView::itemActivated(const QModelIndex &index)
{
    // Do expand/collapse for title divider items
	NmMessageListModelItem* modelItem = mMessageListModel.data(
        		index, Qt::DisplayRole).value<NmMessageListModelItem*>();
    if (modelItem && modelItem->itemType()==
        NmMessageListModelItem::NmMessageItemTitleDivider) {
        if (!mMessageListWidget->isExpanded(index)) {
            mMessageListWidget->setExpanded(index, true);
            modelItem->setExpanded(true);
        }
        else {
            mMessageListWidget->setExpanded(index, false);
            modelItem->setExpanded(false);
        }
    }
    if (modelItem && modelItem->itemType() == NmMessageListModelItem::NmMessageItemMessage)
    {
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
            mStartParam->mailboxId(), mStartParam->folderId(),
            modelItem->envelope().id());
        mApplication.enterNmUiView(startParam);
    }
}


/*!
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmMessageListView::createOptionsMenu()
{
    menu()->clearActions();
    NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewMessageList,
    		NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    QList<NmAction*> list;
    extMngr.getActions(request, list);
    for (int i=0;i<list.count();i++) {
        menu()->addAction(list[i]);
    }
}

/*!
    handleActionCommand. From NmMenuObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmMessageListView::handleActionCommand(NmActionResponse &actionResponse)
{
    // Handle context menu commands here
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandNewMail: {
                // Check that given start response has mailbox and folder id's
                if (actionResponse.mailboxId()!=0){
                    NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                            actionResponse.mailboxId(), mStartParam->folderId());
                    // startParam ownerhips transfers
                    mApplication.enterNmUiView(startParam);
                }
                break;
            }
            case NmActionResponseCommandUpdateMailboxName: {
                setMailboxName();
                break;
            }
            case NmActionResponseCommandMailboxDeleted: {
                mApplication.popView();
				break;
            }
            default: {
                break;
            }
        }
    }

    // Handle context menu commands here
    else if (actionResponse.menuType()==NmActionContextMenu){
        switch (actionResponse.responseCommand()){
           case NmActionResponseCommandOpen:{
               if (mLongPressedItem){
                   NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                       mStartParam->mailboxId(), mStartParam->folderId(),
                       mLongPressedItem->envelope().id());
                   mApplication.enterNmUiView(startParam);
                   mLongPressedItem=NULL;
                   }
               }
               break;
           //temporary solution..
           case NmActionResponseCommandForward:{
                if (mLongPressedItem){
                   NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                       mStartParam->mailboxId(), mStartParam->folderId(),
                       mLongPressedItem->envelope().id());
                   mApplication.enterNmUiView(startParam);
                   mLongPressedItem=NULL;
                   }
               }
               break;
           default:
               break;
        }
    }
    // Handle toolbar commands here
    else if ( actionResponse.menuType() == NmActionToolbar ) {
        if ( actionResponse.responseCommand() == NmActionResponseCommandNewMail ) {
            // Check that given start response has mailbox and folder id's
            if (actionResponse.mailboxId()!=0){
                if (mUiEngine.isSendingMessage()) {
                    // sending is ongoing so just show a note
                    QString noteText = hbTrId("txt_mail_dialog_still_sending");
                    
                    // get message subject from the message being sent
                    const NmMessage *message = mUiEngine.messageBeingSent();
                    if (message) {
                        noteText = noteText.arg(NmUtilities::truncate(message->envelope().subject(), 20));
                    }
                    HbMessageBox::warning(noteText);
                }
                else {
                    NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                            actionResponse.mailboxId(), mStartParam->folderId());
                    // startParam ownerhips transfers
                    mApplication.enterNmUiView(startParam);
                }
            }
        }
    }


}


/*!
    Set mailbox name to title
*/
void NmMessageListView::setMailboxName()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta){
            setTitle(meta->name());
        }
    }
}

/*!
    createToolBar. Function asks menu commands from extension
    to be added to toolbar owned by the HbView. Also toolbar
    specific icons are added in this function.
*/
void NmMessageListView::createToolBar()
{
    HbToolBar *tb = toolBar();
    if (!tb) {
        return;
    }
    tb->clearActions();
    NmActionRequest request(this, NmActionToolbar, NmActionContextViewMessageList,
            NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (!&extMngr) {
        return;
    }
    QList<NmAction *> list;
    extMngr.getActions(request, list);
    for (int i = 0; i < list.count(); i++) {
        tb->addAction(list[i]);
    }
}

/*!
    Observe items added
*/
void NmMessageListView::itemsAdded(const QModelIndex &parent, int start, int end)
{
    NMLOG("nmailui: NmMessageListView::itemsAdded");
    Q_UNUSED(parent);
    Q_UNUSED(end);
    if (mNoMessagesLabel && mNoMessagesLabel->isVisible()) {
        mNoMessagesLabel->hide();
        mMessageListWidget->show();
    }
    if (0 == start && mMessageListWidget) {
        QList<HbAbstractViewItem*> items = mMessageListWidget->visibleItems();
        if (items.count()) {
            QModelIndex index = items.at(0)->modelIndex();
            if (1 == index.row()) {
                QModelIndex previous = mMessageListWidget->previousIndex(index);
                if (previous.isValid()) {
                    mMessageListWidget->scrollTo(previous);
                }
            }
        }
    }
}

/*!
    Observe items removed
*/
void NmMessageListView::itemsRemoved()
{
    if (mNoMessagesLabel && mMessageListModel.rowCount() == 0){
        showNoMessagesText();
    }
}

/*!
    Show "(no messages)" text at the middle of the screen.
*/
void NmMessageListView::showNoMessagesText()
{
    if (mNoMessagesLabel) {
        mMessageListWidget->hide();
        mNoMessagesLabel->show();
    }
}
