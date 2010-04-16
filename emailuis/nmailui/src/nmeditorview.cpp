/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Message editor view
*
*/
#include "nmuiheaders.h"

// Layout file and view
static const char *NMUI_EDITOR_VIEW_XML = ":/docml/nmeditorview.docml";
static const char *NMUI_EDITOR_VIEW= "editorview";
static const char *NMUI_EDITOR_SCROLL_AREA = "scrollArea";
static const char *NMUI_EDITOR_SCROLL_AREA_CONTENTS = "scrollAreaContents";

static const int nmOrientationTimer=100;

static const QString Delimiter("; ");
#define IMAGE_FETCHER_INTERFACE "Image"
#define FETCHER_OPERATION "fetch(QVariantMap,QVariant)"

/*!
	\class NmEditorView
	\brief Mail editor view
*/

/*!
    Constructor
*/
NmEditorView::NmEditorView(
    NmApplication &application,
    NmUiStartParam* startParam,
    NmUiEngine &uiEngine,
    QGraphicsItem *parent)
: NmBaseView(startParam, parent),
mApplication(application),
mUiEngine(uiEngine),
mDocumentLoader(NULL),
mScrollArea(NULL),
mEditWidget(NULL),
mHeaderWidget(NULL),
mMessage(NULL),
mContentWidget(NULL),
mAttachContextMenu(NULL),
mMessageCreationOperation(NULL),
mAddAttachmentOperation(NULL),
mRemoveAttachmentOperation(NULL),
mCheckOutboxOperation(NULL)
{
    mDocumentLoader	= new HbDocumentLoader();
    // Set object name
    setObjectName("NmEditorView");
    // Load view layout
    loadViewLayout();
    // Set mailbox name to title pane
    setMailboxName();
    // Set message data
    setMessageData();
}

/*!
    Destructor
*/
NmEditorView::~NmEditorView()
{
    delete mMessageCreationOperation;
    delete mCheckOutboxOperation;

    delete mMessage;

    mWidgetList.clear();
    delete mDocumentLoader;

    delete mContentWidget;

    delete mPrioritySubMenu;
    delete mAttachContextMenu;
}

/*!
    View layout loading from XML
*/
void NmEditorView::loadViewLayout()
{
    mPrioritySubMenu = NULL;

    // Use document loader to load the view
    bool ok = false;
    mWidgetList = mDocumentLoader->load(NMUI_EDITOR_VIEW_XML, &ok);

    if (ok == true && mWidgetList.count()) {
        // Set view
        QGraphicsWidget *view = mDocumentLoader->findWidget(NMUI_EDITOR_VIEW);
        if (view){
            setWidget(view);
        }

        mScrollArea = qobject_cast<NmBaseViewScrollArea *>
            (mDocumentLoader->findObject(NMUI_EDITOR_SCROLL_AREA));
        mScrollAreaContents = qobject_cast<HbWidget *>
             (mDocumentLoader->findObject(NMUI_EDITOR_SCROLL_AREA_CONTENTS));

        mContentWidget = new NmEditorContent(mScrollArea, this, mDocumentLoader);
        mEditWidget = mContentWidget->editor();
        mHeaderWidget = mContentWidget->header();

        // Set default color for user - entered text if editor is in re/reAll/fw mode
        NmUiEditorStartMode mode = mStartParam->editorStartMode();
        if (mode == NmUiEditorReply || mode == NmUiEditorReplyAll || mode == NmUiEditorForward) {
        mEditWidget->setCustomTextColor(true, Qt::blue);
        }
    }

    // Connect signals from background scroll area
    connect(mScrollArea, SIGNAL(handleMousePressEvent(QGraphicsSceneMouseEvent*)),
            this, SLOT(sendMousePressEventToScroll(QGraphicsSceneMouseEvent*)));
    connect(mScrollArea, SIGNAL(handleMouseReleaseEvent(QGraphicsSceneMouseEvent*)),
            this, SLOT(sendMouseReleaseEventToScroll(QGraphicsSceneMouseEvent*)));
    connect(mScrollArea, SIGNAL(handleMouseMoveEvent(QGraphicsSceneMouseEvent*)),
            this, SLOT(sendMouseMoveEventToScroll(QGraphicsSceneMouseEvent*)));

    connect(mScrollArea, SIGNAL(handleLongPressGesture(const QPointF &)),
                this, SLOT(sendLongPressGesture(const QPointF &)));

    // Connect options menu about to show to create options menu function
    // Menu needs to be create "just-in-time"
    connect(menu(), SIGNAL(aboutToShow()), this, SLOT(createOptionsMenu()));
    NmAction *dummy = new NmAction(0);
    menu()->addAction(dummy);

    initializeVKB();
    connect(mContentWidget->header(), SIGNAL(recipientFieldsHaveContent(bool)),
            this, SLOT(setButtonsDimming(bool)) );

    // call the createToolBar on load view layout
    createToolBar();

    // Set dimensions
    adjustViewDimensions();
	
    // Connect to observe orientation change events
    connect(mApplication.mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(orientationChanged(Qt::Orientation)));
    // Signal for handling the attachment list selection
    connect(mHeaderWidget, SIGNAL(attachmentRemoved(const NmId)),
            this, SLOT(removeAttachment(const NmId)));
}

