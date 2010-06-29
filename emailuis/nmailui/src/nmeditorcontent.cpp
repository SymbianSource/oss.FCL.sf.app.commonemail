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
    "(<img[^<]+(src\\s*=\\s*)(.{0,1}cid)([^<]+)(>\\s*|/>\\s*|></img>\\s*))";
	
/*!
    Constructor
*/
NmEditorContent::NmEditorContent(QObject *parent,
                                 HbDocumentLoader *documentLoader,
                                 QNetworkAccessManager &manager,
                                 NmApplication &application) :
    QObject(parent),
    mHeader(NULL),
    mMessageBodyType(NmPlainText),
    mEditorWidget(NULL),
    mScrollArea(NULL),
    mScrollAreaContents(NULL),
    mApplication(application)
{
    NM_FUNCTION;

    // Construct container for the header widgets
    mHeader = new NmEditorHeader(this, documentLoader);

    // Get pointer to body text area handling widget
    mEditorWidget = qobject_cast<NmEditorTextEdit *>(documentLoader->findWidget(NMUI_EDITOR_BODY));
    
    // Set body editor to use NmEditorTextDocument
    NmEditorTextDocument *textDocument = new NmEditorTextDocument(manager);
    mEditorWidget->setDocument(textDocument); 
    textDocument->setParent(mEditorWidget); // ownership changes

    mScrollArea = qobject_cast<NmBaseViewScrollArea *>
        (documentLoader->findObject(NMUI_EDITOR_SCROLL_AREA));
    mScrollArea->setScrollDirections(Qt::Vertical | Qt::Horizontal);
    
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
    connect(mHeader, SIGNAL(headerHeightChanged(int)), this, SLOT(setEditorContentHeight()),
        Qt::QueuedConnection);

    // we are interested in the document's height changes
    connect(mEditorWidget->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF)), this,
        SLOT(setEditorContentHeight()), Qt::QueuedConnection);

    // We need to update the scroll position according the editor's cursor position
    connect(mHeader->toEdit(), SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(ensureCursorVisibility()), Qt::QueuedConnection);
    connect(mHeader->ccEdit(), SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(ensureCursorVisibility()), Qt::QueuedConnection);
    connect(mHeader->bccEdit(), SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(ensureCursorVisibility()), Qt::QueuedConnection);
    connect(mHeader->subjectEdit(), SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(ensureCursorVisibility()), Qt::QueuedConnection);
    connect(mEditorWidget, SIGNAL(cursorPositionChanged(int, int)), this, 
        SLOT(ensureCursorVisibility()), Qt::QueuedConnection);

    // listen to the parent's (NmEditorView) size changes which happen eg. when VKB is opened/closed
    connect(parent(), SIGNAL(sizeChanged()), this, SLOT(ensureCursorVisibility()),
        Qt::QueuedConnection);
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
    
    return mHeader;
}

/*!
    This slot is called when header widget height has been changed. Function performs
    the repositioning of the body field and resizing of the editor and content area.
 */
void NmEditorContent::setEditorContentHeight()
{
    NM_FUNCTION;
    
    // the height of the margin between the title bar and the header
    qreal topMargin = 0;
    HbStyle().parameter("hb-param-margin-gene-top", topMargin);
    
    // header height
    qreal headerHeight = mHeader->headerHeight();

    // body area editor's document height with margins added
    qreal documentHeightAndMargins = mEditorWidget->document()->size().height() +
        (mEditorWidget->document()->documentMargin() * 2);

    // chrome height
    qreal chromeHeight = 0;
    HbStyle().parameter("hb-param-widget-chrome-height", chromeHeight);
    
    // screen height
    qreal screenHeight = mApplication.screenSize().height();

    // set min size for the body area so that at least the screen area is always filled
    qreal bodyAreaMinSize = screenHeight - chromeHeight - topMargin - headerHeight;
    
    qreal bodyAreaSize = fmax(bodyAreaMinSize, documentHeightAndMargins);

    mScrollAreaContents->setPreferredHeight(topMargin + headerHeight + bodyAreaSize);
}

/*!
    This slot is called when the cursor visibility has to be ensured ie. the scroll position is 
    adjusted so that the cursor can be seen.
*/
void NmEditorContent::ensureCursorVisibility()
{
    NM_FUNCTION;

    // check which of the editors has the focus and get the x/y coordinates for the cursor position
    QGraphicsWidget *focused = mScrollAreaContents->focusWidget();
    
    if (focused) {
        QRectF localRect(0, 0, 0, 0);
        bool notFound = false;
        
        if (focused == mHeader->toEdit()) {
            localRect = mHeader->toEdit()->rectForCursorPosition();
        }
        else if (focused == mHeader->ccEdit()) {
            localRect = mHeader->ccEdit()->rectForCursorPosition();
        }
        else if (focused == mHeader->bccEdit()) {
            localRect = mHeader->bccEdit()->rectForCursorPosition();
        }
        else if (focused == mHeader->subjectEdit()) {
            localRect = mHeader->subjectEdit()->rectForCursorPosition();
        }
        else if (focused == mEditorWidget) {
            localRect = mEditorWidget->rectForCursorPosition();
        }
        else {
            notFound = true;
        }

        if (!notFound) {
            QPointF topLeftPos = focused->mapToItem(mScrollAreaContents, localRect.topLeft());
            QPointF bottomRightPos =
                focused->mapToItem(mScrollAreaContents, localRect.bottomRight());
            qreal marginRight = 0;
            HbStyle().parameter("hb-param-margin-gene-right", marginRight);
            bottomRightPos.rx() += marginRight;
            mScrollArea->ensureVisible(topLeftPos);
            mScrollArea->ensureVisible(bottomRightPos);
        }
    }
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
