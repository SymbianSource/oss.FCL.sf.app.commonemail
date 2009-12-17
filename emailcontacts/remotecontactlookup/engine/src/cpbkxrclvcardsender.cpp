/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CPbkxRclvCardSender.
*
*/


#include "emailtrace.h"
#include <sendui.h>
#include <BCardEng.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <eikenv.h>
#include <featmgr.h>
#include <features.hrh>
#include <s32file.h>
#include <CMessageData.h>
#include <txtrich.h>
#include <MsgBioUids.h>
#include <SendUiConsts.h>

#include "cpbkxrclvcardsender.h"
#include "cpbkxrclcontactconverter.h"
#include "pbkxrclengineconstants.h"
#include "pbkxremotecontactlookuppanic.h"

// temp file created for the business card to be sent
//<cmail> file path is not hardcoded
_LIT( KVcardExtension, ".vcf" );
#include <pathinfo.h>
//</cmail>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclvCardSender* CPbkxRclvCardSender::NewL( 
    CPbkContactEngine& aContactEngine )
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = CPbkxRclvCardSender::NewLC( aContactEngine );
    CleanupStack::Pop( sender );
    return sender;
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::NewLC
// ---------------------------------------------------------------------------
//
CPbkxRclvCardSender* CPbkxRclvCardSender::NewLC( 
    CPbkContactEngine& aContactEngine )
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = new ( ELeave ) CPbkxRclvCardSender();
    CleanupStack::PushL( sender );
    sender->ConstructL( aContactEngine );
    return sender;
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::CPbkxRclvCardSender
// ---------------------------------------------------------------------------
//
CPbkxRclvCardSender::CPbkxRclvCardSender() : CBase()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::~CPbkxRclvCardSender
// ---------------------------------------------------------------------------
//
CPbkxRclvCardSender::~CPbkxRclvCardSender()
    {
    FUNC_LOG;
    delete iSendUi;
    delete iCardEngine;
    delete iFilter;
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclvCardSender::ConstructL( CPbkContactEngine& aContactEngine )
    {
    FUNC_LOG;
    iEikEnv = CEikonEnv::Static();

    __ASSERT_ALWAYS( iEikEnv != NULL, PbkxRclPanic( EPbkxRclPanicGeneral ) );

    iSendUi = CSendUi::NewL();
    iCardEngine = CBCardEngine::NewL( &aContactEngine );
    CreateFilterL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::SendvCardL
// ---------------------------------------------------------------------------
//
void CPbkxRclvCardSender::SendvCardL( 
        CPbkContactItem& aContactItem, 
        CPbkxRclContactConverter& aContactConverter )
    {
    FUNC_LOG;
    User::LeaveIfError( GetTemporaryFileNameL( iTempFileName, aContactItem ) );
    CreateTempFileL( aContactItem, aContactConverter );

    TUid mtmUid = iSendUi->ShowSendQueryL( 
        NULL, 
        KCapabilitiesForAllServices, 
        iFilter );

    if ( mtmUid == KNullUid )
        {
        // user pressed cancel
        DestroyTempFileL();
        return;
        }

    CMessageData* messageData = CMessageData::NewLC();

    if ( mtmUid == KSenduiMtmSmsUid )
        {
        // Sending through SMS
      
        // Copy the one and only attachment into a rich text object
        CRichText* msgBody = CreateRichTextFromFileLC();

        messageData->SetBodyTextL( msgBody );

        // Send the message using Send Ui
        iSendUi->CreateAndSendMessageL( mtmUid, messageData, KMsgBioUidVCard );
        
        CleanupStack::PopAndDestroy( msgBody );
        }
    else
        {
        // Not sending through SMS, just pass the attachments
        //<cmail> hardcoded paths removed
        messageData->AppendAttachmentL(iTempFileName);
        //</cmail>
        
        // Send the message using Send Ui
        iSendUi->CreateAndSendMessageL( mtmUid, messageData, KMsgBioUidVCard );
        }

    CleanupStack::PopAndDestroy( messageData );

    DestroyTempFileL();
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::CreateTempFileL
// ---------------------------------------------------------------------------
//
void CPbkxRclvCardSender::CreateTempFileL( 
        CPbkContactItem& aContactItem, 
        CPbkxRclContactConverter& aContactConverter )
    {
    FUNC_LOG;
    RFileWriteStream stream;
    User::LeaveIfError( 
        stream.Replace( 
            iEikEnv->FsSession(), 
            iTempFileName, //<cmail>
            EFileWrite ) );
    CleanupClosePushL( stream );
    // Convert and export to vcard
    aContactConverter.ConvertAndExportContactL( aContactItem, stream );
    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::DestroyTempFileL
// ---------------------------------------------------------------------------
//
void CPbkxRclvCardSender::DestroyTempFileL()
    {
    FUNC_LOG;
    //<cmail>
    User::LeaveIfError( iEikEnv->FsSession().Delete( iTempFileName ) );
    //<cmail>
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::CreateRichTextFromFileLC
// ---------------------------------------------------------------------------
//
CRichText* CPbkxRclvCardSender::CreateRichTextFromFileLC()
    {
    FUNC_LOG;
   
    // Common allocation granularity and buffer size for rich text and
    // file reading
    const TInt KBufSize = CEditableText::EDefaultTextGranularity;
    
    // Create a rich text object with default formatting
    CRichText* richText = CRichText::NewL(
        iEikEnv->SystemParaFormatLayerL(), 
        iEikEnv->SystemCharFormatLayerL(),
        CEditableText::ESegmentedStorage,
        KBufSize  // Allocation granularity
        );
    CleanupStack::PushL( richText );
    
    // Open the file for reading
    RFile file;
    //<cmail> take away double line break
    User::LeaveIfError( 
        file.Open(
            iEikEnv->FsSession(), 
            iTempFileName, //<cmail>
            EFileRead | EFileStream | EFileShareReadersOnly ) );
    //</cmail>
    CleanupClosePushL( file );
    
    // Create two buffers: 8-bit for reading from file and 16-bit for
    // converting to 16-bit format
    HBufC8* buf8 = HBufC8::NewLC( KBufSize );
    TPtr8 ptr8 = buf8->Des();
    HBufC16* buf16 = HBufC16::NewLC( ptr8.MaxLength() );
    TPtr16 ptr16 = buf16->Des();

    // Read, convert and append to rich text until the file ends
    User::LeaveIfError( file.Read( ptr8 ) );
    while ( ptr8.Length() > 0 )
        {
        ptr16.Copy( ptr8 );
        richText->InsertL( richText->DocumentLength(), ptr16 );
        User::LeaveIfError( file.Read( ptr8 ) );
        }
    
    // Cleanup and return
    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PopAndDestroy( &file );
    return richText;
    }

// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::CreateFilterL
// ---------------------------------------------------------------------------
//
void CPbkxRclvCardSender::CreateFilterL()
    {
    FUNC_LOG;
    if ( iFilter == NULL )
        {
        iFilter = new ( ELeave ) CArrayFixFlat<TUid>( KArrayGranularity );
        }
    else
        {
        iFilter->Reset();
        }

    const TUid KPostcardMtmUid = { KSenduiMtmPostcardUidValue };

    iFilter->AppendL( KPostcardMtmUid );    // hide postcard
    iFilter->AppendL( KSenduiMtmAudioMessageUid ); // hide audio message

    // Check must we disable SMS menuitem
    if ( !iSendUi->CanSendBioMessage( KMsgBioUidVCard ) )
        {
        iFilter->AppendL( KSenduiMtmSmsUid );	// hide SMS
        }
    
    // Hide MMS if not supported
    if ( !FeatureManager::FeatureSupported( KFeatureIdMMS ) )
        {
        iFilter->AppendL(KSenduiMtmMmsUid);
        }
    
    // Hide Email if not supported
    if ( !FeatureManager::FeatureSupported( KFeatureIdEmailUi ) )
        {
        iFilter->AppendL( KSenduiMtmSmtpUid );
        iFilter->AppendL( KSenduiMtmImap4Uid );
        iFilter->AppendL( KSenduiMtmPop3Uid );
        iFilter->AppendL( KSenduiMtmSyncMLEmailUid );
        }
    
    }
//<cmail> hard coded paths removed
// ---------------------------------------------------------------------------
// CPbkxRclvCardSender::GetTemporaryFileName
// ---------------------------------------------------------------------------
//
TInt CPbkxRclvCardSender::GetTemporaryFileNameL( TFileName& aFileName, CPbkContactItem& aContactItem )
    {
    TFileName KPath;
    RFs fsSession;
    TInt result = fsSession.Connect();
    if( result==KErrNone )
        {
        fsSession.PrivatePath( KPath );
        TFindFile findFile( fsSession );
        aFileName = KPath;
        result = findFile.FindByDir( KPath, KNullDesC );
        if (result != KErrNone ) //create path if doesn't exists
            {
            fsSession.CreatePrivatePath( TDriveUnit(PathInfo::PhoneMemoryRootPath() ) );
            result = findFile.FindByDir( KPath, KNullDesC );
            }
        aFileName = findFile.File();
        aFileName.Append( *aContactItem.GetContactTitleL() ); // getting name from contact
        aFileName.Append( KVcardExtension ); // postfix extension
        fsSession.Close();
        }
    return result;
    }
//</cmail>