/*!
    Reload view contents with new start parameters
    Typically when view is already open and external view activation occurs
    for this same view
*/
void NmEditorView::reloadViewContents(NmUiStartParam* startParam)
{
    // Check start parameter validity.
    if (startParam&&startParam->viewId()==NmUiViewMessageEditor) {
        // Delete existing start parameter data
        delete mStartParam;
        mStartParam=NULL;
        // Store new start parameter data
        mStartParam=startParam;
        // Store existing edited message to drafts and reload
        // editor with new start parameters.
        // ..
        // Reload editor with new message data
        setMessageData();
    }
    else {
        NMLOG("nmailui: Invalid editor start parameter");
        // Unused start parameter needs to be deleted
        delete startParam;
        startParam = NULL;
    }
}

/*!
   Screen orientation changed. Editor view needs to be scaled when
   landscape <-> portrait switch occurs because text needs to
   be wrapped again.
*/
void NmEditorView::orientationChanged(Qt::Orientation orientation)
{
    Q_UNUSED(orientation);
    // Adjust content height
    QTimer::singleShot(nmOrientationTimer, this, SLOT(adjustViewDimensions()));
}

/*!
    Set new dimensions after orientation change.
*/
void NmEditorView::adjustViewDimensions()
{
    if (mScrollAreaContents) {
        const QSize reso = mApplication.screenSize();
        mScrollAreaContents->setMinimumWidth(reso.width());
        mScrollAreaContents->setMaximumWidth(reso.width());
    }
}

/*!
    View id
*/
NmUiViewId NmEditorView::nmailViewId() const
{
    return NmUiViewMessageEditor;
}

/*!
    ScrollArea contents
*/
HbWidget* NmEditorView::scrollAreaContents()
{
    return mScrollAreaContents;
}

/*
   Query user if we want to exit the editor
 */
bool NmEditorView::okToExitView()
{
    bool okToExit = true;

    // show the query if the message has not been sent
    if (mMessage && mContentWidget->header()) {
        // see if editor has any content
        NmEditorHeader *header = mContentWidget->header();

        int toTextLength = 0;
        if (header->toField()) {
            toTextLength = header->toField()->text().length();
        }
        
        int ccTextLength = 0;
        if (header->ccField()) {
            ccTextLength = header->ccField()->text().length();
        }

        int bccTextLength = 0;
        if (header->bccField()) {
            bccTextLength = header->bccField()->text().length();
        }
        
        int subjectLength = 0;
        if (header->subjectField()) {
            subjectLength = header->subjectField()->text().length();
        }
        
        okToExit = (toTextLength == 0 && ccTextLength == 0 && bccTextLength == 0 && 
            subjectLength == 0 && mContentWidget->editor()->document()->isEmpty());

        // content exists, verify exit from user
        if (!okToExit) {
            HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
            messageBox->setText(hbTrId("txt_mail_dialog_delete_message"));
            messageBox->setTimeout(HbMessageBox::NoTimeout);
        
            // Read user selection
            HbAction *action = messageBox->exec(); 
            okToExit = (action == messageBox->primaryAction());
        }
    }

    return okToExit;
}

/*!
    About to exit view. Application calls this function when user has
    pressed back key and editor needs to delete the draft message. This is
    called when "auto-exiting" after a successful mail sending.
*/
void NmEditorView::aboutToExitView()
{
    // These operations need to be stopped before message can be deleted
    delete mAddAttachmentOperation;
    delete mRemoveAttachmentOperation;

    if (mMessage) { // this is NULL if sending is started
        // Delete message from drafts
        NmId mailboxId = mMessage->mailboxId();
        NmId folderId = mMessage->parentId();
        NmId msgId = mMessage->envelope().id();
        mUiEngine.removeMessage(mailboxId, folderId, msgId);
    }
}

