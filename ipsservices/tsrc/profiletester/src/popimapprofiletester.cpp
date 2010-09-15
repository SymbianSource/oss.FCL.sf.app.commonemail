/* ============================================================================
 *  Name        :  popimapprofiletester.cpp
 *  Part of     :  ipsservices / ipsprofiletester 
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

// INCLUDE FILES
#include <StifTestInterface.h>
#include "popimapprofiletester.h"
#include <SettingServerClient.h>
/*
 * define flag in ctg file
 */
_LIT( KSetupWizAccountType,"AccountType" );
_LIT( KSetupWizAccountsConfigured,"AccountsConfigured" );
_LIT( KSetupWizMaxAccountsReached,"MaxAccountsReached" );
_LIT( KSetupWizEmailAddress,"EmailAddress" );
_LIT( KSetupWizUsername,"Username" );
_LIT( KSetupWizPassword,"Password" );
_LIT( KSetupWizMailboxName,"MailboxName" );
_LIT( KSetupWizIncomingServer,"IncomingServer" );
_LIT( KSetupWizIncomingPort,"IncomingPort" );
_LIT( KSetupWizOutgoingServer,"OutgoingServer" );
_LIT( KSetupWizOutgoingPort,"OutgoingPort" );
_LIT( KSetupWizIncomingSecurityAuth,"IncomingSecurityAuth" );
_LIT( KSetupWizIncomingSecurityProtocol,"IncomingSecurityProtocol" );
_LIT( KSetupWizOutgoingSecurityAuth,"OutgoingSecurityAuth" );
_LIT( KSetupWizOutgoingSecurityProtocol,"OutgoingSecurityProtocol" );
_LIT( KSetupWizOperatorSecurityAuth,"OperatorSecurityAuth" );
_LIT( KSetupWizOperatorSecurityProtocol,"OperatorSecurityProtocol" );
_LIT( KSetupWizOperatorPort,"OperatorPort" );
_LIT(KProfileTesterDLL,"popimapprofiletester.dll");
_LIT(KAccountType,"AccountType");
_LIT(KIMAPAccount,"imap");
_LIT(KPOPAccount,"pop");
_LIT(KInboxType,"inbox");
_LIT(KOutboxType,"outbox");
_LIT(KDraftsType,"drafts");
_LIT(KSentType,"sent");
// Logging path
_LIT( KpopimapprofiletesterLogPath, "\\logs\\testframework\\" );
// Log file
_LIT( KpopimapprofiletesterLogFile, "popimapprofiletester.txt" );
_LIT( KpopimapprofiletesterLogFileWithTitle, "popimapprofiletester_[%S].txt" );
/*
 * default value for settings
 */
