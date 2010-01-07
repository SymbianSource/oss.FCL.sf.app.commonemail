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
#include <BrCtlSpecialLoadObserver.h>
#include <BrCtlLinkResolver.h>

#include "FreestyleEmailDownloadInformationMediator.h"

class CFSMailMessage;
class CFreestyleEmailUiAppUi;
class CFreestyleMessageHeaderURLEventHandler;

/**
 * Html viewer container.
 */
class CFsEmailUiHtmlViewerContainer : public CCoeControl,
                                      public MBrCtlSpecialLoadObserver,
                                      public MBrCtlLinkResolver,
                                      public MBrCtlSoftkeysObserver,
                                      public MFSEmailDownloadInformationObserver
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
    void LoadContentFromMailMessageL( CFSMailMessage* aMailMessage );
    void ResetContent();
    void CancelFetch();
    void ClearCacheAndLoadEmptyContent();
    // <cmail>
    void PrepareForExit();
    // </cmail>
    
// from base class CCoeControl

    CCoeControl* ComponentControl( TInt aIndex ) const;
    TInt CountComponentControls() const;
    void Draw( const TRect& aRect ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void HandleResourceChange(TInt aType);

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
private:

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
    void CopyFileToHtmlFileL( RFile& aFile, const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );
    // Reads given file content to buffer and return pointer to it
    HBufC8* ReadContentFromFileLC( RFile& aFile );
    // Writes buffer to given file
    void WriteContentToFileL( const TDesC8& aContent, const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );
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
    
    void SetHTMLResourceFlagFullName();
    void EnableHTMLResourceFlagL();
    TBool HTMLResourceFlagEnabled();
    void CopyHTMLResourceL();
    // Ensure all resource files (eg. images) required by HTML display to be ready in C:\[private]\HtmlFile
    void EnsureHTMLResourceL(); 
    void ConvertPlainTextToHTML(CFSMailMessagePart* aTextBodyPart, RFile htmlFile);
    void CopyFileToHtmlFileL( const TDesC8& aBuffer,
            const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );
    void ConvertToHTML( const TDesC8& aContent,
            const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart );
    HBufC8* GetCharacterSetL( CFSMailMessagePart& aHtmlBodyPart );
    

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
    };


#endif // __FREESTYLEEMAILUI_HTML_VIEWER_CONTAINER_H__
