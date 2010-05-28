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
* Description: Message editor header container class. Collects the header widgets.
*
*/

#include "nmuiheaders.h"

// Layout
// These match to the defintions in nmeditorview.docml
static const char *NMUI_EDITOR_CONTAINER = "containerContents";
static const char *NMUI_EDITOR_SUBJECT_FIELD = "editorSubjectField";
static const char *NMUI_EDITOR_SUBJECT_EDIT = "editorSubjectEdit";
static const char *NMUI_EDITOR_CC_FIELD = "editorCcField";
static const char *NMUI_EDITOR_BCC_FIELD = "editorBccField";
static const char *NMUI_EDITOR_PRIORITY_ICON = "editPriorityIcon";
static const char *NMUI_EDITOR_FOLLOWUP_ICON = "editFollowUpIcon";
static const char *NMUI_EDITOR_ATTACHMENT_LIST = "attachmentListWidget";
static const char *NMUI_EDITOR_PREFIX_TO = "editorTo";
static const char *NMUI_EDITOR_PREFIX_CC = "editorCc";
static const char *NMUI_EDITOR_PREFIX_BCC = "editorBcc";

static const int MaxRows = 10000;

static const int nmLayoutSystemWaitTimer = 10;

/*!
    Constructor
*/
NmEditorHeader::NmEditorHeader(HbDocumentLoader *documentLoader) :
    mDocumentLoader(documentLoader),
    mHeaderHeight(0),
    mIconVisible(false),
    mSubjectEdit(NULL),
    mRecipientFieldsEmpty(true),
    mAttachmentList(NULL),
    mToField(NULL),
    mCcField(NULL),
    mBccField(NULL),
    mCcBccFieldVisible(false)
{
    NM_FUNCTION;
    
    loadWidgets();
    createConnections();
}

/*!
    Destructor
*/
NmEditorHeader::~NmEditorHeader()
{
    NM_FUNCTION;
}

/*!
    Load widgets from XML for the header.
*/
void NmEditorHeader::loadWidgets()
{
    NM_FUNCTION;
	
    // Load widgets from docml and construct handlers. Those widgets that are not shown by default
    // are hidden and removed from the layout at this phase.    
    HbWidget *contentWidget =
        qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_CONTAINER));
    mLayout = static_cast<QGraphicsLinearLayout *>(contentWidget->layout());

    // base class QObject takes the deleting responsibility
    mToField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_TO);
    mCcField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_CC);
    mBccField = new NmRecipientField(this, *mDocumentLoader, NMUI_EDITOR_PREFIX_BCC);

    // Cc field is not shown by default. It needs to be both hidden and removed from the layout.
	mCcWidget = qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_CC_FIELD));
    mCcWidget->hide();
    mLayout->removeItem(mCcWidget);
    
    // Bcc field is not shown by default. It needs to be both hidden and removed from the layout.
    mBccWidget = qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_BCC_FIELD));
    mBccWidget->hide();
    mLayout->removeItem(mBccWidget);

    mSubjectWidget =
        qobject_cast<HbWidget *>(mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_FIELD));
    mSubjectLayout = static_cast<QGraphicsLinearLayout *>(mSubjectWidget->layout());
    
    // Add Subject: field
    mSubjectEdit = qobject_cast<NmHtmlLineEdit *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_EDIT));
    mSubjectEdit->setMaxRows(MaxRows);

    // Add attachment list
    NmAttachmentListWidget *attachmentList = qobject_cast<NmAttachmentListWidget *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_ATTACHMENT_LIST));
    // Create attachment list handling object
    mAttachmentList = new NmAttachmentList(*attachmentList);
    mAttachmentList->setParent(this); // ownership changes
    attachmentList->hide();
    mLayout->removeItem(attachmentList);

    // Add priority icon
    mPriorityIcon = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_PRIORITY_ICON));
    mPriorityIcon->hide();
    mSubjectLayout->removeItem(mPriorityIcon);

    // follow-up icon is not yet supported
    HbLabel *followUpIcon = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_FOLLOWUP_ICON));
    followUpIcon->hide();
    mSubjectLayout->removeItem(followUpIcon);
}

