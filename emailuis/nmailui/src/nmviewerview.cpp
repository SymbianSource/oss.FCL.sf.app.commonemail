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

#include "nmuiheaders.h"

static const char *NMUI_MESSAGE_VIEWER_XML = ":/docml/nmmailviewer.docml";
static const char *NMUI_MESSAGE_VIEWER_VIEW = "nmailViewerView";
static const char *NMUI_MESSAGE_VIEWER_CONTENT = "content";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_AREA = "viewerScrollArea";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_AREA_CONTENTS = "viewerScrollAreaContents";
static const char *NMUI_MESSAGE_VIEWER_HEADER = "viewerHeader";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_WEB_VIEW = "viewerWebView";

static const int nmViewLoadTimer=10;

/*!
	\class NmViewerView
	\brief Mail viewer class
*/

/*!
    Constructor
*/
NmViewerView::NmViewerView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    HbMainWindow *mainWindow,
    QGraphicsItem *parent)
:NmBaseView(startParam, parent),
mApplication(application),
mUiEngine(uiEngine),
mMainWindow(mainWindow),
mMessage(NULL),
mScrollArea(NULL),
mViewerContent(NULL),
mWebView(NULL),
mHeaderWidget(NULL),
mViewerContentLayout(NULL),
mMessageFetchingOperation(NULL),
mDisplayingPlainText(false),
mDocumentLoader(NULL),
mScrollAreaContents(NULL),
mViewerHeaderContainer(NULL),
mScreenSize(QSize(0,0)),
mWaitDialog(NULL),
loadingCompleted(false),
mLatestLoadingSize(QSize(0,0)) 
{
    // Create documentloader
    mDocumentLoader = new NmUiDocumentLoader(mMainWindow);
    // Get screensize
    mScreenSize = mApplication.screenSize();
    // Fetch message
    loadMessage();
    // Load view layout
    loadViewLayout();
    // Set message data
    QTimer::singleShot(nmViewLoadTimer, this, SLOT(fetchMessage()));
}

/*!
    Destructor
*/
NmViewerView::~NmViewerView()
{
    delete mWebView;
    mWebView=NULL;
    delete mMessage;
    mMessage = NULL;
    delete mDocumentLoader;
    mDocumentLoader = NULL;
    mWidgetList.clear();
    delete mWaitDialog;
    mWaitDialog = NULL;
}

/*!
    View layout loading from XML
*/
void NmViewerView::loadViewLayout()
{
    // Use document loader to load the view
    bool ok = false;
    setObjectName(QString(NMUI_MESSAGE_VIEWER_VIEW));
    QObjectList objectList;
    objectList.append(this);
    // Pass the view to documentloader. Document loader uses this view
    // when docml is parsed, instead of creating new view.
    if (mDocumentLoader) {
        mDocumentLoader->setObjectTree(objectList);
        mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_VIEWER_XML, &ok);
    }
    int widgetCount = mWidgetList.count();
    if (ok == true && widgetCount)
    {
        // Create content and content layout
        mViewerContent = reinterpret_cast<HbWidget *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_CONTENT));
        // Find scroll area
        mScrollArea = reinterpret_cast<NmBaseViewScrollArea *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA));
        if (mScrollArea){
            mScrollArea->setParentItem(this);
            mScrollArea->setScrollDirections(Qt::Vertical | Qt::Horizontal);
            connect(mScrollArea, SIGNAL(scrollPositionChanged(QPointF)),
                this, SLOT(contentScrollPositionChanged(QPointF)));
            connect(mScrollArea, SIGNAL(handleMousePressEvent(QGraphicsSceneMouseEvent*)),
                this, SLOT(handleMousePressEvent(QGraphicsSceneMouseEvent*)));
            connect(mScrollArea, SIGNAL(handleMouseReleaseEvent(QGraphicsSceneMouseEvent*)),
                this, SLOT(handleMouseReleaseEvent(QGraphicsSceneMouseEvent*)));

            // Get scroll area contents and set layout margins
            mScrollAreaContents = qobject_cast<HbWidget *>(
                    mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA_CONTENTS));
            if (mScrollAreaContents->layout()){
                mScrollAreaContents->layout()->setContentsMargins(0,0,0,0);
            }

            // Set white pixmap to backgrounditem
            QPixmap whitePixmap(10,10);
            whitePixmap.fill(Qt::white);
            QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(whitePixmap);
            mScrollAreaContents->setBackgroundItem(pixmapItem);

            // Load headerwidget
            mHeaderWidget = qobject_cast<NmViewerHeader *>(
                    mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_HEADER));
            if (mHeaderWidget) {
                mHeaderWidget->setView(this);
                mHeaderWidget->rescaleHeader(mScreenSize);
                mHeaderWidget->setMessage(mMessage);
                mHeaderStartScenePos = mHeaderWidget->scenePos();
            }

            // Load webview
            mWebView = reinterpret_cast<NmMailViewerWK *>(
                    mDocumentLoader->findObject(QString(NMUI_MESSAGE_VIEWER_SCROLL_WEB_VIEW)));
            if (mWebView){
                mWebView->setParentView(this);
                HbEditorInterface editorInterface(mWebView);
                editorInterface.setConstraints(HbEditorConstraintIgnoreFocus);
                mWebView->setAcceptedMouseButtons(Qt::NoButton);
                if (mWebView->page()){
                    mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, 
                                                                      Qt::ScrollBarAlwaysOff);
                    mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, 
                                                                      Qt::ScrollBarAlwaysOff);         
                    bool connectOk = connect(mWebView->page()->mainFrame(), SIGNAL(contentsSizeChanged(const QSize&)),
                        this, SLOT(scaleWebViewWhenLoading(const QSize&)));                                       
                }
             }
        } 
    }
    // call the createToolBar on load view layout
    createToolBar();
    // Set mailbox name to title
    setMailboxName();
}

