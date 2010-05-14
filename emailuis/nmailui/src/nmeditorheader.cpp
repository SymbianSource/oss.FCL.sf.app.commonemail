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
* Description: Message editor header widget
*
*/

#include "nmuiheaders.h"

// Layout
static const char *NMUI_EDITOR_RECIPIENT_FIELDS = "RecipientFields";
static const char *NMUI_EDITOR_SUBJECT_LABEL = "EditorHeaderSubjectLabel";
static const char *NMUI_EDITOR_SUBJECT_EDIT = "EditorHeaderSubjectEdit";
static const char *NMUI_EDITOR_PRIORITY_ICON = "labelPriorityIcon";
static const char *NMUI_EDITOR_ATTACHMENT_LIST = "attachmentListWidget";

// Following constants are removed when header fields will offer these
static const double Un = 6.66;
static const double FieldHeightWhenSecondaryFont = 5 * Un;
static const double Margin = 2 * Un;
static const double IconFieldWidth = 5 * Un;

static const int nmLayoutSystemWaitTimer = 10;

/*!
    Constructor
*/
NmEditorHeader::NmEditorHeader(HbDocumentLoader *documentLoader, QGraphicsItem *parent) :
    HbWidget(parent),
    mDocumentLoader(documentLoader),
    mHeaderHeight(0),
    mSubjectLabel(NULL),
    mIconVisible(false),
    mSubjectEdit(NULL),
    mRecipientFieldsEmpty(true),
    mAttachmentList(NULL),
    mToField(NULL),
    mCcField(NULL),
    mBccField(NULL),
    mCcBccFieldVisible(false)
{
    loadWidgets();
    rescaleHeader();
    createConnections();
}

/*!
    Destructor
*/
NmEditorHeader::~NmEditorHeader()
{
    if (mAttachmentList) {
        mAttachmentList->clearList();
        delete mAttachmentList;
    }
}

/*!
    Load widgets from XML for the header.
*/
void NmEditorHeader::loadWidgets()
{
	// Add "To:", "CC:" and "BCC:" fields
	mHeader = qobject_cast<HbWidget *>
	        (mDocumentLoader->findWidget(NMUI_EDITOR_RECIPIENT_FIELDS));

	mLayout = new QGraphicsLinearLayout(Qt::Vertical);
	mLayout->setContentsMargins(0,Un,0,0);

	mToField = new NmRecipientField(QString("To:"), mHeader);
	mCcField = new NmRecipientField(QString("Cc:"), mHeader);
	mBccField = new NmRecipientField(QString("Bcc:"), mHeader);

    // Only 'To:' field is visible at startup
	mLayout->addItem(mToField);
	mCcField->hide();
	mBccField->hide();

	mHeader->setLayout(mLayout);
	mHeader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    // Add Subject: field
    mSubjectLabel = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_LABEL));
    mSubjectEdit = qobject_cast<NmHtmlLineEdit *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_EDIT));

    // Add attachment list
    NmAttachmentListWidget *attachmentList = qobject_cast<NmAttachmentListWidget *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_ATTACHMENT_LIST));
    // Create attachment list handling object
    mAttachmentList = new NmAttachmentList(*attachmentList);

    mPriorityIconLabel = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_PRIORITY_ICON));
}

/*!
    Create signal - slot connections.
*/
void NmEditorHeader::createConnections()
{
    // Signals for checking if the recipient fields have text.
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(editorContentChanged()));

    // Signals for handling the recipient field expanding
    connect(mToField, SIGNAL(textChanged(const QString &)),
            this, SLOT(fixHeaderFieldHeights()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(fixHeaderFieldHeights()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(fixHeaderFieldHeights()));
    connect(mSubjectEdit, SIGNAL(contentsChanged()), this, SLOT(fixHeaderFieldHeights()));

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
			mCcField->show();
			mBccField->show();
			mLayout->addItem(mCcField);
			mLayout->addItem(mBccField);
		}
		else {
			mCcField->hide();
			mBccField->hide();
			mLayout->removeItem(mCcField);
			mLayout->removeItem(mBccField);
		}

		QTimer::singleShot(nmLayoutSystemWaitTimer*2, this, SLOT(fixHeaderFieldHeights()));
	}
}

/*!
    Function can be used to rescale the header area.
*/
void NmEditorHeader::rescaleHeader()
{
}

/*!
    Return the height of the whole header widget.
    (Should find beter way to get the height of the header area.)
 */
int NmEditorHeader::headerHeight() const
{
    qreal toHeight = mToField->height();
    qreal ccHeight = 0;
    qreal bccHeight = 0;
    if (mCcBccFieldVisible) {
		ccHeight = mCcField->height();
		bccHeight = mBccField->height();
    }
    qreal subjectHeight = mSubjectEdit->geometry().height() + Margin;

    // When called first time, height is wrongly 'Margin'
    if (toHeight == Margin) {
        toHeight = FieldHeightWhenSecondaryFont;
    	ccHeight = FieldHeightWhenSecondaryFont;
    	bccHeight = FieldHeightWhenSecondaryFont;
        subjectHeight = FieldHeightWhenSecondaryFont;
    }

    qreal attHeight = 0;
    if (mAttachmentList && mAttachmentList->count() > 0) {
        attHeight = mAttachmentList->listWidget().geometry().height();
    }

    return (int)(toHeight + ccHeight + bccHeight + subjectHeight + attHeight);
}

