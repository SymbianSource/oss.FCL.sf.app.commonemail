/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
mNoMessagesLabel(NULL),
mFolderLabel(NULL),
mSyncIcon(NULL),
mViewReady(false),
mCurrentFolderType(NmFolderInbox)
{
    // Load view layout
    loadViewLayout();
    // Init tree view
    initTreeView();
    // Set folder name
    setFolderName();
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
            mNoMessagesLabel->hide();
        }
        else{
            NMLOG("nmailui: (no messages) object loading failed");
        }

        // Create folder label and set inbox text for it
        mFolderLabel = qobject_cast<HbLabel *>(mDocumentLoader->findWidget(NMUI_MESSAGE_LIST_FOLDER_LABEL));

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
}

/*!
    Lazy loading when view layout has been loaded
*/
void NmMessageListView::viewReady()
{
    if (!mViewReady){
        // Set mailbox name to title pane
        setMailboxName();
        //create toolbar
        createToolBar();
        // Refresh list
        QMetaObject::invokeMethod(this, "refreshList", Qt::QueuedConnection);
        mViewReady=true;
    }
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
        // Show message list if it is hidden
        mMessageListWidget->show();
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
            showNoMessagesText();
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

/*!
    Connection event handling
*/
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
    folder selection handling within current mailbox
*/
void NmMessageListView::folderSelected(NmId mailbox, NmId folder)
{
    // Reloas view contents with new startparams
    if (mStartParam){
        NmUiStartParam* startParam = new NmUiStartParam(NmUiViewMessageList,mailbox,folder);
        // Hide message lis for redraw
        mMessageListWidget->hide();
        // Reload view, ownership of the startparams is passed and old startparams
        // are deleted within reloadViewContents function
        reloadViewContents(startParam);
        //Set folder text to status bar
        setFolderName();
        // Store active folder type
        mCurrentFolderType = NmFolderInbox;
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
                    envelope->messageId(),QVariant::fromValue(envelope));
            extMngr.getActions(request, list);
        }
        else{
            NmActionRequest request(this, NmActionContextMenu, NmActionContextViewMessageList,
                    NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
                    envelope->messageId());
            extMngr.getActions(request, list);
        }
        for (int i=0;i<list.count();i++) {
            mItemContextMenu->addAction(list[i]);
        }
        mItemContextMenu->setPreferredPos(coords);
        mItemContextMenu->open(this, SLOT(contextButton(NmActionResponse&)));
    }
}

/*!
    Slot. Signaled when menu option is selected
*/
void NmMessageListView::contextButton(NmActionResponse &result)
{
    // Handle context menu commands here
    if (result.menuType()==NmActionContextMenu){
        switch (result.responseCommand()){
           case NmActionResponseCommandOpen:{
               if (mLongPressedItem){
                   NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                       mStartParam->mailboxId(), mStartParam->folderId(),
                       mLongPressedItem->envelope().messageId());
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
                       mLongPressedItem->envelope().messageId());
                   mApplication.enterNmUiView(startParam);
                   mLongPressedItem=NULL;
                   }
               }
               break;
           default:
               break;
        }
    }
}

/*!
    Item activation handling. Expand/collapse for title dividers
    opening for mail items
*/
void NmMessageListView::itemActivated(const QModelIndex &index)
{
    mActivatedIndex = index;
    QMetaObject::invokeMethod(this, "handleSelection", Qt::QueuedConnection);
}

/*!
    Item activation handling. Expand/collapse for title dividers
    opening for mail items
*/
void NmMessageListView::handleSelection()
{
    // Do expand/collapse for title divider items
    NmMessageListModelItem* modelItem = mMessageListModel.data(
            mActivatedIndex, Qt::DisplayRole).value<NmMessageListModelItem*>();
    if (modelItem && modelItem->itemType()==
        NmMessageListModelItem::NmMessageItemTitleDivider) {
        if (!mMessageListWidget->isExpanded(mActivatedIndex)) {
            mMessageListWidget->setExpanded(mActivatedIndex, true);
            modelItem->setExpanded(true);
        }
        else {
            mMessageListWidget->setExpanded(mActivatedIndex, false);
            modelItem->setExpanded(false);
        }
    } 
    if (modelItem && modelItem->itemType() == NmMessageListModelItem::NmMessageItemMessage)
    {
        NmFolderType folderType = mUiEngine.folderTypeById(mStartParam->mailboxId(),
                                  mStartParam->folderId());
        if (folderType==NmFolderDrafts){
            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                mStartParam->mailboxId(), mStartParam->folderId(),
                modelItem->envelope().messageId(),NmUiEditorFromDrafts);
            mApplication.enterNmUiView(startParam);            
        }
        else if (folderType!=NmFolderOutbox){
            NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageViewer,
                mStartParam->mailboxId(), mStartParam->folderId(),
                modelItem->envelope().messageId());
            mApplication.enterNmUiView(startParam);        
        }
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
            case NmActionResponseCommandSwitchFolder: {
                folderSelected(actionResponse.mailboxId(), actionResponse.folderId());
                break;
            }
            default: {
                break;
            }
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
        if (actionResponse.responseCommand() == NmActionResponseCommandSearch) {
            // Check that the given start response has mailbox and folder IDs.
            if (actionResponse.mailboxId() != 0) {
                if (mUiEngine.isSendingMessage()) {
                    // Sending is ongoing so just show a note.
                    QString noteText = hbTrId("txt_mail_dialog_still_sending");

                    // Get the message subject from the message being sent.
                    const NmMessage *message = mUiEngine.messageBeingSent();

                    if (message) {
                        noteText = noteText.arg(
                            NmUtilities::truncate(message->envelope().subject(), 20));
                    }

                    HbMessageBox::warning(noteText);
                }
                else {
                    NmUiStartParam *startParam =
                        new NmUiStartParam(NmUiViewMessageSearchList,
                                           actionResponse.mailboxId(),
                                           mStartParam->folderId());

                    // startParam ownership transfers.
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
    setFolderName. Function sets folder name to status bar
*/
void NmMessageListView::setFolderName()
{
    if (mStartParam && mFolderLabel) {
        switch (mUiEngine.folderTypeById(mStartParam->mailboxId(),
                mStartParam->folderId())) {
        case NmFolderOutbox:
            {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_outbox"));
            }
            break;
        case NmFolderDrafts:
            {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_drafts"));
            }
            break;
        case NmFolderSent:
            {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_sent"));
            }
            break;
        case NmFolderDeleted:
            {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_deleted"));
            }
            break;
        case NmFolderInbox:
        default:
            {
            mFolderLabel->setPlainText(hbTrId("txt_mail_subhead_inbox"));
            }
            break;
        }
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
                QModelIndex previous = mMessageListWidget->modelIterator()->previousIndex(index);
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