/*!
    Find message data based on start parameters.  Method is called
    when editor is started. If message data is found it means that
    operation is forward or reply message.
 */
void NmEditorView::setMessageData()
{
    // Check the outbox.
    delete mCheckOutboxOperation;
	mCheckOutboxOperation = NULL;
	
	mCheckOutboxOperation = mUiEngine.checkOutbox(mStartParam->mailboxId());
    
    if (mCheckOutboxOperation) {
        connect(mCheckOutboxOperation, SIGNAL(operationCompleted(int)),
                this, SLOT(outboxChecked(int)));
    }
    else {
        startMessageCreation( mStartParam->editorStartMode() );
    }
}

/*!
 */
void NmEditorView::startMessageCreation(NmUiEditorStartMode startMode)
{
    NmId mailboxId = mStartParam->mailboxId();
    NmId folderId = mStartParam->folderId();
    NmId msgId = mStartParam->messageId();
    
    delete mMessageCreationOperation;
	mMessageCreationOperation = NULL;
	
	// original message is now fetched so start message creation
    if (startMode == NmUiEditorForward) {
        mMessageCreationOperation = mUiEngine.createForwardMessage(mailboxId, msgId);
    }
    else if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll) {
        mMessageCreationOperation = mUiEngine.createReplyMessage(mailboxId, 
            msgId, 
            startMode == NmUiEditorReplyAll);
    }
    else {
        mMessageCreationOperation = mUiEngine.createNewMessage(mailboxId);
    }
    
    // operation continues in messageCreated() once it finishes ok
    if (mMessageCreationOperation) {
        connect(mMessageCreationOperation,
                SIGNAL(operationCompleted(int)),
                this,
                SLOT(messageCreated(int)));
    }    
}

/*!
    Starting the message sending is handled here.
 */
void NmEditorView::startSending()
{
    // The message contents should be verified
    updateMessageWithEditorContents();
    
    // verify addresses before sending
    QList<NmAddress> invalidAddresses;
    NmUtilities::getRecipientsFromMessage(*mMessage, invalidAddresses, NmUtilities::InvalidAddress);
    
    bool okToSend = true;
    if (invalidAddresses.count() > 0) {
        
        // invalid addresses found, verify send from user
        HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeQuestion);
        QString noteText = hbTrId("txt_mail_dialog_invalid_mail_address_send");
        // set the first failing address to the note
        noteText = noteText.arg(invalidAddresses.at(0).address());
        messageBox->setText(noteText);
        messageBox->setTimeout(HbMessageBox::NoTimeout);

        // Read user selection
        HbAction *action = messageBox->exec();
        okToSend = (action == messageBox->primaryAction());
    }
    
    if (okToSend) {
        // ownership of mMessage changes
        mUiEngine.sendMessage(mMessage);
        mMessage = NULL;
        // sending animation should be shown here, then exit
        QTimer::singleShot(1000, &mApplication, SLOT(popView()));
    }
}

/*!
    This is signalled by mMessageCreationOperation when message is created.
 */
void NmEditorView::messageCreated(int result)
{
    delete mMessage;
    mMessage = NULL;

    if (result == NmNoError && mMessageCreationOperation) {
        NmUiEditorStartMode startMode = mStartParam->editorStartMode();
        
        // get message "again" from engine to update the message contents 
        mMessage = mUiEngine.message(
            mStartParam->mailboxId(), 
            mStartParam->folderId(), 
            mMessageCreationOperation->getMessageId());
        
        fillEditorWithMessageContents();       
    }
}

/*!
   Updates the message with the editor contents.
 */
