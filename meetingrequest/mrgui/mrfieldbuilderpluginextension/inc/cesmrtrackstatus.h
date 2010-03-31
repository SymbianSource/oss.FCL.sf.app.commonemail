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
* Description:  field that holds the status of the MR participant
 *
*/


#ifndef CESMRTRACKSTATUS_H_
#define CESMRTRACKSTATUS_H_

#include <eikrted.h>
#include <cntitem.h>
#include <eiklbx.h>
#include <eikedwob.h>

#include "cesmrfield.h"
#include "resmrstatic.h"


// Forward declarations
class CCalAttendee;
class CMRImage;

/**
 * This class shows the status of the attendee of a meeting request
 * a meeting request can have many of these in several modes
 */
NONSHARABLE_CLASS( CESMRTrackStatus ) : public CESMRField
    {
public:
    /**
     * Constructor for the track status field
     * @return New mr partcipant tracking status holder field
     */
    static CESMRTrackStatus* NewL();
    
    /**
     * C++ Destructor.
     */
    ~CESMRTrackStatus();

public: // Interface
    /*
     * Draws right-arrow icon.
     * @param aFocused selects to hide or display icon
     */
    void SetActionMenuIconL( TBool aFocused );

public: // From CESMRField
    void InternalizeL( MESMRCalEntry& aEntry );
    void SetOutlineFocusL( TBool aFocus );

public: // From CoeControl
    TKeyResponse OfferKeyEventL( const TKeyEvent& aEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aInd ) const;
    void SizeChanged();

private: // Implementation
    CESMRTrackStatus();
    void ConstructL();
    HBufC* ClipTextLC( const TDesC& aText, 
                       const CFont& aFont, 
                       TInt aWidth );
private:
    /**
     * The address of the attendee. Own
     */
    HBufC* iAddress;
    
    /**
     * The default address of the attendee. Own
     */
    HBufC* iDefaultAddress;

    /**
     * The Common name of the attendee. Own
     */
    HBufC* iCommonName;

    /**
     * Static TLS stored variables.
     */
    RESMRStatic iESMRStatic;

    /**
     * Ref: Reference to menu handler
     */
    CESMRContactMenuHandler* iMenuHandler;

    /*
     * Action Menu icon
     */
    CFbsBitmap* iActionMenuIcon;
    
    /*
     * Action Menu icon mask.
     */
    CFbsBitmap* iActionMenuIconMask;

    /**
     * Owned. checkbox_mark/checkbox_unmark
     */
    CMRImage* iIcon;
    };

#endif /*CESMRTRACKSTATUS_H_*/

// End of file