/*!
    Create signal - slot connections.
*/
void NmEditorHeader::createConnections()
{
    NM_FUNCTION;
    
    // Signals for checking if the recipient fields have text.
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));

    // Signals for handling the recipient field expanding
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendDelayedHeaderHeightChanged()));
    connect(mSubjectEdit, SIGNAL(contentsChanged()), this, SLOT(sendDelayedHeaderHeightChanged()));

    // Signals for handling the attachment list
    connect(&mAttachmentList->listWidget(), SIGNAL(itemActivated(int)),
            this, SLOT(attachmentActivated(int)));
    connect(&mAttachmentList->listWidget(), SIGNAL(longPressed(int, QPointF)),
            this, SLOT(attachmentLongPressed(int, QPointF)));
    connect(mAttachmentList ,SIGNAL(attachmentListLayoutChanged()),
            this, SLOT(sendHeaderHeightChanged()));
}

/*!
   Show or hide recipient field
*/
void NmEditorHeader::setFieldVisibility(bool isVisible)
{
	if ( mCcBccFieldVisible != isVisible ) {
		mCcBccFieldVisible = isVisible;
		if (mCcBccFieldVisible) {
            mLayout->insertItem(1, mBccWidget);
			mLayout->insertItem(1, mCcWidget);
			mCcWidget->show();
            mBccWidget->show();
		}
		else {
            mCcWidget->hide();
            mBccWidget->hide();
			mLayout->removeItem(mCcWidget);
			mLayout->removeItem(mBccWidget);
		}

		QTimer::singleShot(
		    nmLayoutSystemWaitTimer * 2, this, SLOT(sendDelayedHeaderHeightChanged()));
	}
}

/*!
    Return the height of the whole header widget.
 */
qreal NmEditorHeader::headerHeight() const
{
    NM_FUNCTION;

    // get the layout's vertical spacing
    qreal spacing = 0;
    HbStyle().parameter("hb-param-margin-gene-middle-vertical", spacing);

    // calculate the height
    qreal height = 0;
    
    height += mToField->height(); // returns widget's geometry height
    height += spacing;
    
    if (mCcBccFieldVisible) {
        height += mCcField->height(); // returns widget's geometry height
        height += spacing;
        
        height += mBccField->height(); // returns widget's geometry height
        height += spacing;
    }

    height += mSubjectWidget->geometry().height();
    height += spacing;

    if (mAttachmentList->listWidget().isVisible()) {
        height += mAttachmentList->listWidget().geometry().height();
        height += spacing;
    }

    return height;
}

/*!
    Send signal to inform that one of the recipient fields height has been changed.
 */
void NmEditorHeader::sendDelayedHeaderHeightChanged()
{
    NM_FUNCTION;
	QTimer::singleShot(nmLayoutSystemWaitTimer * 5, this, SLOT(sendHeaderHeightChanged()));
}

/*!
    Send signal to inform that one of the recipient fields height has been changed.
 */
void NmEditorHeader::sendHeaderHeightChanged()
{
    qreal hHeight = headerHeight();
    if (mHeaderHeight != hHeight) {
        mHeaderHeight = hHeight;
        emit headerHeightChanged(mHeaderHeight);
    }
}

/*!
    Return pointer to to edit
 */
NmRecipientLineEdit* NmEditorHeader::toEdit() const
{
    NM_FUNCTION;
    
    return mToField->editor();
}

/*!
    Return pointer to cc edit
 */
NmRecipientLineEdit* NmEditorHeader::ccEdit() const
{
    NM_FUNCTION;
    
    return mCcField->editor();
}

/*!
    Return pointer to bcc edit
 */
NmRecipientLineEdit* NmEditorHeader::bccEdit() const
{
    NM_FUNCTION;
    
    return mBccField->editor();
}

/*!
    Return pointer to subject field
 */
NmHtmlLineEdit* NmEditorHeader::subjectEdit() const
{
    NM_FUNCTION;
    
    return mSubjectEdit;
}

