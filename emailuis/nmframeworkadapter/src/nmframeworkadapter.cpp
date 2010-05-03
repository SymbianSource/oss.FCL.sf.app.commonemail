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

#include "nmframeworkadapterheaders.h"

/*!
    \class NmFrameworkAdapter

    \brief The NmFrameworkAdapter works as an adapter between emailframework which is (legacy)
    Symbian code and emailuis which is done using QT.

    The NmFrameworkAdapter works as an adapter between emailframework which is (legacy)
    Symbian code and emailuis which is done using QT. Most functions in the adapter merely do d
    ata type conversions and forward function calls to emailframework implementation.
    Also events coming from emailframework are converted and emitted as signals.
 */

static const int nmListMessagesBlock = 100;
static const int nmMaxItemsInMessageList = 1000;


/*!
    Constructor
*/
NmFrameworkAdapter::NmFrameworkAdapter( ) : mFSfw(NULL)
{
    NMLOG("NmFrameworkAdapter::NmFrameworkAdapter() <---");
    // get s60email framework
    TRAP_IGNORE(mFSfw = CFSMailClient::NewL());
    if(mFSfw){
        TRAP_IGNORE(mFSfw->AddObserverL(*this));
        }
    NMLOG("NmFrameworkAdapter::NmFrameworkAdapter() --->");
}

/*!
    Destructor
*/
NmFrameworkAdapter::~NmFrameworkAdapter()
{
    if(mFSfw){
        mFSfw->RemoveObserver(*this);
        mFSfw->Close();
        }

    mFSfw = NULL;
}

/*!
    Add ids of all existing mailboxes to list given as reference.

    \param mailboxIdList The list to receive the mailbox ids.

    \return Error code.
 */
int NmFrameworkAdapter::listMailboxIds(QList<NmId>& mailboxIdList)
{
    QList<NmMailbox*> mailboxList;
    int ret = listMailboxes(mailboxList);

    if ( ret == NmNoError ) {
        QListIterator<NmMailbox*> iterator(mailboxList);
        while (iterator.hasNext()) {
            NmMailbox *box = iterator.next();
            mailboxIdList.append(box->id());
            delete box;
            box = NULL;
        }
    }
    return ret;
}

/*!
    Fill array given as reference with new mailbox objects, one for each existing mailbox.

    \param mailboxList The list to receive the mailbox object pointers, ownership of which is transferred to caller.

    \return Error code.
 */
int NmFrameworkAdapter::listMailboxes(QList<NmMailbox*>& mailboxList)
{
    // get list of mailboxes from all plugins
    TFSMailMsgId id;
    id.SetNullId();
    RPointerArray<CFSMailBox> mailBoxes;

    //if id.IsNullId(), mailboxes are listed from all plugins.
    //otherwise, only from the given one.
    TInt rcode = mFSfw->ListMailBoxes(id,mailBoxes);

    if ( rcode == NmNoError ) {
        // convert mailbox data to QT classes
        NmMailbox* box(0);
        for(TInt i=0;i<mailBoxes.Count();i++) {
            box = NULL;
            if (mailBoxes[i]) {
                box = mailBoxes[i]->GetNmMailbox();
                }
            if (box) {
                mailboxList.append(box);
                }
            }
        }
	mailBoxes.ResetAndDestroy();
    return rcode;
}

/*!
    Get a pointer to a new mailbox object for the mailbox identified by id.

    \param id Id of the mailbox.
    \param mailbox Pointer reference to receive the mailbox object, ownership of which is transferred to caller.

    \return Error code.
 */
int NmFrameworkAdapter::getMailboxById(const NmId& id, NmMailbox*& mailbox)
{
    const TFSMailMsgId mailMsgId(id.pluginId32(), id.id32());
    CFSMailBox *box(NULL);
    TRAPD(err, box = mFSfw->GetMailBoxByUidL(mailMsgId));
    if (err == KErrNone && box) {
        mailbox = box->GetNmMailbox();
        delete box;
        box = NULL;
    }
    return err;
}

/*!
    Delete the mailbox with the given id. Not implemented yet.

    \param id Id of the mailbox to be deleted.

    \return Error code.
 */
int NmFrameworkAdapter::deleteMailboxById(const NmId& /*id*/)
{
    return 0;
}

