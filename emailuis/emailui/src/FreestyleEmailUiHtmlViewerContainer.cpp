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
* Description:  CFsEmailUiHtmlViewerContainer class implementation
*
*/


#include "emailtrace.h"
#include <AknsDrawUtils.h>
#include <coemain.h>
#include <commdbconnpref.h>
#include <bautils.h>
#include <biditext.h>
#include <StringLoader.h>
#include <e32base.h>
#include <badesca.h>
#include <utf.h>

//<cmail>
#include "cfsmailmessage.h"
//</cmail>
#include <brctlinterface.h>
#include <sysutil.h>
#include <schemehandler.h>

#include <FreestyleEmailUi.rsg>

#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiHtmlViewerContainer.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUiShortcutBinding.h"

#include "FreestyleMessageHeaderHTML.h"
#include "FreestyleMessageHeaderURLEventHandler.h"
#include "FreestyleEmailUiAttachmentsListModel.h"

_LIT( KContentIdPrefix, "cid:" );
_LIT( KCDrive, "c:" );
_LIT( KHtmlPath, "HtmlFile\\" );
_LIT( KTempHtmlPath, "temp\\" );
_LIT( KAllFiles, "*" );

_LIT( KHeaderHtmlFile, "header.html" );
_LIT( KBodyHtmlFile, "body.html" );
_LIT( KMessageHtmlFile, "email.html" );
_LIT( KMessageHtmlRTLFile, "email_rtl.html" );
_LIT( KZDrive, "z:" );
_LIT( KHtmlFlagFile, "html.flag" );
_LIT( KURLSlash, "/");
// Constants used in html content modification
const TInt KMaxCharsToSearch( 200 );
_LIT8( KStartTag, "<html" );
_LIT8( KHeadTag, "<head>");
_LIT8( KHtmlHeader1, "<html><head><title></title><meta http-equiv=\"Content-Type\" content=\"text/html; charset=");
_LIT8( KHtmlHeader2, "\"/></head><body>\xD\xA");
_LIT8( KHtmlHeader3, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%S\">\n");
_LIT8( KHtmlEndTags, "\xD\xA</body></html>\xD\xA");
_LIT8( KCharsetTag8, "charset");
_LIT( KCharsetTag, "charset");
_LIT8( KHTMLEmptyContent, "<HTML><BODY></BODY></HTML>");
_LIT( KHTMLDataScheme, "data:0");


_LIT( KHtmlLineBreak, "<br>" );
_LIT( KHtmlLineBreakCRLF, "<br>\xD\xA" );
_LIT( KHtmlLessThan, "&lt;" );
_LIT( KHtmlGreaterThan, "&gt;" );
_LIT( KHtmlAmpersand, "&amp;" );
_LIT( KHtmlQuotation, "&quot;" );
// _LIT( KHtmlLinkTag, "<a href=\"%S\">" );
// _LIT( KHtmlLinkTagWWW, "<a href=\"%S%S\">" );
// _LIT( KHtmlLinkEndTag, "</a>" );
_LIT( KURLTypeBody, "body");

_LIT( KURLDisplayImages, "cmail://displayImages/" );
_LIT( KURLLoadImages, "cmail://loadImages/" );
_LIT( KURLCollapseHeader, "cmail://collapseHeader/" );
_LIT( KURLExpandHeader, "cmail://expandHeader/" );

const TText KGreaterThan = 0x3e;
const TText KLessThan = 0x3c;
const TText KAmpersand = 0x26;
const TText KQuotation = 0x22;
// const TText KCharacterSpace = 0x20;
const TText KSOH = 0x01; // Start Of Heading
// const TText KCR = 0x0d; // Carriage Return
const TText KLF = 0x0a; // Line Feed
// const TText KHT = 0x09; // Horizontal Tab
const TText KUnicodeNewLineCharacter = 0x2028;
const TText KUnicodeParagraphCharacter = 0x2029;        
const TReal KOverlayButtonMarginX = 0.01; // 1%
const TReal KOverlayButtonMarginY = 0.01; // 1%
const TReal KOverlayButtonSizeP = 0.15; // 15%
const TReal KOverlayButtonSizeLs = 0.20; // 25%


const TInt KStatusIndicatorHeight = 50;
const TInt KStatusIndicatorXMargin = 50;

