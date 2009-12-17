/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Attachment download progress bar
*
*/


#include "FreestyleAttachmentDownloadProgressBar.h"
#include "FreestyleEmailUi.rsg"
#include "FreestyleEmailUiAttachmentsListModel.h"

#include <eikprogi.h>

CFreestyleAttachmentDownloadProgressBar* CFreestyleAttachmentDownloadProgressBar::NewL()
    {
    CFreestyleAttachmentDownloadProgressBar* self = new (ELeave) CFreestyleAttachmentDownloadProgressBar();
    return self;
    }

CFreestyleAttachmentDownloadProgressBar::~CFreestyleAttachmentDownloadProgressBar()
    {
    TRAP_IGNORE( RemoveProgressBarL() );
    }

TBool CFreestyleAttachmentDownloadProgressBar::IsVisible() const
    {
    return iVisible;
    }

void CFreestyleAttachmentDownloadProgressBar::SetVisible( TBool aVisible )
    {
    iVisible = aVisible;
    }

void CFreestyleAttachmentDownloadProgressBar::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == R_AVKON_SOFTKEYS_CLOSE )
        {
        SetVisible( EFalse );
        }
    iProgressBar = NULL;
    }

void CFreestyleAttachmentDownloadProgressBar::ShowL()
    {
    SetVisible();
    UpdateL( *iCurrentAttachment );
    }

void CFreestyleAttachmentDownloadProgressBar::HideL()
    {
    SetVisible( EFalse );
    RemoveProgressBarL();
    }

void CFreestyleAttachmentDownloadProgressBar::UpdateL( const TAttachmentData& aAttachment )
    {
    TBool changed = AttachmentChanged( aAttachment );
    if (changed)
        {
        SetVisible();
        }
    
    SetAttachment( aAttachment );
    
    if ( iVisible )
        {
        if ( changed && iProgressBar )
            {
            RemoveProgressBarL();
            }
        
        if ( !iProgressBar )
            {
            CreateProgressBarL();
            }
        
        if ( iProgressBar )
            {
            RefreshProgressBarL();
            }
        }
    }

const TAttachmentData* CFreestyleAttachmentDownloadProgressBar::CurrentAttachment() const
    {
    return iCurrentAttachment;
    }

CFreestyleAttachmentDownloadProgressBar::CFreestyleAttachmentDownloadProgressBar()
    :iProgressBar( NULL ),
    iCurrentAttachment( NULL ),
    iVisible( ETrue )
    {    
    }

void CFreestyleAttachmentDownloadProgressBar::CreateProgressBarL()
    {
    ASSERT( iCurrentAttachment );
    
    iProgressBar = new (ELeave) CAknProgressDialog( reinterpret_cast<CEikDialog**>(&iProgressBar) );
    iProgressBar->PrepareLC( R_FSE_ATTACHMENT_DOWNLOAD_PROGRESS );
    iProgressBar->SetCallback( this );
    
    iProgressBar->SetTextL( iCurrentAttachment->fileName );

    CEikProgressInfo* progressInfo = iProgressBar->GetProgressInfoL();
    progressInfo->SetFinalValue( KComplete );

    iProgressBar->RunLD();
    }

void CFreestyleAttachmentDownloadProgressBar::RemoveProgressBarL()
    {
    if ( iProgressBar )
        {
        iProgressBar->ProcessFinishedL();
        }
    delete iProgressBar;
    iProgressBar = NULL;
    }

void CFreestyleAttachmentDownloadProgressBar::RefreshProgressBarL()
    {
    ASSERT( iProgressBar );  
    ASSERT( iCurrentAttachment );
    
    CEikProgressInfo* progressInfo = iProgressBar->GetProgressInfoL();
    progressInfo->SetAndDraw( iCurrentAttachment->downloadProgress );
    if ( iCurrentAttachment->downloadProgress == KComplete )
        {
        RemoveProgressBarL();
        }
    }

TBool CFreestyleAttachmentDownloadProgressBar::AttachmentChanged( const TAttachmentData& aAttachment ) const
    {
    TBool changed = ETrue;
    if ( iCurrentAttachment )
        {
        changed = !( iCurrentAttachment->partData == aAttachment.partData );
        }
    return changed;
    }

void CFreestyleAttachmentDownloadProgressBar::SetAttachment( const TAttachmentData& aAttachment )
    {
    iCurrentAttachment = &aAttachment;
    }

void CFreestyleAttachmentDownloadProgressBar::Reset()
    {
    iCurrentAttachment = NULL;
    }