/*!
    Returns folder

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the requested folder
    \param message Pointer reference to receive a folder object, ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::getFolderById( const NmId& mailboxId, const NmId& folderId, NmFolder*& folder )
    {
    TRAPD(err, getFolderByIdL( mailboxId, folderId, folder ) );
    return err;
    }

/*!
    Leaving version of getFolderById function
 */
void NmFrameworkAdapter::getFolderByIdL( const NmId& mailboxId, const NmId& folderId, NmFolder*& folder )
    {
    CFSMailFolder* fsFolder(NULL);
	if (mFSfw)
		{
    	fsFolder = mFSfw->GetFolderByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId)); 
    	if (fsFolder)
        	{
        	folder = fsFolder->GetNmFolder();
        	delete fsFolder;
        	fsFolder = NULL;
        	}
    	else
    	    {
            User::Leave(KErrNotFound);
    	    }
		}
    }


/*!
    Returns message from the store together with whole message part structure

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message.
    \param message Pointer reference to receive a message object containing requested message,
     ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::getMessageById(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId,
    NmMessage*& message)
{
    TRAPD(err, getMessageByIdL(mailboxId,folderId,messageId,message));
    return err;
}

/*!
    Leaving version of getMessageById function
 */
void NmFrameworkAdapter::getMessageByIdL(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId,
    NmMessage*& message)
{
    // select message details to be listed
    TFSMailDetails details(EFSMsgDataStructure);

    CFSMailMessage* newMessage(NULL);
    newMessage = mFSfw->GetMessageByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId),
        TFSMailMsgId(messageId), details);

    // GetMessageByUidL can return NULL pointer
    if (newMessage) {
        message = newMessage->GetNmMessage();
        //assign all children found by mail plugin to NmMessage

        message->removeAllChildParts();
        childrenToNmMessagePartL(newMessage, message);
    }
    else {
        User::Leave(KErrNotFound);
    }
}

/*!
    Returns list of folders in a mailbox.
	
	\param mailboxId Id of the mailbox containing the folder.
	\param folderList Reference to a pointer list to receive pointers to the folders.
	
	\return Error code.
 */
int NmFrameworkAdapter::listFolders(
    const NmId& mailboxId,
    QList<NmFolder*>& folderList)
{
    CFSMailBox* currentMailbox = NULL;
    TRAPD (err, currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId));
    if (KErrNone == err) {
        RPointerArray<CFSMailFolder> folders = currentMailbox->ListFolders();
        for (int i = 0; i < folders.Count(); i++) {
            folderList.append(folders[i]->GetNmFolder());
        }
        return KErrNone;
    }
    else {
        return err;
    }
}

/*!
    Returns list of envelopes from the backend for specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Folder id.
    \param messageMetaDataList Reference to pointer list to receive the envelope objects,
     ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::listMessages(
	const NmId &mailboxId,
    const NmId &folderId,
    QList<NmMessageEnvelope*> &messageEnvelopeList)
{
    TRAPD(err, listMessagesL(mailboxId,folderId,messageEnvelopeList, nmMaxItemsInMessageList));
    return err;
}

int NmFrameworkAdapter::listMessages(
        const NmId& mailboxId,
        const NmId& folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList,
        const int maxAmountOfEnvelopes)
    {
    TInt err = KErrNone;
    TRAP(err, listMessagesL(mailboxId,folderId, messageEnvelopeList,maxAmountOfEnvelopes) );
    return err;
    }

/*!
    Leaving version of list messages
 */
