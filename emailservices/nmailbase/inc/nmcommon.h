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
* Description:
*
*/

#ifndef NMCOMMON_H_
#define NMCOMMON_H_

#include <QtGlobal>
#include <QString>
#include <QMetaType>
#include <QVariant>

#define USE_POPIMAP_TESTPLUGIN

//for pop/imap client plugins testing
//used in uiengine, clientplugins and testplugin
#ifdef USE_POPIMAP_TESTPLUGIN
#define POPTESTPLUGINID 0x2000EEEE
#define IMAPTESTPLUGINID  0x2000EEEF
#endif

/*!
    \struct NmMailboxEvent
    \brief Enumeration for mailbox related events
 */
enum NmMailboxEvent
{
    NmMailboxCreated,
    NmMailboxChanged,
    NmMailboxDeleted
};
Q_DECLARE_METATYPE(NmMailboxEvent)

/*!
    \struct NmMessageEvent
    \brief Enumeration for message related events
 */
enum NmMessageEvent
{
    NmMessageCreated,
    NmMessageChanged,
    NmMessageDeleted,
    NmMessageFound // Search functionality related enumeration.
};
Q_DECLARE_METATYPE(NmMessageEvent)

enum NmFolderType
{
    NmFolderInbox = 1,
    NmFolderOutbox,
    NmFolderDrafts,
    NmFolderSent,
    NmFolderDeleted,
    NmFolderOther
};

enum NmConnectState
{
	Connecting,
	Connected,
	Disconnecting,
	Disconnected
};
Q_DECLARE_METATYPE(NmConnectState)

enum NmSyncState
{
    Synchronizing,
    SyncComplete
};
Q_DECLARE_METATYPE(NmSyncState)

enum NmOperationType
{
    NoOp,
    Synch
};

enum NmEnvelopeProperties
{
	MarkAsRead,
	MarkAsUnread
};

enum NmMessagePriority
{
    NmMessagePriorityLow,
    NmMessagePriorityNormal,
    NmMessagePriorityHigh
};

/*!
    \struct NmMessageFlag
    \brief Enumeration for message flags
 */