void NmEditorView::updateMessageWithEditorContents()
{
    if (mMessage) {
        if (mContentWidget && mContentWidget->editor()) {
            NmMessagePart* bodyPart = mMessage->htmlBodyPart();
            if (bodyPart) {
                bodyPart->setTextContent(mContentWidget->editor()->toHtml(), NmContentTypeTextHtml);
            }
            bodyPart = mMessage->plainTextBodyPart();
            if (bodyPart) {
                bodyPart->setTextContent(mContentWidget->editor()->toPlainText(), NmContentTypeTextPlain);
            }
        }
        if (mContentWidget && mContentWidget->header() ) {
            if (mContentWidget->header()->subjectField()) {
                mMessage->envelope().setSubject(
                    mContentWidget->header()->subjectField()->text());
            }
            if (mContentWidget->header()->toField()) {
                QString toFieldText =
                    mContentWidget->header()->toField()->text();

                // This verification of zero length string isn't needed
                // after list of addresses
                if (toFieldText.length() > 0) {
                    mMessage->envelope().setToRecipients(mContentWidget->header()->toField()->emailAddressList());  
                }
            }
            if (mContentWidget->header()->ccField()) {
                QString ccFieldText =
                    mContentWidget->header()->ccField()->text();

                if (ccFieldText.length() > 0) {
                    mMessage->envelope().setCcRecipients(mContentWidget->header()->ccField()->emailAddressList());      
                }
            }
            if (mContentWidget->header()->bccField()) {
                QString bccFieldText =
                    mContentWidget->header()->bccField()->text();

                if (bccFieldText.length() > 0) {
                    mMessage->envelope().setBccRecipients(mContentWidget->header()->bccField()->emailAddressList());  
                }
            }
        }
    }
}


/*!
    Updates the message with the editor contents. Called only once when the
    editor is launched.
 */
void NmEditorView::fillEditorWithMessageContents()
{
    if (!mMessage || !mContentWidget) {
        return;
    }

    NmMessageEnvelope messageEnvelope(mMessage->envelope());
    NmUiEditorStartMode editorStartMode = NmUiEditorCreateNew;
    bool useStartParam(false);

    if (mStartParam) {
        editorStartMode = mStartParam->editorStartMode();

        if (editorStartMode == NmUiEditorMailto) {
            // Retrieve the message header data e.g. recipients from mStartParam.
            useStartParam = true;        
        }
    }
    
    // Set recipients (to, cc and bcc).
    QString toAddressesString;
    QString ccAddressesString;
    QString bccAddressesString;

    if (useStartParam) {
        toAddressesString = addressListToString(mStartParam->mailtoAddressList());
        ccAddressesString = addressListToString(mStartParam->ccAddressList());
        bccAddressesString = addressListToString(mStartParam->bccAddressList());
    }
    else {
        toAddressesString = addressListToString(messageEnvelope.toRecipients());
        ccAddressesString = addressListToString(messageEnvelope.ccRecipients());
        bccAddressesString = addressListToString(messageEnvelope.bccRecipients());
    }

    mContentWidget->header()->toField()->setPlainText(toAddressesString);
    mContentWidget->header()->ccField()->setPlainText(ccAddressesString);
    mContentWidget->header()->bccField()->setPlainText(bccAddressesString);

    if (ccAddressesString.length() || bccAddressesString.length()) {
        // Since cc or/and bcc recipients exist, expand the group box to display
        // the addresses by expanding the group box.
        mContentWidget->header()->setGroupBoxCollapsed(false);
    }

    // Set subject.
    if (useStartParam) {
        QString *subject = mStartParam->subject();

        if (subject) {
            mContentWidget->header()->subjectField()->setPlainText(*subject);
        }
    }
    else {
        // If a message is taken from the outbox, no subject formatting is done.
        NmId notUsed(0);

        if (mCheckOutboxOperation &&
            mCheckOutboxOperation->getMessageId(notUsed)) {
            editorStartMode = NmUiEditorCreateNew;
        }

        // Construct the subject field.
        mContentWidget->header()->subjectField()->setPlainText(
            addSubjectPrefix(editorStartMode, messageEnvelope.subject()));
    }

    // Set priority.
    mHeaderWidget->setPriority(messageEnvelope.priority());
    
    // Set the message body.
    // Fetch plain text part form message store.
    NmMessagePart *plainPart = mMessage->plainTextBodyPart();

    if (plainPart) {
        mUiEngine.contentToMessagePart(mMessage->mailboxId(),
                                       mMessage->parentId(),
                                       mMessage->envelope().id(),
                                       *plainPart);
    }

    // Fetch html part form message store.
    NmMessagePart *htmlPart = mMessage->htmlBodyPart();

    if (htmlPart) {
        mUiEngine.contentToMessagePart(mMessage->mailboxId(),
                                       mMessage->parentId(),
                                       mMessage->envelope().id(),
                                       *htmlPart);
    }

    // Fetch attachment.html part form message store if such exists.
    QList<NmMessagePart*> parts;
    mMessage->attachmentList(parts);
    NmMessagePart *attachmentHtml = NULL;

    foreach(NmMessagePart *part, parts) {
        if (part->contentDescription().startsWith(NmContentDescrAttachmentHtml)) {
            attachmentHtml = part;
        }
    }

    if (attachmentHtml) {
        mUiEngine.contentToMessagePart(mMessage->mailboxId(),
                                       mMessage->parentId(),
                                       mMessage->envelope().id(),
                                       *attachmentHtml);
    }

    mContentWidget->setMessageData(mMessage);
    
    // Get list of attachments from the message and set those into UI attachment list
    QList<NmMessagePart*> attachments;
    mMessage->attachmentList(attachments);

    for (int i=0; i<attachments.count(); i++) {
        mHeaderWidget->addAttachment(
            attachments[i]->attachmentName(),
            QString::number(attachments[i]->size()),
            attachments[i]->id());
    }

    if (mStartParam) {
        // Attach passed files to the message.
        QStringList *fileList = mStartParam->attachmentList();

        if (fileList) {
            addAttachments(*fileList);
        }
    }

    // TODO Switch the following arbitrary (magic number) timeout to a
    // meaningful constant, please!
    QTimer::singleShot(200, mHeaderWidget, SLOT(sendHeaderHeightChanged()));
}