/*!
    Function fecthes message data based on parameters
*/
void NmViewerView::loadMessage()
{
    if (mMessage) {
        delete mMessage;
        mMessage = NULL;
    }
    NmId mailboxId;
    NmId folderId;
    NmId msgId;
    // Read start params and message object
    if (mStartParam){
        mailboxId = mStartParam->mailboxId();
        folderId = mStartParam->folderId();
        msgId = mStartParam->messageId();
        mMessage = mUiEngine.message(mailboxId, folderId, msgId);
    }
}

/*!
    Function fecthes message data based on parameters. Returns false if message is available,
    true if message have to be fetched
*/
void NmViewerView::fetchMessage()
{
#ifdef Q_OS_SYMBIAN
    if (mMessage) {
        NmId mailboxId = mStartParam->mailboxId();
        NmId folderId = mStartParam->folderId();
        NmId msgId = mStartParam->messageId();
        NmMessagePart *body = mMessage->htmlBodyPart();
        if (!body) {
            // try plain to plain text part
            body = mMessage->plainTextBodyPart();
        }
        // try to fetch if body missing or fetched size is smaller than content size
        // if body missing it might mean that only header is fetched or message has no body
        if (!body || (body && (body->fetchedSize() < body->size()))) {
            // start fetching operation
            if (mMessageFetchingOperation) {
                mMessageFetchingOperation->cancelOperation();
                delete mMessageFetchingOperation;
                mMessageFetchingOperation = NULL;
            }
            mMessageFetchingOperation = mUiEngine.fetchMessage(mailboxId, folderId, msgId);

            if (mMessageFetchingOperation) {
                // QObject handles deletion
                mMessageFetchingOperation->setParent(this);
                connect(mMessageFetchingOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(messageFetched(int)));
                
                delete mWaitDialog;
                mWaitDialog = NULL;
                
                mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
                mWaitDialog->setModal(true);
                connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(waitNoteCancelled()));
                mWaitDialog->setText(hbTrId("txt_mail_dialog_loading_mail_content"));
                mWaitDialog->show();
            }
        }
        else {
            // message is fetched
            setMessageData();
        }

    }
#else
    setMessageData();
#endif
}

/*!
    This is signalled by mMessageFetchingOperation when the original message is fetched.
 */
