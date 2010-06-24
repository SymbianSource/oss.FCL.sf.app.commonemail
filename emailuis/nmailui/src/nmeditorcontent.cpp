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
* Description: Message editor contents widget
*
*/

#include "nmuiheaders.h"

// Layout
static const char *NMUI_EDITOR_BODY = "BodyTextEdit";
static const char *NMUI_EDITOR_SCROLL_AREA = "scrollArea";
static const char *NMUI_EDITOR_SCROLL_AREA_CONTENTS = "scrollAreaContents";

// Regular expression for selecting img tags with "cid" in the mail.
static const char *NMUI_EDITOR_REMOVE_EMBD_IMAGES_REG = 
    "(<img (src\\s*=\\s*)(.{0,1}cid)([^<]+)(>\\s*|/>\\s*|></img>\\s*))";
	
/*!
    Constructor
*/
NmEditorContent::NmEditorContent(QObject *parent,
                                 HbDocumentLoader *documentLoader,
                                 QNetworkAccessManager &manager,
                                 NmApplication &application) :
    QObject(parent),
    mHeaderWidget(NULL),
    mMessageBodyType(NmPlainText),
    mEditorWidget(NULL),
    mScrollArea(NULL),
    mScrollAreaContents(NULL),
    mApplication(application)
{
    NM_FUNCTION;

    // Construct container for the header widgets
    mHeaderWidget = new NmEditorHeader(this, documentLoader);

    // Get pointer to body text area handling widget
    mEditorWidget = qobject_cast<NmEditorTextEdit *>(documentLoader->findWidget(NMUI_EDITOR_BODY));
    
    // Set body editor to use NmEditorTextDocument
    NmEditorTextDocument *textDocument = new NmEditorTextDocument(manager);
    mEditorWidget->setDocument(textDocument); 
    textDocument->setParent(mEditorWidget); // ownership changes

    mScrollArea = qobject_cast<NmBaseViewScrollArea *>
        (documentLoader->findObject(NMUI_EDITOR_SCROLL_AREA));

    // Enable style picker menu item.
    mEditorWidget->setFormatDialog(new HbFormatDialog());

    mScrollAreaContents =
        qobject_cast<HbWidget *>(documentLoader->findWidget(NMUI_EDITOR_SCROLL_AREA_CONTENTS));
    
    // Create signal slot connections
    createConnections();
}

/*!
    Destructor
*/
NmEditorContent::~NmEditorContent()
{
    NM_FUNCTION;
}

/*!
    Fill message data into header and body fileds. If reply envelopw is
    present, reply header is generated and set to editor. Reply
    envelope ownership is not transferred here.
 */
void NmEditorContent::setMessageData(const NmMessage &originalMessage, 
                                     NmUiEditorStartMode &editorStartMode)
{
    NM_FUNCTION;
    
    QString bodyContent;
    
    // Create the "reply" header (also for forward message)
    if (editorStartMode==NmUiEditorReply || editorStartMode==NmUiEditorReplyAll || 
        editorStartMode==NmUiEditorForward) {
        bodyContent.append(NmUtilities::createReplyHeader(originalMessage.envelope()));
    }
    
    // Check which part is present. Html or plain text part. We use the original message parts.
    const NmMessagePart *htmlPart = originalMessage.htmlBodyPart();
    const NmMessagePart *plainPart = originalMessage.plainTextBodyPart();
 
    if (htmlPart) {
        bodyContent.append(htmlPart->textContent());
        if(editorStartMode==NmUiEditorReply || editorStartMode==NmUiEditorReplyAll ) {
            removeEmbeddedImages(bodyContent);
        }
        emit setHtml(bodyContent);
        mMessageBodyType = NmHTMLText;
    }
    else if (plainPart) {
        // Plain text part was present, set it to HbTextEdit
        bodyContent.append(plainPart->textContent());
        emit setPlainText(bodyContent);
        mMessageBodyType = NmPlainText;
    }
}  

/*!
   This method creates all needed signal-slot connections
 */
void NmEditorContent::createConnections()
{
    NM_FUNCTION;
    
    // Signal for setting HbTextEdit widgets html content
    connect(this, SIGNAL(setHtml(QString)),
            mEditorWidget, SLOT(setHtml(QString)), Qt::QueuedConnection);

    // Signal for setting HbTextEdit widgets plain text content
    connect(this, SIGNAL(setPlainText(QString)),
            mEditorWidget, SLOT(setPlainText(QString)), Qt::QueuedConnection);

    // Inform text edit widget that header height has been changed
    connect(mHeaderWidget, SIGNAL(headerHeightChanged(int)), this, SLOT(setEditorContentHeight()),
        Qt::QueuedConnection);

    // we are interested in the document's height changes
    connect(mEditorWidget->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this,
        SLOT(setEditorContentHeight()), Qt::QueuedConnection);

    // We need to update the scroll position according the editor cursor position
    connect(mEditorWidget, SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(setScrollPosition(int, int)), Qt::QueuedConnection);
    
    // We need to know the scroll area's current position for calculating the new position in
	// setScrollPosition
    connect(mScrollArea, SIGNAL(scrollPositionChanged(QPointF)), this, 
        SLOT(updateScrollPosition(QPointF)), Qt::QueuedConnection);
}