/*!
    createToolBar. Function asks menu commands from extension
    to be added to toolbar owned by the HbView.
*/
void NmEditorView::createToolBar()
{
    HbToolBar *tb = toolBar();
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (tb && &extMngr && mStartParam) {
        tb->clearActions();
        NmActionRequest request(this, NmActionToolbar, NmActionContextViewEditor,
                NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
        QList<NmAction *> list;
        extMngr.getActions(request, list);
        for (int i = 0; i < list.count(); i++) {
            tb->addAction(list[i]);
            // If action has NmSendable condition, it is shown only when send action
            // is available, i.e. when at least one recipient field has data.
            if( list[i]->availabilityCondition() == NmAction::NmSendable ) {
                list[i]->setEnabled(false);
            }
        }
    }
}

/*!
    createOptionsMenu. Functions asks menu commands from extension
    to be added to options menu.
*/
void NmEditorView::createOptionsMenu()
{
    if (!mPrioritySubMenu) {
        mPrioritySubMenu = new HbMenu();
    }
    mPrioritySubMenu->clearActions();
    menu()->clearActions();
    NmActionRequest request(this, NmActionOptionsMenu, NmActionContextViewEditor,
            NmActionContextDataMessage, mStartParam->mailboxId(), mStartParam->folderId(),
            mStartParam->messageId());
    NmUiExtensionManager &extMngr = mApplication.extManager();
    QList<NmAction*> list;
    extMngr.getActions(request, list);
    for (int i = 0; i < list.count(); i++) {
        mPrioritySubMenu->addAction(list[i]);
    }
    mPrioritySubMenu->setObjectName("editorPrioritySubMenu");
    mPrioritySubMenu->setTitle(hbTrId("txt_mail_opt_add_priority"));
    menu()->addMenu(mPrioritySubMenu);
}

/*!
    handleActionCommand. From NmActionObserver, extension manager calls this
    call to handle menu command in the UI.
*/
void NmEditorView::handleActionCommand(NmActionResponse &actionResponse)
{
    NmActionResponseCommand responseCommand = actionResponse.responseCommand();
    
    // Handle options menu
    if (actionResponse.menuType() == NmActionOptionsMenu) {
        setPriority(responseCommand);
    }
    else if (actionResponse.menuType() == NmActionToolbar) {
        switch (responseCommand) {
            case NmActionResponseCommandSendMail: {
                startSending();
                break;
            }
            case NmActionResponseCommandAttach : {
                // Do nothing if previous addAttachment operation is still ongoing.
                if(!mAddAttachmentOperation || !mAddAttachmentOperation->isRunning()) {
                    //will be replaced by toolbar extension                                
                    if (!mAttachContextMenu) {
                        QList<NmAction *> actionList;
                        NmAction* actionPhoto = new NmAction(0);
                        actionPhoto->setText(QObject::tr("Photo", "txt_nmailui_photo_attach"));
                        actionList.append(actionPhoto);
                        connect(actionPhoto, SIGNAL(triggered()), this, SLOT(attachImage()));
                                
                        mAttachContextMenu = new HbMenu();
                        mAttachContextMenu->clearActions();

                        for (int i=0;i<actionList.count();i++) {
                            mAttachContextMenu->addAction(actionList[i]);
                        }
                    }
                
                    QPointF menuPos(qreal(20),qreal(520));
                    mAttachContextMenu->exec(menuPos);
                }
                break;
            }
            default:
                break;
        }
    }
}

/*!
    This function converts background scroll area coordinate point into
    body text editor coordinate point.
 */
QPointF NmEditorView::viewCoordinateToEditCoordinate(QPointF orgPoint)
{
    QPointF contentWidgetPos = mScrollAreaContents->pos();
    qreal y = orgPoint.y() - mHeaderWidget->headerHeight();
    y -= contentWidgetPos.y();
    qreal x = orgPoint.x() - contentWidgetPos.x();
    return QPointF(x, y);
}

/*!
   Send mouse press event to body edit widget
*/
void NmEditorView::sendMousePressEventToScroll(QGraphicsSceneMouseEvent *event)
{
    if (event && mEditWidget && mHeaderWidget) {
        event->setPos(viewCoordinateToEditCoordinate(event->pos()));
        event->setAccepted(true);
        mEditWidget->sendMousePressEvent(event);
    }
}

/*!
   Send mouse release event to body edit widget
*/
void NmEditorView::sendMouseReleaseEventToScroll(QGraphicsSceneMouseEvent *event)
{
    if (event&& mEditWidget && mHeaderWidget) {
        event->setPos(viewCoordinateToEditCoordinate(event->pos()));
        event->setAccepted(true);
        mEditWidget->sendMouseReleaseEvent(event);
    }
}

/*!
   Send mouse move event to body edit widget
*/
void NmEditorView::sendMouseMoveEventToScroll(QGraphicsSceneMouseEvent *event)
{
    if (event&& mEditWidget && mHeaderWidget) {
        event->setPos(viewCoordinateToEditCoordinate(event->pos()));
        event->setAccepted(true);
        mEditWidget->sendMouseMoveEvent(event);
    }
}

void NmEditorView::sendLongPressGesture(const QPointF &point)
{
    if (mEditWidget && mHeaderWidget) {
        QPointF scenePos = mEditWidget->scenePos();
        QPointF newPoint = QPointF(point.x()-scenePos.x(), point.y()-scenePos.y());
        if(mEditWidget->contains(newPoint)) {
            mEditWidget->sendLongPressEvent(point);
        }
    }
}


/*!
   Sets all toolbar and VKB buttons dimmed state. All actions that have the
   availability condition NmSendable set, will be enabled/disabled.
*/
void NmEditorView::setButtonsDimming(bool enabled)
{
    // Set the toolbar action states
    HbToolBar *tb = toolBar();
    if (tb) {
        QList<QAction *> toolbarList = tb->actions();
        int count = toolbarList.count();
        for (int i = 0; i < count; i++) {
            NmAction *action = static_cast<NmAction *>(toolbarList[i]);
            if (action->availabilityCondition() == NmAction::NmSendable) {
                action->setEnabled(enabled);
            }
        }

        // Set the VKB action states
        // All editors of the view share the same action, so it is enough to set
        // this only to one of them.
        HbEditorInterface editorInterface(mContentWidget->editor());
        QList<HbAction *> vkbList = editorInterface.actions();
        count = vkbList.count();
        for (int i = 0; i < count; i++) {
            NmAction *action = static_cast<NmAction *>(vkbList[i]);
            if (action->availabilityCondition() == NmAction::NmSendable) {
                action->setEnabled(enabled);
            }
        }
    }
}

/*!
    Initialize the Virtual Keyboard to show the "Send" button
    for all editors of the view.
*/
void NmEditorView::initializeVKB()
{
    NmActionRequest request(this, NmActionVKB, NmActionContextViewEditor,
         NmActionContextDataNone, mStartParam->mailboxId(), mStartParam->folderId() );
    NmUiExtensionManager &extMngr = mApplication.extManager();
    if (&extMngr) {
        QList<NmAction *> list;
        extMngr.getActions(request, list);

        // VKB only supports one application key, but the responsibility of giving only one
        // action is left to the extension plugin. The rest are still attached to the VKB, but
        // they are not shown (unless VKB starts supporting more than one).
        for (int i = 0; i < list.count(); i++) {
            if( list[i]->availabilityCondition() == NmAction::NmSendable ) {
                list[i]->setEnabled(false);
            }
            list[i]->setIcon(NmIcons::getIcon(NmIcons::NmIconSend));

            // Link VKB to the action. This must be done to all
            // editors that show the button in VKB.
            HbEditorInterface editorInterface(mContentWidget->editor());
            editorInterface.addAction(list[i]);
            HbEditorInterface toEditorInterface(mContentWidget->header()->toField());
            toEditorInterface.addAction(list[i]);
            HbEditorInterface ccEditorInterface(mContentWidget->header()->ccField());
            ccEditorInterface.addAction(list[i]);
            HbEditorInterface bccEditorInterface(mContentWidget->header()->bccField());
            bccEditorInterface.addAction(list[i]);
            HbEditorInterface subjectEditorInterface(mContentWidget->header()->subjectField());
            subjectEditorInterface.addAction(list[i]);
        }
    }
}

/*!
    Set mailbox name to title
*/
void NmEditorView::setMailboxName()
{
    if (mStartParam){
        NmMailboxMetaData *meta = mUiEngine.mailboxById(mStartParam->mailboxId());
        if (meta){
            setTitle(meta->name());
        }
    }
}

/*!
   Adds a prefix to the subject for reply or forward. 
   Strips other occurrences of the prefix from the beginning.
 */
QString NmEditorView::addSubjectPrefix( NmUiEditorStartMode startMode, const QString &subject )
{
    QString newSubject(subject.trimmed());
    
    if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll || 
        startMode == NmUiEditorForward) {
        QString rePrefix(QObject::tr("Re:", "txt_nmailui_reply_subject_prefix"));
        QString fwPrefix(QObject::tr("Fw:", "txt_nmailui_forward_subject_prefix"));
        
        // strip extra prefixes from beginning
        int rePrefixLength(rePrefix.length());
        int fwPrefixLength(fwPrefix.length());
        
        bool startswithRe(newSubject.startsWith(rePrefix, Qt::CaseInsensitive));
        bool startswithFw(newSubject.startsWith(fwPrefix, Qt::CaseInsensitive));
        
        while (startswithRe || startswithFw) {            
            if (startswithRe) {
                newSubject.remove(0,rePrefixLength);
                newSubject = newSubject.trimmed();
            }
            else if (startswithFw) {
                newSubject.remove(0,fwPrefixLength);
                newSubject = newSubject.trimmed();
            }
            startswithRe = newSubject.startsWith(rePrefix, Qt::CaseInsensitive);
            startswithFw = newSubject.startsWith(fwPrefix, Qt::CaseInsensitive);
        }
        
        if (startMode == NmUiEditorReply || startMode == NmUiEditorReplyAll) {
            newSubject = rePrefix + " " + newSubject;
        }
        else if (startMode == NmUiEditorForward) {
            newSubject = fwPrefix + " " + newSubject;
        }
    }
    
    return newSubject;
}