void NmViewerView::messageFetched(int result)
{
    delete mWaitDialog;
    mWaitDialog = NULL;
    
    if (result == NmNoError && mMessageFetchingOperation) {
        if (mMessage) {
            delete mMessage;
            mMessage = NULL;
        }
        NmId mailboxId;
        NmId folderId;
        NmId msgId;
        // Read start params and message object
        if (mStartParam){
            mailboxId = mStartParam->mailboxId();
            folderId = mStartParam->folderId();
            msgId = mStartParam->messageId();
            mMessage = mUiEngine.message(mailboxId, folderId, msgId);
        }
        setMessageData();
        // Update header message data
		if (mHeaderWidget){
        	mHeaderWidget->updateMessageData(mMessage);		
		}
    }
}


/*!
    This is signalled by mWaitDialog when the note is cancelled
 */
void NmViewerView::waitNoteCancelled()
{
    if (mMessageFetchingOperation) {
        mMessageFetchingOperation->cancelOperation();
    }
    QTimer::singleShot(nmViewLoadTimer, &mApplication, SLOT(popView()));
}


/*!
    Function sets message data to web view and header
*/
void NmViewerView::setMessageData()
{
    // Connect to observe orientation change events
    connect(mApplication.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));

    // Set page parameters
    QWebPage *page(NULL);
    if (mWebView){
       page = mWebView->page();
    }
    if (page){
        // Set custom network access manager for embedded image handling
        NmViewerViewNetManager* netMngr = mApplication.networkAccessManager();
        if (netMngr){
            netMngr->setView(this);
            page->setNetworkAccessManager(netMngr);        
        }    
        connect(page, SIGNAL(loadFinished(bool)),
                    this, SLOT(webFrameLoaded(bool)));
        page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
        page->setContentEditable(false);
    }

  	// if everything is ok, set message to html viewer
    if (mMessage && mWebView && page) {
        // Set initial size of component and content before loading data
        mWebView->setMaximumWidth(mScreenSize.width());
        page->setPreferredContentsSize(mScreenSize);
        //Set message data to html viewer.
        mWebView->setHtml(formatMessage());
        // Connect to link clicked
        QObject::connect(page, SIGNAL(linkClicked(const QUrl&)),
                this, SLOT(linkClicked(const QUrl&)));
        changeMessageReadStatus(true);
    }
}

/*!
    Function formats message based on actual data 
*/
QString NmViewerView::formatMessage()
{
    QString msg = "";
    QString body = "";
    // null pointer check for mMessage is done before calling this function
    NmMessagePart *html = mMessage->htmlBodyPart();
    if (html) {
        QList<NmMessagePart*> parts;
        mMessage->attachmentList(parts);
        for (int i=0; i < parts.count(); i++) {
            NmMessagePart *child = parts[i];
            // Browse through embedded image parts and add those
            // the web view.
            if (child->contentType().startsWith("image", Qt::CaseInsensitive)) {
                QString contentId = child->contentId();
                int ret = mUiEngine.contentToMessagePart(
                        mMessage->mailboxId(),
                        mMessage->parentId(),
                        mMessage->envelope().id(),
                        *child);
                if (ret == NmNoError) {
                  mWebView->addContent(contentId, QVariant::fromValue(child->binaryContent()));
                }
            }
        }
        int ret = mUiEngine.contentToMessagePart(
                mMessage->mailboxId(),
                mMessage->parentId(),
                mMessage->envelope().id(),
                *html);
        if (ret == NmNoError) {
            body = html->textContent();
        }
        msg = body;
    }
    else {
        NmMessagePart *plain = mMessage->plainTextBodyPart();
        if (plain) {
            int ret = mUiEngine.contentToMessagePart(
                    mMessage->mailboxId(),
                    mMessage->parentId(),
                    mMessage->envelope().id(),
                    *plain);
            if (ret == NmNoError) {
                body += escapeSpecialCharacters(plain->textContent());
            }
            // Set plain text to viewer according to layout direction
            if (qApp->layoutDirection()==Qt::LeftToRight){
                // Define html start and end tags for plain text
                QString start = "<html><body text=\"black\"><P align=\"left\">";
                QString end = "</p></body></html>";
                msg = start + body + end;
            }
            else {
                // Define html start and end tags for plain text
                QString start = "<html><body text=\"black\"><P align=\"right\">";
                QString end = "</p></body></html>";
                msg = start + body + end;
            }
            mDisplayingPlainText=true;
        }
    }
    return msg;
}

