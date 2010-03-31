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
* Description:  This class is for showing the attendees in viewer mode.
 *
*/

#ifndef CESMRVIEWERATTENDEESFIELD_H
#define CESMRVIEWERATTENDEESFIELD_H

#include <caluser.h>
#include <eikedwob.h>
#include "cesmrfield.h"
#include "cesmrrichtextviewer.h"

class CESMRRichTextLink;
class CMRLabel;

/**
 * This class shows the attendees in viewer mode. It's possible to
 * use contact action menu via a single attendee.
 */
NONSHARABLE_CLASS( CESMRViewerAttendeesField ) : public CESMRField,
public MEikEdwinSizeObserver, public MESMRRichTextObserver
    {
public:
    /**
     * Constructor for the attendee field.
     *
     * @param CCalAttendee::TCalRole - Information that tells is this
     *                                 field for optional or for
     *                                 required attendees.
     * @return New attendees field object.
     */
    static CESMRViewerAttendeesField* NewL( CCalAttendee::TCalRole );

    /**
     * C++ Destructor.
     */
    ~CESMRViewerAttendeesField( );

public: // From CCoeControl
    TInt CountComponentControls( ) const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged( );
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    void SetContainerWindowL(const CCoeControl& aContainer);
    TSize MinimumSize();

public: // From CESMRField
    void InitializeL();
    void SetOutlineFocusL( TBool aFocus );
    void InternalizeL( MESMRCalEntry& aEntry );
    void GetMinimumVisibleVerticalArea(TInt& aUpper, TInt& aLower);
    void GetCursorLineVerticalPos(TInt& aUpper, TInt& aLower);
    void ListObserverSet( );
    TBool ExecuteGenericCommandL( TInt aCommand );
    void HandleLongtapEventL( const TPoint& aPosition );

public: // From MEikEdwinSizeObserver
    TBool HandleEdwinSizeEventL(CEikEdwin* aEdwin, TEdwinSizeEvent aType,
            TSize aDesirableEdwinSize );

public: // From MESMRRichTextObserver
    TBool HandleRichTextLinkSelection(const CESMRRichTextLink* aLink );

private: // Implementation
    CESMRViewerAttendeesField( CCalAttendee::TCalRole aType );
    void ConstructL( );
    HBufC* ClipTextLC( const TDesC& aText, const CFont& aFont, TInt aWidth );
    void UpdateAttendeesListL();
    TInt LineSpacing();
    TInt FocusMargin();

private: // data
    // Own: Title of this field
    CMRLabel* iTitle;
    // Not own: Container for all the attendees
    CESMRRichTextViewer* iRichTextViewer;
    // Size of the field when after field has expanded.
    TSize iExpandedSize;
    // Size of the title
    TSize iTitleSize;
    // Inofrmation about attendee type
    CCalAttendee::TCalRole iRole;
    // Not owned. Attendee list.
    RPointerArray<CCalAttendee> iCalAttendees;    
    // Show all attendees.
    TBool iShowAllAttendees;
    };

#endif // CESMRVIEWERATTENDEESFIELD_H
