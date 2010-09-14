/*
 *  Copyright ?2010-2010 Nokia and/or its subsidiary(-ies).  All rights reserved.
 *  This material, including documentation and any related computer
 *  programs, is protected by copyright controlled by Nokia.  All
 *  rights are reserved.  Copying, including reproducing, storing,
 *  adapting or translating, any or all of this material requires the
 *  prior written consent of Nokia.  This material also contains
 *  confidential information which may not be disclosed to others
 *  without the prior written consent of Nokia.
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 * 
 * Description: STIF testclass declaration
 *
 */

#ifndef POPIMAPPROFILETESTER_H
#define POPIMAPPROFILETESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <cemailaccounts.h>
#include <centralrepository.h>

//cmail side
#include "cfsmailbox.h" //for cmail observer
#include "cfsmailclient.h"
#include "cfsmailcommon.h" //for cmail common flags
#include "timeouttimer.h" //for timeout timer
#include "baseplugincommonutils.h" //for cleanup
#include "FreestyleEmailCenRepKeys.h"
#include "../../../ipssossettings/inc/ipssetwizardsettingscenrepkeys.h"

#include "mfsmaileventobserver.h"
#include "mfsmailrequestobserver.h"
#include "mfsmailboxsearchobserver.h"

// CONSTANTS
#define KIPSSosImap4PluginUid      0x2000e53f
#define KIPSSosPop3PluginUid       0x2000e53e

const TUint KTenSecondsTime(10);
const TUint KHalfMinuteTime(30);
const TUint KOneMinuteTime(60);

// MACROS
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0
#define DELANDNULL( a ) { if( a ) { delete a; a = NULL; } }

class CPopImapProfileTester;
class CTimeoutTimer;
class CFSMailClient;
class CFSMailBox;
class CActiveSchedulerWait;

enum TWaitingState
    {
    EWaitingNothing = 0,
    EWaitingEvent,
    EWaitingResponse,
    EWaitingSearchComplete,
    };

class TAccountSetting
    {
public:
    //Other TInt values
    // amount of accounts configured  
    TInt iAccountsConfigured;
    // maximum accounts reached 1 = yes, 0 = no
    TInt iMaxAccountsReached;
    // protocol 0= POP, 1= IMAP  //-->imap
    TInt iProtocolIndicator;
    // Incoming security auth 0 = no, 1 = yes
    TInt iIncomingSecurityAuth;
    // Incoming security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
    TInt iIncomingSecurityProtocol;
    // Outgoing security auth 0 = no, 1 = yes
    TInt iOutgoingSecurityAuth;
    // Outgoing security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
    TInt iOutgoingSecurityProtocol;
    // Operators security auth 0 = no, 1 = yes
    TInt iOperatorSecurityAuth;
    // Operators Security protocol (0 = OFF, 1 = StartTLS, 2 = SSL/TLS)
    TInt iOperatorSecurityProtocol;
    // Operators outgoing port number
    TInt iOperatorPort;

    TInt iIncomingPort;
    TInt iOutgoingPort;

    TBuf<10> iAccountType;
    TBuf<50> iEmailAddress;
    TBuf<50> iUserName;
    TBuf<100> iPassWord;
    TBuf<50> iIncomingServer;
    TBuf<50> iOutgoingServer;
    TBuf<20> iMailboxName;

    /****
     * Using the default value
     */
    TBuf<5> iWizardAccountType;
    TBuf<2> iOperatorOutgoingServer;
    TBool iHideUserNameInSetting;
    TBuf<10> iAccessPoint;
    };

NONSHARABLE_CLASS(CPopImapProfileTester) : public CScriptBase,
        public MFSMailEventObserver,
        public MFSMailRequestObserver,
        public MFSMailBoxSearchObserver,
        public MTimeoutObserver

    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPopImapProfileTester* NewL(CTestModuleIf& aTestModuleIf);

    /**
     * Destructor.
     */
    virtual ~CPopImapProfileTester();

public:
    // New functions


public:
    // Functions from base classes

    /**
     * From CScriptBase Runs a script line.
     * @since 
     * @param aItem Script line containing method name and parameters
     * @return Symbian OS error code
     */
    virtual TInt RunMethodL(CStifItemParser& aItem);

    // from MFSMailEventObserver
    void EventL(TFSMailEvent aEvent, TFSMailMsgId aMailbox, TAny* aParam1,
            TAny* aParam2, TAny* aParam3);

    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

    // from MFSMailBoxSearchObserver
    void MatchFoundL(CFSMailMessage* aMatchMessage);
    void SearchCompletedL();
    void ClientRequiredSearchPriority(TInt *apRequiredSearchPriority);

    //from MTimeoutObserver
    void TimeoutNotify();

protected:
    // New functions


protected:
    // Functions from base classes


