/* ============================================================================
 *  Name        :  popimapprofiletesterBlocks.cpp
 *  Part of     :  ipsservices / profiletester 
 *  Description :: STIF test cases
 *  Version     : %version: 2 % << Don't touch! Updated by Synergy at check-out.
 *
 *  Copyright © 2010-2010 Nokia and/or its subsidiary(-ies).  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 * ============================================================================
 */

//--INCLUDE FILES-- 
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include <e32math.h>
#include <msvids.h>
#include "popimapprofiletester.h"

//const values
const TInt KMailboxStatusLen = 10;
const TInt KTimeUnits = 3;
const TInt KNumOfRec= 2;

_LIT( KPopImapProfileTester, "c:\\TestFramework\\TestFramework_ips.ini" );
_LIT( KStifEmailSubject, "STIF Basic Operations test" );
_LIT( KStifEmailSubjectDes,"STIF_");
_LIT( KStifEmailBodyText, "This is a temporary STIF test message");
_LIT( KStifEmailSubjectAttHandling, "STIF Attachment Handling Test" );
_LIT( KStifEmailToRec1, "dummyTo1@empty.com" );
_LIT( KStifEmailCcRec1, "dummyCc1@empty.com" );
_LIT( KStifEmailCcRec2, "dummyCc2@empty.com" );
_LIT( KStifEmailBccRec1, "dummyBcc1@empty.com" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPopImapProfileTester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::RunMethodL(CStifItemParser& aItem)
    {
    static TStifFunctionInfo const
            KFunctions[] =
                {
                        // Copy this line for every implemented function.
                        // First string is the function name used in TestScripter script file.
                        // Second is the actual implementation member function. 
                        ENTRY( "SetupAccount", CPopImapProfileTester::SetupAccountL ),
                        ENTRY( "GoOnline", CPopImapProfileTester::GoOnlineL ),
                        ENTRY( "GoOffline", CPopImapProfileTester::GoOfflineL ),
                        ENTRY( "RefreshNow", CPopImapProfileTester::RefreshNowL ),
                        ENTRY( "CancelSync", CPopImapProfileTester::CancelSyncL ),
                        ENTRY( "ListStandardFolders", CPopImapProfileTester::ListStandardFoldersL ),
                        ENTRY( "ListFolders", CPopImapProfileTester::ListFoldersL ),
                        ENTRY( "ListMessages", CPopImapProfileTester::ListMessagesL ),
                        ENTRY( "FetchMessages", CPopImapProfileTester::FetchMessagesL ),
                        ENTRY( "SendMessage", CPopImapProfileTester::SendMessageL ),
                        ENTRY( "BasicMsgOperations", CPopImapProfileTester::BasicMsgOperationsL ),
                        ENTRY( "CopyMoveMsgs", CPopImapProfileTester::CopyMoveMsgsL ),
                        ENTRY( "Search", CPopImapProfileTester::SearchL ),
                        ENTRY( "AttachmentHandling", CPopImapProfileTester::AttachmentHandlingL ),
                        ENTRY( "Branding", CPopImapProfileTester::BrandingL ),
                        ENTRY( "DeleteMsgs", CPopImapProfileTester::DeleteMsgsL ),
                        ENTRY( "RemoveAccount", CPopImapProfileTester::RemoveAccountL ),
                };

    const TInt count = sizeof(KFunctions) / sizeof(TStifFunctionInfo);

    InitMailboxL();
    return RunInternalL(KFunctions, count, aItem);
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester:: SetupAccountL
// Method used to create an imap or pop account, depends on .cfg file
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::SetupAccountL(CStifItemParser& /* aItem */)
    {
    //I.Check if mail client exists
    if (NULL == iMailClient)
        {
        iLog->Log(_L( "Error: Failed to create test instance(No Memory)." ));
        return KErrNoMemory;
        }
    TInt err(KErrNone);

    // II. Check if imap/pop profile already exists

    if (NULL != iIPSMailbox)
        {
        iLog->Log(_L("Error: There is IMAP/POP profile alredy existed"));
        return KErrAlreadyExists;
        }

    iLog->Log(_L( "== SetupAccount Begins ==" ));
    TAccountSetting settings;
    // III. Read settings and write to CR
    ReadAccountSettings(settings);
    err = WriteToWizardCRL(settings);
    iLog->Log(_L( "== WriteToWizardCR Ends err=%d" ), err);
    iLog->Log(_L( "== End a transaction in SetupAccount ==" ));

    // IV. Create mailbox through WizardDataAvailableL()
    err = iMailClient->WizardDataAvailableL();
    if (KErrNone == err)
	  {
        WaitForEvent(TFSEventNewMailbox, NULL, NULL, KTenSecondsTime);
	  }
    err = InitMailboxL();
    if ((KErrNone == err) && NULL != iIPSMailbox)
	  {
        err = KErrNone;
	  }
    else
	  {
        err = KErrGeneral;
	  }

    iLog->Log(_L( "== SetupAccount Ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester:: GoOnlineL
// Method used to let phone online
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::GoOnlineL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }

    iLog->Log(_L( "== GoOnline Begins ==" ));

    // check current mailbox status
    TBuf<KMailboxStatusLen> statusDes;
    TFSMailBoxStatus status = iIPSMailbox->GetMailBoxStatus();
    if (status == EFSMailBoxOffline)
        {
        statusDes.Copy(_L("offline"));
        }
    else if (status == EFSMailBoxOnline)
        {
        statusDes.Copy(_L("online"));
        iLog->Log(_L("Mailbox has been Online, go offline firstly"));
        iLog->Log(_L( "== GoOnline ends err=%d" ), err);
        return KErrGeneral;
        }
    else
        {
        statusDes.Copy(_L("unknown"));
        }

    iLog->Log(_L("Going online (from %S)..."), &statusDes);
    TSSMailSyncState syncstatus = iIPSMailbox->CurrentSyncState();
    // call function to go online
    iEventSynchronousCall = ETrue;
    iIPSMailbox->GoOnlineL();
    syncstatus = iIPSMailbox->CurrentSyncState();
    // we should likely base on TFSEventMailboxOnline, but this does not come always (?)
    err = WaitForEvent(TFSEventMailboxOnline, NULL, NULL, KOneMinuteTime);
    
	if( KErrNone == err)
	{
     syncstatus = iIPSMailbox->CurrentSyncState();

     //check status of current mailbox again
     status = iIPSMailbox->GetMailBoxStatus();

     if (status == EFSMailBoxOnline)
        {
        err = KErrNone;
        iLog->Log(_L("Success: mailbox online"));
        }
     else
        {
        err = KErrGeneral;
        iLog->Log(_L("Failed: mailbox status- %S"), &statusDes);
        }
	} //end if 'KErrNone == err'
	
    iLog->Log(_L( "== GoOnline ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester:: GoOfflineL
// Method used to let phone offline
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::GoOfflineL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }

    iLog->Log(_L( "== GoOffline Begins ==" ));
    TBuf<KMailboxStatusLen> statusDes;
    TFSMailBoxStatus status = iIPSMailbox->GetMailBoxStatus();
    if (status == EFSMailBoxOffline)
        {
        statusDes.Copy(_L("offline"));
        iLog->Log(_L("Mailbox has been Offline, go online firstly"));
        iLog->Log(_L( "== GoOffline ends err=%d" ), err);
        return KErrGeneral;
        }
    else if (status == EFSMailBoxOnline)
        {
        statusDes.Copy(_L("online"));
        }
    else
        {
        statusDes.Copy(_L("unknown"));
        }

    iLog->Log(_L("Going offline (from %S)..."), &statusDes);

    // call function to go offline
    iEventSynchronousCall = ETrue;
    TSSMailSyncState syncstatus = iIPSMailbox->CurrentSyncState();
    iIPSMailbox->GoOfflineL();
    syncstatus = iIPSMailbox->CurrentSyncState();
    err = WaitForEvent(TFSEventMailboxOffline, NULL, NULL, KOneMinuteTime);
	if( KErrNone == err)
	{
     syncstatus = iIPSMailbox->CurrentSyncState();
     //check status of current mailbox again
     status = iIPSMailbox->GetMailBoxStatus();

     if (status == EFSMailBoxOffline)
        {
        iLog->Log(_L("Success: mailbox offline"));
        }
     else
        {
        err = KErrGeneral;
        iLog->Log(_L("Failed: mailbox status- %S"), &statusDes);
        }
    } //end if 'KErrNone == err'
    iLog->Log(_L( "GoOffline ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester:: RefreshNowL
// Method used to let phone refresh
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::RefreshNowL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }
    iLog->Log(_L( "RefreshNow begins" ));
    TInt reqId = iIPSMailbox->RefreshNowL(*this);
    iLog->Log(_L("Request Id -> %d"), reqId);
    TSSMailSyncState syncstatus = iIPSMailbox->CurrentSyncState();
    if ((StartingSync == syncstatus) || (EmailSyncing == syncstatus))
	  {
        err = WaitForEvent(TFSEventMailboxOnline);
	  }
    else
	  {
        err = KErrGeneral;
	  }
    if (err == KErrNone)
        {
        WaitForResponse(TFSProgress::EFSStatus_RequestComplete, KOneMinuteTime*KTimeUnits);
        syncstatus = iIPSMailbox->CurrentSyncState();
        }
    iLog->Log(_L( "RefreshNow ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester:: CancelSyncL
// Method used to cancle action of Sync.
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::CancelSyncL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }
    iLog->Log(_L( "CancelSync begins" ));
    iIPSMailbox->RefreshNowL(*this);
    err = WaitForEvent(TFSEventMailboxOnline);
    if (KErrNone == err)
        {
        iIPSMailbox->CancelSyncL();
        err = WaitForEvent(TFSEventMailboxOffline);
        TSSMailSyncState syncstatus = iIPSMailbox->CurrentSyncState();
        if ((err != KErrNone) || (Idle != syncstatus))
        	{
            err = KErrGeneral;
        	}
        }
    iLog->Log(_L( "CancelSync ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::ListStandardFoldersL
// Method used to list standard folders
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::ListStandardFoldersL(CStifItemParser& /* aItem */)
    {
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }

    TInt err(KErrNone);
    iLog->Log(_L( "ListStandardFolders begins" ));

    // list all root folders
    RPointerArray<CFSMailFolder> rootFolders;
    iIPSMailbox->ListFolders(TFSMailMsgId(), rootFolders);

    // how many root folders we have
    TInt rootFolderCount = rootFolders.Count();

    /* Standard folders are:
     * EFSInbox, EFSOutbox, EFSDraftsFolder, EFSSentFolder, EFSDeleted
     * IMAP/POP may not have 'EFSDeleted'? Four only?
     */

    // check if all standard folders are present among root folders
    TFSMailMsgId folderId = TFSMailMsgId();
    iLog->Log(_L("== List of Standard Folders == "));
    for (TInt i = 1; i < EFSDeleted; ++i)
        {
        folderId = iIPSMailbox->GetStandardFolderId((TFSFolderType) i);
        TBool folderPresent = EFalse;
        for (TInt ii = 0; ii < rootFolderCount; ++ii)
            {
            if (rootFolders[ii]->GetFolderId() == folderId)
                {
                iLog->Log(_L("   %S"), &rootFolders[ii]->GetFolderName());
                folderPresent = ETrue;
                break;
                }
            }
        if (NULL == folderPresent)
            {
            iLog->Log(_L("  Error: folder of id %d is missing"), i);
            err = KErrNotFound;
            }
        }
    rootFolders.ResetAndDestroy();

    iLog->Log(_L( "ListStandardFolders ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::ListFoldersL
// Method used to list all folders
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::ListFoldersL(CStifItemParser& aItem)
    {
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }
    /**
     * 1. Check subfolders of given folder - use ListFolders(FSMailMsgId, RPointerArray<CFSMailFolder>&)
     * 2. List all folders and compare check cout of listed folders - use ListFolders()
     * 
     */
    TInt paramErr(KErrNone);
    TInt err(KErrNone);
    iLog->Log(_L( "ListFoldersL begins" ));
    iLog->Log(_L( "Mailbox's Id: %x"), iIPSMailbox->GetId().Id());

    RPointerArray<CFSMailFolder>& folders = iIPSMailbox->ListFolders();
    //.. 
    // 1. subfolders check
    TPtrC parentFolderS, subfolderSetS, subfolderS;

    //get parent folder name  
    paramErr = aItem.GetString(_L("_parentFolder:"), parentFolderS);
    TFSMailMsgId parentFolderId = GetFolderIdFromName(parentFolderS);

    if (!parentFolderId.IsNullId())
        {
        iLog->Log(_L("Testing folder hierachy"));
        iLog->Log(_L("Parent folder found; id: %x"), parentFolderId.Id());

        RPointerArray<CFSMailFolder> subfolders;
        iIPSMailbox->ListFolders(parentFolderId, subfolders);

        TInt subfolderErr(KErrNone);
        //there should be at least one subfolder defined  
        paramErr &= aItem.GetString(_L("_subfolders:"), subfolderSetS);

        TInt idx(0);
        do
            {
            idx = subfolderSetS.Locate(',');
            if (idx > 0)
                {
                // extract next subfolder
                subfolderS.Set(subfolderSetS.Left(idx));
                // skip current subfolder
                subfolderSetS.Set(subfolderSetS.Mid(idx + 1));
                }
            else
                {
                subfolderS.Set(subfolderSetS);
                }

            // check subfolder 
            subfolderErr = KErrNotFound;
            for (TInt i = 0; (i < folders.Count() && !paramErr); i++)
                {
                if (folders[i]->GetFolderName().Compare(subfolderS) == 0)
                    {
                    subfolderErr = KErrNone;
                    iLog->Log(
                            _L("Subfolder %S of parent folder %S found; id: %x"),
                            &(folders[i]->GetFolderName()), &parentFolderS,
                            folders[i]->GetFolderId().Id());
                    break;
                    }
                }

            }
        while (!subfolderErr && idx >= 0);

        //set global err value - zeroes err if OK 
        err &= subfolderErr;
        subfolders.ResetAndDestroy();
        }//end if '!parentFolderId.IsNullId()'

    //parse params - we have to know what actualy we are testing
    TInt foldersCount(0);
    paramErr &= aItem.GetInt(_L("_expectedFoldersCount:"), foldersCount);
    //.. 
    // 2. total number of folders
    if (foldersCount > 0)
        {
        (folders.Count() == foldersCount) ? err = KErrNone : err = KErrGeneral;
        iLog->Log(_L("Actual folders count: %d vs. %d expected"),
                folders.Count(), foldersCount);
        }
    //..

    if (paramErr != 0)
        {
        iLog->Log(_L("  Error: Invalid test case parameters!"));
        }

    iLog->Log(_L( "ListFolders ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::ListMessagesL
// Method used to list messages in a specific folder
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::ListMessagesL(CStifItemParser& aItem)
    {
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }

    TInt err(KErrNone);
    iLog->Log(_L( "ListMessagesL begins" ));

    // I. get test case params
    TPtrC folderName;
    TInt msgCount(0);

    // since folder name might consists of more than one word
    // lets use quote parsing for param
    aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
    if (aItem.GetString(_L("_folder:"), folderName))
        {
        iLog->Log(_L("Error: test case argument is missing"));
        err = KErrArgument;
        }
    else
        {
        // II. Get folder of given name and list its emails
        CFSMailFolder* folder = FindFolder(folderName);
        if (NULL != folder)
            {
            RPointerArray<CFSMailMessage> messages;
            TInt gotMsgs = GetMessagesFromFolderL(messages, folder,
                    EFSMsgDataSubject, EFSMailSortByDate, msgCount);
            if (gotMsgs > 0)
                {
                iLog->Log(_L("  Listing first %d emails:"), gotMsgs);
                }
			TInt messagesCount = messages.Count();
            for (TInt i = 0; i < messagesCount; ++i)
                {
                iLog->Log(_L("    %d. %S"), i + 1, &messages[i]->GetSubject());
                }
            if (gotMsgs != messages.Count())
                { // warning, NextL() returned fewer entries than expected
                TInt diff = gotMsgs > messages.Count() ? gotMsgs
                        - messages.Count() : messages.Count() - gotMsgs;
                iLog->Log(_L("Warning: %d entries are still not be listed."),
                        diff);
                err = KErrNotFound;
                }
            messages.ResetAndDestroy();
            }//end if 'folder'
        else
            {
            iLog->Log(_L("Error: %S folder not found"), &folderName);
            err = KErrNotFound;
            }
        }//end if 'folderName'

    iLog->Log(_L( "ListMessages ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::FetchMessagesL
// Method used to fetch messages in a specific folder
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::FetchMessagesL(CStifItemParser& aItem)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrGeneral;
        }
    iLog->Log(_L( "FetchMessages begins" ));

    /**
     * Params:
     * - folder name; Inbox if not provided 
     */
    TFSMailMsgId msgFolderId;
    TPtrC folderS;

    //get folder name  
    if (aItem.GetString(_L("_messageFolder:"), folderS) == KErrNone)
        {
        RPointerArray<CFSMailFolder>& folders = iIPSMailbox->ListFolders();
        // find and get folder id
		TInt foldersCount = folders.Count();
        for (TInt i = 0; i < foldersCount; i++)
            {
            if (folders[i]->GetFolderName().Compare(folderS) == 0)
                {
                msgFolderId = folders[i]->GetFolderId();
                iLog->Log(_L("Message will be fetched from %S folder"),
                        &folderS);
                break;
                }
            }
        }
    if (msgFolderId.IsNullId())
        {
        iLog->Log(
                _L("Proper message folder not provided, Inbox folder will be used instead"));
        msgFolderId = iIPSMailbox->GetStandardFolderId(EFSInbox);
        }
    CFSMailFolder* folder = NULL;
    // get message folder
    TRAPD(trapErr, folder = iMailClient->GetFolderByUidL(iIPSMailbox->GetId(),msgFolderId));
    CleanupStack::PushL(folder);

    if (KErrNone == trapErr)
        {
        //to get messages from folder
        RPointerArray<CFSMailMessage> allMessages;
        GetMessagesFromFolderL(allMessages, folder, EFSMsgDataIdOnly,
                EFSMailSortByDate, folder->GetMessageCount());

        if (allMessages.Count() > 0)
            {
            RArray<TFSMailMsgId> messages;
            CleanupClosePushL(messages);
            messages.Append(allMessages[0]->GetMessageId());

            // not used in function
            TFSMailDetails ruleToFetch = EFSMsgDataEnvelope;

            TRAP(err,folder->FetchMessagesL(messages,ruleToFetch,*this));
            WaitForResponse(TFSProgress::EFSStatus_RequestComplete,
                    KOneMinuteTime);
            if (KErrNone == err)
                {
                iLog->Log(_L("Messages fetched successfuly"));
                }
            else
                {
                iLog->Log(_L("Messages fetching failed"));
                }
            CleanupStack::PopAndDestroy(&messages);
            }//end if 'allMessages'
        else
            {
            iLog->Log(_L("No message in %S folder"), &folder->GetFolderName());
            err = KErrNotFound;
            }
        allMessages.ResetAndDestroy();
        }//end 'trapErr'
    else
        {
        iLog->Log(_L("Message folder could not be created"));
        }
    CleanupStack::PopAndDestroy(folder);

    iLog->Log(_L( "FetchMessages ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::SendMessagesL
// Method used to send messages
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::SendMessageL(CStifItemParser& /* aItem*/)
    {
    TInt err(KErrNone);
    TBuf<20> buf;
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }
    iLog->Log(_L( "SendMessageL begins" ));

    // Step01: Create a message subject
    buf.Append(KStifEmailSubjectDes);
    buf.AppendNum(Math::Random());
    TPtrC subject = buf.Mid(0);
    iLog->Log(_L("Message is to be sent(Subject:%S)"), &subject);

    // Step02: Create default msg
    CFSMailMessage* newMsg = CreatePlainTextMsgL(subject, KStifEmailBodyText());

    //Step03: To send message and check it
    if (NULL != newMsg)
        {
        iLog->Log(_L("Tested message with subject '%S' was created"), &subject);
        CleanupStack::PushL(newMsg);
        TFSMailMsgId newMsgId = TFSMailMsgId();
        iLog->Log(_L("Message is to be sent..."));
        err = SendMsgL(*newMsg, subject, newMsgId); //sending message..

        if (KErrNone == err)
		  {
            iLog->Log(_L("Message was sent successfully"));
		  }
        else
		  {
            iLog->Log(_L("Failed to send message with error (ID:%d)"), err);
		  }
        CleanupStack::PopAndDestroy(newMsg);
        }
    else
        {
        iLog->Log(_L("Error: Tested message was NOT created"));
        err = KErrNoMemory;
        }
    iLog->Log(_L( "SendMessageL ends: %d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::BasicMsgOperationsL
// Method used to test basic operations of messages
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::BasicMsgOperationsL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }

    iLog->Log(_L( "BasicMsgOperationsL begins" ));
    // I. Prepare test message to send, using basic operations to set msg details
    CFSMailMessage* newMsg = CreatePlainTextMsgL(KStifEmailSubject,
            KStifEmailBodyText);
    CleanupStack::PushL(newMsg);

    // update msg details, add one more recipient in 'To' field (first is own mail address)
    CFSMailAddress* rec = CFSMailAddress::NewL();
    rec->SetEmailAddress(KStifEmailToRec1);
    newMsg->AppendToRecipient(rec);

    // add some recipients to 'Cc' filed
    rec = CFSMailAddress::NewL();
    rec->SetEmailAddress(KStifEmailCcRec1);
    newMsg->AppendCCRecipient(rec);

    rec = CFSMailAddress::NewL();
    rec->SetEmailAddress(KStifEmailCcRec2);
    newMsg->AppendCCRecipient(rec);

    // add one recipient in 'Bcc' filed
    rec = CFSMailAddress::NewL();
    rec->SetEmailAddress(KStifEmailBccRec1);
    newMsg->AppendBCCRecipient(rec);
    rec = NULL;

    // set low priority flag
    newMsg->SetFlag(EFSMsgFlag_Low);

    // save all changes
    newMsg->SaveMessageL();

    // II. Send test message
    TFSMailMsgId newMsgId = TFSMailMsgId();
    err = SendMsgL(*newMsg, KStifEmailSubject, newMsgId);
	iLog->Log(_L("'SendMsgL' returns value of err=%d"),err);
    CleanupStack::PopAndDestroy(newMsg);
    newMsg = NULL;
    TFSMailMsgId msgFolderId = iIPSMailbox->GetStandardFolderId(EFSInbox);
    CFSMailFolder* pFolder = NULL;
    // get message folder
    TRAPD(trapErr, pFolder = iMailClient->GetFolderByUidL(iIPSMailbox->GetId(),msgFolderId));
    CleanupStack::PushL(pFolder);

    if (KErrNone == trapErr)
        {
        TFSMailDetails dymmy = EFSMsgDataEnvelope;
        RArray<TFSMailMsgId> messages;
        CleanupClosePushL(messages);
        messages.Append(newMsgId);
        TRAP(err,pFolder->FetchMessagesL(messages,dymmy,*this));
        err = WaitForResponse(TFSProgress::EFSStatus_RequestComplete,
                KOneMinuteTime);
        if (KErrNone == err)
            {
            iLog->Log(_L("Messages fetched successfuly"));
            }
        else
            {
            iLog->Log(_L("Messages fetching failed"));
            }
        CleanupStack::PopAndDestroy(&messages);
        }
    CleanupStack::PopAndDestroy(pFolder);

    // III. Check message details, using basic operations to read msg details
    if (KErrNone == err)
        {
        // Test message successfully received
        // pass null id, not used by BasePlugin anyway
        newMsg = iMailClient->GetMessageByUidL(iIPSMailbox->GetId(),
                TFSMailMsgId(), newMsgId, EFSMsgDataEnvelope);
        if (NULL != newMsg)
            {
            CleanupStack::PushL(newMsg);
            iLog->Log(_L("   Checking message details:"));

            // check sender
            CFSMailAddress* sender = newMsg->GetSender();
            TPtrC senderAddr = sender->GetEmailAddress();
            if (senderAddr.Compare(
                    iIPSMailbox->OwnMailAddress().GetEmailAddress()))
                {
                iLog->Log(_L("   Error: Sender '%S' does not match"),
                        &senderAddr);
                err = KErrGeneral;
                }

            // check subject
            TPtrC subject = newMsg->GetSubject();
            if (subject.Compare(KStifEmailSubject()))
                {
                iLog->Log(_L("   Error: Subject '%S' does not match"), &subject);
                err = KErrGeneral;
                }

            // check To recipients
            RPointerArray<CFSMailAddress>& toRecipients =
                    newMsg->GetToRecipients();
            TInt toRec = toRecipients.Count();
            if (toRec != KNumOfRec)
                {
                iLog->Log(_L("   Error: Got %d 'To' recipients, expected 2"),
                        toRec);
                err = KErrGeneral;
                }
            else
                {
                TPtrC recMail = toRecipients[0]->GetEmailAddress();
                if (recMail.Compare(
                        iIPSMailbox->OwnMailAddress().GetEmailAddress()))
                    {
                    iLog->Log(_L("   Error: 1. 'To' recipient does not match"));
                    err = KErrGeneral;
                    }
                recMail.Set(toRecipients[1]->GetEmailAddress());
                if (recMail.Compare(KStifEmailToRec1))
                    {
                    iLog->Log(_L("   Error: 2. 'To' recipient does not match"));
                    err = KErrGeneral;
                    }
                }
            toRecipients.ResetAndDestroy();
            // check Cc recipients
            RPointerArray<CFSMailAddress>& ccRecipients =
                    newMsg->GetCCRecipients();
            TInt ccRec = ccRecipients.Count();
            if (ccRec != KNumOfRec)
                {
                iLog->Log(_L("   Error: Got %d 'Cc' recipients, expected 2"),
                        ccRec);
                err = KErrGeneral;
                }
            else
                {
                TPtrC recMail = ccRecipients[0]->GetEmailAddress();
                if (recMail.Compare(KStifEmailCcRec1))
                    {
                    iLog->Log(_L("   Error: 1. 'Cc' recipient does not match"));
                    err = KErrGeneral;
                    }
                recMail.Set(ccRecipients[1]->GetEmailAddress());
                if (recMail.Compare(KStifEmailCcRec2))
                    {
                    iLog->Log(_L("   Error: 2. 'Cc' recipient does not match"));
                    err = KErrGeneral;
                    }
                }
            ccRecipients.ResetAndDestroy();
            // check Bcc recipients, should be 0
            RPointerArray<CFSMailAddress>& bccRecipients =
                    newMsg->GetBCCRecipients();
            TInt bccRec = bccRecipients.Count();
            if (bccRec != 0)
                {
                iLog->Log(_L("   Error: Got %d 'Bcc' recipients, expected 0"),
                        bccRec);
                err = KErrGeneral;
                }

            // check priority
            if (!(newMsg->GetFlags() & EFSMsgFlag_Low))
                {
                iLog->Log(_L("   Error: Priority not set to low" ));
                err = KErrGeneral;
                }

            // check msg's mailboxId
            if (newMsg->GetMailBoxId() != iIPSMailbox->GetId())
                {
                iLog->Log(_L("  Error: Msg's mailboxId not correct"));
                err = KErrGeneral;
                }

            // check test msg's folderId
            if (newMsg->GetFolderId() != iIPSMailbox->GetStandardFolderId(
                    EFSInbox))
                {
                iLog->Log(_L("  Error: Msg's folderId not correct"));
                err = KErrGeneral;
                }
            if (!err)
                {
                iLog->Log(_L("   Message details all ok"));
                }
            bccRecipients.ResetAndDestroy();
            CleanupStack::PopAndDestroy(newMsg);
            }
        }
    else
        {
        // some problems with sending or receiving came up
        iLog->Log(_L("  Error: Sending/Receiving failed"));
        }

    iLog->Log(_L( "BasicMsgOperationsL ends: %d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::CopyMoveMsgsL
// Method used to copy and move messages from 'Inbox' to a specific folder
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::CopyMoveMsgsL(CStifItemParser& /* aItem */)
    {
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }
    iLog->Log(_L( "CopyMoveMsgs begins" ));
    TInt err(KErrNone);
    TInt errMove(KErrNone);

    if (iPluginId.PluginId() == TUid::Uid(KIPSSosPop3PluginUid))
        {
        //POP plugin not support Move And Copy message functionality.
        iLog->Log(
                _L( "POP plugin not support Move And Copy message functionality." ));
        iLog->Log(_L( "CopyMoveMsgs ends err=%d(Move)" ), errMove);
        return err;
        }

    TFSMailMsgId newMsgId = TFSMailMsgId();
    TFSMailMsgId inboxFolderId = iIPSMailbox->GetStandardFolderId(EFSInbox);
    TFSMailMsgId draftsFolderId = iIPSMailbox->GetStandardFolderId(
            EFSDraftsFolder);
    TFSMailMsgId sentFolderId = iIPSMailbox->GetStandardFolderId(EFSSentFolder);

    CFSMailFolder* folder = iMailClient->GetFolderByUidL(iIPSMailbox->GetId(),
            inboxFolderId);

    if (NULL != folder)
        {
        CleanupStack::PushL(folder);
        RPointerArray<CFSMailMessage> messages;
        TInt msgCount(0);
        TInt gotMsgs = GetMessagesFromFolderL(messages, folder,
                EFSMsgDataSubject, EFSMailSortByDate, msgCount);
        RArray<TFSMailMsgId> originalArray;
        CleanupClosePushL(originalArray);
        /*
         * CIpsPlgSosBasePlugin::CopyMessagesL() is empty.
         * So, it can't test this function
         */
        //Moving tested messages from 'Inbox' to 'Sent Items'
        if (gotMsgs > 0)
            {
            iLog->Log(_L("  Listing %d messages:"), gotMsgs);
			TInt messagesCount = messages.Count();
            for (TInt i = 0; i < messagesCount; i++)
                {
                iLog->Log(_L("%d.ID:,Subject: %S"), i + 1,
                        &messages[i]->GetSubject());
                originalArray.Append(messages[i]->GetMessageId());
                }//end loop: 'for'
            // Note: Messages cannot be moved from Deleted Items, Drafts, Outbox folders

            //Different between 'IMAP' and  'POP'
            iLog->Log(_L("IMAP: Moving from 'Inbox' to 'Sent'..."));
            iIPSMailbox->MoveMessagesL(originalArray, inboxFolderId,
                    sentFolderId);
            originalArray.Reset();

            // there is no TFSEventMailMoved or TFSEventMailCopied event coming, just TFSEventMailDeleted?
            errMove = WaitForEvent(TFSEventMailDeleted);
            }// end of 'gotMsgs > 0'
        else
            {
            iLog->Log(_L("No message in %S folder"), &folder->GetFolderName());
            errMove = KErrNotFound;
            }
        if (KErrNone == errMove)
            {
            iLog->Log(_L("Successfully move messages."));
            }
        else
            {
            iLog->Log(_L("Failed to move with error(ID:%d)"), errMove);
            err = errMove;
            }
        CleanupStack::PopAndDestroy(&originalArray);
        messages.ResetAndDestroy();
        CleanupStack::PopAndDestroy(folder);
        }//end if: 'NULL!=Folder'
    else
        {
        iLog->Log(_L(" Folder is NOT found.(Error's ID:%d)"), err);
        }
    iLog->Log(_L( "CopyMoveMsgs ends err=%d(Move)" ), errMove);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::SearchL
// Method used to test searching functionality
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::SearchL(CStifItemParser& /* aItem */)
    {
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }

    TInt err(KErrNone);
    iLog->Log(_L( "Search begins" ));

    // Step01: to prepare testing msg
    HBufC* bufSubject = HBufC::NewL(KMidBufLen);
    HBufC* bufBody = HBufC::NewL(KMidBufLen);
    TPtr subject = bufSubject->Des();
    TPtr body = bufBody->Des();
    subject.Append(KStifEmailSubjectDes);
    body.Append(KStifEmailBodyText);

    TUint randomNum = Math::Random();
    subject.AppendNum(randomNum);
    body.Append(_L("-"));
    body.AppendNum(randomNum);

    iLog->Log(_L("==Testing Email=="));
    iLog->Log(_L("=Subject is:%S"), &subject);
    iLog->Log(_L("=Body is:%S"), &body);

    // Step02: Send testing msg to own mail address
    CFSMailMessage* newMsg = CreatePlainTextMsgL(subject, body);
    if (NULL != newMsg)
        {
        CleanupStack::PushL(newMsg);
        TFSMailMsgId newMsgId = TFSMailMsgId();
        err = SendMsgL(*newMsg, subject, newMsgId);
        if (KErrNone == err)
            {
            // Step03: to setup searchL params
            TFSMailSortCriteria criteria;
            criteria.iField = EFSMailSortByDate;
            criteria.iOrder = EFSMailDescending;
            RPointerArray<TDesC> searchStrings;
            // Step04: to perform 'Subject' search
            searchStrings.Append(bufSubject);
            err = DoSearch(searchStrings, criteria, KOneMinuteTime);

            // Check results for subject Search
            if (KErrNone == err && iSearchMatches > 0)
                {
                iLog->Log(_L("Success: Subject search passed"));
                }
            else
                {
                iLog->Log(_L("Failed: 'Subject' search with error(%d)"), err);
                err = KErrNotFound;
                }

            // Step05: to perform 'Body' search
            searchStrings.ResetAndDestroy();
            searchStrings.Append(bufBody);

            //Check results for body Search
            err = DoSearch(searchStrings, criteria, KOneMinuteTime);
            if (KErrNone == err && iSearchMatches > 0)
                {
                iLog->Log(_L("Success:Body search passed"));
                }
            else
                {
                iLog->Log(_L("Failed to 'Body' search with error(%d)"), err);
                err = KErrNotFound;
                }
            searchStrings.ResetAndDestroy();
            }//end if 'KErrNone == err'(sent 'msg' with KErrNone)
        else
            {
            iLog->Log(_L(" Failed to sent message with error(ID:%d)"), err);
            }
        CleanupStack::PopAndDestroy(newMsg);
        }//end if 'NULL != newMsg'
    else
        {
        iLog->Log(_L("Error: tested message was NOT created"));
        err = KErrGeneral;
        }
    iLog->Log(_L( "Search ends err=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::AttachmentHandlingL
// Method used to test functionality of hanlding attachments
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::AttachmentHandlingL(CStifItemParser& /* aItem */)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }

    iLog->Log(_L( "AttachmentHandlingL begins" ));

    // Create default msg in 'Drafts' folder
    CFSMailMessage* newMsg = CreatePlainTextMsgL(KStifEmailSubjectAttHandling,
            KStifEmailBodyText);

    if (NULL != newMsg)
        {
        CleanupStack::PushL(newMsg);
        iLog->Log(_L("Test Email created"));
        TFSMailMsgId dummyId = TFSMailMsgId();

        //add attachment
        TFSMailMsgId aInsertBefore;
        CFSMailMessagePart* msgPart = NULL;
        TRAP(err,msgPart = newMsg->AddNewAttachmentL(KPopImapProfileTester,aInsertBefore));
        CleanupStack::PushL(msgPart);

        if (KErrNone != err)
            {
            iLog->Log(_L("Adding attachment failed. Error: %d"), err);
            }
        else
            {
            iLog->Log(_L("New attachment added."));
            newMsg->SaveMessageL();

            //check if attachment was added
            RPointerArray<CFSMailMessagePart> attParts;
            TRAP( err,newMsg->AttachmentListL(attParts) );

            if (KErrNone != err)
                {
                iLog->Log(_L("Unable to take attachment list. Error %d"), err);
                }
            else
                {
                if (attParts.Count() > 0)
                    {
                    TPtrC filename(KPopImapProfileTester);
                    filename.Set(filename.Mid(filename.LocateReverse('\\') + 1));
                    err = attParts[0]->AttachmentNameL().Compare(filename);
                    iLog->Log(
                            _L("Attachment's names comparison: %S (attached file name) vs.  %S (expected). Result: %d"),
                            &(attParts[0]->AttachmentNameL()), &filename, err);
                    }
                }//end if 'KErrNone!=err'
            attParts.ResetAndDestroy();
            }//end if 'KErrNone!=err'
        CleanupStack::PopAndDestroy(msgPart);
        CleanupStack::PopAndDestroy(newMsg);
        }//end if 'newMsg'
    else
        {
        iLog->Log(_L("Error: Test Email not created"));
        err = KErrNoMemory;
        }

    iLog->Log(_L( "AttachmentHandlingL ends: %d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::BrandingL
// Method used to test functionality of branding
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::BrandingL(CStifItemParser& aItem)
    {
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L("Error: IMAP/POP profile does NOT exist"));
        return KErrNotFound;
        }

    iLog->Log(_L( "== Branding begins ==" ));

    // get brand manager
    MFSMailBrandManager* brandManager = NULL;

    TRAP( err, brandManager = &iMailClient->GetBrandManagerL() );

    //to check brand manager
    if (KErrNone != err)
        {
        iLog->Log(
                _L("Error: CFSMailClient::GetBrandManagerL() leaved with %d"),
                err);
        }
    else if (NULL == brandManager)
        {
        iLog->Log(_L("Error: CFSMailClient::GetBrandManagerL() returns NULL"));
        err = KErrGeneral;
        }
    else
        {
        // test updating mailbox names with real mailbox id
        TRAP( err, brandManager->UpdateMailboxNamesL( iIPSMailbox->GetId() ) );
        if (KErrNone != err)
            {
            iLog->Log(
                    _L("Error: MFSMailBrandManager::UpdateMailboxNamesL() leaved with %d"),
                    err);
            }
        else
            {
            // get param regarding expected branding status 
            TBool brand = EFalse;
            TPtrC expectBrandS;
            aItem.GetString(_L("_isBranded:"), expectBrandS);

            if (expectBrandS.Compare(_L("true")) == 0)
                {
                iLog->Log(_L("Branding expected..."));
                brand = ETrue;
                }
            else
                {
                iLog->Log(_L("Branding not expected..."));
                }

            // check mailbox name branding
            TPtrC textBrand;
            TRAP( err, textBrand.Set( brandManager->GetTextL( EFSMailboxName, iIPSMailbox->GetId() ) ) );
            if (KErrNone != err)
                {
                iLog->Log(
                        _L("Error: MFSMailBrandManager::GetTextL() leaved with %d"),
                        err);
                }
            TBool textBranded = textBrand.Length() != 0 ? ETrue : EFalse;
            if (textBranded)
                {
                iLog->Log(_L("Mailbox's name (%S) branded."), &textBrand);
                }
            else
                {
                iLog->Log(_L("Mailbox's name (%S) was not branded."),
                        &textBrand);
                }
            //to check whether it is expected?
            if (brand == textBranded)
                {
                iLog->Log(_L("Successfully test, it was a expected retult."));
                err = KErrNone;
                }
            else
                {
                iLog->Log(_L("Failed to test, it was NOT a expected retult."));
                err = KErrGeneral;
                }
            } // end if 'UpdateMailboxNames'
        } //end if 'BrandManager'

    iLog->Log(_L( "Branding ends ret=%d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::DeleteMsgsL
// Method used to delete messages
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::DeleteMsgsL(CStifItemParser& aItem)
    {
    TInt err(KErrNone);
    iLog->Log(_L( "DeleteMessagesL begins" ));

    // Step 01: To refresh mailbox
    iLog->Log(_L(" Step 01: To synchronize mailbox"));
    TInt reqId = iIPSMailbox->RefreshNowL(*this);
    err = WaitForEvent(TFSEventMailboxOnline);
    WaitForResponse(TFSProgress::EFSStatus_RequestComplete, KOneMinuteTime);
    if (KErrNone == err)
        {
        // Step 02: To delete tested messages
        iLog->Log(_L(" Step 02: To delete tested messages"));
        TPtrC folderName;
        // We intended to delete messages in a folder only. Here, it is 'Inbox' folder. 
        aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
        if (aItem.GetString(_L("_folder:"), folderName))
            {
            iLog->Log(_L("Error: test case argument is missing"));
            err = KErrArgument;
            }
        else
            err = DeleteTestMsgL(folderName);

        if (KErrNone == err)
            {
            //Step 03: To check whether tested messages were sucessfully deleted
            iLog->Log(_L(" Step 03: To tested if messages really deleted"));
            err = DeleteTestMsgL(folderName);

            if (err == KErrNotFound)
                {
                iLog->Log(_L("Succesfully deleted tested messages"));
                err = KErrNone;
                }
            else
                {
                iLog->Log(_L("Failed to delete tested messages"));
                err = KErrGeneral;
                }
            }//end if 'KErrNone == err'
        else
            {
            iLog->Log(_L(" Warning: No tested messages to delete"));
            }
        }//end outer if
    else
        {
        iLog->Log(_L(" Warning: synchronize mailbox failed!"));
        err = KErrGeneral;
        }
    iLog->Log(_L( "DeleteMessagesL ends: %d" ), err);
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::RemoveAccountL
// Method used to remove an imap or pop account
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::RemoveAccountL(CStifItemParser& /* aItem */)
    {
    iLog->Log(_L( "RemoveAccount begins" ));
    TInt err(KErrNone);
    if (NULL == iIPSMailbox)
        {
        iLog->Log(_L( " Error: IMAP/POP profile does NOT exist" ));
        err = KErrNotFound;
        }
    else
        {
        iLog->Log(_L( "Mailbox Id: %x"), iIPSMailbox->GetId().Id());
        // start test case
        TInt reqId = iMailClient->DeleteMailBoxByUidL(iIPSMailbox->GetId(),
                *this);
        // end test case
        iLog->Log(_L("-->ReqResp: %d"), reqId);

        // Check if mailbox is really deleted
        RPointerArray<CFSMailBox> mailboxes;
        iMailClient->ListMailBoxes(iIPSMailbox->GetId(), mailboxes);
        if (mailboxes.Count() > 0)
            {
            iLog->Log(_L( " Failed: Mailbox was not deleted" ));
            err = KErrGeneral;
            }
        else
            {
            iLog->Log(_L( " Success: Mailbox was deleted" ));
            iMailClient->UnsubscribeMailboxEvents(iIPSMailbox->GetId(), *this);
            DELANDNULL(iIPSMailbox);
            err = KErrNone;
            }
        mailboxes.ResetAndDestroy();
        }//end if 'count'

    iLog->Log(_L( "RemoveAccount ends err=%d" ), err);
    return err;
    }

//  End of File