void NmFrameworkAdapter::listMessagesL(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList,
        const int maxAmountOfEnvelopes)
{
    CFSMailBox * currentMailbox(NULL);
    CFSMailFolder* folder(NULL);

    //If we are requesting 0 or less mails so we can return
    if( maxAmountOfEnvelopes <= 0)
        {
        return;
        }

    int blockSize = nmListMessagesBlock;
    int maxLimit = nmMaxItemsInMessageList;
    if( maxAmountOfEnvelopes < nmMaxItemsInMessageList )
        {
        maxLimit = maxAmountOfEnvelopes;
        if(maxAmountOfEnvelopes < nmListMessagesBlock)
            {
            blockSize = maxAmountOfEnvelopes;
            }
        }
   

    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if (!currentMailbox) {
        User::Leave(KErrNotFound);
    }
    CleanupStack::PushL(currentMailbox);
    folder = mFSfw->GetFolderByUidL(currentMailbox->GetId(), TFSMailMsgId(folderId));

    if (folder) {
        CleanupStack::PushL(folder);
        // First prepare all the parameters
        // select message details to be listed
        TFSMailDetails details(EFSMsgDataEnvelope);

        // set sorting criteria
        TFSMailSortCriteria criteria;
        criteria.iField = EFSMailSortByDate;
        criteria.iOrder = EFSMailDescending;
        RArray<TFSMailSortCriteria> sorting;
        CleanupClosePushL(sorting);
        sorting.Append(criteria);

        TFSMailMsgId currentMessageId; // first call contains NULL id as begin id
        // get messages list from the backend
        MFSMailIterator* iterator(NULL);

        iterator = folder->ListMessagesL(details, sorting);
        if (iterator) {
            CleanupStack::PushL(iterator);
            RPointerArray<CFSMailMessage> messages;
            CleanupResetAndDestroy<CFSMailMessage>::PushL(messages);

            //Message list is fetched in blocks to prevent OOM in protocol plugin side
            bool moreMessagesToFollow(false);
            moreMessagesToFollow = iterator->NextL(
                TFSMailMsgId(), blockSize, messages);
            for ( int i = blockSize;
                  i < maxLimit && moreMessagesToFollow ;
                  i += blockSize ) {
                moreMessagesToFollow = iterator->NextL(
                    messages[i-1]->GetMessageId(), blockSize, messages);
            }

            //Add all found emails to the result list
            for(TInt i=0; i<messages.Count(); i++) {
                NmMessageEnvelope* newEnvelope(NULL);
                newEnvelope = messages[i]->GetNmMessageEnvelope();
                if (newEnvelope) {
                    messageEnvelopeList.append(newEnvelope);
                }
            }
            CleanupStack::PopAndDestroy( &messages );
            CleanupStack::Pop(iterator);
            delete iterator;
           	iterator = NULL;
        }
        CleanupStack::PopAndDestroy(); // sorting
        CleanupStack::PopAndDestroy(folder);
    }
    CleanupStack::PopAndDestroy(currentMailbox);
}

/*!
    Starts a message fetching operation.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message to fetch.

    \return An NmOperation object for the operation, ownership is transferred to caller
 */
NmOperation *NmFrameworkAdapter::fetchMessage(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId )
{
    NmOperation *oper = new NmFwaMessageFetchingOperation(mailboxId, folderId, messageId, *mFSfw);
    return oper;
}

/*!
    Starts a message part fetching operation.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of message containing the message parts
    \param messagePartId id of message part

    \return An NmOperation object for the operation, ownership is transferred to caller
 */
NmOperation *NmFrameworkAdapter::fetchMessagePart( 
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const NmId &messagePartId)
{
    NmOperation *oper = new NmFwaMessagePartFetchingOperation(
            mailboxId, folderId, messageId, messagePartId, *mFSfw);
    return oper;
}

/*!
    Returns sharable file handle to message part content

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of message containing the message parts
    \param messagePartId id of message part

    \return XQSharableFile, sharable file object
 */
XQSharableFile NmFrameworkAdapter::messagePartFile(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId)
{
    XQSharableFile retFile;
    TFSMailDetails details(EFSMsgDataEnvelope);
    TFSMailMsgId fsMboxId(mailboxId);
    TFSMailMsgId fsFolderId(folderId);
    TFSMailMsgId fsMsgId(messageId);
    TFSMailMsgId fsMsgPartId(messagePartId);
    
    CFSMailMessage* fsMessage = NULL;
    int error = KErrNone;
    TRAP(error, fsMessage = mFSfw->GetMessageByUidL(fsMboxId, fsFolderId,
            fsMsgId, details) );
    
    CFSMailMessagePart* fsMessagePart = NULL;
    if (fsMessage && error == KErrNone) {
        TRAP(error, fsMessagePart = fsMessage->ChildPartL(fsMsgPartId) );
        
    }
    if (fsMessagePart && error == KErrNone) {
        RFile file = fsMessagePart->GetContentFileL();
        retFile.setHandle(file);
    }
    return retFile;
}

/*!
    Get the id of a standard folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderType Type of standard folder to get the id of.

    \return Id of the standard folder, or NmId for which NmId.getId() == 0 if not found.
 */
