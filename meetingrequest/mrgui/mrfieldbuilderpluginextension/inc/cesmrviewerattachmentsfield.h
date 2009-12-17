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
* Description:  Meeting request attachments field
 *
*/


#ifndef CESMRVIEWERATTACHMENTSFIELD_H
#define CESMRVIEWERATTACHMENTSFIELD_H

#include <eikedwob.h>

#include "cesmrfield.h"
#include "cesmrrichtextviewer.h"
#include "resmrstatic.h"

class CESMRRichTextViewer;
class CESMRRichTextLink;
class CMRImage;

// <cmail>
class CESMRAttachmentInfo;
class CFSMailMessage;
// </cmail>

/**
 * Shows the attachments
 **/
NONSHARABLE_CLASS( CESMRViewerAttachmentsField ):
    public CESMRField,
    public MEikEdwinSizeObserver,
    public MESMRRichTextObserver,
    public MEikCommandObserver
    {
public:
    /**
     * Constructor for attachments field
     *
     * @return New attachments field object
     */
    static CESMRViewerAttachmentsField* NewL( );
    
    /**
     * C++ Destructor.
     */
    ~CESMRViewerAttachmentsField( );

public: // From CESMRField
    TSize MinimumSize();
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void ListObserverSet();
    void SetOutlineFocusL( TBool aFocus );
    void ExecuteGenericCommandL( TInt aCommand );

public: // From MESMRRichTextObserver
    TBool HandleRichTextLinkSelection(const CESMRRichTextLink* aLink );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL( CEikEdwin* aEdwin, TEdwinSizeEvent aType,
            TSize aSize );

protected: // From MEikCommandObserver
    
    /**
     * Process commands from contact menu handler.
     * Forwards commands to event observer. 
     */
    void ProcessCommandL( TInt aCommandId );
    
private: // Implementation
    CESMRViewerAttachmentsField();
    void ConstructL();
    TRect RichTextViewerRect();
    
    // <cmail>
    /**
     * Cleanup operations for RPointerArray.
     *
     * @param aArray Pointer to RPointerArray.
     */
    static void MessagePartPointerArrayCleanup( TAny* aArray );

    void UpdateAttachmentInfoL();
    // </cmail>

private:
    // Size of this field
    TSize iSize;
    // Own: Field icon
    CMRImage* iFieldIcon;
    // Actual text of this field
    CESMRRichTextViewer* iRichTextViewer;
    // RichTextViewer line count. Own
    TInt iLineCount;
    // <cmail>
    /// Own:
    RESMRStatic iESMRStatic;
    /// Ref: Contact Menu handler
    CESMRContactMenuHandler* iCntMenuHdlr;
    // </cmail>
    // The number of attached files.
    TInt iAttachmentCount;
    // The attachment text
    HBufC* iAttachmentText;
    // <cmail>
    CFSMailMessage* iMailMessage;   // ref
    CESMRAttachmentInfo* iAttachmentInfo;   // own
    
    TBool iFocused;
    // </cmail>
    };

#endif 
