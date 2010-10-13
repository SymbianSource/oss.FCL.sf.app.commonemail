/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CFsEmailUiHtmlViewerContainer class definition
*
*/
 


#ifndef __FREESTYLEEMAILUI_HTML_VIEWER_CONTAINER_H__
#define __FREESTYLEEMAILUI_HTML_VIEWER_CONTAINER_H__


#include <coecntrl.h>
#include <brctlspecialloadobserver.h>
#include <brctllinkresolver.h>

#include "FreestyleEmailUiAknStatusIndicator.h"
#include "FreestyleEmailDownloadInformationMediator.h"
#include "freestyleemailcenrepkeys.h"
#include "OverlayControl.h"

class CFSMailMessage;
class CFreestyleEmailUiAppUi;
class CFreestyleMessageHeaderURLEventHandler;
struct TAttachmentData;
class CFsEmailUiHtmlViewerContainer;
class MTouchFeedback;
class CFindItemEngine;

/**
 * Single key listener
 */
class CEUiHtmlViewerSettingsKeyListener : public CActive
    {
public:
    /**
     * Observer interface for key value changes
     */
    class MObserver
        {
    public:
        /**
         * Called when key value changes
         */
        virtual void KeyValueChangedL( TUint32 aKey ) = 0;

        /**
         * Listener can query repository instance using this method. No need
         * to create own repository instance OR to store repository reference.
         */
        virtual CRepository& Repository() = 0;
        };
public:
    /**
     * Constructor
     */
    CEUiHtmlViewerSettingsKeyListener( MObserver& aObserver, TUint32 aKey );

    /**
     * Destructor
     */
    ~CEUiHtmlViewerSettingsKeyListener();

private:
    /**
     * Start listening value changes
     */
    void StartListening();

private: // from CActive
    /**
     * @see CActive::RunL
     */
    void RunL();

    /**
     * @see CActive::DoCancel
     */
    void DoCancel();

private:
    /**
     * Key observer
     */
    MObserver& iObserver;

    /**
     * Key id
     */
    TUint32 iKey;
    };


/**
 * HTML viewer settings class.
 */
class CEUiHtmlViewerSettings : public CBase, public CEUiHtmlViewerSettingsKeyListener::MObserver
    {

public:

    /**
     * Settings observer interface
     */
    class MObserver
        {
    public:
        /**
         * Called when a setting value has changed.
         */
        virtual void ViewerSettingsChangedL( const TUint32 aKey ) = 0;
        };

public:

    /**
     * Static constructor
     */
    static CEUiHtmlViewerSettings* NewL( MObserver& aObserver );

    /**
     * Destructor
     */
    ~CEUiHtmlViewerSettings();

    /**
     * Boolean value for auto load image setting
     */
    TBool AutoLoadImages() const;

private:

    CEUiHtmlViewerSettings( MObserver& aObserver );
    void ConstructL();
    void AddKeyListenerL( TUint32 aKey );
    void UpdateValue( TUint32 aKey );

private: // from CEUiHtmlViewerSettingsKeyListener::MObserver

    /**
     * @see CEUiHtmlViewerSettingsKeyListener::MObserver::KeyValueChangedL
     */
    void KeyValueChangedL( TUint32 aKey );

public:    
    /**
     * @see CEUiHtmlViewerSettingsKeyListener::MObserver::Repository
     */
    CRepository& Repository();

private:

    enum TFlag
        {
        EAutoLoadImages = KFreestyleEmailDownloadHTMLImages
        };

    MObserver& iObserver;
    CRepository* iRepository;
    TBitFlags iFlags;
    RPointerArray<CEUiHtmlViewerSettingsKeyListener> iKeyListeners;
    };

/**
 * Html viewer container.
 */