NmId NmFrameworkAdapter::getStandardFolderId(
    const NmId& mailboxId,
    NmFolderType folderType )
{
    TFSMailMsgId folderId;
    NmId resultId(0);
    CFSMailBox * currentMailbox(NULL);

    TRAPD(error, currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId) );

    if( !currentMailbox || error != KErrNone ) {
    	return resultId;
    }

    switch(folderType) {
    	case NmFolderInbox:
    		folderId = currentMailbox->GetStandardFolderId( EFSInbox );
    		break;
    	case NmFolderOutbox:
    		folderId = currentMailbox->GetStandardFolderId( EFSOutbox );
    		break;
    	case NmFolderDrafts:
    		folderId = currentMailbox->GetStandardFolderId( EFSDraftsFolder );
    		break;
    	case NmFolderSent:
    		folderId = currentMailbox->GetStandardFolderId( EFSSentFolder );
    		break;
    	case NmFolderDeleted:
    		folderId = currentMailbox->GetStandardFolderId( EFSDeleted );
    		break;
    	case NmFolderOther:
    	default:
    		folderId = currentMailbox->GetStandardFolderId( EFSOther );
    		break;
    }

    delete currentMailbox;
    currentMailbox = NULL;

    resultId.setPluginId32(static_cast<quint32>(folderId.PluginId().iUid));
    resultId.setId32(folderId.Id());

    return resultId;
}

/*!
    Connect to mailbox if not already connected and refresh.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::refreshMailbox(NmId mailboxId)
{
    TRAPD(err, RefreshMailboxL(mailboxId)); // return value not used
    return ( err == KErrNone ) ? NmNoError : NmGeneralError;
}

/*!
    Connect to mailbox if not already connected and refresh.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::goOnline(const NmId& mailboxId)
{
    TRAPD(err, GoOnlineL(mailboxId)); // return value not used
    return ( err == KErrNone ) ? NmNoError : NmGeneralError;
}

/*!
    Disconnect to mailbox if not already disconnected.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::goOffline(const NmId& mailboxId)
{
    TRAPD(err, GoOfflineL(mailboxId)); // return value not used
    return ( err == KErrNone ) ? NmNoError : NmGeneralError;
}

/*!
    Sets content for the given message part. Client shouldn't create
    message part on its own. Instead it should be requested by calling
    e.g. getMessageById.

    \param mailboxId Id of the mailbox containing the folder (Unused).
    \param folderId Id of the folder containing the message (Unused).
    \param messageId Id of the message (Unused).
    \param messagePart Part of a message for which content is going to be requested.

    \return Error code.
 */
int NmFrameworkAdapter::contentToMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    TRAPD(err, contentToMessagePartL(mailboxId,folderId,messageId,messagePart));
    return err;
}

/*!
    Leaving version of contentToMessagePart function
 */
void NmFrameworkAdapter::contentToMessagePartL(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    CFSMailMessagePart* cfsPart = CFSMailMessagePart::NewLC(messageId,messagePart);
    cfsPart->SetMailBoxId(TFSMailMsgId(mailboxId));
    cfsPart->SetFolderId(TFSMailMsgId(folderId));
    QString contentType = messagePart.contentType();

    if (cfsPart && contentType.startsWith(NmContentTypeTextPlain)) {
        // add 1 for max size for zero termination and prevend 0 size hbufc
        HBufC* data = HBufC::NewLC(cfsPart->FetchedContentSize()+1);
        TPtr dataPtr = data->Des();
        TInt startOffset = 0;

        cfsPart->GetContentToBufferL(dataPtr, startOffset);
        messagePart.setTextContent(NmConverter::toQString(dataPtr), contentType);
        CleanupStack::PopAndDestroy(data);
    }
    else if (cfsPart){
        RFile file = cfsPart->GetContentFileL();
        TInt fileSize = 0;
        file.Size(fileSize);
        if (fileSize != 0) {
            HBufC8* data = HBufC8::NewLC(fileSize);
            TPtr8 dataPtr = data->Des();

            if (fileSize != 0) {
                TInt dummyPos = 0;
                file.Seek(ESeekStart, dummyPos);
                User::LeaveIfError(file.Read(dataPtr));
            }

            if (contentType.startsWith(NmContentTypeTextHtml) || contentType.contains( NmContentDescrAttachmentHtml )) {
                QRegExp rxlen("(?:charset=)(?:\"?)([\\-\\_a-zA-Z0-9]+)", Qt::CaseInsensitive);
                QString charset;
                int pos = rxlen.indexIn(contentType);
                if (pos > -1) {
                    charset = rxlen.cap(1);
                }
                QByteArray msgBytes = QByteArray(reinterpret_cast<const char*>(dataPtr.Ptr()), fileSize);
                QTextCodec *codec = QTextCodec::codecForName(charset.toAscii());
                if (!codec) {
                    codec = QTextCodec::codecForName("iso-8859-1");
                }
                QString encodedStr = codec->toUnicode(msgBytes); 
                messagePart.setTextContent(encodedStr, contentType);           
            }
            else {
                messagePart.setBinaryContent(QByteArray(
                        reinterpret_cast<const char*>(dataPtr.Ptr()), fileSize), contentType);
            }
            CleanupStack::PopAndDestroy(data);
        }
        file.Close();
    }
    else {
        // null cfsPart
        User::Leave(KErrNotFound);
    }

    CleanupStack::PopAndDestroy(cfsPart);
}

