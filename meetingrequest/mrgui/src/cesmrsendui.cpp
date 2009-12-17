/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ESMR send ui wrapper
*
*/

#include "emailtrace.h"
#include "cesmrsendui.h"
#include "esmrvcalexport.h"

#include <CalenInterimUtils2.h>
#include <eikenv.h>
#include <MsgBioUids.h>
#include <sendui.h>
#include <TSendingCapabilities.h>
#include <CMessageData.h>
#include <CMessageAddress.h>
#include <SendUiConsts.h>
#include <sysutil.h>
#include <txtrich.h>
#include <calsession.h>
#include <pathinfo.h>

// Unnamed namespace for local definitions
namespace {

_LIT( KVCalAttachmentFile, 
	  "\\private\\10005901\\Calendar.vcs" );


_LIT( KCalendarDatabaseFilePath, "c:Calendar" );

const TInt KNumMtmsToDim =6;

}//namespace

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CESMRSendUI::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRSendUI* CESMRSendUI::NewL(TInt aMenuCommandId)
    {
    FUNC_LOG;
    CESMRSendUI* self = new (ELeave) CESMRSendUI();
    CleanupStack::PushL(self);
    self->ConstructL(aMenuCommandId);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::CESMRSendUI()
// ---------------------------------------------------------------------------
//
CESMRSendUI::CESMRSendUI()
    {
    FUNC_LOG;
    // Do nothing
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::ConstructL()
// ---------------------------------------------------------------------------
//
void CESMRSendUI::ConstructL(TInt aMenuCommandId)
    {
    FUNC_LOG;
    iSession = CCalSession::NewL();
    iSession->OpenL( KCalendarDatabaseFilePath );
    iSendUi = CSendUi::NewL();
    iSendAsCmdId=aMenuCommandId;
    iSendMtmsToDim = new (ELeave) CArrayFixFlat<TUid>(2);

    iSendMtmsToDim->SetReserveL( KNumMtmsToDim );  
    // Always disable sending via Audio message
    iSendMtmsToDim->AppendL( KSenduiMtmAudioMessageUid );
    iSendMtmsToDim->AppendL( KSenduiMtmPostcardUid );
    iSendMtmsToDim->AppendL( KMmsDirectUpload );
    iSendMtmsToDim->AppendL( KMmsIndirectUpload );
    //<uid not available in backport env>
    //iSendMtmsToDim->AppendL( KSenduiMtmOnlineAlbumUid );
    
    if( ! iSendUi->CanSendBioMessage( KMsgBioUidVCalendar ) )
        {
        // This disables sending of vCalendars via SMS
        iSendMtmsToDim->AppendL(KSenduiMtmSmsUid);
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::~CESMRSendUI()
// ---------------------------------------------------------------------------
//
EXPORT_C CESMRSendUI::~CESMRSendUI()
    {
    FUNC_LOG;
    delete iSession;
    delete iSendUi;
    delete iSendMtmsToDim;
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::CanSendL()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CESMRSendUI::CanSendL(TInt /*aCommandId*/)
    {
    FUNC_LOG;

    if( iSelectedMtmUid != KNullUid )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::DisplaySendMenuItemL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRSendUI::DisplaySendMenuItemL(CEikMenuPane& aMenuPane,
                                               TInt aIndex)
    {
    FUNC_LOG;
    iSendUi->AddSendMenuItemL( 
    		aMenuPane, aIndex, iSendAsCmdId, TSendingCapabilities() );
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::DisplaySendCascadeMenuL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRSendUI::DisplaySendCascadeMenuL(CEikMenuPane& /*aMenuPane*/)
    {
    FUNC_LOG;
    iSelectedMtmUid = iSendUi->ShowSendQueryL( 
    		NULL, KCapabilitiesForAllServices, iSendMtmsToDim, KNullDesC );
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::SendAsVCalendarL()
// ---------------------------------------------------------------------------
//
EXPORT_C void CESMRSendUI::SendAsVCalendarL(
		TInt /*aCommand*/, 
		CCalEntry& aEntry)
    {
    FUNC_LOG;

    // If the user selected email as their protocol, 
    // and the entry is a MR, send using MR utils.

    CEikonEnv* eikonEnv = CEikonEnv::Static();// codescanner::eikonenvstatic
    CRichText* text = CRichText::NewL(
        eikonEnv->SystemParaFormatLayerL(),
        eikonEnv->SystemCharFormatLayerL());
    CleanupStack::PushL(text);

    TESMRVCalExport exporter(*iSession, eikonEnv->FsSession());
    HBufC8* vcal = exporter.ExportVCalLC(aEntry);

    TUid mtmUid = iSelectedMtmUid;
    TSendingCapabilities capabilities;
    iSendUi->ServiceCapabilitiesL( mtmUid, capabilities );

    if (capabilities.iFlags & TSendingCapabilities::ESupportsAttachments)
        {
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );
        // must share the handle between processes
        User::LeaveIfError( fs.ShareProtected() );        
        TFileName fileName(PathInfo::PhoneMemoryRootPath().Left(2));
        fileName.Append(KVCalAttachmentFile);
        fs.MkDirAll(fileName);
        RFile file;
        User::LeaveIfError(file.Replace(
					fs,fileName,EFileWrite | EFileShareAny ));
        CleanupClosePushL(file);
        TInt spaceNeeded = vcal->Size();
        if ( SysUtil::FFSSpaceBelowCriticalLevelL( &fs, spaceNeeded ) )
            {
            // don't show any own notes here
            User::Leave( KErrDiskFull );
            }

        User::LeaveIfError(file.Write(*vcal));

        TParse parse;
        User::LeaveIfError(parse.SetNoWild(fileName,
                                           NULL, NULL));

        TRAPD(err, DoSendAsAttachmentHandleL(file));

        CleanupStack::PopAndDestroy(&file);
        fs.Delete(parse.FullName());
        CleanupStack::PopAndDestroy( &fs );
        User::LeaveIfError(err);
        }
    else
        {
        TPtrC8 p8(*vcal);
        HBufC* bufCnv = HBufC::NewLC(p8.Length());
        TPtr16 des(bufCnv->Des());
        des.Copy(p8);
        text->InsertL(0, des);

        CMessageData* messageData = CMessageData::NewL();
        CleanupStack::PushL( messageData );
        messageData->SetBodyTextL( text );
        iSendUi->CreateAndSendMessageL( 
        		iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
        CleanupStack::PopAndDestroy(); // messagedata
        CleanupStack::PopAndDestroy(bufCnv);
        }
    CleanupStack::PopAndDestroy(vcal);
    CleanupStack::PopAndDestroy(text);
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::DoSendAsAttachmentFileL()
// ---------------------------------------------------------------------------
//
void CESMRSendUI::DoSendAsAttachmentFileL(TInt /*aCommand*/, TParse& aFilename)
    {
    FUNC_LOG;
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentL( aFilename.FullName() );
    iSendUi->CreateAndSendMessageL( 
    		iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
    CleanupStack::PopAndDestroy(); // messagedata
    }

// ---------------------------------------------------------------------------
// CESMRSendUI::DoSendAsAttachmentHandleL()
// ---------------------------------------------------------------------------
//
void CESMRSendUI::DoSendAsAttachmentHandleL(const RFile& aHandle)
    {
    FUNC_LOG;
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    messageData->AppendAttachmentHandleL(aHandle);
    iSendUi->CreateAndSendMessageL( 
    		iSelectedMtmUid, messageData, KMsgBioUidVCalendar, ETrue );
    CleanupStack::PopAndDestroy( messageData );
    }


// End of File

