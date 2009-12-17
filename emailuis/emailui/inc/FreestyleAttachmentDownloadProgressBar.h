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

#ifndef __CFREESTYLE_ATTACHMENT_DOWNLOAD_PROGRESS_BAR_H__
#define __CFREESTYLE_ATTACHMENT_DOWNLOAD_PROGRESS_BAR_H__

#include <e32base.h>
#include <AknProgressDialog.h>

#include "FreestyleEmailUiConstants.h"
#include "FreestyleEmailUiAttachmentsListModel.h"

class CFreestyleAttachmentDownloadProgressBar : public CBase, 
                                                public MProgressDialogCallback
    {
public:
    IMPORT_C static CFreestyleAttachmentDownloadProgressBar* NewL();
    ~CFreestyleAttachmentDownloadProgressBar();
    
    TBool IsVisible() const;
    
    void ShowL();
    void HideL();
    void UpdateL( const TAttachmentData& aAttachment );
    
    const TAttachmentData* CurrentAttachment() const;
    
private: //MProgressDialogCallback   
    void DialogDismissedL( TInt aButtonId );

private:    
    CFreestyleAttachmentDownloadProgressBar();
    
    void CreateProgressBarL();
    void RemoveProgressBarL();
    void RefreshProgressBarL();
    
    void SetVisible( TBool aVisible = ETrue );

    TBool AttachmentChanged( const TAttachmentData& aAttachment ) const;
    void SetAttachment( const TAttachmentData& aAttachment );
    
    void Reset(); 
    
    
private:
    CAknProgressDialog* iProgressBar;   
    const TAttachmentData* iCurrentAttachment; // not owned
    TBool iVisible;
    };

#endif //__CFREESTYLE_ATTACHMENT_DOWNLOAD_PROGRESS_BAR_H__