/*!
    Deletes the listed messages from specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the messages.
    \param messageIdList List of ids of the messages to be deleted.

    \return Error code.
 */
int NmFrameworkAdapter::deleteMessages(
	const NmId &mailboxId,
	const NmId &folderId,
	const QList<NmId> &messageIdList)
{
    TInt err = NmNoError;
    RArray<TFSMailMsgId> messageIds;
    for (TInt i=0; i<messageIdList.size(); i++) {
        err = messageIds.Append(TFSMailMsgId(messageIdList[i]));
        if (err!=NmNoError) {
            break;
        }
    }
    if (NmNoError == err) {
        TRAP(err, mFSfw->DeleteMessagesByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId), messageIds));
    }
    messageIds.Close();

    return ( err == NmNoError ) ? NmNoError : NmGeneralError;
}

/*!
    Starts an async operation to store listed message envelopes to the given mailbox

    \param mailboxId Id of the mailbox.
    \param folderId Unused.
    \param envelopeList List of the envelope objects to store.

    \return NmStoreEnvelopesOperation
 */
NmStoreEnvelopesOperation *NmFrameworkAdapter::storeEnvelopes(
	const NmId &mailboxId,
	const NmId &folderId,
	const QList<const NmMessageEnvelope*> &envelopeList)
{
    Q_UNUSED(folderId);
    NMLOG("NmFrameworkAdapter::storeEnvelopes() <---");

    NmStoreEnvelopesOperation* operation(NULL);
    RPointerArray<CFSMailMessage> envelopeMessages;

    int count = envelopeList.count();
    for(int i=0;i<count;i++) {
        TRAP_IGNORE( envelopeMessages.AppendL(mailMessageFromEnvelopeL( *envelopeList.at(i)) ));
    }

    if ( envelopeMessages.Count() ) {
        operation = new NmFwaStoreEnvelopesOperation(
                mailboxId,
                envelopeMessages,
                *mFSfw);
        }

    NMLOG("NmFrameworkAdapter::storeEnvelopes() --->");
    return operation;
}

/*!
    Creates a new message into the drafts folder a mailbox.

    \param mailboxId Id of the mailbox.

    \return NmMessageCreationOperation
 */
NmMessageCreationOperation *NmFrameworkAdapter::createNewMessage(const NmId &mailboxId)
{
    NmMessageCreationOperation *oper =
        new NmFwaMessageCreationOperation(mailboxId, *mFSfw);
    return oper;
}

/*!
    Creates a new forward message into the drafts folder of a mailbox.

    \param mailboxId Id of the mailbox.
    \param originalMessageId Id of the original message. Original message must be in the given mailbox.

    \return NmMessageCreationOperation
 */
NmMessageCreationOperation *NmFrameworkAdapter::createForwardMessage(
    const NmId &mailboxId,
    const NmId &originalMessageId)
{
    NmMessageCreationOperation *oper =
        new NmFwaForwardMessageCreationOperation(mailboxId, originalMessageId, *mFSfw);
    return oper;
}

/*!
    Creates a new reply message into the drafts folder of a mailbox.

    \param mailboxId Id of the mailbox.
    \param originalMessageId Id of the original message. Original message must be in the given mailbox.
    \param replyAll Is reply for all recipients?

    \return NmMessageCreationOperation
 */