/*!
    Return pointer to the email body text edit widget
 */
NmEditorTextEdit *NmEditorContent::editor() const
{
    NM_FUNCTION;
    
    return mEditorWidget;
}

/*!
    Return pointer to the header widget
 */
NmEditorHeader *NmEditorContent::header() const
{
    NM_FUNCTION;
    
    return mHeaderWidget;
}

/*!
    This slot is called when header widget height has been changed. Function performs
    the repositioning of the body field and resizing of the editor and content area.
 */
void NmEditorContent::setEditorContentHeight()
{
    NM_FUNCTION;
    
    qreal topMargin = 0;
    HbStyle().parameter("hb-param-margin-gene-top", topMargin);
    
    // header height
    qreal headerHeight = mHeaderWidget->headerHeight();

    // body area editor's document height with margins added
    qreal documentHeightAndMargins = mEditorWidget->document()->size().height() +
        (mEditorWidget->document()->documentMargin() * 2);

    // get the chrome height
    qreal chromeHeight = 0;
    HbStyle().parameter("hb-param-widget-chrome-height", chromeHeight);
    
    qreal toolbarHeight = 0;
    HbStyle().parameter("hb-param-widget-toolbar-height", toolbarHeight);

    // screen height
    qreal screenHeight = mApplication.screenSize().height();

    // set min size for the body area so that at least the screen area is always filled
    qreal bodyAreaMinSize = screenHeight - chromeHeight - topMargin - headerHeight;
    
    qreal bodyAreaSize = fmax(bodyAreaMinSize, documentHeightAndMargins);

    mScrollAreaContents->setPreferredHeight(topMargin + headerHeight + bodyAreaSize);
}

/*!
    This slot is called when cursor position is changed in body area using
    'pointing stick' or keyboard. Function will update the scroll position
    of the content so that cursor does not go outside of the screen or
    behind the virtual keyboard.
 */
void NmEditorContent::setScrollPosition(int oldPos, int newPos)
{
    NM_FUNCTION;
    
    Q_UNUSED(oldPos);

    qreal chromeHeight = 0;
    HbStyle().parameter("hb-param-widget-chrome-height", chromeHeight);
    
    const QSizeF screenReso = mApplication.screenSize();
    qreal maxHeight = screenReso.height() - chromeHeight;

    // Get cursor position coordinates
    QRectF cursorPosPix = mEditorWidget->rectForPosition(newPos);
    
    // Calculate the screen top and bottom boundaries, this means the part of the
    // background scroll area which is currently visible.
    qreal visibleRectTopBoundary;
    qreal visibleRectBottomBoundary;
    
    qreal headerHeight = mHeaderWidget->headerHeight();

    if (mScrollPosition.y() < headerHeight) {
        // Header is completely or partially visible
        visibleRectTopBoundary = headerHeight - mScrollPosition.y();
        visibleRectBottomBoundary = maxHeight - visibleRectTopBoundary;
    }
    else {
        // Header is not visible
        visibleRectTopBoundary = mScrollPosition.y() - headerHeight;
        visibleRectBottomBoundary = visibleRectTopBoundary + maxHeight;
    }

    // Do scrolling if cursor is out of the screen boundaries
    if (cursorPosPix.y() > visibleRectBottomBoundary) {
        // Do scroll forward
        mScrollArea->scrollContentsTo(QPointF(0, cursorPosPix.y() - maxHeight + headerHeight));
    }
    else if (cursorPosPix.y() + headerHeight < mScrollPosition.y()) {
        // Do scroll backward
        mScrollArea->scrollContentsTo(QPointF(0, cursorPosPix.y() + headerHeight));
    }
}

/*!
    This slot is called when background scroll areas scroll position has been shanged.
*/
void NmEditorContent::updateScrollPosition(const QPointF &newPosition)
{
    NM_FUNCTION;
    
    mScrollPosition = newPosition;
}

/*!
    Removes embedded images from the message body
 */
void NmEditorContent::removeEmbeddedImages(QString &bodyContent)
{
    NM_FUNCTION;
 
    QRegExp regExp(NMUI_EDITOR_REMOVE_EMBD_IMAGES_REG, Qt::CaseInsensitive);
    bodyContent.remove(regExp);
}