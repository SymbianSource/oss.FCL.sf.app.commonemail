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
* Description:  Detailed subject field.
 *
*/


#ifndef CESMRVIEWERDETAILEDSUBJECTFIELD_H_
#define CESMRVIEWERDETAILEDSUBJECTFIELD_H_

// INCLUDES
#include <e32cmn.h> // TSize
#include <eikedwob.h> // MEikEdwinSizeObserver

#include "cesmrfield.h"

// FORWARD DECLARATIONS
class CESMRRichTextViewer;
class MESMRTitlePaneObserver;
class CMRImage;

/**
 * This field shows the whole subject of the event. Title pane
 * shows only first few words from the subject so this field is
 * needed to show it all.
 */
NONSHARABLE_CLASS( CESMRViewerDetailedSubjectField ):
        public CESMRField,
        public MEikEdwinSizeObserver
    {
public:
    /**
     * Creates new CESMRViewerDetailedSubjectField object. Ownership
     * is transferred to caller.
     * @return Pointer to created object,
     */
    static CESMRViewerDetailedSubjectField* NewL( );
    
    /**
     * C++ Destructor.
     */
    ~CESMRViewerDetailedSubjectField( );

public: // Interface
    /**
     * Sets titlepane observer. Ownership is not transferred.
     * @param aObserver Pointer to titlepane objerver
     */
    void SetTitlePaneObserver( MESMRTitlePaneObserver* aObserver );


public: // From CESMRField
    TSize MinimumSize();
    void InternalizeL( MESMRCalEntry& aEntry );
    void InitializeL();
    void ListObserverSet();
    void ExecuteGenericCommandL( TInt aCommand );
    void SetOutlineFocusL( TBool aFocus );

public: // From CCoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, 
                                       TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();
    void SetContainerWindowL( const CCoeControl& aContainer );

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL(
            CEikEdwin* aEdwin,
            TEdwinSizeEvent aType,
            TSize aSize );

private: // Implementation
    CESMRViewerDetailedSubjectField();
    void ConstructL();
    TRect RichTextViewerRect();

private: // data
    // Own: Edwin size
    TSize iSize;
    // Own: Field icon
    CMRImage* iFieldIcon;
    // Own: Subject text is storaged here
    CESMRRichTextViewer* iRichTextViewer;
    // RichTextViewer line count. Own
    TInt iLineCount;
    /// Ref: Pointer to title pane observer
    MESMRTitlePaneObserver* iTitlePaneObserver;
    };

#endif // CESMRVIEWERDETAILEDSUBJECTFIELD_H_