/*!
    Send signal to inform that one of the recipient fields height has been changed.
 */
void NmEditorHeader::fixHeaderFieldHeights()
{
    // Adjust height of recipient fields 
	adjustFieldSizeValues(mToField->editor(), mToField->editor()->document()->size().height());
	adjustFieldSizeValues(mCcField->editor(), mCcField->editor()->document()->size().height());
	adjustFieldSizeValues(mBccField->editor(), mBccField->editor()->document()->size().height());

    // Adjust height of the subject field 
	if (mSubjectEdit->document()->size().height() > FieldHeightWhenSecondaryFont) {
	mSubjectEdit->setPreferredHeight(
			mSubjectEdit->document()->size().height() + Margin );
	}
	else {
		mSubjectEdit->setPreferredHeight( FieldHeightWhenSecondaryFont );
	}

	QTimer::singleShot(nmLayoutSystemWaitTimer*5, this, SLOT(sendHeaderHeightChanged()));	
}


/*!
    Private routine to adjust heights of the recipient fields
 */
void NmEditorHeader::adjustFieldSizeValues( NmRecipientLineEdit *widget, qreal height )
{
	if (height > FieldHeightWhenSecondaryFont) {
		widget->setMaximumHeight( height + Margin );
		widget->setMinimumHeight( height + Margin );
	}
	else {
		widget->setMaximumHeight( FieldHeightWhenSecondaryFont );	
		widget->setMinimumHeight( FieldHeightWhenSecondaryFont );	
	}	
}

/*!
    Send signal to inform that one of the recipient fields height has been changed.
 */
void NmEditorHeader::sendHeaderHeightChanged()
{
    int hHeight = headerHeight();
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
    return mToField->editor();
}

/*!
    Return pointer to cc edit
 */
NmRecipientLineEdit* NmEditorHeader::ccEdit() const
{
    return mCcField->editor();
}

/*!
    Return pointer to bcc edit
 */
NmRecipientLineEdit* NmEditorHeader::bccEdit() const
{
    return mBccField->editor();
}

/*!
    Return pointer to subject field
 */
NmHtmlLineEdit* NmEditorHeader::subjectEdit() const
{
    return mSubjectEdit;
}

/*!
    Checks if recipient editors are empty and emits a signal if
    the state is changed.
*/
void NmEditorHeader::editorContentChanged()
{
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
    switch(prio) {
    case NmActionResponseCommandPriorityHigh:
        if (!mIconVisible) {
            mIconVisible = true;
            mPriorityIconLabel->setMaximumWidth(IconFieldWidth);
            mSubjectEdit->setMaximumWidth(mSubjectEdit->geometry().width() - IconFieldWidth);
        }
        mPriorityIconLabel->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityHigh));
        break;
    case NmActionResponseCommandPriorityLow:
        if (!mIconVisible) {
            mIconVisible = true;
            mPriorityIconLabel->setMaximumWidth(IconFieldWidth);
            mSubjectEdit->setMaximumWidth(mSubjectEdit->geometry().width() - IconFieldWidth);
        }
        mPriorityIconLabel->setIcon(
            NmIcons::getIcon(NmIcons::NmIconPriorityLow));
        break;
    default:
        if (mIconVisible) {
            mIconVisible = false;
            HbIcon emptyIcon;
            mPriorityIconLabel->setIcon(emptyIcon);
            mPriorityIconLabel->setMaximumWidth(0);
            mSubjectEdit->setMaximumWidth(mSubjectEdit->geometry().width() + IconFieldWidth);
        }
        break;
    }
    // Update subject field height because row amount might have been changed.
    QTimer::singleShot(nmLayoutSystemWaitTimer * 3, this, SLOT(fixHeaderFieldHeights()));
}

/*!
   Adds an attachment to attachment list for mail.
 */
void NmEditorHeader::addAttachment(
    const QString &fileName, const QString &fileSize, const NmId &nmid)
{
    mAttachmentList->insertAttachment(fileName, fileSize, nmid);
    sendHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment adding has failed and attachment id is not known.
 */
void NmEditorHeader::removeAttachment(const QString &fileName)
{
    mAttachmentList->removeAttachment(fileName);
    sendHeaderHeightChanged();
}

/*!
   Remove attachment from the list. This function is used when
   attachment has been selected for remove.
 */
void NmEditorHeader::removeAttachment(const NmId &nmid)
{
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
    attachmentActivated(mAttachmentList->indexByNmId(itemId));
}

/*!
   Slot attachment activated from attachment list by short tap.
 */
void NmEditorHeader::attachmentActivated(int arrayIndex)
{
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
    // Remove selected attachment
    emit attachmentLongPressed(mAttachmentList->nmIdByIndex(arrayIndex), point);
}
    