#ifdef Q_OS_SYMBIAN

void NmEditorView::attachImage()
{      
    XQAiwRequest *request;
    XQApplicationManager mAppmgr;
    request = mAppmgr.create(IMAGE_FETCHER_INTERFACE, FETCHER_OPERATION, true);
        
    if (request) {
        connect(request, SIGNAL(requestOk(const QVariant&)), this, SLOT(onAttachmentReqCompleted(const QVariant&)));
    }
    else {
        //create request failed
        NMLOG("appmgr: create request failed");
        return;
    }               

    if (!(request)->send()) {
        //sending request failed
        NMLOG("appmgr: send request failed");
    }
    delete request;
}

/*!
    This slot is called when 'attachment picker' request has been performed succesfully
    Parameter 'value' contains file currently one file name but later list of the files. 
*/
void NmEditorView::onAttachmentReqCompleted(const QVariant &value)
{
    if (!value.isNull()) {
        addAttachments(value.toStringList());
    }
}

#endif

/*!
    Add list of attachments
*/
void NmEditorView::addAttachments(const QStringList& fileNames) 
{
    NMLOG("NmEditorView::addAttachments");

    // Add attachment name into UI
    foreach (QString fileName, fileNames)  {
        // At this phase attachment size and nmid are not known
        mHeaderWidget->addAttachment(fileName, QString("0"), NmId(0));
        NMLOG(fileName);
    }

    // Start operation to attach file or list of files into mail message
    // This will also copy files into message store
    // Delete previous operation
    if (mAddAttachmentOperation) {
        if (!mAddAttachmentOperation->isRunning()) {
            delete mAddAttachmentOperation;
            mAddAttachmentOperation = NULL;
        }
    }
    if (!mAddAttachmentOperation) {
        mAddAttachmentOperation = mUiEngine.addAttachments(*mMessage, fileNames);

        if (mAddAttachmentOperation) {
            // Signal to inform completion of one attachment
            connect(mAddAttachmentOperation,
                    SIGNAL(operationPartCompleted(const QString &, const NmId &, int)),
                    this,
                    SLOT(oneAttachmentAdded(const QString &, const NmId &, int)));
        
            // Signal to inform the completion of the whole operation
            connect(mAddAttachmentOperation,
                    SIGNAL(operationCompleted(int)),
                    this,
                    SLOT(allAttachmentsAdded(int)));
        }
    }
}

