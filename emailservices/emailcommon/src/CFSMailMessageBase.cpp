/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  common email base object
*
*/


//<cmail>
#include <nmcommonheaders.h>
#include "emailtrace.h"
#include "CFSMailMessageBase.h"
//</cmail>

//<qmail>
#include "nmmessageenvelope.h"
#include "nmconverter.h"
//</qmail>

// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewLC( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
    CFSMailMessageBase* message = new (ELeave) CFSMailMessageBase();
    CleanupStack:: PushL(message);
    message->ConstructL(aMessageId);
    return message;
} 

// -----------------------------------------------------------------------------
// CFSMailMessageBase::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase * CFSMailMessageBase::NewL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;
    CFSMailMessageBase* message =  CFSMailMessageBase::NewLC(aMessageId);
    CleanupStack:: Pop(message);
    return message;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::CFSMailMessageBase
// -----------------------------------------------------------------------------
CFSMailMessageBase::CFSMailMessageBase(): iSender(NULL)
{
    FUNC_LOG;
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::ConstructL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ConstructL( TFSMailMsgId aMessageId )
{
    FUNC_LOG;

    // Construction of shared data object
    iNmPrivateMessageEnvelope = new NmMessageEnvelopePrivate();

	// typedef long int TInt32 -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mId.setId32((quint32)aMessageId.Id() );

	// typedef unsigned int TUint -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mId.setPluginId32((quint32)aMessageId.PluginId().iUid);

    // construct the CFSMailAddress object and connect it with NmAddress private data
    iSender = CFSMailAddress::NewL(iNmPrivateMessageEnvelope->mSender);
    //iSender->ShareNmAddressPrivate(iNmPrivateMessageEnvelope->mSender);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ConstructL
// -----------------------------------------------------------------------------
void CFSMailMessageBase::ConstructL(
    const NmMessageEnvelope &aMessageEnvelope )
{
    FUNC_LOG;

    iNmPrivateMessageEnvelope = aMessageEnvelope.d;
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::~CFSMailMessageBase
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessageBase::~CFSMailMessageBase()
{
    FUNC_LOG;
    if(iSender)
        {
        delete iSender;
        iSender = NULL;
        }
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMessageId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMessageId(  ) const
{
    FUNC_LOG;
    //For message  
    TFSMailMsgId id = TFSMailMsgId(iNmPrivateMessageEnvelope->mId);
    
    //For message part
    if(id.IsNullId()){
        id = TFSMailMsgId(iNmPrivateMessageEnvelope->mParentId);
    }
    
    return id;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetMessageId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetMessageId( const TFSMailMsgId aMessageId )
{
    // typedef long int TInt32 -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mId.setId32( aMessageId.Id() );

    // typedef unsigned int TUint -> typedef unsigned int quint32
    iNmPrivateMessageEnvelope->mId.setPluginId32((unsigned int)aMessageId.PluginId().iUid);
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetFolderId( ) const
{
    FUNC_LOG;
    return NmConverter::nmIdToMailMsgId(iNmPrivateMessageEnvelope->mParentId);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFolderId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFolderId( const TFSMailMsgId aFolderId )
{
    FUNC_LOG;
    iNmPrivateMessageEnvelope->mParentId =
        NmConverter::mailMsgIdToNmId(aFolderId);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::GetMailBoxId( ) const
{
    FUNC_LOG;
    return NmConverter::nmIdToMailMsgId(iNmPrivateMessageEnvelope->mMailboxId);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetMailBoxId
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetMailBoxId( const TFSMailMsgId aMailBoxId )
{
    FUNC_LOG;
    iNmPrivateMessageEnvelope->mMailboxId = NmConverter::mailMsgIdToNmId(aMailBoxId);
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSender
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSender( CFSMailAddress* aSender )
{
    FUNC_LOG;
    if (iSender) 
        {
        delete iSender;
        iSender = NULL;
        }
    iSender = aSender;
    iNmPrivateMessageEnvelope->mSender = iSender->GetNmAddress();
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSender
// -----------------------------------------------------------------------------
EXPORT_C CFSMailAddress* CFSMailMessageBase::GetSender() const
{
    FUNC_LOG;
    return iSender;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetToRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetToRecipients()
{
// <qmail>
    FUNC_LOG;
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mToRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetCCRecipients()
{
// <qmail>
    FUNC_LOG;
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mCcRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetBCCRecipients
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailAddress> CFSMailMessageBase::GetBCCRecipients()
{
// <qmail>
    FUNC_LOG;
    return NmConverter::toRPointerArray(
        iNmPrivateMessageEnvelope->mBccRecipients);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendToRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendToRecipient(
    CFSMailAddress* aRecipient )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mToRecipients.append(
        aRecipient->GetNmAddress());
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendCCRecipient( CFSMailAddress* aRecipient )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mCcRecipients.append(
        aRecipient->GetNmAddress());
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::AppendBCCRecipient
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::AppendBCCRecipient( CFSMailAddress* aRecipient )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mBccRecipients.append(
        aRecipient->GetNmAddress());
// </qmail>      
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearToRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearToRecipients( )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mToRecipients.clear();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearCcRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearCcRecipients( )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mCcRecipients.clear();
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ClearBccRecipients
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ClearBccRecipients( )
{
    FUNC_LOG;
// <qmail>
    iNmPrivateMessageEnvelope->mBccRecipients.clear();
// </qmail>
}

//<qmail>
// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetSubject
// -----------------------------------------------------------------------------
EXPORT_C TDesC& CFSMailMessageBase::GetSubject() const
{
    FUNC_LOG;
    iSubjectPtr.Set(reinterpret_cast<const TUint16*> (iNmPrivateMessageEnvelope->mSubject.utf16()),
        iNmPrivateMessageEnvelope->mSubject.length());
    return iSubjectPtr;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetDate
// -----------------------------------------------------------------------------
EXPORT_C TTime CFSMailMessageBase::GetDate() const
{
    FUNC_LOG;
    return NmConverter::toTTime(iNmPrivateMessageEnvelope->mSentTime);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetDate
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetDate( const TTime aDate )
{
    FUNC_LOG;
    iNmPrivateMessageEnvelope->mSentTime = NmConverter::toQDateTime(aDate);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetSubject
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetSubject( const TDesC& aSubject )
{
    FUNC_LOG;
    QString qtSubject = QString::fromUtf16(aSubject.Ptr(), aSubject.Length());
    iNmPrivateMessageEnvelope->mSubject = qtSubject;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetFlags
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailMessageBase::GetFlags( ) const
{
    FUNC_LOG;
    return (TInt)iNmPrivateMessageEnvelope->flags();
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetFlag( const TInt aFlag )
{
    FUNC_LOG;
    iNmPrivateMessageEnvelope->setFlags((NmMessageFlags)aFlag, true);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::ResetFlag
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::ResetFlag( const TInt aFlag )
{
    FUNC_LOG;
    iNmPrivateMessageEnvelope->setFlags((NmMessageFlags)aFlag, false);
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::IsFlagSet
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailMessageBase::IsFlagSet( const TInt aFlag ) const
{
    FUNC_LOG;
    TBool result = EFalse;
    if (iNmPrivateMessageEnvelope->isFlagSet((NmMessageFlag)aFlag)) 
        {
        result = ETrue;
        }
    return result;
}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailMessageBase::IsRelatedTo
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailMessageBase::IsRelatedTo() const
{
    FUNC_LOG;
    return iRelatedTo;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetRelatedTo
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetRelatedTo( const TFSMailMsgId aMessageId )
{
    FUNC_LOG;
    iRelatedTo = aMessageId;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::SetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailMessageBase::SetReplyToAddress( CFSMailAddress* aReplyToAddress )
{
    FUNC_LOG;
    // store sender
    if (iReplyTo)
        {
        delete iReplyTo;
        }
    iReplyTo = aReplyToAddress;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetReplyToAddress
// -----------------------------------------------------------------------------
EXPORT_C const CFSMailAddress& CFSMailMessageBase::GetReplyToAddress()
{
    FUNC_LOG;
    return *iReplyTo;
}

// -----------------------------------------------------------------------------
// CFSMailMessageBase::GetNmMessageEnvelope
// -----------------------------------------------------------------------------
EXPORT_C NmMessageEnvelope* CFSMailMessageBase::GetNmMessageEnvelope()
{
    FUNC_LOG;
    return new NmMessageEnvelope(iNmPrivateMessageEnvelope);
}