_LIT( KWizAccountTypeDefault,"fsp" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPopImapProfileTester::CPopImapProfileTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPopImapProfileTester::CPopImapProfileTester(CTestModuleIf& aTestModuleIf) :
    CScriptBase(aTestModuleIf), iTimeoutTimer(NULL), iMailClient(NULL),
            iIPSMailbox(NULL), iTimeout(EFalse), iEventParam1(NULL),
            iEventParam2(NULL), iEventSynchronousCall(EFalse), iErr(KErrNone),
            iWaitingState(EWaitingNothing), iPluginId(TFSMailMsgId()),
            iSearchOngoing(EFalse), iSearchMatches(NULL)
    {
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if (ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings;
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if (ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;

    if (loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KpopimapprofiletesterLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KpopimapprofiletesterLogFile);
        }

    iLog = CStifLogger::NewL(KpopimapprofiletesterLogPath, logFileName,
            CStifLogger::ETxt, CStifLogger::EFile, EFalse);

    SendTestClassVersion();

    iWait = new (ELeave) CActiveSchedulerWait();
    iTimeoutTimer = CTimeoutTimer::NewL(*this);

    // create a mail client
    iMailClient = CFSMailClient::NewL();
    if (iMailClient)
        {
        iMailClient->AddObserverL(*this);
        iLog->Log(_L( "Mail Client was created" ));
        }
    else
        {
        iLog->Log(_L( "ERROR: Mail Client not created" ));
        }
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPopImapProfileTester* CPopImapProfileTester::NewL(CTestModuleIf& aTestModuleIf)
    {
    CPopImapProfileTester* self = new (ELeave) CPopImapProfileTester(
            aTestModuleIf);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CPopImapProfileTester::~CPopImapProfileTester()
    {
    // Delete resources allocated from test methods
    Delete();
    if (iMailClient)
        {
        if (NULL != iIPSMailbox)
            {
            iMailClient->UnsubscribeMailboxEvents(iIPSMailbox->GetId(), *this);
            delete iIPSMailbox;
            iIPSMailbox = NULL;
            }
        iMailClient->Close();
        }
    DELANDNULL(iTimeoutTimer);

    // Delete logger
    DELANDNULL(iLog);

    //Delete waiter
    if (NULL != iWait && iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    DELANDNULL(iWait);
    }

//-----------------------------------------------------------------------------
// CPopImapProfileTester::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CPopImapProfileTester::SendTestClassVersion()
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
    moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;

    TFileName moduleName;
    moduleName = KProfileTesterDLL;

    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName,
            newVersionOfMethod);
    }
//-----------------------------------------------------------------------------
// CPopImapProfileTester::DeleteTestMsgL
// Method used to delete msgs in a specific folder
//-----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::DeleteTestMsgL(TDesC& aFolderName)
    {
    TInt err(KErrNone);
    TInt msgCount(0); //number of messages in folder

    // Step 01: to find a folder
    CFSMailFolder* folder = FindFolder(aFolderName);
    RArray<TFSMailMsgId> msgsToBeDeleted;
    CleanupClosePushL(msgsToBeDeleted);

    if (NULL != folder)
        {
        // Step 02: to list all test messages to be deleted from this folder
        RPointerArray<CFSMailMessage> messages;
        TInt gotMsgs = GetMessagesFromFolderL(messages, folder,
                EFSMsgDataSubject, EFSMailSortByDate, msgCount);

        // Step 03: to find to be deleted msgs 
        for (TInt i = 0; i < messages.Count(); i++)
            {
            //deleted msgs with STIF
            if (messages[i]->GetSubject().Find(_L("STIF" )) != KErrNotFound)
                {
                iLog->Log(_L("Found: %d. %S"), i + 1,
                        &messages[i]->GetSubject());
                msgsToBeDeleted.Append(messages[i]->GetMessageId());
                }
            }//end loop: 'for'

        if (msgsToBeDeleted.Count() > 0)
            {
            iLog->Log(_L("  -Deleting %d message(s)"), msgsToBeDeleted.Count());
            iMailClient->DeleteMessagesByUidL(iIPSMailbox->GetId(),
                    folder->GetFolderId(), msgsToBeDeleted);
            err = WaitForEvent(TFSEventMailDeleted, NULL, NULL, KOneMinuteTime);
            iIPSMailbox->RefreshNowL(*this);
            if (KErrNone == WaitForEvent(TFSEventMailboxOnline))
                {
                WaitForResponse(TFSProgress::EFSStatus_RequestComplete,
                        KHalfMinuteTime);
                }
            }//end if: 'count()>0'
        else
            {
            iLog->Log(_L("Not find messages(STIF) to delete"));
            err = KErrNotFound;
            }
        messages.ResetAndDestroy();
        }// end if 'inboxFolder'
    else
        {
        err = KErrNotFound;
        iLog->Log(_L(" %S is NOT found.(Error's ID:%d)"), &aFolderName, err);
        }
    CleanupStack::PopAndDestroy(&msgsToBeDeleted);
    return err;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::FindFoler
// Method used to find a folder with folder's name
// -----------------------------------------------------------------------------
CFSMailFolder* CPopImapProfileTester::FindFolder(const TDesC& aFolderName)
    {
    CFSMailFolder* folder(NULL);
    RPointerArray<CFSMailFolder>& allFolders = iIPSMailbox->ListFolders();

    TFSMailMsgId folderId = GetFolderIdFromName(aFolderName);
    for (TInt i = 0; i < allFolders.Count(); ++i)
        {
        if (allFolders[i]->GetFolderId() == folderId)
            {
            iLog->Log(_L("Found: %S folder"), &allFolders[i]->GetFolderName());
            folder = allFolders[i];
            break;
            }
        }
    return folder;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::GetMessagesFromFolderL
// Method used to get messages from specific folder
// -----------------------------------------------------------------------------
TInt CPopImapProfileTester::GetMessagesFromFolderL(
        RPointerArray<CFSMailMessage>& aMessages, CFSMailFolder* aFolder,
        TFSMailDetails aDet, TFSMailSortField aSort, TUint aCount)
    {
    TFSMailSortCriteria sortCriteria;
    sortCriteria.iField = aSort;
    sortCriteria.iOrder = EFSMailDescending; // default sort order: Descending

    RArray<TFSMailSortCriteria> sorting;
    CleanupClosePushL(sorting);

    // append primary criteria
    sorting.Append(sortCriteria);

    MFSMailIterator* iterator = aFolder->ListMessagesL(aDet, sorting);
    CleanupStack::PopAndDestroy(&sorting);
    CleanupDeletePushL(iterator);

    TInt messageCount = aFolder->GetMessageCount();
    iLog->Log(_L("  %d message(s) in '%S'"), messageCount,
            &aFolder->GetFolderName());

    if (aCount > 0)
        {
        if (messageCount < aCount)
            {
            aCount = messageCount;
            }
        }
    else
        {
        aCount = messageCount;
        }

    iterator->NextL(TFSMailMsgId(), aCount, aMessages);
    CleanupStack::PopAndDestroy(iterator);
    return aCount;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::EventL
// from MFSMailEventObserver
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::EventL(TFSMailEvent aEvent,
        TFSMailMsgId /*aMailbox*/, TAny* aParam1, TAny* aParam2, TAny* aParam3)
    {
    LogEvent(_L("Receive Event->"), aEvent, aParam1, aParam2, aParam3);
    if ((aEvent == iCurrWaitedEvent) && (iWait->IsStarted()) && (iWaitingState
            == EWaitingEvent) && ParseEventParams(aParam1, aParam2))
        OpComplete();
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::RequestResponseL
// from MFSMailRequestObserver
// It proivdes progress/status information of requested functions
// i.e. DeleteMailBoxByUidL, RefreshNowL
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::RequestResponseL(TFSProgress aEvent,
        TInt aRequestId)
    {
    LogTFSProgress(_L("Receive Response->"), aEvent.iProgressStatus, aRequestId);
    if ((aEvent.iProgressStatus == iCurrWaitedResponse) && (iWaitingState
            == EWaitingResponse) && (iWait->IsStarted()))
        OpComplete();
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::MatchFoundL
// from MFSMailBoxSearchObserver
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::MatchFoundL(CFSMailMessage* aMatchMessage)
    {
    TBuf<20> tempFolderName;
    GetFolderNameFromId(aMatchMessage->GetFolderId(), tempFolderName);
    TPtrC folderName = tempFolderName.Mid(0);
    iLog->Log(_L("  MatchFound %S in %S"), &aMatchMessage->GetSubject(),
            &folderName);
    if (iSearchOngoing)
        {
        iSearchMatches++;
        }
    delete aMatchMessage;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::SearchCompletedL
// from MFSMailBoxSearchObserver
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::SearchCompletedL()
    {
    if (iWait->IsStarted())
        {
        OpComplete();
        iLog->Log(_L(" SearchCompleted"));
        }
    else
        {
        iLog->Log(_L("Error: SearchCompleted callback"));
        }

    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::ClientRequiredSearchPriority
// from MFSMailBoxSearchObserver
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::ClientRequiredSearchPriority(TInt* /*apRequiredSearchPriority*/)
    {
    iLog->Log(_L("  ClientRequiredSearchPriority"));
    }

void CPopImapProfileTester::TimeoutNotify()
    {
    iTimeout = ETrue;
    OpComplete();
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::SendMsgL
// Method is a actual sending message function.
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::SendMsgL(CFSMailMessage& aMsg,
        const TDesC &aSubject, TFSMailMsgId& aGotMsgId)
    {
    TInt err(KErrNone);
    TInt msgCount(0);

    //to call actual function to send message
    TRAP( err, iIPSMailbox->SendMessageL( aMsg ) );

    //second asynchronous when moving msg from Outbox to Sent Items ( this actually tells msg was sent )
    if (KErrNone == err)
        {
        //Wait for email to be sent
        TFSMailMsgId folderId = iIPSMailbox->GetStandardFolderId(EFSSentFolder);
        iIPSMailbox->RefreshNowL();
        err = WaitForEvent(TFSEventMailMoved, NULL, &folderId);
        iIPSMailbox->GoOfflineL();
        WaitForEvent(TFSEventMailboxOffline);
        iIPSMailbox->RefreshNowL(*this);
        WaitForResponse(TFSProgress::EFSStatus_RequestComplete);

        if (KErrNone == err)
            {
            // Step04: To check 'Inbox' folder to sure message sent
            TFSMailMsgId folderId = iIPSMailbox->GetStandardFolderId(EFSInbox);
            CFSMailFolder* folder = iMailClient->GetFolderByUidL(
                    iIPSMailbox->GetId(), folderId);
            CleanupStack::PushL(folder);
            if (NULL != folder)
                {
                RPointerArray<CFSMailMessage> messages;
                TInt gotMsgs = GetMessagesFromFolderL(messages, folder,
                        EFSMsgDataSubject, EFSMailSortByDate, msgCount);
                if (gotMsgs > 0)
                    {
                    iLog->Log(_L("  Listing first %d emails:"), gotMsgs);
                    for (TInt i = 0; i < messages.Count(); i++)
                        {
                        iLog->Log(_L("Found: %S"), &messages[i]->GetSubject());

                        if (messages[i]->GetSubject().Compare(aSubject) == 0)
                            {
                            iLog->Log(
                                    _L("Success:Email '%S' sent to own mail address"),
                                    &messages[i]->GetSubject());
                            aGotMsgId.SetPluginId(
                                    messages[i]->GetMessageId().PluginId());
                            aGotMsgId.SetId(messages[i]->GetMessageId().Id());
                            err = KErrNone;
                            break;
                            }
                        else
                            {
                            err = KErrNotFound;
                            }
                        }//end loop: 'for'
                    }//end if 'msgCount'
                else
                    {
                    err = KErrNotFound;
                    iLog->Log(_L("No message in %S folder"),
                            &folder->GetFolderName());
                    }
                messages.ResetAndDestroy();
                }//end if 'folder'
            else
                {
                err = KErrNotFound;
                iLog->Log(_L("Folder is NOT found.(Error's ID:%d)"), err);
                }
            CleanupStack::PopAndDestroy(folder);
            }//end if: checking whether 'msg' sent or not
        }//end if 'KErrNone == err'
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::DoSearch
// Method used to launches SearchL and waits for completion
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::DoSearch(
        const RPointerArray<TDesC>& aSearchStrings,
        const TFSMailSortCriteria& aSortCriteria, TInt aTimeout)
    {
    TInt err(KErrNone);
    if (iSearchOngoing)
        {
        iLog->Log(_L("  Error: Search in progress"));
        err = KErrInUse;
        }
    else
        {
        iIPSMailbox->ClearSearchResultCache();
        iSearchMatches = EFalse;
        TRAP( err, iIPSMailbox->SearchL( aSearchStrings, aSortCriteria, *this ) );
        if (KErrNone != err)
            {
            iLog->Log(_L("  Error: SearchL leaves with %d"), err);
            }
        else
            {
            iLog->Log(_L("  Searching for '%S':"), aSearchStrings[0]);
            iSearchOngoing = ETrue;
            iTimeoutTimer->Start(aTimeout * 1000000);
            iWait->Start();
            if (iTimeout)
                {
                iLog->Log(_L("  Error: Search not completed within %d sec"),
                        aTimeout);
                err = KErrTimedOut;
                }
            else
                {
                iLog->Log(_L(" Search completed with %d result(s)"),
                        iSearchMatches);
                err = KErrNone;
                }
            iSearchOngoing = EFalse;
            }
        }
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::WaitForEvent
// Method used to wait incoming events from obsever
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::WaitForEvent(TFSMailEvent aWaitedEvent,
        TAny *aEventParam1, TAny *aEventParam2, TInt aTimeout)
    {
    LogEvent(_L("Waiting for Event->"), aWaitedEvent, aEventParam1,
            aEventParam2, NULL);
    iWaitingState = EWaitingEvent;
    iCurrWaitedEvent = aWaitedEvent;
    iEventParam1 = aEventParam1;
    iEventParam2 = aEventParam2;
    iTimeoutTimer->Start(aTimeout * 1000000);
    if (iWait->IsStarted() == EFalse)
        {
        iWait->Start();
        iWaitingState = EWaitingNothing;
        iEventParam1 = NULL;
        iEventParam2 = NULL;
        if (iTimeout)
            {
            iLog->Log(_L(" Operation is timeout over %d"), aTimeout);
            iTimeout = EFalse;
            return KErrTimedOut;
            }
        else
            {
            iTimeoutTimer->Stop();
            return KErrNone;
            }
        }
    else
        {
        iLog->Log(_L(" Stop the wait before start it"));
        return KErrGeneral;
        }
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::WaitForResponse
// Method used to wait response from observer
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::WaitForResponse(
        TFSProgress::TFSProgressStatus aWaitedResponse, TInt aTimeout)
    {
    LogTFSProgress(_L("Waiting for Response->"), aWaitedResponse, 0);
    iWaitingState = EWaitingResponse;
    iCurrWaitedResponse = aWaitedResponse;
    iTimeoutTimer->Start(aTimeout * 1000000);
    if (iWait->IsStarted() == EFalse)
        {
        iWait->Start();
        iWaitingState = EWaitingNothing;
        if (iTimeout)
            {
            iLog->Log(_L(" Operation is timeout over %d"), aTimeout);
            iTimeout = EFalse;
            return KErrTimedOut;
            }
        else
            {
            iTimeoutTimer->Stop();
            return KErrNone;
            }
        }
    else
        return KErrGeneral;
    }

void CPopImapProfileTester::OpComplete()
    {
    if (iWait)
        iWait->AsyncStop();
    if (iTimeoutTimer)
        iTimeoutTimer->Stop();
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::InitMailbox()
// Method to initiate mailbxo, depends on type of account in .cfg file
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::InitMailboxL()
    {
    if (NULL != iIPSMailbox)
        return KErrNone;

    TBuf<10> accountType;

    //to separate type of mailbox: 'imap or pop' 
    GetConstantValue(KAccountType, accountType);
    accountType.TrimAll();
    accountType.LowerCase();
    if (accountType.Compare(KIMAPAccount) == 0)
        iPluginId.SetPluginId(TUid::Uid(KIPSSosImap4PluginUid));
    else if (accountType.Compare(KPOPAccount) == 0)
        {
        iPluginId.SetPluginId(TUid::Uid(KIPSSosPop3PluginUid));
        }
    else
        {
        iLog->Log(_L("Wrong mailbox type %S"), &accountType);
        return KErrGeneral;
        }

    //to create mail
    RPointerArray<CFSMailBox> mailboxes;
    iMailClient->ListMailBoxes(iPluginId, mailboxes);
    if (mailboxes.Count() > 0)
        {
        iIPSMailbox = iMailClient->GetMailBoxByUidL(mailboxes[0]->GetId());
        //to subscribe mailbox's event,remember to unsubscribe 
        iMailClient->SubscribeMailboxEventsL(iIPSMailbox->GetId(), *this);
        iLog->Log(_L("%S mailbox was found"), &accountType);
        }
    else
        {
        iLog->Log(_L("%S mailbox was not found"), &accountType);
        }
    mailboxes.ResetAndDestroy();

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::MoveMessageToFolderL
// Method used to move messages to a specific folder
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::MoveMessageToFolderL(CFSMailMessage& aMsg,
        TFSFolderType aFolderType)
    {
    // check if msg is not in given folder already
    TInt err(KErrNone);
    TFSMailMsgId folderId = iIPSMailbox->GetStandardFolderId(aFolderType);
    TFSMailMsgId msgFolderId = aMsg.GetFolderId();
    if (folderId != msgFolderId)
        {
        // move message
        RArray<TFSMailMsgId> msgs;
        CleanupClosePushL(msgs);
        msgs.Append(aMsg.GetMessageId());
        TRAP(err, iIPSMailbox->MoveMessagesL( msgs, msgFolderId, folderId ));
        msgs.Reset();
        CleanupStack::PopAndDestroy(&msgs);
        }
    return err;
    }
// -----------------------------------------------------------------------------
// CPopImapProfileTester::CreatePlainTextMsgL
// Info: Creates defalult msg with plain text body part in 'Drafts' folder
//       with own mail address in 'To' recipient
// Params: DesC& aSubject, subject text
//           TDesC& aMsgBodyText, message body text ( max. 256 chars )
// -----------------------------------------------------------------------------
//
CFSMailMessage* CPopImapProfileTester::CreatePlainTextMsgL(
        const TDesC& aSubject, const TDesC& aMsgBodyText)
    {
    TInt err(KErrNone);
    CFSMailMessage* newMsg(NULL);
    CFSMailMessagePart* newMsgPart(NULL);

    newMsg = iIPSMailbox->CreateMessageToSend();
    if (NULL != newMsg)
        {
        CleanupStack::PushL(newMsg);
        // add subject
        newMsg->SetSubject(aSubject);

        // add default sender address
        CFSMailAddress* senderAddr = CFSMailAddress::NewL();
        senderAddr->SetEmailAddress(_L("stif.testcase@stif.com"));
        newMsg->SetSender(senderAddr);
        // senderAddr - do not delete it, ownership transferred from user

        // add default To recipient ( own mail address )
        CFSMailAddress* toAddr = CFSMailAddress::NewL();
        toAddr->SetEmailAddress(iIPSMailbox->OwnMailAddress().GetEmailAddress());
        newMsg->AppendToRecipient(toAddr);
        // toAddr - do not delete it, ownership transferred from user

        // assert: move message to Drafts ( it should be already in Drafts )
        err = MoveMessageToFolderL(*newMsg, EFSDraftsFolder);
        if (KErrNone == err)
            {
            newMsg->SetContentType(KFSMailContentTypeMultipartMixed);
            newMsg->SaveMessageL();
            newMsgPart = newMsg->PlainTextBodyPartL();
            if (NULL == newMsgPart)
                {
                newMsgPart = newMsg->NewChildPartL(TFSMailMsgId(),
                        KFSMailContentTypeTextPlain);
                }
            if (NULL != newMsgPart)
                {
                CleanupStack::PushL(newMsgPart);
                TBuf<256> buf(aMsgBodyText);
                newMsgPart->SetContent(buf);
                newMsgPart->SetContentSize(buf.Length());
                newMsgPart->SetFetchedContentSize(buf.Length());
                newMsgPart->SaveL();
                CleanupStack::PopAndDestroy(newMsgPart);
                }
            else
                {
                iLog->Log(_L( "Error: Failed to create msg part" ));
                err = KErrNoMemory;
                }
            }//end if 'KErrNone == err'
        else
            {
            iLog->Log(_L( "Error: Failed to move new msg to Drafts folder" ));
            }
        CleanupStack::Pop(newMsg);
        }//end if 'NULL != newMsg'
    else
        {
        iLog->Log(_L( "   Error: Failed to create new msg" ));
        err = KErrNoMemory;
        }
    return newMsg;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::ReadAccountSettings()
// Method used to read setting data from .cfg file
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::ReadAccountSettings(
        TAccountSetting& aAccountSetting)
    {
    TInt err(KErrNone);
    err = GetConstantValue(KSetupWizAccountType, aAccountSetting.iAccountType);
    aAccountSetting.iAccountType.TrimAll();
    aAccountSetting.iAccountType.LowerCase();
    if (aAccountSetting.iAccountType.Compare(KIMAPAccount) == 0)
        {
        aAccountSetting.iProtocolIndicator = 1;
        }
    else if (aAccountSetting.iAccountType.Compare(KPOPAccount) == 0)
        {
        aAccountSetting.iProtocolIndicator = 0;
        }
    else
        {
        iLog->Log(_L("Wrong Account Type %S, "), &aAccountSetting.iAccountType);
        return KErrNotFound;
        }
    err = GetConstantValue(KSetupWizAccountsConfigured,
            aAccountSetting.iAccountsConfigured);
    err = GetConstantValue(KSetupWizMaxAccountsReached,
            aAccountSetting.iMaxAccountsReached);
    err
            = GetConstantValue(KSetupWizEmailAddress,
                    aAccountSetting.iEmailAddress);
    err = GetConstantValue(KSetupWizUsername, aAccountSetting.iUserName);
    err = GetConstantValue(KSetupWizPassword, aAccountSetting.iPassWord);
    err = GetConstantValue(KSetupWizMailboxName, aAccountSetting.iMailboxName);
    err = GetConstantValue(KSetupWizIncomingServer,
            aAccountSetting.iIncomingServer);
    err = GetConstantValue(KSetupWizOutgoingServer,
            aAccountSetting.iOutgoingServer);
    err
            = GetConstantValue(KSetupWizIncomingPort,
                    aAccountSetting.iIncomingPort);
    err
            = GetConstantValue(KSetupWizOutgoingPort,
                    aAccountSetting.iOutgoingPort);

    err = GetConstantValue(KSetupWizIncomingSecurityAuth,
            aAccountSetting.iIncomingSecurityAuth);
    err = GetConstantValue(KSetupWizOutgoingSecurityAuth,
            aAccountSetting.iOutgoingSecurityAuth);
    err = GetConstantValue(KSetupWizIncomingSecurityProtocol,
            aAccountSetting.iIncomingSecurityProtocol);
    err = GetConstantValue(KSetupWizOutgoingSecurityProtocol,
            aAccountSetting.iOutgoingSecurityProtocol);
    err = GetConstantValue(KSetupWizOperatorSecurityAuth,
            aAccountSetting.iOperatorSecurityAuth);
    err = GetConstantValue(KSetupWizOperatorSecurityProtocol,
            aAccountSetting.iOperatorSecurityProtocol);
    err
            = GetConstantValue(KSetupWizOperatorPort,
                    aAccountSetting.iOperatorPort);

    aAccountSetting.iWizardAccountType = KWizAccountTypeDefault;
    aAccountSetting.iOperatorOutgoingServer = KNullDesC;
    aAccountSetting.iHideUserNameInSetting = ETrue;
    aAccountSetting.iAccessPoint = KNullDesC;

    return err;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::WriteToWizardCRL
// Prepare data for profile settings
// write configuration settings to cenrep, just like SW6 would do
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::WriteToWizardCRL(TAccountSetting aAccountSetting)
    {
    TInt err(KErrNone);
    TUint32 keyInfo;
    CRepository* rep = CRepository::NewLC(KCRUidWizardSettings);
    err = rep->StartTransaction(CRepository::EConcurrentReadWriteTransaction);
    iLog->Log(_L( "== Start a transaction in SetupAccount ==" ));
    if (KErrNone == err)
        {
        err = rep->Set(ECRKSetupWizAccountType,
                aAccountSetting.iWizardAccountType);
        err = rep->Set(ECRKAccountsConfigured,
                aAccountSetting.iAccountsConfigured);
        err = rep->Set(ECRKMaxAccountsReached,
                aAccountSetting.iMaxAccountsReached);
        err = rep->Set(ECRKPopImapEmailAddressId,
                aAccountSetting.iEmailAddress);
        err = rep->Set(ECRKPopImapUsernameId, aAccountSetting.iUserName);
        err = rep->Set(ECRKPopImapPasswordId, aAccountSetting.iPassWord);
        err = rep->Set(ECRKPopImapIncomingServerId,
                aAccountSetting.iIncomingServer);
        err = rep->Set(ECRKPopImapOutgoingServerId,
                aAccountSetting.iOutgoingServer);
        err = rep->Set(ECRKPopImapProtocolIndicatorId,
                aAccountSetting.iProtocolIndicator);
        err = rep->Set(ECRKPopImapIncomingPortId,
                aAccountSetting.iIncomingPort);
        err = rep->Set(ECRKPopImapOutgoingPortId,
                aAccountSetting.iOutgoingPort);
        err = rep->Set(ECRKPopImapIncomingSecurityAuthId,
                aAccountSetting.iIncomingSecurityAuth);
        err = rep->Set(ECRKPopImapIncomingSecurityProtocolId,
                aAccountSetting.iIncomingSecurityProtocol);
        err = rep->Set(ECRKPopImapOutgoingSecurityAuthId,
                aAccountSetting.iOutgoingSecurityAuth);
        err = rep->Set(ECRKPopImapOutgoingSecurityProtocolId,
                aAccountSetting.iOutgoingSecurityProtocol);
        err = rep->Set(ECRKPopImapOperatorOutgoingServerId,
                aAccountSetting.iOperatorOutgoingServer);
        err = rep->Set(ECRKPopImapOperatorSecurityAuthId,
                aAccountSetting.iOperatorSecurityAuth);
        err = rep->Set(ECRKPopImapOperatorSecurityProtocolId,
                aAccountSetting.iOperatorSecurityProtocol);
        err = rep->Set(ECRKPopImapOperatorPortId,
                aAccountSetting.iOperatorPort);
        err = rep->Set(ECRKHideUsernameInSettings,
                aAccountSetting.iHideUserNameInSetting); // 1 = ETrue if there are some field that should be hidden from ui
        err = rep->Set(ECRKPopImapAccessPointId, aAccountSetting.iAccessPoint);
        err = rep->Set(ECRKPopImapMailboxName, aAccountSetting.iMailboxName);
        iLog->Log(_L( "==Error:=%d" ), err);

        err = rep->CommitTransaction(keyInfo);
        if (KErrLocked == err || KErrAbort == err || KErrNone != err)
            {
            rep->CancelTransaction();
            iLog->Log(
                    _L( "== Error occurs in transaction processed in SetupAccount=%d" ),
                    err);
            }
        }
    else
        {
        rep->CancelTransaction();
        iLog->Log(_L( "== The transaction was canceled in SetupAccount=%d" ),
                err);
        }
    CleanupStack::PopAndDestroy(rep);
    return err;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::LogEvent
// Method used to print log about event
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::LogEvent(const TDesC& aLogText,
        TFSMailEvent aEvent, TAny* aParam1, TAny* aParam2, TAny* aParam3)
    {
    TBuf<256> log(aLogText);

    switch (aEvent)
        {
        // Mailbox is unavailable
        // params NULL
        case TFSMailboxUnavailable:
            log.Append(_L("MailboxUnavailable"));
            break;

            // Mailbox is available
            // params NULL
        case TFSMailboxAvailable:
            log.Append(_L( "MailboxAvailable" ));
            break;

            // New mailbox created
            // params NULL
        case TFSEventNewMailbox:
            log.Append(_L( "NewMailbox" ));
            break;

            // New mailbox creation failure
            // params NULL
        case TFSEventMailboxCreationFailure:
            log.Append(_L( "MailboxCreationFailure" ));
            break;

            // Mailbox renamed
            // params NULL
        case TFSEventMailboxRenamed:
            log.Append(_L( "MailboxRenamed" ));
            break;

            // Mailbox deleted
            // params NULL
        case TFSEventMailboxDeleted:
            log.Append(_L( "MailboxDeleted" ));
            break;

            //
            // Following events need to be subscribed per mailbox
            //

            // Mailbox capability changed
            // params NULL
        case TFSEventMailboxCapabilityChanged:
            log.Append(_L( "MailboxCapabilityChanged" ));
            break;

            // Mailbox settings changed
            // params NULL
        case TFSEventMailboxSettingsChanged:
            log.Append(_L( "MailboxSettingsChanged" ));
            break;

            // Mailbox went online
            // params NULL
        case TFSEventMailboxOnline:
            log.Append(_L( "MailboxOnline" ));
            break;

            // Mailbox went offline
            // params NULL
        case TFSEventMailboxOffline:
            log.Append(_L( "MailboxOffline" ));
            break;

            // Mailbox sync state changed
            // aParam1: TSSMailSyncState* newState
            // aParam2: NULL
            // aParam3: NULL
        case TFSEventMailboxSyncStateChanged:
            {
            TSSMailSyncState state = *((TSSMailSyncState*) aParam1);
            log.Append(_L( "SyncStateChanged" ));
            switch (state)
                {
                case Idle:
                    log.Append(_L("(Idle)"));
                    break;
                case StartingSync:
                    log.Append(_L("(StartingSync)"));
                    break;
                case DataSyncStarting:
                    log.Append(_L("(DataSyncStarting)"));
                    break;
                case EmailSyncing:
                    log.Append(_L("(EmailSyncing)"));
                    break;
                case InboxSyncing:
                    log.Append(_L("(InboxSyncing)"));
                    break;
                case OutboxSyncing:
                    log.Append(_L("(OutboxSyncing)"));
                    break;
                case SentItemsSyncing:
                    log.Append(_L("(SentItemsSyncing)"));
                    break;
                case DraftsSyncing:
                    log.Append(_L("(DraftsSyncing)"));
                    break;
                case CalendarSyncing:
                    log.Append(_L("(CalendarSyncing)"));
                    break;
                case ContactsSyncing:
                    log.Append(_L("(ContactsSyncing)"));
                    break;
                case TasksSyncing:
                    log.Append(_L("(TasksSyncing)"));
                    break;
                case NotesSyncing:
                    log.Append(_L("(NotesSyncing)"));
                    break;
                case FilesSyncing:
                    log.Append(_L("(FilesSyncing)"));
                    break;
                case FinishedSuccessfully:
                    log.Append(_L("(FinishedSuccessfully)"));
                    break;
                case SyncError:
                    log.Append(_L("(SyncError)"));
                    break;
                case SyncCancelled:
                    log.Append(_L("(SyncCancelled)"));
                    break;
                case PasswordExpiredError:
                    log.Append(_L("(PasswordExpiredError)"));
                    break;
                case PasswordVerified:
                    log.Append(_L("(PasswordVerified)"));
                    break;
                case PasswordNotVerified:
                    log.Append(_L("(PasswordNotVerified)"));
                    break;
                case PushChannelOff:
                    log.Append(_L("(PushChannelOff)"));
                    break;
                case PushChannelEstablished:
                    log.Append(_L("(PushChannelEstablished)"));
                    break;
                case PushChannelOffBecauseBatteryIsLow:
                    log.Append(_L("(PushChannelOffBecauseBatteryIsLow)"));
                    break;
                case OutOfDiskSpace:
                    log.Append(_L("(OutOfDiskSpace)"));
                    break;
                default:
                    log.Append(_L("( no description )"));
                }
            }
            break;

            // New mails created
            // aParam1: RArray<TFSMailMsgId>* aNewEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventNewMail:
            log.Append(_L( "NewEmail" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Mails changed
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventMailChanged:
            log.Append(_L( "MailChanged" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Mails deleted
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventMailDeleted:
            log.Append(_L( "MailDeleted" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Mails moved
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aNewParentFolder
            // aParam3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailMoved:
            log.Append(_L( "MailMoved" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(_L("---"));
                    log.Append(folderName);
                    }
                }
            if (aParam3)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam3);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Mails copied
            // aParam1: RArray<TFSMailMsgId>* aNewEntries
            // aParam2: TFSMailMsgId* aNewParentFolder
            // aParam3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailCopied:
            log.Append(_L( "MailCopied" ));
            break;

            // New folders created
            // aParam1: RArray<TFSMailMsgId>* aNewEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL 
        case TFSEventNewFolder:
            log.Append(_L( "NewFolder" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Folders changed
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventFolderChanged:
            log.Append(_L( "FolderChanged" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Folders deleted
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventFoldersDeleted:
            log.Append(_L( "FoldersDeleted" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

            // Folders moved
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aNewParentFolder
            // aParam3: TFSMailMsgId* aOldParentFolder
        case TFSEventFoldersMoved:
            log.Append(_L( "FoldersMoved" ));
            break;

            // exception / error happened 
            // aParam1: TInt (TFsEmailNotifierSystemMessageType) aEventType
            // aParam2: TDesC* aCustomMessage or NULL (optional)
            // aParam3: MFSMailExceptionEventCallback* aCallback or NULL(optional)
        case TFSEventException:
            {
            log.Append(_L( "Exception " ));
            }
            break;

            // Mail deleted from viewer
            // aParam1: RArray<TFSMailMsgId>* aEntries
            // aParam2: TFSMailMsgId* aParentFolder
            // aParam3: NULL
        case TFSEventMailDeletedFromViewer:
            log.Append(_L( "MailDeletedFromViewer" ));
            if (aParam1)
                {
                RArray<TFSMailMsgId>* newEntries = static_cast<RArray<
                        TFSMailMsgId>*> (aParam1);
                TInt newEntriesCount = newEntries->Count();
                log.AppendFormat(_L("(%d)"), newEntriesCount);
                }
            if (aParam2)
                {
                TFSMailMsgId* parentFolder =
                        static_cast<TFSMailMsgId*> (aParam2);
                TBuf<20> folderName;
                if (KErrNone == GetFolderNameFromId(*parentFolder, folderName))
                    {
                    log.Append(folderName);
                    }
                }
            break;

        default:
            log.Append(_L( "Unknown" ));
            break;
        }
    iLog->Log(log);
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::LogTFSProgress
// Method used to print logs about TFSProgress state 
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::LogTFSProgress(const TDesC& aLogText,
        const TFSProgress::TFSProgressStatus aProgress, TInt aReqId)
    {
    TBuf<100> log(aLogText);
    if (aReqId >= 0)
        {
        log.AppendFormat(_L("%d-"), aReqId);
        }

    switch (aProgress)
        {
        case TFSProgress::EFSStatus_Waiting:
            log.Append(_L("Waiting"));
            break;

        case TFSProgress::EFSStatus_Started:
            log.Append(_L("Started"));
            break;

        case TFSProgress::EFSStatus_Connecting:
            log.Append(_L("Connecting"));
            break;

        case TFSProgress::EFSStatus_Connected:
            log.Append(_L("Connected"));
            break;

        case TFSProgress::EFSStatus_Authenticating:
            log.Append(_L("Authenticating"));
            break;

        case TFSProgress::EFSStatus_Authenticated:
            log.Append(_L("Authenticated"));
            break;

        case TFSProgress::EFSStatus_Status:
            log.Append(_L("Status"));
            break;

        case TFSProgress::EFSStatus_RequestComplete:
            log.Append(_L("Complete"));
            break;

        case TFSProgress::EFSStatus_RequestCancelled:
            log.Append(_L("Cancelled"));
            break;

        default:
            log.Append(_L("UnknownStatus"));
        }
    iLog->Log(log);
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::LogTSSMailSyncState
// Method used to print logs about TSSMailSyncState state 
// -----------------------------------------------------------------------------
//
void CPopImapProfileTester::LogTSSMailSyncState(const TDesC& aLogText,
        const TSSMailSyncState aState)
    {
    TBuf<100> log(aLogText);
    switch (aState)
        {
        case Idle:
            log.Append(_L("Idle"));
            break;

        case StartingSync:
            log.Append(_L("StartingSync"));
            break;

        case DataSyncStarting:
            log.Append(_L("DataSyncStarting"));
            break;

        case EmailSyncing:
            log.Append(_L("EmailSyncing"));
            break;

        case InboxSyncing:
            log.Append(_L("InboxSyncing"));
            break;

        case OutboxSyncing:
            log.Append(_L("OutboxSyncing"));
            break;

        case SentItemsSyncing:
            log.Append(_L("SentItemsSyncing"));
            break;

        case DraftsSyncing:
            log.Append(_L("DraftsSyncing"));
            break;

        case CalendarSyncing:
            log.Append(_L("CalendarSyncing"));
            break;

        case ContactsSyncing:
            log.Append(_L("ContactsSyncing"));
            break;

        case TasksSyncing:
            log.Append(_L("TasksSyncing"));
            break;

        case NotesSyncing:
            log.Append(_L("NotesSyncing"));
            break;

        case FilesSyncing:
            log.Append(_L("FilesSyncing"));
            break;

        case FinishedSuccessfully:
            log.Append(_L("FinishedSuccessfully"));
            break;

        case SyncError:
            log.Append(_L("SyncError"));
            break;

        case SyncCancelled:
            log.Append(_L("SyncCancelled"));
            break;

        case PasswordExpiredError:
            log.Append(_L("PasswordExpiredError"));
            break;

        case PasswordVerified:
            log.Append(_L("PasswordVerified"));
            break;

        case PasswordNotVerified:
            log.Append(_L("PasswordNotVerified"));
            break;

        case PushChannelOff:
            log.Append(_L("PushChannelOff"));
            break;

        case PushChannelEstablished:
            log.Append(_L("PushChannelEstablished"));
            break;

        case PushChannelOffBecauseBatteryIsLow:
            log.Append(_L("PushChannelOffBecauseBatteryIsLow"));
            break;

        case OutOfDiskSpace:
            log.Append(_L("OutOfDiskSpace"));
            break;
        }
    iLog->Log(log);
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::GetFolderNameFromId
// Method used to get name of folder with given ID
// -----------------------------------------------------------------------------
//
TInt CPopImapProfileTester::GetFolderNameFromId(TFSMailMsgId aFolderId,
        TDes& aFolderName)
    {
    RPointerArray<CFSMailFolder>& folders = iIPSMailbox->ListFolders();
    TInt err(KErrNotFound);
    for (TInt i = 0; i < folders.Count(); i++)
        {
        if (folders[i]->GetFolderId() == aFolderId)
            {
            aFolderName = folders[i]->GetFolderName();
            err = KErrNone;
            break;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::GetFolderIdFromName
// Method used to get ID of folder with given name
// -----------------------------------------------------------------------------
//
TFSMailMsgId CPopImapProfileTester::GetFolderIdFromName(
        const TDesC& aFolderName)
    {
    TBuf<10> bufFolderName(aFolderName);
    TFSMailMsgId folderId;

    bufFolderName.TrimAll();
    bufFolderName.LowerCase();

    if (bufFolderName.Compare(KInboxType) == 0)
        {
        folderId = iIPSMailbox->GetStandardFolderId(EFSInbox);
        }
    else if (bufFolderName.Compare(KOutboxType) == 0)
        {
        folderId = iIPSMailbox->GetStandardFolderId(EFSOutbox);
        }
    else if (bufFolderName.Compare(KDraftsType) == 0)
        {
        folderId = iIPSMailbox->GetStandardFolderId(EFSDraftsFolder);
        }
    else if (bufFolderName.Compare(KSentType) == 0)
        {
        folderId = iIPSMailbox->GetStandardFolderId(EFSSentFolder);
        }
    else
        {
        folderId = iIPSMailbox->GetStandardFolderId(EFSOther);
        }
    return folderId;
    }

// -----------------------------------------------------------------------------
// CPopImapProfileTester::ParseEventParams
// Method used to parese params from event
// -----------------------------------------------------------------------------
//
TBool CPopImapProfileTester::ParseEventParams(TAny *aEventParam1,
        TAny *aEventParam2)
    {
    TBool ret(EFalse);
    switch (iCurrWaitedEvent)
        {
        case TFSEventMailboxSyncStateChanged:
            if (NULL == iEventParam1)
                {
                ret = ETrue;
                }
            else if (NULL == aEventParam1)
                {
                ret = EFalse;
                }
            else if (*((TSSMailSyncState*) aEventParam1)
                    == *((TSSMailSyncState*) iEventParam1))
                {
                ret = ETrue;
                }
            else
                {
                ret = EFalse;
                }
            break;

        case TFSEventMailMoved:
            if (NULL == iEventParam1)
                {
                ret = ETrue;
                }
            else if (NULL == aEventParam2)
                {
                ret = EFalse;
                }
            else if (*((TFSMailMsgId*) aEventParam2)
                    == *((TFSMailMsgId*) iEventParam2))
                {
                ret = ETrue;
                }
            else
                {
                ret = EFalse;
                }
            break;

        default:
            ret = ETrue;
            break;
        }
    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(CTestModuleIf& aTestModuleIf) // Backpointer to STIF Test Framework
    {

    return (CScriptBase*) CPopImapProfileTester::NewL(aTestModuleIf);

    }

//  End of File