private:

    /**
     * C++ default constructor.
     */
    CPopImapProfileTester(CTestModuleIf& aTestModuleIf);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Frees all resources allocated from test methods.
     * @since 
     */
    void Delete();

    // _______________________ Test Methods __________________________

    // Ordinary test cases
    TInt SetupAccountL(CStifItemParser& /* aItem */);
    TInt GoOnlineL(CStifItemParser& /* aItem */);
    TInt GoOfflineL(CStifItemParser& /* aItem */);
    TInt RefreshNowL(CStifItemParser& /* aItem */);
    TInt CancelSyncL(CStifItemParser& /* aItem */);
    TInt ListStandardFoldersL(CStifItemParser& /* aItem */);

    // list folders from all mailboxes
    TInt ListFoldersL(CStifItemParser& /* aItem */);
    // lists  messages in folders of each found mailbox
    TInt ListMessagesL(CStifItemParser& /* aItem */);
    // Fetch the specified messages
    TInt FetchMessagesL(CStifItemParser& /* aItem */);
    // Send out a mail
    TInt SendMessageL(CStifItemParser& /* aItem */);
    // Basic operation of user for creating email
    TInt BasicMsgOperationsL(CStifItemParser& /* aItem */);
    // Copy or move message between folders
    TInt CopyMoveMsgsL(CStifItemParser& /* aItem */);
    // Search data from messages
    TInt SearchL(CStifItemParser& /* aItem */);
    // Add a attachment to new email
    TInt AttachmentHandlingL(CStifItemParser& /* aItem */);
    // check if the mailbox is a nokia branding one.
    TInt BrandingL(CStifItemParser& /* aItem */);
    // Delete message from Inbox
    TInt DeleteMsgsL(CStifItemParser& /* aItem */);
    //remove mailbox
    TInt RemoveAccountL(CStifItemParser& /* aItem */);

    /**
     * Method used to log version of test class
     */
    void SendTestClassVersion();

    CFSMailMessage* CreatePlainTextMsgL(const TDesC& aSubject,
            const TDesC& aMsgText);
    TInt MoveMessageToFolderL(CFSMailMessage& aMsg, TFSFolderType aFolderType);

    TInt GetMessagesFromFolderL(RPointerArray<CFSMailMessage>& aMessages,
            CFSMailFolder* aFolder, TFSMailDetails aDet,
            TFSMailSortField aSort, TUint aCount);

    TInt WaitForEvent(TFSMailEvent aWaitedEvent, TAny *aEventParam1 = NULL,
            TAny *aEventParam2 = NULL, TInt aTimeout = 60);

    TInt WaitForResponse(TFSProgress::TFSProgressStatus aWaitedResponse,
            TInt aTimeout = 60);

    void OpComplete();

    TInt InitMailboxL();

    TInt DeleteTestMsgL(TDesC& aFolderName);
    TInt ReadAccountSettings(TAccountSetting& aAccountSetting);

    TInt WriteToWizardCRL(TAccountSetting aAccountSetting);

    void LogEvent(const TDesC& aLogText, TFSMailEvent aEvents, TAny* aParam1,
            TAny* aParam2, TAny* /*aParam3*/);
    void LogTFSProgress(const TDesC& aLogText,
            const TFSProgress::TFSProgressStatus aProgress, TInt aReqId);

    void LogTSSMailSyncState(const TDesC& aLogText,
            const TSSMailSyncState aState);

    TInt DoSearch(const RPointerArray<TDesC>& aSearchStrings,
            const TFSMailSortCriteria& aSortCriteria, TInt aTimeout);

    TInt SendMsgL(CFSMailMessage& aMsg, const TDesC &aSubject,
            TFSMailMsgId& aGotMsgId);

    TInt GetFolderNameFromId(TFSMailMsgId aFolderId, TDes& aFolderName);

    TFSMailMsgId GetFolderIdFromName(const TDesC& aFolderName);

    TBool ParseEventParams(TAny *aEventParam1, TAny *aEventParam2);

    CFSMailFolder* FindFolder(const TDesC& aFolderName);

public:
    // Data


protected:
    // Data


private:
    // Data
    TBool iEventSynchronousCall;
    CFSMailClient* iMailClient;

    CFSMailBox* iIPSMailbox;

    //variables for mfsMailEvent
    TBool iTimeout;
    TInt iErr;

    CTimeoutTimer* iTimeoutTimer;
    TFSMailEvent iCurrWaitedEvent;
    TAny* iEventParam1;
    TAny* iEventParam2;

    TFSProgress::TFSProgressStatus iCurrWaitedResponse;

    //OWN: Active scheduler wait and Cb received indication
    CActiveSchedulerWait* iWait;
    TWaitingState iWaitingState;
    TFSMailMsgId iPluginId;

    //checking status of searching
    TBool iSearchOngoing;
    TInt iSearchMatches;

public:
    // Friend classes

protected:
    // Friend classes

private:
    // Friend classes


    };

#endif      // POPIMAPPROFILETESTER_H
// End of File
