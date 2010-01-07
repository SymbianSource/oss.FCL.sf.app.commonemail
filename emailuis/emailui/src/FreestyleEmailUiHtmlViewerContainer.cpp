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

//<cmail>
#include "CFSMailMessage.h"
//</cmail>
#include <brctlinterface.h>
#include <sysutil.h>


#include "FreestyleEmailUiAppui.h"
#include "FreestyleEmailUiUtilities.h"
#include "FreestyleEmailUiHtmlViewerContainer.h"
#include "FreestyleEmailUiHtmlViewerView.h"
#include "FreestyleEmailUiShortcutBinding.h"

#include "FreestyleMessageHeaderHTML.h"
#include "FreestyleMessageHeaderURLEventHandler.h"

_LIT( KContentIdPrefix, "cid:" );
_LIT( KCDrive, "c:" );
_LIT( KHtmlPath, "HtmlFile\\" );
_LIT( KTempHtmlPath, "temp\\" );
_LIT( KAllFiles, "*" );

_LIT( KHeaderHtmlFile, "header.html" );
_LIT( KBodyHtmlFile, "body.html" );
_LIT( KMessageHtmlFile, "email.html" );
_LIT( KZDrive, "z:" );
_LIT( KHtmlFlagFile, "html.flag" );

// <cmail>
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

// </cmail>

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
    iFs( CCoeEnv::Static()->FsSession() ),
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
    }

// <cmail> Because of browser changes, followings must be performed before iAppUi.exit()
void CFsEmailUiHtmlViewerContainer::PrepareForExit()
    {
    FUNC_LOG;
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
// </cmail>

void CFsEmailUiHtmlViewerContainer::ConstructL()
    {
    FUNC_LOG;
    
    SetHtmlFolderPathL();
    BaflUtils::EnsurePathExistsL( iFs, iHtmlFolderPath );
    SetTempHtmlFolderPath();
    BaflUtils::EnsurePathExistsL( iFs, iTempHtmlFolderPath );
    SetHTMLResourceFlagFullName();
    EnsureHTMLResourceL();
    
    CreateWindowL();
    SetRect( iAppUi.ClientRect() );


    TUint brCtlCapabilities = TBrCtlDefs::ECapabilityClientResolveEmbeddedURL |
                              TBrCtlDefs::ECapabilityDisplayScrollBar |
                              TBrCtlDefs::ECapabilityClientNotifyURL |
                              TBrCtlDefs::ECapabilityLoadHttpFw |
                              TBrCtlDefs::ECapabilityCursorNavigation;

    // Set browsercontrol to whole screen
    TRect rect( TPoint(), Size() );

    iBrCtlInterface = CreateBrowserControlL( this, rect, brCtlCapabilities,
        TBrCtlDefs::ECommandIdBase, NULL, this, this );

    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsEmbedded, ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsAutoLoadImages, ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsPageOverview, EFalse );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsTextWrapEnabled, ETrue );
    iBrCtlInterface->SetBrowserSettingL( TBrCtlDefs::ESettingsFontSize, TBrCtlDefs::EFontSizeLevelLarger );

    iEventHandler = CFreestyleMessageHeaderURLEventHandler::NewL( iAppUi, iView );
    ActivateL();
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
    CFSMailMessage* aMailMessage )
    {
    FUNC_LOG;
    ASSERT( aMailMessage );
    iMessage = aMailMessage;
    
    TPath headerHtmlFile;
    headerHtmlFile.Copy( iHtmlFolderPath );
    headerHtmlFile.Append( KHeaderHtmlFile );
    
    // insert email header into email.html file
    // CFreestyleMessageHeaderHTML will replace contents of email.html
    // So, no need to clear the contents
    CFreestyleMessageHeaderHTML::ExportL( *iMessage, iFs, headerHtmlFile, iAppUi.ClientRect().Width() );
    
    // Remove all previously created files from temporary HTML folder
    EmptyTempHtmlFolderL();

    CFSMailMessagePart* htmlBodyPart = iMessage->HtmlBodyPartL();

    TBool bodyPartAvailable( EFalse );
    if ( htmlBodyPart )
        {
        CleanupStack::PushL( htmlBodyPart );
        
        RFile htmlFile = htmlBodyPart->GetContentFileL();
        CleanupClosePushL( htmlFile );
        
        // Copy html body part to email html file
        CopyFileToHtmlFileL( htmlFile, KBodyHtmlFile, *htmlBodyPart );
        bodyPartAvailable = ETrue;
        
        CleanupStack::PopAndDestroy( &htmlFile );
        CleanupStack::PopAndDestroy( htmlBodyPart );
        }
    else
        {
        CFSMailMessagePart* textBodyPart = iMessage->PlainTextBodyPartL();;

        if ( textBodyPart )
            {
            CleanupStack::PushL( textBodyPart );
            //For now do not do any conversion just add start and end..Let browser display plain text as is
            //Later need to refine this and add conversions
            //htmlFile = textBodyPart->GetContentFileL();
            //Need to get buffer in this case, file does not work
            //HBufC* contentBuffer=HBufC::NewLC(textBodyPart->ContentSize());
            
            HBufC* contentBuffer=HBufC::NewLC(textBodyPart->FetchedContentSize());
            TPtr bufPtr(contentBuffer->Des());
            
            textBodyPart->GetContentToBufferL(bufPtr,0);
            HBufC8* contentBuffer8=HBufC8::NewLC(contentBuffer->Length());
            
            contentBuffer8->Des().Copy(*contentBuffer);         
            CopyFileToHtmlFileL( *contentBuffer8, KBodyHtmlFile, *textBodyPart );
            bodyPartAvailable = ETrue;
            
            CleanupStack::PopAndDestroy( contentBuffer8 );
            CleanupStack::PopAndDestroy( contentBuffer );
            CleanupStack::PopAndDestroy( textBodyPart ); 
            }

        }
    // pass the emailHtmlFile to the browser for it to load
    if ( bodyPartAvailable )
        {
        TPath emailHtmlFile;
        emailHtmlFile.Copy( iHtmlFolderPath );
        emailHtmlFile.Append( KMessageHtmlFile );
        LoadContentFromFileL( emailHtmlFile );
        }
    }