/*!
    This slot is called when attachment has been deleted from UI
*/
void NmEditorView::removeAttachment(const NmId attachmentPartId)
{
    // Delete previous operation
    if (mRemoveAttachmentOperation) {
        if (!mRemoveAttachmentOperation->isRunning()) {
            delete mRemoveAttachmentOperation;
            mRemoveAttachmentOperation = NULL;
        }
    }
    if (!mRemoveAttachmentOperation) {
        mRemoveAttachmentOperation = mUiEngine.removeAttachment(*mMessage, attachmentPartId);
    }
}

/*!
    This is signalled by mAddAttachmentOperation when the operation is
    completed for one attachment.
 */
void NmEditorView::oneAttachmentAdded(const QString &fileName, const NmId &msgPartId, int result)
{
    if (result == NmNoError) {
        // Get file size from the message when it works
        mHeaderWidget->setAttachmentParameters(fileName, msgPartId, QString("0"), result);
    }
    else {
        // Attachment adding failed. Show an error note and remove from UI attachment list.
        NMLOG(QString("nmailui: attachment adding into message failed: %1").arg(fileName));
        mHeaderWidget->removeAttachment(fileName);
    }
}

/*!
    This is signalled by mAddAttachmentOperation when the operation is
    completed totally.
 */
void NmEditorView::allAttachmentsAdded(int result)
{
    if (result != NmNoError) {
        HbMessageBox::warning(hbTrId("txt_mail_dialog_unable_to_add_attachment"));
    }
}