// CEUiHtmlViewerSettingsKeyListener

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEUiHtmlViewerSettingsKeyListener::CEUiHtmlViewerSettingsKeyListener( MObserver& aObserver, TUint32 aKey )
    : CActive( EPriorityStandard ), iObserver( aObserver ), iKey( aKey )
    {
    CActiveScheduler::Add(this);
    StartListening();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEUiHtmlViewerSettingsKeyListener::~CEUiHtmlViewerSettingsKeyListener()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettingsKeyListener::StartListening()
    {
    SetActive();
    iObserver.Repository().NotifyRequest(iKey, iStatus);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettingsKeyListener::RunL()
    {
    iObserver.KeyValueChangedL(iKey);
    StartListening();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettingsKeyListener::DoCancel()
    {
    iObserver.Repository().NotifyCancel(iKey);
    }

// CEUiHtmlViewerSettings

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEUiHtmlViewerSettings* CEUiHtmlViewerSettings::NewL( MObserver& aObserver )
    {
    CEUiHtmlViewerSettings* self = new (ELeave) CEUiHtmlViewerSettings(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEUiHtmlViewerSettings::~CEUiHtmlViewerSettings()
    {
    iKeyListeners.ResetAndDestroy();
    delete iRepository;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CEUiHtmlViewerSettings::CEUiHtmlViewerSettings( MObserver& aObserver ) : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettings::ConstructL()
    {
    iRepository = CRepository::NewL(KFreestyleEmailCenRep);
    AddKeyListenerL(KFreestyleEmailDownloadHTMLImages);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettings::AddKeyListenerL( TUint32 aKey )
    {
    CEUiHtmlViewerSettingsKeyListener* listener = new (ELeave) CEUiHtmlViewerSettingsKeyListener(*this, aKey);
    CleanupStack::PushL(listener);
    iKeyListeners.AppendL(listener);
    CleanupStack::Pop(); // listener
    UpdateValue(aKey);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettings::UpdateValue( TUint32 aKey )
    {
    TInt value;
    iRepository->Get(aKey, value);
    switch (aKey)
        {
        case KFreestyleEmailDownloadHTMLImages:
            // 0 = automatic, 1 = ask always
            iFlags.Assign(aKey, value == 0);
            break;
        default:
            iFlags.Assign(aKey, value);
            break;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CEUiHtmlViewerSettings::KeyValueChangedL( TUint32 aKey )
    {
    UpdateValue(aKey);
    iObserver.ViewerSettingsChangedL(aKey);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRepository& CEUiHtmlViewerSettings::Repository()
    {
    return *iRepository;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CEUiHtmlViewerSettings::AutoLoadImages() const
    {
    return iFlags.IsSet(EAutoLoadImages);
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFsEmailUiHtmlViewerContainer* CFsEmailUiHtmlViewerContainer::NewL(
    CFreestyleEmailUiAppUi& aAppUi, CFsEmailUiHtmlViewerView& aView )
    {
    FUNC_LOG;
    CFsEmailUiHtmlViewerContainer* self =
        new (ELeave) CFsEmailUiHtmlViewerContainer( aAppUi, aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CFsEmailUiHtmlViewerContainer::CFsEmailUiHtmlViewerContainer(
    CFreestyleEmailUiAppUi& aAppUi, CFsEmailUiHtmlViewerView& aView )
    :
    iAppUi( aAppUi ),
    iView( aView ),
    iFs( iCoeEnv->FsSession() ),
    iFirstTime( ETrue )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CFsEmailUiHtmlViewerContainer::~CFsEmailUiHtmlViewerContainer()
    {
    FUNC_LOG;
    delete iViewerSettings;
    if ( iObservingDownload && iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this );
        }
    iFile.Close();
    iLinkContents.Close();
    iMessageParts.Close();
    delete iEventHandler;
    delete iBrCtlInterface;
    iConnection.Close();
    iSocketServer.Close();
    delete iStatusIndicator;
    delete iOverlayControlNext;
    delete iOverlayControlPrev;
    }

void CFsEmailUiHtmlViewerContainer::PrepareForExit()
    {
    FUNC_LOG;
    HideDownloadStatus();
    if ( iObservingDownload && iAppUi.DownloadInfoMediator() )
        {
        iAppUi.DownloadInfoMediator()->StopObserving( this );
        iObservingDownload = EFalse;
        }
    delete iBrCtlInterface;
    iBrCtlInterface = NULL;
    iConnection.Close();
    iSocketServer.Close();
    }

void CFsEmailUiHtmlViewerContainer::PrepareForMessageNavigation()
    {
    FUNC_LOG;
    ResetContent();
    }

void CFsEmailUiHtmlViewerContainer::ConstructL()
    {
    FUNC_LOG;
    iViewerSettings = CEUiHtmlViewerSettings::NewL(*this);

    SetHtmlFolderPathL();
    BaflUtils::EnsurePathExistsL( iFs, iHtmlFolderPath );
    SetTempHtmlFolderPath();
    BaflUtils::EnsurePathExistsL( iFs, iTempHtmlFolderPath );
    SetHTMLResourceFlagFullName();
    EnsureHTMLResourceL();
    
    CreateWindowL();
    SetRect( iView.ContainerRect() );

    TUint brCtlCapabilities = TBrCtlDefs::ECapabilityClientResolveEmbeddedURL |
                              TBrCtlDefs::ECapabilityDisplayScrollBar |
                              TBrCtlDefs::ECapabilityClientNotifyURL |
                              TBrCtlDefs::ECapabilityLoadHttpFw |
                              TBrCtlDefs::ECapabilityCursorNavigation |
                              TBrCtlDefs::ECapabilityPinchZoom;

    // Set browsercontrol to whole screen
    TRect rect( TPoint(), Size() );
   
    iBrCtlInterface = CreateBrowserControlL( 
            this, // aParent 
            rect,  // aRect
            brCtlCapabilities, // aBrCtlCapabilities
            TBrCtlDefs::ECommandIdBase, // aCommandIdBase
            NULL, // aBrCtlSoftkeysObserver
            this, // aBrCtlLinkResolver
            this, // aBrCtlSpecialLoadObserver
            NULL, // aBrCtlLayoutObserver
            NULL, // aBrCtlDialogsProvider
            this, // aBrCtlWindowObserver
            NULL // aBrCtlDownloadObserver 
            );

    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsEmbedded, ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsAutoLoadImages, iViewerSettings->AutoLoadImages() );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsPageOverview, EFalse );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsTextWrapEnabled, ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsFontSize, TBrCtlDefs::EFontSizeLevelLarger );
    
    iEventHandler = CFreestyleMessageHeaderURLEventHandler::NewL( iAppUi, iView );
    
    TRect nextButtonRect = OverlayButtonRect( EFalse );
    iOverlayControlNext = COverlayControl::NewL( this, this, nextButtonRect, 
            EMbmFreestyleemailuiQgn_indi_cmail_arrow_next, 
            EMbmFreestyleemailuiQgn_indi_cmail_arrow_next_mask );

    TRect prevButtonRect = OverlayButtonRect( ETrue );
    iOverlayControlPrev = COverlayControl::NewL( this, this, prevButtonRect,
            EMbmFreestyleemailuiQgn_indi_cmail_arrow_previous,
            EMbmFreestyleemailuiQgn_indi_cmail_arrow_previous_mask );
    iScrollPosition = 0;
    iAttachmentDownloadImageHandle = 0;

    iTouchFeedBack = MTouchFeedback::Instance();
    iTouchFeedBack->EnableFeedbackForControl(this, ETrue);

    ActivateL();
    }

void CFsEmailUiHtmlViewerContainer::MakeVisible( TBool aVisible )
    {
    UpdateOverlayButtons( aVisible );
    CCoeControl::MakeVisible( aVisible );
    }

void CFsEmailUiHtmlViewerContainer::HandleOverlayPointerEventL( COverlayControl* aControl, 
        const TPointerEvent& aEvent )
    {
    if( aEvent.iType == TPointerEvent::EButton1Up )
        {
        if( aControl == iOverlayControlNext )
            {
            iView.HandleCommandL( EFsEmailUiCmdNextMessage );
            }
        else if( aControl == iOverlayControlPrev )
            {
            iView.HandleCommandL( EFsEmailUiCmdPreviousMessage );
            }
        }
    }

void CFsEmailUiHtmlViewerContainer::UpdateOverlayButtons( TBool aVisible )
    {
    TBool nextAvailable = EFalse;
    TBool prevAvailable = EFalse;
    if( iMessage )
        {
        TFSMailMsgId currentMsgId = iMessage->GetMessageId();
        TFSMailMsgId tmpMsgId;
        TFSMailMsgId tmpMsgFolderId;
        nextAvailable = iAppUi.IsNextMsgAvailable( currentMsgId, tmpMsgId, tmpMsgFolderId );
        prevAvailable = iAppUi.IsPreviousMsgAvailable( currentMsgId, tmpMsgId, tmpMsgFolderId );
        }
    
    if( iOverlayControlPrev )
        {
        iOverlayControlPrev->SetRect( OverlayButtonRect( ETrue ) );
        iOverlayControlPrev->MakeVisible( aVisible && prevAvailable );
        }
    if( iOverlayControlNext )
        {
        iOverlayControlNext->SetRect( OverlayButtonRect( EFalse ) );
        iOverlayControlNext->MakeVisible( aVisible && nextAvailable );
        }
    }

// Get rect for button
TRect CFsEmailUiHtmlViewerContainer::OverlayButtonRect( TBool aLeft )
    {
    TRect rect = Rect();
    TSize size = rect.Size();
    
    TBool landscape = size.iWidth > size.iHeight;
    TInt buttonSize;

    if( landscape )
        {
        buttonSize = size.iHeight * KOverlayButtonSizeLs;
        }
    else
        {
        buttonSize = size.iWidth * KOverlayButtonSizeP;
        }

    rect.iBr.iY = size.iHeight * (1-KOverlayButtonMarginY);
    
    if( aLeft )
        {
        rect.iTl.iX = size.iWidth * KOverlayButtonMarginX;
        rect.iBr.iX = rect.iTl.iX + buttonSize;
        }
    else
        {
        rect.iBr.iX = size.iWidth * (1 - KOverlayButtonMarginX);
        rect.iTl.iX = rect.iBr.iX - buttonSize;
        }
    
    rect.iTl.iY = rect.iBr.iY - buttonSize;
    return rect;
    }

// Getter for br contro, interface
CBrCtlInterface* CFsEmailUiHtmlViewerContainer::BrowserControlIf()
    {
    FUNC_LOG;
    return iBrCtlInterface;
    }

void CFsEmailUiHtmlViewerContainer::LoadContentFromFileL( const TDesC& aFileName )
    {
    FUNC_LOG;
    iBrCtlInterface->LoadFileL( aFileName );
    }

void CFsEmailUiHtmlViewerContainer::LoadContentFromFileL( RFile& aFile )
    {
    FUNC_LOG;
    iBrCtlInterface->LoadFileL( aFile );
    }

void CFsEmailUiHtmlViewerContainer::LoadContentFromUrlL( const TDesC& aUrl )
    {
    FUNC_LOG;
    iBrCtlInterface->LoadUrlL( aUrl );
    }

// ---------------------------------------------------------------------------
// Loads content from given mail message.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::LoadContentFromMailMessageL(
        CFSMailMessage* aMailMessage, TBool aResetScrollPos )
    {
    FUNC_LOG;
    ASSERT( aMailMessage );
    iMessage = aMailMessage;
    // Cancel any browser fetch operation, just in case the browser is still
    // loading a previous message (since we are about to overwrite it).
    CancelFetch();
    
    TPath headerHtmlFile;
    headerHtmlFile.Copy( iHtmlFolderPath );
    headerHtmlFile.Append( KHeaderHtmlFile );
    
    // insert email header into email.html file
    // CFreestyleMessageHeaderHTML will replace contents of email.html
    // So, no need to clear the contents
    if(aResetScrollPos)
        {
        iScrollPosition = 0; 
        }
    const TInt visibleWidth(iAppUi.ClientRect().Width());
    CFreestyleMessageHeaderHTML::ExportL( *iMessage, iFs, headerHtmlFile, visibleWidth, iScrollPosition,
                                            iViewerSettings->AutoLoadImages() || iAppUi.DisplayImagesCache().Contains(*iMessage),
                                            iHeaderExpanded );
    
    // Remove all previously created files from temporary HTML folder
    EmptyTempHtmlFolderL();

    TRAPD( error, PrepareBodyHtmlL( KBodyHtmlFile ) );
    if ( error != KErrNone )
        {
        WriteEmptyBodyHtmlL( KBodyHtmlFile );
        }

    // pass the emailHtmlFile to the browser for it to load
    TPath emailHtmlFile;
    emailHtmlFile.Copy( iHtmlFolderPath );
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        emailHtmlFile.Append( KMessageHtmlFile );
        }
    else
        {
        emailHtmlFile.Append( KMessageHtmlRTLFile );
        }
    LoadContentFromFileL( emailHtmlFile );
    
    UpdateOverlayButtons( ETrue );
    }

// ---------------------------------------------------------------------------
// Reset content
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::ResetContent(const TBool aDisconnect)
    {
    FUNC_LOG;
    if ( iBrCtlInterface )
        {
        TRAP_IGNORE( 
                iBrCtlInterface->HandleCommandL( ( TInt )TBrCtlDefs::ECommandIdBase +
                        ( TInt )TBrCtlDefs::ECommandFreeMemory ) );
        if (aDisconnect)
            {
            TRAP_IGNORE( 
                    iBrCtlInterface->HandleCommandL( ( TInt )TBrCtlDefs::ECommandIdBase +
                                                      ( TInt )TBrCtlDefs::ECommandDisconnect ) );
            }
        }
    iFile.Close();
    iLinkContents.Reset();
    iMessageParts.Reset();
    iMessage = NULL;
    iHeaderExpanded = EFalse;
    iScrollPosition = 0;
    }

// ---------------------------------------------------------------------------
// From CCoeControl.
// ---------------------------------------------------------------------------
//
CCoeControl* CFsEmailUiHtmlViewerContainer::ComponentControl( TInt aIndex ) const
    {
    FUNC_LOG;
    switch ( aIndex )
        {
        case 0:
            {
            return iBrCtlInterface;
            }
        case 1:
            {
            return iOverlayControlNext;
            }
        case 2:
            {
            return iOverlayControlPrev;
            }
        case 3:
            {
            if ( iStatusIndicator )
                return iStatusIndicator;
            else
                return NULL;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl.
// ---------------------------------------------------------------------------
//
TInt CFsEmailUiHtmlViewerContainer::CountComponentControls() const
    {
    FUNC_LOG;
    if ( iStatusIndicator )
        {
        return 4;
        }
    else 
        {
        return 3;
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl.
// Draw this application's view to the screen
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::Draw( const TRect& /*aRect*/ ) const
    {
    FUNC_LOG;
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect rect = Rect();

    // Clears the screen
    gc.Clear( rect );
    }

// ---------------------------------------------------------------------------
// From CCoeControl.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::SizeChanged()
    {
    FUNC_LOG;

    TRect rect = Rect();
    if ( iBrCtlInterface )
        {
        iBrCtlInterface->SetRect( rect );
        }
    
    UpdateOverlayButtons( IsVisible() );
    
    if ( iStatusIndicator )
        {
        TRect rect = CalcAttachmentStatusRect();
        iStatusIndicator->SetRect( rect );
        }
    }

// ---------------------------------------------------------------------------
// From CCoeControl.
// ---------------------------------------------------------------------------
//
TKeyResponse CFsEmailUiHtmlViewerContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    FUNC_LOG;
    
    TKeyResponse retVal = EKeyWasNotConsumed;

    // Handle keyboard shortcuts already on key down event as all keys
    // do not necessarily send the key event at all.
    if ( aType == EEventKeyDown )
        {
        // Check keyboard shortcuts
        TInt shortcutCommand = iAppUi.ShortcutBinding().CommandForShortcutKey(
            aKeyEvent, CFSEmailUiShortcutBinding::EContextHtmlViewer );

        if ( shortcutCommand != KErrNotFound )
            {
            iView.HandleCommandL( shortcutCommand );
            retVal = EKeyWasConsumed;
            }
        }

    else if ( iBrCtlInterface && retVal == EKeyWasNotConsumed )
        {
        retVal = iBrCtlInterface->OfferKeyEventL( aKeyEvent, aType );
        }

    iView.SetMskL();

    return retVal;
    }

// ---------------------------------------------------------------------------
// From MBrCtlSpecialLoadObserver.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::NetworkConnectionNeededL(
        TInt* aConnectionPtr,
        TInt* aSockSvrHandle,
        TBool* aNewConn,
        TApBearerType* aBearerType )
    {
    FUNC_LOG;
    *aBearerType = EApBearerTypeAllBearers;

    if ( iFirstTime )
        {
        User::LeaveIfError( iSocketServer.Connect( KESockDefaultMessageSlots ) );
        User::LeaveIfError( iConnection.Open( iSocketServer, KConnectionTypeDefault ) );
        TCommDbConnPref prefs;
        prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
        prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
        prefs.SetIapId( 0 ); // Ask for access point
        User::LeaveIfError( iConnection.Start( prefs ) );
        *aNewConn = ETrue;
        iFirstTime = EFalse;
        }
    else
        {
        *aNewConn = EFalse;
        }

    *aConnectionPtr = reinterpret_cast<TInt>(&iConnection);
    *aSockSvrHandle = iSocketServer.Handle();
    }

// ---------------------------------------------------------------------------
// From MBrCtlSpecialLoadObserver.
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::HandleRequestL(
    RArray<TUint>* /*aTypeArray*/, CDesCArrayFlat* /*aDesArray*/ )
    {
    FUNC_LOG;
    // Let browser control handle these
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From MBrCtlSpecialLoadObserver.
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::HandleDownloadL(
    RArray<TUint>* /*aTypeArray*/, CDesCArrayFlat* /*aDesArray*/ )
    {
    FUNC_LOG;
    // Let browser control handle these
    return EFalse;
    }

// ---------------------------------------------------------------------------
// From MFSEmailDownloadInformationObserver.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::RequestResponseL( const TFSProgress& aEvent,
    const TPartData& aPart )
    {
    FUNC_LOG;
    if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
        {
        MBrCtlLinkContent* linkContent = NULL;
        for ( TInt ii = iMessageParts.Count() - 1; ii >= 0 && !linkContent; --ii )
            {
            if ( iMessageParts[ii] == aPart )
                {
                iMessageParts.Remove( ii );
                linkContent = iLinkContents[ii];
                iLinkContents.Remove( ii );
                }
            }

        if ( !iMessageParts.Count() && iAppUi.DownloadInfoMediator() )
            {
            iAppUi.DownloadInfoMediator()->StopObserving( this );
            }
               
        if ( iMessage && linkContent )
            {
            CFSMailMessagePart* part = iMessage->ChildPartL( aPart.iMessagePartId );
            CleanupStack::PushL( part );
            RFile contentFile = part->GetContentFileL();
            CleanupClosePushL( contentFile );
            HBufC8* content = ReadContentFromFileLC( contentFile );
            linkContent->HandleResolveComplete(
                part->GetContentType(), KNullDesC(), content );
            CleanupStack::PopAndDestroy( content );
            CleanupStack::PopAndDestroy( &contentFile );
            CleanupStack::PopAndDestroy( part );
            }
        
        if ( iMessage )
            {
            LoadContentFromMailMessageL( iMessage, EFalse );            
            UpdateOverlayButtons( ETrue );
            }
        }
    else if ( aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled ||
            aEvent.iProgressStatus < 0 )
        {
        if ( iAppUi.DownloadInfoMediator() )
            {
            iAppUi.DownloadInfoMediator()->StopObserving( this, aPart.iMessageId );
            }
        }
    }

// ---------------------------------------------------------------------------
// From MBrCtlLinkResolver.
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::ResolveEmbeddedLinkL(
    const TDesC& aEmbeddedUrl, const TDesC& /*aCurrentUrl*/,
    TBrCtlLoadContentType /*aLoadContentType*/,
    MBrCtlLinkContent& aEmbeddedLinkContent )
    {
    FUNC_LOG;
    TBool linkResolved = EFalse;

    // Check if given link contains content ID
    if ( aEmbeddedUrl.Left( KContentIdPrefix().Length() ).CompareF( KContentIdPrefix ) == 0 )
        {
        TPtrC cid = aEmbeddedUrl.Mid( KContentIdPrefix().Length() );
        linkResolved = ResolveLinkL( cid, ETrue, aEmbeddedLinkContent );
        }
    else
        {
        // Replace all slash character with backslash characters
        HBufC* embeddedUrl = aEmbeddedUrl.AllocLC();
        TPtr ptr = embeddedUrl->Des();
        _LIT( KBackslash, "\\" );
        for ( TInt pos = ptr.Locate('/'); pos >= 0; pos = ptr.Locate('/') )
            {
            ptr.Replace( pos, 1, KBackslash );
            }

        // Check whether given url refers to file in the temporary html folder
        TInt pos = embeddedUrl->FindF( iTempHtmlFolderPath );
        if ( pos >= 0 )
            {
            TPtrC filename = embeddedUrl->Mid( pos + iTempHtmlFolderPath.Length() );
            linkResolved = ResolveLinkL( filename, EFalse, aEmbeddedLinkContent );
            }

        CleanupStack::PopAndDestroy( embeddedUrl );
        }

    // Return whether link is resolved by host application.
    return linkResolved;
    }

// ---------------------------------------------------------------------------
// From MBrCtlLinkResolver.
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::ResolveLinkL( const TDesC& aUrl,
    const TDesC& /*aCurrentUrl*/, MBrCtlLinkContent& /*aBrCtlLinkContent*/ )
    {
    FUNC_LOG;
    if ( IsMessageBodyURLL(aUrl) )
        {
        if ( iMessage )
            {
            iView.StartFetchingMessageL();
            }
        return ETrue;
        }
    else
        {
        if ( NeedToLaunchBrowserL( aUrl ) )
            {
            LaunchBrowserL( aUrl );
            return ETrue;
            }
        else
            {
            return iEventHandler->HandleEventL( aUrl );
            }     
        }
    }

// ---------------------------------------------------------------------------
// From MBrCtlLinkResolver.
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::CancelAll()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// From MBrCtlSoftkeysObserver
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::UpdateSoftkeyL(
    TBrCtlKeySoftkey /*aKeySoftkey*/, const TDesC& /*aLabel*/,
    TUint32 /*aCommandId*/,
    TBrCtlSoftkeyChangeReason /*aBrCtlSoftkeyChangeReason*/ )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Set HTML folder path name
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::SetHtmlFolderPathL()
    {
    FUNC_LOG;
    iHtmlFolderPath.Copy( KCDrive );

    TFileName privatePath;
    User::LeaveIfError( iFs.PrivatePath( privatePath ) );
    iHtmlFolderPath.Append( privatePath );

    iHtmlFolderPath.Append( KHtmlPath );
    }

// ---------------------------------------------------------------------------
// Sets temporary HTML folder path name
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::SetTempHtmlFolderPath()
    {
    FUNC_LOG;    
    iTempHtmlFolderPath.Copy( iHtmlFolderPath );
    iTempHtmlFolderPath.Append( KTempHtmlPath );
    }

// ---------------------------------------------------------------------------
// Remove all previously created files from temporary HTML folder
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::EmptyTempHtmlFolderL()
    {
    FUNC_LOG;
    // Delete all files from temp folder
    TFileName allFiles;
    allFiles.Append( iTempHtmlFolderPath );
    allFiles.Append( KAllFiles );
    TInt err = BaflUtils::DeleteFile( iFs, allFiles );
    if ( err != KErrNotFound && err != KErrNone )
        {
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------------------------
// Copies given read-only file to HTML file
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::CopyToHtmlFileL( CFSMailMessagePart& aHtmlBodyPart, const TDesC& aFileName )
    {
    FUNC_LOG;
    TFileName targetFileName;
    targetFileName.Copy( iTempHtmlFolderPath );
    targetFileName.Append( aFileName );

    RFile htmlFile = aHtmlBodyPart.GetContentFileL();
    CleanupClosePushL( htmlFile );
    
    // Read content from given source file
    HBufC8* content = ReadContentFromFileLC( htmlFile );

    // Write content to target file
    WriteContentToFileL( *content, targetFileName, aHtmlBodyPart );

    CleanupStack::PopAndDestroy( content );
    CleanupStack::PopAndDestroy( &htmlFile );
    }
// </cmail>

// ---------------------------------------------------------------------------
// Copies given buffer to HTML file
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::ConvertToHtmlFileL( CFSMailMessagePart& aTextBodyPart, const TDesC& aHtmlFileName )
    {
    FUNC_LOG;
    TFileName targetFileName;
    targetFileName.Copy( iTempHtmlFolderPath );
    targetFileName.Append( aHtmlFileName );
    
    HBufC* content = HBufC::NewLC( aTextBodyPart.FetchedContentSize() );
    TPtr contentPtr( content->Des() );
    
    aTextBodyPart.GetContentToBufferL( contentPtr, 0 );
    
    ConvertToHTML( *content, targetFileName, aTextBodyPart );
    
    CleanupStack::PopAndDestroy( content );

    }

// ---------------------------------------------------------------------------
// Reads given file content to buffer and return pointer to it
// ---------------------------------------------------------------------------
//
HBufC8* CFsEmailUiHtmlViewerContainer::ReadContentFromFileLC( RFile& aFile )
    {
    FUNC_LOG;
    TInt size = 0;
    User::LeaveIfError( aFile.Size( size ) );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 ptr = buffer->Des();
    User::LeaveIfError( aFile.Read( ptr, size ) );
    return buffer;
    }

// ---------------------------------------------------------------------------
// Writes buffer content to given file
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::WriteContentToFileL( const TDesC8& aContent,
    const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart )
    {
    FUNC_LOG;
    
    RBuf8 buffer;
    buffer.CreateL( aContent );
    buffer.CleanupClosePushL();
    
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, buffer.Size(), EDriveC ) )
         {
         // Can not write the data, there's not enough free space on disk.
         User::Leave( KErrDiskFull );
        }
    else
        {
        RFile targetFile;
        CleanupClosePushL( targetFile );
        
        User::LeaveIfError( targetFile.Replace( iFs, aFileName, EFileWrite ) );

        // Try to find initial html tag (both '<html' and '<HTML' versions are searched)
        TInt startTagOffset = buffer.Left( KMaxCharsToSearch ).FindF( KStartTag );
        
        // Modifying text/html atta IF there's no '<html>' tag anywhere in
        // the begining of file (KMaxCharsToSearch) AND there is charset
        // parameter available
        TBool modificationNeeded( EFalse );
        if( startTagOffset == KErrNotFound )
            {
            modificationNeeded = ETrue;
            }

        // Write starting metadata if needed
        if ( modificationNeeded )
            {
            HBufC8* charSet = GetCharacterSetL( aHtmlBodyPart );
            CleanupStack::PushL( charSet );
            
            User::LeaveIfError( targetFile.Write( KHtmlHeader1 ) );
            User::LeaveIfError( targetFile.Write( *charSet ) );
            User::LeaveIfError( targetFile.Write( KHtmlHeader2 ) );
            CleanupStack::PopAndDestroy( charSet );
            }
        else 
            {
            // Charset tag not found in html body
            
            if ( buffer.Left( KMaxCharsToSearch ).FindF( KCharsetTag8 ) == KErrNotFound )
                {
                TInt startPos(0);
                if ( ( startPos = buffer.Left( KMaxCharsToSearch ).FindF( KHeadTag ) ) != KErrNotFound )
                    { 
                    
                    HBufC8* charSet = GetCharacterSetL( aHtmlBodyPart );
                    CleanupStack::PushL( charSet );
                    
                    HBufC8* metaBuffer = HBufC8::NewLC( charSet->Des().Length() + KHtmlHeader3().Length() );
                    TPtr8 metaHeader( metaBuffer->Des() );
                    metaHeader.AppendFormat( KHtmlHeader3, charSet );                
                    TInt maxLength = buffer.Length() + metaHeader.Length();         
                    buffer.ReAllocL( maxLength );
                   
                    startPos += KHeadTag().Length();
                    buffer.Insert( startPos, metaHeader );
                    
                    CleanupStack::PopAndDestroy( metaBuffer );
                    CleanupStack::PopAndDestroy( charSet );
                    }
                }
            }

        // Write the original content
        User::LeaveIfError( targetFile.Write( buffer ) );

        // Write ending metadata if needed
        if ( modificationNeeded )
            {
            INFO("Add end tags");
            User::LeaveIfError( targetFile.Write( KHtmlEndTags ) );
            }

        CleanupStack::PopAndDestroy( &targetFile );
        }
    CleanupStack::PopAndDestroy( &buffer );
// </cmail>
    }

// ---------------------------------------------------------------------------
// Finds the attachment from the list that matches the given content ID
// ---------------------------------------------------------------------------
//
CFSMailMessagePart* CFsEmailUiHtmlViewerContainer::MatchingAttacmentL(
    const TDesC& aContentId,
    const RPointerArray<CFSMailMessagePart>& aAttachments ) const
    {
    FUNC_LOG;
    TBool contentFound = EFalse;
    CFSMailMessagePart* attachment = NULL;
    TInt attachmentCount = aAttachments.Count();

    // Check whether one of the attachments has the given conten ID
    for ( TInt i = 0; i < attachmentCount && !contentFound; i++ )
        {
        attachment = aAttachments[i];
        const TDesC& contentId = attachment->ContentID();
        if ( contentId.Compare( aContentId ) == 0 )
            {
            contentFound = ETrue;
            }
        }

    // If attacment with given content ID was not found, check whether
    // the given content ID contains the name of one of the attachments
    for ( TInt i = 0; i < attachmentCount && !contentFound; ++i )
        {
        attachment = aAttachments[i];
        const TDesC& name = attachment->AttachmentNameL();
        if ( aContentId.FindF( name ) >= 0 )
            {
            contentFound = ETrue;
            }
        }

    if ( !contentFound )
        {
        attachment = NULL;
        }
    return attachment;
    }

// ---------------------------------------------------------------------------
// Resolves link referring to file in temporary HTML folder
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::ResolveLinkL( const TDesC& aLink,
    TBool aContentId, MBrCtlLinkContent& aEmbeddedLinkContent  )
    {
    FUNC_LOG;
    TBool linkResolved = EFalse;
    CFSMailMessagePart* attachment = NULL;

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyClosePushL( attachments );
    iMessage->AttachmentListL( attachments );

    if ( aContentId )
        {
        attachment = MatchingAttacmentL( aLink, attachments );
        }
    else
        {
        const TInt attachmentCount = attachments.Count();
        for ( TInt ii = 0; ii < attachmentCount && !attachment; ++ii )
            {
            if ( aLink.CompareF( attachments[ii]->AttachmentNameL() ) == 0 )
                {
                attachment = attachments[ii];
                }
            }
        }

    if ( attachment )
        {
        DownloadAttachmentL( *attachment, aEmbeddedLinkContent );
        linkResolved = ETrue;
        }

    CleanupStack::PopAndDestroy( &attachments );
    return linkResolved;
    }

// ---------------------------------------------------------------------------
// Download attachment and return the content via MBrCtlLinkContent interface
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::DownloadAttachmentL(
    CFSMailMessagePart& aAttachment, MBrCtlLinkContent& aEmbeddedLinkContent )
    {
    FUNC_LOG;
    if ( aAttachment.FetchLoadState() == EFSFull )
        {
        RFile attachmentFile = aAttachment.GetContentFileL();
        CleanupClosePushL( attachmentFile );
        HBufC8* content = ReadContentFromFileLC( attachmentFile );
        aEmbeddedLinkContent.HandleResolveComplete(
            aAttachment.GetContentType(), KNullDesC(), content );
        CleanupStack::PopAndDestroy( content );
        CleanupStack::PopAndDestroy( &attachmentFile );
        }
    else
        {
        TPartData partData;
        if( iMessage )
            {
            partData.iMailBoxId = iMessage->GetMailBoxId();
            partData.iFolderId = iMessage->GetFolderId();
            partData.iMessageId = iMessage->GetMessageId();
            
            partData.iMessagePartId = aAttachment.GetPartId();

            if ( iAppUi.DownloadInfoMediator() &&
                 iAppUi.DownloadInfoMediator()->IsDownloadableL( partData ) )
                {
                ASSERT( iLinkContents.Count() == iMessageParts.Count() );
    
                // Append message part details and embedded link content interface
                // to corresponding arrays so that the content can be returned
                // when the download is completed.
                iLinkContents.AppendL( &aEmbeddedLinkContent );
                if ( iMessageParts.Append( partData ) != KErrNone )
                    {
                    iLinkContents.Remove( iLinkContents.Count() - 1 );
                    }
    
                ASSERT( iLinkContents.Count() == iMessageParts.Count() );
                if(!iView.GetAsyncFetchStatus())
                    {
                    iAppUi.DownloadInfoMediator()->AddObserver( this, aAttachment.GetMessageId() );
                    iObservingDownload=ETrue;
                    iAppUi.DownloadInfoMediator()->DownloadL( partData, EFalse );
                    }
                }
            }
        }
    }

void CFsEmailUiHtmlViewerContainer::SetHTMLResourceFlagFullName()
    {
    FUNC_LOG;
    iHtmlResourceFlagPath.Copy( iHtmlFolderPath );
    iHtmlResourceFlagPath.Append( KHtmlFlagFile );    
    }

void CFsEmailUiHtmlViewerContainer::EnableHTMLResourceFlagL()
    {
    FUNC_LOG;
    RFile flag;
    User::LeaveIfError( flag.Replace(iFs, iHtmlResourceFlagPath, EFileWrite) );
    flag.Close();
    }

TBool CFsEmailUiHtmlViewerContainer::HTMLResourceFlagEnabled()
    {
    FUNC_LOG;
    return BaflUtils::FileExists( iFs, iHtmlResourceFlagPath );
    }

void CFsEmailUiHtmlViewerContainer::CopyHTMLResourceL()
    {
    FUNC_LOG;

    TPath htmlFolderPathInZ;
    htmlFolderPathInZ.Copy( KZDrive );

    TFileName privatePath;
    User::LeaveIfError( iFs.PrivatePath( privatePath ) );
    htmlFolderPathInZ.Append( privatePath );

    htmlFolderPathInZ.Append( KHtmlPath );
    
    CDir* dirList;
    TPath listSpec;
    listSpec.Copy( htmlFolderPathInZ );
     
    listSpec.Append( _L("*.*") );
    User::LeaveIfError( iFs.GetDir( listSpec,  KEntryAttMaskSupported, ESortByName, dirList ) );
    CleanupStack::PushL( dirList );
    for ( TInt i=0; i < dirList->Count(); i++)
        {
        TPath sourceFileFullName;
        sourceFileFullName.Copy( htmlFolderPathInZ );
        sourceFileFullName.Append( (*dirList)[i].iName );
        
        TBool isFolder( EFalse );
        BaflUtils::IsFolder( iFs, sourceFileFullName, isFolder);
        if ( isFolder )
            {
            break;            
            }
        
        TPath targetFileFullName;
        targetFileFullName.Copy( iHtmlFolderPath );
        targetFileFullName.Append( (*dirList)[i].iName );
        BaflUtils::DeleteFile( iFs, targetFileFullName );        
        
        BaflUtils::CopyFile( iFs, sourceFileFullName, targetFileFullName);
        }    
    CleanupStack::PopAndDestroy( dirList );
    }

void CFsEmailUiHtmlViewerContainer::EnsureHTMLResourceL()
    {
    FUNC_LOG;
    
    if ( !HTMLResourceFlagEnabled() )
        {
        CopyHTMLResourceL();
        EnableHTMLResourceFlagL();
        }
    }

// ---------------------------------------------------------------------------
// Writes buffer content to given file after adding tags
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::ConvertToHTML( const TDesC& aContent,
    const TDesC& aFileName, CFSMailMessagePart& /*aTextBodyPart*/ )
    {
    FUNC_LOG;
    const TInt KBodyTextChunkSize = 2048;
    
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aContent.Size(), EDriveC ) )
         {
         // Can not write the data, there's not enough free space on disk.
         User::Leave( KErrDiskFull );
        }
    else
        {
        RBuf bodyBuf;
        bodyBuf.CreateL( aContent.Size() + KBodyTextChunkSize );
        bodyBuf.CleanupClosePushL();
        bodyBuf.Insert( 0, aContent);
        TInt maxlength = bodyBuf.MaxSize();
        
        TInt position( 0 );
        TBool EndOfString( EFalse );
        
        while ( !EndOfString )
            {
            TInt startPosition = position;
            TPtr segment( bodyBuf.MidTPtr( startPosition ) );
            TInt i = 0;
            
            while(i < segment.Length())
                {
                TInt currentPos = position + i;
                TText ch = segment[i];
                
                switch( ch )
                    {
                    case KSOH:  // end of line for IMAP and POP
                        bodyBuf.Delete( currentPos, 1 );
                        maxlength = maxlength + KHtmlLineBreakCRLF().Length();
                        bodyBuf.ReAlloc( maxlength );
                        bodyBuf.Insert( currentPos, KHtmlLineBreakCRLF );
                        i += KHtmlLineBreakCRLF().Length();
                        segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;
                    case KLF: // line feed
                    case KUnicodeNewLineCharacter:
                    case KUnicodeParagraphCharacter:
                            maxlength = maxlength + KHtmlLineBreak().Length();
                            bodyBuf.ReAlloc( maxlength );
                            bodyBuf.Insert( currentPos, KHtmlLineBreak );
                            i += KHtmlLineBreak().Length() + 1;
                            segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;  
                    case KQuotation:
                        bodyBuf.Delete( currentPos, 1 );
                        maxlength = maxlength + KHtmlQuotation().Length();
                        bodyBuf.ReAlloc( maxlength );
                        bodyBuf.Insert( currentPos, KHtmlQuotation );
                        i += KHtmlQuotation().Length();
                        segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;
                    case KAmpersand:
                        bodyBuf.Delete( currentPos, 1 );
                        maxlength = maxlength + KHtmlAmpersand().Length();
                        bodyBuf.ReAlloc( maxlength  );
                        bodyBuf.Insert( currentPos, KHtmlAmpersand );
                        i += KHtmlAmpersand().Length();
                        segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;
                    case KGreaterThan:
                        bodyBuf.Delete( currentPos, 1 );
                        maxlength = maxlength + KHtmlGreaterThan().Length();
                        bodyBuf.ReAlloc( maxlength );
                        bodyBuf.Insert( currentPos, KHtmlGreaterThan );
                        i += KHtmlGreaterThan().Length();
                        segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;
                    case KLessThan:
                        bodyBuf.Delete( currentPos, 1 );
                        maxlength =  maxlength + KHtmlLessThan().Length();
                        bodyBuf.ReAlloc( maxlength );
                        bodyBuf.Insert( currentPos, KHtmlLessThan );
                        i += KHtmlLessThan().Length();
                        segment.Set( bodyBuf.MidTPtr( startPosition ) );
                        break;
                    default:
                        i++;
                        break;
                    }
                }
            position += segment.Length();
            if ( ( bodyBuf.Length() - position ) <= 0 )
                {
                EndOfString = ETrue;
                }
            }
        
        CreateHyperlinksFromUrlsL( bodyBuf );
        
        HBufC8* content8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( bodyBuf );
        CleanupStack::PushL( content8 );        
        
        RFile targetFile;
        CleanupClosePushL( targetFile );
        User::LeaveIfError( targetFile.Replace( iFs, aFileName, EFileWrite ) );
        
        RBuf8 messageHeader;
        _LIT( KCharsetUtf8, "UTF-8" );
        TInt bufSize = KHtmlHeader1().Length() + KCharsetUtf8().Length() + KHtmlHeader2().Length();
        messageHeader.CreateL( bufSize );
        messageHeader.CleanupClosePushL();
        
        messageHeader.Append( KHtmlHeader1 );
        messageHeader.Append( KCharsetUtf8 );
        messageHeader.Append( KHtmlHeader2 );

        RFileWriteStream fileStream( targetFile );
        fileStream.PushL();
        fileStream.WriteL( messageHeader.Ptr(), messageHeader.Length() );

        TInt bufPos( 0 );
        TInt bufTotalSize = content8->Size();

        while ( bufPos < bufTotalSize )
            {
            TInt segmentLength = content8->Mid( bufPos ).Length();
            fileStream.WriteL( content8->Mid( bufPos ).Ptr(), segmentLength );
            bufPos += segmentLength;
            }

        fileStream.CommitL();

        CleanupStack::PopAndDestroy( &fileStream );
        CleanupStack::PopAndDestroy( &messageHeader );
        CleanupStack::PopAndDestroy( &targetFile );
        CleanupStack::PopAndDestroy( content8 );
        CleanupStack::PopAndDestroy( &bodyBuf ); // calls bodyBuf.Close();

// </cmail>
        }
    }

// ---------------------------------------------------------------------------
// Finds and html formats hyperlinks in a document
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::CreateHyperlinksFromUrlsL( RBuf& aSource )
    {
    FUNC_LOG;
    const TInt searhCases( CFindItemEngine::EFindItemSearchURLBin );
    CFindItemEngine* itemEngine = CFindItemEngine::NewL( aSource, CFindItemEngine::TFindItemSearchCase( searhCases ) );
    CleanupStack::PushL ( itemEngine );
    if ( itemEngine->ItemCount() > 0 )
        {
        _LIT( KSchemeDelimiter, "://" );
        _LIT( KUrlFormat, "<a href=\"%S\">%S</a>" );
        _LIT( KUrlFormatWithHttp, "<a href=\"http://%S\">%S</a>" );
        
        const TInt sourceLength( aSource.Length() );
        // Allocate enough space for the final result
        aSource.ReAllocL( sourceLength + TotalLengthOfItems( *itemEngine ) + KUrlFormatWithHttp().Length() * itemEngine->ItemCount() );
        aSource.SetMax();
        // Organize buffer so that original data is in the back of the aSource
        aSource.RightTPtr( sourceLength ).Copy( aSource.Left( sourceLength ) );
        // Set source to new original data's position
        const TPtrC source( aSource.RightTPtr( sourceLength ) );
        // Set target to aSource's beginning
        TPtr target( aSource.MidTPtr( 0 ) );
        // Reset length, we now have an empty buffer to fill
        target.SetLength( 0 );
        
        TInt currentSourcePosition( 0 );
        CFindItemEngine::SFoundItem item;
        for ( TBool available( itemEngine->Item( item ) ); available; available = itemEngine->NextItem( item ) )
            {
            target.Append( source.Mid( currentSourcePosition, item.iStartPos - currentSourcePosition ) );
            const TPtrC url( source.Mid( item.iStartPos, item.iLength ) );
            TPtrC format( KUrlFormat() );
            if ( url.FindF( KSchemeDelimiter() ) == KErrNotFound ) 
                {
                format.Set( KUrlFormatWithHttp() );
                }
            HBufC* formatBuffer = HBufC::NewLC( format.Length() + url.Length() * 2 );
            formatBuffer->Des().Format( format, &url, &url );            
            target.Append( *formatBuffer );
            CleanupStack::PopAndDestroy(); // formatBuffer
            currentSourcePosition = item.iStartPos + item.iLength;
            }        
        // Append characters that are left in buffer
        if ( currentSourcePosition < sourceLength )
            {
            target.Append( source.Mid( currentSourcePosition, sourceLength - currentSourcePosition ) );
            }
        aSource.SetLength( target.Length() );
        }
    CleanupStack::PopAndDestroy(); // itemEngine
    }


TInt CFsEmailUiHtmlViewerContainer::TotalLengthOfItems( CFindItemEngine& aItemEngine ) const
    {
    TInt totalLength( 0 );
    CFindItemEngine::SFoundItem item;
    aItemEngine.ResetPosition();
    for ( TBool available( aItemEngine.Item( item ) ); available; available = aItemEngine.NextItem( item ) )
        {
        totalLength += item.iLength;
        }
    aItemEngine.ResetPosition();
    return totalLength;
    }

// ---------------------------------------------------------------------------
// Get Character set from CFSMailMessagePart
// ---------------------------------------------------------------------------
//
// <cmail>
HBufC8* CFsEmailUiHtmlViewerContainer::GetCharacterSetL( CFSMailMessagePart& aHtmlBodyPart )
    {
    FUNC_LOG;
    
    CDesCArray& contentTypeArray( aHtmlBodyPart.ContentTypeParameters() );
    HBufC8* charSet = KNullDesC8().AllocLC();   
    
    for ( TInt i = 0; i < contentTypeArray.Count(); i++ )
        {
        TPtrC contentEntry( contentTypeArray.MdcaPoint( i ) );
        if ( ( contentEntry.FindF( KCharsetTag ) != KErrNotFound ) &&
                contentTypeArray.Count() >= ( i+1) )  
            {
            TPtrC value( contentTypeArray.MdcaPoint( i+1 ) );
            if ( value.Length() )
                {
                CleanupStack::PopAndDestroy( charSet );
                charSet = NULL;
                charSet = HBufC8::NewLC( value.Length() );
                charSet->Des().Copy( value );
                break;
                }
            }
        i++;
        }
    
    CleanupStack::Pop( charSet );
    return charSet;
    }

void CFsEmailUiHtmlViewerContainer::StopObserving()
    {
    if( iObservingDownload )
        {
        if ( iMessage && iAppUi.DownloadInfoMediator() )
            {
            iAppUi.DownloadInfoMediator()->StopObserving( this, iMessage->GetMessageId() );
            }
        iObservingDownload=EFalse;
        }
    }

void CFsEmailUiHtmlViewerContainer::CancelFetch()
    {
    FUNC_LOG;
    if ( iBrCtlInterface )
        {
        TRAP_IGNORE( iBrCtlInterface->HandleCommandL( (TInt)TBrCtlDefs::ECommandCancelFetch + (TInt)TBrCtlDefs::ECommandIdBase )); 
        }   
    }

void CFsEmailUiHtmlViewerContainer::ClearCacheAndLoadEmptyContent()
    {
    FUNC_LOG;
    if ( iBrCtlInterface )
        {
        iBrCtlInterface->ClearCache(); 
        TUid uid;
        uid.iUid = KCharacterSetIdentifierUtf8;
        TRAP_IGNORE( iBrCtlInterface->LoadDataL(KHTMLDataScheme, KHTMLEmptyContent, _L8("text/html"), uid) );
        }   
    }

void CFsEmailUiHtmlViewerContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == CFsEmailUiViewBase::EScreenLayoutChanged )
        {
        RefreshCurrentMailHeader();
	    SetRect( iView.ContainerRect() );
        }
   }

void CFsEmailUiHtmlViewerContainer::RefreshCurrentMailHeader()
    {
    if ( iMessage )
        {
        // Update the width in header part and reload
        TPath headerHtmlFile;
        headerHtmlFile.Copy( iHtmlFolderPath );
        headerHtmlFile.Append( KHeaderHtmlFile );
        
            TRAP_IGNORE( CFreestyleMessageHeaderHTML::ExportL( *iMessage, iFs,
                headerHtmlFile, iAppUi.ClientRect().Width(), iScrollPosition,
                iViewerSettings->AutoLoadImages() || iAppUi.DisplayImagesCache().Contains(*iMessage),
                iHeaderExpanded ) )
        
        
        if(!iEventHandler->IsMenuVisible())
            {
            TRAP_IGNORE( ReloadPageL() );
            }
        else
            {
            //Load page asynchronously after dismissing menu 
            //this is outdated call because it cancels Action menu which is no longer used in 9.2
           // iEventHandler->DismissMenuAndReload();
            }
        }
    }

void CFsEmailUiHtmlViewerContainer::ReloadPageL()
    {
    TRAP_IGNORE( iBrCtlInterface->HandleCommandL( ( TInt )TBrCtlDefs::ECommandIdBase +
            ( TInt )TBrCtlDefs::ECommandReload ) );
    }

void CFsEmailUiHtmlViewerContainer::ShowAttachmentDownloadStatusL( 
        TFSProgress::TFSProgressStatus aProgressStatus, 
        const TAttachmentData& aAttachmentData )
    {
    TBool freshDraw = EFalse;
    //If the indicator was displaying fetching body then we will be displaying a different indicator
    //this is just to make sure the image is updated
    if(iStatusIndicator)
        {
        if (iStatusIndicator->Image())
            {
            if(iStatusIndicator->Image()->Handle() != iAttachmentDownloadImageHandle)
                {
                freshDraw = ETrue;
                }
            }
        }
    if ( !iStatusIndicator )
        {
        TRect rect = CalcAttachmentStatusRect();
        iStatusIndicator = CFreestyleEmailUiAknStatusIndicator::NewL( rect, this );
        freshDraw = ETrue;
        }    
    
    if ( !iStatusIndicator->IsVisible() 
         || ( aAttachmentData.downloadProgress == KNone ) 
         || ( aProgressStatus == TFSProgress::EFSStatus_RequestCancelled ) )
        {
        freshDraw = ETrue;
        }
    
    TInt duration = KStatusIndicatorDefaultDuration;
    if ( ( aAttachmentData.downloadProgress == TFSProgress::EFSStatus_RequestComplete ) 
         || ( aProgressStatus == TFSProgress::EFSStatus_RequestCancelled ) 
         || ( aAttachmentData.downloadProgress == KComplete ) )
        {
        duration = KStatusIndicatorAutomaticHidingDuration;
        }

    HBufC* statusText = NULL;
    switch ( aProgressStatus )
        {
        case TFSProgress::EFSStatus_Status:
        case TFSProgress::EFSStatus_RequestComplete:
            {
            CDesCArray* descArray = new (ELeave) CDesCArrayFlat( 1 );
            CleanupStack::PushL( descArray );
            descArray->AppendL( aAttachmentData.fileName );
            CArrayFix<TInt>* intArray = new (ELeave) CArrayFixFlat<TInt>( 1 );
            CleanupStack::PushL( intArray );
            intArray->AppendL( aAttachmentData.downloadProgress );
            
            statusText = StringLoader::LoadL( R_FSE_VIEWER_ATTACHMENTS_LIST_DOWNLOAD,
                                              *descArray, 
                                              *intArray );
            CleanupStack::PopAndDestroy( intArray );
            CleanupStack::PopAndDestroy( descArray );
            CleanupStack::PushL( statusText );
            }
        break;
        
        case TFSProgress::EFSStatus_RequestCancelled:
            {
            statusText = aAttachmentData.fileName.AllocLC();
            }
        break;
        
        default:
            statusText = KNullDesC().AllocLC();
            break;
        }

    if ( statusText->Length() > 0 )
        {
        if ( freshDraw )
            {
            CFbsBitmap* image = NULL;
            CFbsBitmap* imageMask = NULL;
            if ( aProgressStatus == TFSProgress::EFSStatus_RequestCancelled )
                {
                iAppUi.FsTextureManager()->ProvideBitmapL(EAttachmentsCancelDownload, image, imageMask );
                }
            else
                {
                iAppUi.FsTextureManager()->ProvideBitmapL(EAttachmentsDownload, image, imageMask );
                }
            if(image)
                {
                iAttachmentDownloadImageHandle = image->Handle();
                }
            iStatusIndicator->ShowIndicatorL( image, imageMask, statusText, duration );
            }
        else
            {
            iStatusIndicator->SetTextL( statusText );
            if ( duration > -1 )
                {
                iStatusIndicator->HideIndicator( duration );
                }
            }
        }
    
    CleanupStack::Pop( statusText );
    }

TBool CFsEmailUiHtmlViewerContainer::AttachmentDownloadStatusVisible()
    {
    if ( iStatusIndicator )
        {
        return iStatusIndicator->IsVisible();
        }
    else
        {
        return EFalse;
        }
    }
void CFsEmailUiHtmlViewerContainer::ViewerSettingsChangedL( const TUint32 aKey )
    {
    FUNC_LOG;
    if (aKey == KFreestyleEmailDownloadHTMLImages)
        {
        if (iBrCtlInterface)
            {
            iBrCtlInterface->SetBrowserSettingL(
                TBrCtlDefs::ESettingsAutoLoadImages,
                    iViewerSettings->AutoLoadImages() );
            if (iViewerSettings->AutoLoadImages() && iMessage)
                {
                LoadContentFromMailMessageL(iMessage, EFalse);
                }
            }
        }
    }

void CFsEmailUiHtmlViewerContainer::HideDownloadStatus()
    {
    if ( iStatusIndicator )
        {
        iStatusIndicator->MakeVisible( EFalse );
        }
    }

TRect CFsEmailUiHtmlViewerContainer::CalcAttachmentStatusRect()
    {
    TRect rect = Rect();
    TPoint topLeft = rect.iTl;
    TPoint bottomRight = rect.iBr;
    
    TPoint statusTopLeft( topLeft.iX + KStatusIndicatorXMargin, bottomRight.iY - KStatusIndicatorHeight + 1 );
    TPoint statusBottomRight( bottomRight.iX - KStatusIndicatorXMargin, bottomRight.iY );
    return TRect( statusTopLeft, statusBottomRight );
    }

void CFsEmailUiHtmlViewerContainer::TouchFeedback()
    {
    iTouchFeedBack->InstantFeedback(this, ETouchFeedbackBasic);  
    }

/**
 * The body fetch link is cmail://body/fetch. Look for the URL separator
 * and the presence of cmail and body on the url.
 * @param aUrl 
 * return ETrue for  a valid body URL
 */
TBool CFsEmailUiHtmlViewerContainer::IsMessageBodyURLL(const TDesC& aUrl)
    {
    TInt index = aUrl.Find(KURLSchemeSeparator);
    if (index == KErrNotFound)
        {
        return EFalse;
        }
    else
        {
        if (aUrl.Compare(KURLLoadImages()) == 0)
            {
            iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandLoadImages + TBrCtlDefs::ECommandIdBase);
            return ETrue;
            }
        else if (aUrl.Compare(KURLDisplayImages()) == 0)
            {
            DisplayStatusIndicatorL(KStatusIndicatorAutomaticHidingDuration);
			if( iMessage )
			{
            iAppUi.DisplayImagesCache().AddMessageL(*iMessage);
			}
            iBrCtlInterface->HandleCommandL(TBrCtlDefs::ECommandLoadImages + TBrCtlDefs::ECommandIdBase);
            return ETrue;
            }
        else if (aUrl.Compare(KURLCollapseHeader()) == 0)
            {
            TouchFeedback();
            iHeaderExpanded = EFalse;
            return ETrue;
            }
        else if (aUrl.Compare(KURLExpandHeader()) == 0)
            {
            TouchFeedback();
            iHeaderExpanded = ETrue;
            return ETrue;        
            }
        else if (aUrl.Left(index).CompareF(KURLSchemeCmail) == 0)
            {
            TInt bodyIndex = aUrl.Find(KURLTypeBody);                      
            if (bodyIndex == KErrNotFound)
                {
                return EFalse;
                }
            else
                {
                TPtrC16 data= aUrl.Mid(bodyIndex);
                TInt separator = data.Find(KURLSlash);
                if(separator == KErrNotFound)
                    {
                    return EFalse;
                    }
                else
                    {
                    TPtrC16 temp = data.Mid(separator+1);
                    TLex lex(temp);
                    lex.Val(iScrollPosition);                  
                    }
                return ETrue;
                }
            
            }
        else
            {
            return EFalse;
            }
        }
    } 

// ---------------------------------------------------------------------------
// From MBrCtlWindowObserver
// ---------------------------------------------------------------------------
//
CBrCtlInterface* CFsEmailUiHtmlViewerContainer::OpenWindowL( TDesC& /*aUrl*/, TDesC* /*aTargetName*/, 
                                                             TBool /*aUserInitiated*/, TAny* /*aReserved*/ )
    {
    return iBrCtlInterface;
    }

// ---------------------------------------------------------------------------
// From MBrCtlWindowObserver
// ---------------------------------------------------------------------------
//
CBrCtlInterface* CFsEmailUiHtmlViewerContainer::FindWindowL( const TDesC& /*aTargetName*/ ) const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// From MBrCtlWindowObserver
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::HandleWindowCommandL( const TDesC& /*aTargetName*/, 
                                                          TBrCtlWindowCommand /*aCommand*/ )
    {
    
    }

// ---------------------------------------------------------------------------
// Check if a tap on the URL requires browser(standalone) to be launched
// ---------------------------------------------------------------------------
//
TBool CFsEmailUiHtmlViewerContainer::NeedToLaunchBrowserL( const TDesC& aUrl )
    {
    TBool launchBrowser( ETrue );
    // look for file:///
    _LIT( KFileLink, "file:///");
    _LIT( KUrlLink, "http");
    
    // This might be linking to header.html or body.html frames
    // Ignore them.
    if ( aUrl.Left( KFileLink().Length() ).CompareF( KFileLink ) == 0 )
        {
        //Now there is a chance that this could be from HTML folder
        // Replace all slash character with backslash characters
        HBufC* embeddedUrl = aUrl.AllocLC();
        TPtr ptr = embeddedUrl->Des();
        
        _LIT( KBackslash, "\\" );
        for ( TInt pos = ptr.Locate('/'); pos >= 0; pos = ptr.Locate('/') )
            {
            ptr.Replace( pos, 1, KBackslash );
            }

        // Check whether given url refers to file in the html folder
        TInt pos = embeddedUrl->FindF( iHtmlFolderPath );
        CleanupStack::PopAndDestroy( embeddedUrl );
        pos >= 0 ? launchBrowser = EFalse : ETrue;        
        }
    // Ignore links starting with cmail://
    else if ( aUrl.Left( KURLSchemeCmail().Length() ).CompareF( KURLSchemeCmail ) == 0 )
        {
        launchBrowser = EFalse;
        }
    //    THAA-82BEAZ - show popup first 
    else if ( aUrl.Left(KUrlLink().Length() ).CompareF( KUrlLink ) == 0 )
        {
        launchBrowser = EFalse;
        }
    // end THAA-82BEAZ

    return launchBrowser;
    }

// ---------------------------------------------------------------------------
// Launch the browser as a standalone app
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::LaunchBrowserL( const TDesC& aUrl )
    {
    CSchemeHandler* handler = CSchemeHandler::NewL( aUrl );
    CleanupStack::PushL( handler );
    handler->HandleUrlStandaloneL();
    CleanupStack::PopAndDestroy( handler );
    }

void CFsEmailUiHtmlViewerContainer::PrepareBodyHtmlL( const TDesC& aFileName )
    {
    
    if( iMessage )
        {
        CFSMailMessagePart* htmlBodyPart = iMessage->HtmlBodyPartL();

        if ( htmlBodyPart )
            {
            CleanupStack::PushL( htmlBodyPart );
            // Copy html body part to email html file
            CopyToHtmlFileL( *htmlBodyPart, aFileName );
            CleanupStack::PopAndDestroy( htmlBodyPart );
            }
        else
            {
            CFSMailMessagePart* textBodyPart = iMessage->PlainTextBodyPartL();
    
            if ( textBodyPart )
                {
                CleanupStack::PushL( textBodyPart );
                // Copy html body part to email html file
                ConvertToHtmlFileL( *textBodyPart, aFileName );
                CleanupStack::PopAndDestroy( textBodyPart );
                }
            else
                {
                User::Leave( KErrNotFound );
                }
            }
        }
    }

void CFsEmailUiHtmlViewerContainer::WriteEmptyBodyHtmlL( const TDesC& aFileName )
    {
    FUNC_LOG;
    TFileName targetFileName;
    targetFileName.Copy( iTempHtmlFolderPath );
    targetFileName.Append( aFileName );
    
    RFile targetFile;
    CleanupClosePushL( targetFile );
    User::LeaveIfError( targetFile.Replace( iFs, targetFileName, EFileWrite ) );
    User::LeaveIfError( targetFile.Write( KHTMLEmptyContent ) );   
    CleanupStack::PopAndDestroy( &targetFile );    
    }

void CFsEmailUiHtmlViewerContainer::DisplayStatusIndicatorL(TInt aDuration)
    {
    FUNC_LOG;
    TRect rect = CalcAttachmentStatusRect();
    if(!iStatusIndicator)
        {
        iStatusIndicator  = CFreestyleEmailUiAknStatusIndicator::NewL( rect, this );
        }  
    CFbsBitmap* image = NULL;
    CFbsBitmap* imageMask = NULL;
    HBufC* statusText = NULL;
    statusText = StringLoader::LoadL(R_FREESTYLE_EMAIL_UI_VIEWER_FETCHING_CONTENT_TEXT);
    iAppUi.FsTextureManager()->ProvideBitmapL(EStatusTextureSynchronising, image, imageMask );
    iStatusIndicator->ShowIndicatorL( image, imageMask, statusText, aDuration );
    }

void CFsEmailUiHtmlViewerContainer::MailListModelUpdatedL()
    {
    FUNC_LOG;
    UpdateOverlayButtons( IsVisible() );    
    }