NmMessageCreationOperation *NmFrameworkAdapter::createReplyMessage(
    const NmId &mailboxId,
    const NmId &originalMessageId,
    const bool replyAll)
{
    NmMessageCreationOperation *oper =
        new NmFwaReplyMessageCreationOperation(mailboxId, originalMessageId, replyAll, *mFSfw);
    return oper;
}

/*!
    Not implemented yet.
 */
int NmFrameworkAdapter::saveMessage(const NmMessage &message)
{
    Q_UNUSED(message);
    return NmNoError;
}

/*!
    Store asynchronously message with its parts.
 */
NmOperation* NmFrameworkAdapter::saveMessageWithSubparts(const NmMessage &message)
{
    CFSMailMessage * cfsMessage = NULL;
    NmOperation *oper = NULL;

    int err = KErrNone;
    TRAP(err, cfsMessage = CFSMailMessage::NewL(message));
    if (err == KErrNone) {
        //transfers ownership of cfsMessage
        oper = new NmFwaStoreMessageOperation(cfsMessage, *mFSfw);
    }

    return oper;
}

/*!
    From MFSMailEventObserver

    \sa MFSMailEventObserver
 */
void NmFrameworkAdapter::EventL(
    TFSMailEvent aEvent,
    TFSMailMsgId mailbox,
    TAny* param1,
    TAny* param2,
    TAny* param3)
{
    switch (aEvent) {
        // Mailbox related events:
        case TFSEventNewMailbox:
            handleMailboxEvent(mailbox, NmMailboxCreated);
            break;
        case TFSEventMailboxDeleted:
            handleMailboxEvent(mailbox, NmMailboxDeleted);
            break;
        case TFSEventMailboxRenamed:
            handleMailboxEvent(mailbox, NmMailboxChanged);
            break;

        //
        // Mail related events:
        // signal:
        // void messageEvent(
        //      NmMessageEvent event,
        //      const NmId &folderId,
        //      QList<NmId> &messageIds);
        //
        // enum NmMessageEvent
        //   NmMessageCreated,
        //   NmMessageChanged,
        //   NmMessageDeleted
        //

        // New mails created
        // param1: RArray<TFSMailMsgId>* aNewEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventNewMail:
            handleMessageEvent(param1, param2, NmMessageCreated, mailbox);
            break;
        // Mails changed
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventMailChanged:
            handleMessageEvent(param1, param2, NmMessageChanged, mailbox);
            break;
        // Mails deleted
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventMailDeleted:
            handleMessageEvent(param1, param2, NmMessageDeleted, mailbox);
            break;


        // Mails moved
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aNewParentFolder
        // param3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailMoved:
            handleMailMoved(param1, param2, param3, mailbox);
            break;

        // Mails copied
        // param1: RArray<TFSMailMsgId>* aNewEntries
        // param2: TFSMailMsgId* aNewParentFolder
        // param3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailCopied:
            handleMailCopied(param1, param2, mailbox);
            break;


        //sync state related events
        case TFSEventMailboxSyncStateChanged:
            handleSyncstateEvent(param1, mailbox);
            break;

        case TFSEventMailboxOnline:{
            NmId id = NmConverter::mailMsgIdToNmId(mailbox);
            emit connectionEvent(Connected, id);
            }
            break;
        case TFSEventMailboxOffline:{
            NmId id = NmConverter::mailMsgIdToNmId(mailbox);
            emit connectionEvent(Disconnected, id);
            }
            break;

        default:
            break;
    }
}

/*!
    Delete the message from specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message.

    \return Error code.
 */
int NmFrameworkAdapter::removeMessage(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId)
{
    TRAPD(error, removeMessageL(mailboxId, folderId, messageId));
    return error;
}

/*!
    Subscribe to events from a mailbox.

    \param mailboxId Id of the mailbox.
*/
void NmFrameworkAdapter::subscribeMailboxEvents(const NmId& mailboxId)
{
    TRAP_IGNORE(mFSfw->SubscribeMailboxEventsL(mailboxId, *this));
}

/*!
    Unsubscribe to events from a mailbox.

    \param mailboxId Id of the mailbox.
*/
void NmFrameworkAdapter::unsubscribeMailboxEvents(const NmId& mailboxId)
{
    mFSfw->UnsubscribeMailboxEvents(mailboxId, *this);
}

