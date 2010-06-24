/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
static const char *NMUI_MESSAGE_VIEWER_ATTALIST = "viewerAttaList";
static const char *NMUI_MESSAGE_VIEWER_SCROLL_WEB_VIEW = "viewerWebView";

static const int NmOrientationTimer = 100;
static const int NmHeaderMargin = 3;

static const QString NmParamTextHeightSecondary = "hb-param-text-height-secondary";

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
    NmAttachmentManager &attaManager,
    bool toolbarEnabled,
    QGraphicsItem *parent)
:NmBaseView(startParam, application, parent),
mApplication(application),
mUiEngine(uiEngine),
mMainWindow(mainWindow),
mAttaManager(attaManager),
mToolbarEnabled(toolbarEnabled),
mMessage(NULL),
mScrollArea(NULL),
mViewerContent(NULL),
mWebView(NULL),
mHeaderWidget(NULL),
mMessageFetchingOperation(NULL),
mDisplayingPlainText(false),
mDocumentLoader(NULL),
mScrollAreaContents(NULL),
mViewerHeaderContainer(NULL),
mScreenSize(QSize(0,0)),
mWaitDialog(NULL),
webFrameloadingCompleted(false),
mLatestLoadingSize(QSize(0,0)),
mAttaIndexUnderFetch(NmNotFoundError),
mAttaWidget(NULL),
mViewReady(false),
mWaitNoteCancelled(false),
mErrorNote(NULL)
{
    // Create documentloader
    mDocumentLoader = new NmUiDocumentLoader(mMainWindow);
    // Get screensize
    mScreenSize = mApplication.screenSize();
    // Connect external delete handling to uiengine signal
    connect(&mUiEngine,
            SIGNAL(messageDeleted(const NmId &, const NmId &, const NmId &)),
            this, SLOT(messageDeleted(const NmId &, const NmId &, const NmId &)),
            Qt::UniqueConnection);
    // Fetch message
    loadMessage();
    // Load view layout
    loadViewLayout();
}

/*!
    Destructor
*/
NmViewerView::~NmViewerView()
{
    delete mErrorNote;
    mErrorNote=NULL;
    delete mWebView;
    mWebView = NULL;
    delete mMessage;
    mMessage = NULL;
    delete mDocumentLoader;
    mDocumentLoader = NULL;
    mWidgetList.clear();
    delete mWaitDialog;
    mWaitDialog = NULL;
    // remove view from osbserving atta manager events
    mAttaManager.clearObserver();
    mAttaManager.cancelFetch();
}

/*!
    View is about to exit
*/
void NmViewerView::aboutToExitView()
{
    // View is about to exit, for safety, stop 
    // loading of content before closing the view
    if (mWebView){
        mAttaManager.cancelFetch();
        mWebView->stop();
        if (mWebView->page()){
            mWebView->page()->deleteLater();
        }
    }
}

/*!
    View layout loading from XML
*/
void NmViewerView::loadViewLayout()
{
    NM_FUNCTION;
    
    // Use document loader to load the view
    bool ok(false);
    setObjectName(QString(NMUI_MESSAGE_VIEWER_VIEW));
    // Pass the view to documentloader. Document loader uses this view
    // when docml is parsed, instead of creating new view.
    // documentloader is created in constructor
    mWidgetList = mDocumentLoader->load(NMUI_MESSAGE_VIEWER_XML, &ok);
    int widgetCount = mWidgetList.count();
    if (ok == true && widgetCount)
    {
        // Create content and content layout
        // qobject_cast not work in this case, using reinterpret_cast
        mViewerContent = reinterpret_cast<HbWidget *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_CONTENT));
        // Find scroll area
        mScrollArea = reinterpret_cast<HbScrollArea *>(
                mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA));
        if (mScrollArea) {
            mScrollArea->setParentItem(this);
            mScrollArea->setScrollDirections(Qt::Vertical | Qt::Horizontal);
            connect(mScrollArea, SIGNAL(scrollPositionChanged(QPointF)),
                this, SLOT(contentScrollPositionChanged(QPointF)));

            // Get scroll area contents and set layout margins
            mScrollAreaContents = qobject_cast<HbWidget *>(
                    mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_SCROLL_AREA_CONTENTS));
            if (mScrollAreaContents) {
                QGraphicsLayout *layout = mScrollAreaContents->layout();
                if (layout){
                    layout->setContentsMargins(0,0,0,0);
                }
                // Set white pixmap to backgrounditem 
                QPixmap whitePixmap(10,10);
                whitePixmap.fill(Qt::white);
                QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem(whitePixmap);
                mScrollAreaContents->setBackgroundItem(pixmapItem);
            }

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
            if (mWebView) {
                mWebView->setParentView(this);
                // Set auto load images and private browsing(no history) attributes
                QWebSettings *settings = mWebView->settings();
                if (settings) {
                    settings->setAttribute(QWebSettings::AutoLoadImages, true);
                    settings->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);   
                }
                QWebPage *page = mWebView->page();
                if (page) {
                    QWebFrame *frame = page->mainFrame();
                    if (frame) {
                        frame->setScrollBarPolicy(Qt::Vertical,Qt::ScrollBarAlwaysOff);
                        frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
                        connect(mWebView->page()->mainFrame(),
                                SIGNAL(contentsSizeChanged(const QSize&)),
                            this, SLOT(scaleWebViewWhenLoading(const QSize&)));  
                    }
                }
            }
        }
    }
}
 
