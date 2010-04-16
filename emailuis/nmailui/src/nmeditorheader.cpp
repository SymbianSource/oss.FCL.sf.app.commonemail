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
static const char *NMUI_EDITOR_TO_LABEL = "EditorHeaderToLabel";
static const char *NMUI_EDITOR_TO_EDIT = "EditorHeaderToEdit";
static const char *NMUI_EDITOR_TO_BUTTON = "EditorHeaderToButton";
static const char *NMUI_EDITOR_RECIPIENT_GB = "EditorHeaderRecipientGB";
static const char *NMUI_EDITOR_SUBJECT_LABEL = "EditorHeaderSubjectLabel";
static const char *NMUI_EDITOR_SUBJECT_EDIT = "EditorHeaderSubjectEdit";
static const char *NMUI_EDITOR_PRIORITY_ICON = "labelPriorityIcon";
static const char *NMUI_EDITOR_ATTACHMENT_LIST = "attachmentListWidget";

// Following constants are removed when header fields will offer these
static const double Un = 6.66;
static const double FieldHeightWhenSecondaryFont = 5 * Un;
static const int GroupBoxTitleHeight = 42;
static const double Margin = 2 * Un;
static const double HeaderAreaMarginsTotal = 3 * Un;
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
    mGroupBoxRecipient(NULL),
    mAttachmentList(NULL)
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
    // To: field objects
    HbLabel *toLabel = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_TO_LABEL));
    toLabel->setPlainText(hbTrId("txt_mail_editor_to"));
    NmRecipientLineEdit *toEdit = qobject_cast<NmRecipientLineEdit *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_TO_EDIT));
    HbPushButton *toButton = qobject_cast<HbPushButton *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_TO_BUTTON));
    mToField = new NmRecipientField(toLabel, toEdit, toButton);

    // Create recipient group box which includes cc and bcc fields
    mGroupBoxRecipient = qobject_cast<HbGroupBox *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_RECIPIENT_GB));
    // Ownership is transfered
    mGroupBoxRecipient->setContentWidget(createRecipientGroupBoxContentWidget());
    mGroupBoxRecipient->setHeading(hbTrId("txt_mail_subhead_ccbcc"));
    mGroupBoxRecipient->setCollapsable(true);
    mGroupBoxRecipient->setCollapsed(true);

    // Add Subject: field
    mSubjectLabel = qobject_cast<HbLabel *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_LABEL));
    mSubjectLabel->setPlainText(hbTrId("txt_mail_editor_subject"));
    mSubjectEdit = qobject_cast<NmHtmlLineEdit *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_SUBJECT_EDIT));

    // Add attachment list
    NmAttachmentListWidget *attachmentList = qobject_cast<NmAttachmentListWidget *>
        (mDocumentLoader->findWidget(NMUI_EDITOR_ATTACHMENT_LIST));
    // Create attachment list handling object
    mAttachmentList = new NmAttachmentList(attachmentList);

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
            this, SLOT(sendHeaderHeightChanged()));
    connect(mCcField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendHeaderHeightChanged()));
    connect(mBccField, SIGNAL(textChanged(const QString &)),
            this, SLOT(sendHeaderHeightChanged()));
    connect(mSubjectEdit, SIGNAL(contentsChanged()), this, SLOT(sendHeaderHeightChanged()));

    // Signal for handling the recipient group box expanding/collapsing
    connect(mGroupBoxRecipient, SIGNAL(toggled(bool)),
            this, SLOT(groupBoxExpandCollapse()));

    // Signals for handling the attachment list
    connect(mAttachmentList->listWidget(), SIGNAL(longPressed(int, QPointF)),
            this, SLOT(attachmentSelected(int, QPointF)));
    connect(mAttachmentList ,SIGNAL(attachmentListLayoutChanged()),
            this, SLOT(sendHeaderHeightChanged()));
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
    qreal subjectHeight = mSubjectEdit->geometry().height() + Margin;

    // When called first time, height is wrongly 'Margin'
    if (toHeight == Margin) {
        toHeight = FieldHeightWhenSecondaryFont;
        subjectHeight = FieldHeightWhenSecondaryFont;
    }

    // Recipient GroupBox
    qreal recipientGroupBoxHeight = GroupBoxTitleHeight;
    if (!mGroupBoxRecipient->isCollapsed()) {
        recipientGroupBoxHeight +=
            mGroupBoxRecipientContent->geometry().height() + HeaderAreaMarginsTotal;
    }

    qreal attHeight = 0;
    if (mAttachmentList && mAttachmentList->count() > 0) {
        attHeight = mAttachmentList->listWidget()->geometry().height();
    }

    return (int)(toHeight + recipientGroupBoxHeight + subjectHeight + attHeight);
}