/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmViewerView::reloadViewContents(NmUiStartParam* startParam)
{
    // Check start parameter validity, message view cannot
    // be updated if given parameter is zero.
    if (startParam && startParam->viewId() == NmUiViewMessageViewer &&
        startParam->messageId()!= 0) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam = NULL;
        // Store new start parameter data
        mStartParam = startParam;
        // Reload viewer with new message information
        setMessageData();
    }
    else {
        NMLOG("nmailui: Invalid viewer start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
    }
}

/*!
    nmailViewId
*/
NmUiViewId NmViewerView::nmailViewId() const
{
    return NmUiViewMessageViewer;
}

/*!
    Scale web view width
*/
void NmViewerView::webFrameLoaded(bool loaded)
{
    if (loaded){
        loadingCompleted = true;
        // Scale web view after loading the
        // complete contents, including images
        QTimer::singleShot(nmViewLoadTimer, this, SLOT(scaleWebViewWhenLoaded()));
    }
} 

/*!
    Scale web view width when loading is ongoing
*/
void NmViewerView::scaleWebViewWhenLoading(const QSize &size)
{
    // Try to scale web view while mainframe is being loaded. 
    // So that screen is scrollable even before images are fully loaded
    // First check that new size is different than previous, no need to react if
    // same size value is received more than once.
    if (size!=mLatestLoadingSize){
        if (!loadingCompleted&&mWebView&&mWebView->page()&&
            (size.width()>mScreenSize.width()||size.height()>mScreenSize.height())) {
            int width = (int)size.width();
            int height = (int)size.height();
            // Set content (webview) width
            if (mDisplayingPlainText){
                mWebView->setMaximumWidth(mScreenSize.width());
                mWebView->setMinimumWidth(mScreenSize.width());
                mWebView->setPreferredWidth(mScreenSize.width()); 
            }
            else {
                 mWebView->setMaximumWidth(width);
                 mWebView->setMinimumWidth(width);
                 mWebView->setPreferredWidth(width);                
            }
            mWebView->setMinimumHeight(height);
            mWebView->setPreferredHeight(height);
        }    
    }
    mLatestLoadingSize=size;
}

/*!
    Scale web view width when loading is completed
*/
void NmViewerView::scaleWebViewWhenLoaded()
{
    if (mWebView&&mWebView->page()) {
        QSizeF contentSize = mWebView->page()->mainFrame()->contentsSize(); 
        int width = (int)contentSize.width();
        int height = (int)contentSize.height();
        // Set content (webview) width
        if (mDisplayingPlainText){
            mWebView->page()->setPreferredContentsSize(mScreenSize);
            mWebView->setMinimumWidth(mScreenSize.width());
            mWebView->setMaximumWidth(mScreenSize.width());
            mWebView->setPreferredWidth(mScreenSize.width());
        }
        else {
            mWebView->setMinimumWidth(width);
            mWebView->setMaximumWidth(width);
            mWebView->setPreferredWidth(width);
        }
        // Set content (webview) height
        if (mScrollAreaContents){
            QRectF contentRect = mScrollAreaContents->geometry();
            if (contentRect.height()<geometry().height()){
                contentRect.setHeight(geometry().height());
                mViewerContent->setPreferredHeight(contentRect.height());
                qreal webViewHeight = geometry().height()-mHeaderWidget->geometry().height();
                mWebView->setMinimumHeight(webViewHeight);
                mWebView->setMaximumHeight(webViewHeight);
                mWebView->setPreferredHeight(webViewHeight);
            }
            else{
                mWebView->setMinimumHeight(height);
                mWebView->setMaximumHeight(height);
                mWebView->setPreferredHeight(height);
            }                
        }
    }
}


