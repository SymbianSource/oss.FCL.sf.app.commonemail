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

/*!
    Constructor
*/
NmEditorContent::NmEditorContent(QGraphicsItem *parent,
                                 NmEditorView *parentView,
                                 HbDocumentLoader *documentLoader,
                                 QNetworkAccessManager &manager) :
    HbWidget(parent),
    mHeaderWidget(NULL),
    mParentView(parentView),
    mEditorLayout(NULL),
    mMessageBodyType(PlainText),
    mEditorWidget(NULL),
    mBackgroundScrollArea((NmBaseViewScrollArea*)parent)
{
    NM_FUNCTION;

    // Add header area handling widget into layout
    mHeaderWidget = new NmEditorHeader(documentLoader);

    // Get pointer to body text area handling widget
    mEditorWidget = qobject_cast<NmEditorTextEdit *>(documentLoader->findWidget(NMUI_EDITOR_BODY));
    
    // Set body editor to use NmEditorTextDocument
    NmEditorTextDocument *textDocument = new NmEditorTextDocument(manager);
    mEditorWidget->setDocument(textDocument); 
    textDocument->setParent(mEditorWidget); // ownership changes

    mEditorWidget->init(mBackgroundScrollArea);
    // we are interested in the editor widget's height changes
    connect(mEditorWidget, SIGNAL(editorContentHeightChanged()), this,
        SLOT(setEditorContentHeight()));

    // Enable style picker menu item.
    mEditorWidget->setFormatDialog(new HbFormatDialog());

    // Create signal slot connections
    createConnections();
}

/*!
    Destructor
*/
NmEditorContent::~NmEditorContent()
{
    NM_FUNCTION;
    
   delete mHeaderWidget;
}

/*!
    Fill message data into header and body fileds. If reply envelopw is
    present, reply header is generated and set to editor. Reply
    envelope ownership is not transferred here.
 */
void NmEditorContent::setMessageData(const NmMessage &originalMessage)
{
    NM_FUNCTION;
    // Check which part is present. Html or plain text part. We use the original message parts.
    const NmMessagePart *htmlPart = originalMessage.htmlBodyPart();

    const NmMessagePart *plainPart = originalMessage.plainTextBodyPart();

    if (htmlPart) {
        emit setHtml(htmlPart->textContent());    
        mMessageBodyType = HTMLText;
    }
    else if (plainPart) {
        // Plain text part was present, set it to HbTextEdit
        emit setPlainText(plainPart->textContent());
        mMessageBodyType = PlainText;
    }
    
    // We create the "reply" header (also for forward message)
    if (mEditorWidget) {          
        QTextCursor cursor = mEditorWidget->textCursor();
        cursor.setPosition(0);
        cursor.insertHtml(NmUtilities::createReplyHeader(originalMessage.envelope()));
    }
}  

/*!
   This method set new height for the editor content when header or body field
   height has been changed.
 */
void NmEditorContent::setEditorContentHeight()
{
    NM_FUNCTION;
    
	const QSizeF reso = HbDeviceProfile::current().logicalSize();
    qreal containerHeight = mEditorWidget->contentHeight() + mHeaderWidget->headerHeight();
    if (containerHeight < reso.height()) {
        //Currently content height is too long because Chrome hiding is not supported.
        //Fix this when Chrome works.
        containerHeight = reso.height();
        qreal bodyContentHeight =
            reso.height() - mHeaderWidget->headerHeight();
        mEditorWidget->setPreferredHeight(bodyContentHeight);
        mEditorWidget->setMaximumHeight(bodyContentHeight);
    }
    mParentView->scrollAreaContents()->setMinimumHeight(containerHeight);
    mParentView->scrollAreaContents()->setMaximumHeight(containerHeight);
    mBackgroundScrollArea->setMaximumHeight(containerHeight);
}

/*!
   This method creates all needed signal-slot connections
 */
void NmEditorContent::createConnections()
{
    NM_FUNCTION;
    
    // Body edit widget is also interested about bg scroll position change
    connect(mBackgroundScrollArea, SIGNAL(scrollPositionChanged(QPointF)),
            mEditorWidget, SLOT(updateScrollPosition(QPointF)));
    // Signal for setting HbTextEdit widgets html content
    connect(this, SIGNAL(setHtml(QString)),
            mEditorWidget, SLOT(setHtml(QString)));
    // Signal for setting HbTextEdit widgets plain text content
    connect(this, SIGNAL(setPlainText(QString)),
            mEditorWidget, SLOT(setPlainText(QString)));
    // Inform text edit widget that header height has been changed
    connect(mHeaderWidget, SIGNAL(headerHeightChanged(int)),
            mEditorWidget, SLOT(setHeaderHeight(int)));
}

/*!
    Return pointer to the email body text edit widget
 */
NmEditorTextEdit* NmEditorContent::editor() const
{
    NM_FUNCTION;
    
    return mEditorWidget;
}

/*!
    Return pointer to the header widget
 */
NmEditorHeader* NmEditorContent::header() const
{
    NM_FUNCTION;
    
    return mHeaderWidget;
}

