/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MR Attachment field for editor and viewer
 *
*/


#ifndef CMRVIEWERATTACHMENTSFIELD_H
#define CMRVIEWERATTACHMENTSFIELD_H

#include <e32base.h>
#include <eikedwob.h>

#include "esmrdef.h"
#include "cesmrfield.h"
#include "cesmrrichtextviewer.h"

// Forward declarations
class CMRImage;
class CEikButtonGroupContainer;
class CAknsBasicBackgroundControlContext;
class CESMRRichTextViewer;
class CCalAttachment;
class CMRAttachmentCommandHandler;
class CCalRemoteAttachmentApi;
class CCalRemoteAttachmentInfo;
class MCalRemoteAttachmentOperation;

/**
 * CMRViewerAttachmentsField is entry viewer attachment field
 */
NONSHARABLE_CLASS( CMRViewerAttachmentsField ):
        public CESMRField,
        public MEikEdwinSizeObserver,
        public MESMRRichTextObserver
    {
public:
    /**
     * Two phase constructor.
     */
    static CMRViewerAttachmentsField* NewL();

    /**
     * Destructor.
     */
    ~CMRViewerAttachmentsField();

protected: // From CESMRField
    TSize MinimumSize();
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );
    void ListObserverSet();
    void HandleLongtapEventL( const TPoint& aPosition );
    TBool HandleRawPointerEventL( const TPointerEvent& aPointerEvent );
    void DynInitMenuPaneL(
                 TInt aResourceId,
                 CEikMenuPane* aMenuPane );
    void LockL();
    TBool SupportsLongTapFunctionalityL(
        		 const TPointerEvent &aPointerEvent );

protected: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void SetContainerWindowL( const CCoeControl& aControl );

protected: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aDesirableEdwinSize);

protected: // From MESMRRichTextObserver
    TBool HandleRichTextLinkSelection( const CESMRRichTextLink* aLink );

private: // Implementation
    CMRViewerAttachmentsField();
    void ConstructL();

    void UpdateAttachmentsListL();
    HBufC* ClipTextLC( const TDesC& aText, const CFont& aFont, TInt aWidth );
    void UpdateFieldL();
    HBufC* AttachmentNameAndSizeL( const TDesC& aDes, const CCalAttachment& aAttachment );
    TInt LineSpacing();
    TInt FocusMargin();
    void CreateRemoteAttApiL();
    void DynInitMenuPaneForCurrentAttachmentL(
                 CEikMenuPane* aMenuPane );
    TBool IsAttachmentDownloadInProgress( const CCalAttachment& aAttachment );
    TInt MaxTextLengthInPixelsL();

private: //data
    // Own: Field icon
    CMRImage* iFieldIcon;
    /// Own: Lock icon
    CMRImage* iLockIcon;
    // Owned. Attachment list.
    CESMRRichTextViewer* iRichTextViewer;
    // Not owned. CBA.
    CEikButtonGroupContainer* iButtonGroupContainer;
    // Own. Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    // Attachment count
    TInt iAttachmentCount;
    /// Ref: Reference to internalized entry
    MESMRCalEntry* iEntry;
    // Field rect for comparison purposes
    TRect iFieldRect;
    // Rich Text viewer size
    TSize iRichTextSize;
    /// Own: Attachment command handler
    CMRAttachmentCommandHandler* iAttachmentCommandHandler;
    /// Own: Remote attachment API
    CCalRemoteAttachmentApi* iRemoteAttApi;
    /// Own: Remote attachment URIs
    CDesCArrayFlat* iRemoteAttUris;
    /// Own: Remote attachment info
    CCalRemoteAttachmentInfo* iAttachmentInfo;
    };

#endif  // CMRATTACHMENTSFIELD_H
