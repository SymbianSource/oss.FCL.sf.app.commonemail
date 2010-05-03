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

static const double Un = 6.66;
static const double HeaderAreaMarginsTotal = 3 * Un;

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
    mBackgroundScrollArea->setLongPressEnabled(true);

    // Add header area handling widget into layout
    mHeaderWidget = new NmEditorHeader(documentLoader, this);

    // Get pointer to body text area handling widget
    mEditorWidget = qobject_cast<NmEditorTextEdit *>(documentLoader->findWidget(NMUI_EDITOR_BODY));

    // Set body editor to use NmEditorTextDocument
    NmEditorTextDocument *textDocument = new NmEditorTextDocument(manager);
    mEditorWidget->setDocument(textDocument); 
    textDocument->setParent(mEditorWidget); // ownership changes

    mEditorWidget->init(this, mBackgroundScrollArea);
    
    // Remove the comment to enable style picker menu item.
    mEditorWidget->setFormatDialog(new HbFormatDialog());

    // Create signal slot connections
    createConnections();
}

/*!
    Destructor
*/
NmEditorContent::~NmEditorContent()
{
   delete mHeaderWidget;
}

/*!
    Fill message data into header and body fileds. If reply envelopw is
    present, reply header is generated and set to editor. Reply
    envelope ownership is not transferred here.
 */
void NmEditorContent::setMessageData(const NmMessage &message,
                                     NmMessageEnvelope *replyMsgEnvelope)
{
    // Check which part is present. Html or plain text part
    const NmMessagePart *htmlPart = message.htmlBodyPart();
    const NmMessagePart *plainPart = message.plainTextBodyPart();

    QList<NmMessagePart*> parts;
    message.attachmentList(parts);
    NmMessagePart* attachmentHtml = NULL;

    foreach(NmMessagePart* part, parts) {
        if (part->contentDescription().startsWith( NmContentDescrAttachmentHtml )) {
                attachmentHtml = part;
            }
        }
    
    if (htmlPart) {	    
        // Html part was present, set it to HbTextEdit
        // This will generate contentsChanged() event which is used to
        // set new height for the editor widget and content.
        if(attachmentHtml){
            QString htmlText = htmlPart->textContent() + attachmentHtml->textContent();
            emit setHtml(htmlText);
        } 
        else{
            emit setHtml(htmlPart->textContent());    
        }
        mMessageBodyType = HTMLText;
    }
    else if (plainPart) {
        // Plain text part was present, set it to HbTextEdit
        emit setPlainText(plainPart->textContent());
        mMessageBodyType = PlainText;
    }
    
    // Original message text to editor content fiel
    if (replyMsgEnvelope && mEditorWidget) {          
        QTextCursor cursor = mEditorWidget->textCursor();
        cursor.setPosition(1);
        cursor.insertHtml(NmUtilities::createReplyHeader(*replyMsgEnvelope));
    }
}  

/*!
   This method set new height for the editor content when header or body field
   height has been changed.
 */
void NmEditorContent::setEditorContentHeight()
{
	const QSizeF reso = HbDeviceProfile::current().logicalSize();
    qreal containerHeight =
        mEditorWidget->contentHeight() + mHeaderWidget->headerHeight() + HeaderAreaMarginsTotal;
    if (containerHeight < reso.height()) {
        //Currently content height is too long because Chrome hiding is not supported.
        //Fix this when Chrome works.
        containerHeight = reso.height();
        qreal bodyContentHeight =
            reso.height() - mHeaderWidget->headerHeight() - HeaderAreaMarginsTotal;
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
    return mEditorWidget;
}

/*!
    Return pointer to the header widget
 */
NmEditorHeader* NmEditorContent::header() const
{
    return mHeaderWidget;
}