/*!
    Lazy loading when view layout has been loaded
*/
void NmViewerView::viewReady()
{
    if (!mViewReady){
        // Set mailbox name to title
        setMailboxName();
        // Create toolbar if needed
        if (mToolbarEnabled) {
            createToolBar();
        } else {
            // Connect options menu about to show to create options menu function
            QObject::connect(menu(), SIGNAL(aboutToShow()),
                    this, SLOT(createOptionsMenu())); 
            // Menu needs one dummy item so that aboutToShow signal is emitted.
            NmAction *dummy = new NmAction(0);
            menu()->addAction(dummy);
        }
                
        if (mHeaderWidget) {
            QPointF contentWidgetPos = mScrollArea->pos();
            qreal headerHeight = mHeaderWidget->geometry().height();
            if (mMainWindow->orientation() == Qt::Horizontal) {
                const QPointF pointToWebView(contentWidgetPos.x(), headerHeight+NmHeaderMargin);
                mScrollArea->scrollContentsTo(pointToWebView,0);
            }
        }
        
        // Run fetchmessage in queue
        QMetaObject::invokeMethod(this, "fetchMessage", Qt::QueuedConnection);
        // Set view ready
        mViewReady = true;
    }
}

/*!
    Function fecthes message data based on parameters
*/
void NmViewerView::loadMessage()
{
    NM_FUNCTION;
    
    if (mMessage) {
        delete mMessage;
        mMessage = NULL;
    }
    // Read start params and message object
    if (mStartParam){
        NmId mailboxId = mStartParam->mailboxId();
        NmId folderId = mStartParam->folderId();
        NmId msgId = mStartParam->messageId();
        mMessage = mUiEngine.message(mailboxId, folderId, msgId);
    }
}

/*!
    Function fecthes message data based on parameters. Returns false if message is available,
    true if message have to be fetched
*/
void NmViewerView::fetchMessage()
{
    NM_FUNCTION;
    
    if (mMessage) {
        NmId mailboxId = mStartParam->mailboxId();
        NmId folderId = mStartParam->folderId();
        NmId msgId = mStartParam->messageId();
        const NmMessagePart *body = mMessage->htmlBodyPart();
        if (!body) {
            // try plain to plain text part
            body = mMessage->plainTextBodyPart();
        }
        // try to fetch if body missing or fetched size is smaller than content size
        // if body missing it might mean that only header is fetched or message has no body
        if (!body || (body && (body->fetchedSize() < body->size()))) {
            // start fetching operation
            if (mMessageFetchingOperation && mMessageFetchingOperation->isRunning()) { 
                mMessageFetchingOperation->cancelOperation();
            }
            mMessageFetchingOperation = mUiEngine.fetchMessage(mailboxId, folderId, msgId);

            if (mMessageFetchingOperation) {
                connect(mMessageFetchingOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(messageFetched(int)));
                createAndShowWaitDialog();
            }
        } else {
            // message is fetched
            setMessageData();
        }
    }
}

/*!
    This is signalled by mMessageFetchingOperation when the original message is fetched.
 */