class CFsEmailUiHtmlViewerContainer : public CCoeControl,
                                      public MBrCtlSpecialLoadObserver,
                                      public MBrCtlLinkResolver,
                                      public MBrCtlSoftkeysObserver,
                                      public MFSEmailDownloadInformationObserver,
                                      public MOverlayControlObserver,
                                      public MBrCtlWindowObserver,
                                      public CEUiHtmlViewerSettings::MObserver
    {
public:

    /** Two-phased constructor. */
    static CFsEmailUiHtmlViewerContainer* NewL( CFreestyleEmailUiAppUi& aAppUi,
        CFsEmailUiHtmlViewerView& aView );
    /** Destrcutor. */
    ~CFsEmailUiHtmlViewerContainer();

    CBrCtlInterface* BrowserControlIf();
    void LoadContentFromFileL( const TDesC& aFileName );
    void LoadContentFromFileL( RFile& aFile );
    void LoadContentFromUrlL( const TDesC& aUrl );
    void LoadContentFromMailMessageL( CFSMailMessage* aMailMessage, TBool aResetScrollPos=ETrue );
    void ResetContent(const TBool aDisconnect = EFalse);
    void CancelFetch();
    void ClearCacheAndLoadEmptyContent();
    // <cmail>
    void PrepareForExit();
    void PrepareForMessageNavigation();
    // </cmail>
    
// from base class CCoeControl

    CCoeControl* ComponentControl( TInt aIndex ) const;
    TInt CountComponentControls() const;
    void Draw( const TRect& aRect ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandleResourceChange(TInt aType);
    void MakeVisible( TBool aVisible );

// from base class MBrCtlSpecialLoadObserver

    void NetworkConnectionNeededL( TInt* aConnectionPtr, TInt* aSockSvrHandle,
        TBool* aNewConn, TApBearerType* aBearerType );
    TBool HandleRequestL( RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray );
    TBool HandleDownloadL( RArray<TUint>* aTypeArray, CDesCArrayFlat* aDesArray );

// from base class MFSEmailDownloadInformationObserver

    void RequestResponseL( const TFSProgress& aEvent, const TPartData& aPart );

// from base class MBrCtlLinkResolver

    TBool ResolveEmbeddedLinkL( const TDesC& aEmbeddedUrl,
        const TDesC& aCurrentUrl, TBrCtlLoadContentType aLoadContentType, 
        MBrCtlLinkContent& aEmbeddedLinkContent ); 
    TBool ResolveLinkL( const TDesC& aUrl, const TDesC& aCurrentUrl,
        MBrCtlLinkContent& aBrCtlLinkContent );
    void CancelAll();

// from base class MBrCtlSoftkeysObserver

    void UpdateSoftkeyL( TBrCtlKeySoftkey aKeySoftkey, const TDesC& aLabel,
        TUint32 aCommandId, TBrCtlSoftkeyChangeReason aBrCtlSoftkeyChangeReason );
    void StopObserving();
    
// from MOverlayControlObserver
    void HandleOverlayPointerEventL( COverlayControl* aControl, const TPointerEvent& aEvent );

    /*
     * Reloads the header of the opened mail.
     */
    void RefreshCurrentMailHeader();
    /** Handles asynchronous reload of page for case when orientation
     * changed
     */
    void ReloadPageL();
    
    void ShowAttachmentDownloadStatusL( TFSProgress::TFSProgressStatus aProgressStatus, const TAttachmentData& aAttachmentData );
    TBool AttachmentDownloadStatusVisible();
    void HideDownloadStatus();
    void DisplayStatusIndicatorL(TInt aDuration = KStatusIndicatorDefaultDuration);
    
    // Inform that maillist model has updated
    void MailListModelUpdatedL();
    
    // Zoom handling
    void ZoomInL();
    void ZoomOutL();
    TInt ZoomLevelL() const;
    void SetZoomLevelL( const TInt aZoomLevel );
    TInt MaxZoomLevel() const;

private:
    
    static TInt DoZoom( TAny* aPtr  );
    void DoZoomL();
    
private: // from CEUiHtmlViewerSettings::MObserver

    /**
     * @see CEUiHtmlViewerSettings::MObserver::ViewerSettingsChangedL
     */
    void ViewerSettingsChangedL( const TUint32 aKey );

private:

    void CreateBrowserControlInterfaceL();
    
    // Second phase constructor.
    void ConstructL();
    // C++ constructor.
    CFsEmailUiHtmlViewerContainer( CFreestyleEmailUiAppUi& aAppUi,
        CFsEmailUiHtmlViewerView& aView );

    // Set HTML folder path name
    void SetHtmlFolderPathL();
    // Sets temporary HTML folder path name
    void SetTempHtmlFolderPath();
    // Remove all previously created files from temporary HTML folder
    void EmptyTempHtmlFolderL();
    // Copies given file to temporary HTML folder and returns an open file
    // handle to the created copy.
// <cmail>
    void CopyToHtmlFileL( CFSMailMessagePart& aHtmlBodyPart, const TDesC& aFileName );
    void ConvertToHtmlFileL( CFSMailMessagePart& aTextBodyPart, const TDesC& aHtmlFileName );

    // Reads given file content to buffer and return pointer to it
    HBufC8* ReadContentFromFileLC( RFile& aFile, CFSMailMessagePart& aBodyPart );
    // Writes buffer to given file
    void WriteContentToFileL( const TDesC8& aContent, const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );

    void PrepareBodyHtmlL(  const TDesC& aFileName  );
    void WriteEmptyBodyHtmlL( const TDesC& aFileName );
    
// </cmail>
    // Finds the attachment from the list that matches the given content ID 
    CFSMailMessagePart* MatchingAttacmentL( const TDesC& aContentId,
        const RPointerArray<CFSMailMessagePart>& aAttachments ) const;
    // Resolves embedded link referring to file in temporary HTML folder
    TBool ResolveLinkL( const TDesC& aLink, TBool aContentId,
        MBrCtlLinkContent& aEmbeddedLinkContent );
    // Downloads and return attachment content via MBrCtlLinkContent interface
    void DownloadAttachmentL( CFSMailMessagePart& aAttachment,
        MBrCtlLinkContent& aEmbeddedLinkContent );
    
    // Get area for overlay button
    TRect OverlayButtonRect( TBool aLeft );
    
    // Update overlay button position and visibility
    void UpdateOverlayButtons( TBool aVisible );
    
    void SetHTMLResourceFlagFullName();
    void EnableHTMLResourceFlagL();
    TBool HTMLResourceFlagEnabled();
    void CopyHTMLResourceL();
    // Ensure all resource files (eg. images) required by HTML display to be ready in C:\[private]\HtmlFile
    void EnsureHTMLResourceL(); 
    void ConvertToHTML( const TDesC& aContent,
            const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );
    HBufC8* GetCharacterSetL( CFSMailMessagePart& aHtmlBodyPart );
    TBool IsMessageBodyURLL(const TDesC& aUrl);
	TInt TotalLengthOfItems( CFindItemEngine& aItemEngine ) const;
	
    //Returns ETrue of clicking on a link requires a browser to be launched
    TBool NeedToLaunchBrowserL( const TDesC& aUrl );
    //Launch the browser as a standalone app
    void LaunchBrowserL( const TDesC& aUrl );
    
    //from MBrCtlWindowObserver    
    CBrCtlInterface* OpenWindowL( TDesC& aUrl, TDesC* aTargetName, 
                                  TBool aUserInitiated, TAny* aReserved );
    CBrCtlInterface* FindWindowL( const TDesC& aTargetName ) const;  
    void HandleWindowCommandL( const TDesC& aTargetName, TBrCtlWindowCommand aCommand ); 
       
    TRect CalcAttachmentStatusRect();
    void TouchFeedback();
    
    void WriteToFileL( const TDesC& aFileName, RBuf& aHtmlText );

    HBufC* CreateModifiedUrlIfNeededL( const TDesC& aUrl );
    
private: // data

    // Reference to AppUi.
    CFreestyleEmailUiAppUi& iAppUi;
    // Reference to Html viewer view.
    CFsEmailUiHtmlViewerView& iView;
    // Browser control interface. Own.
    CBrCtlInterface* iBrCtlInterface;
    // Message. Not own.
    CFSMailMessage*  iMessage;
    RFile            iFile;
    RFs              iFs;
    RSocketServ      iSocketServer;
    RConnection      iConnection;
    TBool            iFirstTime;
    
    // HTML folder path.
    TPath iHtmlFolderPath;
    // Temporary HTML folder path.
    TPath iTempHtmlFolderPath;
    // HTML resource flag file path
    TPath iHtmlResourceFlagPath;
    
    // Interfaces waiting for embedded link content.
    RPointerArray<MBrCtlLinkContent> iLinkContents;
    // Message parts being downloaded as embedded link content.
    RArray<TPartData> iMessageParts;
    CFreestyleMessageHeaderURLEventHandler* iEventHandler;
    TBool iObservingDownload;
    TInt iScrollPosition;
    COverlayControl* iOverlayControlNext;
    COverlayControl* iOverlayControlPrev;
    //way to keep track of the current image being displayed by the indicator
    TInt iAttachmentDownloadImageHandle;
    CFreestyleEmailUiAknStatusIndicator* iStatusIndicator;
    CEUiHtmlViewerSettings* iViewerSettings;
    
    TBool iHeaderExpanded;
    // tactile feed back -- not owned
    MTouchFeedback* iTouchFeedBack;
    TBitFlags iFlags;
    TInt iZoomLevel;
    };


/**
* PlainTextToHtmlConverter
* 
* PlainTextToHtmlConverter converts plain text to html. It adds html entities
* and hyperlinks.
*/
NONSHARABLE_CLASS (PlainTextToHtmlConverter)
    {
    private:
        PlainTextToHtmlConverter();
        
    public:
        static void PlainTextToHtmlL(const TDesC& aPlainText, RBuf& aHtmlText);
        
    private:
        static void ConvertTextL(const TDesC& aSource, RBuf& aTarget);
        static void ConvertUrlL(const TDesC& aSource, RBuf& aTarget);
    };
#endif // __FREESTYLEEMAILUI_HTML_VIEWER_CONTAINER_H__