// ---------------------------------------------------------------------------
// Reset content
// ---------------------------------------------------------------------------
//
void CFsEmailUiHtmlViewerContainer::ResetContent()
    {
    FUNC_LOG;
    if ( iBrCtlInterface )
        {
        TRAP_IGNORE( iBrCtlInterface->HandleCommandL( ( TInt )TBrCtlDefs::ECommandIdBase + 
                                                      ( TInt )TBrCtlDefs::ECommandFreeMemory ) );
        }
    iFile.Close();
    iLinkContents.Reset();
    iMessageParts.Reset();
    iMessage = NULL;
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
    return 1;
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
    if ( iBrCtlInterface )
        {
        TRect rect = Rect();
        iBrCtlInterface->SetRect( rect );
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

    if ( iBrCtlInterface && retVal == EKeyWasNotConsumed )
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
            LoadContentFromMailMessageL( iMessage );
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
    return iEventHandler->HandleEventL( aUrl );
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
void CFsEmailUiHtmlViewerContainer::CopyFileToHtmlFileL( RFile& aFile,
    const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart )
    {
    FUNC_LOG;
    TFileName targetFileName;
    targetFileName.Copy( iTempHtmlFolderPath );
    targetFileName.Append( aFileName );

    // Read content from given source file
    HBufC8* content = ReadContentFromFileLC( aFile );

    // Write content to target file
    WriteContentToFileL( *content, targetFileName, aHtmlBodyPart );

    CleanupStack::PopAndDestroy( content );
    }
// </cmail>

// ---------------------------------------------------------------------------
// Copies given buffer to HTML file
// ---------------------------------------------------------------------------
//
// <cmail>
void CFsEmailUiHtmlViewerContainer::CopyFileToHtmlFileL( const TDesC8& aBuffer,
    const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart )
    {
    FUNC_LOG;
    TFileName targetFileName;
    targetFileName.Copy( iTempHtmlFolderPath );
    targetFileName.Append( aFileName );
    
    // Write content to target file
    ConvertToHTML( aBuffer, targetFileName, aHtmlBodyPart );
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
void CFsEmailUiHtmlViewerContainer::ConvertToHTML( const TDesC8& aContent,
    const TDesC& aFileName, CFSMailMessagePart& aHtmlBodyPart )
    {
    FUNC_LOG;
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aContent.Size(), EDriveC ) )
         {
         // Can not write the data, there's not enough free space on disk.
         User::Leave( KErrDiskFull );
        }
    else
        {
        RFile targetFile;
        CleanupClosePushL( targetFile );

        User::LeaveIfError( targetFile.Replace( iFs, aFileName, EFileWrite ) );
        
        HBufC8* charSet = GetCharacterSetL( aHtmlBodyPart );
        CleanupStack::PushL( charSet );
        
        User::LeaveIfError( targetFile.Write( KHtmlHeader1 ) );
        User::LeaveIfError( targetFile.Write( *charSet ) );
        User::LeaveIfError( targetFile.Write( KHtmlHeader2 ) );
        CleanupStack::PopAndDestroy( charSet );


        // Write the original content
        for(int i=0;i<aContent.Length();i++)
            {
          if( i==aContent.Length()-1 )
            {
            aContent.Mid( i,1 ).CompareC( _L8("\x01" ) )==0 ? 
            User::LeaveIfError( targetFile.Write( _L8("<br>") ) ):
            User::LeaveIfError( targetFile.Write( aContent.Mid(i,1) ) );
            }
          else
            {
            if(aContent.Mid( i,1 ).CompareC( _L8("\x01" ) )==0 ||
            aContent.Mid( i,2 ).CompareC( _L8("\x0D\x0A" ) )==0 ) 
              {
              User::LeaveIfError( targetFile.Write( _L8("<br>") ));
              }
            else
              {
          User::LeaveIfError( targetFile.Write( aContent.Mid(i,1) ) );                        
                }
              }
            }
        INFO("Add end tags");
        User::LeaveIfError( targetFile.Write( KHtmlEndTags ) );
            

        CleanupStack::PopAndDestroy( &targetFile );
        }
// </cmail>
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
        if ( ( contentTypeArray.MdcaPoint( i ).Find( KCharsetTag ) != KErrNotFound ) &&
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
	    // only update header if we get a layout change from email ui
		if ( iMessage )
            {
            // update the width in header part and reload
            TPath headerHtmlFile;
            headerHtmlFile.Copy( iHtmlFolderPath );
            headerHtmlFile.Append( KHeaderHtmlFile );
            
            TRAP_IGNORE( CFreestyleMessageHeaderHTML::ExportL( *iMessage, iFs, headerHtmlFile, iAppUi.ClientRect().Width() ) )
            
            TPath emailHtmlFile;
            emailHtmlFile.Copy( iHtmlFolderPath );
            emailHtmlFile.Append( KMessageHtmlFile );
            
            TRAP_IGNORE( LoadContentFromFileL( emailHtmlFile ) )
            
            SetRect( iAppUi.ClientRect() );
            }
		}
	}