void NmViewerView::messageFetched(int result)
{
    mWaitDialog->close();
    disconnect(mWaitDialog->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));
    
    if (result == NmNoError && mMessageFetchingOperation) {
        if (mMessage) {
            delete mMessage;
            mMessage = NULL;
        }
        // Read start params and message object
        if (mStartParam) {
            NmId mailboxId = mStartParam->mailboxId();
            NmId folderId = mStartParam->folderId();
            NmId msgId = mStartParam->messageId();
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
    if (!mWaitNoteCancelled) {
        if (mMessageFetchingOperation && mMessageFetchingOperation->isRunning()) { 
	        mMessageFetchingOperation->cancelOperation();
        }
        mWaitNoteCancelled = true;
        QMetaObject::invokeMethod(&mApplication, "prepareForPopView", Qt::QueuedConnection);
    }
}


/*!
    Function sets message data to web view and header
*/
void NmViewerView::setMessageData()
{
    NM_FUNCTION;
    
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
        NmViewerViewNetManager &netMngr = mApplication.networkAccessManager();
        netMngr.setView(this);
        page->setNetworkAccessManager(&netMngr);
        QWebSettings *webSettings = page->settings();
        if (webSettings) {
            webSettings->setObjectCacheCapacities(0,0,0);
        }

        connect(page, SIGNAL(loadFinished(bool)),
                    this, SLOT(webFrameLoaded(bool)));
        page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
        page->setContentEditable(false);
    }

  	// if everything is ok, set message to html viewer
    if (mMessage && mWebView && page) {
        // Set initial size of component and content before loading data
        mWebView->setPreferredWidth(mScreenSize.width());
        QRectF myGeometry = geometry();
        page->setViewportSize(myGeometry.size().toSize());
        //Set message data to html viewer.
        mWebView->setHtml(formatMessage());
        // Connect to link clicked
        QObject::connect(page, SIGNAL(linkClicked(const QUrl&)),
                this, SLOT(linkClicked(const QUrl&)));
        changeMessageReadStatus(true);
        setAttachmentList();
    }
}

/*!

*/
void NmViewerView::setAttachmentList()
{
    NM_FUNCTION;
    
    // Load headerwidget
    mAttaWidget = qobject_cast<NmAttachmentListWidget *>(
            mDocumentLoader->findObject(NMUI_MESSAGE_VIEWER_ATTALIST));
    if (mMessage && mAttaWidget) {
        // connect atta widget to listen progress value events
        QObject::connect(this, SIGNAL(progressValueChanged(int, int)),
                mAttaWidget, SLOT(setProgressBarValue(int, int)));
        // Set atta widget text color black since header has static white bg.
        mAttaWidget->setTextColor(Qt::black);
        // Set attawidget minimum & maximum size
        mAttaWidget->setMinimumWidth(mScreenSize.width());
        mAttaWidget->setMaximumWidth(mScreenSize.width());
        bool inserted(false);
        QList<NmMessagePart*> messageParts;
        mMessage->attachmentList(messageParts);
        for (int i = 0; i < messageParts.count();i++) {
            NmMessagePart *part = messageParts[i];
            if (part &&
                ((part->contentDisposition().trimmed().startsWith("inline", Qt::CaseInsensitive)==false) ||
                 (part->contentType().trimmed().startsWith("image", Qt::CaseInsensitive)==false))) {
                QString fileName = part->attachmentName();
                // index starts from zero, next index is same as count
                int attaIndex = mAttaWidget->count();
                mAttaWidget->insertAttachment(attaIndex, fileName,
                                              NmUtilities::attachmentSizeString(part->size()));
                mAttaIdList.insert(attaIndex, part->partId());
                // check is part under fetch, add progress value to attachment widget
                if (mAttaManager.isFetching() && 
                        mAttaManager.partIdUnderFetch() == part->partId()) {
                    mAttaIndexUnderFetch = attaIndex;
                }
                inserted = true;
            }
        }
        if (inserted) {
            QObject::connect(mAttaWidget, SIGNAL(itemActivated(int)),
                    this, SLOT(openAttachment(int)));
        }
        else {
            // No attachments, set height to 0
            mAttaWidget->setMinimumHeight(0);
            mAttaWidget->setMaximumHeight(0);
        }
        // set this as a observer for attachment manager
        mAttaManager.setObserver(this);
    }
}