/*!
   Screen orientation changed. Web view needs to be scaled when
   landscape <-> portrait switch occurs because text needs to
   be wrapped again.
*/
void NmViewerView::orientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    // Update current screensize
    mScreenSize = mApplication.screenSize();
    // Scale header to screen width
    if (mHeaderWidget){
        mHeaderWidget->rescaleHeader(mScreenSize);
    }

    // Scale web view and its contens
    if (mWebView){
        if (mDisplayingPlainText){
            mWebView->setMaximumWidth((int)mScreenSize.width());
            mWebView->page()->setPreferredContentsSize(QSize((int)mScreenSize.width(),
                                           (int)mScreenSize.height()));  
        }
        else{
            // Check whether contentsize fits to screen
            // and if not, set preferred size again to allow panning
            QSizeF contentSize = mWebView->page()->mainFrame()->contentsSize();
            if (contentSize.width()>mScreenSize.width()){
                mWebView->setMaximumWidth((int)contentSize.width());
                mWebView->page()->setPreferredContentsSize(QSize((int)contentSize.width(),
                                                           (int)contentSize.height()));
            }
            else{
                mWebView->setMaximumWidth((int)mScreenSize.width());
                mWebView->page()->setPreferredContentsSize(QSize((int)mScreenSize.width(),
                                               (int)mScreenSize.height()));
            }        
        }
    }

    // Re-create toolbar in orientation switch
    createToolBar();
}

/*!
   Link clicked callback
*/
void NmViewerView::linkClicked(const QUrl& link)
{
    NMLOG("link clicked");
      if (link.scheme() == "http" ||
          link.scheme() == "https" ) {
          QDesktopServices::openUrl(link);
      }
      else if (link.scheme() == "mailto"){
          QList<NmAddress*> *addrList = new QList<NmAddress*>();
          NmAddress *mailtoAddr = new NmAddress();
          QString address = link.toString(QUrl::RemoveScheme);
          mailtoAddr->setAddress(address);
          mailtoAddr->setDisplayName(address);
          addrList->append(mailtoAddr);
          // Create start parameters. Address list ownership
          // is transferred to startparam object
          NmUiStartParam* param = new NmUiStartParam(NmUiViewMessageEditor,
                                                     mStartParam->mailboxId(),
                                                      mStartParam->folderId(),
                                                     0, NmUiEditorMailto, addrList );
          mApplication.enterNmUiView(param);
      }
}

/*!
   Send mouse release event to web view
*/
void NmViewerView::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NmMailViewerWK* view = webView();
    if (event&& view && mHeaderWidget && mScrollAreaContents) {
        QPointF lastReleasePoint = event->pos();
        QPointF contentWidgetPos = mScrollAreaContents->pos();
        qreal headerHeight = mHeaderWidget->geometry().height();
        qreal y = lastReleasePoint.y()-headerHeight;
        y -= contentWidgetPos.y();
        qreal x = lastReleasePoint.x()-contentWidgetPos.x();
        const QPointF pointToWebView(x, y);
        event->setPos(pointToWebView);
        event->setAccepted(true);
        view->sendMouseReleaseEvent(event);
    }
}

/*!
   Send mouse press event
*/
void NmViewerView::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    NmMailViewerWK* view = webView();
    if (event&& view && mHeaderWidget && mScrollAreaContents) {
        QPointF lastPressPoint = event->pos();
        QPointF contentWidgetPos = mScrollAreaContents->pos();
        qreal headerHeight = mHeaderWidget->geometry().height();
        qreal y = lastPressPoint.y()-headerHeight;
        y -= contentWidgetPos.y();
        qreal x = lastPressPoint.x()-contentWidgetPos.x();
        const QPointF pointToWebView(x, y);
        event->setPos(pointToWebView);
        event->setAccepted(true);
        view->sendMousePressEvent(event);
    }
}

/*!
   Function can be used to check whether mouse event has
   occured on top of header area.
*/
bool NmViewerView::eventOnTopOfHeaderArea(QGraphicsSceneMouseEvent *event)
{
    bool ret(false);
    if (event && mHeaderWidget){
        QPointF lastReleasePoint = event->lastPos();
        QPointF contentWidgetPos = mScrollAreaContents->pos();
        int headerHeight = (int)mHeaderWidget->geometry().height();
        if (lastReleasePoint.y()<headerHeight+contentWidgetPos.y()){
            ret=true;
        }
    }
    return ret;
}

/*!
   get function for content widget web view.
*/
NmMailViewerWK* NmViewerView::webView()
{
    return mWebView;
}