NmId NmFrameworkAdapter::getMailboxIdByMailMsgId(TFSMailMsgId mailbox)
{
    NmId nmId(0);

    QList<NmId> mailboxIds;
    listMailboxIds(mailboxIds);

    QListIterator<NmId> iterator(mailboxIds);
    while (iterator.hasNext()) {
        nmId = iterator.next();
        if (nmId.id32() == mailbox.Id()) {
            break;
        }
    }
    mailboxIds.clear();
    return nmId;
}

/*!
    Leaving version of removeMessage
*/
void NmFrameworkAdapter::removeMessageL(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId)
{
    CFSMailFolder* folder = mFSfw->GetFolderByUidL( TFSMailMsgId(mailboxId), TFSMailMsgId(folderId));
    CleanupStack::PushL(folder);
    if ( folder ) {
        folder->RemoveMessageL(TFSMailMsgId(messageId));
    }
    CleanupStack::PopAndDestroy(folder);
}


/*!
   Sends the given message.
 */
NmMessageSendingOperation *NmFrameworkAdapter::sendMessage(
    NmMessage *message)
{
    NmMessageSendingOperation *oper = new NmFwaMessageSendingOperation(*this, message, *mFSfw);
	return oper;
}

/*!
   Add attachment into the given message.
 */
NmAddAttachmentsOperation *NmFrameworkAdapter::addAttachments(
    const NmMessage &message,
    const QList<QString> &fileList)
{
    NmAddAttachmentsOperation *oper = new NmFwaAddAttachmentsOperation(message, fileList, *mFSfw);
    return oper;
}

/*!
   Remove attachment from the given message.
 */
NmOperation *NmFrameworkAdapter::removeAttachment(
    const NmMessage &message,
    const NmId &attachmentPartId)
{
    NmOperation *oper = new NmFwaRemoveAttachmentOperation(message, attachmentPartId, *mFSfw);
    return oper;
}

/*!
   Checks outbox for messages
 */
NmCheckOutboxOperation *NmFrameworkAdapter::checkOutbox(const NmId& mailboxId)
{
    NmCheckOutboxOperation *oper =
        new NmFwaCheckOutboxOperation(mailboxId, *mFSfw);

    return oper;
}

/*!
    Returns the current sync state of the given mailbox
 */
NmSyncState NmFrameworkAdapter::syncState(const NmId& mailboxId) const
{
   CFSMailBox* mailBox = NULL;
   TRAPD(err, mailBox = mFSfw->GetMailBoxByUidL(TFSMailMsgId(mailboxId)) );
   if (KErrNone == err && mailBox) {
       TSSMailSyncState syncState = mailBox->CurrentSyncState();
       delete mailBox;
       if (EmailSyncing == syncState) {
           return Synchronizing;
       }
       else {
           return SyncComplete;
       }
   }
   else {
       return SyncComplete;
   }
}

/*!
    Returns the current connection state of the given mailbox
 */
NmConnectState NmFrameworkAdapter::connectionState(const NmId& mailboxId) const
{
    CFSMailBox* mailBox = NULL;
    TRAPD(err, mailBox = mFSfw->GetMailBoxByUidL(TFSMailMsgId(mailboxId)) );
    if (KErrNone == err && mailBox) {
        TFSMailBoxStatus status = mailBox->GetMailBoxStatus();
        delete mailBox;
        if (status == EFSMailBoxOnline) {
            return Connected;
        }
        else {
            return Disconnected;
        }
    }
    else {
       return Disconnected;
    }
}

/*!
   creates CFSMailMessage based on envelope
   Call to this must be trapped
 */
CFSMailMessage* NmFrameworkAdapter::mailMessageFromEnvelopeL(
    const NmMessageEnvelope& envelope)
{
    NmMessage* nmMessage = new(ELeave) NmMessage( envelope );
    CFSMailMessage* message = CFSMailMessage::NewL( *nmMessage );
    delete nmMessage;
    nmMessage = NULL;
    return message;
}


/*!
   Assigns recursively all children to NmMessagePart object.
 */