/*!
    Send signal to inform that one of the recipient fields height has been changed.
 */
void NmEditorHeader::sendHeaderHeightChanged()
{
    // Adjust field heights
    mToField->editor()->setPreferredHeight(
        mToField->editor()->document()->size().height() + Margin);
    mCcField->editor()->setPreferredHeight(
        mCcField->editor()->document()->size().height() + Margin);
    mBccField->editor()->setPreferredHeight(
        mBccField->editor()->document()->size().height() + Margin);
    mSubjectEdit->setPreferredHeight(
        mSubjectEdit->document()->size().height() + Margin);

    int hHeight = headerHeight();
    if (mHeaderHeight != hHeight) {
        mHeaderHeight = hHeight;
        emit headerHeightChanged(mHeaderHeight);
    }
}

/*!
    This slot is called when group box state is changed. Timer is used to give some time
    for layout system so that header hight can be recalculated correctly
 */
void NmEditorHeader::groupBoxExpandCollapse()
{
    QTimer::singleShot(nmLayoutSystemWaitTimer, this, SLOT(sendHeaderHeightChanged()));
}

/*!
    Return pointer to to field
 */
NmRecipientLineEdit* NmEditorHeader::toField() const
{
    return mToField->editor();
}

/*!
    Return pointer to cc field
 */
NmRecipientLineEdit* NmEditorHeader::ccField() const
{
    return mCcField->editor();
}

/*!
    Return pointer to bcc field
 */
NmRecipientLineEdit* NmEditorHeader::bccField() const
{
    return mBccField->editor();
}

/*!
    Return pointer to subject field
 */
NmHtmlLineEdit* NmEditorHeader::subjectField() const
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
    This function create content widget for recipient group box.
    When AD offers groupBox content widget handling. This function
    need to be changed to use AD/docml
 */
HbWidget* NmEditorHeader::createRecipientGroupBoxContentWidget()
{
    mGroupBoxRecipientContent = new HbWidget();

    // Create layout for the widget
    mGbVerticalLayout = new QGraphicsLinearLayout(Qt::Vertical,mGroupBoxRecipientContent);
    mCcFieldLayout = new QGraphicsLinearLayout(Qt::Horizontal, mGbVerticalLayout);
    mBccFieldLayout = new QGraphicsLinearLayout(Qt::Horizontal, mGbVerticalLayout);

    // Add Cc: field into widget
    mCcField = new NmRecipientField(hbTrId("txt_mail_editor_cc"));
    if (mCcField) {
        mCcField->setObjectName("lineEditCcField");
        mGbVerticalLayout->insertItem(EEditorCcLine, mCcField );
    }

    // Add Bcc: field into widget
    mBccField = new NmRecipientField(hbTrId("txt_mail_editor_bcc"));
    if (mBccField){
        mBccField->setObjectName("lineEditBccField");
        mGbVerticalLayout->insertItem(EEditorBccLine, mBccField);
    }
    mGbVerticalLayout->setContentsMargins(0,0,0,0);

    mGroupBoxRecipientContent->setLayout(mGbVerticalLayout);
    return mGroupBoxRecipientContent;
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
    // Done with delayed timer so that layout system has finished before.
    QTimer::singleShot(nmLayoutSystemWaitTimer, this, SLOT(sendHeaderHeightChanged()));
    // This second call will set new position for body if subject field height has been changed.
    QTimer::singleShot(nmLayoutSystemWaitTimer * 2, this, SLOT(sendHeaderHeightChanged()));
}

/*!
 * \brief Sets the groupbox to be expanded or collapsed. 
 */
void NmEditorHeader::setGroupBoxCollapsed( bool collapsed )
{
    mGroupBoxRecipient->setCollapsed( collapsed );
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
   Remove attachment from the list. This function is used when attachment adding has failed
   and attachment id is not known.
 */
void NmEditorHeader::removeAttachment(const QString &fileName)
{
    mAttachmentList->removeAttachment(fileName);
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
   Slot attachment selected from attachment list by longtap.
 */
void NmEditorHeader::attachmentSelected(int arrayIndex, QPointF point)
{
    Q_UNUSED(point);
    // Remove selected attachment
    emit attachmentRemoved(mAttachmentList->nmIdByIndex(arrayIndex));
    mAttachmentList->removeAttachment(arrayIndex);
}
    