/*!

*/
void NmViewerView::openAttachment(int index)
{
    NM_FUNCTION;
    
    NmId attaId = mAttaIdList.at(index);
    // reload message to get updates part sizes
    loadMessage();
    QList<NmMessagePart*> messageParts;
    mMessage->attachmentList(messageParts);
    NmId mailboxId = mMessage->envelope().mailboxId();
    NmId folderId = mMessage->envelope().folderId();
    NmId messageId = mMessage->envelope().messageId();
    for (int i = 0; i < messageParts.count(); i++) {
        // message part found have to found
        // and its fetched size is smaller than size, then start part fetch
        if (messageParts[i]->partId() == attaId &&
            messageParts[i]->size() > messageParts[i]->fetchedSize()) {
            // do not start if there's already ongoing fetch
            if (mAttaIndexUnderFetch == NmNotFoundError) {
                mAttaIndexUnderFetch = index;
                mAttaManager.fetchAttachment(mailboxId, folderId,
                                             messageId, attaId);
            }
        }
        // attachment is fetched, open file
        else if (messageParts[i]->partId() == attaId) {
            mAttaManager.cancelFetch();
            XQSharableFile file = mUiEngine.messagePartFile(mailboxId, folderId,
                                                            messageId, attaId);
            int error = NmUtilities::openFile(file);
            file.close();
            if (error==NmNotFoundError){
                delete mErrorNote;
                mErrorNote=NULL;
                mErrorNote = NmUtilities::displayWarningNote(hbTrId("txt_mail_dialog_unable_to_open_attachment_file_ty"));
            }
        }
    }
}