void NmFrameworkAdapter::childrenToNmMessagePartL(
        CFSMailMessagePart *cfsParent,
        NmMessagePart *nmParent)
{
    User::LeaveIfNull(cfsParent);
    User::LeaveIfNull(nmParent);

    RPointerArray<CFSMailMessagePart> parts;
    cfsParent->ChildPartsL(parts);
    CleanupResetAndDestroy<CFSMailMessagePart>::PushL(parts);

    NmMessagePart *nmPart = NULL;
    if(parts.Count()){
        for(int i=0; i<parts.Count(); i++){
            nmPart = parts[i]->GetNmMessagePart();
            nmParent->addChildPart(nmPart);
            childrenToNmMessagePartL(parts[i], nmPart);
        }
    }
    CleanupStack::PopAndDestroy(1, &parts);
}

/*!
   Leaving Refresh function
 */
int NmFrameworkAdapter::RefreshMailboxL(NmId mailboxId)
{
    int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
        result = currentMailbox->RefreshNowL();
        CleanupStack::PopAndDestroy(currentMailbox);
        currentMailbox = NULL;
    }
    return result;
}

/*!
   Leaving Go Online function
 */
int NmFrameworkAdapter::GoOnlineL(const NmId& mailboxId)
{
    int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
        currentMailbox->GoOnlineL();
        CleanupStack::PopAndDestroy(currentMailbox);
        currentMailbox = NULL;
    }
    return result;
}

/*!
   Leaving Go Offline function
 */
int NmFrameworkAdapter::GoOfflineL(const NmId& mailboxId)
{
	int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
		currentMailbox->GoOfflineL();
        CleanupStack::PopAndDestroy(currentMailbox);
        currentMailbox = NULL;
    }
    return result;
}

/*!
   handles mailbox related events
 */
void NmFrameworkAdapter::handleMailboxEvent( TFSMailMsgId mailbox, NmMailboxEvent event)
{
    QList<NmId> mailboxIds;
    NmId nmId;
    if (event == NmMailboxDeleted) {
		nmId = mailbox.GetNmId();
    } else {
		nmId = getMailboxIdByMailMsgId(mailbox);
    }
    mailboxIds.append(nmId);
    emit mailboxEvent(event, mailboxIds);
    }

/*!
   handles new, changed, deleted events
 */
void NmFrameworkAdapter::handleMessageEvent(
    TAny* param1,
    TAny* param2,
    NmMessageEvent event,
    TFSMailMsgId mailbox)
{
    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId folderId = fsFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    emit messageEvent(event, folderId, messageIds, mailbox.GetNmId());
}

/*!
   function to handle mailmoved event
 */
void NmFrameworkAdapter::handleMailMoved(TAny* param1,TAny* param2,TAny* param3, TFSMailMsgId mailbox)
{
    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsFromFolderId = reinterpret_cast<TFSMailMsgId*> (param3);
    NmId fromFolderId = fsFromFolderId->GetNmId();
    TFSMailMsgId* fsToFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId toFolderId = fsToFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    // Yes, this is supposed to emit two signals from single incoming
    // event. Design decison was to create separate signals.
    emit messageEvent(NmMessageDeleted, fromFolderId, messageIds, mailbox.GetNmId());
    emit messageEvent(NmMessageCreated, toFolderId, messageIds, mailbox.GetNmId());
}

/*!
   function to handle mailcopied event
 */
void NmFrameworkAdapter::handleMailCopied(TAny* param1,TAny* param2, TFSMailMsgId mailbox)
{
    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsToFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId toFolderId = fsToFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    // Not interested in param3 (aOldParentFolder)
    emit messageEvent(NmMessageCreated, toFolderId, messageIds, mailbox.GetNmId());
}

void NmFrameworkAdapter::handleSyncstateEvent(TAny* param1, TFSMailMsgId mailbox)
{
    TSSMailSyncState* state = static_cast<TSSMailSyncState*>( param1 );
    NmOperationCompletionEvent event;
    event.mMailboxId = NmConverter::mailMsgIdToNmId(mailbox);

    switch(*state)
        {
        case StartingSync:
            {
            event.mCompletionCode = NmNoError;
            emit syncStateEvent(Synchronizing, event);
            break;
            }
        case FinishedSuccessfully:
            {
            event.mCompletionCode = NmNoError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case PasswordNotVerified:
            {
            event.mCompletionCode = NmAuthenticationError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case SyncError:
        default:
            {
            event.mCompletionCode = NmServerConnectionError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        };
}
Q_EXPORT_PLUGIN(NmFrameworkAdapter)

