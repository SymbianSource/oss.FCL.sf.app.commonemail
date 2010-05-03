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

// Layout file and view
static const char *NMUI_MAILBOX_LIST_VIEW_XML = ":/docml/nmmailboxlistview.docml";
static const char *NMUI_MAILBOX_LIST_VIEW = "NmMailboxListView";
static const char *NMUI_MAILBOX_LIST_WIDGET = "MailboxListWidget";

#include "nmuiheaders.h"

/*!
	\class NmMailboxListView
	\brief Mailbox list view
*/

/*!
    Constructor
*/
NmMailboxListView::NmMailboxListView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    NmMailboxListModel &mailboxListModel,
    HbDocumentLoader *documentLoader,
    QGraphicsItem *parent)
: NmBaseView(startParam,parent),
mApplication(application),
mMailboxListWidget(NULL),
mUiEngine(uiEngine),
mListModel(mailboxListModel),
mItemContextMenu(NULL),
mDocumentLoader(documentLoader)
{
    loadViewLayout();
    refreshList();
}

/*!
    Destructor
*/
NmMailboxListView::~NmMailboxListView()
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
void NmMailboxListView::loadViewLayout()
{
    // Use document loader to load the view
    bool ok = false;
    setObjectName(QString(NMUI_MAILBOX_LIST_VIEW));
    QObjectList objectList;
    objectList.append(this);
    // Pass the view to documentloader. Document loader uses this view
    // when docml is parsed, instead of creating new view.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MAILBOX_LIST_VIEW_XML, &ok);
    }

    if (ok == true && mWidgetList.count()) {
        // Create item context menu
        mItemContextMenu = new HbMenu();
        // Get mailbox widget pointer and set parameters
        mMailboxListWidget = qobject_cast<HbListView *>
            (mDocumentLoader->findWidget(NMUI_MAILBOX_LIST_WIDGET));
        if (mMailboxListWidget) {
            NMLOG("nmailui: mailboxlistview: List object loaded");
            // Set item prototype.
            mMailboxListWidget->setItemPrototype(new NmMailboxListViewItem(this));
            mMailboxListWidget->setItemRecycling(true);
            QObject::connect(mMailboxListWidget,
                    SIGNAL(activated(const QModelIndex &)),
                this, SLOT(openSelectedMailBox(const QModelIndex &)));
            QObject::connect(mMailboxListWidget,
                    SIGNAL(longPressed(HbAbstractViewItem*, const QPointF&)),
                this, SLOT(showItemContextMenu(HbAbstractViewItem*,const QPointF&)));
            mMailboxListWidget->setClampingStyle(HbScrollArea::BounceBackClamping);
            mMailboxListWidget->setFrictionEnabled(true);
        }
        else {
            NMLOG("nmailui: mailboxlistview: List object loading failed");
        }
    }
    else {
         NMLOG("nmailui: mailboxlistview: Reasource loading failed");
    }

	// set menubar title
	setTitle(hbTrId("txt_mail_title_mail"));
}

/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmMailboxListView::reloadViewContents(NmUiStartParam* startParam)
{
    // Check start parameter validity.
    if (startParam&&startParam->viewId()==NmUiViewMailboxList) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam=NULL;
        // Store new start parameter data
        mStartParam=startParam;
        // Refresh model
        refreshList();
    }
    else {
        NMLOG("nmailui: mailboxlistview: Invalid start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
        startParam = NULL;
    }
}

/*!
    View id
*/
NmUiViewId NmMailboxListView::nmailViewId() const
{
    return NmUiViewMailboxList;
}

/*!
    Refresh list
*/
void NmMailboxListView::refreshList()
{
	if (mMailboxListWidget) {
        mMailboxListWidget->setModel(&mListModel);
	}
}

/*!
    Open selected mailbox
*/
void NmMailboxListView::openSelectedMailBox(const QModelIndex &index)
{
    // Get mailbox meta data
    NmMailboxMetaData *mailbox =
        mListModel.data(index, Qt::DisplayRole).value<NmMailboxMetaData*>();
    if (mailbox) {
        // Get standard folder inbox id
        NmId inboxId = mUiEngine.standardFolderId(mailbox->id(), NmFolderInbox);
        // Create start params and launch message list view
        NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageList,mailbox->id(),inboxId);
        mApplication.enterNmUiView(startParam);
    }
}

/*!
    showItemContextMenu. Functions starts fetching item context menu objects
    from extension. Menu is displayed in view callback funtion.
*/
void NmMailboxListView::showItemContextMenu(HbAbstractViewItem *item, const QPointF &coords)
{
    if (mItemContextMenu&&item){
        // Clear previous items from context menu
        mItemContextMenu->clearActions();
        // Get mailbox meta data
        NmMailboxMetaData *mailbox =
            mListModel.data(item->modelIndex(), Qt::DisplayRole).value<NmMailboxMetaData*>();
        NmId mailboxId(0);
        if (mailbox) {
            mailboxId = mailbox->id();
        }
        // Fetch items from extension based on item
        NmActionRequest request(this, NmActionContextMenu, NmActionContextViewNone,
        		NmActionContextDataMailbox, mailboxId);
        NmUiExtensionManager &extMngr = mApplication.extManager();
        QList<NmAction*> list;
        extMngr.getActions(request, list);
        for (int i=0;i<list.count();i++) {
            mItemContextMenu->addAction(list[i]);
        }
        // Display menu
        if (mMailboxListWidget){
            mMailboxListWidget->setCurrentIndex(item->modelIndex());
            mItemContextMenu->setObjectName("MailboxItemContextMenu");
            mItemContextMenu->exec(coords);
        }
    }
}

/*!
    handleActionCommand. From NmMenuObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmMailboxListView::handleActionCommand(NmActionResponse &actionResponse)
{
    // Handle context menu commands here
    if (actionResponse.menuType()==NmActionContextMenu){
        switch (actionResponse.responseCommand()){
            case NmActionResponseCommandOpen:{
                // Check that given start response has mailbox and folder id's
                if (actionResponse.mailboxId()!=0){
                    // Use standard folder id inbox if folder has not been specified
                    NmId folderId = actionResponse.folderId();
                    if (folderId==0){
                        folderId=mUiEngine.standardFolderId(actionResponse.mailboxId(),
                                                            NmFolderInbox);
                    }
                    NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageList,
                                                                    actionResponse.mailboxId(),
                                                                    folderId);
                    mApplication.enterNmUiView(startParam);
                    }
                }
                break;
            default:
                break;
        }
    }
}