/*!
    Function formats message based on actual data
*/
QString NmViewerView::formatMessage()
{
    NM_FUNCTION;
    
    QString msg = "";
    // null pointer check for mMessage is done before calling this function
    NmId mailboxId = mMessage->envelope().mailboxId();
    NmId folderId = mMessage->envelope().folderId();
    NmId messageId = mMessage->envelope().messageId();
    NmMessagePart *html = mMessage->htmlBodyPart();
    if (html) {
        QList<NmMessagePart*> parts;
        mMessage->attachmentList(parts);
        for (int i=0; i < parts.count(); i++) {
            NmMessagePart *child = parts[i];
            // Browse through embedded image parts and add those
            // the web view.
            bool isFetched = child->fetchedSize() >= child->size();
            if (child->contentType().startsWith("image", Qt::CaseInsensitive)) {
                QString contentId = child->contentId();
                if (isFetched) {
                    int ret = mUiEngine.contentToMessagePart(
                            mailboxId, folderId, messageId, *child);
                    if (ret == NmNoError) {
                      mWebView->addContent(contentId, QVariant::fromValue(child->binaryContent()), 
                              child->partId(), isFetched);
                    }
                }
                else {
                    mWebView->addContent(contentId, QVariant::fromValue(QByteArray()), 
                            child->partId(), isFetched);
                }
            }
        }
        int ret = mUiEngine.contentToMessagePart(mailboxId, folderId, messageId, *html);
        if (ret == NmNoError) {
            msg = html->textContent();
        }
    }
    else {
        NmMessagePart *plain = mMessage->plainTextBodyPart();
        if (plain) {
            int ret = mUiEngine.contentToMessagePart(mailboxId, folderId,
                                                     messageId, *plain);
            if (ret == NmNoError) {
                QTextDocument document;
                // set font
                QFont currentFont = document.defaultFont();
                currentFont.setWeight(QFont::Normal);
                qreal secondarySize;
                HbStyle myStyle;
                bool found = myStyle.parameter(NmParamTextHeightSecondary, secondarySize);
                if (found) {
                    HbFontSpec fontSpec(HbFontSpec::Secondary);
                    fontSpec.setTextHeight(secondarySize);
                    currentFont.setPixelSize(fontSpec.font().pixelSize());
                }
                document.setDefaultFont(currentFont);
                // convert to html
                document.setPlainText(plain->textContent());
                msg = document.toHtml();

                if (qApp->layoutDirection()==Qt::RightToLeft){
                    // add right alignment to document css section
                    QRegExp rx("(<style type=\"text/css\">)(.+)(</style>)", Qt::CaseInsensitive);
                    rx.setMinimal(true);
                    int pos = rx.indexIn(msg);
                    if (pos > -1) {
                        QString newStr = rx.cap(1);
                        newStr.append(rx.cap(2));
                        newStr.append("p { text-align: right } ");
                        newStr.append(rx.cap(3));
                        msg.replace(rx, newStr);
                    }
                }
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
        webFrameloadingCompleted = true;
        // Scale web view after loading the
        // complete contents, including images
        QMetaObject::invokeMethod(this, "scaleWebViewWhenLoaded", Qt::QueuedConnection);       
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
    if (size != mLatestLoadingSize) {
        if (!webFrameloadingCompleted && mWebView && mWebView->page() &&
            (size.width() > mScreenSize.width() || size.height() > geometry().height())) {
            int width = (int)size.width();
            int height = (int)size.height();
            // Set content (webview) width
            if (mDisplayingPlainText){
                mWebView->setPreferredWidth(geometry().width());           
            }
            else {
                mWebView->setPreferredWidth(width);
            }
            mWebView->setPreferredHeight(height);        
        }
    }
    mLatestLoadingSize = size;
}

/*!
    Scale web view width when loading is completed
*/
void NmViewerView::scaleWebViewWhenLoaded()
{
    QRectF myGeometry = geometry();
    if (mWebView && mWebView->page()) {
        mWebView->page()->setViewportSize(myGeometry.size().toSize());
        QSizeF contentSize = mWebView->page()->mainFrame()->contentsSize();
        int width = (int)contentSize.width();
        int height = (int)contentSize.height();  
        mWebView->setPreferredWidth(width);
        mWebView->setPreferredHeight(height);
    }
}

/*!
    Set new dimensions after orientation change.
*/
void NmViewerView::adjustViewDimensions()
{
    // Update current screensize
    mScreenSize = mApplication.screenSize();
    // Scale header to screen width
    if (mHeaderWidget){
        mHeaderWidget->rescaleHeader(mScreenSize);
    }
    if (mAttaWidget){
        // Set attawidget minimum & maximum size
        mAttaWidget->setMinimumWidth(mScreenSize.width());
        mAttaWidget->setMaximumWidth(mScreenSize.width());
    }    
    scaleWebViewWhenLoaded();

    if (mToolbarEnabled) {
		// Re-create toolbar in orientation switch
		createToolBar();
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
    QTimer::singleShot(NmOrientationTimer, this, SLOT(adjustViewDimensions()));
}

/*!
   Link clicked callback
*/
void NmViewerView::linkClicked(const QUrl& link)
{
    NM_FUNCTION;
    
    if (link.scheme() == "http" ||
        link.scheme() == "https" ) {
        mAttaManager.cancelFetch();
        QDesktopServices::openUrl(link);
    } else if (link.scheme() == "mailto"){
        mAttaManager.cancelFetch();
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
                                                   0,
                                                   NmUiEditorMailto,
                                                   addrList);
        mApplication.enterNmUiView(param);
    }    
}

/*!
   Function can be used to check whether mouse event has
   occured on top of header area.
*/
bool NmViewerView::eventOnTopOfHeaderArea(QGraphicsSceneMouseEvent *event)
{
    bool ret(false);
    if (event && mHeaderWidget) {
        QPointF lastReleasePoint = event->lastPos();
        QPointF contentWidgetPos = mScrollAreaContents->pos();
        int headerHeight = (int)mHeaderWidget->geometry().height();
        if (lastReleasePoint.y()<headerHeight+contentWidgetPos.y()) {
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

*/
NmMessage* NmViewerView::message()
{
    return mMessage;
}

/*!
   Function to set message read status
*/
void NmViewerView::changeMessageReadStatus(bool read)
{
    NM_FUNCTION;
    
    QList<const NmMessageEnvelope*> envelopeList;
    NmMessageEnvelope *envelope = &mMessage->envelope();
    QPointer<NmStoreEnvelopesOperation> op(NULL);
    if (envelope) {
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
    }
}

/*!
    Set mailbox name to title
*/
void NmViewerView::setMailboxName()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta) {
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
        if (newPosition.x()<0) {
            tr.translate(webViewRect.topLeft().x() ,0);
        }
        else if (newPosition.x()>=0 && newPosition.x()<leftMovementThreshold) {
            tr.translate(mHeaderStartScenePos.x()+newPosition.x() ,0);
        }
        else {
            tr.translate(webViewRect.topLeft().x()+leftMovementThreshold ,0);
        }
        mHeaderWidget->setTransform(tr);
        if (mAttaWidget) {
            mAttaWidget->setTransform(tr);
        }
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
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmViewerView::createOptionsMenu()
{
	HbMenu *optionsMenu = menu();
	NmUiExtensionManager &extMngr = mApplication.extManager();
	if (optionsMenu && &extMngr && mStartParam) {
		optionsMenu->clearActions();
		NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewViewer,
				NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );

		QList<NmAction*> list;
		extMngr.getActions(request, list);
		for (int i=0;i<list.count();i++) {
			optionsMenu->addAction(list[i]);
		}
	}
}

/*!
    handleActionCommand. From NmActionObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmViewerView::handleActionCommand(NmActionResponse &actionResponse)
{
    // Handle options menu or toolbar
    if (actionResponse.menuType() == NmActionOptionsMenu ||
    	actionResponse.menuType() == NmActionToolbar) {
        switch (actionResponse.responseCommand()) {
            case NmActionResponseCommandReply: {
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReply);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandReplyAll: {
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorReplyAll);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandForward: {
                mAttaManager.cancelFetch();
                NmUiStartParam *startParam = new NmUiStartParam(NmUiViewMessageEditor,
                    mStartParam->mailboxId(), mStartParam->folderId(),
                    mStartParam->messageId(), NmUiEditorForward);
                mApplication.enterNmUiView(startParam);
            }
            break;
            case NmActionResponseCommandDeleteMail: {
                mAttaManager.cancelFetch();
                deleteMessage();
                }
            break;
            default:
                break;
        }
    }
}

/*!
    Deletes the currently open message
*/
void NmViewerView::deleteMessage()
{
    NM_FUNCTION;
    
    QList<NmId> messageList;
    messageList.append(mStartParam->messageId());
    
    int err = mUiEngine.deleteMessages(mStartParam->mailboxId(),
                                       mStartParam->folderId(),
                                       messageList);
    
    messageList.clear();
    if (NmNoError != err) {
        // Failed to delete the messages!
        NMLOG(QString("NmViewerView::handleActionCommand(): failed err=%1").arg(err));
    }
}


/*!
    This is called when attachment fetch progress changes
*/
void NmViewerView::progressChanged(int value)
{
    if (mAttaIndexUnderFetch != NmNotFoundError) {
        // emit signal
        if (mAttaWidget && mAttaWidget->progressValue(mAttaIndexUnderFetch) < value) {
            progressValueChanged(mAttaIndexUnderFetch, value);
        }
    }
}

/*!
    This is called when attachment fetch is completed
*/
void NmViewerView::fetchCompleted(int result)
{
    if (mAttaWidget && mAttaIndexUnderFetch != NmNotFoundError) {
        if (result == NmNoError) {
            progressValueChanged(mAttaIndexUnderFetch, 100);
            openAttachment(mAttaIndexUnderFetch);
        } else {
            mAttaWidget->hideProgressBar(mAttaIndexUnderFetch);
        }
    }
    mAttaIndexUnderFetch = NmNotFoundError;
}

/*!
    externalDelete. From NmMessageListModel, handles viewer shutdown when current message is deleted.
*/
void NmViewerView::messageDeleted(const NmId &mailboxId, const NmId &folderId, const NmId &messageId)
{
    if ((mStartParam->viewId() == NmUiViewMessageViewer)
        && (mStartParam->mailboxId()== mailboxId)
        && (mStartParam->folderId()== folderId)
        && (mStartParam->messageId()== messageId)) {
        mApplication.prepareForPopView();
    }
}

/*!
    Helper function for wait dialog creation.
*/
void NmViewerView::createAndShowWaitDialog()
{
    delete mWaitDialog;
    mWaitDialog = NULL;
    // Create new wait dialog and set it to me modal with dimmed background
    mWaitDialog = new HbProgressDialog(HbProgressDialog::WaitDialog);
    // Connect to observe orientation change events
    connect(mWaitDialog->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
                this, SLOT(orientationChanged(Qt::Orientation)));
    mWaitDialog->setModal(true);
    mWaitDialog->setBackgroundFaded(true);
    connect(mWaitDialog, SIGNAL(cancelled()), this, SLOT(waitNoteCancelled()));
    mWaitDialog->setText(hbTrId("txt_mail_dialog_loading_mail_content"));
    // Display wait dialog
    mWaitDialog->show(); 
}