/*!
    Checks if recipient editors are empty and emits a signal if
    the state is changed.
*/
void NmEditorHeader::editorContentChanged()
{
    NM_FUNCTION;
    
    bool recipientsFieldsEmpty = true;
    if (mToField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    else if (mCcField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    else if (mBccField->text().length()) {
        recipientsFieldsEmpty = false;
    }
    if (mRecipientFieldsEmpty != recipientsFieldsEmpty) {
        mRecipientFieldsEmpty = recipientsFieldsEmpty;
        emit recipientFieldsHaveContent(!mRecipientFieldsEmpty);
    }
}

/*!
    Sets the icon for priority
 */
void NmEditorHeader::setPriority(NmMessagePriority priority)
{
    NM_FUNCTION;
    
    switch (priority) {
    case NmMessagePriorityHigh:
        setPriority(NmActionResponseCommandPriorityHigh);
        break;
    case NmMessagePriorityLow:
        setPriority(NmActionResponseCommandPriorityLow);
        break;
    default:
        setPriority(NmActionResponseCommandNone);
        break;
    }
}

/*!
    Sets the icon for priority
 */
void NmEditorHeader::setPriority(NmActionResponseCommand prio)
{
    NM_FUNCTION;
    
    switch(prio) {
    case NmActionResponseCommandPriorityHigh:
        if (!mIconVisible) {
            mIconVisible = true;
            // icon widget is just after the subject line edit (see docml)
            mSubjectLayout->insertItem(2, mPriorityIcon);
            mPriorityIcon->show();
        }
        mPriorityIcon->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
        break;
    case NmActionResponseCommandPriorityLow:
        if (!mIconVisible) {
            mIconVisible = true;
            // icon widget is just after the subject line edit (see docml)
            mSubjectLayout->insertItem(2, mPriorityIcon);
            mPriorityIcon->show();
        }
        mPriorityIcon->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityLow));
        break;
    default:
        if (mIconVisible) {
            mIconVisible = false;
            HbIcon emptyIcon;
            mPriorityIcon->setIcon(emptyIcon);
            mSubjectLayout->removeItem(mPriorityIcon);
            mPriorityIcon->hide();
        }
        break;
    }
    // Update subject field height because row amount might have been changed.
    QTimer::singleShot(nmLayoutSystemWaitTimer * 3, this, SLOT(sendDelayedHeaderHeightChanged()));
}

/*!
   Adds an attachment to attachment list for mail.
 */
void NmEditorHeader::addAttachment(
    const QString &fileName, const QString &fileSize, const NmId &nmid)
{
    NM_FUNCTION;
    
    mAttachmentList->insertAttachment(fileName, fileSize, nmid);
    if (!mAttachmentList->listWidget().isVisible()) {
        // attachment list is inserted just before the body widget (see docml).
        mLayout->insertItem(mLayout->count() - 1, &mAttachmentList->listWidget());
        mAttachmentList->listWidget().show();
    }
    sendHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment adding has failed and attachment id is not known.
 */
void NmEditorHeader::removeAttachment(const QString &fileName)
{
    NM_FUNCTION;
    
    mAttachmentList->removeAttachment(fileName);
    if (mAttachmentList->count() == 0) {
        mAttachmentList->listWidget().hide();
        mLayout->removeItem(&mAttachmentList->listWidget());
    }
    sendHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment has been selected for remove.
 */
void NmEditorHeader::removeAttachment(const NmId &nmid)
{
    NM_FUNCTION;
    
    mAttachmentList->removeAttachment(nmid);
    sendHeaderHeightChanged();
}

/*!
    This function set messagePartId and fileSize for attachment.
 */
void NmEditorHeader::setAttachmentParameters(
    const QString &fileName,
    const NmId &msgPartId,
    const QString &fileSize,
    int result)
{
    NM_FUNCTION;
    
    if (result == NmNoError) {
        // Attachment adding succesful, set message part id and size for attachment
        mAttachmentList->setAttachmentPartId(fileName, msgPartId);
        mAttachmentList->setAttachmentSize(msgPartId, fileSize);
    }
}

/*!
   Attachment launched from attachment list by "open" menu item.
 */
void NmEditorHeader::launchAttachment(const NmId &itemId)
{
    NM_FUNCTION;
    
    attachmentActivated(mAttachmentList->indexByNmId(itemId));
}

/*!
   Slot attachment activated from attachment list by short tap.
 */
void NmEditorHeader::attachmentActivated(int arrayIndex)
{
    NM_FUNCTION;
    
    QFile launchFile(mAttachmentList->getFullFileNameByIndex(arrayIndex));
    if (NmUtilities::openFile( launchFile ) == NmNotFoundError) {
        NmUtilities::displayErrorNote(hbTrId("txt_mail_dialog_unable_to_open_attachment_file_ty")); 
    }
}

/*!
   Slot attachment selected from attachment list by longtap.
 */
void NmEditorHeader::attachmentLongPressed(int arrayIndex, QPointF point)
{
    NM_FUNCTION;
    
    // Remove selected attachment
    emit attachmentLongPressed(mAttachmentList->nmIdByIndex(arrayIndex), point);
}
    