enum NmMessageFlag {
    NmMessageFlagRead = 1,
    NmMessageFlagReadLocally = 2,
    NmMessageFlagLow = 4,
    NmMessageFlagImportant = 8,
    NmMessageFlagFollowUpComplete = 16,
    NmMessageFlagFollowUp = 32,
    NmMessageFlagAttachments = 64,
    NmMessageFlagMultiple = 128,
    NmMessageFlagCalendarMsg = 256,
    NmMessageFlagAnswered = 512,
    NmMessageFlagForwarded = 1024,
    NmMessageFlagOnlyToMe = 2048,
    NmMessageFlagRemoteDeleted = 4096,
    NmMessageFlagHasMsgSender = 8192
};
Q_DECLARE_FLAGS(NmMessageFlags, NmMessageFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(NmMessageFlags)

/** email list sorting options */
enum NmMailSortField
    {
    NmMailDontCare,            // user accepts any sort order
    NmMailSortByDate,          // sorting is done by date
    NmMailSortBySender,        // sorting is done by sender
    NmMailSortByRecipient,     // sorting is done by recipients
    NmMailSortBySubject,       // sorting is done by subject
    NmMailSortByPriority,      // sorting is done by priority
    NmMailSortByFlagStatus,    // follow up and completed
    NmMailSortByUnread,        // sorting is based on unread
    NmMailSortBySize,          // sorting is done by size
    NmMailSortByAttachment     // sorting is based on if message has attachments
    };
Q_DECLARE_METATYPE(NmMailSortField)


/** email list sort order */
enum NmMailSortOrder
    {
	NmMailDescending,
	NmMailAscending
    };
Q_DECLARE_METATYPE(NmMailSortOrder)

/*!
	NMail error codes
*/

const int NmNoError = 0;
const int NmNotFoundError = -1;
const int NmGeneralError = -2;
const int NmCancelError = -3;
const int NmAuthenticationError = -200;
const int NmServerConnectionError = -201;
const int NmConnectionError = -202;

/*!
	Predefined constants for ContentTypes and parameters
	for ContentType and ContentDisposition
*/
const QString NmContentTypeMultipartMixed = "multipart/mixed";
const QString NmContentTypeMultipartAlternative = "multipart/alternative";
const QString NmContentTypeMultipartDigest = "multipart/digest";
const QString NmContentTypeMultipartParallel = "multipart/parallel";
const QString NmContentTypeMultipartRelated =  "multipart/related";
const QString NmContentTypeTextPlain = "text/plain";
const QString NmContentTypeTextHtml = "text/html";
const QString NmContentTypeTypeMessage = "message/rfc822";
const QString NmContentTypeParamName = " name=";
const QString NmContentTypeParamCharset = " charset=";

const QString NmContentDispAttachment = " attachment";
const QString NmContentDispParamFilename = " filename=";

const QString NmContentDescrAttachmentHtml = "Attachment.html";

/*!
   It keep info about maximum messages in message list.
   
   It is used in NmFrameworkAdapter and NmDataPluginInterface
 */
static const int NmMaxItemsInMessageList = 1000;

/*!
    \class NmId
    \brief Unique identifier for any mail spesific objects like mailbox, folder,
    message, message part etc. Id is stored in unsigned 64 bit integer
 */
class NmId
{
public:
	inline NmId() : mPluginId(0), mId(0) { }
    inline NmId(quint64 combinedId);
	inline quint64 id() const;
    inline void setId(quint64 pluginIdAndId);
	inline bool operator==(const NmId& id) const;
	inline bool operator!=(const NmId& id) const;

    inline quint32 id32() const;
    inline void setId32(quint32 id32);
    inline quint32 pluginId32() const;
    inline void setPluginId32(quint32 pluginId32);

private:
	quint32 mPluginId;
	quint32 mId;
};

Q_DECLARE_METATYPE(NmId)
Q_DECLARE_METATYPE(QList<NmId>)

/*!
	 Constructor for nm id \a id is 64 bit unsigned integer
 */
inline NmId::NmId(quint64 combinedId)
{
    setId(combinedId);
}

/*!
	 Setter for nm id \a id is 64 bit unsigned integer
 */
inline void NmId::setId(quint64 pluginIdAndId)
{
	// first 32 bits (from big end) refers fs plugin id
    mPluginId = pluginIdAndId >> 32;
	// last 32 bits refers fs id
    mId = pluginIdAndId;
}

/*!
	 Returns 64 unsigned integer id
 */
inline quint64 NmId::id() const
{
    quint64 id = mPluginId;
    id = id << 32;
    id += mId;
    return id;
}

/*!
	 Equal operator
 */
inline bool NmId::operator==(const NmId& id) const
{
	bool ret = true;
    if ((mId != id.mId) || (mPluginId != id.mPluginId)) {
		ret = false;
	}
	return ret;
}

/*!
	 Not equal operator
 */
inline bool NmId::operator!=(const NmId& id) const
{
	bool ret = false;
    if ((mId != id.mId) || (mPluginId != id.mPluginId)) {
		ret = true;
	}
	return ret;
}

/*!
     return unsigned 32-bit id
 */
inline quint32 NmId::id32() const
{
    return mId;
}

/*!
     returns unsigned 32-bit plugin id
 */
inline quint32 NmId::pluginId32() const
{
    return mPluginId;
}

/*!
     sets unique 32-bit id
 */
inline void NmId::setId32(quint32 id32)
{
    mId = id32;
}

/*!
     sets unique 32-bit plugin id
 */
inline void NmId::setPluginId32(quint32 pluginId32)
{
    mPluginId = pluginId32;
}

/*!
     static function for debug prints
 */
#include <QDebug>
#include <QFile>
#include <QDateTime>

static void printToFileAndConsole(QString str, QString filename)
{    
    // Print to file
    QFile debugFile(filename);
    QIODevice::OpenMode openMode = QIODevice::Text;
    if (debugFile.exists()) {
        openMode |= QIODevice::Append;
    } else {
        openMode |= QIODevice::WriteOnly;
    }

    // Create date string
    QString d = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");

    if (debugFile.open(openMode)) {
        QTextStream debugStream(&debugFile);
        debugStream << d << "  " << str << endl;
        debugFile.close();
    }
    // Print to console
    qDebug() << d << str << endl; 
}

// 
// NMLOG is used to log QStrings to text file. For example:
//
// NMLOG("nmailui: application opened successfully");
//
// QString fileName = "mailbox.xml";
// int error = -12;
// NMLOG(QString("### cannot open file: err=%1  file='%2' ###").arg(error).arg(fileName));
//
#ifdef _DEBUG
#define NMLOG(a) { printToFileAndConsole(a, "c:/logs/nmail.log"); }
#else
#define NMLOG(a)
#endif


/*! email list sort criteria definition */
class NmMailSortCriteria
{
public:
    NmMailSortField mField;
    NmMailSortOrder mOrder;
};

/*!
    \class NmOperationCompletionEvent
    \brief Generic wrapper for information related to asynchronous operations
 */
class NmOperationCompletionEvent
{
public:
    NmOperationType  mOperationType;
    int  mCompletionCode;
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
};
Q_DECLARE_METATYPE(NmOperationCompletionEvent)

#endif /* NMCOMMON_H_ */