/*!
    This is signalled by mCheckOutboxOperation when the operation is complete.
 */
void NmEditorView::outboxChecked(int result)
{
    bool messageInOutbox = false;
    
    if (result == NmNoError && mCheckOutboxOperation) {

        NmId messageId;
        messageInOutbox = mCheckOutboxOperation->getMessageId(messageId);
        
        if (messageInOutbox) {
            delete mMessage;
            mMessage = NULL;

            mMessage = mUiEngine.message(
                mStartParam->mailboxId(), 
                mUiEngine.standardFolderId(
                    mStartParam->mailboxId(), NmFolderOutbox), 
                messageId);
            
            fillEditorWithMessageContents();
            
            if (mMessage) {
                HbMessageBox::warning(
                    hbTrId("txt_mail_dialog_sending failed").arg(
                        NmUtilities::truncate(
                            mMessage->envelope().subject(), 20)));
            }
        }
    }

    if (!messageInOutbox) {
        startMessageCreation(mStartParam->editorStartMode());
    }
}

/*!
   Sets priority for the message object that is being edited 
*/
void NmEditorView::setPriority(NmActionResponseCommand priority)
{
    mHeaderWidget->setPriority(priority);

    if (mMessage) {
        NmMessagePriority messagePriority = NmMessagePriorityNormal;
        
        if (priority == NmActionResponseCommandPriorityHigh) {
            messagePriority = NmMessagePriorityHigh;
        }
        else if (priority == NmActionResponseCommandPriorityLow) {
            messagePriority = NmMessagePriorityLow;
        }
        mMessage->envelope().setPriority(messagePriority);
    }
}


/*!
    Extracts the addresses from the given list into a string separated with a
    delimiter.

    \param list The list containing the addresses.
    \return String containing the addresses.
*/
QString NmEditorView::addressListToString(const QList<NmAddress*> &list) const
{
    QString addressesString;
    QList<NmAddress*>::const_iterator i = list.constBegin();
    
    while (i != list.constEnd() && *i) {
        if (i > list.constBegin()) {
            // Add the delimiter.
            addressesString += Delimiter;
        }

        addressesString += (*i)->address();
        ++i;
    }

    return addressesString;
}


/*!
    Extracts the addresses from the given list into a string separated with a
    delimiter.

    \param list The list containing the addresses.
    \return String containing the addresses.
*/
QString NmEditorView::addressListToString(const QList<NmAddress> &list) const
{
    QString addressesString;
    QList<NmAddress>::const_iterator i = list.constBegin();
    
    while (i != list.constEnd()) {
        if (i > list.constBegin()) {
            // Add the delimiter.
            addressesString += Delimiter;
        }

        addressesString += (*i).address();
        ++i;
    }

    return addressesString;
}


// End of file.
