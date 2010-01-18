/*
* ============================================================================
*  Name     : CTestAppAppUi from TestAppAppUi.h
* ============================================================================
*/

#ifndef TESTAPPAPPUI_H
#define TESTAPPAPPUI_H

// INCLUDES
#include <aknappui.h>
#include <msvapi.h>
#include <Imumdasettingskeys.h>

// FORWARD DECLARATIONS
class CTestAppContainer;

const TInt TTestAppUiIpsBufLength = 200;

// CLASS DECLARATION

// Name of the ini KIpsSosTestIni
//_LIT( KIpsSosTestFilePath, "c:\\data\\ipssostestcases\\" );
_LIT( KIpsSosTestFilePath, "c:\\" );
_LIT( KIpsSosTestFileName, "ipssostest" );
_LIT( KIpsSosTestFileExtension, ".ini" );

// Literals used in ini
_LIT8( KIpsSosTestProtocol, "Protocol" );
_LIT8( KIpsSosTestIMAP, "IMAP4" );
_LIT8( KIpsSosTestPOP, "POP3" );
_LIT8( KIpsSosTestMailboxName, "MailboxName" );
_LIT8( KIpsSosTestEmailAddress, "EmailAddress" );
_LIT8( KIpsSosTestUserName, "UserName" );
_LIT8( KIpsSosTestPassword, "Password" );
_LIT8( KIpsSosTestUserAlias, "UserAlias" );
_LIT8( KIpsSosTestSignature, "Signature" );
_LIT8( KIpsSosTestFolderPath, "FolderPath" );
_LIT8( KIpsSosTestAccessPoint, "AccessPoint" );
_LIT8( KIpsSosTestIncomingServerAddress, "IncomingServerAddress" );
_LIT8( KIpsSosTestIncomingPort, "IncomingPort" );
_LIT8( KIpsSosTestIncomingSecurity, "IncomingSecurity" );
_LIT8( KIpsSosTestOutgoingServerAddress, "OutgoingServerAddress" );
_LIT8( KIpsSosTestOutgoingPort, "OutgoingPort" );
_LIT8( KIpsSosTestOutgoingSecurity, "OutgoingSecurity" );
_LIT8( KIpsSosTestPortValueDefault, "Default" );
_LIT8( KIpsSosTestPortValueUserDefined, "UserDefined" );
_LIT8( KIpsSosTestAPAlwaysAsk, "AlwaysAsk" );
_LIT8( KIpsSosTestSecurityTls, "Tls" );
_LIT8( KIpsSosTestSecuritySsl, "Ssl" );


// Ini-file commentline identifier
_LIT8( KIpsSosTestIniCommentLine, "//");
// Ini-file key value separator
_LIT8( KIpsSosTestIniSeparator, "=");
// Ini-file hex value identifier
_LIT8( KIpsSosTestIniHexMark, "0x");

// Used with panic if error occurs
_LIT( KIpsSosTestPanicText, "KIpsSosTestPanic" );

// Notes
_LIT( KIpsSosTestRecentNotFound, "Recent view not found" );
_LIT( KIpsSosTestErrorLaunching, "Error in plugin launching" );
_LIT( KIpsSosTestErrorInWriting, "Error in settins writing" );
_LIT( KIpsSosTestSettingsCreated, "Settings created" );
_LIT( KIpsSosTestFileNotFound, "File not found" );              
_LIT( KIpsSosTestErrorInReading, "Error in settings reading" );
_LIT( KIpsSosTestUnknownValue, "Unknown settings key value");
                                                            
// Used as delimeter in ini-file
const TInt KIpsSosTestLineFeed = 0x000D;

// Used for removing delimeter 
const TInt KIpsSosTestDelimeterSize = 1;
 
// Max length of line in ini-file
const TInt KIpsSosTestMaxLineLength = 128; 

// Buf lenght for format
const TInt KIpsSosTestFormatBufLength = 50;

const TInt KIpsSosTestDefaultFile = 0;

const TInt KIpsSosTestPortDefault = 0;

const TInt KIpsSosTestAlwaysAskAp = 0;

// P&S KEY and categories
enum TIpsSosTestPropertyKeys 
    {
	EIpsSosTestRecentMsvId = 1
    };    

const TUid KIpsSosTestPropertyCat = {0x10012349}; 

/**
* Application UI class.
* 
*/
class CTestAppAppUi : public CAknAppUi, public MMsvSessionObserver                            
    {
    public: // // Constructors and destructor
        
        /**
        * Default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CTestAppAppUi();
        
    public: //from MIdleFindObserver
        void IdleFindCallback();
        
    public: // from MMsvSessionObserver
        
        void HandleSessionEventL(
                TMsvSessionEvent aEvent, 
                TAny* aArg1, 
                TAny* aArg2, 
                TAny* aArg3 );

    private: // New functions          
        //Put 3 pointer in CleanupStack        
        void DebugL( TRefByValue<const TDesC> aText, ...);
        void TimeDebugL( TTime aStartTime, TTime aEndTime );
    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
    
        void LaunchPluginL();
        void WriteSettingsViaODSUtilityL();
        void WriteSettingsToWizardCenRepL();
        void WriteSettingsViaImumL();
        void ReadSettingsL( TInt aFileId );
        void DeleteAllSettingsL();
        void SetRecentMsvIdL( TMsvId aMsvId );
        TMsvId RecentMsvId();        
        void ShowNoteL( const TDesC& aMessage );


        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: // Data
        // Own:
        CTestAppContainer* iAppContainer; 
        
        TMsvId                                  iMailboxId;
        TInt32                                  iProtocol;
        TBuf<TTestAppUiIpsBufLength>            iMailboxName;
        TBuf<TTestAppUiIpsBufLength>            iEmailAddress;
        TBuf<TTestAppUiIpsBufLength>            iIncomingServerAddress;
        TBuf<TTestAppUiIpsBufLength>            iUserName;
        TBuf<TTestAppUiIpsBufLength>            iPassword;
        TBuf<TTestAppUiIpsBufLength>            iUserAlias;
        TBuf<TTestAppUiIpsBufLength>            iSignature;
        TBuf<TTestAppUiIpsBufLength>            iFolderPath;
        TInt                                    iIncomingPort;
        TBuf<TTestAppUiIpsBufLength>            iAccessPoint;
        TInt                                    iIncomingSecurity;
        TInt                                    iOutProtocol;
        TBuf<TTestAppUiIpsBufLength>            iOutgoingServerAddress;
        TInt                                    iOutgoingPort;
        TInt                                    iOutgoingSecurity;

    };

#endif  // TESTAPPAPPUI_H

// End of File
