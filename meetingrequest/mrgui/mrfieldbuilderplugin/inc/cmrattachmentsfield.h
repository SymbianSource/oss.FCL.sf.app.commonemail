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


#ifndef CMRATTACHMENTSFIELD_H
#define CMRATTACHMENTSFIELD_H

#include <e32base.h>
#include <eikedwob.h>

#include "esmrdef.h"
#include "cesmrfield.h"
#include "cesmrrichtextviewer.h"
#include "cmrattachmentui.h"

// Forward declarations
class CEikButtonGroupContainer;
class MESMRMeetingRequestEntry;
class CMRLabel;
class CAknsBasicBackgroundControlContext;
class CMRImage;
class CESMRRichTextViewer;
class CCalAttachment;
class CMRAttachmentUi;
class CMRAttachmentCommandHandler;

NONSHARABLE_CLASS( CMRAttachmentsField ): public CESMRField,
                                          public MEikEdwinSizeObserver,
                                          public MESMRRichTextObserver,
                                          public MMRAttachmentUiObserver

    {
public:
    /**
     * Two phase constructor.
     */
    static CMRAttachmentsField* NewL();

    /**
     * Destructor.
     */
    ~CMRAttachmentsField();

protected: // From CESMRField
    TSize MinimumSize();
    void InitializeL();
    void InternalizeL( MESMRCalEntry& aEntry );
    void ExternalizeL( MESMRCalEntry& aEntry );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );
    void ListObserverSet();
    TBool HandleSingletapEventL( const TPoint& aPosition );
    void HandleLongtapEventL( const TPoint& aPosition );
    TBool HandleRawPointerEventL( const TPointerEvent& aPointerEvent );
    void DynInitMenuPaneL(
                 TInt aResourceId,
                 CEikMenuPane* aMenuPane );    

protected: // From CCoeControl
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    void SetContainerWindowL( const CCoeControl& aControl );
    
protected: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType, TSize aDesirableEdwinSize);
    
public: // From MESMRRichTextObserver
    TBool HandleRichTextLinkSelection( const CESMRRichTextLink* aLink );
    
public: // From MMRAttachmentUiObserver
    void AttachmentOperationCompleted( MDesC16Array& aArray );
    
private: // Implementation

    CMRAttachmentsField();
    void ConstructL();

    void UpdateAttachmentsListL();
    HBufC* ClipTextLC( const TDesC& aText, const CFont& aFont, TInt aWidth );
    void UpdateFieldL();
    HBufC* AttachmentNameAndSizeL( const TDesC& aDes, const CCalAttachment& aAttachment );
    TBool AddAttachmentL();
    TInt LineSpacing();
    TInt FocusMargin();
    
private: //data
    // Own: Title of this field
    CMRLabel* iTitle;
    // Own: Field icon
    CMRImage* iFieldIcon;
    // Owned. Attendee editor.
    CESMRRichTextViewer* iRichTextViewer;
    // Not owned. CBA.
    CEikButtonGroupContainer* iButtonGroupContainer;
    // Own. Background control context.
    CAknsBasicBackgroundControlContext* iBgCtrlContext;
    // Attachment count
    TInt iAttachmentCount;
    /// Own: Attachment ui for adding attachments
    CMRAttachmentUi* iAttachmentUi;
    /// Ref: Reference to internalized entry
    MESMRCalEntry* iEntry;
    // Field rect for comparison purposes
    TRect iFieldRect;
    // Rich Text viewer size
    TSize iRichTextSize;
    /// Own: Attachment command handler
    CMRAttachmentCommandHandler* iAttCommandHandler;
    };

#endif  // CMRATTACHMENTSFIELD_H