/*!
   Replace html special characters from plain text content.
*/
QString NmViewerView::escapeSpecialCharacters(const QString text)
{
    const QString amp("&");
    const QString ampReplace("&amp;");
    const QString lt("<");
    const QString ltReplace("&lt;");
    const QString gt(">");
    const QString gtReplace("&gt;");
    const QString quot("\"");
    const QString quotReplace("&quot;");
    const QString apos("'");
    const QString aposReplace("&apos;");
    QString ret = text;
    ret.replace(amp, ampReplace);
    ret.replace(lt, ltReplace);
    ret.replace(gt, gtReplace);
    ret.replace(quot, quotReplace);
    ret.replace(apos, aposReplace);
    return ret;
}

/*!
   Function to set message read status
*/
void NmViewerView::changeMessageReadStatus(bool read)
{
    QList<const NmMessageEnvelope*> envelopeList;
    NmMessageEnvelope *envelope = &mMessage->envelope();
    NmStoreEnvelopesOperation* op = NULL;
    if (envelope){
        if ( read != envelope->isRead() ){
            if (read){
                envelope->setRead(true);
                envelopeList.append(envelope);
                op = mUiEngine.setEnvelopes(
                    mStartParam->mailboxId(),
                    mStartParam->folderId(),
                    MarkAsRead,
                    envelopeList);
            }
            else {
                envelope->setRead(false);
                envelopeList.append(envelope);
                op = mUiEngine.setEnvelopes(
                    mStartParam->mailboxId(),
                    mStartParam->folderId(),
                    MarkAsUnread,
                    envelopeList);
            }
        }
        if(op){
            mUiEngine.storeOperation(op);
            }
    }
}

/*!
    Set mailbox name to title
*/
void NmViewerView::setMailboxName()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta){
            setTitle(meta->name());
        }
    }
}

/*!
    contentScrollPositionChanged.
    Function reacts to scroll position change events and sets
    header to correct position
*/
void NmViewerView::contentScrollPositionChanged(const QPointF &newPosition)
{
    if (mWebView&&mHeaderWidget){
        QRectF webViewRect = mWebView->geometry();
        QTransform tr;
        qreal leftMovementThreshold(webViewRect.width()-mHeaderWidget->geometry().width());
        if (newPosition.x()<0){
            tr.translate(webViewRect.topLeft().x() ,0);
        }
        else if (newPosition.x()>=0 && newPosition.x()<leftMovementThreshold){
            tr.translate(mHeaderStartScenePos.x()+newPosition.x() ,0);
        }
        else {
            tr.translate(webViewRect.topLeft().x()+leftMovementThreshold ,0);
        }
        mHeaderWidget->setTransform(tr);
    }
    mLatestScrollPos = newPosition;
}

/*!
    createToolBar. Function asks menu commands from extension
    to be added to toolbar owned by the HbView.
*/
void NmViewerView::createToolBar()
{
    HbToolBar *tb = toolBar();
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (tb && &extMngr && mStartParam) {
        tb->clearActions();
        NmActionRequest request(this, NmActionToolbar, NmActionContextViewViewer,
                NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
        QList<NmAction *> list;
        extMngr.getActions(request, list);
        for (int i = 0; i < list.count(); i++) {
            tb->addAction(list[i]);
        }
    }
}

/*!
    handleActionCommand. From NmActionObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmViewerView::handleActionCommand(NmActionResponse &actionResponse)
{
    bool showSendInProgressNote = false;
    
    // Handle options menu
    if (actionResponse.menuType() == NmActionOptionsMenu) {
    }
    else if (actionResponse.menuType() == NmActionToolbar) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandReply: {
                if (mUiEngine.isSendingMessage()) {
                    showSendInProgressNote = true;
                    break;
                }
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReply);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandReplyAll: {
                if (mUiEngine.isSendingMessage()) {
                    showSendInProgressNote = true;
                    break;
                }
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReplyAll);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandForward: {
                if (mUiEngine.isSendingMessage()) {
                    showSendInProgressNote = true;
                    break;
                }
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorForward);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandDeleteMail: {
            }
            break;
            default:
                break;
        }
    }
    
    if (showSendInProgressNote) {
        QString noteText = hbTrId("txt_mail_dialog_still_sending");
        
        // get message subject from the message being sent
        const NmMessage *message = mUiEngine.messageBeingSent();
        if (message) {
            noteText = noteText.arg(NmUtilities::truncate(message->envelope().subject(), 20));
        }
        HbMessageBox::warning(noteText);
    }
}
